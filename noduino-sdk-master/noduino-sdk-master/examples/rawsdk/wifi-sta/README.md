Noduino SDK - Hello World
===========================================

This simple project shows how to setup wifi station


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

```bash
$ make flash
```

It use the ```/dev/ttyUSB0``` serial device to upload the firmware into board.

You need to modify the varible ```ESPPORT``` according to your system in
Makefile. It should be /dev/cu.SLAB_USBtoUART in Mac OS X or COM3 in windows.


### Check

Open the serial with 115200, you can get the outputing message


### Clean

```bash
$ make clean
```
