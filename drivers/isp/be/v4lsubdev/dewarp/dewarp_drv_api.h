/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

struct frame_info {
	struct v4l2_format format;
	void *paddr[4];
	void *vaddr;
};

struct dewarp_api_req {
	void                  *pUserData;
	struct frame_info     *src;
	struct frame_info     *dst;
	int                   mmu_enable;
};

struct DewarpCfg {
	/**
	 * @brief when Dewarp HAL finish one requestor, it callback this function.
	 *
	 * @param Req      reutrned Req
	 */
	int (*cbFunc)(struct dewarp_api_req *Req);

	/**
	 *@brief Set this variable to avoid Dewarp matrix load during bypass mode.
	 *
	 *
	 */
	unsigned int bypassMode;

	/**
	 *@brief this variable gives the lut file name to be used.
	 *
	 *
	 */
	char lut_file[128];
	struct ISPBE_DEWARP_LUT_CFG *lutCFG;
};

struct DwpCtx {
	struct DewarpCfg *cfg;
	int clientID;
};

struct DwpPushBufCtx {
	struct DwpCtx *pDwpCtx;
	struct dewarp_api_req *req;
};

int dewarp_api_init(void);
void dewarp_api_deinit(void);
int dewarp_api_open(struct DewarpCfg *cfg, void **ppDwp);
void dewarp_api_close(void *pDwp);
int dewarp_api_pushBuf(struct DwpCtx *pDwpCtx, struct dewarp_api_req *req);
