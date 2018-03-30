/*
 *  Copyright (c) 2015 - 2025 MaiKe Labs
 *
 *  Http Client example
 *
 *  Rewritten by Jack Tan <jiankemeng@gmail.com>
 *  Based on great work from Martin d'Allens <martin.dallens@gmail.com>
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
#include "sntp.h"
#include "driver/uart.h"

#define AP_NAME		"YOUR_ROUTER_AP_NAME"
#define AP_PASS		"YOUR_ROUTER_AP_PASSWORD"

void wifi_enter_sta()
{
	struct station_config config;

	//Connect to the defined access point.
	wifi_station_get_config(&config);

	os_memset(&config, 0x0, sizeof(struct station_config));
	strcpy(config.ssid, AP_NAME);
	strcpy(config.password, AP_PASS);

	wifi_set_opmode(STATIONAP_MODE);
	wifi_station_set_config(&config);
	wifi_station_disconnect();
	wifi_station_connect();
}

static ETSTimer test_timer;

static void ICACHE_FLASH_ATTR test_timer_cb()
{
	int32_t cs = sntp_get_current_timestamp();

	os_printf("Current timestamp: %d\r\n", cs);
	os_printf("Current Time: %s\r\n\r\n", sntp_get_real_time(cs));
	os_timer_arm(&test_timer, 3000, 0);
}

void ICACHE_FLASH_ATTR sntp_test()
{
	sntp_init();
	sntp_setservername(0, (char*)"cn.pool.ntp.org");
	sntp_setservername(1, (char*)"1.cn.pool.ntp.org");
	sntp_setservername(2, (char*)"2.cn.pool.ntp.org");

	os_timer_disarm(&test_timer);
	os_timer_setfn(&test_timer, test_timer_cb, NULL);
	os_timer_arm(&test_timer, 0, 0);	// Start immediately.
}

//user_init is the user entry point of the Espressif SDK
void ICACHE_FLASH_ATTR user_init()
{
	//Initialize the uart0 and uart1 in 115200 bitrate
	uart_init(115200, 115200);

	wifi_enter_sta();

	os_delay_us(100000);
	os_delay_us(100000);

	system_init_done_cb(sntp_test);
}
