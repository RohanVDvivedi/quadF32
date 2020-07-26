# Flight controller using STM32F103

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
 * {PA0, PA1, PA2, PA3, PA4, PA5} -> RC receiver
 * {PA0, PA1, PA2, PA3} -> PWM to ESCs of corresponding motors (Left Front, Right Front, Left Back, Right Back) of Quadcopter
