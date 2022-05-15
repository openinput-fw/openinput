
MEMORY
{
    rom (rx)        : ORIGIN = 0x00400000, LENGTH = 0x00060000
	data(r)			: ORIGIN = 0x00460000, LENGTH = 0x0001E000
	table(r)		: ORIGIN = 0x0047FE00, LENGTH = 0x00000200
    ram (rwx)   	: ORIGIN = 0x20400000, LENGTH = 0x00040000
}

INCLUDE samx7x/common.ld
