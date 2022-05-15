#!/usr/bin/env python3

# import argparse
import struct


BIN = 'partition_table.bin'
# /* version */
PARTITION_TABLE_VERSION = 0x00

# /* magic words */
PARTITION_TABLE_MW = 0x07AB1E00
PARTITION_TABLE_ENTRY_MW = 0x55

# /* partition location */
# /* 0 = internal memory, 1..etc = application dependant */
PARTITION_LOC_INTERNAL = 0

# /* partition type */
# /* partition type: 0 = firmware, 1 = binary blob, 2 = nvs */
PARTITION_TYPE_FW = 0
PARTITION_TYPE_BLOB = 1
PARTITION_TYPE_NVS = 2


def table_header(numEntries: int = 1) -> bytes:
    # struct partition_table_t {
    # 	u32 magic_word;
    # 	u8 reserved0;
    # 	u8 reserved1;
    # 	u8 reserved2;
    # 	u8 num_entries;
    # 	struct partition_table_entry_t *entries;
    # } __attribute__((__packed__));

    data = struct.pack('<I', PARTITION_TABLE_MW + PARTITION_TABLE_VERSION)
    data += struct.pack('<B', 0x55)
    data += struct.pack('<B', 0xAA)
    data += struct.pack('<B', 0x55)
    data += struct.pack('<B', numEntries)

    return data


def add_entry(
        location: int = PARTITION_LOC_INTERNAL,
        ptype: int = PARTITION_TYPE_BLOB,
        id: int = 0,
        start_addr: int = 0,
        end_addr: int = 0,
        name: str = '') -> bytes:

    # struct partition_table_entry_t {
    # 	u8 magic_word;
    # 	u8 location;
    # 	u8 type;
    # 	u8 id;
    # 	u32 start_addr;
    # 	u32 end_addr;
    # 	u8 name[16];
    # } __attribute__((__packed__));

    data = struct.pack('<B', PARTITION_TABLE_ENTRY_MW)
    data += struct.pack('<B', location)
    data += struct.pack('<B', ptype)
    data += struct.pack('<B', id)
    data += struct.pack('<I', start_addr)
    data += struct.pack('<I', end_addr)

    if(len(name) > 16):
        exit(1)

    for b in bytes(name, 'utf-8'):
        data += struct.pack('<B', b)

    data += bytes(16 - (len(name) % 16))

    return data


if __name__ == '__main__':

    bin = open(BIN, 'wb')

    bin.write(table_header(numEntries=2))  # @ 0x0047FE00
    bin.write(add_entry(name='nv_data', ptype=PARTITION_TYPE_NVS, start_addr=0x00460000, end_addr=0x0047C000))
    bin.write(add_entry(name='sensor_blob', ptype=PARTITION_TYPE_BLOB, start_addr=0x0047C000, end_addr=0x47E000))
