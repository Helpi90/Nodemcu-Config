MJYUN Quick Start
===================


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

Using following command to upload the firmware into hardware board:

```bash
$ make produce ESPPORT=COM6

or:

$ make produce ESPPORT=/dev/ttyUSB2
...

```

It use the ```/dev/ttyUSB0``` serial device to upload the firmware into board.

You need to modify the varible ```CONFIG_ESPPORT``` in according to your system in
.config placed in current directory. It should be /dev/cu.SLAB_USBtoUART in
Mac OS X or COM3 in windows.


### Try Controlling

Using the WeChat to Scan the following QRcode:


### Clean

```bash
$ make clean
```
