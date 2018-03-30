/*
 *  Copyright (c) 2015 - 2025 MaiKe Labs
 *
 *  Connect VCC of the BMP085 sensor to 3.3V (NOT 5.0V!)
 *  Connect GND to Ground
 *  Connect SDA to i2c data  - GPIO4
 *  Connect SCL to i2c clock - GPIO5
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
#include "bmp180.h"

irom void setup()
{
	serial_begin(115200);

	if (!bmp180_begin()) {
		serial_print("Could not find a valid BMP180 sensor,\
						check wiring!\r\n");

		while (1) {}
	}
}
  
irom void loop()
{
	char obuf[16];
	double t = 0, p = 0;
	
	bmp180_startTemperature();
	delay(15);
	bmp180_getTemperature(&t);

	dtostrf(t, 16, 2, obuf);
	serial_printf("Temperature = \t\t%s C\r\n", obuf);

	bmp180_startPressure(1);
	delay(15);
	bmp180_getPressure(&p, &t);

	dtostrf(p, 16, 2, obuf);
	serial_printf("Pressure = \t\t%s mbar\r\n", obuf);

	dtostrf(bmp180_sealevel(p, 50), 16, 2, obuf);

	// Calculate altitude assuming 'standard' barometric
	// pressure of 1013.25 millibar = 101325 Pascal
	serial_printf("Pressure at sealevel (calculated) = %s mbar\r\n", obuf);

	// you can get a more precise measurement of altitude
	// if you know the current sea level pressure which will
	// vary with weather and such. If it is 1015 millibars
	// that is equal to 101500 Pascals.
	dtostrf(bmp180_altitude(p, bmp180_sealevel(p, 50)), 16, 2, obuf);
	serial_printf("Real altitude = \t%s M\r\n\r\n", obuf);

	delay(3000);
}
