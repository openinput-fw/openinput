MEMORY
{
    rom (rx)        : ORIGIN = 0x00400000, LENGTH = 0x00080000
    ram (rw)        : ORIGIN = 0x20400000, LENGTH = 0x00060000
}

INCLUDE samx7x/common.ld
