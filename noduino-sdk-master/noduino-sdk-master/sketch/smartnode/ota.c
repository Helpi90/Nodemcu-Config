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
#include "user_interface.h"

#ifdef DEBUG
#define ESP_DBG os_printf
#else
#define ESP_DBG
#endif

#define pheadbuffer "Connection: keep-alive\r\n\
Cache-Control: no-cache\r\n\
User-Agent: Mozilla/5.0 (Windows NT 5.1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.101 Safari/537.36 \r\n\
Accept: */*\r\n\
Accept-Encoding: gzip,deflate,sdch\r\n\
Accept-Language: zh-CN,zh;q=0.8\r\n\r\n"

static os_timer_t client_timer;

void check_ip(void);

/* Processing the downloaded data from the server */
irom static void upgrade_rsp(void *arg)
{
    struct upgrade_server_info *server = arg;
    if (server->upgrade_flag == true) {
    	ESP_DBG("OTA: upgrade successful!\n");
		ESP_DBG("OTA: upgrade flag is: %d\n", system_upgrade_flag_check());
    	ESP_DBG("OTA: reboot to the new firmware now!\n");
		system_upgrade_reboot();
    } else {
        ESP_DBG("OTA: upgrade failed!\n");
    }

    os_free(server->url);
    server->url = NULL;
    os_free(server);
    server = NULL;
}

/* Processing the received data from the server */
irom static void upgrade_begin(struct espconn *pespconn, struct upgrade_server_info *server)
{
    uint8 user_bin[30] = {0};

#ifndef LOCAL_OTA
	// icamgo.com/ota/dev/mjyun-wifi-mod/
    const char ota_server_ip[4] = {182,92,5,106};
#else
    const char ota_server_ip[4] = {192,168,1,72};
#endif

    server->pespconn = pespconn;
    server->port = 80;
    server->check_cb = upgrade_rsp;
    server->check_times = 120000;

    os_memcpy(server->ip, ota_server_ip, 4);

    if (server->url == NULL) {
        server->url = (uint8 *)os_zalloc(512);
    }
#ifndef LOCAL_OTA
	// in general, we need to check user bin here
    if (system_upgrade_userbin_check() == UPGRADE_FW_BIN1) {
        os_memcpy(user_bin, "ota/dev/smartnode/files/user2.bin", 39);
    } else if (system_upgrade_userbin_check() == UPGRADE_FW_BIN2) {
        os_memcpy(user_bin, "ota/dev/smartnode/files/user1.bin", 39);
    }
    
    os_sprintf(server->url, "GET /%s HTTP/1.0\r\nHost: "IPSTR":%d\r\n"pheadbuffer"",
           user_bin, IP2STR(server->ip), 80);
#else
	// in general, we need to check user bin here
    if (system_upgrade_userbin_check() == UPGRADE_FW_BIN1) {
        os_memcpy(user_bin, "ota/user2.bin", 14);
    } else if (system_upgrade_userbin_check() == UPGRADE_FW_BIN2) {
        os_memcpy(user_bin, "ota/user1.bin", 14);
    }
    
    os_sprintf(server->url, "GET /%s HTTP/1.0\r\nHost: "IPSTR":%d\r\n"pheadbuffer"",
           user_bin, IP2STR(server->ip), 80);
#endif

    if (system_upgrade_start(server) == false) {
        ESP_DBG("OTA: upgrade is already started\n");
    }
}

/* espconn struct parame init when get ip addr */
irom void check_ip(void)
{
    struct ip_info ipconfig;

    os_timer_disarm(&client_timer);

    wifi_get_ip_info(STATION_IF, &ipconfig);

    if (ipconfig.ip.addr != 0) {
    	struct espconn *pespconn = NULL;
    	struct upgrade_server_info *server = NULL;
    	server = (struct upgrade_server_info *)os_zalloc(sizeof(struct upgrade_server_info));
    	upgrade_begin(pespconn , server);
    } else {
        os_timer_setfn(&client_timer, (os_timer_func_t *)check_ip, NULL);
        os_timer_arm(&client_timer, 100, 0);
    }
}

/* device parame init based on espressif platform */
irom void ota_start(void)
{
	os_timer_disarm(&client_timer);
	os_timer_setfn(&client_timer, (os_timer_func_t *)check_ip, NULL);
	os_timer_arm(&client_timer, 100, 0);
}
