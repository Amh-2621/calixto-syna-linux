/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __DEWARP_DBG_H__
#define __DEWARP_DBG_H__

#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/debugfs.h>

struct dewarp_ctx;

#define MAX_DEWARP_QUEUE_ITEM 120

extern int dewarp_debug_on;
#define dewarp_print(arg...) \
    do { \
        if (dewarp_debug_on) \
            pr_info(arg); \
    } while (0);

extern int dewarp_dump_debugfs;

struct dewarp_dbg_time {
    ktime_t intrp_time;
    ktime_t push_time;
    ktime_t cb_time;
    uint32_t frame_process_time;
    uint32_t interval_time;
    uint64_t current_avg_time;
};

struct dewarp_dbg {
    struct dentry *debugfs_entry;
    struct dewarp_dbg_time buf_time[MAX_DEWARP_QUEUE_ITEM];
    u32 push_idx;
    u32 cb_idx;
    uint64_t total_process_time;
    uint32_t total_frame_count;
};

void dewarp_dbg_create(struct dewarp_dbg *dbg);
void dewarp_dbg_remove(struct dewarp_dbg *dbg);
void dewarp_dbg_clear(struct dewarp_dbg *dbg);
void dewarp_dbg_buf_push(struct dewarp_dbg *dbg);
void dewarp_dbg_callback(struct dewarp_dbg *dbg);

#endif /* __DEWARP_DBG_H__ */
