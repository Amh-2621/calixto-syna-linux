// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/version.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>
#include <media/videobuf2-v4l2.h>
#include <linux/v4l2-mediabus.h>
#include <media/v4l2-subdev.h>
#include "dewarp_subdev.h"
#include "dewarp_procfs.h"

#define DEWARP_MIN_WIDTH   256
#define DEWARP_MAX_WIDTH   3840
#define DEWARP_MIN_HEIGHT  256
#define DEWARP_MAX_HEIGHT  2160

#define DEWARP_CHECK_RESOLUTION(val, min, max, target, field, label) \
	do { \
		if ((val) < (min)) { \
			target->field = (min); \
		} else if ((val) > (max)) { \
			target->field = (max); \
		} else { \
			target->field = (uint32_t)(val); \
		} \
	} while (0)

struct dewarp_procfs {
	struct proc_dir_entry *pde;
	struct dewarp_subdev_dev *dw_dev;
	struct mutex lock;
};

static int dewarp_procfs_info_show(struct seq_file *sfile, void *offset)
{
	struct dewarp_procfs *dewarp_proc;
	struct dewarp_subdev_dev *dw_dev;

	dewarp_proc = (struct dewarp_procfs *) sfile->private;

	mutex_lock(&dewarp_proc->lock);
	dw_dev = dewarp_proc->dw_dev;

	seq_printf(sfile, "lut       :/lib/firmware/%s\n", dw_dev->lut.lut_file);
	seq_printf(sfile, "width     :%d\n", dw_dev->in_width);
	seq_printf(sfile, "height    :%d\n", dw_dev->in_height);

	mutex_unlock(&dewarp_proc->lock);

	return 0;
}

static int dewarp_procfs_open(struct inode *inode, struct file *file)
{
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0)
	return single_open(file, dewarp_procfs_info_show, PDE_DATA(inode));
#else
	return single_open(file, dewarp_procfs_info_show, pde_data(inode));
#endif
}

static int32_t dewarp_proc_process(struct seq_file *sfile,
		struct dewarp_procfs *dewarp_proc, char *str_buf)
{
	struct dewarp_subdev_dev *dw_dev;
	char *token = NULL, *cur = str_buf;
	char *val = NULL, *kv_cur = NULL;
	char *const delim = " ";
	char *const kv_delim = "=";
	unsigned long width = 0, height = 0;

	dw_dev = dewarp_proc->dw_dev;

	mutex_lock(&dewarp_proc->lock);

	while ((token = strsep(&cur, delim))) {
		kv_cur = token;
		val = strsep(&kv_cur, kv_delim);
		if (val) {
			if (strcmp(val, "lut") == 0) {
				val = strsep(&kv_cur, kv_delim);
				if (val) {
					memset(dw_dev->lut.lut_file, 0,
						sizeof(dw_dev->lut.lut_file));
					strncpy(dw_dev->lut.lut_file, val, strlen(val));
				}
			} else if (strcmp(val, "width") == 0) {
				val = strsep(&kv_cur, kv_delim);
				if (val) {
					if (!kstrtoul(val, 0, &width)) {
						DEWARP_CHECK_RESOLUTION(width, DEWARP_MIN_WIDTH,
						DEWARP_MAX_WIDTH,dw_dev, in_width, "width");
					} else {
						pr_warn("invalid width value: %s\n", val);
					}
				}
			} else if (strcmp(val, "height") == 0) {
				val = strsep(&kv_cur, kv_delim);
				if (val) {
					if (!kstrtoul(val, 0, &height)) {
						DEWARP_CHECK_RESOLUTION(height, DEWARP_MIN_HEIGHT,
						DEWARP_MAX_HEIGHT,dw_dev, in_height, "height");
					} else {
						pr_warn("invalid height value: %s\n", val);
					}
				}
			}
		}
	}
	mutex_unlock(&dewarp_proc->lock);

	return 0;
}

static ssize_t dewarp_procfs_write(struct file *file,
		const char __user *buffer, size_t count, loff_t *ppos)
{
	struct dewarp_procfs *dewarp_proc;
	struct seq_file *sfile;
	char *str_buf;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 17, 0)
	dewarp_proc = (struct dewarp_procfs *) PDE_DATA(file_inode(file));
#else
	dewarp_proc = (struct dewarp_procfs *) pde_data(file_inode(file));
#endif
	sfile = file->private_data;

	str_buf = (char *)kzalloc(count, GFP_KERNEL);
	if (!str_buf)
		return -ENOMEM;

	if (copy_from_user(str_buf, buffer, count))
		return -EFAULT;

	*(str_buf + count - 1) = '\0';

	dewarp_proc_process(sfile, dewarp_proc, str_buf);

	kfree(str_buf);

	return count;
}

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 6, 0)
static const struct file_operations dewarp_procfs_ops = {
	.open = dewarp_procfs_open,
	.release = single_release,
	.read = seq_read,
	.write = dewarp_procfs_write,
	.llseek = seq_lseek,
};
#else
static const struct proc_ops dewarp_procfs_ops = {
	.proc_open = dewarp_procfs_open,
	.proc_release = single_release,
	.proc_read = seq_read,
	.proc_write = dewarp_procfs_write,
	.proc_lseek = seq_lseek,
};
#endif

struct finddir_callback {
	struct dir_context ctx;
	const char *name;
	int32_t files_cnt;
	bool found;
};

#if LINUX_VERSION_CODE < KERNEL_VERSION(6, 1, 0)
static int readdir_callback(struct dir_context *ctx, const char *name,
		int namelen, loff_t offset, u64 ino, unsigned int d_type) {
	struct finddir_callback *fc =
		container_of(ctx, struct finddir_callback, ctx);
	if (fc->found)
		return 0;

	if(strcmp(name, fc->name) == 0) {
		fc->found = true;
	}
	fc->files_cnt++;
	return 0;
}

#else
static bool readdir_callback(struct dir_context *ctx, const char *name,
		int namelen, loff_t offset, u64 ino, unsigned int d_type) {

	struct finddir_callback *fc =
		container_of(ctx, struct finddir_callback, ctx);
	if (fc->found)
		return true;
	if(strcmp(name, fc->name) == 0) {
		fc->found = true;
	}
	fc->files_cnt++;
	return true;
}
#endif

static int find_proc_dir_by_name(const char *root,
		const char *name, bool *found, int32_t *files_cnt) {
	struct file *pfile;
	int ret = 0;
	struct finddir_callback fc = {
		.ctx.actor = readdir_callback,
		.name = name,
		.found = false,
		.files_cnt = -2,
	};

	pfile = filp_open(root, O_RDONLY | O_DIRECTORY, 0);
	if (pfile->f_op->iterate_shared) {
		ret = pfile->f_op->iterate_shared(pfile, &fc.ctx);
#if LINUX_VERSION_CODE <= KERNEL_VERSION(6, 2, 0)
	} else {
		ret = pfile->f_op->iterate(pfile, &fc.ctx);
#endif
	}

	if (ret == 0) {
		*found = fc.found;
	}

	if (files_cnt != NULL) {
		*files_cnt = fc.files_cnt;
	}

	filp_close(pfile, NULL);
	return ret;
}

int dewarp_procfs_register(struct dewarp_subdev_dev *dw_dev, unsigned long *pde)
{
	struct dewarp_procfs *dewarp_proc;
	char dewarp_proc_name[32];
	int ret = 0;
	bool found = false;

	if (!dw_dev)
		return -1;
	sprintf(dewarp_proc_name, "syna-dewarp/dewarp_subdev%d", dw_dev->id);

	dewarp_proc = devm_kzalloc(dw_dev->dev,
			sizeof(struct dewarp_procfs), GFP_KERNEL);

	if (!dewarp_proc)
		return -ENOMEM;

	ret = find_proc_dir_by_name("/proc", "syna-dewarp", &found, NULL);
	if (ret == 0) {
		if (!found)
			proc_mkdir("syna-dewarp", NULL);
	} else {
		return -EFAULT;
	}

	dewarp_proc->dw_dev = dw_dev;
	dewarp_proc->pde = proc_create_data(dewarp_proc_name, 0664, NULL,
			&dewarp_procfs_ops, dewarp_proc);
	if (!dewarp_proc->pde)
		return -EFAULT;
	*pde = (unsigned long)&dewarp_proc->pde;

	mutex_init(&(dewarp_proc->lock));
	return 0;
}

void dewarp_procfs_unregister(unsigned long pde)
{
	int ret = 0;
	bool found = false;
	int32_t files_cnt;
	struct dewarp_procfs *dewarp_proc = (struct dewarp_procfs*)pde;

	ret = find_proc_dir_by_name("/proc", "syna-dewarp", &found, NULL);
	if (ret == 0) {
		if (found) {
			proc_remove(dewarp_proc->pde);
			ret = find_proc_dir_by_name("/proc/syna-dewarp", "", &found, &files_cnt);
			if (files_cnt == 0) {
				remove_proc_subtree("syna-dewarp", NULL);
			}
		}
	}
}
