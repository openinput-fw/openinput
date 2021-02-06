# openinput

<img src="assets/logo.svg" alt="" width="20%" align="right">

Open source input device firmware.

### Supported Devices
- Blue pill
- Steelseries Rival 310
- Linux UHID (mainly for testing/emulation)

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
