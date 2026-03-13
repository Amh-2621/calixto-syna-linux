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
#include <linux/seq_file.h>

#include "ispbe_isr_dbg.h"

int ispbe_isr_debug_on;
module_param(ispbe_isr_debug_on, int, 0644);
MODULE_PARM_DESC(ispbe_isr_debug_on, "Enable ISPBE ISR debugging");

void ispbe_isr_dbg_start(struct ispbe_isr_dbg *dbg, enum ispbe_isr_type type, u32 intr_num)
{
    struct ispbe_isr_stats *stats;
    struct ispbe_isr_entry *entry;
    ktime_t now;
    u32 idx;

    if (!dbg || type >= ISPBE_ISR_MAX || !ispbe_isr_debug_on)
        return;

    stats = &dbg->stats[type];
    now = ktime_get();

    mutex_lock(&stats->lock);

    idx = stats->cur_index;
    entry = &stats->entries[idx];

    /* Calculate interrupt interval */
    if (stats->last_interrupt_time != 0) {
        entry->interrupt_interval = ktime_to_us(now - stats->last_interrupt_time);
        stats->total_interval_time += entry->interrupt_interval;
    } else {
        entry->interrupt_interval = 0; /* First interrupt */
    }

    /* Store current time for next interval calculation and processing time */
    entry->entry_time = ktime_to_us(now);
    stats->last_interrupt_time = now;
    stats->current_start_time = now;
    entry->interrupt_num = intr_num;
    entry->processing_time = 0; /* Will be set in _end() */

    mutex_unlock(&stats->lock);
}

void ispbe_isr_dbg_end(struct ispbe_isr_dbg *dbg, enum ispbe_isr_type type)
{
    struct ispbe_isr_stats *stats;
    struct ispbe_isr_entry *entry;
    ktime_t now;
    u32 idx;

    if (!dbg || type >= ISPBE_ISR_MAX || !ispbe_isr_debug_on)
        return;

    stats = &dbg->stats[type];
    now = ktime_get();

    mutex_lock(&stats->lock);

    idx = stats->cur_index;
    entry = &stats->entries[idx];

    /* Calculate processing time */
    if (stats->current_start_time != 0) {
        entry->exit_time = ktime_to_us(now);
        entry->processing_time = ktime_to_us(now - stats->current_start_time);
        stats->total_processing_time += entry->processing_time;
    }

    stats->total_interrupts++;

    /* Calculate averages */
    if (stats->total_interrupts > 0) {
        stats->avg_interrupt_interval = stats->total_interval_time / stats->total_interrupts;
    }
    stats->avg_processing_time = stats->total_processing_time / stats->total_interrupts;

    stats->cur_index = (idx + 1) % MAX_ISR_QUEUE_ITEM;

    mutex_unlock(&stats->lock);
}

void ispbe_isr_dbg_clear(struct ispbe_isr_dbg *dbg, enum ispbe_isr_type type)
{
    struct ispbe_isr_stats *stats;

    if (!dbg || type >= ISPBE_ISR_MAX)
        return;

    stats = &dbg->stats[type];

    mutex_lock(&stats->lock);
    memset(stats->entries, 0, sizeof(stats->entries));
    stats->cur_index = 0;
    stats->total_interrupts = 0;
    stats->total_interval_time = 0;
    stats->total_processing_time = 0;
    stats->avg_interrupt_interval = 0;
    stats->avg_processing_time = 0;
    stats->last_interrupt_time = 0;
    stats->current_start_time = 0;
    mutex_unlock(&stats->lock);
}

/* Show function for tiler stats - displays timing information */
static int tiler_stats_show(struct seq_file *m, void *v)
{
    struct ispbe_isr_dbg *dbg = (struct ispbe_isr_dbg *)m->private;
    struct ispbe_isr_stats *stats = &dbg->stats[ISPBE_ISR_TILER];
    int i;

    if (!ispbe_isr_debug_on)
        return 0;

    mutex_lock(&stats->lock);
    seq_printf(m, "%-8s %-10s %-12s %-10s %-12s\n",
               "intNum", "intDur", "avgIntDur", "ProcDur", "AvgProcDur");
    seq_printf(m, "%-8s %-10s %-12s %-10s %-12s\n",
               "------", "------", "---------", "-------", "----------");

    for (i = 0; i < MAX_ISR_QUEUE_ITEM && i < stats->total_interrupts; i++) {
        int idx = (stats->cur_index - 1 - i + MAX_ISR_QUEUE_ITEM) % MAX_ISR_QUEUE_ITEM;
        seq_printf(m, "%-8u %-10u %-12u %-10u %-12u\n",
                   stats->entries[idx].interrupt_num,
                   stats->entries[idx].interrupt_interval,
                   stats->avg_interrupt_interval,
                   stats->entries[idx].processing_time,
                   stats->avg_processing_time);
    }
    mutex_unlock(&stats->lock);
    return 0;
}

/* Show function for dewarp stats - displays timing information */
static int dewarp_stats_show(struct seq_file *m, void *v)
{
    struct ispbe_isr_dbg *dbg = (struct ispbe_isr_dbg *)m->private;
    struct ispbe_isr_stats *stats = &dbg->stats[ISPBE_ISR_DEWARP];
    int i;

    if (!ispbe_isr_debug_on)
        return 0;

    mutex_lock(&stats->lock);
    seq_printf(m, "%-8s %-10s %-12s %-10s %-12s\n",
               "intNum", "intDur", "avgIntDur", "ProcDur", "AvgProcDur");
    seq_printf(m, "%-8s %-10s %-12s %-10s %-12s\n",
               "------", "------", "---------", "-------", "----------");

    for (i = 0; i < MAX_ISR_QUEUE_ITEM && i < stats->total_interrupts; i++) {
        int idx = (stats->cur_index - 1 - i + MAX_ISR_QUEUE_ITEM) % MAX_ISR_QUEUE_ITEM;
        seq_printf(m, "%-8u %-10u %-12u %-10u %-12u\n",
                   stats->entries[idx].interrupt_num,
                   stats->entries[idx].interrupt_interval,
                   stats->avg_interrupt_interval,
                   stats->entries[idx].processing_time,
                   stats->avg_processing_time);
    }
    mutex_unlock(&stats->lock);
    return 0;
}

DEFINE_SHOW_ATTRIBUTE(tiler_stats);
DEFINE_SHOW_ATTRIBUTE(dewarp_stats);

void ispbe_isr_dbg_create(struct ispbe_isr_dbg *dbg)
{
    int i;

    /* Initialize stats for both tiler and dewarp */
    for (i = 0; i < ISPBE_ISR_MAX; i++) {
        mutex_init(&dbg->stats[i].lock);
        dbg->stats[i].cur_index = 0;
        dbg->stats[i].total_interrupts = 0;
        dbg->stats[i].total_interval_time = 0;
        dbg->stats[i].total_processing_time = 0;
        dbg->stats[i].avg_interrupt_interval = 0;
        dbg->stats[i].avg_processing_time = 0;
        dbg->stats[i].last_interrupt_time = 0;
    }

    /* Create debugfs root directory */
    dbg->debugfs_root = debugfs_create_dir("ispbe_isr", NULL);
    if (!dbg->debugfs_root) {
        pr_err("Failed to create ispbe_isr debugfs directory\n");
        return;
    }

    /* Create tiler debugfs entry*/
    dbg->debugfs_entries[ISPBE_ISR_TILER] = debugfs_create_file("tiler", 0444,
                                                               dbg->debugfs_root,
                                                               dbg, &tiler_stats_fops);
    if (!dbg->debugfs_entries[ISPBE_ISR_TILER])
        pr_err("Failed to create tiler debugfs entry\n");

    /* Create dewarp debugfs entry*/
    dbg->debugfs_entries[ISPBE_ISR_DEWARP] = debugfs_create_file("dewarp", 0444,
                                                                dbg->debugfs_root,
                                                                dbg, &dewarp_stats_fops);
    if (!dbg->debugfs_entries[ISPBE_ISR_DEWARP])
        pr_err("Failed to create dewarp debugfs entry\n");

}

void ispbe_isr_dbg_remove(struct ispbe_isr_dbg *dbg)
{
    if (dbg->debugfs_root) {
        debugfs_remove_recursive(dbg->debugfs_root);
        dbg->debugfs_root = NULL;
    }
}
