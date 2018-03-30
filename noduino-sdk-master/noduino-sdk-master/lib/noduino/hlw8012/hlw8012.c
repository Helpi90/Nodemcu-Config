/*

HLW8012 0.1.0

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

#include "noduino.h"
#include "hlw8012.h"

unsigned char _cf_pin;
unsigned char _cf1_pin;
unsigned char _sel_pin;

double _current_resistor = R_CURRENT;
double _voltage_resistor = R_VOLTAGE;

double _current_multiplier;
double _voltage_multiplier;
double _power_multiplier;

unsigned long _pulse_timeout = PULSE_TIMEOUT;
unsigned long _voltage_pulse_width = 0;
unsigned long _current_pulse_width = 0;
unsigned long _power_pulse_width = 0;

double _current = 0;
unsigned int _voltage = 0;
unsigned int _power = 0;

unsigned char _current_mode = HIGH;
unsigned char _mode;

bool _use_interrupts = true;
unsigned long _last_cf_interrupt = 0;
unsigned long _last_cf1_interrupt = 0;
unsigned long _first_cf1_interrupt = 0;

irom void hlw8012_begin(
    unsigned char cf_pin,
    unsigned char cf1_pin,
    unsigned char sel_pin,
    unsigned char currentWhen,
    bool use_interrupts,
    unsigned long pulse_timeout
    )
{

    _cf_pin = cf_pin;
    _cf1_pin = cf1_pin;
    _sel_pin = sel_pin;
    _current_mode = currentWhen;
    _use_interrupts = use_interrupts;
    _pulse_timeout = pulse_timeout;

    pinMode(_cf_pin, INPUT_PULLUP);
    pinMode(_cf1_pin, INPUT_PULLUP);
    pinMode(_sel_pin, OUTPUT);

    hlw8012_calculateDefaultMultipliers();

    _mode = _current_mode;
    digitalWrite(_sel_pin, _mode);
}

irom double hlw8012_getCurrentMultiplier() { return _current_multiplier; };
irom double hlw8012_getVoltageMultiplier() { return _voltage_multiplier; };
irom double hlw8012_getPowerMultiplier() { return _power_multiplier; };

irom void hlw8012_setCurrentMultiplier(double current_multiplier) { _current_multiplier = current_multiplier; };
irom void hlw8012_setVoltageMultiplier(double voltage_multiplier) { _voltage_multiplier = voltage_multiplier; };
irom void hlw8012_setPowerMultiplier(double power_multiplier) { _power_multiplier = power_multiplier; };

irom void hlw8012_setMode(hlw8012_mode_t mode) {
    _mode = (mode == MODE_CURRENT) ? _current_mode : 1 - _current_mode;
    digitalWrite(_sel_pin, _mode);
    if (_use_interrupts) {
        _last_cf1_interrupt = _first_cf1_interrupt = micros();
    }
}

irom hlw8012_mode_t hlw8012_getMode() {
    return (_mode == _current_mode) ? MODE_CURRENT : MODE_VOLTAGE;
}

irom hlw8012_mode_t hlw8012_toggleMode() {
    hlw8012_mode_t new_mode = hlw8012_getMode() == MODE_CURRENT ? MODE_VOLTAGE : MODE_CURRENT;
    hlw8012_setMode(new_mode);
    return new_mode;
}

irom double hlw8012_getCurrent() {

    // Power measurements are more sensitive to switch offs,
    // so we first check if power is 0 to set _current to 0 too
    if (_power == 0) {
        _current_pulse_width = 0;

    } else if (_use_interrupts) {
        hlw8012_checkCF1Signal();

    } else if (_mode == _current_mode) {
        _current_pulse_width = pulseIn(_cf1_pin, HIGH, _pulse_timeout);
    }

    _current = (_current_pulse_width > 0) ? _current_multiplier / _current_pulse_width / 2 : 0;
    return _current;

}

irom unsigned int hlw8012_getVoltage() {
    if (_use_interrupts) {
        hlw8012_checkCF1Signal();
    } else if (_mode != _current_mode) {
        _voltage_pulse_width = pulseIn(_cf1_pin, HIGH, _pulse_timeout);
    }
    _voltage = (_voltage_pulse_width > 0) ? _voltage_multiplier / _voltage_pulse_width / 2 : 0;
    return _voltage;
}

irom unsigned int hlw8012_getActivePower() {
    if (_use_interrupts) {
        hlw8012_checkCFSignal();
    } else {
        _power_pulse_width = pulseIn(_cf_pin, HIGH, _pulse_timeout);
    }
    _power = (_power_pulse_width > 0) ? _power_multiplier / _power_pulse_width / 2 : 0;
    return _power;
}

irom unsigned int hlw8012_getApparentPower() {
    double current = hlw8012_getCurrent();
    unsigned int voltage = hlw8012_getVoltage();
    return voltage * current;
}

irom double hlw8012_getPowerFactor() {
    unsigned int active = hlw8012_getActivePower();
    unsigned int apparent = hlw8012_getApparentPower();
    if (active > apparent) return 1;
    if (apparent == 0) return 0;
    return (double) active / apparent;
}

irom void hlw8012_expectedCurrent(double value) {
    if (_current == 0) hlw8012_getCurrent();
    if (_current > 0) _current_multiplier *= (value / _current);
}

irom void hlw8012_expectedVoltage(unsigned int value) {
    if (_voltage == 0) hlw8012_getVoltage();
    if (_voltage > 0) _voltage_multiplier *= ((double) value / _voltage);
}

irom void hlw8012_expectedActivePower(unsigned int value) {
    if (_power == 0) hlw8012_getActivePower();
    if (_power > 0) _power_multiplier *= ((double) value / _power);
}

irom void hlw8012_setResistors(double current, double voltage_upstream, double voltage_downstream) {
    if (voltage_downstream > 0) {
        _current_resistor = current;
        _voltage_resistor = (voltage_upstream + voltage_downstream) / voltage_downstream;
        hlw8012_calculateDefaultMultipliers();
    }
}

void hlw8012_cf_interrupt() {
    unsigned long now = micros();
    _power_pulse_width = now - _last_cf_interrupt;
    _last_cf_interrupt = now;
}

void hlw8012_cf1_interrupt() {

    unsigned long now = micros();
    unsigned long pulse_width;

    if ((now - _first_cf1_interrupt) > _pulse_timeout) {

        if (_last_cf1_interrupt == _first_cf1_interrupt) {
            pulse_width = 0;
        } else {
            pulse_width = now - _last_cf1_interrupt;
        }

        if (_mode == _current_mode) {
            _current_pulse_width = pulse_width;
        } else {
            _voltage_pulse_width = pulse_width;
        }

        _mode = 1 - _mode;
        digitalWrite(_sel_pin, _mode);
        _first_cf1_interrupt = now;

    }

    _last_cf1_interrupt = now;
}

irom void hlw8012_checkCFSignal() {
    if ((micros() - _last_cf_interrupt) > _pulse_timeout) _power_pulse_width = 0;
}

irom void hlw8012_checkCF1Signal() {
    if ((micros() - _last_cf1_interrupt) > _pulse_timeout) {
        if (_mode == _current_mode) {
            _current_pulse_width = 0;
        } else {
            _voltage_pulse_width = 0;
        }
        hlw8012_toggleMode();
    }
}

// These are the multipliers for current, voltage and power as per datasheet
// These values divided by output period (in useconds) give the actual value
// For power a frequency of 1Hz means around 12W
// For current a frequency of 1Hz means around 15mA
// For voltage a frequency of 1Hz means around 0.5V
irom void hlw8012_calculateDefaultMultipliers() {
    _current_multiplier = ( 1000000.0 * 512 * V_REF / _current_resistor / 24.0 / F_OSC );
    _voltage_multiplier = ( 1000000.0 * 512 * V_REF * _voltage_resistor / 2.0 / F_OSC );
    _power_multiplier = ( 1000000.0 * 128 * V_REF * V_REF * _voltage_resistor / _current_resistor / 48.0 / F_OSC );
}
