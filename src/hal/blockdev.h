/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#pragma once

#include "util/types.h"

struct blockdev_hal_t {
	/* read block from a block device */
	s8 (*read)(struct blockdev_hal_t mem_block, u16 block, u16 offset, void *buffer, u16 size);

	/* program block to a block device */
	s8 (*write)(struct blockdev_hal_t mem_block, u16 block, u16 offset, void *buffer, u16 size);

	/* erase block on a block device */
	s8 (*erase)(struct blockdev_hal_t mem_block, u16 block);

	/* ensure data on storage is in sync with the driver */
	s8 (*sync)(struct blockdev_hal_t mem_block);

	/* minimum size of a block read */
	u16 read_size;

	/* minimum size of a block write */
	u16 write_size;

	/* size of an erasable block */
	u16 block_size;

	/* total number of blocks */
	u16 block_count;

	void *drv_data;
};
