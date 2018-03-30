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
#include "user_config.h"

void ICACHE_FLASH_ATTR led_init()
{
	// GPIO2: the wifi status led
	pinMode(2, OUTPUT);
	wifi_status_led_install (2, PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
}

void ICACHE_FLASH_ATTR wifi_led_enable()
{
	// GPIO2: the wifi status led
	wifi_status_led_install (2, PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2);
}

void ICACHE_FLASH_ATTR wifi_led_disable()
{
	wifi_status_led_uninstall();
}
