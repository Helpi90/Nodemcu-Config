MCU=attiny13
CC=avr-gcc
OBJCOPY=avr-objcopy
CFLAGS=-g -mmcu=$(MCU) -O1

APP = main

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
flash: all
	sudo modprobe -r ftdi_sio
	#sudo avrdude -cft232r1 -pt13 -P ft0 -U flash:w:$(APP).hex -B 1
	# 01 - > 4.8MHz/8 0x65
	sudo avrdude -cft232r1 -pt13 -P ft0 -U flash:w:$(APP).hex -U lfuse:w:0x65:m -B 1
	# 11 -> 128KHz/8 0x67
	#sudo avrdude -cft232r1 -pt13 -P ft0 -U flash:w:$(APP).hex -U lfuse:w:0x67:m -B 1
