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
#include "user_config.h"

struct minik_saved_param {
	uint8_t status;
	uint8_t pad[3];
};

LOCAL struct minik_saved_param minik_param;

void ICACHE_FLASH_ATTR param_set_status(uint8_t status)
{
	if (status != minik_param.status) {
		if (status > 1) {
#ifdef DEBUG
			os_printf("Error status input!\n");
#endif
			return;
		}

		minik_param.status = status;
	}
}

uint8_t ICACHE_FLASH_ATTR param_get_status(void)
{
	return minik_param.status;
}

void ICACHE_FLASH_ATTR param_save(void)
{
	spi_flash_erase_sector(PARAM_START_SEC + 0);
	spi_flash_write((PARAM_START_SEC + 0) * SPI_FLASH_SEC_SIZE,
			(uint32 *) & minik_param,
			sizeof(struct minik_saved_param));
}

void ICACHE_FLASH_ATTR param_init()
{
	spi_flash_read((PARAM_START_SEC + 0) * SPI_FLASH_SEC_SIZE,
		       (uint32 *) & minik_param,
		       sizeof(struct minik_saved_param));

	// init data of spi flash
	if (minik_param.status == 0xff) {
#ifdef DEBUG
		os_printf("Invalid status value, reset to 0!\n");
#endif
		minik_param.status = 0;
	}

	os_printf("Saved Status is: %d\n", param_get_status());
}
