MEMORY
{
    rom  (rx)     : ORIGIN = 0x00080000, LENGTH = 0x00020000
    ram  (rw)     : ORIGIN = 0x20000000, LENGTH = 0x00008000
}

INCLUDE sam3u/common.ld
