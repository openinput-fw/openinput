MEMORY
{
    rom (rx)        : ORIGIN = 0x00400000, LENGTH = 0x00200000
    ram (rwx)       : ORIGIN = 0x00000000, LENGTH = 0x00008000
}

INCLUDE samx7x/common.ld
