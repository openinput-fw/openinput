MEMORY
{
    rom  (rx)     : ORIGIN = 0x00080000, LENGTH = 0x00040000
    ram  (rw)     : ORIGIN = 0x20000000, LENGTH = 0x0000C000
}

INCLUDE sam3u/common.ld
