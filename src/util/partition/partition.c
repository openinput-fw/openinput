/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Filipe Laíns <lains@riseup.net>
 */

#include <string.h>

#include "util/data.h"
#include "util/partition/partition.h"

const struct partition_table_t *partition_table_read(void *addr)
{
	const struct partition_table_t *table = (const struct partition_table_t *) addr;

	if (table->magic_word != PARTITION_TABLE_MW | PARTITION_TABLE_VERSION)
		return NULL;

	if (table->num_entries == 0)
		return NULL;

	for (size_t i = 0; i < table->num_entries; i++) {
		if (table->entries[i].magic_word != PARTITION_TABLE_ENTRY_MW)
			return NULL;
	}

	return table;
}

const struct partition_table_entry_t *partition_from_index(const struct partition_table_t *table, uint8_t index)
{
	if (!table)
		return NULL;

	if (index >= table->num_entries)
		return NULL;

	if (table->entries[index].magic_word != PARTITION_TABLE_ENTRY_MW)
		return NULL;

	return &table->entries[index];
}

const struct partition_table_entry_t *partition_from_type(const struct partition_table_t *table, uint8_t type)
{
	if (!table)
		return NULL;

	for (size_t i = 0; i < table->num_entries; i++) {
		if (table->entries[i].type == type)
			return &table->entries[i];
	}

	return NULL;
}

const struct partition_table_entry_t *partition_from_id(const struct partition_table_t *table, uint8_t id)
{
	if (!table)
		return NULL;

	for (size_t i = 0; i < table->num_entries; i++) {
		if (table->entries[i].id == id)
			return &table->entries[i];
	}

	return NULL;
}

const struct partition_table_entry_t *partition_from_name(const struct partition_table_t *table, char *name, uint8_t name_len)
{
	if (!table)
		return NULL;

	char name_buf[16] = {};

	memcpy(name_buf, name, min(name_len, 16));

	for (size_t i = 0; i < table->num_entries; i++) {
		if (strncmp(table->entries[i].name, name_buf, 16) == 0)
			return &table->entries[i];
	}

	return NULL;
}
