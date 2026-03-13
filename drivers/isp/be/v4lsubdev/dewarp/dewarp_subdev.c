// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/of_reserved_mem.h>
#include <linux/of_graph.h>
#include <linux/vmalloc.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/pm_runtime.h>
#include <uapi/linux/sched/types.h>
#include <linux/scs.h>
#include <media/v4l2-device.h>
#include <media/v4l2-event.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-ioctl.h>
#include <media/v4l2-mc.h>
#include <media/videobuf2-dma-contig.h>
#include <media/v4l2-fwnode.h>
#include <media/v4l2-mediabus.h>
#include <media/v4l2-ctrls.h>

#include "vvcam_v4l2_common.h"
#include "vvcam_isp_driver.h"
#include "ispSS_shm.h"
#include "dewarp_drv_api.h"
#include "dewarp_subdev.h"
#include "dewarp_procfs.h"
#include "dewarp_sysfs.h"

//#define NO_DEWARP
#define DEWARP_NAME "dewarp-subdev"
#define GET_CONNECTED_SINK_PAD(pad, flags) ((flags == MEDIA_PAD_FL_SINK) ? (pad) : (pad - 1))
#define GET_CONNECTED_SOURCE_PAD(pad, flags) ((flags == MEDIA_PAD_FL_SOURCE) ? (pad) : (pad + 1))
#define DEWARP_DEFAULT_LUT "isp/DEWARP_LUT.isp"
#define DEWARP_ISR_THREAD_PRIORITY 30
#define DEWARP_DEFAULT_WIDTH 3840
#define DEWARP_DEFAULT_HEIGHT 2160

#define READ_PARAM(devnode, prop, member)                           \
    do {                                                            \
        uint32_t __val;                                             \
        if (!of_property_read_u32(devnode, prop, &__val)) {         \
            member = __val;                                         \
        }                                                           \
    } while (0)

#define DEWARP_PAD_SP1 3
enum DEWARP_HW_STATE {
	HW_FREE,
	HW_BUSY
};

struct dewarp_data {
	struct dewarp_subdev_dev *dw_dev;
	struct vvcam_vb2_buffer *buf;
};

static int dewarp_subdev_querycap(struct v4l2_subdev *sd, void *arg)
{
	struct v4l2_capability *cap = (struct v4l2_capability *)arg;

	strscpy(cap->driver, sd->name, sizeof(cap->driver));
	strscpy(cap->card, sd->name, sizeof(cap->card));
	snprintf(cap->bus_info, sizeof(cap->bus_info),
			"platform:%s", sd->name);

	return 0;
}

static void dewarp_process_bufs(struct DwpCtx *pDwpCtx, struct vvcam_vb2_buffer *buf,
		struct dewarp_subdev_dev *dw_dev)
{
	int ret = 0;
	struct dewarp_api_req *req = NULL;
	struct dewarp_data *data = NULL;
	struct v4l2_pix_format_mplane *pix_mp = &dw_dev->format.fmt.pix_mp;

	req = kzalloc(sizeof(*req), GFP_KERNEL);
	if (!req) {
		ret = -ENOMEM;
		pr_err("fails memory for dewarp request\n");
		return;
	}
	req->src = kzalloc(sizeof(struct frame_info), GFP_KERNEL);
	req->dst = kzalloc(sizeof(struct frame_info), GFP_KERNEL);

	/* SP1 path will always use contiguous memory, calculate the UV addr manually */
	req->src->paddr[0] = (void *)((unsigned long)buf->planes[0].dma_addr);
	req->src->paddr[1] = (void *)((unsigned long)(buf->planes[0].dma_addr +
			pix_mp->plane_fmt[0].bytesperline * pix_mp->height));

	req->dst->paddr[0] = (void *)((unsigned long)buf->planes[0].phy_addr);
	req->dst->paddr[1] = (void *)((unsigned long)(buf->planes[0].phy_addr +
			pix_mp->plane_fmt[0].bytesperline * pix_mp->height));

	data = kzalloc(sizeof(*data), GFP_KERNEL);
	data->buf = buf;
	data->dw_dev = dw_dev;
	req->pUserData = (void *)data;

	memcpy(&req->src->format, &dw_dev->format, sizeof(struct v4l2_format));
	memcpy(&req->dst->format, &dw_dev->format, sizeof(struct v4l2_format));

	dewarp_dbg_buf_push(&dw_dev->dbg);

	ret = dewarp_api_pushBuf(pDwpCtx, req);

	if (ret) {
		pr_err("failed to push dewarp request\n");
		goto dewarp_error;
	}
	return;

dewarp_error:
	kfree(data);
	kfree(req->src);
	kfree(req->dst);
	kfree(req);
}

static void dewarp_try_run(struct dewarp_subdev_dev *dw_dev)
{
	struct dw_vb2_buffer *dw_buf;

	if (dw_dev->state == HW_FREE) {
		if (!list_empty(&dw_dev->job_queue)) {
			dw_buf = list_first_entry(&dw_dev->job_queue, struct dw_vb2_buffer, list);
			dw_dev->state = HW_BUSY;
			dewarp_process_bufs(dw_dev->pDwpCtx, &dw_buf->isp_vb, dw_dev);
		}
	}
}

static int process_dewarp(void *arg)
{
	struct dewarp_subdev_dev *dw_dev = (struct dewarp_subdev_dev *)arg;
	struct dw_vb2_buffer *dw_buf = NULL;

	while (!kthread_should_stop()) {
		wait_event_interruptible_timeout(dw_dev->dewarp_wq,
				dw_dev->dw_process_done > 0,
				msecs_to_jiffies(10000));

		mutex_lock(&dw_dev->state_lock);
		if (dw_dev->dw_process_done > 0 && dw_dev->is_running ) {
			pr_debug("%s:v4l2 buf: 0x%lx Yval: 0X%lX seq: %d\n", __func__,
					(unsigned long)dw_dev->processed_buf,
					(unsigned long)dw_dev->processed_buf->planes[0].dma_addr,
					dw_dev->processed_buf->sequence);

			if (dw_dev->processed_buf->is_tiler_processed == 0) {
				dw_dev->processed_buf->vb.vb2_buf.timestamp = ktime_to_ns(ktime_get());
				vb2_buffer_done(&dw_dev->processed_buf->vb.vb2_buf, VB2_BUF_STATE_DONE);

				dw_dev->processed_buf->is_tiler_processed = 1;
			}

			dw_dev->dw_process_done--;
			dw_dev->state = HW_FREE;
			dw_buf = container_of(dw_dev->processed_buf, struct dw_vb2_buffer, isp_vb);
			list_del(&dw_buf->list);
		}
		dewarp_try_run(dw_dev);
		mutex_unlock(&dw_dev->state_lock);
		usleep_range(100, 500);
	}

	return 0;
}

#ifdef SET_RT_THREAD
static void dewarp_sched_set_fifo(struct task_struct *task)
{
	struct sched_attr attr = {0};
	int ret = 0;

	attr.size = sizeof(struct sched_attr);
	attr.sched_policy = SCHED_FIFO;
	attr.sched_priority = DEWARP_ISR_THREAD_PRIORITY;

	ret = sched_setattr_nocheck(task, &attr);
	if (ret)
		pr_err("Failed to set SCHED_FIFO: %d\n", ret);
	else
		pr_info("Task %s set to SCHED_FIFO with prio %d\n",
				task->comm, attr.sched_priority);
}
#endif

static int create_process_dewarp_thread(struct dewarp_subdev_dev *dw_dev)
{
	init_waitqueue_head(&dw_dev->dewarp_wq);

	dw_dev->dewarp_task = kthread_run(process_dewarp, dw_dev, "process_dewarp");
	if (IS_ERR(dw_dev->dewarp_task)) {
		pr_err("%s: Failed to create process dewarp thread\n", __func__);
		return PTR_ERR(dw_dev->dewarp_task);
	}

#ifdef SET_RT_THREAD
	dewarp_sched_set_fifo(dw_dev->dewarp_task);
#endif

	return 0;
}

static int dewarp_callback(struct dewarp_api_req *req)
{
	struct dewarp_data *data = NULL;
	struct dewarp_subdev_dev *dw_dev;
	int ret = 0;

	if (req == NULL) {
		ret = -EINVAL;
		pr_err("dewarp req is NULL\n");
		goto mem_error;
	}

	data = (struct dewarp_data *) req->pUserData;
	dw_dev = data->dw_dev;

	dewarp_dbg_callback(&dw_dev->dbg);
	dw_dev->processed_buf = data->buf;

	if (dw_dev->is_running) {
		mutex_lock(&dw_dev->state_lock);
		dw_dev->dw_process_done++;
		mutex_unlock(&dw_dev->state_lock);

		wake_up(&dw_dev->dewarp_wq);
	}

	kfree(data);
	kfree(req->src);
	kfree(req->dst);
	kfree(req);
mem_error:
	return ret;
}

static int dewarp_subdev_pad_requbufs(struct v4l2_subdev *sd, void *arg)
{
	struct vvcam_pad_reqbufs *pad_requbufs = (struct vvcam_pad_reqbufs *)arg;
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct media_pad *pad;
	struct v4l2_subdev *subdev;
	int ret, pad_num;

	pad_num = GET_CONNECTED_SINK_PAD(pad_requbufs->pad,
				dw_dev->pads[pad_requbufs->pad].flags);

	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);
	if (!pad)
		return -EINVAL;

	if (is_media_entity_v4l2_subdev(pad->entity)) {
		pad_requbufs->pad = pad->index;

		subdev = media_entity_to_v4l2_subdev(pad->entity);
		ret = v4l2_subdev_call(subdev, core, ioctl, VVCAM_PAD_REQUBUFS, pad_requbufs);
		if (ret)
			return ret;

	}
	return 0;
}

static int dewarp_subdev_pad_buf_init(struct v4l2_subdev *sd, void *arg)
{
	struct vvcam_pad_buf *pad_buf = (struct vvcam_pad_buf *)arg;
	struct v4l2_format *format = pad_buf->format;
	struct vvcam_vb2_buffer *buf = pad_buf->buf;
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	int size, i;

	if (pad_buf->pad == DEWARP_PAD_SP1 && dw_dev->pad_data[pad_buf->pad].dewarp_enable) {
		if (format->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
			size = format->fmt.pix.sizeimage;
			if (ispSS_SHM_Allocate(SHM_NONSECURE, size, 32,
					&(buf->phShm[0]), SHM_NONSECURE_CONTIG)) {
				pr_err("%s: Error while allocating memory\n", __func__);
				return -EINVAL;
			}
		} else if (format->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
			for (i = 0; i < format->fmt.pix_mp.num_planes; i++) {
				size = format->fmt.pix_mp.plane_fmt[i].sizeimage;
				if (ispSS_SHM_Allocate(SHM_NONSECURE, size, 32,
						&(buf->phShm[i]), SHM_NONSECURE_CONTIG))
					return -EINVAL;
			}
		} else {
			return -EINVAL;
		}
	}

	memcpy(&dw_dev->format, format, sizeof(struct v4l2_format));
	return  0;
}

static int dewarp_subdev_pad_buf_cleanup(struct v4l2_subdev *sd, void *arg)
{
	struct vvcam_pad_buf *pad_buf = (struct vvcam_pad_buf *)arg;
	struct v4l2_format *format = pad_buf->format;
	struct vvcam_vb2_buffer *buf = pad_buf->buf;
	int i;

	if (pad_buf->pad == DEWARP_PAD_SP1 && pad_buf->dewarp_enable) {
		if (format->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
			if (ispSS_SHM_Release((buf->phShm[0]))) {
				pr_err("%s: Error while releasing memory\n", __func__);
				return -EINVAL;
			}
		} else if (format->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
			for (i = 0; i < format->fmt.pix_mp.num_planes; i++) {
				if (ispSS_SHM_Release((buf->phShm[i]))) {
					pr_err("%s: Error while releasing memory\n", __func__);
					return -EINVAL;
				}
			}
		} else {
			return -EINVAL;
		}
	}

	return  0;
}

static int dewarp_subdev_pad_buf_prepare(struct v4l2_subdev *sd, void *arg)
{
	struct vvcam_pad_buf *pad_buf = (struct vvcam_pad_buf *)arg;
	struct v4l2_format *format = pad_buf->format;
	struct vvcam_vb2_buffer *buf = pad_buf->buf;
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	int i;

	if (pad_buf->pad == DEWARP_PAD_SP1 && dw_dev->pad_data[pad_buf->pad].dewarp_enable) {
		if (format->type == V4L2_BUF_TYPE_VIDEO_CAPTURE) {
			ispSS_SHM_GetPhysicalAddress(buf->phShm[0], 0,
					(void *)&(buf->planes[0].dma_addr));
			ispSS_SHM_GetVirtualAddress(buf->phShm[0], 0,
					(void *)&(buf->planes[0].dma_vaddr));
		} else if (format->type == V4L2_BUF_TYPE_VIDEO_CAPTURE_MPLANE) {
			for (i = 0; i < format->fmt.pix_mp.num_planes; i++) {
				ispSS_SHM_GetPhysicalAddress(buf->phShm[i], 0,
						(void *)&(buf->planes[i].dma_addr));
				ispSS_SHM_GetVirtualAddress(buf->phShm[i], 0,
						(void *)&(buf->planes[i].dma_vaddr));
			}
		} else {
			return -EINVAL;
		}
	}

	return 0;
}

static int dewarp_subdev_pad_buf_queue(struct v4l2_subdev *sd, void *arg)
{
	struct vvcam_pad_buf *pad_buf = (struct vvcam_pad_buf *)arg;
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct media_pad *pad;
	struct v4l2_subdev *subdev;
	int ret, pad_num;

	pad_num = GET_CONNECTED_SINK_PAD(pad_buf->pad, dw_dev->pads[pad_buf->pad].flags);

	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);
	if (!pad)
		return -EINVAL;

	pr_debug("%s: buf: 0x%lx", __func__, (unsigned long)pad_buf->buf);

	if (is_media_entity_v4l2_subdev(pad->entity)) {
		pad_buf->pad = pad->index;

		subdev = media_entity_to_v4l2_subdev(pad->entity);
		ret = v4l2_subdev_call(subdev, core, ioctl, VVCAM_PAD_BUF_QUEUE, pad_buf);
		if (ret)
			return ret;

	}
	return 0;
}

static int dewarp_subdev_pad_set_format(struct v4l2_subdev *sd, void *arg)
{
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct vvcam_pad_set_format *v4l2_format_pad =
		(struct vvcam_pad_set_format *)arg;
	struct media_pad *pad;
	struct v4l2_subdev *subdev;
	int ret = 0;
	int pad_num;
	struct v4l2_pix_format_mplane *pix_mp = &(v4l2_format_pad->v4l2_format.fmt.pix_mp);

	pad_num = GET_CONNECTED_SINK_PAD(v4l2_format_pad->pad,
				dw_dev->pads[v4l2_format_pad->pad].flags);
	dw_dev->pad_data[v4l2_format_pad->pad].dewarp_enable = v4l2_format_pad->dewarp_enable;
	dw_dev->pad_data[pad_num].dewarp_enable = v4l2_format_pad->dewarp_enable;

	if ((pad_num % VVCAM_ISP_PORT_PAD_NR == VVCAM_ISP_PORT_PAD_SOURCE_SP1) &&
			dw_dev->pad_data[pad_num].dewarp_enable) {
		if (pix_mp->height != dw_dev->in_height &&
				pix_mp->width != dw_dev->in_width) {
			pr_err("%s: LuT file doesn't support resolution %dx%d",
					__func__, pix_mp->width, pix_mp->height);
			dw_dev->pad_data[pad_num].dewarp_enable = 0;
			return -EINVAL;
		}
	}

	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);
	if (!pad)
		return -EINVAL;

	if (is_media_entity_v4l2_subdev(pad->entity)) {
		v4l2_format_pad->pad = pad->index;

		subdev = media_entity_to_v4l2_subdev(pad->entity);
		ret = v4l2_subdev_call(subdev, core, ioctl, VVCAM_PAD_SET_FORMAT, v4l2_format_pad);
		if (ret)
			return ret;

	}

	return ret;
}

static int dewarp_subdev_set_mcm_mode(struct v4l2_subdev *sd, void *arg)
{
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct vvcam_mcm_mode *mcm = (struct vvcam_mcm_mode *)arg;
	struct media_pad *pad;
	struct v4l2_subdev *subdev;
	int ret = 0;
	int pad_num = 0;

	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);
	if (!pad)
		return -EINVAL;

	if (is_media_entity_v4l2_subdev(pad->entity)) {
		subdev = media_entity_to_v4l2_subdev(pad->entity);
		ret = v4l2_subdev_call(subdev, core, ioctl, VVCAM_PAD_SET_MCM_MODE, mcm);
		if (ret)
			return ret;

	}

	return ret;
}

static int dewarp_subdev_set_sensor_out_state(struct v4l2_subdev *sd, void *arg)
{
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct vvcam_sensor_out_state *sensor_out_state =
		(struct vvcam_sensor_out_state *)arg;
	struct media_pad *pad;
	struct v4l2_subdev *subdev;
	int ret = 0;
	int pad_num;

	pad_num = GET_CONNECTED_SINK_PAD(sensor_out_state->pad,
				dw_dev->pads[sensor_out_state->pad].flags);

	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);
	if (!pad)
		return -EINVAL;

	if (is_media_entity_v4l2_subdev(pad->entity)) {
		sensor_out_state->pad = pad->index;

		subdev = media_entity_to_v4l2_subdev(pad->entity);
		ret = v4l2_subdev_call(subdev, core, ioctl,
				VVCAM_PAD_SET_SENSOR_OUT_STATE, sensor_out_state);
		if (ret)
			return ret;
	}

	return ret;
}

static int dewarp_subdev_capability(struct v4l2_subdev *sd, void *arg)
{
    int ret = 0;
    struct vvcam_dewarp_capability *dewarp_cap = (struct vvcam_dewarp_capability*) arg;

    dewarp_cap->dewarp_capability =  DEWARP_CAPABILITY_ENABLED;

    return ret;
}

static int dewarp_subdev_pad_s_stream(struct v4l2_subdev *sd, void *arg)
{
	struct vvcam_pad_stream_status *pad_stream = (struct vvcam_pad_stream_status *)arg;
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct media_pad *pad;
	struct v4l2_subdev *subdev;
	int ret, pad_num;
	struct DewarpCfg *cfg;

	pad_num = GET_CONNECTED_SINK_PAD(pad_stream->pad, dw_dev->pads[pad_stream->pad].flags);

	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);
	if (!pad)
		return -EINVAL;

	if (pad_stream->pad == DEWARP_PAD_SP1 && dw_dev->pad_data[DEWARP_PAD_SP1].dewarp_enable) {
		if (pad_stream->param.status) {
			dewarp_dbg_clear(&dw_dev->dbg);
			cfg = kzalloc(sizeof(*cfg), GFP_KERNEL);
			cfg->cbFunc = dewarp_callback;
			strcpy(cfg->lut_file, dw_dev->lut.lut_file);
			cfg->lutCFG = &dw_dev->lut_cfg;
			ret = dewarp_api_open(cfg, (void **)&dw_dev->pDwpCtx);
			if (!ret)
				dw_dev->is_running = 1;
			else
				return ret;
		} else {
			mutex_lock(&dw_dev->state_lock);
			dw_dev->is_running = 0;
			dw_dev->state = HW_FREE;
			mutex_unlock(&dw_dev->state_lock);

			// Do not use current pad's dewarp_enable variable after stream closes
			dw_dev->pad_data[pad_stream->pad].dewarp_enable = 0;
			dw_dev->pad_data[pad_num].dewarp_enable = 0;
			dw_dev->processed_buf = NULL;
			dw_dev->dw_process_done = 0;
			dewarp_api_close(dw_dev->pDwpCtx);
			INIT_LIST_HEAD(&dw_dev->job_queue);
		}
	}

	if (is_media_entity_v4l2_subdev(pad->entity)) {
		pad_stream->pad = pad->index;

		subdev = media_entity_to_v4l2_subdev(pad->entity);
		ret = v4l2_subdev_call(subdev, core, ioctl, VVCAM_PAD_S_STREAM, pad_stream);
		if (ret)
			return ret;

	}

	return 0;
}

static int dewarp_subdev_buf_done(struct v4l2_subdev *sd, void *arg)
{
	struct vvcam_pad_buf *pad_buf = (struct vvcam_pad_buf *)arg;
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct media_pad *pad;
	struct video_device *video;
	struct vvcam_vb2_buffer *buf = pad_buf->buf;
	int pad_num;
	struct dw_vb2_buffer *dw_buf;

	pad_num = GET_CONNECTED_SOURCE_PAD(pad_buf->pad, dw_dev->pads[pad_buf->pad].flags);

	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);

	if (!pad)
		return -EINVAL;

	if (!is_media_entity_v4l2_video_device(pad->entity)) {
		pr_err("%s remote entity not a video device\n", __func__);
		return -EINVAL;
	}

	pr_debug("%s:v4l2 buf: 0x%lx Yval: 0X%lX\n", __func__, (unsigned long)buf,
			(unsigned long)buf->planes[0].dma_addr);
	pr_debug("%s:internal buf: 0x%lx Yval: 0X%lX\n", __func__, (unsigned long)buf,
			(unsigned long)buf->planes[0].phy_addr);

	video = media_entity_to_video_device(pad->entity);
	if (buf->sequence < video->queue->num_buffers) {
		if (buf->vb.vb2_buf.state == VB2_BUF_STATE_ACTIVE) {
#ifdef NO_DEWARP
			buf->vb.vb2_buf.timestamp = ktime_to_ns(ktime_get());
			memcpy((void *)buf->planes[0].vir_addr,
					(void *)buf->planes[0].dma_vaddr, buf->planes[0].size);
			vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_DONE);
#else
			if (pad_num == DEWARP_PAD_SP1 &&
					dw_dev->pad_data[pad_buf->pad].dewarp_enable) {
				dw_buf = container_of(buf, struct dw_vb2_buffer, isp_vb);
				mutex_lock(&dw_dev->state_lock);
				if (dw_dev->is_running) {
					list_add_tail(&dw_buf->list, &dw_dev->job_queue);
					dewarp_try_run(dw_dev);
				}
				mutex_unlock(&dw_dev->state_lock);
			} else {
				buf->vb.vb2_buf.timestamp = ktime_to_ns(ktime_get());
				vb2_buffer_done(&buf->vb.vb2_buf, VB2_BUF_STATE_DONE);
			}
#endif
		}
	}

	return 0;
}


static int dewarp_subdev_ctrls(struct v4l2_subdev *sd, void *arg, unsigned long ioctl_val)
{
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct media_pad *pad;
	struct v4l2_subdev *subdev;
	int ret = 0;
	int pad_num = 0;

	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);
	if (!pad)
		return -EINVAL;

	if (is_media_entity_v4l2_subdev(pad->entity)) {
		subdev = media_entity_to_v4l2_subdev(pad->entity);
		ret = v4l2_subdev_call(subdev, core, ioctl, ioctl_val, arg);
		if (ret)
			return ret;

	}

	return ret;
}

static int dewarp_subdev_gs_ctrl(struct v4l2_subdev *sd, void *arg, unsigned long ioctl_val)
{
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct vvcam_pad_control *pad_ctrl =
		(struct vvcam_pad_control *)arg;
	struct media_pad *pad;
	struct v4l2_subdev *subdev;
	int ret, pad_num;

	pad_num = GET_CONNECTED_SINK_PAD(pad_ctrl->pad, dw_dev->pads[pad_ctrl->pad].flags);

	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);
	if (!pad)
		return -EINVAL;

	if (is_media_entity_v4l2_subdev(pad->entity)) {
		pad_ctrl->pad = pad->index;

		subdev = media_entity_to_v4l2_subdev(pad->entity);
		ret = v4l2_subdev_call(subdev, core, ioctl, ioctl_val, pad_ctrl);
		if (ret)
			return ret;

	}

	return 0;
}

static int dewarp_subdev_gs_ext_ctrl(struct v4l2_subdev *sd, void *arg,
			unsigned long ioctl_val)
{
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct vvcam_pad_ext_controls *pad_ext_ctrls =
		(struct vvcam_pad_ext_controls *)arg;
	struct media_pad *pad;
	struct v4l2_subdev *subdev;
	int ret, pad_num;

	pad_num = GET_CONNECTED_SINK_PAD(pad_ext_ctrls->pad,
			dw_dev->pads[pad_ext_ctrls->pad].flags);

	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);
	if (!pad)
		return -EINVAL;

	if (is_media_entity_v4l2_subdev(pad->entity)) {
		pad_ext_ctrls->pad = pad->index;

		subdev = media_entity_to_v4l2_subdev(pad->entity);
		ret = v4l2_subdev_call(subdev, core, ioctl, ioctl_val, pad_ext_ctrls);
		if (ret)
			return ret;

	}

	return 0;
}

// TODO: For now we are using VVCAM ioctls, need to implement DEWARP ioctls
static long dewarp_subdev_priv_ioctl(struct v4l2_subdev *sd,
		unsigned int cmd, void *arg)
{
	int ret = -EINVAL;

	switch (cmd) {
	case VIDIOC_QUERYCAP:
		ret = dewarp_subdev_querycap(sd, arg);
		ret = 0;
		break;
	case VVCAM_PAD_REQUBUFS:
		ret = dewarp_subdev_pad_requbufs(sd, arg);
		break;
	case VVCAM_PAD_BUF_INIT:
		ret = dewarp_subdev_pad_buf_init(sd, arg);
		break;
	case VVCAM_PAD_BUF_CLEANUP:
		ret = dewarp_subdev_pad_buf_cleanup(sd, arg);
		break;
	case VVCAM_PAD_BUF_PREPARE:
		ret = dewarp_subdev_pad_buf_prepare(sd, arg);
		break;
	case VVCAM_PAD_BUF_QUEUE:
		ret = dewarp_subdev_pad_buf_queue(sd, arg);
		break;
	case VVCAM_PAD_S_STREAM:
		ret = dewarp_subdev_pad_s_stream(sd, arg);
		break;
	case VVCAM_PAD_BUF_DONE:
		ret = dewarp_subdev_buf_done(sd, arg);
		break;
	case VVCAM_PAD_SET_FORMAT:
		ret = dewarp_subdev_pad_set_format(sd, arg);
		break;
	case VVCAM_PAD_SET_MCM_MODE:
		ret = dewarp_subdev_set_mcm_mode(sd, arg);
		break;
	case VVCAM_PAD_SET_SENSOR_OUT_STATE:
		ret = dewarp_subdev_set_sensor_out_state(sd, arg);
		break;
	case VVCAM_PAD_GET_DEWARP_CAPABILITY:
		ret = dewarp_subdev_capability(sd, arg);
		break;
	/* Generic function is used for controls that do not depend on pad index */
	case VVCAM_PAD_QUERYCTRL:
	case VVCAM_PAD_QUERY_EXT_CTRL:
	case VVCAM_PAD_TRY_EXT_CTRLS:
	case VVCAM_PAD_QUERYMENU:
		ret = dewarp_subdev_ctrls(sd, arg, cmd);
		break;
	case VVCAM_PAD_G_CTRL:
	case VVCAM_PAD_S_CTRL:
		ret = dewarp_subdev_gs_ctrl(sd, arg, cmd);
		break;
	case VVCAM_PAD_G_EXT_CTRLS:
	case VVCAM_PAD_S_EXT_CTRLS:
		ret = dewarp_subdev_gs_ext_ctrl(sd, arg, cmd);
		break;
	default:
		break;
	}

	return ret;
}

static struct v4l2_subdev_core_ops dewarp_subdev_core_ops = {
	.ioctl             = dewarp_subdev_priv_ioctl,
};

static struct v4l2_subdev_video_ops dewarp_subdev_video_ops = {
	/*.s_stream = dewarp_subdev_s_stream,*/
};


static int dewarp_subdev_set_fmt(struct v4l2_subdev *sd,
		struct v4l2_subdev_state *sd_state,
		struct v4l2_subdev_format *format)
{
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct media_pad *pad;
	struct v4l2_subdev *subdev;
	int ret, pad_num;

	pad_num = GET_CONNECTED_SINK_PAD(format->pad, dw_dev->pads[format->pad].flags);
	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);
	if (!pad)
		return -EINVAL;

	if (is_media_entity_v4l2_subdev(pad->entity)) {
		format->pad = pad->index;

		subdev = media_entity_to_v4l2_subdev(pad->entity);
		ret = v4l2_subdev_call(subdev, pad, set_fmt, sd_state, format);
		if (ret)
			return ret;

	}

	return 0;
}

static int dewarp_subdev_get_fmt(struct v4l2_subdev *sd,
		struct v4l2_subdev_state *sd_state,
		struct v4l2_subdev_format *format)
{
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct media_pad *pad;
	struct v4l2_subdev *subdev;
	int ret, pad_num;

	pad_num = GET_CONNECTED_SINK_PAD(format->pad, dw_dev->pads[format->pad].flags);
	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);
	if (!pad)
		return -EINVAL;

	if (is_media_entity_v4l2_subdev(pad->entity)) {
		format->pad = pad->index;

		subdev = media_entity_to_v4l2_subdev(pad->entity);
		ret = v4l2_subdev_call(subdev, pad, get_fmt, sd_state, format);
		if (ret)
			return ret;

	}
	return 0;
}

static int dewarp_subdev_enum_mbus_code(struct v4l2_subdev *sd,
		struct v4l2_subdev_state *sd_state,
		struct v4l2_subdev_mbus_code_enum *code)
{
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);
	struct media_pad *pad;
	struct v4l2_subdev *subdev;
	int ret, pad_num;

	pad_num = GET_CONNECTED_SINK_PAD(code->pad, dw_dev->pads[code->pad].flags);
	pad = media_entity_remote_pad(&dw_dev->pads[pad_num]);
	if (!pad)
		return -EINVAL;

	if (is_media_entity_v4l2_subdev(pad->entity)) {
		code->pad = pad->index;

		subdev = media_entity_to_v4l2_subdev(pad->entity);
		ret = v4l2_subdev_call(subdev, pad, enum_mbus_code, sd_state, code);
		if (ret)
			return ret;

	}

	return 0;
}

static const struct v4l2_subdev_pad_ops dewarp_subdev_pad_ops = {
	.set_fmt        = dewarp_subdev_set_fmt,
	.get_fmt        = dewarp_subdev_get_fmt,
	.enum_mbus_code = dewarp_subdev_enum_mbus_code,
};

struct v4l2_subdev_ops dewarp_subdev_subdev_ops = {
	.core  = &dewarp_subdev_core_ops,
	.video = &dewarp_subdev_video_ops,
	.pad   = &dewarp_subdev_pad_ops,
};


static int dewarp_subdev_open(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);

	mutex_lock(&dw_dev->mlock);
	dw_dev->refcnt++;
	mutex_unlock(&dw_dev->mlock);

	return 0;
}

static int dewarp_subdev_close(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh)
{
	struct dewarp_subdev_dev *dw_dev = v4l2_get_subdevdata(sd);

	mutex_lock(&dw_dev->mlock);
	dw_dev->refcnt--;
	mutex_unlock(&dw_dev->mlock);

	return 0;
}


static struct v4l2_subdev_internal_ops dewarp_subdev_internal_ops = {
	.open  = dewarp_subdev_open,
	.close = dewarp_subdev_close,
};

static int dewarp_subdev_link_setup(struct media_entity *entity,
		const struct media_pad *local,
		const struct media_pad *remote, u32 flags)
{
	return 0;
}

static const struct media_entity_operations dewarp_subdev_entity_ops = {
	.link_setup     = dewarp_subdev_link_setup,
	.link_validate  = v4l2_subdev_link_validate,
	.get_fwnode_pad = v4l2_subdev_get_fwnode_pad_1_to_1,

};

static int dewarp_subdev_notifier_bound(struct v4l2_async_notifier *notifier,
		struct v4l2_subdev *sd,
		struct v4l2_async_subdev *asd)
{
	int ret = 0;
	struct dewarp_subdev_dev *dw_dev = container_of(notifier,
			struct dewarp_subdev_dev, notifier);
	struct device *dev =  dw_dev->dev;

	struct fwnode_handle *ep = NULL;
	struct v4l2_fwnode_link link;
	struct media_entity *source, *sink;
	unsigned int source_pad, sink_pad;
	struct fwnode_handle *remote_ep = NULL;
	int port_id = 0;

	while (1) {
next_sd_ep:
		port_id = 0;
		ep = fwnode_graph_get_next_endpoint(sd->fwnode, ep);
		if (!ep)
			break;

		remote_ep = fwnode_graph_get_remote_endpoint(ep);
		if (!remote_ep)
			break;

		while (1) {
			// Are we at the end of dewarp endpoints ?
			if (!dw_dev->dewarp_ep[port_id])
				goto next_sd_ep;

			// If there is a match, continue to link creation
			if (dw_dev->dewarp_ep[port_id++] == remote_ep)
				break;
		}

		ret = v4l2_fwnode_parse_link(ep, &link);
		if (ret < 0) {
			dev_err(dev, "failed to parse link for %pOF: %d\n",
					to_of_node(ep), ret);
			continue;
		}

		if (sd->entity.pads[link.local_port].flags == MEDIA_PAD_FL_SINK)
			continue;

		source     = &sd->entity;
		source_pad = link.local_port;
		sink       = &dw_dev->sd.entity;
		sink_pad   = link.remote_port;
		v4l2_fwnode_put_link(&link);
		pr_debug("%s: linking %s source_pad %d flags %ld and %s sink_pad %d flags %ld\n",
				__func__, source->name, source_pad, source->pads[source_pad].flags,
				sink->name, sink_pad, sink->pads[sink_pad].flags);
		ret = media_create_pad_link(source, source_pad,
				sink, sink_pad, MEDIA_LNK_FL_ENABLED);
		if (ret) {
			dev_err(dev, "failed to create %s:%u -> %s:%u link\n",
					source->name, source_pad,
					sink->name, sink_pad);
			break;
		}

	}

	fwnode_handle_put(ep);

	return ret;
}

static void dewarp_subdev_notifier_unbound(struct v4l2_async_notifier *notifier,
		struct v4l2_subdev *sd,
		struct v4l2_async_subdev *asd)
{
}

static const struct v4l2_async_notifier_operations dewarp_subdev_notify_ops = {
	.bound    = dewarp_subdev_notifier_bound,
	.unbind   = dewarp_subdev_notifier_unbound,
};

static int dewarp_subdev_async_notifier(struct dewarp_subdev_dev *dw_dev)
{
	struct fwnode_handle *ep;
	struct fwnode_handle *remote_ep;
	struct v4l2_async_subdev *asd;
	struct device *dev = dw_dev->dev;
	int ret = 0;
	int pad = 0;
	int sink_pad = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
	v4l2_async_nf_init(&dw_dev->notifier);
#else
	v4l2_async_notifier_init(&dw_dev->notifier);
#endif

	dw_dev->notifier.ops = &dewarp_subdev_notify_ops;

	if (dev_fwnode(dw_dev->dev) == NULL)
		return 0;

	for (pad = 0; pad < DEWARP_SUBDEV_PAD_NR; pad++) {

		if (dw_dev->pads[pad].flags != MEDIA_PAD_FL_SINK)
			continue;

		ep = fwnode_graph_get_endpoint_by_id(dev_fwnode(dev),
				pad, 0, FWNODE_GRAPH_ENDPOINT_NEXT);
		if (!ep)
			continue;

		dw_dev->dewarp_ep[sink_pad++] = ep;

		remote_ep = fwnode_graph_get_remote_endpoint(ep);
		if (!remote_ep) {
			fwnode_handle_put(ep);
			continue;
		}
		fwnode_handle_put(remote_ep);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
		asd = v4l2_async_nf_add_fwnode_remote(&dw_dev->notifier,
				ep, struct v4l2_async_subdev);
#else
		asd = v4l2_async_notifier_add_fwnode_remote_subdev(&dw_dev->notifier,
				ep, struct v4l2_async_subdev);
#endif

		fwnode_handle_put(ep);

		if (IS_ERR(asd)) {
			ret = PTR_ERR(asd);
			if (ret != -EEXIST) {
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
				v4l2_async_nf_cleanup(&dw_dev->notifier);
#else
				v4l2_async_notifier_cleanup(&dw_dev->notifier);
#endif
				return ret;
			}
		}
	}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
	ret = v4l2_async_subdev_nf_register(&dw_dev->sd,
			&dw_dev->notifier);
#else
	ret = v4l2_async_subdev_notifier_register(&dw_dev->sd,
			&dw_dev->notifier);
#endif
	if (ret) {
		dev_err(dw_dev->dev, "Async notifier register error\n");
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
		v4l2_async_nf_cleanup(&dw_dev->notifier);
#else
		v4l2_async_notifier_cleanup(&dw_dev->notifier);
#endif
	}

	return ret;
}

static void dewarp_subdev_pads_init(struct dewarp_subdev_dev *dw_dev)
{
	int pad = 0;

	for (pad = 0; pad < DEWARP_SUBDEV_PAD_NR; pad++) {
		if (pad % DEWARP_CHN_MAX == 0)
			dw_dev->pads[pad].flags = MEDIA_PAD_FL_SINK;
		else
			dw_dev->pads[pad].flags = MEDIA_PAD_FL_SOURCE;
	}
	dw_dev->pad_data[2].dewarp_support = 1;
	dw_dev->pad_data[3].dewarp_support = 1;
}

static void dewarp_subdev_parse_params(struct dewarp_subdev_dev *dw_dev,
		struct platform_device *pdev)
{
	struct device_node *node = pdev->dev.of_node;
	struct ISPBE_DEWARP_LUT_CFG *cfg = &dw_dev->lut_cfg;
	struct ISPSS_FRAME_PARAM *in = &cfg->frameParam.in_frame;
	struct ISPSS_FRAME_PARAM *out = &cfg->frameParam.out_frame;

	fwnode_property_read_u32(of_fwnode_handle(node), "id", &dw_dev->id);

	READ_PARAM(node, "cisHwin", cfg->hwParams.cisHwin);
	READ_PARAM(node, "cisVwin", cfg->hwParams.cisVwin);
	READ_PARAM(node, "disHwin", cfg->hwParams.disHwin);
	READ_PARAM(node, "disVwin", cfg->hwParams.disVwin);
	READ_PARAM(node, "disHgrid", cfg->hwParams.disHgrid);
	READ_PARAM(node, "disVgrid", cfg->hwParams.disVgrid);
	READ_PARAM(node, "cisMatSize", cfg->derivedParam.cisMatSize);
	READ_PARAM(node, "cisBlkHtotal", cfg->derivedParam.cisBlkHtotal);
	READ_PARAM(node, "cisBlkVtotal", cfg->derivedParam.cisBlkVtotal);
	READ_PARAM(node, "inWidth", in->win.width);
	READ_PARAM(node, "outWidth", out->win.width);
	READ_PARAM(node, "inHeight", in->win.height);
	READ_PARAM(node, "outHeight", out->win.height);
	READ_PARAM(node, "maxDisBlocks", cfg->derivedParam.maxDisBlocks);
	READ_PARAM(node, "disV4H2scan", cfg->derivedParam.disV4H2scan);
	READ_PARAM(node, "biCubicOff", cfg->hwParams.biCubicOff);
	READ_PARAM(node, "cisBsize", cfg->derivedParam.cisBsize);
	READ_PARAM(node, "limitCbuff", cfg->derivedParam.limitCbuff);
	READ_PARAM(node, "disLumaTiles", cfg->derivedParam.disLumaTiles);
	READ_PARAM(node, "disChromaTiles", cfg->derivedParam.disChromaTiles);
	READ_PARAM(node, "disBlkSize", cfg->derivedParam.disBlkSize);
	READ_PARAM(node, "disBlkChromaSize", cfg->derivedParam.disBlkChromaSize);
	READ_PARAM(node, "blankLuma", cfg->derivedParam.blankLuma);
	READ_PARAM(node, "blankChroma", cfg->derivedParam.blankChroma);
	READ_PARAM(node, "validDisBlks", cfg->derivedParam.validDisBlks);
	READ_PARAM(node, "inBitDepth", in->bit_depth);
	READ_PARAM(node, "outBitDepth", out->bit_depth);
	READ_PARAM(node, "inFmt", in->fmt);
	READ_PARAM(node, "outFmt", out->fmt);
}

/*
 * Currently we are using 4K Lut config
 */
static void dewarp_load_default_cfg(struct ISPBE_DEWARP_LUT_CFG *lutCFG)
{
	struct ISPSS_FRAME_PARAM *in_frame = &lutCFG->frameParam.in_frame;
	struct ISPSS_FRAME_PARAM *out_frame = &lutCFG->frameParam.out_frame;

	lutCFG->hwParams.cisHwin = 0x8;
	lutCFG->hwParams.cisVwin = 0x10;
	lutCFG->hwParams.disHwin = 0x30;
	lutCFG->hwParams.disVwin = 0x30;
	lutCFG->hwParams.disHgrid = 0x8;
	lutCFG->hwParams.disVgrid = 0x10;
	lutCFG->derivedParam.cisMatSize = 0x80;
	lutCFG->derivedParam.cisBlkHtotal = 0x1e0;
	lutCFG->derivedParam.cisBlkVtotal = 0x87;
	in_frame->win.width = 0xF00;
	out_frame->win.width = 0xF00;
	in_frame->win.height = 0x870;
	out_frame->win.height = 0x870;
	lutCFG->derivedParam.maxDisBlocks = 0x56;
	lutCFG->derivedParam.disV4H2scan = 0;
	lutCFG->hwParams.biCubicOff = 0;
	lutCFG->derivedParam.cisBsize = 1;
	lutCFG->derivedParam.limitCbuff = 1;
	lutCFG->derivedParam.disLumaTiles = 0x24;
	lutCFG->derivedParam.disChromaTiles = 0x12;
	lutCFG->derivedParam.disBlkSize = 0x900;
	lutCFG->derivedParam.disBlkChromaSize = 0x900;
	lutCFG->derivedParam.blankLuma = 0;
	lutCFG->derivedParam.blankChroma = 0x80;
	lutCFG->derivedParam.validDisBlks = 0x2c14;
	in_frame->bit_depth = 0;
	out_frame->bit_depth = 0;
	in_frame->fmt = 8;
	out_frame->fmt = 8;
}

static int dewarp_subdev_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct dewarp_subdev_dev *dw_dev;
	int ret;

	dw_dev = devm_kzalloc(&pdev->dev,
			sizeof(struct dewarp_subdev_dev), GFP_KERNEL);
	if (!dw_dev)
		return -ENOMEM;

	mutex_init(&dw_dev->mlock);
	mutex_init(&dw_dev->state_lock);
	INIT_LIST_HEAD(&dw_dev->job_queue);
	dw_dev->dev = &pdev->dev;
	platform_set_drvdata(pdev, dw_dev);

	dewarp_load_default_cfg(&dw_dev->lut_cfg);
	dewarp_subdev_parse_params(dw_dev, pdev);

	v4l2_subdev_init(&dw_dev->sd, &dewarp_subdev_subdev_ops);
	snprintf(dw_dev->sd.name, V4L2_SUBDEV_NAME_SIZE,
			"%s.%d", DEWARP_NAME, dw_dev->id);

	dw_dev->sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;
	dw_dev->sd.flags |= V4L2_SUBDEV_FL_HAS_EVENTS;
	dw_dev->sd.dev =  &pdev->dev;
	dw_dev->sd.owner = THIS_MODULE;
	dw_dev->sd.internal_ops = &dewarp_subdev_internal_ops;
	dw_dev->sd.entity.ops = &dewarp_subdev_entity_ops;
	dw_dev->sd.entity.function = MEDIA_ENT_F_IO_V4L;
	dw_dev->sd.entity.obj_type = MEDIA_ENTITY_TYPE_V4L2_SUBDEV;
	dw_dev->sd.entity.name = dw_dev->sd.name;
	dw_dev->processed_buf = NULL;
	strcpy(dw_dev->lut.lut_file, DEWARP_DEFAULT_LUT);
	v4l2_set_subdevdata(&dw_dev->sd, dw_dev);

	dewarp_subdev_pads_init(dw_dev);
	ret = media_entity_pads_init(&dw_dev->sd.entity,
			DEWARP_SUBDEV_PAD_NR, dw_dev->pads);
	if (ret)
		return ret;

	ret = dewarp_subdev_async_notifier(dw_dev);
	if (ret)
		goto err_async_notifier;

	ret = v4l2_async_register_subdev(&dw_dev->sd);
	if (ret) {
		dev_err(dev, "register subdev error\n");
		goto error_regiter_subdev;
	}

	ret = dewarp_create_sysfs_dev_files(dw_dev);
	if (ret) {
		dev_err(dev, "register sysfs failed.\n");
		goto err_register_sysfs;
	}

	ret = dewarp_procfs_register(dw_dev, &dw_dev->pde);
	if (ret) {
		dev_err(dev, "register procfs failed.\n");
		goto err_register_procfs;
	}

	dewarp_dbg_create(&dw_dev->dbg);

	dw_dev->dewarp_task = NULL;
	dw_dev->dw_process_done = 0;
	dw_dev->in_width  =  DEWARP_DEFAULT_WIDTH;
	dw_dev->in_height =  DEWARP_DEFAULT_HEIGHT;
	create_process_dewarp_thread(dw_dev);

	ret = dewarp_api_init();
	if (ret)
		goto error_dewarp;

	dev_info(&pdev->dev, "dewarp driver probe success\n");

	return 0;

error_dewarp:
	dewarp_remove_sysfs_dev_files(dw_dev);
err_register_sysfs:
	dewarp_procfs_unregister(dw_dev->pde);
err_register_procfs:
	v4l2_async_unregister_subdev(&dw_dev->sd);
error_regiter_subdev:
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
	v4l2_async_nf_unregister(&dw_dev->notifier);
	v4l2_async_nf_cleanup(&dw_dev->notifier);
#else
	v4l2_async_notifier_unregister(&dw_dev->notifier);
	v4l2_async_notifier_cleanup(&dw_dev->notifier);
#endif
err_async_notifier:
	media_entity_cleanup(&dw_dev->sd.entity);

	return ret;
}

static int dewarp_subdev_remove(struct platform_device *pdev)
{
	struct dewarp_subdev_dev *dw_dev;

	dw_dev = platform_get_drvdata(pdev);

	dewarp_api_deinit();
	if (dw_dev->dewarp_task) {
		wake_up(&dw_dev->dewarp_wq);
		kthread_stop(dw_dev->dewarp_task);
	}

	dw_dev->dewarp_task = NULL;
	dewarp_dbg_remove(&dw_dev->dbg);
	dewarp_remove_sysfs_dev_files(dw_dev);
	dewarp_procfs_unregister(dw_dev->pde);
	v4l2_async_unregister_subdev(&dw_dev->sd);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
	v4l2_async_nf_unregister(&dw_dev->notifier);
	v4l2_async_nf_cleanup(&dw_dev->notifier);
#else
	v4l2_async_notifier_unregister(&dw_dev->notifier);
	v4l2_async_notifier_cleanup(&dw_dev->notifier);
#endif
	media_entity_cleanup(&dw_dev->sd.entity);
	dev_info(&pdev->dev, "vvcam isp driver remove\n");

	return 0;
}

static const struct of_device_id dewarp_subdev_of_match[] = {
	{.compatible = "syna,dolphin-dewarp",},
	{ /* sentinel */ },
};

MODULE_DEVICE_TABLE(of, dewarp_subdev_of_match);

static struct platform_driver dewarp_subdev_driver = {
	.probe  = dewarp_subdev_probe,
	.remove = dewarp_subdev_remove,
	.driver = {
		.name           = DEWARP_NAME,
		.owner          = THIS_MODULE,
		.of_match_table = dewarp_subdev_of_match,
	}
};

static int __init dewarp_subdev_init_module(void)
{
	int ret;

	ret = platform_driver_register(&dewarp_subdev_driver);
	if (ret) {
		pr_err("Failed to register dewarp driver\n");
		return ret;
	}

	return ret;
}

static void __exit dewarp_subdev_exit_module(void)
{
	platform_driver_unregister(&dewarp_subdev_driver);
}

module_init(dewarp_subdev_init_module);
module_exit(dewarp_subdev_exit_module);

MODULE_DESCRIPTION("Dewarp V4l2 Subdev driver");
MODULE_LICENSE("GPL");
