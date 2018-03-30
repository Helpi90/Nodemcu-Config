/*
 *  Copyright (c) 2015 - 2025 MaiKe Labs
 *
 *  Decompiling the esp8266 bootloader v1.3(b3)
 *  Based on the Richard's great work
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

typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

extern uint32 SPIRead(uint32 addr, void *outptr, uint32 len);
extern void ets_printf(char *, ...);
extern void SPIEraseSector(int);
extern void SPIWrite(uint32 addr, void *inptr, uint32 len);
extern void ets_delay_us(int);
extern void ets_memset(void *, uint8, uint32);
extern void ets_memcpy(void *, const void *, uint32);

#define CHKSUM_INIT 0xef

typedef struct {
	// general rom header
	uint8 magic1;
	uint8 count;
	uint8 byte2;
	uint8 byte3;
	uint32 entry;
	// new type rom, lib header
	uint8 add[4];
	uint8 len[4];
} rom_header;

typedef void func(void);
