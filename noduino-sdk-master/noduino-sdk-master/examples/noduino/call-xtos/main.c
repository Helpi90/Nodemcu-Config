/*
 *  Copyright (c) 2015 - 2025 MaiKe Labs
 *
 *  This example show that how to call the in-chip bootloader(XTOS) function
 *
 *  Written by Jack Tan <jiankemeng@gmail.com>
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
#include "xtos.h"

static const char hex_chars[] = "0123456789abcdef";

void convert_hex(unsigned char *md, unsigned char *mdstr)
{
	int i;
	int j = 0;
	unsigned int c;

	for (i = 0; i < 20; i++) {
		c = (md[i] >> 4) & 0x0f;
		mdstr[j++] = hex_chars[c];
		mdstr[j++] = hex_chars[md[i] & 0x0f];
	}
	mdstr[40] = '\0';
}

int test_sha1()
{
	SHA1_CTX sha_ctx;
	char data[] = "Hello Noduino!";
	char md[SHA1_HASH_LEN];
	char mdstr[40];

	SHA1Init(&sha_ctx);
	SHA1Update(&sha_ctx, data, 6);
	SHA1Update(&sha_ctx, data + 6, 8);
	SHA1Final(md, &sha_ctx);
	convert_hex(md, mdstr);

	serial_printf("Result of SHA1 : %s\r\n", mdstr);
	return 0;
}

void setup()
{
	serial_begin(115200);
}

void loop()
{
	serial_printf("Hello In-ROM XTOS!\r\n");
	serial_printf("SPI Function: %d\r\n", SelectSpiFunction());
	test_sha1();
	delay(1500);
}
