/**
 * @file ElectricityMeter.cpp
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

#include <stdio.h>

#include "ElectricityMeter.h"

ElectricityMeter::ElectricityMeter ( FT_ElectricityMeterType type )
{
    this->type=type;
    pulseCount_Wh = 0;
    pulseCount_kWh= 0;
}

void ElectricityMeter::pulse()
{
    //Avoid to much work here since it should be
    //called from inside irq.

    switch ( this->type )
    {
        case ELECTRICITYMETER_IMPL_PER_KWH_500:
            pulseCount_Wh+=2;
            break;
        case ELECTRICITYMETER_IMPL_PER_KWH_1000:
            pulseCount_Wh++;
            break;
    }

    if(pulseCount_Wh == 1000)
    {
        pulseCount_Wh = 0;
        pulseCount_kWh++;
    }
}

bool ElectricityMeter::getValue(char* val, unsigned int size)
{
    if(size <= 25)
        return false;

    unsigned int res = snprintf(val, size,
            "energy=%u.%03ukWh",
            this->pulseCount_kWh,
            this->pulseCount_Wh);

    if (size<res)
        return false;

    return true;
}
