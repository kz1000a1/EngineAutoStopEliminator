# Engine Auto Stop Eliminator Firmware for SUBARU Levorg VN5

This repository contains sources for the engine start-stop system eliminator firmware for SUBARU Levorg VN5, based off of the CANable firwmare.

## Building

Firmware builds with GCC. Specifically, you will need gcc-arm-none-eabi, which
is packaged for Windows, OS X, and Linux on
[arm Developer](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain). Download for your
system and add the `bin` folder to your PATH.

Your Linux distribution may also have a prebuilt package for `arm-none-eabi-gcc`, check your distro's repositories to see if a build exists.

- If you have a CANable device, you can compile using `make`. 
- If you have a CANtact or other device with external oscillator, you can compile using `make INTERNAL_OSCILLATOR=1`

## Flashing with the Bootloader

Simply plug in your CANable with the BOOT jumper enabled (or depress the boot button on the CANable Pro while plugging in). Next, type `make flash` and your CANable will be updated to the engine auto stop eliminator firwmare for SUBARU Levorg VN5. Unplug/replug the device after moving the boot jumper back, and your CANable will be up and running as engine auto stop eliminator for SUBARU Levorg VN5.

## License

See LICENSE.md
