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
#include "noduino.h"
#include "dht.h"

void setup()
{
	serial_begin(115200);
}

void loop()
{
	char t_buf[8];
	char h_buf[8];

	switch (dht_read_universal(D1)) {
		case DHTLIB_OK:
			serial_printf("Temp: %sC, Humi: %s%\r\n",
				dtostrf(dht_getTemperature(), 5, 2, t_buf),
				dtostrf(dht_getHumidity(), 5, 2, h_buf));
			break;
		case DHTLIB_ERROR_CHECKSUM:
			serial_printf("Checksum error\r\n");
			break;
		case DHTLIB_ERROR_TIMEOUT:
			serial_printf("Timeout error\r\n");
			break;
		case DHTLIB_INVALID_VALUE:
			serial_printf("Invalid value\r\n");
		default:
			serial_printf("Unknown error\r\n");
			break;
	}

	delay(3000);
}
