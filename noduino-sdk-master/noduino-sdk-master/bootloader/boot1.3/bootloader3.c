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

static uint32 sub_3FFE8018(uint32 readpos)
{

	uint8 stack[0x10];
	uint8 temp[0x10];

	rom_header header;
	uint32 sectaddr;
	uint32 sectcount;
	uint32 sectlen;
	uint8 chksum;
	uint32 loop;
	bool fullsize;
	uint32 blocksof16;
	uint32 remainder;
	uint32 processed;
	uint32 toprocess;

	uint8 *datapos;

	if (SPIRead(readpos, &header, 0x10) != 0) {
		return 1;
	}

	if (header.magic1 != 0xe9) {
		return 1;
	}

	sectaddr =
	    header.add[3] << 24 | header.
	    add[2] << 16 | header.add[1] << 8 | header.add[0];
	sectlen =
	    header.len[3] << 24 | header.
	    len[2] << 16 | header.len[1] << 8 | header.len[0];

	if (SPIRead(readpos + 0x10, stack, 0x10) != 0) {
		return 1;
	}
	readpos += 0x20;

	if (sectcount != 0) {

		datapos = stack;	// read data
		chksum = CHKSUM_INIT;	// checksum init
		outaddr = sectaddr;	// section address
		processed = 0;

		for (sectcount = header.count; sectcount > 0; sectcount--) {

			toprocess = 0x10 - processed;	// left to process
			if (sectlen < toprocess) {	// section shorter than remaining bytes
				fullsize = false;
				blocksof16 = 0;
				remainder = sectlen;
			} else {
				fullsize = true;

				// process what's left in the buffer
				if ((toprocess & 3 == 0) && (toprocess > 0)) {	// non-zero exact multiple of 4
					a9 = toprocess / 4;
					for (loop = 0; loop < a9; loop++) {	// process 4 bytes at a time
						chksum ^= datapos[0];	// chksum the 4 bytes
						chksum ^= datapos[1];
						chksum ^= datapos[2];
						chksum ^= datapos[3];
						*(uint32 *) outaddr = *(uint32 *) datapos;	// copy the 4 bytes
						datapos += 4;	// advance input pointer
						outaddr += 4;	// advance output pointer
					}
				} else {	// single byte method
					for (loop = 0; loop < toprocess; loop++) {
						chksum ^= datapos[0];	// add to chksum
						outaddr[0] = datapos[0];	// copy byte
						datapos++;	// advance input pointer
						outaddr++;	// advance output pointer
					}
				}
				blocksof16 = (sectlen - toprocess) / 16;
				remainder = (sectlen - toprocess) % 16;
			}

			// process full blocks of 16
			if (blocksof16) {
				for (loop = 0; loop < blocksof16; loop++) {	// loop for remaining/16 times
					if (SPIRead(readpos, stack, 0x10) != 0) {	// read 16 bytes
						return 1;
					}
					readpos += 0x10;	// increment readpos
					for (a0 = 0; a0 < 0x10; a0++) {
						chksum ^= stack[a0];	// add to chksum
					}
					ets_memcpy(outaddr, stack, 0x10);	// copy block
					outaddr += 0x10;	// increment outaddr
				}
			}

			if (fullsize) {	// get next block for remainder
				if (SPIRead(readpos, stack, 0x10) != 0) {
					return 1;
				}
				readpos += 0x10;
				datapos = stack;
				processed = 0;
			}

			if (remainder) {	// process the remainder after the blocks of 16
				if (remainder % 4 == 0) {	// multiple of 4
					a7 = remainder / 4;
					for (loop = 0; loop < a7; loop++) {	// process 4 bytes at a time
						chksum ^= datapos[3];	// chksum the 4 bytes
						chksum ^= datapos[2];
						chksum ^= datapos[1];
						chksum ^= datapos[0];
						*(uint32 *) (outaddr) = *(uint32 *) (datapos);	// copy the 4 bytes
						datapos += 4;	// advance input pointer
						outaddr += 4;	// advance output pointer
					}
				} else {	// single byte version
					for (loop = 0; loop < remainder; loop++) {
						chksum ^= datapos[0];	// chksum
						outaddr[0] = datapos[0];	// copy byte
						datapos++;
						outaddr++;
					}
				}
				processed += remainder;
			}

			if (sectcount > 1) {	// read next section
				toprocess = 0x10 - processed;	// left to process in buffer
				if (toprocess >= 8) {	// next header is already in buffer
					sectlen =
					    datapos[7] << 24 | datapos[6] << 16
					    | datapos[5] << 8 | datapos[4];
					outaddr =
					    datapos[3] << 24 | datapos[2] << 16
					    | datapos[1] << 8 | datapos[0];
					processed += 8;	// processed another 8
				} else {
					ets_memcopy(temp, stack + processed, toprocess);	// copy remaining to temp buffer
					if (SPIRead(readpos, stack, 0x10) != 0) {	// read as normal
						break;
					}
					readpos += 0x10;
					processed = 8 - toprocess;	// how many of new will be processed in this operation
					ets_memcpy(temp + toprocess, stack, processed);	// copy needed bytes from new read to temp buffer
					outaddr = *(uint32 *) (temp);	// get values from temp buffer
					sectlen = *(uint32 *) (temp + 4);
				}
				datapos = stack + processed;
			}
		}
		if (stack[0x0f] != chksum) {	// compare calculated and stored checksums
			return 1;	// chksum failed
		}
	}

	callx0(header.entry);
	return 0;
}
