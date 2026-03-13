// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/debugfs.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/ktime.h>
#include <linux/string.h>
#include <linux/mutex.h>

#include "tiler_dbg.h"

int tiler_debug_on;
module_param(tiler_debug_on, int, 0644);

int tiler_dump_debugfs;
module_param(tiler_dump_debugfs, int, 0644);

void tiler_dbg_buf_push(struct tiler_dbg *dbg)
{
	int idx;

	mutex_lock(&dbg->lock);
	idx = dbg->cur_indx;
	dbg->buf_time[idx].push_time = ktime_get();
	mutex_unlock(&dbg->lock);
}

void tiler_dbg_callback(struct tiler_dbg *dbg)
{
	int idx;

	mutex_lock(&dbg->lock);
	idx = dbg->cur_indx;
	dbg->buf_time[idx].callback_arrived = ktime_get();
	dbg->buf_time[idx].frame_process_time = ktime_get() - dbg->buf_time[idx].push_time;
	++dbg->total_frame_count;
	dbg->total_process_time += dbg->buf_time[idx].frame_process_time;
	dbg->buf_time[idx].current_avg_time = (dbg->total_process_time)/dbg->total_frame_count;
	dbg->cur_indx = (idx + 1) % MAX_TILER_QUEUE_ITEM;
	mutex_unlock(&dbg->lock);
}

void tiler_dbg_callback_processed(struct tiler_dbg *dbg)
{
	int idx;

	mutex_lock(&dbg->lock);
	idx = dbg->cur_indx;
	dbg->buf_time[idx].callback_processed = ktime_get() - dbg->buf_time[idx].callback_arrived;
	dbg->cur_indx = (idx + 1) % MAX_TILER_QUEUE_ITEM;
	mutex_unlock(&dbg->lock);
}
void tiler_dbg_bcm_process(struct tiler_dbg *dbg)
{
	int idx;

	mutex_lock(&dbg->lock);
	idx = dbg->cur_indx;
	dbg->buf_time[idx].bcm_process_time = ktime_get() - dbg->buf_time[idx].push_time;
	mutex_unlock(&dbg->lock);
}

void tiler_dbg_clear(struct tiler_dbg *dbg)
{
	mutex_lock(&dbg->lock);
	memset(dbg->buf_time, 0,
			sizeof(struct tiler_dbg_time) * MAX_TILER_QUEUE_ITEM);
	dbg->cur_indx = 0;
	dbg->total_process_time = 0;
	dbg->total_frame_count = 0;
	mutex_unlock(&dbg->lock);
}

static int perf_show(struct seq_file *s, void *data)
{
	struct tiler_dbg *dbg = s->private;
	int i;
	ktime_t cb, bcm_process, callback_process;
	ktime_t avg;

	seq_puts(s, "FPTime (us)\t  CAvgP (us) BP (us) \t CP(\n");
	for (i = 0; i < MAX_TILER_QUEUE_ITEM; i++) {
		mutex_lock(&dbg->lock);
		cb = dbg->buf_time[i].frame_process_time;
		avg = dbg->buf_time[i].current_avg_time;
		bcm_process = dbg->buf_time[i].bcm_process_time;
		callback_process =  dbg->buf_time[i].callback_processed;
		seq_printf(s, "%lld \t %lld \t %lld  \t %lld \n",
			(long long)ktime_to_us(cb), (long long)ktime_to_us(avg),
			(long long)ktime_to_us(bcm_process),
			(long long)ktime_to_us(callback_process));
		mutex_unlock(&dbg->lock);
	}

	return 0;
}

#define tiler_dbg_create_entry(name) \
	debugfs_create_file(#name, S_IRUGO, dbg->debugfs_entry, \
			dbg, &name##_fops)

DEFINE_SHOW_ATTRIBUTE(perf);

void tiler_dbg_create(struct tiler_dbg *dbg)
{
	mutex_init(&dbg->lock);

	dbg->debugfs_entry = debugfs_create_dir("tiler", NULL);
	if (!tiler_dbg_create_entry(perf))
		pr_err("tiler perf create failed\n");
}

void tiler_dbg_remove(struct tiler_dbg *dbg)
{
	debugfs_remove_recursive(dbg->debugfs_entry);
	dbg->debugfs_entry = NULL;
}
