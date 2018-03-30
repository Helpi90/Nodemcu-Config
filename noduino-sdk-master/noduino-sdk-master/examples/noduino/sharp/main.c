/*
 *  Copyright (c) 2016 - 2025 MaiKe Labs
 *
 *  Interface to Sharp GP2Y1010AU0F Particle Sensor
 *
 *  http://www.sparkfun.com/datasheets/Sensors/gp2y1010au_e.pdf
 *
 *  Sharp pin 1 (V-LED)   => 3.3V (connected to 150ohm resister)
 *  Sharp pin 2 (LED-GND) => GND
 *  Sharp pin 3 (LED)     => Noduino D7 (GPIO16)
 *  Sharp pin 4 (S-GND)   => GND
 *  Sharp pin 5 (Vo)      => Noduino A0 pin
 *  Sharp pin 6 (Vcc)     => 3.3V
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

#include "noduino.h"

int dustVal = 0;
  
int ledPower = 16;
int delayTime = 280;
int delayTime2 = 40;
int offTime = 9680;

irom void setup()
{
	serial_begin(115200);

	//disable the wifi
	wifi_set_opmode(NULL_MODE);

	pinMode(ledPower,OUTPUT);
	digitalWrite(ledPower,HIGH);
	serial_printf("Sharp GP2Y1010AU0F Particle Sensor Example\r\n");
}

  
void loop()
{
	digitalWrite(ledPower,LOW); // power on the LED
	delayMicroseconds(delayTime);

	dustVal = system_adc_read(); // read the dust value via A0 pin
	delayMicroseconds(delayTime2);

	digitalWrite(ledPower,HIGH); // turn the LED off
	delayMicroseconds(offTime);
	  
	delay(3000);
	serial_printf("dush val = %d\r\n", dustVal);
}
