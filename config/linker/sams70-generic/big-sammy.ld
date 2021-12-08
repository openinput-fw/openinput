_partition_table_size = 0x100;
_blob_size = 0x1000;
_rom_size = 0x00080000;

MEMORY
{
    rom (rx)        : ORIGIN = 0x00400000, LENGTH = _rom_size - _partition_table_size - _blob_size
	blob(r)			: ORIGIN = ORIGIN(rom) + LENGTH(rom), LENGTH = _blob_size
	table(r)		: ORIGIN = ORIGIN(blob) + LENGTH(blob), LENGTH = _partition_table_size
    ram (rwx)       : ORIGIN = 0x20400000, LENGTH = 0x00040000
}

INCLUDE samx7x/common.ld
