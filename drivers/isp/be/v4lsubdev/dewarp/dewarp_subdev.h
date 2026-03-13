/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "vvcam_v4l2_common.h"
#include "ispBE_module_common.h"
#include "dewarp_dbg.h"

#define DEWARP_CHN_MAX 2
#define DEWARP_SUBDEV_PAD_NR 8

struct dw_vb2_buffer {
	struct vvcam_vb2_buffer isp_vb;
	struct list_head list;
};

struct dewarp_mbus_fmt {
	uint32_t code;
};

struct dewarp_pad_data {
	uint32_t sink_detected;
	struct v4l2_mbus_framefmt format;
	struct v4l2_fract frmival_min;
	struct v4l2_fract frmival_max;
	uint32_t num_formats;
	struct dewarp_mbus_fmt *mbus_fmt;
	struct list_head queue;
	spinlock_t qlock;
	uint32_t stream;
	uint32_t dewarp_support;
	uint32_t dewarp_enable;
};

struct dewarp_lut_info {
	char lut_file[128];
};

struct dewarp_subdev_dev {
	phys_addr_t paddr;
	uint32_t regs_size;
	void __iomem *base;
	void __iomem *reset;
	int  id;
	int fe_irq;
	int isp_irq;
	struct device *class_dev;
	int mi_irq;
	struct device *dev;
	struct mutex mlock;
	uint32_t refcnt;
	struct v4l2_subdev sd;
	struct media_pad pads[DEWARP_SUBDEV_PAD_NR];
	struct v4l2_async_notifier notifier;
	struct fwnode_handle *dewarp_ep[DEWARP_SUBDEV_PAD_NR];
	struct dewarp_pad_data pad_data[DEWARP_SUBDEV_PAD_NR];
	struct dewarp_lut_info lut;
	struct ISPBE_DEWARP_LUT_CFG lut_cfg;

	struct mutex state_lock;
	int state;
	int is_running;
	struct list_head job_queue;
	uint32_t ctrl_pad;

	unsigned long pde;

	struct v4l2_format format;
	uint32_t in_height;
	uint32_t in_width;
	struct DwpCtx *pDwpCtx;
	struct dewarp_dbg dbg;
	//struct work_struct dewarp_work;
	struct task_struct *dewarp_task;
	wait_queue_head_t dewarp_wq;
	struct vvcam_vb2_buffer *processed_buf;
	uint8_t dw_process_done;

};
