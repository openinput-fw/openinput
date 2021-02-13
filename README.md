# openinput

<img src="docs/assets/logo.svg" alt="" width="20%" align="right">

![check](https://github.com/openinput-fw/openinput/workflows/check/badge.svg)
![build](https://github.com/openinput-fw/openinput/workflows/build/badge.svg)
[![Documentation Status](https://readthedocs.org/projects/openinput/badge/?version=latest)](https://openinput.readthedocs.io/en/latest/?badge=latest)

Open source firmware for input devices.

### Supported Targets
- Linux UHID (mainly for testing/emulation)
- STM32F1 (planned)
  - Blue pill
  - Steelseries Rival 310

### Instructions

```sh
./configure.py TARGET [target options]
ninja
```

The binaries will be located at `build/`.

### Dependencies

- Build system
  - [`ninja_syntax`](https://pypi.org/project/ninja_syntax) (`pip install ninja_syntax`)
  - [`ninja`](https://github.com/ninja-build/ninja) (`pacman -S ninja`)
- Compiler
  - GCC for the target architecture (eg. `pacman -S arm-none-eabi-gcc`)

Currently, official support is only available for Arch Linux. If you are running into
trouble, consider using a docker container with the
[`archlinux` image](https://hub.docker.com/_/archlinux).

### License

The openinput firmware is licensed under the permissive [MIT](LICENSE) license.
