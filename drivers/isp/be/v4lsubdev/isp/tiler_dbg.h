/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __TILER_DBG_H__
#define __TILER_DBG_H__

#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/debugfs.h>

#define MAX_TILER_QUEUE_ITEM 120

struct tiler_dbg_time {
	ktime_t push_time;
	uint32_t frame_process_time;
	uint64_t current_avg_time;
	uint32_t bcm_process_time;
	uint32_t callback_arrived;
	uint32_t callback_processed;
};

struct tiler_dbg {
	struct dentry *debugfs_entry;
	struct tiler_dbg_time buf_time[MAX_TILER_QUEUE_ITEM];
	u32 cur_indx;
	uint64_t total_process_time;
	uint32_t total_frame_count;
	struct mutex lock;
};

void tiler_dbg_create(struct tiler_dbg *dbg);
void tiler_dbg_remove(struct tiler_dbg *dbg);
void tiler_dbg_clear(struct tiler_dbg *dbg);
void tiler_dbg_buf_push(struct tiler_dbg *dbg);
void tiler_dbg_callback(struct tiler_dbg *dbg);
void tiler_dbg_bcm_process(struct tiler_dbg *dbg);
void tiler_dbg_callback_processed(struct tiler_dbg *dbg);

#endif /* __TILER_DBG_H__ */
