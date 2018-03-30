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

#define ICACHE_FLASH_ATTR __attribute__((section(".irom0.text")))

#include "bootloader2-extra.h"

extern uint32 SPIRead(uint32 addr, void *outptr, uint32 len);
extern void ets_printf(char *, ...);
extern void SPIEraseSector(int);
extern void SPIWrite(uint32 addr, void *inptr, uint32 len);
extern void ets_delay_us(int);
extern void ets_memset(void *, uint8, uint32);
extern void ets_memcpy(void *, const void *, uint32);
extern void Uart_Init(uint32, uint32);
extern void uart_div_modify(uint8, uint32);

#define  CPU_CLK_FREQ                                80*1000000
#define  APB_CLK_FREQ                                CPU_CLK_FREQ
#define  UART_CLK_FREQ                               APB_CLK_FREQ

#define BYTE0_USEADDRESS2 0x04

#define BYTE1_TESTMODE    0x20
#define BYTE1_xxxMODE     0x40
#define BYTE1_RUNUSERMODE 0x80

#define FAIL 1

typedef uint32 stage3loader(void *);

// normal rom header is 8 bytes
// if a new type rom (sdk lib first) there is
// another 8 byte header straight afterwards
typedef struct {
	// general rom header
	uint8 magic1;
	uint8 magic2;
	uint8 stuff[6];
	// new type rom, lib header
	uint32 add;
	uint32 len;
} rom_header;

typedef struct {
	uint8 byte0;
	uint8 byte1;
	uint8 address1[3];
	uint8 address2[3];
} rom_config;
