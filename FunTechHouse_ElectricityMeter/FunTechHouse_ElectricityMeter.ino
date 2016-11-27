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

/// This device MAC adress, it is written on the Shield and must be uniq.
uint8_t mac[]    = { 0x90, 0xA2, 0xDA, 0x00, 0x29, 0x7D };

/// The MQTT device name, this must be unique
char project_name[]  = "FunTechHouse_ElectricityMeter";
char topic_meter01[] = "FunTechHouse/Energy/meter01";
char topic_meter02[] = "FunTechHouse/Energy/meter02";

//Number of pulses, used to measure energy.
volatile unsigned int pulseCount1_Wh  = 0;
volatile unsigned int pulseCount1_kWh = 0;

volatile unsigned int pulseCount2_Wh  = 0;
volatile unsigned int pulseCount2_kWh = 0;

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

//Define to configure input
#define PULSE1_IMPL_PER_KWH_500
//#define PULSE1_IMPL_PER_KWH_1000
#define PULSE2_IMPL_PER_KWH_500
//#define PULSE2_IMPL_PER_KWH_1000

// The interrupt routine
void onPulse1()
{
    //One pulse from the meter equals 1 interupt
    //if we have 1000 pulse/kWh then every interupt is worth 1Wh => pulseCount1_Wh +1
    //if we have  500 pulse/kWh then every interupt is worth 2Wh => pulseCount1_Wh +2

    //pulseCounter
#if defined (PULSE1_IMPL_PER_KWH_500)
    pulseCount1_Wh+=2;
#endif
#if defined (PULSE1_IMPL_PER_KWH_1000)
    pulseCount1_Wh++;
#endif

    if(pulseCount1_Wh == 1000)
    {
        pulseCount1_Wh = 0;
        pulseCount1_kWh++;
    }
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

    //pulseCounter
#if defined (PULSE2_IMPL_PER_KWH_500)
    pulseCount2_Wh+=2;
#endif
#if defined (PULSE2_IMPL_PER_KWH_1000)
    pulseCount2_Wh++;
#endif
    if(pulseCount2_Wh == 1000)
    {
        pulseCount2_Wh = 0;
        pulseCount2_kWh++;
    }
}

void setup()
{
    // KWH interrupt attached to IRQ 0  = pin2
    attachInterrupt(0, onPulse1, FALLING);
    // KWH interrupt attached to IRQ 1  = pin3
    attachInterrupt(1, onPulse2, FALLING);

    Ethernet.begin(mac);
    if (client.connect(project_name))
    {
        client.publish(topic_meter01, "#Hello world");
        client.publish(topic_meter02, "#Hello world");
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

    //But only send data every minute or so
    // 6 ->   6*10s =  60s = 1min
    //12 -> 2*6*10s = 120s = 2min
    //18 -> 3*6*10s = 180s = 3min
    //BUT since the delay is not that accurate,
    // 18 is more like 3.5 to 4 minutes in real life...
    if(updateCount > 18)
    {
        updateCount = 0;
        char str[30];

        snprintf(str, 30, "energy=%u.%03u kWh", pulseCount1_kWh, pulseCount1_Wh);
        if(client.connected())
        {
            client.publish(topic_meter01, str);
        }

        snprintf(str, 30, "energy=%u.%03u kWh", pulseCount2_kWh, pulseCount2_Wh);
        if(client.connected())
        {
            client.publish(topic_meter02, str);
        }
    }

    updateCount++;
    delay(10000); // 10*1000ms = 10s
}
