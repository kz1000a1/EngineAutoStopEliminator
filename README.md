# Engine Auto Stop Eliminator Firmware for SUBARU Levorg VN5

This repository contains sources for the engine start-stop system eliminator firmware for SUBARU Levorg VN5, based off of the [CANable firmware](https://github.com/normaldotcom/canable-fw).

## Safety disclaimer

CAN bus is like a "nervous system" in a car. It is a network that connects various ECUs, sensors, etc. Connecting a new device to this network poses risks such as data corruption, packet losses, etc., that can negatively affect the performance of some or all components of a car.
Same applies to incorrect connections and alternations to the CAN bus wiring. This can cause various undesirable effects, such as "Check engine" lights, electrical and mechanical damage, loss of control, injuries and even death.

By using any code in this project you assume any and all risk, and release any liability from the author.

## Supported Commands

These commands are only accepted in debug mode.
For enable debug mode, you can compile using `make BUILD_MODE=1`.
If you do not connect the USB CDC port to the host computer (SmartPhone's OTG port,Personal Computer ...), the debug mode will not work properly.

- `M0` - Set mode to Engine Auto Stop Eliminator mode (default)
- `M1` - Set mode to Engine Auto Stop Eliminator with Debug Message output to USB CDC port
- `M2` - Disable Engine Auto Stop Eliminator function and dump can bus traffic with candump log formatted output to USB CDC port
- `V` - Returns firmware version and remote path as a string

This firmware currently does not provide any ACK/NACK feedback for serial commands.

## Building

Firmware builds with GCC. Specifically, you will need gcc-arm-none-eabi, which
is packaged for Windows, OS X, and Linux on
[arm Developer](https://developer.arm.com/Tools%20and%20Software/GNU%20Toolchain). Download for your
system and add the `bin` folder to your PATH.

Your Linux distribution may also have a prebuilt package for `arm-none-eabi-gcc`, check your distro's repositories to see if a build exists.

- If you have a CANable device, you can compile using `make`. 
- If you have a CANtact or other device with external oscillator, you can compile using `make EXTERNAL_OSCILLATOR=1`.

## Flashing with the Bootloader

Simply plug in your CANable with the BOOT jumper enabled (or depress the boot button on the CANable Pro while plugging in). Next, type `make flash` and your CANable will be updated to the engine auto stop eliminator firmware for SUBARU Levorg VN5. Unplug/replug the device after moving the boot jumper back, and your CANable will be up and running as engine auto stop eliminator for SUBARU Levorg VN5.

## License

See LICENSE.md
