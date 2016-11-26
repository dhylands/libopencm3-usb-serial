USB Serial example for libopencm3

This started life as [usb_cdcacm](https://github.com/libopencm3/libopencm3-examples/tree/master/examples/stm32/f4/stm32f4-discovery/usb_cdcacm)
but has been modularized a bit.

Reading and writing via the usb serial now uses a circular buffer and has been
made interrupt driven.

### Prerequisites

- arm-none-eabi toolchain. Tested with the 4.9.3 version from [launchpad](https://launchpad.net/gcc-arm-embedded)
- [stlink](https://github.com/texane/stlink)
- python 2.7 (libopencm3's build uses this)

### Build

```
git clone https://github.com/dhylands/libopencm3-usb-serial usb-serial
cd usb-serial
make
```

By default, this builds for the 1BitSy. You can specify a board to build
for the STM32F4 Discovery:
```
make BOARD=STM32F4DISC
```

### Flash (for 1BitSy)

To flash using the BlackMagic Probe:
```
make run
```
This will stop at main. Type 'c' to continue and it will run. You can remove
the `break main` statement from gdbinit if you'd just like it to run all the time.

### Flash (for STM32F4 Discovery board)

To flash using the STM32F4 builtin discovery STLINK flasher:
```
make BOARD=STM32F4DISC stlink
```
