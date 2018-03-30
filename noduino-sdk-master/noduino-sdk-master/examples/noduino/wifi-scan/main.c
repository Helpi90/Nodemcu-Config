/*
 *  Copyright (c) 2015 - 2025 MaiKe Labs
 *
 *  Connect SDA to i2c data  - GPIO4
 *  Connect SCL to i2c clock - GPIO5
 *
 *  Scanning addresses changed from 1..127
 *  This sketch tests the standard 7-bit addresses
 *  Devices with higher bit address might not be seen properly.
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

#include "noduino.h"

static void ap_scan_done(void *arg, STATUS status)
{
	if(status == OK) {
		struct bss_info *bss_link = (struct bss_info *)arg;
		while (bss_link != NULL) {
			serial_printf("%32s (%02x:%02x:%02x:%02x:%02x:%02x) rssi: %02d ch: %02d auth: %02d hidden: %d\r\n",
					bss_link->ssid,
					MAC2STR(bss_link->bssid),
					bss_link->rssi,
					bss_link->channel,
					bss_link->authmode,
					bss_link->is_hidden
				);

			bss_link = bss_link->next.stqe_next;
		}
		serial_printf("\r\n");
	} else {
		serial_printf("Scan Failed\r\n");
	}
	serial_printf("------------ Delay 6s to start a new scan ... --------------\r\n");
}

void setup()
{
	serial_begin(115200);
	wifi_set_opmode(STATION_MODE);

	serial_printf("\r\nWelcom to Noduino!\r\n");
	serial_printf("WiFi AP SSID Scanning...\r\n");
}

void loop()
{
	wifi_station_scan(NULL, ap_scan_done);
	delay(6*1000);
}
