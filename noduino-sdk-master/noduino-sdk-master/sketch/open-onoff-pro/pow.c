/*
 *  Copyright (c) 2015 - 2025 MaiKe Labs
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/
#include "user_config.h"
#include "hlw8012.h"

// GPIOs
#define SEL_PIN                         16
#define CF1_PIN                         14
#define CF_PIN                          13

// Check values every 10 seconds
#define UPDATE_TIME                     10000

// Set SEL_PIN to HIGH to sample current
// This is the case for Itead's Sonoff POW, where a
// the SEL_PIN drives a transistor that pulls down
// the SEL pin in the HLW8012 when closed
#define CURRENT_MODE                    HIGH

// These are the nominal values for the resistors in the circuit
#define CURRENT_RESISTOR                0.001
#define VOLTAGE_RESISTOR_UPSTREAM       ( 4 * 470000 ) // Real: 1880k
#define VOLTAGE_RESISTOR_DOWNSTREAM     ( 1000 ) // Real 1.009k

char obuf[16];

// Library expects an interrupt on both edges
irom void setInterrupts() {
    attachInterrupt(CF1_PIN, hlw8012_cf1_interrupt, CHANGE);
    attachInterrupt(CF_PIN, hlw8012_cf_interrupt, CHANGE);
}

irom void calibrate() {

    // Let some time to register values
    unsigned long timeout = millis();
    while ((millis() - timeout) < 10000) {
        delay(1);
    }

    // Calibrate using a 60W bulb (pure resistive) on a 220V line
    hlw8012_expectedActivePower(1156.0);
    hlw8012_expectedVoltage(220.0);
    hlw8012_expectedCurrent(1156.0 / 220.0);

    // Show corrected factors
	dtostrf(hlw8012_getCurrentMultiplier(), 16, 2, obuf);
    serial_printf("[HLW] New current multiplier : %s\r\n", obuf);
	dtostrf(hlw8012_getVoltageMultiplier(), 16, 2, obuf);
    serial_printf("[HLW] New voltage multiplier : %s\r\n", obuf);
	dtostrf(hlw8012_getPowerMultiplier(), 16, 2, obuf);
    serial_printf("[HLW] New power multiplier   : %s\r\n", obuf);
}

irom void pow_init() {

    // Initialize HLW8012
    // void begin(uint8_t cf_pin, uint8_t cf1_pin, uint8_t sel_pin
	// 			 	uint8_t currentWhen = HIGH, bool use_interrupts = false,
	// 			 	unsigned long pulse_timeout = PULSE_TIMEOUT);
    // * cf_pin, cf1_pin and sel_pin are GPIOs to the HLW8012 IC
    // * currentWhen is the value in sel_pin to select current sampling
    // * set use_interrupts to true to use interrupts to monitor pulse widths
    // * leave pulse_timeout to the default value, recommended when using interrupts
#ifdef USE_INTERRUPT
    hlw8012_begin(CF_PIN, CF1_PIN, SEL_PIN, CURRENT_MODE, true, PULSE_TIMEOUT);
#else
    hlw8012_begin(CF_PIN, CF1_PIN, SEL_PIN, CURRENT_MODE, false, 500000);
#endif

    // These values are used to calculate current, voltage and power factors as per datasheet formula
    // These are the nominal values for the OpenOnoff Pro resistors:
    // * The CURRENT_RESISTOR is the 1milliOhm copper-manganese resistor in series with the main line
    // * The VOLTAGE_RESISTOR_UPSTREAM are the 4 470kOhm resistors in the voltage divider that feeds the V2P pin in the HLW8012
    // * The VOLTAGE_RESISTOR_DOWNSTREAM is the 1kOhm resistor in the voltage divider that feeds the V2P pin in the HLW8012
    hlw8012_setResistors(CURRENT_RESISTOR, VOLTAGE_RESISTOR_UPSTREAM, VOLTAGE_RESISTOR_DOWNSTREAM);

    // Show default (as per datasheet) multipliers
	dtostrf(hlw8012_getCurrentMultiplier(), 16, 2, obuf);
    serial_printf("[HLW] Default current multiplier : %s\r\n", obuf);
	dtostrf(hlw8012_getVoltageMultiplier(), 16, 2, obuf);
    serial_printf("[HLW] Default voltage multiplier : %s\r\n", obuf);
	dtostrf(hlw8012_getPowerMultiplier(), 16, 2, obuf);
    serial_printf("[HLW] Default power multiplier   : %s\r\n", obuf);
#ifdef USE_INTERRUPTS
    setInterrupts();
#endif

    //calibrate();
}

void pow_test() {

    static unsigned long last = 0;

    // This UPDATE_TIME should be at least twice the interrupt timeout (2 second by default)
	serial_printf("[HLW] Active Power (W)    : %d\r\n", hlw8012_getActivePower());
	serial_printf("[HLW] Voltage (V)         : %d\r\n", hlw8012_getVoltage());
	dtostrf(hlw8012_getCurrent(), 16, 2, obuf);
	serial_printf("[HLW] Current (A)         : %s\r\n", obuf);
	serial_printf("[HLW] Apparent Power (VA) : %d\r\n", hlw8012_getApparentPower());
	serial_printf("[HLW] Power Factor (%)    : %d\r\n\r\n", (int) (100 * hlw8012_getPowerFactor()));

	hlw8012_toggleMode();
}

void pow_publish()
{
	char msg[80];
	char obuf[8];
	os_memset(obuf, 0, 16);
	os_memset(msg, 0, 80);

	os_sprintf(msg, "{\"P\":%d,\"V\":%d,\"C\":%s,\"aP\":%d,\"PF\":%d}",
				hlw8012_getActivePower(),
				hlw8012_getVoltage(),
				dtostrf(hlw8012_getCurrent(), 8, 2, obuf),
				hlw8012_getApparentPower(),
				(int)(100 * hlw8012_getPowerFactor())
	);

	hlw8012_toggleMode();
	mjyun_publishstatus(msg);
}
