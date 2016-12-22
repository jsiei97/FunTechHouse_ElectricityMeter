/**
 * @file hw_ElectricityMeter.ino
 * @author Johan Simonsson
 * @brief Show basic usage of ElectricityMeter on target
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

#include "ElectricityMeter.h"

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

    //Set topic for MQTT
    meter.setTopic("topic_sub", "topic_pub");

    //For demo it is in setup, normally it should be in loop.
    Serial.println("hw_ElectricityMeter.ino");
    topic = meter.getTopicPublish();
    Serial.println(topic);
}

// the loop function runs over and over again forever
void loop() {

    if(0==cnt%100)
    {
        meter.getValue(str, SIZE);
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
