###############################################################################
# Makefile for the project mushLight2
###############################################################################

## General Flags
PROJECT = mushLight2
MCU = atmega328p
TARGET = mushLight2.elf
CC = avr-gcc

CPP = avr-g++

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

## Compile options common for all C compilation units.
CFLAGS = $(COMMON)
CFLAGS += -Wall -gdwarf-2 -std=gnu99 -O0 -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
CFLAGS += -MD -MP -MT $(*F).o -MF dep/$(@F).d 

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS +=  -Wl,-Map=mushLight2.map


## Intel Hex file production flags
HEX_FLASH_FLAGS = -R .eeprom -R .fuse -R .lock -R .signature

HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings


## Objects that must be built in order to link
OBJECTS = mushLight.o pwmPID.o pwmUSART.o pwmpwm.o pwmadc.o 

## Objects explicitly added by the user
LINKONLYOBJECTS = 

## Build
all: $(TARGET) mushLight2.hex mushLight2.eep mushLight2.lss size

## Compile
mushLight.o: ../mushLight.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pwmPID.o: ../pwmPID.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pwmUSART.o: ../pwmUSART.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pwmpwm.o: ../pwmpwm.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

pwmadc.o: ../pwmadc.c
	$(CC) $(INCLUDES) $(CFLAGS) -c  $<

##Link
$(TARGET): $(OBJECTS)
	 $(CC) $(LDFLAGS) $(OBJECTS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) -o $(TARGET)

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@

%.eep: $(TARGET)
	-avr-objcopy $(HEX_EEPROM_FLAGS) -O ihex $< $@ || exit 0

%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: ${TARGET}
	@echo
	@avr-size -C --mcu=${MCU} ${TARGET}

## Clean target
.PHONY: clean
clean:
	-rm -rf $(OBJECTS) mushLight2.elf dep/* mushLight2.hex mushLight2.eep mushLight2.lss mushLight2.map


## Other dependencies
-include $(shell mkdir dep 2>NUL) $(wildcard dep/*)

