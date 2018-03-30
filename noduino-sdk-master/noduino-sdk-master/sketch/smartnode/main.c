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
#include "compile.h"

static os_timer_t client_timer;

void first_check_ip(void)
{
    struct ip_info ipconfig;
	static int count = 0;

    os_timer_disarm(&client_timer);

    wifi_get_ip_info(STATION_IF, &ipconfig);

    if (ipconfig.ip.addr != 0) {
		ota_start();
		count = 0;
	} else {
        os_timer_setfn(&client_timer, (os_timer_func_t *)first_check_ip, NULL);
        os_timer_arm(&client_timer, 1000, 0);
		count++;
    }

	if (count > 8)
		system_restart();
}

irom void produce_init(void)
{
    if (wifi_get_opmode() != SOFTAP_MODE) {
        os_timer_disarm(&client_timer);
        os_timer_setfn(&client_timer, (os_timer_func_t *)first_check_ip, NULL);
        os_timer_arm(&client_timer, 1000, 0);
    }
}

irom void user_init(void)
{
#ifdef DEBUG
	uart_init(115200, 115200);
#endif
	os_printf("\r\n\r\n\r\n\r\n\r\n\r\n");
	os_printf("\r\nWelcom to Noduino SmartNode module!\r\n");
	os_printf("Current firmware is user%d.bin\r\n", system_upgrade_userbin_check()+1);
	os_printf("Will connect to the WiFi Router which SSID and Password are 'noduino'!\r\n");
	os_printf("Then download firmware from http://192.168.1.72/ota/user2.bin\r\n");
	os_printf("%s", noduino_banner);

    struct station_config conf = {
        .ssid = AP_SSID,
        .password = AP_PASSWORD,
    };

    wifi_set_opmode(STATIONAP_MODE);
    wifi_station_set_config(&conf);
    wifi_station_disconnect();
    wifi_station_connect();

	system_init_done_cb(produce_init);
}
