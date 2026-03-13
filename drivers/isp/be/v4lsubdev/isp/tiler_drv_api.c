// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/videodev2.h>
#include "ispbe_api.h"
#include "ispbe_err.h"
#include "ispbe_debug.h"
#include "ispBE_common.h"
#include "common.h"
#include "ispSS_reg.h"
#include "ispSS_bcmbuf_debug.h"
#include "tiler_drv_api.h"
#include "isp_bcm.h"

#define CEIL(a, b)              ((a / b) + ((a % b) != 0))
#define TILER_TIMEOUT_COUNTER	100

#define TILER_BCM_Q             8
#define	TILER_BCM_IMM_Q         12

#define TILE_FALSE_INT_DEFAULT  7
#define TILE_HWIN               8
#define TILE_VWIN               8

#define ISPSS_BE_TILE_WR        4

#ifdef DUMP_RAW_BCM
static void dump_to_hex(const unsigned char *buffer, size_t length)
{
	int i;

	pr_info("Dumping buffer at address %p to hex:\n", buffer);

	for (i = 0; i < length; ++i) {
		pr_cont("%02X ", buffer[i]);

		if ((i + 1) % 16 == 0)
			pr_cont("\n");
	}

	if (i % 16 != 0)
		pr_cont("\n");

	pr_info("End of buffer dump\n");
}
#endif

static int ISP_TilerCommitBcmBuff(struct ISP_BE_BCM *pBcmBuf, int is_immediate)
{
	int res;

	pr_debug("%s immediate: %d\n", __func__, is_immediate);

	//Generate cfgQ
	ISPSS_BCMBUF_To_CFGQ(pBcmBuf->clear_bcm_buf, pBcmBuf->final_bcm_cfgQ);
	ISPSS_BCMBUF_To_CFGQ(pBcmBuf->bcm_buf, pBcmBuf->final_bcm_cfgQ);
	ISPSS_CFGQ_To_CFGQ(pBcmBuf->dma_cfgQ, pBcmBuf->final_bcm_cfgQ);

#ifdef DUMP_RAW_BCM
	pr_info("bcm_buf:       vaddr: %lx, paddr: %lx\n",
			(unsigned long)pBcmBuf->shmHandle[0].addr,
			(unsigned long)pBcmBuf->shmHandle[0].phy_addr);
	dump_to_hex(pBcmBuf->shmHandle[0].addr, 160);
	pr_info("clear_bcm_buf: vaddr: %lx, paddr: %lx\n",
			(unsigned long)pBcmBuf->shmHandle[1].addr,
			(unsigned long)pBcmBuf->shmHandle[1].phy_addr);
	dump_to_hex(pBcmBuf->shmHandle[1].addr, 256);
	pr_info("dma_cfgQ:	     vaddr: %lx, paddr: %lx\n",
			(unsigned long)pBcmBuf->shmHandle[2].addr,
			(unsigned long)pBcmBuf->shmHandle[2].phy_addr);
	dump_to_hex(pBcmBuf->shmHandle[2].addr, 256);
	pr_info("final_bcm_cfgQ:vaddr: %lx, paddr: %lx\n",
			(unsigned long)pBcmBuf->shmHandle[3].addr,
			(unsigned long)pBcmBuf->shmHandle[3].phy_addr);
	dump_to_hex(pBcmBuf->shmHandle[3].addr, 160);

	pr_err("CLEAR BCM LOG PRINT\n");
	ISPSS_BCMBUF_LogPrint(pBcmBuf->clear_bcm_buf);
	pr_err("BCM LOG PRINT\n");
	ISPSS_BCMBUF_LogPrint(pBcmBuf->bcm_buf);
	pr_err("DMA CFGQ LOG PRINT\n");
	ISPSS_CFGQ_LogPrint(pBcmBuf->dma_cfgQ);
	pr_err("Final CFGQ LOG PRINT\n");
	ISPSS_CFGQ_LogPrint(pBcmBuf->final_bcm_cfgQ);
#endif

	if (is_immediate)
		res = ISPSS_BCMDHUB_CFGQ_Commit(pBcmBuf->final_bcm_cfgQ,
				CPCB_1, TILER_BCM_IMM_Q, BCM_BLOCKING_WAIT);
	else
		res = ISPSS_BCMDHUB_CFGQ_Commit(pBcmBuf->final_bcm_cfgQ,
				CPCB_1, TILER_BCM_Q, BCM_NON_BLOCKING_WAIT);

	return res;
}


static void calculate_frame_size(enum TILER_OP_MODE opMode, UINT32 uiBitsPerPixel,
		INT iHres, INT iVres, INT iVresD2, unsigned int *pYSize, unsigned int *pUVSize)
{
	uint32_t uLineBytes = (iHres * uiBitsPerPixel) / 8;

	switch (opMode) {
	case TILER_MODE_YUV420:
		*pYSize = uLineBytes * iVres;
		*pUVSize = uLineBytes * iVresD2;
		break;
	case TILER_MODE_YUV422:
		*pYSize = uLineBytes * iVres;
		*pUVSize = uLineBytes * iVres;
		break;
	case TILER_MODE_INVALID:
	default:
		*pYSize = 0;
		*pUVSize = 0;
		break;
	}
}

uint8_t ISPSS_TilerCalculateOutputParams(struct ISP_BE_RQST_MSG *pRqstMsg,
		enum TILER_OP_MODE opMode, UINT32 uiBitsPerPixel)
{
	struct ISP_BE_TILE_RQST_MSG *pTileReqMsg = &pRqstMsg->tile;
	INT iHres, iVres, iVresD2;

	iHres = CEIL(pRqstMsg->active.width, pTileReqMsg->tileHwin) * pTileReqMsg->tileHwin;
	iVres = CEIL(pRqstMsg->active.height, pTileReqMsg->tileVwin) * pTileReqMsg->tileVwin;
	iVresD2 = CEIL(pRqstMsg->active.height/2, pTileReqMsg->tileVwin) * pTileReqMsg->tileVwin;

	calculate_frame_size(opMode, uiBitsPerPixel,
		iHres, iVres, iVresD2, &pRqstMsg->out_frame_Ysize, &pRqstMsg->out_frame_UVsize);

	return SUCCESS;
}

static uint8_t get_bit_depth(uint32_t pixelformat)
{
	switch (pixelformat) {
	case V4L2_PIX_FMT_NV12:
	case V4L2_PIX_FMT_NV16:
		return 8;
	default:
		return 8;
	}
}

static enum TILER_OP_MODE get_op_mode(uint32_t pixelformat)
{
	switch (pixelformat) {
	case V4L2_PIX_FMT_NV12:
		return TILER_MODE_YUV420;
	case V4L2_PIX_FMT_NV16:
		return TILER_MODE_YUV422;
	default:
		return TILER_MODE_INVALID;
	}
}

static HRESULT tiler_fillRequest(struct tiler_drv_ctx *ptiler_drv_ctx,
		struct ISP_BE_RQST_MSG *pRqstMsg,
		struct tiler_api_req *req)
{
	uint32_t uFrameSizeY = 0;
	uint32_t uFrameSizeUV = 0;
	uint8_t tileHWin = TILE_HWIN;
	uint8_t tileVWin = TILE_VWIN;
	uint32_t width = 0, height = 0;
	uint8_t bitDepth = 8;
	enum TILER_OP_MODE opMode = TILER_MODE_INVALID;
	enum ISPBE_ENUM_FMT eTilerOutFmt = ISPSS_SRCFMT_YUV420SP_TILED_V8H8;
	enum ISPBE_ENUM_FMT eTilerInFmt = ISPSS_SRCFMT_YUV420SP;
	struct tiler_api_push_buf_ctx  *pPushBufCtx = NULL;

	if (!ptiler_drv_ctx || !pRqstMsg || !req || !req->format) {
		pr_err("%s: Invalid argument\n", __func__);
		return FAILURE;
	}

	bitDepth = get_bit_depth(req->format->fmt.pix_mp.pixelformat);
	opMode = get_op_mode(req->format->fmt.pix_mp.pixelformat);

	// Extract width and height from the v4l2_format inside req
	width = req->format->fmt.pix_mp.width;
	height = req->format->fmt.pix_mp.height;

	pr_debug("%s format width=%u height=%u\n", __func__, width, height);

	// Determine tiler mode and output format based on pixel format
	switch (req->format->fmt.pix_mp.pixelformat) {
	case V4L2_PIX_FMT_NV12:  // YUV420 semi planar
		uFrameSizeY = width * height;
		uFrameSizeUV = width * height / 2;
		eTilerOutFmt = ISPSS_SRCFMT_YUV420SP_TILED_V8H8;
		opMode = TILER_MODE_YUV420;
		break;
	case V4L2_PIX_FMT_NV16:  // YUV422 semi planar
		uFrameSizeY = width * height;
		uFrameSizeUV = width * height;
		eTilerOutFmt = ISPSS_SRCFMT_YUV422SP_TILED_V8H8;
		opMode = TILER_MODE_YUV422;
		break;
	default:
		pr_err("%s: Unsupported pixelformat 0x%x\n",
			__func__, req->format->fmt.pix_mp.pixelformat);
		return FAILURE;
	}

	// Fill common fields
	pRqstMsg->m_BuffID = 0;
	pRqstMsg->m_Priority = 0;
	pRqstMsg->pBcmBuf = NULL;
	pRqstMsg->bcm_prepared = 0;
	pRqstMsg->pCurrBcmBuf = NULL;
	pRqstMsg->sensorID = 0;

	pRqstMsg->in_frame_fmt = eTilerInFmt;
	pRqstMsg->in_frame_bit_depth = (bitDepth == 8) ? 0 : 1;
	pRqstMsg->active.width = width;
	pRqstMsg->active.height = height;

	// Set output buffer addresses and sizes
	pRqstMsg->OutputFrameAddr_Y = req->paddr[0];
	pRqstMsg->OutputFrameAddr_UV = req->paddr[0] + uFrameSizeY;
	pRqstMsg->out_frame_Ysize = uFrameSizeY;
	pRqstMsg->out_frame_UVsize = uFrameSizeUV;
	pRqstMsg->out_frame_fmt = eTilerOutFmt;
	pRqstMsg->out_frame_bit_depth = (bitDepth == 8) ? 0 : 1;

	// Flags for hardware submission
	pRqstMsg->SubmitHw =  0;   // Set to 0 to defer hardware submission
	pRqstMsg->WaitforIntr = 0; // No interrupt wait required

	// Tile parameters
	pRqstMsg->tile.TileFalseInt = TILE_FALSE_INT_DEFAULT;
	pRqstMsg->tile.tileHwin = tileHWin;
	pRqstMsg->tile.tileVwin = tileVWin;

	pRqstMsg->tile.isRsToTile = 1;
	pRqstMsg->tile.dataAlignment = 1;
	pRqstMsg->tile.isSwapEnable = 0;

	ISPSS_TilerCalculateOutputParams(pRqstMsg, opMode, bitDepth);

	pPushBufCtx = kzalloc(sizeof(*pPushBufCtx), GFP_KERNEL);
	if (pPushBufCtx == NULL)
		goto error;

	pPushBufCtx->ptiler_drv_ctx = ptiler_drv_ctx;
	pPushBufCtx->req     = req;

	pRqstMsg->user_data = (void *)pPushBufCtx;

	pr_debug("[FILL REQUEST] Success (pRqstMsg=%p) (param=%p)\n",
			pRqstMsg, pRqstMsg->user_data);
	return SUCCESS;
error:
	pr_err("[FILL REQUEST] Failed\n");
	return FAILURE;
}

HRESULT tiler_api_pushBuf(struct tiler_drv_ctx *ptiler_drv_ctx,
		struct tiler_api_req *req)
{
	HRESULT result = SUCCESS;
	struct ISP_BE_RQST_MSG *pRqstMsg = NULL;
	static int count;

	if (!ptiler_drv_ctx || !req) {
		pr_err("%s: Invalid argument\n", __func__);
		result = FAILURE;
		goto error_invalid;
	}

	//mutex_lock(&ptiler_drv_ctx->tiler_mutex);
	if (ptiler_drv_ctx->tiler_state != TILER_HW_RUNNING) {
		pr_err("%s: Tiler HW is busy skip pushing %d\n",
			__func__, ptiler_drv_ctx->tiler_state);
		result = FAILURE;
		goto error_alloc;
	}

	// Check whether bcm queue is empty when is_immediate = 0
	if (!req->is_immediate) {
		if (ISPSS_BCMBuffer_Full(ISPSS_BE_TILE_WR)) {
			pr_debug("%s: BCM buffer full\n", __func__);
			result = -100;
			goto error_alloc;
		}
	}

	pRqstMsg = kzalloc(sizeof(struct ISP_BE_RQST_MSG), GFP_KERNEL);
	if (pRqstMsg == NULL) {
		result = FAILURE;
		goto error_alloc;
	}

	result = tiler_fillRequest(ptiler_drv_ctx, pRqstMsg, req);
	if (result != SUCCESS) {
		pr_err("%s: tiler_fillRequest failed\n", __func__);
		goto error_fill;
	}

	result = ISPBE_MODULE_PushRequest(ISPBE_MODULE_TILER, 0, pRqstMsg);
	if (result != SUCCESS) {
		pr_err("%s: ISPBE_MODULE_PushRequest failed %d\n", __func__, result);
		goto error_push;
	}

	if (pRqstMsg->pBcmBuf != NULL) {
		result = ISP_TilerCommitBcmBuff(pRqstMsg->pBcmBuf, req->is_immediate);
		if (result != ISPSS_OK)
			pr_err("%s: ISP_TilerCommitBcmBuff failed %d\n", __func__, result);
		else
			pr_debug("%s: number of push %d %d %lx\n", __func__,
					++count, req->is_immediate, (unsigned long)req);
	}

	//mutex_unlock(&ptiler_drv_ctx->tiler_mutex);
	pr_debug("%s: [PUSH] Success\n", __func__);
	return result;

error_push:
	kfree(pRqstMsg->user_data);
error_fill:
	kfree(pRqstMsg);
error_alloc:
	//mutex_unlock(&ptiler_drv_ctx->tiler_mutex);
error_invalid:
	pr_debug("%s: [PUSH] Failed (result = %d)\n", __func__, result);
	return result;
}

static void tilerIntrHandlerCallback(struct ISP_BE_RQST_MSG *rqstMsg)
{
	struct tiler_api_push_buf_ctx *pPushBufCtx = NULL;
	struct tiler_drv_ctx *ptiler_drv_ctx = NULL;

	pr_debug("[TILER INTR] (rqstMsg=%p) (param=%p)\n",
			rqstMsg, rqstMsg->user_data);

	pPushBufCtx = (struct tiler_api_push_buf_ctx *)rqstMsg->user_data;
	if (pPushBufCtx == NULL) {
		pr_err("%s: Error: no TILER ctx found\n", __func__);
		goto msg_error;
	}

	ptiler_drv_ctx = (struct tiler_drv_ctx *)pPushBufCtx->ptiler_drv_ctx;
	if (ptiler_drv_ctx == NULL) {
		pr_err("%s: Major error found with no TILER ctx\n", __func__);
		goto ctx_error;
	}

	ptiler_drv_ctx->cfg.cbFunc(pPushBufCtx->req);
	pr_debug("[CB to COMP] Done\n");

ctx_error:
	kfree(pPushBufCtx);
msg_error:
	kfree(rqstMsg);
}

HRESULT tiler_api_start(struct tiler_api_cfg cfg, void **ppTiler)
{
	HRESULT result  = FAILURE;
	struct tiler_drv_ctx *ptiler_drv_ctx = NULL;

	pr_debug("[OPEN]\n");

	ptiler_drv_ctx = kzalloc(sizeof(*ptiler_drv_ctx), GFP_KERNEL);
	WARN_ON(!ptiler_drv_ctx);

	ptiler_drv_ctx->tiler_state = TILER_HW_IDLE;
	mutex_init(&ptiler_drv_ctx->tiler_mutex);

	result = ISPBE_MODULE_Open(ISPBE_MODULE_TILER, &ptiler_drv_ctx->instance_id,
			0, tilerIntrHandlerCallback);
	if (result != SUCCESS) {
		pr_err("[OPEN] Failed (result = 0x%x)\n", result);
		kfree(ptiler_drv_ctx);
		return result;
	}

	ptiler_drv_ctx->tiler_state = TILER_HW_RUNNING;
	pr_info("Client ID : 0x%x\n", ptiler_drv_ctx->instance_id);
	ptiler_drv_ctx->cfg = cfg;
	pr_info("[Open]ptiler_drv_ctx=%p cb=%p\n",
			ptiler_drv_ctx, ptiler_drv_ctx->cfg.cbFunc);
	result  = SUCCESS;

	*ppTiler = ptiler_drv_ctx;

	pr_info("[OPEN] Success\n");

	return result;
}


HRESULT tiler_api_stop(void *pTiler)
{
	HRESULT result  = SUCCESS;
	struct tiler_drv_ctx *ptiler_drv_ctx = (struct tiler_drv_ctx *)pTiler;
	UINT32 uWaitCnt = TILER_TIMEOUT_COUNTER;
	struct ISP_BE_RQST_MSG *pRqstMsg = NULL;

	WARN_ON(!ptiler_drv_ctx);
	pr_info("[CLOSE]\n");

	mutex_lock(&ptiler_drv_ctx->tiler_mutex);
	ptiler_drv_ctx->tiler_state = TILER_HW_STOPPING;
	mutex_unlock(&ptiler_drv_ctx->tiler_mutex);

	while (1) {
		result = ISPBE_MODULE_Close(ISPBE_MODULE_TILER, ptiler_drv_ctx->instance_id);

		/*Wait for HW to become Idle */
		if (result == ISPSS_EHARDWAREBUSY && uWaitCnt > 0) {
			usleep_range(500, 1000);
			uWaitCnt--;
			continue;
		} else {
			break;
		}
	}

	uWaitCnt = TILER_TIMEOUT_COUNTER;
	while (1) {
		pRqstMsg = NULL;
		ISPBE_MODULE_PopRequest(ISPBE_MODULE_TILER, ptiler_drv_ctx->instance_id, &pRqstMsg);
		pr_err("%s: clean up the queue %lx\n", __func__, (unsigned long)pRqstMsg);
		if (pRqstMsg == NULL)
			break;

		kfree(pRqstMsg);
		usleep_range(500, 1000);
		uWaitCnt--;
	}

	mutex_lock(&ptiler_drv_ctx->tiler_mutex);
	ptiler_drv_ctx->tiler_state = TILER_HW_IDLE;
	mutex_unlock(&ptiler_drv_ctx->tiler_mutex);
	if (result != SUCCESS) {
		pr_err("[CLOSE] Failed to shutdown BE HW\n");
		kfree(ptiler_drv_ctx);
		return result;
	}

	kfree(ptiler_drv_ctx);

	pr_info("[CLOSE] Success\n");
	return SUCCESS;
}

HRESULT tiler_api_init(void)
{
	struct ISPBE_CA_DRV_CTX drv_ctx;
	HRESULT result = SUCCESS;

	ISPSS_BE_TILER_Probe(&drv_ctx);

	result = ISPBE_MODULE_Init(ISPBE_MODULE_TILER, drv_ctx, NULL);
	if (result != SUCCESS) {
		pr_err("[INIT] Failed (result = 0x%x)\n", result);
		result = FAILURE;
		goto err_tiler_fail;
	}

	pr_debug("[INIT] Success\n");
	return SUCCESS;

err_tiler_fail:
	return result;
}

HRESULT tiler_api_deinit(void)
{
	HRESULT result = SUCCESS;

	pr_debug("[DEINIT]\n");

	result = ISPBE_MODULE_Destroy(ISPBE_MODULE_TILER);
	if (result != SUCCESS) {
		pr_err("[DEINIT] Failed (result = 0x%x)\n", result);
		return result;
	}

	pr_debug("[DEINIT] success\n");
	return SUCCESS;
}

