/*
 *  Copyright (c) 2016 - 2025 MaiKe Labs
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
#include "sht2x.h"

void setup()
{
	serial_begin(115200);
	serial_printf("Start to test SHT2x sensor!\r\n");
	wire_begin();
}

void loop()
{
	char t_buf[8];
	char h_buf[8];

	sht2x_reset();
	serial_printf("After reset, User Reg = 0x%x\r\n", sht2x_read_reg());

	dtostrf(sht2x_GetTemperature(), 5, 2, t_buf),
	dtostrf(sht2x_GetHumidity(), 5, 2, h_buf);
	serial_printf("Temperature(C): %s\r\n", t_buf);
	serial_printf("Humidity(%RH): %s\r\n", h_buf);

	sht2x_setResolution(0x81);
	serial_printf("After set resolution, User Reg = 0x%x\r\n", sht2x_read_reg());

	dtostrf(sht2x_GetTemperature(), 5, 2, t_buf),
	dtostrf(sht2x_GetHumidity(), 5, 2, h_buf);
	serial_printf("Temperature(C): %s\r\n", t_buf);
	serial_printf("Humidity(%RH): %s\r\n", h_buf);

	delay(5000);
}

