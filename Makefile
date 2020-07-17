CC:=arm-none-eabi-gcc -c
LD:=arm-none-eabi-gcc #arm-none-eabi-ld
OC:=arm-none-eabi-objcopy

# no OS to return to baremetal application
USE_NOHOST:=--specs=nosys.specs

# specify the instruction set to use and the cpu (can not change this)
ARCH_FLAGS:=-mthumb -mcpu=cortex-m3
# optimization for code size, use -O(0,1,2,3) for execution performance
OPTIMIZATION:=-Os
# the final compiler flags
CCFLAGS:=$(ARCH_FLAGS) ${OPTIMIZATION} -I. -fsingle-precision-constant -flto -ffunction-sections -fdata-sections
# I am using only single precission floating point constants

# add this option to LDFLAGS to build the map file along side the executable elf
MAP:=-Map=main.map
# introduce the path to the linker script and the name of the linker script
LDSCRIPTS:=-L. -T gcc.ld
# remove unused function and data sections
GC:=-Wl,--gc-sections
#the final linker flags
LDFLAGS:=$(LDSCRIPTS) $(GC)

# libraries
LIB:=-lm

# figure out all the sources in the project
SOURCES:=${shell find . -name '*.c'}
# and the required objects ot be built
OBJECTS:=$(patsubst %.c,%.o,${SOURCES})

# generate objects from c sources
%.o : %.c
	${CC} $(CCFLAGS) $< -o $@

# generate objects from asm sources
%.o : %.S
	${CC} $(CCFLAGS) $< -o $@

# add startup default defines for the project here
# check startup code, this will enable the startup file to zero the bss section, before starting
STARTUP_DEFS:=-D__STARTUP_CLEAR_BSS

# generate explicitly the startup object elf for this controller and project
startup_ARMCM3.o : startup_ARMCM3.S
	${CC} $(CCFLAGS) $(STARTUP_DEFS) $< -o $@

# generate final elf by linking all the object files
main.elf : startup_ARMCM3.o $(OBJECTS)
	$(LD) $(ARCH_FLAGS) $(LDFLAGS) $(USE_NOHOST) $^ -o $@ $(LIB)

# convert to hex or binary that can be transfered by the corresponding uploader driver
main.bin : main.elf
	${OC} -O binary main.elf main.bin

all : main.bin

# clean project
clean :
	rm -f *.o $(OBJECTS) *.elf *.bin *.d *.map

# upload command to upload the code to the microcontroller
upload :
	stm32flash -w main.bin /dev/ttyUSB0
