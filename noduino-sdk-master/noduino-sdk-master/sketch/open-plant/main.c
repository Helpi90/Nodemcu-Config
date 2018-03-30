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
#include "time.h"
#include "sht2x.h"
#include "compile.h"

#define DEBUG				1

static uint32_t realtime = 1;
static uint32_t network_state = 0;

irom static void mjyun_stated_cb(mjyun_state_t state)
{
    if (mjyun_state() != state)
        INFO("Platform: mjyun_state error \r\n");

    switch (state)
    {
        case WIFI_IDLE:
            INFO("Platform: WIFI_IDLE\r\n");
            break;
        case WIFI_SMARTLINK_LINKING:
            INFO("Platform: WIFI_SMARTLINK_LINKING\r\n");
            break;
        case WIFI_SMARTLINK_FINDING:
            INFO("Platform: WIFI_SMARTLINK_FINDING\r\n");
            break;
        case WIFI_SMARTLINK_TIMEOUT:
            INFO("Platform: WIFI_SMARTLINK_TIMEOUT\r\n");
            break;
        case WIFI_SMARTLINK_GETTING:
            INFO("Platform: WIFI_SMARTLINK_GETTING\r\n");
            break;
        case WIFI_SMARTLINK_OK:
            INFO("Platform: WIFI_SMARTLINK_OK\r\n");
            break;
        case WIFI_AP_OK:
            INFO("Platform: WIFI_AP_OK\r\n");
            break;
        case WIFI_AP_ERROR:
            INFO("Platform: WIFI_AP_ERROR\r\n");
            break;
        case WIFI_AP_STATION_OK:
            INFO("Platform: WIFI_AP_STATION_OK\r\n");
			network_state = WIFI_AP_STATION_OK;
            break;
        case WIFI_AP_STATION_ERROR:
            INFO("Platform: WIFI_AP_STATION_ERROR\r\n");
            break;
        case WIFI_STATION_OK:
            INFO("Platform: WIFI_STATION_OK\r\n");
			network_state = WIFI_STATION_OK;
            break;
        case WIFI_STATION_ERROR:
            INFO("Platform: WIFI_STATION_ERROR\r\n");
            break;
        case WIFI_STA_DISCONNECTED:
            INFO("Platform: WIFI_STA_DISCONNECTED\r\n");
			network_state = WIFI_STA_DISCONNECTED;
            break;
        case MJYUN_CONNECTING:
            INFO("Platform: MJYUN_CONNECTING\r\n");
            break;
        case MJYUN_CONNECTING_ERROR:
            INFO("Platform: MJYUN_CONNECTING_ERROR\r\n");
            break;
        case MJYUN_CONNECTED:
            INFO("Platform: MJYUN_CONNECTED \r\n");
            break;
        case MJYUN_DISCONNECTED:
            INFO("Platform: MJYUN_DISCONNECTED\r\n");
            break;
        default:
            break;
	}
}

irom void mjyun_receive(const char *event_name, const char *event_data)
{
#ifdef DEBUG
	INFO("RECEIVED: key:value [%s]:[%s]", event_name, event_data);
#endif

	if(os_strncmp(event_data, "ota", 3) == 0)
	{
#ifdef DEBUG
		INFO("OTA: upgrade the firmware!\r\n");
#endif
		mjyun_mini_ota_start("ota/dev/open-plant/files");
	}
}

os_timer_t time_temp;

static int mqttrate_sec = MQTT_SEND_RATE_SEC; //2 second
static int httprate_min = HTTP_SEND_RATE_MIN; //5 min

static int tmp_timer = 0;

irom char *str_trim(char *str)
{
	char *end;

	// Trim leading space
	while(isspace(*str)) str++;

	if(*str == 0)  // All spaces?
	return str;

	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace(*end)) end--;

	// Write new null terminator
	*(end+1) = 0;

	return str;
}

static upload_fail_cnt = 0;

void http_upload_temp_error_handle()
{
	upload_fail_cnt++;
	if(upload_fail_cnt >= 3) {
		// failed about 5min
		INFO("http pushed failed %d times, reset the system\r\n", upload_fail_cnt);
		system_restart();
	}

	//TODO: store the data in flash
}

void http_upload_temp_cb(char *response, int http_status, char *full_response)
{
	if ( HTTP_STATUS_GENERIC_ERROR != http_status )
	{
#ifdef DEBUG
		INFO( "%s: strlen(full_response)=%d\r\n", __func__, strlen( full_response ) );
		INFO( "%s: response=%s<EOF>\r\n", __func__, response );
		INFO( "%s: memory left=%d\r\n", __func__, system_get_free_heap_size() );
#endif

		if(os_strncmp(response, "ok", 2) != 0)
			http_upload_temp_error_handle();

	} else {
		http_upload_temp_error_handle();
#ifdef DEBUG
		INFO( "%s: http_status=%d\r\n", __func__, http_status );
#endif
	}
}

void http_upload_temp(char *tt)
{
	uint8_t * URL = (uint8_t *) os_zalloc( os_strlen(HTTP_UPLOAD_URL) +
	                  os_strlen( mjyun_getdeviceid() ) +
	                  os_strlen( "gh_8356436e6809" ) +
	                  os_strlen( tt ) +
	                  12);
	if ( URL == NULL ) {
#ifdef DEBUG
		INFO( "%s: not enough memory\r\n", __func__ );
#endif
		return;
	}

	uint32_t cs = time(NULL);

	os_sprintf( URL,
	            HTTP_UPLOAD_URL,
	            mjyun_getdeviceid(),
				"gh_95fae1ba6fa0",
	            str_trim(tt),
	            cs);
	http_get((const char *) URL , "", http_upload_temp_cb);
#ifdef DEBUG
	INFO("%s\r\n", (char *)URL);
#endif
	os_free( URL );
}

void push_temp_humi()
{
	char t_buf[8];
	char h_buf[8];

	sht2x_reset();
	dtostrf(sht2x_GetTemperature(), 5, 2, t_buf),
	dtostrf(sht2x_GetHumidity(), 5, 2, h_buf);

	INFO("Temperature(C): %s\r\n", t_buf);
	INFO("Humidity(%RH): %s\r\n", h_buf);

	pcf8563_now();

	if (1 == realtime) {
		mjyun_publishstatus(t_buf);

		if (tmp_timer==(httprate_min*60)/mqttrate_sec) {
			// per first luanch send temp data
			http_upload_temp(t_buf);
			tmp_timer=1;
		}

		tmp_timer++;
	} else {
		http_upload_temp(t_buf);
	}
}

irom void time_init()
{
	//struct tm *tblock;
	uint32_t cs = time(NULL);
	INFO("Current timestamp: %d\r\n", cs);

	time_t timer = cs;
	INFO("%s @ %d\r\n", __func__, __LINE__);

	set_dt_from_seconds(cs);
	INFO("%s @ %d\r\n", __func__, __LINE__);
	pcf8563_set(NULL);
	INFO("Update the latest time to RTC PCF8563!\r\n");
	pcf8563_now();
}

void mjyun_connected()
{
	time_init();

	mjyun_setssidprefix("MJY_");

	// stop to show the wifi status
	wifi_led_disable();
}

void mjyun_disconnected()
{
	// show the wifi status
	wifi_led_enable();
}


mjyun_config_t mjyun_conf = {
	"MJP1841706138",	/* MAIKE WiFi Soil Sensor */
	HW_VERSION,
	FW_VERSION,
	FW_VERSION,
	"Device Offline",
	0
};

irom void init_yun()
{
	mjyun_statechanged(mjyun_stated_cb);
	mjyun_ondata(mjyun_receive);
	mjyun_onconnected(mjyun_connected);
	mjyun_ondisconnected(mjyun_disconnected);

	if (realtime == 1)
		mjyun_conf.run_flag |= WITH_MQTT;
	mjyun_run(&mjyun_conf);
}

irom void setup()
{
#ifdef DEBUG
	uart_init(115200, 115200);
#endif
	os_delay_us(100);
	INFO("\r\n\r\n\r\n\r\n\r\n\r\n");
	INFO("\r\nWelcom to Noduino Open Plant!\r\n");
	INFO("Current firmware is user%d.bin\r\n", system_upgrade_userbin_check()+1);
	INFO("%s", noduino_banner);

	pcf8563_init();
	pcf8563_now();

	led_init();
	xkey_init();

	init_yun();

	wifi_set_sleep_type(MODEM_SLEEP_T);
}

void loop()
{
	if (network_state == WIFI_STATION_OK ||
			network_state == WIFI_AP_STATION_OK) {
		push_temp_humi();
		if(realtime == 1)
			delay(mqttrate_sec*1000);
		else
			delay(httprate_min*60*1000);
	}
}
