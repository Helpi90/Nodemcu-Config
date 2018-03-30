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

static int32_t pos = 0;
static bool past_b = 0;

void do_encoder_a()
{
	past_b = digitalRead(D9);
	past_b ? pos++ : pos--;
	serial_printf("pos = %d\r\n", pos);
}

void setup()
{
	serial_begin(115200);

	pinMode(D9, INPUT);

	past_b = (bool)digitalRead(D9);
	
	attachInterrupt(D8, do_encoder_a, RISING);
}

void loop()
{
}
