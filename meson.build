project('openinput-cm3', 'c', license: 'MIT')

if not meson.is_cross_build()
    error('Cross-compiling is required. Please run meson with the \'--cross-file targets/family/my-target-family.txt\' \'--cross-file targets/my-target.txt\' flag')
endif

# ---------------------------------------
# Target independent sources & flags
# ---------------------------------------

sources = [
    'src/main.c',
    'src/usb_descriptors.c',
    'src/drv/sensor/truemove3/truemove3.c',
    'external/tinyusb/src/tusb.c',
    'external/tinyusb/src/class/hid/hid_device.c',
    'external/tinyusb/src/common/tusb_fifo.c',
    'external/tinyusb/src/device/usbd_control.c',
    'external/tinyusb/src/device/usbd.c'
]

include_dirs = [
    'config/',
    'src',
    'src/target',
    'src/drv/sensor/truemove3',
    'external/tinyusb/src',
    'external/tinyusb/src/class/hid',
    'external/tinyusb/src/common',
    'external/tinyusb/src/device',
    'external/tinyusb/src/osal'
]

c_flags = [
    '-Os',
    '-nostdlib',
    '-nostartfiles',
    '-ffunction-sections',
    '-fdata-sections',
    '-std=gnu99',
    '-Wpointer-arith',
    '-Wundef',
    '-Werror',
    '-D' + meson.get_cross_property('family').to_upper(),
    '-D' + meson.get_cross_property('mcu'),
    '-DTARGET_CONFIG_FILE="' + meson.get_cross_property('target') + '.h"'
]

ld_flags = [
    '-lm',
    '-lc',
    '-lgcc',
    '-lnosys',
    '-nostdlib',
    '-nostartfiles',
    '-fdata-sections',
    '-ffunction-sections',
    '--specs=nano.specs',
    '--specs=nosys.specs',
    '-Wl,--gc-sections',
    '-L../build-targets/ld',
    '-T' + meson.get_cross_property('linker') + '.ld'
]

# ---------------------------------------
# Target dependent sources & flags
# ---------------------------------------

target_family = meson.get_cross_property('family')

if target_family == 'stm32f1'
    include_dirs += [
        'src/drv/mcu/stm32f1',
        'external/cmsis-5/CMSIS/Core/Include',
        'external/cmsis-device-stm32f1/Include',
        'external/tinyusb/src/portable/st/stm32_fsdev'
    ]
    sources += [
        'src/drv/mcu/stm32f1/flash.c',
        'src/drv/mcu/stm32f1/gpio.c',
        'src/drv/mcu/stm32f1/rcc.c',
        'src/drv/mcu/stm32f1/spi.c',
        'src/drv/mcu/stm32f1/systick.c',
        'external/tinyusb/src/portable/st/stm32_fsdev/dcd_stm32_fsdev.c'
    ]
else
    error('This family is not supported.')
endif

target = meson.get_cross_property('target')

if target == 'blue-pill'
    include_dirs += [
        'src/target/stm32f1'
    ]
    sources += [
        'src/target/stm32f1/exc_handler.c',
        'src/target/stm32f1/startup.c',
        'src/target/stm32f1/stm32f1.c'
    ]
elif target == 'rival310'
    include_dirs += [
        'src/target/rival310'
    ]
    sources += [
        'src/target/rival310/exc_handler.c',
        'src/target/rival310/startup.c',
        'src/target/rival310/rival310.c',
        'src/target/rival310/encoder.c',
        'src/target/rival310/leds.c',
        'src/target/rival310/spi_eeprom.c'
    ]
else
    error('This target is not supported.')
endif

include = include_directories(include_dirs)

# ---------------------------------------
# System binaries
# ---------------------------------------

st_flash = find_program('st-flash')
size = find_program('size')
objcopy =  find_program('objcopy')

# ---------------------------------------
# Targets
# ---------------------------------------

exe = executable(meson.project_name() + '.elf', sources,
        c_args: c_flags,
        link_args: ld_flags,
        include_directories: include)

bin = custom_target('bin',
        input: exe,
        output: exe.full_path().split('/')[-1].split('.elf')[0] + '.bin',
        command: [objcopy, '-O', 'binary', '@INPUT@', '@OUTPUT@'],
        build_by_default: true,
        depends: exe)

hex = run_target('hex',
        command: [objcopy, ['-Obinary', exe.full_path(), exe.full_path() + '.hex']],
        depends: exe)

run_target('flash',
        command: [st_flash, 'write', bin.full_path(), '0x08000000'],
        depends: bin)

run_target('size',
        command: [size, exe.full_path(), '-B'],
        depends: exe)
