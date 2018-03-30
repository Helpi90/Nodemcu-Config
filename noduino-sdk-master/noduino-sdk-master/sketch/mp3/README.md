# ESP8266 MP3 webradio streaming player 
---
Do not use external components.<br> 
Module ESP-01 + resistor and capacitor:<br>
https://www.youtube.com/watch?v=XIneZEfahmQ<br>

GPIO3 (RXD) - Output PWM/Sigma-Delta Sound<br>
MP3-decodec uses IRAM Buffer > 20 Kbytes<br>

Support all ESP2866 modules (0.5..16 Mbytes flash)<br>

Set connect AP in playerconfig.h:<br> 
define AP_NAME "HOMEAP"<br>
define AP_PASS "01234567890"<br>

The base code is at the [official Espressif Github](https://github.com/espressif/esp8266_mp3_decoder)<br>

[MAD: MPEG Audio Decoder](http://www.underbit.com/products/mad/)<br>
