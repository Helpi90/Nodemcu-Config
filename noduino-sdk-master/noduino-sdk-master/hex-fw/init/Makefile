MCU=attiny13
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-g -mmcu=$(MCU) -Os

APP = init

all: $(APP).hex

$(APP).hex : $(APP).o 
	$(OBJCOPY) -R .eeprom -O ihex $(APP).o $(APP).hex 
$(APP).o : $(APP).c 
	$(CC) $(CFLAGS) -Os -o $(APP).o $(APP).c
clean:
	rm -f *.hex *.o
test:
	sudo modprobe -r ftdi_sio
	sudo avrdude -cft232r1 -pt13 -P ft0 -U hfuse:r:-:h -B 1
flash:
	sudo modprobe -r ftdi_sio
	#sudo avrdude -cft232r1 -pt13 -P ft0 -U flash:w:$(APP).hex -B 1
	# 01 - > 4.8/8 MHz 0x65
	sudo avrdude -cft232r1 -pt13 -P ft0 -U flash:w:$(APP).hex -U lfuse:w:0x65:m -B 1
	# 11 -> 128/8 KHz 0x67, 
	# WARNING: This can make the chip can not be programed next time! 
	#sudo avrdude -cft232r1 -pt13 -P ft0 -U flash:w:$(APP).hex -U lfuse:w:0x67:m -B 1
rc:
	sudo modprobe -r ftdi_sio
	#sudo avrdude -cft232r1 -pt13 -P ft0 -U flash:w:$(APP).hex -B 1
	# 01 - > 4.8/8 MHz 0x65
	sudo avrdude -cft232r1 -pt13 -B 1 -P ft0 -F -V -U lfuse:w:0x65:m 
