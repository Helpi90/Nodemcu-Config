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

/*
 * pos = 0 is closed
 * pos = 100 is opened
 *
 */
static os_timer_t check_pos_timer;

void curtain_open()
{
	INFO("curtain open switch\n");
	digitalWrite(D4, HIGH);
	delayMicroseconds(100000);
	digitalWrite(D4, LOW);
}

void curtain_close()
{
	INFO("curtain close switch\n");
	digitalWrite(D5, HIGH);
	delayMicroseconds(100000);
	digitalWrite(D5, LOW);
}

void curtain_stop()
{
	INFO("curtain stop\n");
	switch (param_get_status())
	{
		case 0:
			//re-enter to stop the close process
			// set the runing state in encoder interrupt!
			curtain_close();
			break;
		case 2:
			// re-enter to stop the open process
			// set the runing state in encoder interrupt!
			curtain_open();
			break;
	}
}

irom void curtain_init()
{
	/* Init the ctrl pin */
	pinMode(D4, OUTPUT);
	pinMode(D5, OUTPUT);

	digitalWrite(D4, LOW);
	digitalWrite(D5, LOW);
}

void check_encoder_pos(void *parm)
{
	int target_pos = *((int *) parm);

	INFO("check encoder pos: target_pos = %d\r\n", target_pos);
	//INFO("param pos = %d\r\n", param_get_position());

	//int epos = encoder_pos();
	INFO("epos = %d\r\n", encoder_pos());

	int epos = param_get_position();
	INFO("param pos = %d\r\n", epos);

	switch (param_get_status())
	{
		case 2:
			/* cw running */
			if (epos >= target_pos) {
				os_timer_disarm(&check_pos_timer);
				INFO("cw, epos >= target_pos, disarm!\r\n");
				curtain_stop();
			}
			break;
		case 0:
			/* ccw running */
			if (epos <= target_pos) {
				os_timer_disarm(&check_pos_timer);
				INFO("ccw, epos <= target_pos, disarm!\r\n");
				curtain_stop();
			}
			break;
		case 1:
			/* stopped */
			INFO("stopped, disarm!\r\n");
			os_timer_disarm(&check_pos_timer);
			break;
	}
}

static int tt_pos;

void curtain_set_status(int status, int pos)
{
	tt_pos = pos;

	int delta;

	// only need to process the stop status
	if(status == 1) {
		INFO("set curtain stop\r\n");
		curtain_stop();
	} else {
		delta = pos - param_get_position();
		if((pos == 100 || delta > 0) && param_get_status() != 2) {
			// open
			curtain_open();
			INFO("set curtain open to %d position\r\n", pos);
			// need to start a timer to check the encoder
			// when meetting the pos, call the curtain_stop()
			delayMicroseconds(500000);
			os_timer_disarm(&check_pos_timer);
			os_timer_setfn(&check_pos_timer, (os_timer_func_t *)check_encoder_pos, &tt_pos);
			os_timer_arm(&check_pos_timer, 20 * encoder_circle(), 1);

		} else if ((pos == 0 || delta < 0) && param_get_status() != 0) {
			// close
			curtain_close();
			INFO("set curtain close to %d position\r\n", pos);
			// need to start a timer to check the encoder
			delayMicroseconds(500000);
			os_timer_disarm(&check_pos_timer);
			os_timer_setfn(&check_pos_timer, (os_timer_func_t *)check_encoder_pos, &tt_pos);
			os_timer_arm(&check_pos_timer, 20 * encoder_circle(), 1);
		}
	}
}

void curtain_set_status_and_publish(int status, int pos)
{
	char msg[32];
	os_memset(msg, 0, 32);

	curtain_set_status(status, pos);
}

void curtain_publish_status()
{
	char msg[32];
	os_memset(msg, 0, 32);

	int status = param_get_status();
	int pos = param_get_position();

	os_sprintf(msg, "{\"status\":%d,\"position\":%d}", status, pos);
	mjyun_publishstatus(msg);
}
