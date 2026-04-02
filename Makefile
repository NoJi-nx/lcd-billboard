MCU = atmega328p
F_CPU = 16000000UL
CC = avr-gcc

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os

TARGET = main

all:
	$(CC) $(CFLAGS) -o $(TARGET).elf $(TARGET).c
	avr-objcopy -O ihex -R .eeprom $(TARGET).elf $(TARGET).hex

clean:
	rm -f *.elf *.hex