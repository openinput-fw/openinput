/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

/* version */
#define PARTITION_TABLE_VERSION 0x00

/* magic words */
#define PARTITION_TABLE_MW	 0x7AB1E00
#define PARTITION_TABLE_ENTRY_MW 0x55

/* partition location */
/* 0 = internal memory, 1..etc = application dependant */
#define PARTITION_LOC_INTERNAL 0

/* partition type */
/* partition type: 0 = firmware, 1 = binary blob, 2 = nvs */
#define PARTITION_TYPE_FW   0
#define PARTITION_TYPE_BLOB 1
#define PARTITION_TYPE_NVS  2

struct partition_table_entry_t {
	u8 magic_word;
	u8 location;
	u8 type;
	u8 id;
	u32 start_addr;
	u32 end_addr;
	u8 name[16];
} __attribute__((__packed__));

struct partition_table_t {
	u32 magic_word;
	u8 reserved0;
	u8 reserved1;
	u8 reserved2;
	u8 num_entries;
	struct partition_table_entry_t *entries;
} __attribute__((__packed__));

const struct partition_table_t *partition_table_read(void *addr);
const struct partition_table_entry_t *partition_from_index(const struct partition_table_t *table, uint8_t index);
const struct partition_table_entry_t *partition_from_type(const struct partition_table_t *table, uint8_t type);
const struct partition_table_entry_t *partition_from_id(const struct partition_table_t *table, uint8_t id);
const struct partition_table_entry_t *partition_from_name(const struct partition_table_t *table, char *name, uint8_t name_len);
