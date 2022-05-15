/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2022 Rafael Silva <perigoso@riseup.net>
 */

#include "platform/samx7x/hal/blockdev.h"

s8 eefc_hal_read(struct blockdev_hal_t mem_block, u16 block, u16 offset, void *buffer, u16 size)
{
	struct blockdev_drv_t *drv_data = mem_block.drv_data;
	eefc_read(buffer, (block * mem_block.block_size) + drv_data->start_addr + offset, size);
}

s8 eefc_hal_write(struct blockdev_hal_t mem_block, u16 block, u16 offset, void *buffer, u16 size)
{
	struct blockdev_drv_t *drv_data = mem_block.drv_data;
	eefc_write((block * mem_block.block_size) + drv_data->start_addr + offset, buffer, size);
}

s8 eefc_hal_erase(struct blockdev_hal_t mem_block, u16 block)
{
	struct blockdev_drv_t *drv_data = mem_block.drv_data;
	/* erase in 16 page groups */
	eefc_erase_16((block * mem_block.block_size) + drv_data->start_addr);
}

s8 eefc_hal_sync(struct blockdev_hal_t mem_block)
{
	/* not used */
	(void) mem_block;
}

struct blockdev_hal_t blockdev_hal_init_eefc(struct blockdev_drv_t *drv_data)
{
	const struct flash_info_t *info = eefc_get_info();

	struct blockdev_hal_t hal = {
		.read_size = 1,
		.write_size = 16,
		/* in large sectors we are only allowed to erase in 16 page groups*/
		.block_size = info->page_size * 16,
		.block_count = drv_data->size / (16 * info->page_size),

		.read = eefc_hal_read,
		.write = eefc_hal_write,
		.erase = eefc_hal_erase,
		.sync = eefc_hal_sync,

		.drv_data = drv_data,
	};
	return hal;
}
