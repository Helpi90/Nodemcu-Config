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
#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C / TWI 

const uint8_t rook_bitmap[] PROGMEM = {
	0x00,			// 00000000 
	0x55,			// 01010101
	0x7f,			// 01111111
	0x3e,			// 00111110
	0x3e,			// 00111110 
	0x3e,			// 00111110
	0x3e,			// 00111110 
	0x7f			// 01111111
};

void draw(void)
{
	// graphic commands to redraw the complete screen should be placed here  
	u8g.drawBitmapP(60, 30, 1, 8, rook_bitmap);
}

void setup(void)
{
}

void loop(void)
{
	// picture loop
	u8g.firstPage();
	do {
		draw();
	} while (u8g.nextPage());

	// rebuild the picture after some delay
	delay(1000);
}
