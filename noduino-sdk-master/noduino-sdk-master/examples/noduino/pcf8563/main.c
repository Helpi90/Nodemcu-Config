/*
 *  Copyright (c) 2015 - 2025 MaiKe Labs
 *
 *  Example of BH1750 digital light library usage.
 *
 *  Using the default high resolution mode and then makes
 *   a light level reading every second.
 *
 *  Connect VCC of the BH1750 sensor to 3.3V (NOT 5.0V!)
 *  Connect GND to Ground
 *  Connect SCL to i2c clock - GPIO4
 *  Connect SDA to i2c data  - GPIO5
 *  Connect ADD-NC or GND
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
#include "pcf8563.h"

irom void setup()
{
	serial_begin(115200);
	pcf8563_init();
	pcf8563_set_from_int(2016, 8, 28, 12, 0, 0);

	serial_printf("PCF8563 RTC Example!\r\n");
}

void loop()
{
	pcf8563_now();
	pcf8563_print();
	delay(2000);
}
