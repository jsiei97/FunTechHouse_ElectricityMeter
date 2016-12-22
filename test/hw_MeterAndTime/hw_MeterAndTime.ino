/**
 * @file hw_MeterAndTime.ino
 * @author Johan Simonsson
 * @brief Show basic usage of ElectricityMeter on target with time from RTC
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

#include <Wire.h>

#include "DateTime.h"
#include "RTC_DS1307.h"
#include "ElectricityMeter.h"

DateTime now;
DateTime last;
RTC_DS1307 rtc;
char isoDate[25];


bool blink = false;
unsigned int cnt = 0;

#define SIZE 50
char str[SIZE];

char* topic;

ElectricityMeter meter(ELECTRICITYMETER_IMPL_PER_KWH_1000);

// the setup function runs once when you press reset or power the board
void setup() {
    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    Serial.println("hw_ElectricityMeter.ino");
    
    Wire.begin();
    if(rtc.isrunning())
    {
        Serial.println("Clock is running");
    }
    else
    {
        Serial.println("Clock is NOT running");
    }

    //Set topic for MQTT
    meter.setTopic("topic_sub", "topic_pub");

    //For demo it is in setup, normally it should be in loop.

    topic = meter.getTopicPublish();
    Serial.println(topic);
}

// the loop function runs over and over again forever
void loop() {

    //Print every 10ms x 200 ~ 2s
    if(0==cnt%200)
    {
        meter.getValue(str, SIZE);
        sprintf(str + strlen(str)," time=");
        rtc.getTime(&now, &last);
        now.appendIsoDateString(str, SIZE);
        Serial.println(str);        
    }

    //Tick one.
    meter.pulse();


    //10ms every 50 times ~ 500ms toggle ~ 1Hz
    if(0==cnt%50)
    {
        if(blink) {
            digitalWrite(LED_BUILTIN, HIGH);
            blink = false;
        }else{
            digitalWrite(LED_BUILTIN, LOW);
            blink = true;
        }
    }

    delay(10);
    cnt++;
}
