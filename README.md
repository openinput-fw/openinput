# openinput-cm3

Open source mouse/keyboard firmware.

```
.
├── tools	# Utilities
├── build-targets	# Meson cross-compile files
├── src		# Source files
│   ├── drv  # Drivers, for mcus and other devices like sensors
│   │    ├── mcu  # Drivers for family of mcu
│   │    ├── sensor  # Drivers for optical mouse sensors
│   │    └── ...  # Other driver classes
│   └── target	# Target abstraction
│         ├── rival310 # Target specific to rival310
│         └── stm32f1 # Generic stm32f1 family target
└── external  # Sources from external projects like tinyusb
```

### Supported MCU Families
- STM32F1 family

### Supported Devices
- Bluepill
- Steelseries Rival 310

### Instructions

```sh
mkdir build && cd build
meson .. --cross-file ../targets/family/stm32f1.txt --cross-file ../targets/blue-pill.txt
ninja
```

The binaries will be located at `build/openinput-cm3.bin` and `build/openinput-cm3.elf`.

The `target` option selects the linker script to be used (usually located in
`/usr/arm-none-eabi/lib/`).


If you want to flash them using the `st-flash` utility from the [stlink](https://github.com/texane/stlink)
project you can simply run `ninja` with the `flash` target.

```
ninja flash
```

If you want to analyze the size of the complied elf file you can run `ninja size`.

### Dependencies

This project requires libopencm3 to be installed system-wide. I will only be able
to provide techincal support for Archlinux.

  - Build system: [meson](https://www.archlinux.org/packages/extra/any/meson/)
  - Compiler: [arm-none-eabi-gcc](https://www.archlinux.org/packages/community/x86_64/arm-none-eabi-gcc/)
  - libopencm3: [libopencm3](https://www.archlinux.org/packages/community/x86_64/libopencm3/)
  - Flash (optional): [stlink](https://www.archlinux.org/packages/community/x86_64/stlink/)
