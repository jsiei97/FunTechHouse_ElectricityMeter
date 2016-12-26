/**
 * @file ElectricityMeter.h
 * @author Johan Simonsson
 * @brief Electricitymeter with send on change
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

#ifndef  __ELECTRICITYMETER_H
#define  __ELECTRICITYMETER_H

#include "MQTT_Logic.h"


/// A enumeration of supported Electricity Meters
typedef enum
{
    ELECTRICITYMETER_IMPL_PER_KWH_500, ///< ElectricityMeter with  500 impulses per kWh
    ELECTRICITYMETER_IMPL_PER_KWH_1000 ///< ElectricityMeter with 1000 impulses per kWh
} FT_ElectricityMeterType;

class ElectricityMeter : public MQTT_Logic
{
    private:
        FT_ElectricityMeterType type;

        unsigned int pulseCount_Wh; // Current value
        unsigned int pulseCount_kWh;

        unsigned int last_pulseCount_Wh; // Last value sent to server
        unsigned int last_pulseCount_kWh;

        bool nextTime; //Shall we do "nextTime" with isItNextTime
        bool lastTime; //Shall we do "lastTime" with oldValue
    public:
        ElectricityMeter(FT_ElectricityMeterType type);

        void pulse();
        bool getValue(char* val, unsigned int size);
        bool getLastValue(char* val, unsigned int size);

        bool oldValue();     // First value (last value)
        bool newValue();     // Second normal value
        bool isItNextTime(); // Third extra value

        void saveValue();
};

#endif  // __ELECTRICITYMETER_H
