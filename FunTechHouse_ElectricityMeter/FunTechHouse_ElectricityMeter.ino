/**
 * @file FunTechHouse_ElectricityMeter.ino
 * @author Johan Simonsson
 * @brief Main file
 *
 * @mainpage The FunTechHouse ElectricityMeter
 *
 * Reads pulses from a electricity meter for the FunTechHouse project.
 * This project uses a Arduino with a Ethernet shield,
 * and sends its results using MQTT to a Mosquitto server.
 *
 * @see http://fun-tech.se/FunTechHouse/ElectricityMeter/index.php
 * @see https://github.com/jsiei97/FunTechHouse_ElectricityMeter
 */

/*
 * Copyright (C) 2016 Johan Simonsson
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <SPI.h>
#include <Ethernet.h>
#include "PubSubClient.h"
#include <Wire.h>
#include <avr/wdt.h>

#include "QuickDate.h"
#include "DateTime.h"
#include "RTC_DS1307.h"
#include "ElectricityMeter.h"
#include "LoopTimer.h"

//#define DEBUG

#ifdef DEBUG
#define A_PRINTLN(x) Serial.println(x)
#define A_PRINT(x) Serial.print(x)
#else
#define A_PRINTLN(x)
#define A_PRINT(x)
#endif

#define SIZE 100
char str[SIZE];

DateTime now;  ///< Current time
DateTime last; ///< Last synced time
RTC_DS1307 rtc;
QuickDate qd;

/// This device MAC adress, it is written on the Shield and must be uniq.
uint8_t mac[]    = { 0x90, 0xA2, 0xDA, 0x00, 0x62, 0x80 };

/// The MQTT device name, this must be unique
char project_name[]  = "FunTechHouse_ElectricityMeter";

ElectricityMeter m1(ELECTRICITYMETER_IMPL_PER_KWH_500);
ElectricityMeter m2(ELECTRICITYMETER_IMPL_PER_KWH_500);

unsigned int lastUpdateMeter1 = 0;
unsigned int lastUpdateMeter2 = 0;

//Outer loop 2s -> blink freq
#define SLEEP_TIME 2000UL
//When to send if needed, 2s*30=60s=1min
#define LOOP_CNT 30
//If no new data, send anyway, 1min*30=30min
#define NO_UPDATE_CNT 30

#define LED_LIFE 9

LoopTimer loopTimer(SLEEP_TIME);

/**
 * The MQTT subscribe callback function.
 *
 * @param[in] topic What mqtt topic triggered this callback
 * @param[in] payload The actual message
 * @param[in] length The message size
 */
void callback(char* topic, uint8_t* payload, unsigned int length)
{
}

/// The MQTT client
PubSubClient client("mosqhub", 1883, callback);


// The interrupt routine
void onPulse1()
{
    m1.pulse();
}

//volatile unsigned long prevTimePulse2 = 0;

// The interrupt routine
void onPulse2()
{
    //If it is less than Xms since last irq,
    //then it is probably a false pulse.
    //if (millis() - prevTimePulse2 < 10)
    //{
    //    return;
    //}
    //prevTimePulse2 = millis();
    m2.pulse();
}


bool doTimeSync()
{
    bool status = true;

    // Get current time from RTC
    rtc.getTime(&now, &last);

    bool timeToSync = false;

    if( !rtc.isrunning() )
    {
        A_PRINTLN("Time to sync time...(RTC is not running)");
        timeToSync  = true;
    }

    // This may be bad if network is not there.
    if( false == now.isTimeSet() )
    {
        A_PRINTLN("Time to sync time...(Time not set)");
        timeToSync  = true;
    }

    if( (now.secSince2000()-last.secSince2000()) > (1*60*60) )
    {
        A_PRINTLN("Time to sync time...(Sync to old)");
        A_PRINTLN(now.secSince2000());
        A_PRINTLN(last.secSince2000());
        timeToSync  = true;
    }

    //Check if it is time to sync time with server
    if( true == timeToSync )
    {
        //A_PRINTLN("Time to sync time...");
        int qdStatus = qd.doTimeSync(str);
        if(qdStatus > 0)
        {
            A_PRINT("ok: ");
            A_PRINT(str);
            A_PRINT(" : ");
            now.setTime(str);
            rtc.adjust(&now);
        }
        else
        {
            A_PRINT("fail: ");
            status = false;
        }

        A_PRINTLN(qdStatus);
    }

    return status;
}


void setup()
{
    pinMode(LED_LIFE, OUTPUT);

#ifdef DEBUG
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
#endif

    Wire.begin();

    // KWH interrupt attached to IRQ 0  = pin2
    attachInterrupt(0, onPulse1, FALLING);
    // KWH interrupt attached to IRQ 1  = pin3
    attachInterrupt(1, onPulse2, FALLING);

    m1.setTopic(
            "FunTechHouse/VMP/meter01data",
            "FunTechHouse/VMP/meter01"
            );
    m2.setTopic(
            "FunTechHouse/ELP/meter02data",
            "FunTechHouse/ELP/meter02"
            );

    //http://www.nongnu.org/avr-libc/user-manual/group__avr__watchdog.html
    wdt_enable(WDTO_8S);

    A_PRINTLN("Setup: done");
    //Now some init

    Ethernet.begin(mac);

    bool timeSyncOK = false;
    int cnt=0;
    do
    {
        timeSyncOK = doTimeSync();
        cnt++;
    } while( false == timeSyncOK && cnt < 10);

    if (client.connect(project_name))
    {
        //Start to send value 0 to server...
        m1.getValue(str, SIZE);
        sprintf(str + strlen(str)," time=");
        now.appendIsoDateString(str, SIZE);
        A_PRINTLN(str);
        client.publish(m1.getTopicPublish(), str);

        m2.getValue(str, SIZE);
        sprintf(str + strlen(str)," time=");
        now.appendIsoDateString(str, SIZE);
        A_PRINTLN(str);
        client.publish(m2.getTopicPublish(), str);
    }

    A_PRINTLN("Init: done");
}

/**
 * The main loop, runs all the time, over and over again.
 */
void loop()
{
    loopTimer.mark(millis());

    //Feed the watchdog
    wdt_reset();

    //A_PRINT("Loop:");
    //A_PRINTLN(millis());

    //lc -> loop cnt
    //spread the load over time
    static unsigned int lc = 0;
    //A_PRINTLN(lc);

    //Talk with the server so he dont forget us.
    if(client.loop() == false)
    {
        client.connect(project_name);
    }

    if(false == client.connected())
    {
        client.connect(project_name);
    }


    //If there is new data,
    //then send last value with last time
    //before we update the time
    if(0==lc)
    {
        A_PRINTLN("LoopCnt 0: ");

        if(m1.oldValue())
        {
            A_PRINTLN("M1: old value");
            m1.getLastValue(str, SIZE);
            sprintf(str + strlen(str)," time=");
            now.appendIsoDateString(str, SIZE);
            A_PRINTLN(str);

            if(client.connected())
            {
                client.publish(m1.getTopicPublish(), str);
            }
        }

        if(m2.oldValue())
        {
            A_PRINTLN("M2: old value");
            m2.getLastValue(str, SIZE);
            sprintf(str + strlen(str)," time=");
            now.appendIsoDateString(str, SIZE);
            A_PRINTLN(str);

            if(client.connected())
            {
                client.publish(m2.getTopicPublish(), str);
            }
        }


        //Before this point now is the old time
        doTimeSync();
        //Now we have current time

        if(m1.isItNextTime())
        {
            A_PRINTLN("M1: next value");
            m1.getValue(str, SIZE);
            sprintf(str + strlen(str)," time=");
            now.appendIsoDateString(str, SIZE);
            A_PRINTLN(str);

            if(client.connected())
            {
                client.publish(m1.getTopicPublish(), str);
            }
        }

        if(m2.isItNextTime())
        {
            A_PRINTLN("M2: next value");
            m2.getValue(str, SIZE);
            sprintf(str + strlen(str)," time=");
            now.appendIsoDateString(str, SIZE);
            A_PRINTLN(str);

            if(client.connected())
            {
                client.publish(m2.getTopicPublish(), str);
            }
        }

        //Check for value 2, the current value
        //and how long since last update.
        //60s * lastUpdate => 1min * 30 = 30min
        if(m1.newValue() || lastUpdateMeter1 >=NO_UPDATE_CNT)
        {
            A_PRINTLN("M1: current value");
            m1.getValue(str, SIZE);
            sprintf(str + strlen(str)," time=");
            now.appendIsoDateString(str, SIZE);
            A_PRINTLN(str);

            if(client.connected())
            {
                client.publish(m1.getTopicPublish(), str);
                m1.saveValue();
                lastUpdateMeter1=0;
            }
        }

        if(m2.newValue() || lastUpdateMeter2 >=NO_UPDATE_CNT)
        {
            A_PRINTLN("M2: current value");
            m2.getValue(str, SIZE);
            sprintf(str + strlen(str)," time=");
            now.appendIsoDateString(str, SIZE);
            A_PRINTLN(str);

            if(client.connected())
            {
                client.publish(m2.getTopicPublish(), str);
                m2.saveValue();
                lastUpdateMeter2=0;
            }
        }

        lastUpdateMeter1++;
        lastUpdateMeter2++;
    }



    if(lc%2==0)
    {
        //A_PRINTLN("HI");
        //digitalWrite(LED_LIFE, HIGH);
        analogWrite(LED_LIFE, 48); // PWM 0..255
    }
    else
    {
        //A_PRINTLN("LO");
        //digitalWrite(LED_LIFE, LOW);
        analogWrite(LED_LIFE, 0); // PWM 0..255
    }

    lc++;
    //If delay is 2s and lc is 30
    //Then every windows executes every 60s.
    //This is the maximun send intervall.
    if(lc>=LOOP_CNT)
    {
        lc=0;
    }

    delay(loopTimer.correctedTime(millis()));
}
