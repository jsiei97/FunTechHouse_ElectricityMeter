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

#include "QuickDate.h"
#include "DateTime.h"
#include "RTC_DS1307.h"
#include "ElectricityMeter.h"

#define SIZE 100
char str[SIZE];

DateTime now;  ///< Current time
DateTime last; ///< Last synced time
RTC_DS1307 rtc;
QuickDate qd;

/// This device MAC adress, it is written on the Shield and must be uniq.
uint8_t mac[]    = { 0x90, 0xA2, 0xDA, 0x00, 0x29, 0x7D };

/// The MQTT device name, this must be unique
char project_name[]  = "FunTechHouse_ElectricityMeter";

ElectricityMeter m1(ELECTRICITYMETER_IMPL_PER_KWH_500);
ElectricityMeter m2(ELECTRICITYMETER_IMPL_PER_KWH_500);


volatile unsigned int updateCount = 0;

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

void setup()
{
    // Open serial communications and wait for port to open:
    Serial.begin(9600);

    Wire.begin();
    if(rtc.isrunning())
    {
        Serial.println("Clock is running");
    }
    else
    {
        Serial.println("Clock is NOT running");
    }

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


    Ethernet.begin(mac);
    if (client.connect(project_name))
    {
        client.publish(m1.getTopicPublish(), "#Hello world - Meter01");
        client.publish(m2.getTopicPublish(), "#Hello world - Meter02");
    }
}

/**
 * The main loop, runs all the time, over and over again.
 */
void loop()
{
    //Talk with the server so he dont forget us.
    if(client.loop() == false)
    {
        client.connect(project_name);
    }

    if(false == client.connected())
    {
        client.connect(project_name);
    }

    //Serial.println("Time to check time...");
    //Check time

    //rtc.getTime(&now, &last);
    if(!rtc.isrunning() || (now.secSince2000()-last.secSince2000()>(1*60*60)) )
    {
        Serial.println("Time to sync time...");
        int qdStatus = qd.doTimeSync(str);
        if(qdStatus > 0)
        {
            Serial.print("ok: ");
            Serial.print(str);
            Serial.print(" : ");
            now.setTime(str);
            rtc.adjust(&now);
        }
        else
        {
            Serial.print("fail: ");
        }

        Serial.println(qdStatus);
    }

    //But only send data every minute or so
    // 6 ->   6*10s =  60s = 1min
    //12 -> 2*6*10s = 120s = 2min
    //18 -> 3*6*10s = 180s = 3min
    //BUT since the delay is not that accurate,
    // 18 is more like 3.5 to 4 minutes in real life...
    if(updateCount > 18)
    {
        updateCount = 0;

        m1.getValue(str, SIZE);
        sprintf(str + strlen(str)," time=");
        rtc.getTime(&now, &last);
        now.appendIsoDateString(str, SIZE);
        Serial.println(str);

        if(client.connected())
        {
            client.publish(m1.getTopicPublish(), str);
        }

        m2.getValue(str, SIZE);
        sprintf(str + strlen(str)," time=");
        rtc.getTime(&now, &last);
        now.appendIsoDateString(str, SIZE);
        Serial.println(str);

        if(client.connected())
        {
            client.publish(m2.getTopicPublish(), str);
        }
    }

    updateCount++;
    delay(10000); // 10*1000ms = 10s
}
