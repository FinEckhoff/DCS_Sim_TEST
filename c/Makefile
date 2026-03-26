MCU = attiny25
F_CPU = 8000000UL
CC = avr-gcc
OBJCOPY = avr-objcopy
CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os

TARGET = main

all: $(TARGET).hex

$(TARGET).elf: $(TARGET).c
	$(CC) $(CFLAGS) -o $@ $<

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex $< $@

flash: $(TARGET).hex
	"/usr/bin/avrdude" \
	"/etc/avrdude.conf" \
	-pattiny25 -cstk500v1 -P /dev/ttyACM0 -b19200 \
	-Uflash:w:$(TARGET).hex:i \
	-U lfuse:w:0xE2:m

clean:
	del *.elf *.hex
