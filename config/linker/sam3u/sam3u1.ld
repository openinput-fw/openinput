MEMORY
{
    rom  (rx)     : ORIGIN = 0x00080000, LENGTH = 0x00010000
    ram  (rw)     : ORIGIN = 0x20000000, LENGTH = 0x00004000
}

INCLUDE sam3u/common.ld
