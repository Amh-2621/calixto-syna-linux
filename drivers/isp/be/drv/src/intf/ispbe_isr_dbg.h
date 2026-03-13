/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __ISPBE_ISR_DBG_H__
#define __ISPBE_ISR_DBG_H__

#include <linux/types.h>
#include <linux/ktime.h>
#include <linux/debugfs.h>
#include <linux/mutex.h>
#include "ispbe_api.h"

#define MAX_ISR_QUEUE_ITEM 120

/* ISR interrupt types */
enum ispbe_isr_type {
    ISPBE_ISR_TILER = 0,
    ISPBE_ISR_DEWARP,
    ISPBE_ISR_MAX
};

struct ispbe_isr_entry {
    u32 interrupt_num;          /* Interrupt number */
    u32 interrupt_interval;     /* Time between previous and current interrupt*/
    u32 processing_time;        /* Time to process one interrupt */
    u64 entry_time;
    u64 exit_time;
};

struct ispbe_isr_stats {
    struct ispbe_isr_entry entries[MAX_ISR_QUEUE_ITEM];
    u32 cur_index;
    u64 total_interrupts;
    u64 total_interval_time;     /* For calculating avg interrupt interval */
    u64 total_processing_time;   /* For calculating avg processing time */
    u32 avg_interrupt_interval;  /* Average interrupt interval */
    u32 avg_processing_time;     /* Average processing time */
    ktime_t last_interrupt_time; /* To calculate interval */
    ktime_t current_start_time;  /* To calculate processing time */
    struct mutex lock;
};

struct ispbe_isr_dbg {
    struct dentry *debugfs_root;
    struct dentry *debugfs_entries[2]; /* for dewarp and tiler */
    struct ispbe_isr_stats stats[ISPBE_ISR_MAX];
};

/* Function prototypes */
void ispbe_isr_dbg_create(struct ispbe_isr_dbg *dbg);
void ispbe_isr_dbg_remove(struct ispbe_isr_dbg *dbg);
void ispbe_isr_dbg_start(struct ispbe_isr_dbg *dbg, enum ispbe_isr_type type, u32 int_num);
void ispbe_isr_dbg_end(struct ispbe_isr_dbg *dbg, enum ispbe_isr_type type);
void ispbe_isr_dbg_clear(struct ispbe_isr_dbg *dbg, enum ispbe_isr_type type);

#endif /* __ISPBE_ISR_DBG_H__ */
