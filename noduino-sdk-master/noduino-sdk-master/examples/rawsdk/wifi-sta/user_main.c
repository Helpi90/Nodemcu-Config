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

#include "osapi.h"
#include "os_type.h"
#include "user_interface.h"
#include "driver/uart.h"

#define AP_NAME		"YOUR_ROUTER_AP_NAME"
#define AP_PASS		"YOUR_ROUTER_AP_PASSWORD"

// user entry
void ICACHE_FLASH_ATTR user_init()
{
	struct station_config config;

	uart_init(115200, 115200);

	//Go to station mode
	if (wifi_get_opmode() != STATION_MODE) {
		wifi_set_opmode(STATION_MODE);
	}

	//Connect to the defined access point.
	wifi_station_get_config(&config);

	os_memset(&config, 0x0, sizeof(struct station_config));
	strcpy(config.ssid, AP_NAME);
	strcpy(config.password, AP_PASS);

	wifi_station_set_config(&config);
}
