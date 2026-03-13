// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/videodev2.h>
#include <linux/delay.h>

#include "ispBE_common.h"
#include "ispbe_err.h"
#include "ispbe_api.h"
#include "ispbe_api.h"
#include "ispSS_rqstq.h"
#include "ISP_BE_dewarp_api.h"
#include "ISP_BE_dewarp_helper_apis.h"
#include "common.h"

#include "dewarp_drv_api.h"

static int dewarp_loadMatrix(struct DwpCtx *pDwpCtx,
		char* lut_file, struct ISPBE_DEWARP_LUT_CFG *lutCFG)
{
	int result = ISPSS_OK;
	INT sensorId;

	sensorId = 0;
	result = ISPBE_CA_LoadMatrix(sensorId, lut_file, lutCFG);

	if (result != ISPSS_OK)
		pr_err("[LOAD MATRIX] (result=0x%x)\n", result);

	return result;
}

static void dewarpIntrHandlerCallback(struct ISP_BE_RQST_MSG *rqstMsg)
{
	struct DwpPushBufCtx *pPushBufCtx = NULL;
	struct DwpCtx *pDwpCtx = NULL;

	pr_debug("[DEWARP INTR] (rqstMsg=%p) (param=%p)\n",
			rqstMsg, rqstMsg->user_data);

	pPushBufCtx = (struct DwpPushBufCtx *)rqstMsg->user_data;
	if (pPushBufCtx == NULL)
		return;

	pDwpCtx = pPushBufCtx->pDwpCtx;
	if (pDwpCtx == NULL)
		return;

	pDwpCtx->cfg->cbFunc(pPushBufCtx->req);
	pr_debug("[CB to COMP] Done\n");

	kfree(pPushBufCtx);
	kfree(rqstMsg);
}

int dewarp_api_init(void)
{
	struct ISPBE_CA_DRV_CTX drv_ctx;
	int result = ISPSS_OK;

	ISPSS_BE_DEWARP_Probe(&drv_ctx);

	result = ISPBE_MODULE_Init(ISPBE_MODULE_DEWARP, drv_ctx, NULL);
	if (result != ISPSS_OK)
		pr_err("%s Failed (result=0x%x)\n", __func__, result);

	return result;
}

void dewarp_api_deinit(void)
{
	int result = ISPSS_OK;

	result = ISPBE_MODULE_Destroy(ISPBE_MODULE_DEWARP);
	if (result != ISPSS_OK)
		pr_err("%s Failed (result=0x%x)\n", __func__, result);
}

int dewarp_api_open(struct DewarpCfg *cfg, void **ppDwp)
{
	int result  = ISPSS_OK;
	struct DwpCtx *pDwpCtx = NULL;

	pDwpCtx  = kzalloc(sizeof(*pDwpCtx), GFP_KERNEL);
	if (pDwpCtx == NULL) {
		result = -ENOMEM;
		goto error;
	}

	result = ISPBE_MODULE_Open(ISPBE_MODULE_DEWARP, &pDwpCtx->clientID, 0,
				dewarpIntrHandlerCallback);
	if (result != ISPSS_OK) {
		pr_err("%s Module open failed (result=0x%x)\n", __func__, result);
		goto error_free;
	}

	pDwpCtx->cfg = cfg;

	if (!cfg->bypassMode) {
		result = dewarp_loadMatrix(pDwpCtx, cfg->lut_file, cfg->lutCFG);
		if (result != ISPSS_OK) {
			pr_err("%s loadMatrix Failed (result=0x%x)\n", __func__, result);
			goto error_free;
		}
	}
	*ppDwp = pDwpCtx;

	return ISPSS_OK;

error_free:
	kfree(pDwpCtx);
error:
	return result;
}

void dewarp_api_close(void *pDwp)
{
	int result  = ISPSS_OK;
	struct DwpCtx *pDwpCtx = (struct DwpCtx *)pDwp;
	UINT8 uWaitCnt = 100;

	WARN_ON(!pDwpCtx);

	while (1) {
		result = ISPBE_MODULE_Close(ISPBE_MODULE_DEWARP, pDwpCtx->clientID);

		/*Wait for HW to become Idle */
		if ((result == ISPSS_EHARDWAREBUSY || result == ISPSS_EQNOTEMPTY) && uWaitCnt > 0) {
			usleep_range(500, 1000);
			uWaitCnt--;
			continue;
		} else
			break;
	}
	if (result != ISPSS_OK)
		pr_err("%s Failed to shutdown dewarp HW\n", __func__);

	kfree(pDwpCtx->cfg);
	kfree(pDwpCtx);
}

static int dewarp_calculateInputOutputParams(struct ISP_BE_RQST_MSG *pRqstMsg)
{
	int ret = ISPSS_OK;
	struct ISP_BE_DWP_LUT_FRAME_PARAM lutParam;
	struct ISPSS_FRAME_PARAM *in_frame = &lutParam.in_frame;
	struct ISPSS_FRAME_PARAM *out_frame = &lutParam.out_frame;

	lutParam.sensorID = pRqstMsg->sensorID;
	lutParam.dwp_out_path = pRqstMsg->dwp.dwp_out_path;

	in_frame->win.width = pRqstMsg->active.width;
	in_frame->win.height = pRqstMsg->active.height;
	in_frame->bit_depth = pRqstMsg->in_frame_bit_depth;
	in_frame->fmt = pRqstMsg->in_frame_fmt;

	out_frame->win.width = pRqstMsg->active.width;
	out_frame->win.height = pRqstMsg->active.height;
	out_frame->bit_depth = pRqstMsg->out_frame_bit_depth;
	out_frame->fmt = pRqstMsg->out_frame_fmt;

	ret = ISPBE_MODULE_Ioctl(ISPBE_MODULE_DEWARP, ISP_BE_IOCTL_CMD_GET_FB_SIZE, &lutParam);
	if (ret == ISPSS_OK) {
		pRqstMsg->in_frame_Ysize = lutParam.in_frame.size_y;
		pRqstMsg->in_frame_UVsize = lutParam.in_frame.size_uv;
		pRqstMsg->out_frame_Ysize = lutParam.out_frame.size_y;
		pRqstMsg->out_frame_UVsize = lutParam.out_frame.size_uv;
	}

	return ret;
}

/* TODO other formats need to be tested */
static HRESULT dewarp_v4l2be_srcFmt(UINT32 dispFmt, enum ISPBE_ENUM_FMT *pBeFmt)
{
	HRESULT result = SUCCESS;

	switch (dispFmt) {
	case V4L2_PIX_FMT_NV12:
		*pBeFmt = ISPSS_SRCFMT_YUV420SP;
		break;
	case V4L2_PIX_FMT_NV16:
		*pBeFmt = ISPSS_SRCFMT_YUV422SP;
		break;
	default:
		result = ISPSS_SRCFMT_MAX;
		break;
	}
	return result;
}

static int dewarp_fillRequest(struct DwpCtx *pDwpCtx, struct ISP_BE_RQST_MSG *pRqstMsg,
		struct dewarp_api_req *req)
{
	int result  = ISPSS_OK;
	struct ISP_BE_DEWARP_RQST_MSG *pDwpReqMsg = NULL;
	struct DwpPushBufCtx *pPushBufCtx = NULL;
	struct v4l2_format *format;

	format = &req->src->format;
	pRqstMsg->m_BuffID       = pDwpCtx->clientID;

	pDwpReqMsg = &pRqstMsg->dwp;

	//TODO: check when to use CISP_TILE_PATH
	pDwpReqMsg->dwp_out_path =  CISP_RS_PATH;

	pRqstMsg->active.width  = format->fmt.pix_mp.width;
	pRqstMsg->active.height = format->fmt.pix_mp.height;

	/* Tiler always gives V8H8 output, hence dewarp input is fixed to V8H8 */
	pRqstMsg->in_frame_bit_depth = ISPSS_BIT_DEPTH_8BIT;
	pRqstMsg->out_frame_bit_depth = ISPSS_BIT_DEPTH_8BIT;
	pRqstMsg->in_frame_fmt = ISPSS_SRCFMT_YUV420SP_TILED_V8H8;
	result = dewarp_v4l2be_srcFmt(format->fmt.pix_mp.pixelformat,
			(enum ISPBE_ENUM_FMT *)&pRqstMsg->out_frame_fmt);
	if (result != SUCCESS) {
		pr_err("result: %d Invalid Outfmt[%d]\n", result, format->fmt.pix_mp.pixelformat);
		goto error;
	}

	result = dewarp_calculateInputOutputParams(pRqstMsg);
	if (result != ISPSS_OK)
		goto error;

	pRqstMsg->pBcmBuf      = NULL;

	pRqstMsg->InputFrameAddr_Y  = req->src->paddr[0];
	pRqstMsg->InputFrameAddr_UV = req->src->paddr[1];

	pRqstMsg->OutputFrameAddr_Y  = req->dst->paddr[0];
	pRqstMsg->OutputFrameAddr_UV = req->dst->paddr[1];

	pr_debug("%s In YAddr[%lx] UVAddr[%lx] Ysize(%d) UVSize(%d) ", __func__,
			(unsigned long)pRqstMsg->InputFrameAddr_Y,
			(unsigned long)pRqstMsg->InputFrameAddr_UV,
			pRqstMsg->in_frame_Ysize, pRqstMsg->in_frame_UVsize);

	pr_debug("%s Out YAddr[%lx] UVAddr[%lx] Ysize(%d) UVSize(%d)\n", __func__,
			(unsigned long)pRqstMsg->OutputFrameAddr_Y,
			(unsigned long)pRqstMsg->OutputFrameAddr_UV,
			pRqstMsg->out_frame_Ysize, pRqstMsg->out_frame_UVsize);

	pRqstMsg->bcm_prepared = 0;

	pRqstMsg->SubmitHw     = 1;
	pRqstMsg->WaitforIntr  = 0;

	pPushBufCtx = kzalloc(sizeof(struct DwpPushBufCtx), GFP_KERNEL);
	if (pPushBufCtx == NULL)
		goto error;

	pPushBufCtx->pDwpCtx = pDwpCtx;
	pPushBufCtx->req     = req;

	pRqstMsg->user_data = (void *)pPushBufCtx;

	return ISPSS_OK;

error:
	pr_err("%s Failed (result=0x%x)\n", __func__, result);
	return result;
}

int dewarp_api_pushBuf(struct DwpCtx *pDwpCtx, struct dewarp_api_req *req)
{
	int result = ISPSS_OK;
	struct ISP_BE_RQST_MSG *pRqstMsg = NULL;

	pRqstMsg = kzalloc(sizeof(struct ISP_BE_RQST_MSG), GFP_KERNEL);
	if (pRqstMsg == NULL)
		goto error_alloc;

	result = dewarp_fillRequest(pDwpCtx, pRqstMsg, req);
	if (result != ISPSS_OK)
		goto error_fill;

	result = ISPBE_MODULE_PushRequest(ISPBE_MODULE_DEWARP, pDwpCtx->clientID, pRqstMsg);
	if (result != ISPSS_OK)
		goto error_push;

	if (result == ISPSS_EHARDWAREBUSY) { //dewarp driver pushes to its Q and returns busy
		result = ISPSS_OK;
	}

	return ISPSS_OK;

error_push:
	kfree(pRqstMsg->user_data);
error_fill:
	kfree(pRqstMsg);
error_alloc:
	pr_err("%s Failed (result=0x%x)\n", __func__, result);

	return result;
}
