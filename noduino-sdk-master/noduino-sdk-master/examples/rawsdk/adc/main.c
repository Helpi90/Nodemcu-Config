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
#include "user_interface.h"

#include "driver/uart.h"

static volatile os_timer_t adc_timer;

uint16_t get_adc()
{
	return system_adc_read();
}

void *adc_timefunc()
{
	os_printf("AnalogInput = %d\r\n", get_adc());
}

void ICACHE_FLASH_ATTR user_init(void)
{
	//disable the wifi
	wifi_set_opmode(NULL_MODE);

	uart_init(115200, 115200);

	os_printf("\r\nSystem started ...\r\n");

	//Disable the timer
	os_timer_disarm(&adc_timer);

	//Setup timer
	os_timer_setfn(&adc_timer, (os_timer_func_t *) adc_timefunc, NULL);

	//enable the timer
	os_timer_arm(&adc_timer, 1500, 1);
}
