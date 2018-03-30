Open Plug Reference Firmware
===========================================

### Overview

![Open Plug](doc/open-plug-1.jpg)


![Open Plug](doc/open-plug-2.jpg)


![Open Plug](doc/open-plug-layout.png)


### Pin Resource

Open Plug used resource:

1. GPIO12 control the relay
2. GPIO0 related to the key (pull up, pressed is low)
3. GPIO3 (U0RX) control the blue LED
4. Red LED show the state of the relay (no related gpio)


### Build

```bash
$ make
```

Or make output more message:

```bash
$ make V=1
```

The target file is at build/ directory.


### Upload

Using a FT232RL USB to Serial Board, and connect:

* FT232_RXD -----> Plug_TX
* FT232_TXD -----> Plug_RX
* FT232_GND -----> Plug_GND
* FT232_GND ------> Plug_GPIO0

Then connect the 3.3V to power up the board:

* FT232_VCC3.3 ---> Plug_3.3V (Power supply must be 3.3V!)


```bash
$ make produce ESPPORT=/dev/ttyUSB0
```

It use the ```/dev/ttyUSB0``` serial device to upload the firmware into board.

You need to modify the varible ```ESPPORT``` according to your system in
Makefile. It should be /dev/cu.SLAB_USBtoUART in Mac OS X or COM3 in windows.


### Erase Flash

Enter flush mode and execute:

```bash
$ make erase
```

### Dump Flash

Enter flush mode and execute:

```bash
$ make dump
```

The flash.bin is the whole flash content


### Re-Init RF

Enter flush mode and execute:

```bash
$ make rfinit
```

It's override the rfinit data in flash using the esp_init_data_default.bin
in sdk1.4


### Show the memory usage

After building the firmware:

```bash
$ make mem
```


### Clean the system parameter

If you want to clean the ssid and password stored in flash by SDK:

```bash
$ make sysclean
```


### Clean

```bash
$ make clean
```
