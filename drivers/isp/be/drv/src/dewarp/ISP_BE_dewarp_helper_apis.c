// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "ISP_BE_dewarp_api.h"
#include "ISP_BE_dewarp_module.h"
#include "ISP_BE_dewarp_apifuncs.h"
#include "ispSS_api_dhub_config.h"
#include "ispSS_bcmbuf_debug.h"
#include "ispSS_common.h"
#include "deWarp.h"
#include "ispMISC.h"
#include "ispSS_memmap.h"
#include "Galois_memmap.h"
#include "ispSS_reg.h"
#include "ispBE_common.h"
#include "ispBE_module_common.h"

#define ISP_BE_GET_YUV_MODE(srcfmt, yuvMode) do { \
	if ((srcfmt == ISPSS_SRCFMT_YUV422SP_TILED_V8H8) || \
			(srcfmt == ISPSS_SRCFMT_YUV422SP_TILED_V8H6) \
			|| (srcfmt == ISPSS_SRCFMT_YUV422SP_TILED_V6H8) \
			|| (srcfmt == ISPSS_SRCFMT_YUV422SP)) { \
		yuvMode = MODE_YUV422; \
	} \
	else if ((srcfmt == ISPSS_SRCFMT_YUV420SP_TILED_V8H8) || \
			(srcfmt == ISPSS_SRCFMT_YUV420SP_TILED_V8H6) || \
			(srcfmt == ISPSS_SRCFMT_YUV420SP_TILED_V6H8) \
			|| (srcfmt == ISPSS_SRCFMT_YUV420SP)) { \
		yuvMode = MODE_YUV420; \
	} \
} while (0)

//[bitdepth][srcfmt]
struct ISP_BE_DWP_HW_CONFIG dwp_hw_config[2][ISPSS_SRCFMT_MAX] = {
	{   {0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_RGB444 */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_RGB888 */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV422P */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV444P */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV420SP */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV422SP */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV422SP_DWA */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV420SP_DWA */
#ifdef SYNA_ISP_BE_A0
		{8, 16, 48, 48, 8, 16, 0}, /* ISPSS_SRCFMT_YUV420SP_TILED_V8H8 */
#else
		{8, 16, 64, 48, 8, 16, 0},
#endif
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV420SP_TILED_V8H6 */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV420SP_TILED_V6H8 */
		{8, 16, 48, 24, 8, 8, 0}, /* ISPSS_SRCFMT_YUV422SP_TILED_V8H8 */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV422SP_TILED_V8H6 */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV422SP_TILED_V6H8 */
	},
	{   {0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_RGB444 */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_RGB888 */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV422P */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV444P */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV420SP */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV422SP */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV422SP_DWA */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV420SP_DWA */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV420SP_TILED_V8H8 */
		{12, 16, 36, 32, 6, 16, 0}, /* ISPSS_SRCFMT_YUV420SP_TILED_V8H6 */
		{8, 12, 32, 24, 8, 12, 0}, /* ISPSS_SRCFMT_YUV420SP_TILED_V6H8 */
		{0, 0, 0, 0, 0, 0, 0}, /* ISPSS_SRCFMT_YUV422SP_TILED_V8H8 */
		{6, 16, 36, 24, 6, 8, 0}, /* ISPSS_SRCFMT_YUV422SP_TILED_V8H6 */
		{8, 12, 32, 18, 8, 6, 0}, /* ISPSS_SRCFMT_YUV422SP_TILED_V6H8 */
	},
};

/* Derive the YUV mode based on srcfmt
 *  0 - Invalid format
 *  1 - MODE_YUV422
 *  2 - MODE_YUV420
 */
INT ISPSS_BE_DEWARP_GetYUVMode(UINT32 format)
{
	int yuvMode = 0;

	if (format == ISPSS_SRCFMT_YUV420SP_TILED_V8H8 ||
			format == ISPSS_SRCFMT_YUV420SP_TILED_V8H6 ||
			format == ISPSS_SRCFMT_YUV420SP_TILED_V6H8){
		yuvMode = MODE_YUV420;
	} else if (format == ISPSS_SRCFMT_YUV422SP_TILED_V8H8 ||
			format == ISPSS_SRCFMT_YUV422SP_TILED_V8H6 ||
			format == ISPSS_SRCFMT_YUV422SP_TILED_V6H8) {
		yuvMode = MODE_YUV422;
	}
	DWPLOGT("%s:%d yuvMode=%d\n", __func__, __LINE__, yuvMode);
	return yuvMode;
}

/* Derive
 *  -1 - Invalid bit depth
 *  1 - 8 bit
 *  0 - 10 bit
 */
INT ISPSS_BE_DEWARP_GetBitDepth(INT bit_depth_idx)
{
	int bitDepth = 0;

	if (bit_depth_idx == ISPSS_BIT_DEPTH_8BIT)
		bitDepth = 8;

	if (bit_depth_idx == ISPSS_BIT_DEPTH_10BIT)
		bitDepth = 10;

	DWPLOGT("%s:%d bit_depth=%d\n", __func__, __LINE__, bitDepth);

	return bitDepth;
}


/****************************************************************
 * FUNCTION: Get output frame size
 * PARAMS: out_frame - frame params
 *         out_path - output path 0-tile 1-RS
 * RETURN: ISPSS_OK - succeed
 *
 ****************************************************************/
static void ISPSS_BE_DEWARP_GetOutputFrameSize(struct ISPSS_FRAME_PARAM *out_frame,
				struct ISPSS_FRAME_PARAM *in_frame, BOOL out_path, UINT32 sensorID)
{
	UINT32 vRes_CA, hRes_CA, vResD, tileHwin, tileVwin;
	UINT32 lineBytes, maxLineBytes, extraBytes = 0;
	/*
	 * For fishi, lineBytes changed to int to float.
	 * If this causes any sideeffect other formats ?
	 */
	int lineBytesTile;
	INT out_bit_depth, yuvMode = MODE_YUV420;
	INT active_lut_idx;
	struct ISP_BE_DEWARP_LGDC *dwpLGDC = NULL;
	struct ISP_BE_DWP_HW_CONFIG *hwParams = NULL;

	mutex_lock(&g_dewarp.lut_ctx.active_lut_lock);
	active_lut_idx = g_dewarp.lut_ctx.active_lut[sensorID];
	mutex_unlock(&g_dewarp.lut_ctx.active_lut_lock);

	/* If Lut is not loaded, active_lut_idx will be -1 */
	if (active_lut_idx >= 0) {
		dwpLGDC = &g_dewarp.lut_ctx.dwp_lgdc[sensorID][active_lut_idx];
		hwParams = &dwpLGDC->lutCfg.hwParams;
		mutex_lock(&dwpLGDC->active_rqst_lock);
		hRes_CA = CEIL(out_frame->win.width, hwParams->cisHwin) * hwParams->cisHwin;
		vRes_CA = CEIL(out_frame->win.height, hwParams->cisVwin) * hwParams->cisVwin;
		mutex_unlock(&dwpLGDC->active_rqst_lock);
	} else {
		hwParams = &dwp_hw_config[in_frame->bit_depth][in_frame->fmt];
		hRes_CA = CEIL(out_frame->win.width, hwParams->cisHwin) * hwParams->cisHwin;
		vRes_CA = CEIL(out_frame->win.height, hwParams->cisVwin) * hwParams->cisVwin;
	}

	if (out_frame->bit_depth == ISPSS_BIT_DEPTH_8BIT)
		out_bit_depth = 8;
	else if (out_frame->bit_depth == ISPSS_BIT_DEPTH_10BIT)
		out_bit_depth = 10;

	ISP_BE_GET_YUV_MODE(out_frame->fmt, yuvMode);
	ISPBE_GET_TILE_SIZE(out_frame->fmt, tileHwin, tileVwin);

	vResD = (yuvMode == MODE_YUV420) ? (vRes_CA / 2) : (vRes_CA);

	/* If RS path */
	if (out_path == 1) {
		lineBytes = (hRes_CA * out_bit_depth) / 8;
		maxLineBytes = CEIL(lineBytes, ISPSS_BE_DEWARP_DBUB_BURST_SZ) *
						ISPSS_BE_DEWARP_DBUB_BURST_SZ;
		out_frame->stride_y = maxLineBytes;

		//dewarpDiagSt.out_frame_Ysize = lineBytes * vRes_CA;
		out_frame->size_y = maxLineBytes * vRes_CA;
		extraBytes = out_frame->size_y - (lineBytes * vRes_CA);
		DWPLOGT("Output frame Luma size=%x Dhub burst Padding Bytes=%x\n",
						out_frame->size_y, extraBytes);

		out_frame->size_uv = maxLineBytes * vResD;
		out_frame->stride_uv = maxLineBytes;
		extraBytes = out_frame->size_uv - (lineBytes * vResD);
		DWPLOGT("Output frame Chroma size=%x Dhub burst Padding Bytes=%x\n",
						out_frame->size_uv, extraBytes);
	} else if (out_path == 0) { /* if tile path*/
		//lineBytesTile = (float)(hRes_CA * out_bit_depth) / 8;
		lineBytesTile = (hRes_CA * out_bit_depth) / 8;

		if (tileHwin != 8 || tileVwin != 8) {//and 10 bit
			//For every 16 byte there will be one extra byte
			out_frame->size_y =  lineBytesTile * vRes_CA * 16/15;
			out_frame->size_uv = lineBytesTile * vResD * 16/15;
		} else {
			//For every 16 byte there will be one extra byte
			out_frame->size_y =  lineBytesTile * vRes_CA;
			out_frame->size_uv = lineBytesTile * vResD;
		}
		DWPLOGT("Output frame Luma size=%x\n", out_frame->size_y);
		DWPLOGT("Output frame Chroma size=%x\n", out_frame->size_uv);
	}
}


/****************************************************************
 * FUNCTION: Get input frame size
 * PARAMS: in_frame - frame params
 *
 * RETURN: ISPSS_OK - succeed
 *
 ****************************************************************/
static void ISPSS_BE_DEWARP_GetInputFrameSize(struct ISPSS_FRAME_PARAM *in_frame)
{
	unsigned int iHres, iVres, iVresD2, tileHwin, tileVwin;
	unsigned int lineBytes;
	//float coeff = 1;
	int coeff = 1;
	int in_bit_depth = 0, yuvMode = MODE_YUV420;

	if (in_frame->bit_depth == ISPSS_BIT_DEPTH_8BIT)
		in_bit_depth = 8;
	else if (in_frame->bit_depth == ISPSS_BIT_DEPTH_10BIT)
		in_bit_depth = 10;

	ISP_BE_GET_YUV_MODE(in_frame->fmt, yuvMode);
	ISPBE_GET_TILE_SIZE(in_frame->fmt, tileHwin, tileVwin);

	iHres = CEIL(in_frame->win.width, tileHwin) * tileHwin;
	iVres = CEIL(in_frame->win.height, tileVwin) * tileVwin;
	iVresD2 = CEIL(in_frame->win.height/2, tileVwin) * tileVwin;

	//TODO: 10-bit not tested
	//if 10-bit, there will be extra padding byte for every 15 bytes, in tile format(here V8H6).
	if (in_bit_depth == 10)
		coeff = (float)16/15;

	lineBytes = (iHres * in_bit_depth * coeff) / 8;

	in_frame->size_y = lineBytes * iVres;
	DWPLOGT("Input frame Y size=%x\n", in_frame->size_y);

	in_frame->size_uv = (yuvMode == MODE_YUV420) ? (lineBytes * iVresD2) : (lineBytes * iVres);
	DWPLOGT("Input frame UV size=%x\n", in_frame->size_uv);
}

/****************************************************************
 * FUNCTION: Get the input/output frame buffer size
 * PARAMS: in_frame - frame params
 *
 * RETURN: ISPSS_OK - succeed
 *
 ****************************************************************/
void ISPSS_BE_DEWARP_GetFBSize(struct ISP_BE_DWP_LUT_FRAME_PARAM *pstLutParam)
{
	ISPSS_BE_DEWARP_GetInputFrameSize(&pstLutParam->in_frame);
	ISPSS_BE_DEWARP_GetOutputFrameSize(&pstLutParam->out_frame,
			&pstLutParam->in_frame, pstLutParam->dwp_out_path, pstLutParam->sensorID);
}
