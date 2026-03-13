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

#include "dewarp_dbg.h"

int dewarp_debug_on;
module_param(dewarp_debug_on, int, 0644);

int dewarp_dump_debugfs;
module_param(dewarp_dump_debugfs, int, 0644);

void dewarp_dbg_buf_push(struct dewarp_dbg *dbg)
{
    int idx = dbg->push_idx;
    int prev_cb_idx = (dbg->cb_idx + MAX_DEWARP_QUEUE_ITEM - 1) % MAX_DEWARP_QUEUE_ITEM;
    ktime_t now = ktime_get();
    dbg->buf_time[idx].push_time = now;

    if (dbg->buf_time[prev_cb_idx].cb_time) {
        dbg->buf_time[prev_cb_idx].interval_time = now - dbg->buf_time[prev_cb_idx].cb_time;
    }

    dbg->push_idx = (idx + 1) % MAX_DEWARP_QUEUE_ITEM;
}

void dewarp_dbg_callback(struct dewarp_dbg *dbg)
{
    int idx = dbg->cb_idx;
    ktime_t now = ktime_get();
    dbg->buf_time[idx].cb_time = now;

    // Calculate process time from push to callback
    dbg->buf_time[idx].frame_process_time = now - dbg->buf_time[idx].push_time;

    ++dbg->total_frame_count;
    dbg->total_process_time += dbg->buf_time[idx].frame_process_time;
    dbg->buf_time[idx].current_avg_time =
        dbg->total_process_time / dbg->total_frame_count;

    dbg->cb_idx = (idx + 1) % MAX_DEWARP_QUEUE_ITEM;
}

void dewarp_dbg_clear(struct dewarp_dbg *dbg)
{
    memset(dbg->buf_time, 0,
           sizeof(struct dewarp_dbg_time) * MAX_DEWARP_QUEUE_ITEM);
    dbg->push_idx = 0;
    dbg->cb_idx = 0;
    dbg->total_process_time = 0;
    dbg->total_frame_count = 0;
}

static int perf_show(struct seq_file *s, void *data)
{
    struct dewarp_dbg *dbg = s->private;
    int i;
    seq_puts(s, "Frame Process Time (us)\tInterval Time (us)\tCurrent Avg Process (us)\n");
    for (i = 0; i < MAX_DEWARP_QUEUE_ITEM; i++) {
        ktime_t proc = dbg->buf_time[i].frame_process_time;
        ktime_t interval = dbg->buf_time[i].interval_time;
        ktime_t avg = dbg->buf_time[i].current_avg_time;
        seq_printf(s, "%lld\t%lld\t%lld\n",
                (long long)ktime_to_us(proc),
                (long long)ktime_to_us(interval),
                (long long)ktime_to_us(avg));
    }
    return 0;
}

#define dewarp_dbg_create_entry(name) \
    debugfs_create_file(#name, S_IRUGO, dbg->debugfs_entry, \
                       dbg, &name##_fops)

DEFINE_SHOW_ATTRIBUTE(perf);

void dewarp_dbg_create(struct dewarp_dbg *dbg)
{
    dbg->debugfs_entry = debugfs_create_dir("dewarp", NULL);
    if (!dewarp_dbg_create_entry(perf))
        pr_err("dewarp perf create failed\n");
}

void dewarp_dbg_remove(struct dewarp_dbg *dbg)
{
    debugfs_remove_recursive(dbg->debugfs_entry);
    dbg->debugfs_entry = NULL;
}
