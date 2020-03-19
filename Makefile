CC:=arm-none-eabi-gcc
OC:=arm-none-eabi-objcopy

ARCH_FLAGS:=-mthumb -mcpu=cortex-m3
STARTUP_DEFS=-D__STARTUP_CLEAR_BSS -D__START=main
CFLAGS:=$(ARCH_FLAGS) $(STARTUP_DEFS) -Os -flto -ffunction-sections -fdata-sections

USE_NOHOST:=--specs=nosys.specs
GC:=-Wl,--gc-sections
MAP:=-Wl,-Map=main.map
LDSCRIPTS:=-L. -T gcc.ld
LFLAGS:=$(USE_NOHOST) $(LDSCRIPTS) $(GC) $(MAP)

SRCS:=./main.c

clean :
	rm -f *.o *.elf *.bin *.d

compile : 
	${CC} ./startup_ARMCM3.S ${SRCS} -o main.elf $(CFLAGS) $(LFLAGS)
	${OC} -O binary main.elf main.bin

upload :
	stm32flash -w main.bin /dev/ttyUSB0