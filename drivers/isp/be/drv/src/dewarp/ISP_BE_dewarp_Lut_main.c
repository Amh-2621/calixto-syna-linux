// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/kthread.h>
#include "ISP_BE_dewarp_api.h"
#include "ISP_BE_dewarp_module.h"
#include "ISP_BE_dewarp_apifuncs.h"
#include "ispSS_api_dhub_config.h"
#include "ispSS_common.h"
#include "ispSS_dhub_intr.h"
#include "ispBE_common.h"
#include "ispBE_module_common.h"
#include "ispbe_api.h"
#include "ispbe_err.h"
#include "ISP_BE_dewarp_helper_apis.h"

/***********************************************************
 * FUNCTION: initialize DWP Lut specific code.
 * PARAMS: none
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADCALL - function called previously
 *
 **********************************************************/
INT ISPSS_BE_Lut_Init(void)
{
	INT count, loop_count;

	for (count = 0; count < MAX_CAM_SENSORS; count++) {
		/* set to -1 to indicate that no LUT is under use */
		g_dewarp.lut_ctx.active_lut[count] = -1;
		for (loop_count = 0; loop_count < MAX_LUT_BUFFERS; loop_count++) {
			g_dewarp.lut_ctx.dwp_lgdc[count][loop_count].isLutCfgAvailable = 0;
			g_dewarp.lut_ctx.dwp_lgdc[count][loop_count].rqstPreparedCnt = 0;
			mutex_init(&g_dewarp.lut_ctx.dwp_lgdc[count][loop_count].active_rqst_lock);
		}
	}
	/* Mutex lock for active_lut index */
	mutex_init(&g_dewarp.lut_ctx.active_lut_lock);

	g_dewarp.lut_ctx.dwpSwAlgoStatus = DEWARP_SW_ALGO_STATUS_IDLE;

	return ISPSS_OK;
}


/***********************************************************
 * FUNCTION: Destroy DWP Lut specific code.
 * PARAMS: none
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADCALL - function called previously
 *
 **********************************************************/
INT ISPSS_BE_Lut_Destroy(void)
{
	HRESULT Ret = ISPSS_OK;
	INT cnt, loop;

	for (cnt = 0; cnt < MAX_CAM_SENSORS; cnt++) {
		for (loop = 0; loop < MAX_LUT_BUFFERS; loop++)
			mutex_destroy(&(g_dewarp.lut_ctx.dwp_lgdc[cnt][loop].active_rqst_lock));
	}
	mutex_destroy(&g_dewarp.lut_ctx.active_lut_lock);

	return Ret;
}


/***********************************************************
 * FUNCTION: Copy active lut config data to rqstMsg
 * PARAMS: None
 * RETURN: None.
 *
 **********************************************************/
void ISPSS_BE_LoadLutCfgData(struct ISP_BE_RQST_MSG *pstRqstMsg)
{
	INT active_lut_idx;
	struct ISP_BE_DEWARP_LGDC *dwpLGDC = NULL;

	mutex_lock(&g_dewarp.lut_ctx.active_lut_lock);
	active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	mutex_unlock(&g_dewarp.lut_ctx.active_lut_lock);

	dwpLGDC = &g_dewarp.lut_ctx.dwp_lgdc[pstRqstMsg->sensorID][active_lut_idx];

	mutex_lock(&dwpLGDC->active_rqst_lock);
	memcpy(&(pstRqstMsg->dwp.lutCFG), &dwpLGDC->lutCfg, sizeof(struct ISPBE_DEWARP_LUT_CFG));
	mutex_unlock(&dwpLGDC->active_rqst_lock);
}

/*******************************************************************
 * FUNCTION: Check if the algorithm exists for the current request
 * PARAMS: pstdewarpRqstMsg - Request message
 * RETURN: ISPSS_DWP_LUT_PRESENT - Lut Exists
 *         ISPSS_DWP_LUT_NOTPRESENT - Lut doesn't exists.
 *
 ********************************************************************/
INT ISPSS_BE_DEWARP_IsLutExists(struct ISP_BE_RQST_MSG *pRqstMsg)
{
	INT isLutExists = ISPSS_DWP_LUT_PRESENT;
	INT sensorID = pRqstMsg->sensorID;
	struct ISPSS_FRAME_PARAM *in_frame;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[sensorID];
	struct ISPBE_DEWARP_LUT_CFG *lutCfg = NULL;

	mutex_lock(&g_dewarp.lut_ctx.dwp_lgdc[sensorID][active_lut_idx].active_rqst_lock);

	lutCfg = &g_dewarp.lut_ctx.dwp_lgdc[sensorID][active_lut_idx].lutCfg;
	in_frame = &lutCfg->frameParam.in_frame;

	pr_debug("%s sensorID: %d %d in_frame_fmt: %d %d bit depth: %d %d\n",
		__func__, sensorID,
		g_dewarp.lut_ctx.dwp_lgdc[sensorID][active_lut_idx].dwpSensorId,
		pRqstMsg->in_frame_fmt, in_frame->fmt, pRqstMsg->in_frame_bit_depth,
		in_frame->bit_depth);
	pr_debug("%s width: %d %d height: %d %d\n", __func__,
			pRqstMsg->active.width, in_frame->win.width,
			pRqstMsg->active.height, in_frame->win.height);
	if ((sensorID != g_dewarp.lut_ctx.dwp_lgdc[sensorID][active_lut_idx].dwpSensorId)
			|| (pRqstMsg->in_frame_fmt) != (in_frame->fmt) ||
			(pRqstMsg->in_frame_bit_depth != in_frame->bit_depth) ||
			(pRqstMsg->active.width != in_frame->win.width) ||
			(pRqstMsg->active.height != in_frame->win.height)) {
		isLutExists = ISPSS_DWP_LUT_NOTPRESENT;
	}
	mutex_unlock(&g_dewarp.lut_ctx.dwp_lgdc[sensorID][active_lut_idx].active_rqst_lock);

	return isLutExists;
}

/*******************************************************************
 * FUNCTION: Check Algorithm status
 * PARAMS: pendRqstMsg - Request buffer
 * RETURN: ISPSS_DWP_LUT_PRESENT - Lut Exists
 *         ISPSS_DWP_LUT_NOTPRESENT - Lut doesn't exists. Need to trigger Lut gen
 *         ISPSS_DWP_LUTGEN_INPROGRESS - LUT generation is in progress
 *
 ********************************************************************/
INT ISPSS_BE_DEWARP_CheckLutStatus(struct ISP_BE_RQST_MSG *pendRqstMsg)
{
	HRESULT Ret = ISPSS_OK;

	if (pendRqstMsg) {
		/* Check if the Algo is under process*/
		if (g_dewarp.lut_ctx.dwpSwAlgoStatus == DEWARP_SW_ALGO_STATUS_RUNNING)
			Ret = ISPSS_DWP_LUTGEN_INPROGRESS;

		/* copy the active LUT to request message */
		ISPSS_BE_LoadLutCfgData(pendRqstMsg);
		/* If not same request, generate the lut */
		Ret = ISPSS_BE_DEWARP_IsLutExists(pendRqstMsg);
	}

	return Ret;
}
