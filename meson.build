project('openinput-cm3', 'c')

sources = [
        'src/main.c'
]

c_flags = [
        '-Os'
]

ld_flags = [
        '--specs=nosys.specs',
        '-nostartfiles',
        '-Wl,--gc-sections',
        '-T' + meson.get_cross_property('linker') + '.ld'
]

if not meson.is_cross_build()
        error('Cross-compiling is required. Please run meson with the \'--cross-file targets/family/my-target-family.txt\' \'--cross-file targets/my-target.txt\' flag')
endif

# libopencm3 special handling

prefix = meson.current_source_dir() / '/usr' / meson.get_cross_property('prefix')
include_dir = prefix / 'include'
lib_dir = prefix / 'lib'

compiler = meson.get_compiler('c', native: true)
libopencm3 = compiler.find_library(meson.get_cross_property('libopencm3_cpu'), dirs: lib_dir)

inc = include_directories(include_dir)

# System binaries

st_flash = find_program('st-flash')
size = find_program('size')
objcopy =  find_program('objcopy')

# Targets

exe = executable(meson.project_name() + '.elf', sources,
        c_args: c_flags,
        link_args: ld_flags,
        dependencies: libopencm3)

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
