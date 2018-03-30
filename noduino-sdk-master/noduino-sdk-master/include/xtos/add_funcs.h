/*
 * Do not sorted functions ROM-XTOS
 * Author: PV`
 * (c) PV` 2015
 */

#ifndef __ADD_FUNCS_H__
#define __ADD_FUNCS_H__

int rom_get_power_db(void);
void rom_en_pwdet(int);
void rom_i2c_writeReg(uint32 block, uint32 host_id, uint32 reg_add,
		      uint32 data);
void rom_i2c_writeReg_Mask(uint32 block, uint32 host_id, uint32 reg_add,
			   uint32 Msb, uint32 Lsb, uint32 indata);
uint8 rom_i2c_readReg_Mask(uint32 block, uint32 host_id, uint32 reg_add,
			   uint32 Msb, uint32 Lsb);
uint8 rom_i2c_readReg(uint32 block, uint32 host_id, uint32 reg_add);

#endif
