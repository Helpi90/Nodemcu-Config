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

#include "bootloader2.h"

// check the magic in the rom header at supplied rom addr
static uint32 checkMagic(uint32 addr)
{

	rom_header header;
	ets_printf("checkMagic 0x%08x\r\n", addr);

	// read rom header
	SPIRead(addr, &header, sizeof(rom_header));

	ets_printf("checkMagic 0x%02x 0x%02x\r\n", header.magic1,
		   header.magic2);

	// 0xe9 magic = standard rom header
	if (header.magic1 == 0xe9) {
		return 0;
	}
	// 0xea 0x04 magic = new type rom header (sdk lib written first)
	if (header.magic1 == 0xea && header.magic2 == 0x04) {
		// returns the length of the sdk lib
		return header.len;
	}
	// if we get here the magic was bad
	ets_printf("error magic!\n");
	return 0xffffffff;
}

static uint32 loadImage(uint32 romaddr)
{

	uint32 ret;
	stage3loader *loader;

	// check the magic
	ret = checkMagic(romaddr);
	if (ret == 0xffffffff) {
		return FAIL;
	}
	if (ret != 0) {
		// new type image with sdk lib first, need to skip all this to get to normal header
		romaddr += (sizeof(rom_header) + ret);
	}
	// run 3rd stage, pass pointer to start of rom to load
	loader = (stage3loader *) 0x4010FC08;
	return loader((void *)romaddr);
}

// loc_4010009C
static void swapboot(uint32 configsect, rom_config * a3)
{

	uint8 bootconfig[4];
	uint8 romconfig[0x1000];
	uint32 sectoffset;
	uint32 addroffset;

	uint32 configaddr = configsect << 0x0c;

	// read boot config (just 4 bytes)
	SPIRead(configaddr + 0x3000, bootconfig, 4);
	// find rom config offset, based on user rom to boot
	if (bootconfig[0] == 0) {
		addroffset = 0x1000;
	} else {
		addroffset = 0x2000;
	}

	// switch selected boot rom in boot config
	// and work out address for opposite config slot
	if (bootconfig[0] == 0) {
		sectoffset = 2;
		addroffset = 0x2000;
		bootconfig[0] = 1;
	} else {
		sectoffset = 1;
		addroffset = 0x1000;
		bootconfig[0] = 0;
	}

	// todo: have moved from above, check
	// read rom config
	SPIRead(configaddr + addroffset, romconfig, 0x1000);

	// write ??? something to start of rom config sector
	ets_memcpy(romconfig, a3, sizeof(rom_config));

	// write back rom config, to opposite slot!
	ets_printf("erase %08x, write to %08x\r\n", configsect + sectoffset,
		   configaddr + addroffset);
	SPIEraseSector(configsect + sectoffset);
	SPIWrite(configaddr + addroffset, romconfig, 0x1000);

	// write back boot config
	ets_printf("erase %08x, write to %08x\r\n", configsect + 3,
		   configaddr + 0x3000);
	SPIEraseSector(configsect + 3);
	SPIWrite(configaddr + 0x3000, bootconfig, 4);

	return;
}

// loc_40100160
void reboot()
{

	uint32 x;
	uint32 base = 0x60000600;
	volatile uint32 *r300 = (uint32 *) (base + 0x300);
	volatile uint32 *r304 = (uint32 *) (base + 0x304);

	*r304 = 0;

	x = *r300;
	x |= 0x38;
	*r300 = x;

	x = *r300;
	x &= 0x79;
	x |= 4;
	*r300 = x;

	x = *r300;
	x |= 1;
	*r300 = x;

	while (1) {
		ets_delay_us(0x4E20);
	}
}

// loc_401001BC
void bootFailedSwap(uint32 a2, rom_config * a3)
{

	if (a3->byte0 & BYTE0_USEADDRESS2 == 0) {
		// already marked as bad
		ets_printf("backup boot failed.\n\n");
		return;
	}
	a3->byte0 ^= BYTE0_USEADDRESS2;	// mark as bad
	swapboot(a2, a3);
	ets_printf("first boot failed, reboot to try backup bin\n\n");
	reboot();

	return;
}

// loc_401001F4
uint32 getAddress(uint8 * a2)
{
	return ((uint32) a2[2] << 0x10) | ((uint32) a2[1] << 0x08) | (uint32)
	    a2[0];
}

// rom layout
// 0x000000 main rom header
// 0x000010 bootloader
// 0x001000 rom1 inc header
// 0xVaries rom2 inc header
// 0xVVc000 (last 0x4000):
//   0xc000 ???
//   0xd000 rom1 config
//   0xe000 rom2 config
//   0xf000 bootloader config

void call_user_start()
{

	uint8 header[8];
	uint8 config[4];
	rom_config romconf;
	uint8 a0, a2, a3;
	uint32 a12, runAddr;

	//uart_div_modify(0,UART_CLK_FREQ / (115200));

	ets_delay_us(2000000);

	ets_printf("\n2nd boot version : 1.3(b3) - rab\n");

	// read rom header
	SPIRead(0, header, 0x08);

	// print spi speed
	ets_printf("  SPI Speed      : ");
	a0 = header[3] & 0x0f;
	if (a0 == 0) {
		ets_printf("40MHz\n");
	} else if (a0 == 1) {
		ets_printf("26.7MHz\n");
	} else if (a0 == 2) {
		ets_printf("20MHz\n");
	} else if (a0 == 0x0f) {
		ets_printf("80MHz\n");
	}
	// print spi mode
	ets_printf("  SPI Mode       : ");
	a0 = header[2];
	if (a0 == 0) {
		ets_printf("QIO\n");
	} else if (a0 == 1) {
		ets_printf("QOUT\n");
	} else if (a0 == 2) {
		ets_printf("DIO\n");
	} else if (a0 == 3) {
		ets_printf("DOUT\n");
	}
	// print flash size and get config sector
	ets_printf("  SPI Flash Size : ");
	a0 = header[3] >> 4;
	if (a0 == 0) {
		ets_printf("4Mbit\n");
		a12 = 0x7C;
	} else if (a0 == 1) {
		ets_printf("2Mbit\n");
		a12 = 0x3C;
	} else if (a0 == 2) {
		ets_printf("8Mbit\n");
		a12 = 0xFC;
	} else if (a0 == 3) {
		ets_printf("16Mbit\n");
		a12 = 0x1FC;
	} else if (a0 == 4) {
		ets_printf("32Mbit\n");
		a12 = 0x3FC;
	} else {
		ets_printf("4Mbit\n");
		a12 = 0x7C;
	}

	// read 4 bytes from config section (last 0x1000 bytes of eeprom)
	SPIRead((a12 << 0xc) + 0x3000, config, 4);
	// sector offset for rom config
	if (config[0] == 0) {
		a2 = 1;
	} else {
		a2 = 2;
	}

	// read 8 bytes from romX config section
	ets_printf("read conf at %08x\r\n", (a2 + a12) << 0x0c);
	SPIRead((a2 + a12) << 0x0c, &romconf, 8);
	ets_printf("byte1 %02x, byte0 %02x\r\n", romconf.byte1, romconf.byte0);
	if (((romconf.byte1 & 0x1f) != 3) || ((romconf.byte0 & 0x03) == 3)) {
		ets_printf("log 1\n");
		// create new rom config and swap rom
		ets_memset(&romconf, 0xff, sizeof(rom_config));
		romconf.byte0 &= 0xfc;
		romconf.byte1 &= 0xe0;
		romconf.byte1 |= 3;
		swapboot(a12, &romconf);
		ets_printf("log 2\n");
	}

	ets_printf("log 3\n");

	// copy third stage bootloader
	// later will pass execution to function at 0x4010FC08
	ets_memcpy((void *)0x4010FC00, _text_data, _text_len);
	ets_printf("log 4\n");
	if ((romconf.byte1 & 0x40) || (romconf.byte1 & BYTE1_TESTMODE == 0)) {
		ets_printf("log 5\n");
		if (romconf.byte1 >> 7) {
			ets_printf("jump to run user");
			a3 = romconf.byte0 & 3;
			if (a3 != 0) {
				if (romconf.byte0 & BYTE0_USEADDRESS2) {
					if (a3 == 1) {
						ets_printf("2\n\n");
						if ((a12 == 0x1FC)
						    || (a12 == 0x3FC)) {
							runAddr = 0xFC;	// cap rom size for bigger roms
						} else {
							runAddr = a12;
						}
						runAddr += 4;	// end of config sector
						runAddr = runAddr >> 1;	// divide by two (middle of rom sector)
						runAddr = runAddr << 0x0c;	// convert sector to address
						runAddr += 0x1000;	// skip 4k (in lower rom this contains the bootloader, in upper rom it's wasted)
						ets_printf("log 6");
						if (loadImage(runAddr) == FAIL) {
							bootFailedSwap(a12,
								       &romconf);
						}
					}
					return;
				}
				if (a3 != 1) {
					ets_printf
					    ("error user bin flag, flag = %x\n");
					return;
				}
			}
			ets_printf("1\n\n");
			ets_printf("log 7\n");

			if (loadImage(0x1000) == FAIL) {
				bootFailedSwap(a12, &romconf);
			}
			return;
		}
	}

	ets_printf("log 8\n");
	return;

	if ((romconf.byte1 & BYTE1_xxxMODE == 0)
	    || (romconf.byte1 & BYTE1_TESTMODE)) {
		runAddr = getAddress(romconf.address1);
		ets_printf("test mode, ");
	} else {
		if (romconf.byte1 & BYTE1_RUNUSERMODE == 0) {
			if (romconf.byte0 & BYTE0_USEADDRESS2) {
				runAddr = getAddress(romconf.address2);
			} else {
				runAddr = getAddress(romconf.address1);
			}
			ets_printf("enhanced boot mode, ");
		}
	}

	// jump to arbitrary location for boot
	ets_printf("jump to run bin @ %x\n\n", runAddr);
	if (loadImage(runAddr) == FAIL) {
		if ((romconf.byte1 & BYTE1_xxxMODE == 0)
		    && (romconf.byte1 & BYTE1_TESTMODE))
			return;
		if (romconf.byte1 >> 7 == 0) {
			bootFailedSwap(a12, &romconf);
		}
	}

}
