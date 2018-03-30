#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "os_type.h"
#include "mem.h"
#include "gpio.h"
#include "math.h"

#include "driver/uart.h"
#include "driver/key.h"
#include "noduino.h"
#include "led.h"
#include "xkey.h"
#include "twi.h"
#include "pcf8563.h"

#include "mjyun.h"
#include "httpclient.h"

#define	DEBUG			    1
#define	MQTT_SEND_RATE_SEC	2
#define	HTTP_SEND_RATE_MIN	3

#define	HTTP_UPLOAD_URL	"http://wx.mjyun.com/tempdata/updata.php?dvid=%s&productid=%s&temp=%s&time=%u"

#ifdef DEBUG
#define INFO( format, ... ) os_printf( format, ## __VA_ARGS__ )
#else
#define INFO( format, ... )
#endif

#endif
