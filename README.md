# Flight controller using STM32F103
[Documentation and demonstration video link](https://youtu.be/UyUF_jVTML4)

### setup requirements
(for programming stm32f103 microcontroller)

* Linux debian (to use make build system and code upload tool stm32flash)
* sublime/vi/vim/gedit any texteditor
* ARM cross compiler toolchain
  * sudo apt-get install gcc-arm-none-eabi binutils-arm-none-eabi
* stm32flash to flash the controller, assuming the controller has default factory bootloader
  * sudo apt-get install stm32flash
* stm32f103 microcontroller, with default factory bootloader (must)
* usb to uart converter
  * PL2303HX, CP2102, ftdi etc

### sensors
 * mpu6050  sccelerometer and gyroscope
 * hmc5883l magnetometer
 * ms5611 barometer

### additional accessories
 * rc transmitter and receiver

#### [STM32F10x Reference Manual](https://www.st.com/resource/en/reference_manual/cd00171190-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-arm-based-32-bit-mcus-stmicroelectronics.pdf)

### connections (from microcontroller)
 * I2c1 {PB6, PB7} -> GY86 sensor {SCL, SDA}
 * UART1 {PA10, PA9} -> UART debug port {RX, TX}
 * {PB10, PB11, PB12, PB13, PB14, PB15} -> RC receiver
 * {PA0, PA1, PA2, PA3} -> PWM to ESCs of corresponding motors (Left Front, Right Front, Left Back, Right Back) of Quadcopter
 
### PCB designs
 * You will find the .sch and .brd, QuadF32 flight controller board designs in the Eagle_CAD folder.
 * It would also include a generated .cam file that I used for building my board, I would request you to use a different cam file provided by your pcb manufacturer.
 * The folder also does include .lbr library for the stm32f103 module, that I am using.

### Third party acknowledgements
 * [STM32F103-c8t6 eagle cad library](https://os.mbed.com/users/hudakz/code/STM32F103C8T6_Hello/file/accb2c83a007/mbed-STM32F103C8T6.lib/)
