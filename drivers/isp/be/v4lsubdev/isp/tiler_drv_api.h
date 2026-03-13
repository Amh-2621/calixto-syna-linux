/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __TILER_DRV_API_H__
#define __TILER_DRV_API_H__

/**
 * struct tiler_api_push_buf_ctx - Context for pushing buffer to TILER
 * @ptiler_drv_ctx: Pointer to the TILER driver context
 * @req:            Pointer to the TILER API request
 */
struct tiler_api_push_buf_ctx {
	struct tiler_drv_ctx *ptiler_drv_ctx;
	struct tiler_api_req *req;
};

/**
 * enum TILER_OP_MODE - Supported TILER operation modes
 * @TILER_MODE_YUV422: TILER operates in YUV422 mode
 * @TILER_MODE_YUV420: TILER operates in YUV420 mode
 * @TILER_MODE_INVALID: Invalid TILER mode
 */
enum TILER_OP_MODE {
	TILER_MODE_YUV422 = 1,
	TILER_MODE_YUV420,
	TILER_MODE_INVALID,
};

/**
 * enum BUF_TILER_STATE - State of a buffer in the TILER pipeline
 * @BUF_TILER_STATE_READY:  Buffer is ready to be pushed
 * @BUF_TILER_STATE_PUSHED: Buffer has been pushed to the TILER
 * @BUF_TILER_STATE_DONE:   Buffer processing is complete
 */
enum BUF_TILER_STATE {
	BUF_TILER_STATE_READY = 0,
	BUF_TILER_STATE_PUSHED,
	BUF_TILER_STATE_DONE,
};

/**
 * enum TILER_OP_STATE - Operational state of the TILER
 * @TILER_OP_STATE_IDLE:     TILER is idle
 * @TILER_OP_STATE_STARTING: TILER is initializing or starting
 * @TILER_OP_STATE_RUNNING:  TILER is actively processing
 */
enum TILER_OP_STATE {
	TILER_OP_STATE_IDLE,
	TILER_OP_STATE_STARTING,
	TILER_OP_STATE_RUNNING,
	TILER_OP_STATE_WAITING,
};

/**
 * struct tiler_api_req - TILER Request properties
 * @pUserData:    Pointer to user-defined private data, passed back during callbacks.
 * @format:       Pointer to a V4L2 format structure specifying the video format.
 * @mmu_enable:   Flag indicating whether the MMU is enabled (1) or not (0).
 * @paddr:        Array of physical addresses (up to 4 planes) for the image buffers.
 * @is_immediate: Flag indicating if the request should be processed
 *                immediately (1) or deferred (0).
 */
struct tiler_api_req {
	void                  *pUserData;
	struct v4l2_format    *format;
	int                   mmu_enable;
	void                  *paddr[4];
	int                   is_immediate;
};

/**
 * struct tiler_api_cfg - TILER configuration properties
 * @cbFunc:       Callback function once frame is processed
 */
struct tiler_api_cfg {
	/**
	 * @brief when TILER HAL finish one requestor, it callback this function.
	 * @param Req      reutrned Req
	 */
	int (*cbFunc)(struct tiler_api_req *Req);
};

enum TILER_HW_STATE {
	TILER_HW_IDLE,
	TILER_HW_RUNNING,
	TILER_HW_STOPPING
};

/**
 * struct tiler_drv_ctx - TILER driver context
 * @cfg:         Configuration structure containing callback function
 * @instance_id: Unique identifier for the TILER instance
 */
struct tiler_drv_ctx {
	struct tiler_api_cfg cfg;
	int instance_id;
	enum TILER_HW_STATE tiler_state;
	struct mutex tiler_mutex;
};

HRESULT tiler_api_start(struct tiler_api_cfg cfg, void **ppTiler);
HRESULT tiler_api_stop(void *pTiler);
HRESULT tiler_api_init(void);
HRESULT tiler_api_deinit(void);
HRESULT tiler_api_pushBuf(struct tiler_drv_ctx *ptiler_drv_ctx, struct tiler_api_req *req);

#endif  // __TILER_DRV_API_H__
