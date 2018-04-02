/*
 * Filename:          sub
 * Project:           Wemos-D1-ESP8266-PIR-Alarm-master
 * Author:            macbook 
 * -----
 * File Created:      02-04-2018
 * -----
 * Last Modified:     02-04-2018
 * Modified By:       macbook 
 * -----
 */

# -*- coding: utf-8 -*-

import paho.mqtt.client as mqtt


def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    client.subscribe("test/iOS")


def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.payload))


if __name__ == '__main__':
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.connect("10.12.0.244", 1883, keepalive=60)
    client.loop_forever()
