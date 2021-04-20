MEMORY
{
    rom  (rx)       : ORIGIN = 0x00000000, LENGTH = 0x080000
    rom_boot  (rx)  : ORIGIN = 0x0FE10000, LENGTH = 0x008000
    ram  (rw)       : ORIGIN = 0x20000000, LENGTH = 0x030000
}

INCLUDE efm32gg12b-generic/common.ld
