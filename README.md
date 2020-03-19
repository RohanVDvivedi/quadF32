# STM32F103 template bare metal programming

This is a sample project for bare metal programming STM32F103 blue pill board

### setup requirements

* Linux debian
* sublime/vi/vim/gedit any
* ARM cross compiler toolchain
  * sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi
* stm32flash to flash the controller, considering the controller has default factory bootloader
  * sudo apt-get install stm32flash
* stm32f103 microcontroller, with default factory bootloader (must)
* usb to uart converter
  * PL2303HX, CP2102, ftdi etc
* linker script and the startup code(with flash and ram addresses modified)
  * The project uses them directly copied from the arm crosscompiler toolchain samples and examples
  * it is tedious and generally unproductive to write them, but a very healthy mental excercise for people not used to them like myself.

#### [STM32F10x Reference Manual](https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)

I do not recommend baremetal programming,
It is just me exploring brainfuck and masochism.
