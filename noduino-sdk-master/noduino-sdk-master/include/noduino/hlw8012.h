/*

HLW8012 0.1.1

Copyright (C) 2016 by Xose Pérez <xose dot perez at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#ifndef __HLW8012_H__
#define __HLW8012_H__

#include "noduino.h"

// Internal voltage reference value
#define V_REF               2.43

// The factor of a 1mOhm resistor
// as per recomended circuit in datasheet
// A 1mOhm resistor allows a ~30A max measurement
#define R_CURRENT           0.001

// This is the factor of a voltage divider of 6x 470K upstream and 1k downstream
// as per recomended circuit in datasheet
#define R_VOLTAGE           2821

// Frequency of the HLW8012 internal clock
#define F_OSC               3579000

// Minimum delay between selecting a mode and reading a sample
#define READING_INTERVAL    3000

// Maximum pulse with in microseconds
// If longer than this pulse width is reset to 0
// This value is purely experimental.
// Higher values allow for a better precission but reduce sampling rate
// and response speed to change
// Lower values increase sampling rate but reduce precission
// Values below 0.5s are not recommended since current and voltage output
// will have no time to stabilise
#define PULSE_TIMEOUT       2000000

// CF1 mode
typedef enum {
    MODE_CURRENT,
    MODE_VOLTAGE
} hlw8012_mode_t;

//public api
void hlw8012_cf_interrupt();
void hlw8012_cf1_interrupt();

void hlw8012_begin(
	unsigned char cf_pin,
	unsigned char cf1_pin,
	unsigned char sel_pin,
	unsigned char currentWhen,
	bool use_interrupts,
	unsigned long pulse_timeout);

void hlw8012_setMode(hlw8012_mode_t mode);
hlw8012_mode_t hlw8012_getMode();
hlw8012_mode_t hlw8012_toggleMode();

double hlw8012_getCurrent();
unsigned int hlw8012_getVoltage();
unsigned int hlw8012_getActivePower();
unsigned int hlw8012_getApparentPower();
double hlw8012_getPowerFactor();

void hlw8012_setResistors(double current, double voltage_upstream, double voltage_downstream);

void hlw8012_expectedCurrent(double current);
void hlw8012_expectedVoltage(unsigned int current);
void hlw8012_expectedActivePower(unsigned int power);

double hlw8012_getCurrentMultiplier();
double hlw8012_getVoltageMultiplier();
double hlw8012_getPowerMultiplier();

void hlw8012_setCurrentMultiplier(double current_multiplier);
void hlw8012_setVoltageMultiplier(double voltage_multiplier);
void hlw8012_setPowerMultiplier(double power_multiplier);

void hlw8012_checkCFSignal();
void hlw8012_checkCF1Signal();
void hlw8012_calculateDefaultMultipliers();

#endif
