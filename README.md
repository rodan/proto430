
 BoosterPack-compatible development boards used for quick prototyping of i2c/spi/uart-based devices

 source:       https://github.com/rodan/proto430
 author:       Petre Rodan <2b4eda@subdimension.ro>
 license:      BSD

 ////// UNDER DEVELOPMENT ///////////////////////

Collection of plug-in Booster-Pack PCBs compatible with the Texas Instruments LaunchPad development ecosystem. one board provides access to any 3v3 device via a serial interface, the other brings access to a graphical VFD display, a rotary encoder and also has infrared remote input. the two will be linked via i2c or spi.

* project directory structure

 ./hardware/display.* - eagle-based schematics and pcbs for the spi-based Noritake VFD 
 ./hardware/sensor.*  - eagle-based schematics and pcbs for the i2c/spi/uart devboard

testing is done with MSP-EXP430FR5994 development kits

