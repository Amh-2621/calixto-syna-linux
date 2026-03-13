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
#include "ISP_BE_dewarp_helper_apis.h"
#include "ispbe_err.h"

#define DEWARP_BASE (MEMMAP_ISPSS_REG_BASE + ISPSS_MEMMAP_DEWARP_REG_BASE)
#define DEWARP_LGDC_BASE (DEWARP_BASE + RA_DEWARP_LGDC)
#define DEWARP_LDI_BASE (DEWARP_BASE + RA_DEWARP_LDI)
#define ISPMISC_BASE (MEMMAP_ISP_REG_BASE + ISPSS_MEMMAP_GLB_REG_BASE)

//Define this to write to DeWarp queue, undefine to write to generic queue
#define ISPSS_BE_DEWARP_SUBMIT_DEWARP_QUEUE_12

#define ISPSS_BE_DEWARP_2NDBURST (65520)

#define ISPSS_BE_DEWARP_BE_CLK (400000000) //400MHz
#define ISPSS_BE_DEWARP_FRAME_RATE (60)
#define ISPSS_BE_DEWARP_TG_BLANK_MODE0 (66666)
#define ISPSS_BE_DEWARP_TG_BLANK_MODE1 (60000)

#define ISPSS_BE_DEWARP_TG_MODE 2

#define bTST(x, b)                    (((x) >> (b)) & 1)

#define CEIL(a, b) ((a / b) + ((a % b) != 0))


/***********************************************
 * FUNCTION: Setup LGDC Timing Generator
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 ***********************************************/
INT ISPSS_BE_DEWARP_Config_LGDC_TG(struct ISP_BE_RQST_MSG *pstRqstMsg, struct DEWARP_OBJ *dwpObj)
{
	UINT32 RegAddr = 0, totalFrameDots = 0, strides = 0, strDots = 0, vSize = 0, hSize = 0;
	struct ISP_BE_BCM *pBcmBuf;
	T32DWTG_INIT tg_init;
	T32DWTG_SIZE size;
	T32DWTG_STRPOS strPos;
	T32DWTG_STRVS strVS;
	T32DWTG_FRSTPOS frstPOS;
	T32DWTG_INTRPOS intrPOS;
	INT strideChangeRequired = 0;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
			&g_dewarp.lut_ctx.dwp_lgdc[pstRqstMsg->sensorID][active_lut_idx].lutCfg;

	struct ISP_BE_DWP_HW_CONFIG *hwParams =
	&(dwpLutCfg->hwParams), *prvHwParams = &(g_dewarp.prvDwpRqst.prvDwpLutCfg->hwParams);
	//struct ISP_BE_RQST_MSG *prvRqstMsg = &(g_dewarp.prvDwpRqst.prvRqstMsg);

	if (!g_dewarp.enablePrevRqstComparision || hwParams->cisVwin != prvHwParams->cisVwin)
		strideChangeRequired = 1;

	pBcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);

	RegAddr = DEWARP_LGDC_BASE + RA_LGDC_TG;

	tg_init.u32 = 0;
	tg_init.uINIT_X = 1;
	tg_init.uINIT_Y = 1;
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_TG_INIT, tg_init.u32);

	size.u32 = 0;
	totalFrameDots = CEIL(ISPSS_BE_DEWARP_BE_CLK, ISPSS_BE_DEWARP_FRAME_RATE);
	strides = CEIL(pstRqstMsg->active.height, hwParams->cisVwin);
	strDots = (totalFrameDots / strides); //strides per frame
	vSize = strides * hwParams->cisVwin; //raster lines
	//Dots per raster line
	hSize = CEIL(pstRqstMsg->active.width, hwParams->cisHwin) * hwParams->cisHwin;
	//totalFrameDots / vSize; //actual X_SIZE

	if (ISPSS_BE_DEWARP_TG_MODE == 0) {
		//Fixed window Stride operation
		//Here 20% bandwidth used by the CIS Pixel Write Client latency
		//size.uSIZE_X = floor((totalFrameDots - ISPSS_BE_DEWARP_TG_BLANK_MODE0)/
		//vSize) *(1-0.2);
		//size.uSIZE_X = floor((totalFrameDots - ISPSS_BE_DEWARP_TG_BLANK_MODE0)/
		//vSize) *(1-0.2);
		size.uSIZE_X = hSize * 2.55;
		//size.uSIZE_Y = CEIL(totalFrameDots, size.uSIZE_X);
		size.uSIZE_Y = 65535; //TG restart done by CPU interrupt
	} else
		if (ISPSS_BE_DEWARP_TG_MODE == 1) {
			//Variable window stride operation
			size.uSIZE_X =
				((totalFrameDots - ISPSS_BE_DEWARP_TG_BLANK_MODE1) / vSize) * 0.6;
			//size.uSIZE_Y = CEIL(totalFrameDots, size.uSIZE_X);
			size.uSIZE_Y = 65535; //TG restart done by CPU interrupt
		} else
			if (ISPSS_BE_DEWARP_TG_MODE == 2) {
				//size.uSIZE_X = pstRqstMsg->active.width +
				//(2 * hwParams->cisHwin);
				//To fix interrupt miss in BWT test in VELOCE
				//size.uSIZE_X = pstRqstMsg->active.width * 1.5;
				//To fix interrupt miss in BWT test in VELOCE
				size.uSIZE_X = pstRqstMsg->active.width * 3/2;
				size.uSIZE_Y = 65535; //TG restart done by CPU interrupt
			}
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_TG_SIZE, size.u32);

	//STRPOS
	/* Stride Horizontal start count */
	strPos.u32 = 0;
	strPos.uSTRPOS_StrdHstart = 0x4;
	/* Stride Vertical start count */
	strPos.uSTRPOS_StrdVstart = 0x1;
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_DWTG_STRPOS, strPos.u32);

	//STRVS
	/*
	 * Stride Vertical Dimension in terms of Horz lines.
	 * I.e Equal to CisVwin in Dewarping Engine.
	 */
	if (strideChangeRequired) {
		strVS.u32 = 0;
		strVS.uSTRVS_StrdVend = strPos.uSTRPOS_StrdVstart +
		CEIL(pstRqstMsg->active.height, hwParams->cisVwin) * hwParams->cisVwin;
		strVS.uSTRVS_StrdVsize = hwParams->cisVwin;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_DWTG_STRVS, strVS.u32);
	}
	intrPOS.u32 = 0;
	intrPOS.uINTRPOS_IntrHstart = 8;
	//Fix for invlaid 2nd frame in some cases. ConfigZoom_V16H8_420
	intrPOS.uINTRPOS_IntrVstart = strVS.uSTRVS_StrdVend + 8;
	//intrPOS.uINTRPOS_IntrVstart = strVS.uSTRVS_StrdVend + 4; //0x65; //StrideVEnd+4
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_DWTG_INTRPOS, intrPOS.u32);

	frstPOS.u32 = 0;
	frstPOS.uFRSTPOS_FrstHstart = 2;
	frstPOS.uFRSTPOS_FrstVstart = 1;
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_DWTG_FRSTPOS, frstPOS.u32);

	return ISPSS_OK;
}

/***********************************************
 * FUNCTION: Configure LGDC
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 ***********************************************/
INT ISPSS_BE_DEWARP_Config_LGDC(struct ISP_BE_RQST_MSG *pstRqstMsg,
			struct DEWARP_OBJ *dwpObj)
{
	UINT32 RegAddr = 0, hDimension, vDimension, tileHwin, tileVwin;
	UINT32 lgdc_yuvMode, yuvMode;
	INT cisChangeRequired = 0, disChangeRequired = 0, resChangeRequired = 0;
	struct ISP_BE_BCM *pBcmBuf;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
	&g_dewarp.lut_ctx.dwp_lgdc[pstRqstMsg->sensorID][active_lut_idx].lutCfg;
	struct ISP_BE_DWP_HW_CONFIG *hwParams =
	&(dwpLutCfg->hwParams), *prvHwParams = &(g_dewarp.prvDwpRqst.prvDwpLutCfg->hwParams);
	struct ISP_BE_DWP_DERIVED *derivedParams = &(dwpLutCfg->derivedParam),
			*prvDerivedParams = &(g_dewarp.prvDwpRqst.prvDwpLutCfg->derivedParam);
	struct ISP_BE_RQST_MSG *prvRqstMsg = &(g_dewarp.prvDwpRqst.prvRqstMsg);

	//DEWARP_LGDC_BASE
	T32LGDC_CFG0 lgdc_cfg0;
	T32LGDC_CFG1 lgdc_cfg1;
	T32LGDC_CFG2 lgdc_cfg2;
	T32LGDC_CFG3 lgdc_cfg3;
	T32LGDC_CFG4 lgdc_cfg4;
	T32LGDC_CFG5 lgdc_cfg5;
	T32LGDC_CFG6 lgdc_cfg6;
	T32LGDC_CFG7 lgdc_cfg7;
	T32LGDC_CFG8 lgdc_cfg8;
#ifdef SYNA_ISP_BE_A0
	T32LGDC_CFG9 lgdc_cfg9;
#endif

	pBcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);
	ISPBE_GET_TILE_SIZE(pstRqstMsg->in_frame_fmt, tileHwin, tileVwin);

	if (!g_dewarp.enablePrevRqstComparision ||
			(hwParams->cisHwin != prvHwParams->cisHwin ||
			hwParams->cisHwin != prvHwParams->cisHwin)) {
		cisChangeRequired = 1;
	}
	if (!g_dewarp.enablePrevRqstComparision ||
			(hwParams->disHwin != prvHwParams->disHwin ||
			hwParams->disHwin != prvHwParams->disHwin)) {
		disChangeRequired = 1;
	}
	if (!g_dewarp.enablePrevRqstComparision ||
			(pstRqstMsg->active.width != prvRqstMsg->active.width ||
			pstRqstMsg->active.height != prvRqstMsg->active.height)) {
		resChangeRequired = 1;
	}

	RegAddr = DEWARP_LGDC_BASE;

	if (!g_dewarp.enablePrevRqstComparision || (disChangeRequired ||
			cisChangeRequired)) {
		lgdc_cfg0.uCFG0_CisHwin = hwParams->cisHwin;
		lgdc_cfg0.uCFG0_CisVwin = hwParams->cisVwin;
		lgdc_cfg0.uCFG0_DisHwin = hwParams->disHwin;
		lgdc_cfg0.uCFG0_DisVwin = hwParams->disVwin;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LGDC_CFG0, lgdc_cfg0.u32);
	}

	if (!g_dewarp.enablePrevRqstComparision ||
			(hwParams->disHgrid != prvHwParams->disHgrid ||
			hwParams->disVgrid != prvHwParams->disVgrid ||
			pstRqstMsg->in_frame_fmt != prvRqstMsg->in_frame_fmt)) {
		lgdc_cfg1.uCFG1_DisHgrid = hwParams->disHgrid;
		lgdc_cfg1.uCFG1_DisVgrid = hwParams->disVgrid;
		lgdc_cfg1.uCFG1_TileHwin = tileHwin;
		lgdc_cfg1.uCFG1_TileVwin = tileVwin;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LGDC_CFG1, lgdc_cfg1.u32);
	}

	hDimension = hwParams->cisHwin * CEIL(pstRqstMsg->active.width, hwParams->cisHwin);
	vDimension = hwParams->cisVwin * CEIL(pstRqstMsg->active.height, hwParams->cisVwin);

	if (!g_dewarp.enablePrevRqstComparision || (cisChangeRequired || resChangeRequired)) {
		lgdc_cfg2.uCFG2_CisMatSize = hwParams->cisHwin * hwParams->cisVwin;
		lgdc_cfg2.uCFG2_CisBlkHtotal = CEIL(pstRqstMsg->active.width, hwParams->cisHwin);
		lgdc_cfg2.uCFG2_CisBlkVtotal = CEIL(pstRqstMsg->active.height, hwParams->cisVwin);
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LGDC_CFG2, lgdc_cfg2.u32);

		lgdc_cfg3.uCFG3_Hdimension = hDimension;
		lgdc_cfg3.uCFG3_Vdimension = vDimension;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LGDC_CFG3, lgdc_cfg3.u32);
	}

	if (!g_dewarp.enablePrevRqstComparision || resChangeRequired) {
		lgdc_cfg4.uCFG4_HRES = pstRqstMsg->active.width;
		lgdc_cfg4.uCFG4_VRES = pstRqstMsg->active.height;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LGDC_CFG4, lgdc_cfg4.u32);
	}
#ifdef SYNA_ISP_BE_A0
	lgdc_cfg9.uCFG9_IHRES =  pstRqstMsg->active.width;
	lgdc_cfg9.uCFG9_IVRES =  pstRqstMsg->active.height;
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LGDC_CFG9, lgdc_cfg9.u32);
#endif

	if (!g_dewarp.enablePrevRqstComparision ||
				(derivedParams->cisBsize != prvDerivedParams->cisBsize ||
				(cisChangeRequired || resChangeRequired) ||
				ISPSS_BE_DEWARP_GetYUVMode(pstRqstMsg->in_frame_fmt) !=
				ISPSS_BE_DEWARP_GetYUVMode(prvRqstMsg->in_frame_fmt) ||
				derivedParams->disV4H2scan != prvDerivedParams->disV4H2scan ||
				derivedParams->limitCbuff != prvDerivedParams->limitCbuff ||
				hwParams->biCubicOff != prvHwParams->biCubicOff)) {
		//TODO :Need more info about this
		lgdc_cfg5.uCFG5_CisBsize = derivedParams->cisBsize;

		lgdc_cfg5.uCFG5_MaxDisBlocks = derivedParams->maxDisBlocks;
		yuvMode = ISPSS_BE_DEWARP_GetYUVMode(pstRqstMsg->in_frame_fmt);
		if (yuvMode == MODE_YUV422)
			lgdc_yuvMode = 1;
		else if (yuvMode == MODE_YUV420)
			lgdc_yuvMode = 2;

		lgdc_cfg5.uCFG5_YUVmode = lgdc_yuvMode;
		lgdc_cfg5.uCFG5_DisV4H2scan = derivedParams->disV4H2scan;
		lgdc_cfg5.uCFG5_LimitCbuff = derivedParams->limitCbuff;
		lgdc_cfg5.uCFG5_BicubicOff = hwParams->biCubicOff;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LGDC_CFG5, lgdc_cfg5.u32);
	}

	if (!g_dewarp.enablePrevRqstComparision ||
			(pstRqstMsg->in_frame_fmt != prvRqstMsg->in_frame_fmt)) {
		if (ISPSS_BE_DEWARP_GetBitDepth(pstRqstMsg->out_frame_bit_depth) == 8) {
			switch (pstRqstMsg->in_frame_fmt) {
			case ISPSS_SRCFMT_YUV422SP_TILED_V8H8:
				lgdc_cfg6.uCFG6_DisLumaTiles =
				    (hwParams->disVwin/8)*(hwParams->disHwin/8);
				lgdc_cfg6.uCFG6_DisChromaTiles =
				    (hwParams->disVwin/8)*(hwParams->disHwin/8);
				break;
			case ISPSS_SRCFMT_YUV420SP_TILED_V8H8:
				lgdc_cfg6.uCFG6_DisLumaTiles =
				    (hwParams->disVwin/8)*(hwParams->disHwin/8);
				lgdc_cfg6.uCFG6_DisChromaTiles =
				    CEIL(hwParams->disVwin, 16)*(hwParams->disHwin/8);
				break;
			}
		} else if (ISPSS_BE_DEWARP_GetBitDepth(pstRqstMsg->out_frame_bit_depth) == 10) {
			switch (pstRqstMsg->in_frame_fmt) {
			case ISPSS_SRCFMT_YUV422SP_TILED_V6H8:
				lgdc_cfg6.uCFG6_DisLumaTiles =
				    (hwParams->disVwin/6)*(hwParams->disHwin/8);
				lgdc_cfg6.uCFG6_DisChromaTiles =
				    (hwParams->disVwin/6)*(hwParams->disHwin/8);
				break;
			case ISPSS_SRCFMT_YUV422SP_TILED_V8H6:
				lgdc_cfg6.uCFG6_DisLumaTiles =
				    (hwParams->disVwin/8)*(hwParams->disHwin/6);
				lgdc_cfg6.uCFG6_DisChromaTiles =
				    (hwParams->disVwin/8)*(hwParams->disHwin/6);
				break;
			case ISPSS_SRCFMT_YUV420SP_TILED_V6H8:
				lgdc_cfg6.uCFG6_DisLumaTiles =
				    (hwParams->disVwin/6)*(hwParams->disHwin/8);
				lgdc_cfg6.uCFG6_DisChromaTiles =
				    CEIL(hwParams->disVwin, 12)*(hwParams->disHwin/8);
				break;
			case ISPSS_SRCFMT_YUV420SP_TILED_V8H6:
				lgdc_cfg6.uCFG6_DisLumaTiles =
				    (hwParams->disVwin/8)*(hwParams->disHwin/6);
				lgdc_cfg6.uCFG6_DisChromaTiles =
				    CEIL(hwParams->disVwin, 16)*(hwParams->disHwin/6);
				break;
			}
		}
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LGDC_CFG6, lgdc_cfg6.u32);
	}

	if (!g_dewarp.enablePrevRqstComparision || (disChangeRequired ||
				ISPSS_BE_DEWARP_GetYUVMode(pstRqstMsg->in_frame_fmt) !=
				ISPSS_BE_DEWARP_GetYUVMode(prvRqstMsg->in_frame_fmt) ||
				derivedParams->disBlkSize != prvDerivedParams->disBlkSize)) {
		//Total Pixels in a DIS pixel 2D blocks = (DisVwin * DisHwin)
		lgdc_cfg7.uCFG7_DisBlkSize = hwParams->disVwin * hwParams->disHwin;
		/*
		 * Total Pixels in a DIS pixel 2D blocks (Max 4096)
		 * For YUV422:
		 * DisBlkChromaSize = (DisVwin * DisHwin)
		 * for YUV420
		 * DisBlkChromaSize=(DisBlkSize+(DisVwin & 0x3)*DisHwin)
		 */
		lgdc_cfg7.uCFG7_DisBlkChromaSize =
			(ISPSS_BE_DEWARP_GetYUVMode(pstRqstMsg->in_frame_fmt) == MODE_YUV420) ?
			(hwParams->disVwin * hwParams->disHwin) : (derivedParams->disBlkSize +
			((hwParams->disVwin & 0x3) * hwParams->disHwin));
		//Debug: For Test5
		lgdc_cfg7.uCFG7_DisBlkChromaSize = derivedParams->disBlkChromaSize;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LGDC_CFG7, lgdc_cfg7.u32);
	}

	if (!g_dewarp.enablePrevRqstComparision ||
				(derivedParams->blankLuma != prvDerivedParams->blankLuma ||
				derivedParams->blankChroma != prvDerivedParams->blankChroma ||
				hwParams->disHgrid != prvHwParams->disHgrid ||
				hwParams->disVgrid != prvHwParams->disVgrid)) {
		/* TODO: Verify. What is this? Luma value Out of active video */
		lgdc_cfg8.u32 = 0;
		lgdc_cfg8.uCFG8_BlankLuma = derivedParams->blankLuma;
		/* TODO: Verify. What is this? Chroma value Out of active video */
		lgdc_cfg8.uCFG8_BlankChroma = derivedParams->blankChroma;
		/* if source is 8-bit? */
		if ((hwParams->disHgrid % 8) == 0 && (hwParams->disVgrid % 8) == 0)
			lgdc_cfg8.uCFG8_Source8bits = 1;
		else
			lgdc_cfg8.uCFG8_Source8bits = 0;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LGDC_CFG8, lgdc_cfg8.u32);
	}

	return ISPSS_OK;
}


/***********************************************
 * FUNCTION: Configure ReadClient
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 ***********************************************/
INT ISPSS_BE_DEWARP_ReadClient(struct ISP_BE_RQST_MSG *pstRqstMsg, struct DEWARP_OBJ *dwpObj)
{
	UINT32 RegAddr = 0;
	T32ReadClient_Word RdWord;
	//T32ReadClient_NonStdRes nonStdRes;
	struct ISP_BE_BCM *pBcmBuf;
	struct ISP_BE_DEWARP_LGDC *pLgdc;
	UINT32 dmaSize = 0;
	INT *cfgQ, *cfgQ_shadow;
	INT *cfgQ_len;
	//INT chanID;
	INT sensorID = pstRqstMsg->sensorID;
	struct ISPSS_DHUB_CFG_2NDDMA_PARAM stCfg2NDDMAParam;
	SIGN32 burst = 0, step1 = 0, size1 = 0, step2 = 0, size2 = 0;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
				&g_dewarp.lut_ctx.dwp_lgdc[sensorID][active_lut_idx].lutCfg;

	struct ISP_BE_DWP_HW_CONFIG *hwParams = &(dwpLutCfg->hwParams);
	//, *prvHwParams = &(g_dewarp.prvDwpRqst.prvDwpLutCfg->hwParams);
	struct ISP_BE_DWP_DERIVED *derivedParams = &(dwpLutCfg->derivedParam);
	//, *prvDerivedParams = &(g_dewarp.prvDwpRqst.prvDwpLutCfg->derivedParam);
	//struct ISP_BE_RQST_MSG *prvRqstMsg = &(g_dewarp.prvDwpRqst.prvRqstMsg);

	pBcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);

	cfgQ_shadow = (int *)pBcmBuf->dma_cfgQ->addr;
	cfgQ_len = &(pBcmBuf->dma_cfgQ->len);

	pLgdc = &g_dewarp.lut_ctx.dwp_lgdc[sensorID][g_dewarp.lut_ctx.active_lut[sensorID]];

	//If IdxMat is being programmed for first time or different sensor is being requested
	if (!g_dewarp.enablePrevRqstComparision || (pLgdc->dwpSensorId == -1 ||
				pLgdc->dwpSensorId != sensorID)) {
		/* 4 to convert to bytes from words
		 * Stride of DISLUTCISMat: Data in the file is kept as:
		 * Number of entries of LUT values, LUT values, CIS matrix
		 * +1 to specify number of entries of LUT
		 */
		dmaSize = (((CEIL(pstRqstMsg->active.width, hwParams->cisHwin) *
		hwParams->cisHwin * hwParams->cisVwin) + (derivedParams->maxDisBlocks + 1)) * 4) *
		(CEIL(pstRqstMsg->active.height, hwParams->cisVwin));
		/*
		 * Dhub register(don't know which one) is 16-bit size.
		 * So max value you can specify is 2^16-1 = 65535
		 * Whenever it crosses, 65535, align it to ISPSS_BE_DEWARP_2NDBURST.
		 * ISPSS_BE_DEWARP_2NDBURST is random number
		 * which should be multiple of 16
		 */
		if (dmaSize > 65535)
			dmaSize = CEIL(dmaSize, ISPSS_BE_DEWARP_2NDBURST) *
			    ISPSS_BE_DEWARP_2NDBURST;
		else//keep dmasize multiple of 16
			dmaSize = CEIL(dmaSize, 16) * 16;

		cfgQ = cfgQ_shadow + *cfgQ_len * 2;
		stCfg2NDDMAParam.pBcmBuf = pBcmBuf->bcm_buf;
		stCfg2NDDMAParam.dhubId = (UINT64)(&ISPSS_FWR_dhubHandle);
		stCfg2NDDMAParam.dmaId  = ispDhubChMap_FWR_Dwrp_Matrix_W;
		//stCfg2NDDMAParam.addr   = (UNSG32 *)pLgdc->phy_addr;
		stCfg2NDDMAParam.addr   = (UINT64)pLgdc->phy_addr;
		stCfg2NDDMAParam.cfgQ   = cfgQ;

		if (dmaSize <= 65535) {
			burst = dmaSize;
			step1 = dmaSize;
			size1 = 1;
		} else {
			burst = ISPSS_BE_DEWARP_2NDBURST;
			step1 = ISPSS_BE_DEWARP_2NDBURST;
			size1 = dmaSize / ISPSS_BE_DEWARP_2NDBURST;
		}
		step2 = 1;
		size2 = 1;

		pr_debug("DEWARP %s:%d 2NDDMA Params: dmaID=%x addr=%llx\n",
			__func__, __LINE__, stCfg2NDDMAParam.dmaId, stCfg2NDDMAParam.addr);
		pr_debug("DEWARP %s:%d burst=%x step1=%x size1=%x\n",
			__func__, __LINE__, burst, step1, size1);
		*cfgQ_len +=  START_2NDDMA(stCfg2NDDMAParam.dhubId, stCfg2NDDMAParam.dmaId,
			stCfg2NDDMAParam.addr, burst, step1, size1, step2, size2,
			(T64b *)stCfg2NDDMAParam.cfgQ);

		//Clear DIS Order LUT / CIS Matrix Read Client
		RegAddr = DEWARP_LDI_BASE;
		ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf, RegAddr +
				RA_LDI_RCCmDo + RA_ReadClient_Rd, 2);
		ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf, RegAddr +
				RA_LDI_RCCmDo + RA_ReadClient_Rd, 0);

		//Configure number of pixels to be transferred in a Luma frame
		//total number of 128-bit words that have to be fetched from DDR for read Client
		//RdWord.uWord_tot = pLgdc->dwpDisCis_sz[pstRqstMsg->sensorID] / 16;
		//RdWord.uWord_tot = (stCfg2NDDMAParam.stride * stCfg2NDDMAParam.height) / 16;
		//Number of 128 bit words
		RdWord.uWord_tot = (dmaSize) / 16; //Number of 128 bit words
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_RCCmDo +
						RA_ReadClient_Word, RdWord.u32);

		//Configure packSel
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr +
			RA_LDI_RCCmDo + RA_ReadClient_pack, 0);
	}

	return ISPSS_OK;
}


/**********************************************************
 * FUNCTION: Write client configuration for Luma and Chroma
 * PARAMS: pstRqstMsg - Request message
 *         component   0 - Luma
 1 - Chroma
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 *********************************************************/
INT ISPSS_BE_DEWARP_WriteClient(struct ISP_BE_RQST_MSG *pstRqstMsg, struct DEWARP_OBJ *dwpObj,
			UINT32 component, UINT32 nWords, UINT32 packSel)
{
	UINT32 RegAddr = 0;
	T32WriteClient_pix WrPix;
	T32WriteClient_pack pack;
	struct ISP_BE_BCM *pBcmBuf;

	pBcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);

	if (!component)
		RegAddr = DEWARP_LDI_BASE + RA_LDI_WCCispY;
	else
		RegAddr = DEWARP_LDI_BASE + RA_LDI_WCCispC;

	/* Write Client Configuration */
	//Clear Write Client in CIS Pixel Write – Data lane
	ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf, RegAddr + RA_WriteClient_Wr, 2);
	ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf, RegAddr + RA_WriteClient_Wr, 0);

	//Number of pixels to be tranferred
	DWPLOGT("%s:%d nWords=%x\n", __func__, __LINE__, nWords);
	WrPix.upix_tot = nWords;
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_WriteClient_pix, WrPix.u32);

	//Configure packSel
	pack.upack_Sel =  packSel;
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_WriteClient_pack, pack.u32);

	return ISPSS_OK;
}


/***********************************************
 * FUNCTION: Configure DIS Pixel Read Command Lane
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 ***********************************************/
static INT ISPSS_BE_DEWARP_DIS_PixRd_Cfg_CmdLane(struct ISP_BE_RQST_MSG *pstRqstMsg,
					struct DEWARP_OBJ *dwpObj)
{
	UINT32 RegAddr = 0, tileHwin, tileVwin;
	T32LDI_CFGD0 cfgd0;
	T32LDI_CFGD1 cfgd1;
	T32LDI_CFGD2 cfgd2;
	struct ISP_BE_BCM *pBcmBuf;

	struct ISP_BE_RQST_MSG *prvRqstMsg = &(g_dewarp.prvDwpRqst.prvRqstMsg);

	pBcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);

	ISPBE_GET_TILE_SIZE(pstRqstMsg->in_frame_fmt, tileHwin, tileVwin);

	RegAddr = DEWARP_LDI_BASE;

	if (!g_dewarp.enablePrevRqstComparision ||
		((UINT32 *)pstRqstMsg->InputFrameAddr_Y != prvRqstMsg->InputFrameAddr_Y)) {
		/* Configure base address of Luma frame in DRAM */
		cfgd0.u32 = 0;
		//cfgd0.uCFGD0_BaseAddrDY = (UINT32 *)pstRqstMsg->InputFrameAddr_Y;
		cfgd0.uCFGD0_BaseAddrDY = (UINT64)pstRqstMsg->InputFrameAddr_Y;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGD0, cfgd0.u32);
	}

	if (!g_dewarp.enablePrevRqstComparision ||
		((UINT32 *)pstRqstMsg->InputFrameAddr_UV != prvRqstMsg->InputFrameAddr_UV)) {
		/* Configure base address of Chroma frame in DRAM */
		cfgd1.u32 = 0;
		//cfgd1.uCFGD1_BaseAddrDC = (UINT32 *)pstRqstMsg->InputFrameAddr_UV;
		cfgd1.uCFGD1_BaseAddrDC = (UINT64)pstRqstMsg->InputFrameAddr_UV;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGD1, cfgd1.u32);
	}

	if (!g_dewarp.enablePrevRqstComparision ||
				(pstRqstMsg->active.width != prvRqstMsg->active.width ||
				pstRqstMsg->in_frame_fmt != prvRqstMsg->in_frame_fmt)) {
		cfgd2.u32 = 0;
		cfgd2.uCFGD2_StrideDisp = CEIL(pstRqstMsg->active.width, tileHwin) * 64;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGD2, cfgd2.u32);
	}

	return ISPSS_OK;
}


/***********************************************
 * FUNCTION: Configure DIS Pixel Read Data Lane
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 ***********************************************/
static INT ISPSS_BE_DEWARP_DIS_PixRd_Cfg_DataLane(struct ISP_BE_RQST_MSG *pstRqstMsg,
				struct DEWARP_OBJ *dwpObj)
{
	UINT32 RegAddr = 0, tileHwin, tileVwin;
#ifdef SYNA_ISP_BE_A0
	T32DIF64_CFG0 cfg0;
#else
	T32DIF1K_CFG0 cfg0;
#endif

	SHM_HANDLE  hShm;
	UINT32 lut_index, *dif_dislut, dlutcnt = 0;
	struct ISP_BE_BCM *pBcmBuf;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
			&g_dewarp.lut_ctx.dwp_lgdc[pstRqstMsg->sensorID][active_lut_idx].lutCfg;

	struct ISP_BE_DWP_HW_CONFIG *hwParams = &(dwpLutCfg->hwParams);
	struct ISP_BE_DWP_HW_CONFIG *prvHwParams = &(g_dewarp.prvDwpRqst.prvDwpLutCfg->hwParams);
	struct ISP_BE_RQST_MSG *prvRqstMsg = &(g_dewarp.prvDwpRqst.prvRqstMsg);

	pBcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);

	ISPBE_GET_TILE_SIZE(pstRqstMsg->in_frame_fmt, tileHwin, tileVwin);

	RegAddr = DEWARP_LDI_BASE;

	if (!g_dewarp.enablePrevRqstComparision ||
			(hwParams->disHwin != prvHwParams->disHwin ||
			hwParams->disVwin != prvHwParams->disVwin ||
			pstRqstMsg->in_frame_fmt != prvRqstMsg->in_frame_fmt)) {

		//Generate DIF(DeTile input formatter) DIS LUT
		dlutcnt = ISPSS_BE_DEWARP_Create_DIFDisLut(pstRqstMsg, &hShm, &dif_dislut);

		/* DIF Configuration for Luma */
		cfg0.u32 = 0x0;
		cfg0.uCFG0_sft_rst = 0x1;
#ifdef SYNA_ISP_BE_A0
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF64DY + RA_DIF64_CFG0, cfg0.u32);
#else
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF1KDY + RA_DIF1K_CFG0, cfg0.u32);
#endif

		//Configure buffer size.
		cfg0.uCFG0_tile_size = hwParams->disHwin * tileVwin;
#ifdef SYNA_ISP_BE_A0
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF64DY + RA_DIF64_CFG0, cfg0.u32);
#else
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF1KDY + RA_DIF1K_CFG0, cfg0.u32);
#endif
		// Configure DIF LUT
		for (lut_index = 0; lut_index < (hwParams->disHwin * tileVwin) / 8; lut_index++) {
#ifdef SYNA_ISP_BE_A0
			ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_DIF64DY +
				RA_DIF64_DIFLUT + lut_index*4, *(dif_dislut + lut_index));
#else
			ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_DIF1KDY +
				RA_DIF1K_DIFLUT + lut_index*4, *(dif_dislut + lut_index));
#endif
		}
		// Bring DIF out of reset
		cfg0.uCFG0_sft_rst = 0x0;
#ifdef SYNA_ISP_BE_A0
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF64DY + RA_DIF64_CFG0, cfg0.u32);
#else
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF1KDY + RA_DIF1K_CFG0, cfg0.u32);
#endif

		/* DIF  Configuration for Chroma */
		// Reset the DIF
		cfg0.u32 = 0;
		cfg0.uCFG0_sft_rst = 0x1;
#ifdef SYNA_ISP_BE_A0
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF64DC + RA_DIF64_CFG0, cfg0.u32);
#else
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF1KDC + RA_DIF1K_CFG0, cfg0.u32);
#endif
		// Configure buffer size.
		cfg0.uCFG0_tile_size = hwParams->disHwin * tileVwin;
#ifdef SYNA_ISP_BE_A0
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF64DC + RA_DIF64_CFG0, cfg0.u32);
#else
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF1KDC + RA_DIF1K_CFG0, cfg0.u32);
#endif
		// Configure DIF LUT
		for (lut_index = 0; lut_index < (hwParams->disHwin * tileVwin) / 8; lut_index++) {
#ifdef SYNA_ISP_BE_A0
			ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_DIF64DC +
				RA_DIF64_DIFLUT + lut_index*4, *(dif_dislut+lut_index));
#else
			ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_DIF1KDC +
				RA_DIF1K_DIFLUT + lut_index*4, *(dif_dislut+lut_index));
#endif
		}
		// Bring DIF out of reset
		cfg0.uCFG0_sft_rst = 0x0;
#ifdef SYNA_ISP_BE_A0
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF64DC + RA_DIF64_CFG0, cfg0.u32);
#else
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF1KDC + RA_DIF1K_CFG0, cfg0.u32);
#endif

		dif_dislut = NULL;
		ispSS_SHM_Release(hShm);
	}

	return ISPSS_OK;
}

/***********************************************
 * FUNCTION: Configure CIS Pixel Write
 command lane for Tile mode
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 ***********************************************/
static INT ISPSS_BE_DEWARP_CIS_TilePixWr_Cfg_CmdLane(struct ISP_BE_RQST_MSG *pstRqstMsg,
					struct DEWARP_OBJ *dwpObj)
{
	UINT32 RegAddr = 0, vRes_CA, hRes_CA, tileHwin, tileVwin;
	T32LDI_CFGC0 cfgc0;
	T32LDI_CFGC1 cfgc1;
	T32LDI_CFGC2 cfgc2;
	T32LDI_CFGC3 cfgc3;
	T32LDI_CFGCRS cfgcrs;
#ifdef SYNA_ISP_BE_A0
	T32LDI_CFGC7 cfgc7;
#endif
	struct ISP_BE_BCM *pBcmBuf;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
		&g_dewarp.lut_ctx.dwp_lgdc[pstRqstMsg->sensorID][active_lut_idx].lutCfg;

	struct ISP_BE_DWP_HW_CONFIG *hwParams = &(dwpLutCfg->hwParams);
	struct ISP_BE_DWP_HW_CONFIG *prvHwParams = &(g_dewarp.prvDwpRqst.prvDwpLutCfg->hwParams);
	struct ISP_BE_RQST_MSG *prvRqstMsg = &(g_dewarp.prvDwpRqst.prvRqstMsg);

	pBcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);

	ISPBE_GET_TILE_SIZE(pstRqstMsg->in_frame_fmt, tileHwin, tileVwin);

	RegAddr = DEWARP_LDI_BASE;

	if (!g_dewarp.enablePrevRqstComparision ||
		((UINT32 *)pstRqstMsg->OutputFrameAddr_Y != prvRqstMsg->OutputFrameAddr_Y)) {
		/* Configure base address of Luma frame in DRAM */
		//cfgc0.uCFGC0_BaseAddrCY = (UINT32 *)pstRqstMsg->OutputFrameAddr_Y;
		cfgc0.uCFGC0_BaseAddrCY = (UINT64)pstRqstMsg->OutputFrameAddr_Y;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGC0, cfgc0.u32);
	}

	if (!g_dewarp.enablePrevRqstComparision ||
		((UINT32 *)pstRqstMsg->OutputFrameAddr_UV != prvRqstMsg->OutputFrameAddr_UV)) {
		/* Configure base address of Chroma frame in DRAM */
		//cfgc1.uCFGC1_BaseAddrCC = (UINT32 *)pstRqstMsg->OutputFrameAddr_UV;
		cfgc1.uCFGC1_BaseAddrCC = (UINT64)pstRqstMsg->OutputFrameAddr_UV;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGC1, cfgc1.u32);
	}

	if (!g_dewarp.enablePrevRqstComparision ||
				(pstRqstMsg->active.width != prvRqstMsg->active.width ||
				hwParams->cisHwin != prvHwParams->cisHwin ||
				pstRqstMsg->in_frame_fmt != prvRqstMsg->in_frame_fmt)) {
		//Configure size of tile-stride in bytes.
		hRes_CA = CEIL(pstRqstMsg->active.width, hwParams->cisHwin) * hwParams->cisHwin;
		cfgc2.uCFGC2_StrideCisp = (hRes_CA / tileHwin) * 64;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGC2, cfgc2.u32);
#ifdef SYNA_ISP_BE_A0
		cfgc7.uCFGC7_JumpStrideCisp = cfgc2.uCFGC2_StrideCisp;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGC7, cfgc7.u32);
#endif
	}

	if (!g_dewarp.enablePrevRqstComparision ||
				(pstRqstMsg->active.height != prvRqstMsg->active.height ||
				hwParams->cisVwin != prvHwParams->cisVwin ||
				pstRqstMsg->in_frame_fmt != prvRqstMsg->in_frame_fmt)) {
		//Configure Number of tile-strides in a Luma frame
		cfgc3.u32 = 0x0;
		vRes_CA = CEIL(pstRqstMsg->active.height, hwParams->cisVwin) * hwParams->cisVwin;
		cfgc3.uCFGC3_nStridesCisp = vRes_CA / tileVwin;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGC3, cfgc3.u32);
	}

	//Enable CISP Tile Path
	ISPSS_REG_READ32(RegAddr + RA_LDI_CFGCRS, &cfgcrs.u32);
	cfgcrs.uCFGCRS_CispRsEn = 0x0;
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGCRS, cfgcrs.u32);

	return ISPSS_OK;
}

/***********************************************
 * FUNCTION: Configure CIS pixel write data
 lane for Tile format
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 ***********************************************/
static INT ISPSS_BE_DEWARP_CIS_TilePixWr_Cfg_DataLane(struct ISP_BE_RQST_MSG *pstRqstMsg,
					struct DEWARP_OBJ *dwpObj)
{
	UINT32 RegAddr = 0, tileHwin, tileVwin;
	UINT32 lut_index, *dif_cislut, dlutcnt = 0;
#ifdef SYNA_ISP_BE_A0
	T32DIF128_CFG0 cfg0;
#else
	T32DIF512_CFG0 cfg0;
#endif
	struct ISP_BE_BCM *pBcmBuf;
	SHM_HANDLE  hShm;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
		&g_dewarp.lut_ctx.dwp_lgdc[pstRqstMsg->sensorID][active_lut_idx].lutCfg;

	struct ISP_BE_DWP_HW_CONFIG *hwParams = &(dwpLutCfg->hwParams);
	struct ISP_BE_DWP_HW_CONFIG *prvHwParams = &(g_dewarp.prvDwpRqst.prvDwpLutCfg->hwParams);
	struct ISP_BE_RQST_MSG *prvRqstMsg = &(g_dewarp.prvDwpRqst.prvRqstMsg);

	pBcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);

	ISPBE_GET_TILE_SIZE(pstRqstMsg->in_frame_fmt, tileHwin, tileVwin);

	RegAddr = DEWARP_LDI_BASE;

	if (!g_dewarp.enablePrevRqstComparision ||
				(hwParams->cisHwin != prvHwParams->cisHwin ||
				hwParams->cisVwin != prvHwParams->cisVwin ||
				pstRqstMsg->in_frame_fmt != prvRqstMsg->in_frame_fmt)) {

		//Generate DIF(DeTile input formatter) DIS LUT
		dlutcnt = ISPSS_BE_DEWARP_Create_DIFCisLut(pstRqstMsg, &hShm, &dif_cislut);

		/* DIF Configuration for Luma */
		//Reset the DIF
		//TODO: Verify the reset sequence
		cfg0.u32 = 0;
#ifdef SYNA_ISP_BE_A0
		ISPSS_REG_READ32(RegAddr + RA_LDI_DIF128CY + RA_DIF128_CFG0, &cfg0.u32);
#else
		ISPSS_REG_READ32(RegAddr + RA_LDI_DIF512CY + RA_DIF512_CFG0, &cfg0.u32);
#endif

		cfg0.uCFG0_sft_rst = 0x1;
#ifdef SYNA_ISP_BE_A0
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF128CY + RA_DIF128_CFG0, cfg0.u32);
#else
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF512CY + RA_DIF512_CFG0, cfg0.u32);
#endif
		//Configure buffer size.
		cfg0.uCFG0_tile_size = hwParams->cisHwin * tileVwin;
#ifdef SYNA_ISP_BE_A0
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF128CY + RA_DIF128_CFG0, cfg0.u32);
#else
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF512CY + RA_DIF512_CFG0, cfg0.u32);
#endif
		// Configure DIF LUT
		for (lut_index = 0; lut_index < (hwParams->cisHwin * tileVwin);  lut_index++) {
#ifdef SYNA_ISP_BE_A0
			ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_DIF128CY +
				RA_DIF128_DIFLUT + lut_index*4, *(dif_cislut+lut_index));
#else
			ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_DIF512CY +
				RA_DIF512_DIFLUT + lut_index*4, *(dif_cislut+lut_index));
#endif
		}
		// Bring DIF out of reset
		cfg0.uCFG0_sft_rst = 0x0;
#ifdef SYNA_ISP_BE_A0
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF128CY + RA_DIF128_CFG0, cfg0.u32);
#else
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF512CY + RA_DIF512_CFG0, cfg0.u32);
#endif

		/* DIF  Configuration for Chroma */
		// Reset the DIF
		cfg0.u32 = 0;
#ifdef SYNA_ISP_BE_A0
		ISPSS_REG_READ32(RegAddr + RA_LDI_DIF128CC + RA_DIF128_CFG0, &cfg0.u32);
#else
		ISPSS_REG_READ32(RegAddr + RA_LDI_DIF512CC + RA_DIF512_CFG0, &cfg0.u32);
#endif
		cfg0.uCFG0_sft_rst = 0x1;
#ifdef SYNA_ISP_BE_A0
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF128CC + RA_DIF128_CFG0, cfg0.u32);
#else
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF512CC + RA_DIF512_CFG0, cfg0.u32);
#endif
		// Configure buffer size.
		cfg0.uCFG0_tile_size = hwParams->cisHwin * tileVwin;
#ifdef SYNA_ISP_BE_A0
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF128CC + RA_DIF128_CFG0, cfg0.u32);
#else
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF512CC + RA_DIF512_CFG0, cfg0.u32);
#endif
		// Configure DIF LUT
		for (lut_index = 0; lut_index < (hwParams->cisHwin * tileVwin); lut_index++) {
#ifdef SYNA_ISP_BE_A0
			ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_DIF128CC +
				RA_DIF128_DIFLUT + lut_index*4, *(dif_cislut+lut_index));
#else
			ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_DIF512CC +
				RA_DIF512_DIFLUT + lut_index*4, *(dif_cislut+lut_index));
#endif
		}
		// Bring DIF out of reset
		cfg0.uCFG0_sft_rst = 0x0;
#ifdef SYNA_ISP_BE_A0
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF128CC + RA_DIF128_CFG0, cfg0.u32);
#else
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
				RegAddr + RA_LDI_DIF512CC + RA_DIF512_CFG0, cfg0.u32);
#endif

		dif_cislut = NULL;
		ispSS_SHM_Release(hShm);
	}

	return ISPSS_OK;
}

/***********************************************
 * FUNCTION: Configure CIS pixel write
 config command lane for RS format
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 ***********************************************/
static INT ISPSS_BE_DEWARP_CIS_RSPixWr_Cfg_CmdLane(struct ISP_BE_RQST_MSG *pstRqstMsg,
					struct DEWARP_OBJ *dwpObj)
{
	UINT32 RegAddr = 0, vRes_CA, hRes_CA, tileHwin, tileVwin;
	T32LDI_CFGC0 cfgc0;
	T32LDI_CFGC1 cfgc1;
	T32LDI_CFGC2 cfgc2;
	T32LDI_CFGC3 cfgc3;
	T32LDI_CFGCRS cfgcrs;
#ifdef SYNA_ISP_BE_A0
	T32LDI_CFGC7 cfgc7;
#endif
	struct ISP_BE_BCM *pBcmBuf;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
			&g_dewarp.lut_ctx.dwp_lgdc[pstRqstMsg->sensorID][active_lut_idx].lutCfg;

	struct ISP_BE_DWP_HW_CONFIG *hwParams = &(dwpLutCfg->hwParams);
	struct ISP_BE_DWP_HW_CONFIG *prvHwParams = &(g_dewarp.prvDwpRqst.prvDwpLutCfg->hwParams);
	struct ISP_BE_RQST_MSG *prvRqstMsg = &(g_dewarp.prvDwpRqst.prvRqstMsg);

	pBcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);

	ISPBE_GET_TILE_SIZE(pstRqstMsg->in_frame_fmt, tileHwin, tileVwin);

	RegAddr = DEWARP_LDI_BASE;

	if (!g_dewarp.enablePrevRqstComparision ||
		((UINT32 *)pstRqstMsg->OutputFrameAddr_Y != prvRqstMsg->OutputFrameAddr_Y)) {
		/* Configure base address of Luma frame in DRAM */
		//cfgc0.uCFGC0_BaseAddrCY = (UINT32 *)pstRqstMsg->OutputFrameAddr_Y;
		cfgc0.uCFGC0_BaseAddrCY = (UINT64)pstRqstMsg->OutputFrameAddr_Y;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGC0, cfgc0.u32);
	}

	if (!g_dewarp.enablePrevRqstComparision ||
		((UINT32 *)pstRqstMsg->OutputFrameAddr_UV != prvRqstMsg->OutputFrameAddr_UV)) {
		/* Configure base address of Chroma frame in DRAM */
		//cfgc1.uCFGC1_BaseAddrCC = (UINT32 *)pstRqstMsg->OutputFrameAddr_UV;
		cfgc1.uCFGC1_BaseAddrCC = (UINT64)pstRqstMsg->OutputFrameAddr_UV;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGC1, cfgc1.u32);
	}

	if (!g_dewarp.enablePrevRqstComparision ||
			(pstRqstMsg->active.width != prvRqstMsg->active.width ||
				hwParams->cisHwin != prvHwParams->cisHwin ||
				ISPSS_BE_DEWARP_GetBitDepth(pstRqstMsg->out_frame_bit_depth) !=
				ISPSS_BE_DEWARP_GetBitDepth(prvRqstMsg->out_frame_bit_depth))) {
		//Configure Stride. For RS, Stride = Number of bytes in a horizontal line of frame.
		hRes_CA = CEIL(pstRqstMsg->active.width, hwParams->cisHwin) * hwParams->cisHwin;
		cfgc2.uCFGC2_StrideCisp =
		(hRes_CA * ISPSS_BE_DEWARP_GetBitDepth(pstRqstMsg->out_frame_bit_depth)) / 8;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGC2, cfgc2.u32);
#ifdef SYNA_ISP_BE_A0
		cfgc7.uCFGC7_JumpStrideCisp = cfgc2.uCFGC2_StrideCisp;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGC7, cfgc7.u32);
#endif
	}

	if (!g_dewarp.enablePrevRqstComparision ||
			(pstRqstMsg->active.height != prvRqstMsg->active.height ||
				hwParams->cisVwin != prvHwParams->cisVwin ||
				pstRqstMsg->in_frame_fmt != prvRqstMsg->in_frame_fmt)) {
		//Configure Number of tile-strides in a Luma frame
		vRes_CA = CEIL(pstRqstMsg->active.height, hwParams->cisVwin) * hwParams->cisVwin;
		cfgc3.uCFGC3_nStridesCisp = vRes_CA / tileVwin;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGC3, cfgc3.u32);
	}

	//Configure DHUB burst size. It can only be 64, 128, or 256
	cfgcrs.uCFGCRS_CispRsNob = ISPSS_BE_DEWARP_DBUB_BURST_SZ;
	cfgcrs.uCFGCRS_CispRsPixLineTot = hRes_CA;
	cfgcrs.uCFGCRS_CispRsEn = 0x1; //Enable CISP RS Path
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGCRS, cfgcrs.u32);

	return ISPSS_OK;
}

void ISPSS_BE_DEWARP_DbgRegDump(void)
{
	int dbg_regval[4], i;

	for (i = 0; i < 4; i++)
		ISPSS_REG_READ32(0xF91C7078+i*4, &dbg_regval[i]);

	DWPLOGT("DBG : %x, %x, %x, %x\n",
			dbg_regval[0], dbg_regval[1], dbg_regval[2], dbg_regval[3]);
}


/***********************************************
 * FUNCTION: Clear TG
 *
 * PARAMS: NONE.
 * RETURN: ISPSS_OK - succeed
 *
 ***********************************************/
INT ISPSS_BE_DEWARP_Reset(void)
{
	UINT32 RegAddr = 0;

	//ISPSS_BE_DEWARP_DbgRegDump();

	RegAddr = DEWARP_LGDC_BASE;
	//Clear DEWARP TG
	ISPSS_REG_WRITE32(RegAddr + RA_LGDC_CFTG, 2);

	//ISPSS_BE_DEWARP_DbgRegDump();
	return ISPSS_OK;
}


/***********************************************
 * FUNCTION: Reset DeWarp
 *
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 ***********************************************/
INT ISPSS_BE_DEWARP_FunctionalReset(struct ISP_BE_RQST_MSG *pstRqstMsg)
{
	UINT32 RegAddr = 0;
	T32CLKRST_FuncReset funcReset;
	struct ISP_BE_BCM *bcmBuf = NULL;
	//static  int dhub_clear_enable = 0;

	bcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);

	/* Dewarp func reset */
	RegAddr = ISPMISC_BASE + RA_IspMISC_clkrst + RA_CLKRST_FuncReset;
	ISPSS_REG_READ32(RegAddr, &funcReset);

	funcReset.uFuncReset_dwrpReset = 0x1;
	ISPSS_BCMBUF_Write(bcmBuf->clear_bcm_buf, RegAddr, funcReset.u32);

	funcReset.uFuncReset_dwrpReset = 0x0;
	ISPSS_BCMBUF_Write(bcmBuf->clear_bcm_buf, RegAddr, funcReset.u32);

	//Dummy write to delay:FIXME
	//bcm_buffer_dummy_write(bcmBuf->clear_bcm_buf, 98);

	RegAddr = DEWARP_LGDC_BASE;
	//Clear DEWARP TG
	ISPSS_BCMBUF_Write(bcmBuf->clear_bcm_buf, RegAddr + RA_LGDC_CFTG, 2);

	return ISPSS_OK;
}


/*********************************************************
 * FUNCTION: Initiation Sequence of DeWarp
BRIEF: At the time of power-on, apply the
top-level reset of this function
after all the clocks are stabilized.
Then, apply this reset on every frame reset
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 *********************************************************/
INT ISPSS_BE_DEWARP_Start(struct ISP_BE_RQST_MSG *pstRqstMsg, struct DEWARP_OBJ *dwpObj)
{
	UINT32 RegAddr = 0;
	T32LGDC_CFTG cftg;
	struct ISP_BE_BCM *pBcmBuf;

	pBcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);

	//Start DIS Order LUT / CIS Matrix Read Client
	RegAddr = DEWARP_LDI_BASE;
	ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf, RegAddr + RA_LDI_RCCmDo + RA_ReadClient_Rd, 2);
	ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf, RegAddr + RA_LDI_RCCmDo + RA_ReadClient_Rd, 0);
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_RCCmDo + RA_ReadClient_Rd, 1);
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_RCCmDo + RA_ReadClient_Rd, 0);

	//Start Write Client in CIS Pixel Write – Data lane - Luma
	RegAddr = DEWARP_LDI_BASE;
	ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf,
			RegAddr + RA_LDI_WCCispY + RA_WriteClient_Wr, 2);
	ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf,
			RegAddr + RA_LDI_WCCispY + RA_WriteClient_Wr, 0);
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
			RegAddr + RA_LDI_WCCispY + RA_WriteClient_Wr, 1);
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf,
			RegAddr + RA_LDI_WCCispY + RA_WriteClient_Wr, 0);

	//Start Write Client in CIS Pixel Write – Data lane - Chroma
	ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf,
			RegAddr + RA_LDI_WCCispC + RA_WriteClient_Wr, 2);
	ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf,
			RegAddr + RA_LDI_WCCispC + RA_WriteClient_Wr, 0);
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_WCCispC + RA_WriteClient_Wr, 1);
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_WCCispC + RA_WriteClient_Wr, 0);

	RegAddr = DEWARP_LGDC_BASE;
	//Clear DEWARP TG
	ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf, RegAddr + RA_LGDC_CFTG, 2);

	//Start DEWARP TG
	cftg.u32 = 0x0;
	cftg.uCFTG_start = 1;
	cftg.uCFTG_clken_stall = 2;

	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LGDC_CFTG, cftg.u32);
	cftg.uCFTG_start = 0;
	ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LGDC_CFTG, cftg.u32);

	return ISPSS_OK;
}


/********************************************************
 * FUNCTION: Submit the BCM and CFGQ buffers to Hardware
 * PARAMS: pstRqstMsg - Request message
 * RETURN: None
 ********************************************************/
INT ISPSS_BE_DEWARP_SubmitHW(struct ISP_BE_RQST_MSG *pstRqstMsg,
					struct DEWARP_OBJ *dwpObj, int qid)
{
	struct ISP_BE_BCM *pBcmBuf;
	//INT dbgLevel = 0;

	pBcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);

#ifdef ISPSS_BE_DEWARP_SUBMIT_DEWARP_QUEUE_12
	// commit to the Q associated with DEWARP interrupt (Qx)
	ISPSS_BCMDHUB_CFGQ_Commit(pBcmBuf->final_bcm_cfgQ, CPCB_1, qid, 0); //12 is generic queue
#else
	ISPSS_BCMDHUB_CFGQ_Commit(pstRqstMsg->final_bcm_cfgQ, CPCB_1, 13);
#endif

	return ISPSS_OK;
}


/***********************************************
 * FUNCTION: Invoke write client for Tile path
 * PARAMS: pstRqstMsg - Request message
 * RETURN: None
 ***********************************************/
static void ISPSS_BE_DEWARP_WrClient_Tile(struct ISP_BE_RQST_MSG *pstRqstMsg,
					struct DEWARP_OBJ *dwpObj)
{
	UINT32 vRes_CA, hRes_CA, vResD, tileHwin, tileVwin;
	UINT32 nWords, packSel;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
		&g_dewarp.lut_ctx.dwp_lgdc[pstRqstMsg->sensorID][active_lut_idx].lutCfg;

	struct ISP_BE_DWP_HW_CONFIG *hwParams = &(dwpLutCfg->hwParams);
	//, *prvHwParams = &(g_dewarp.prvDwpRqst.prvDwpLutCfg->hwParams);
	//struct ISP_BE_RQST_MSG *prvRqstMsg = &(g_dewarp.prvDwpRqst.prvRqstMsg);

	ISPBE_GET_TILE_SIZE(pstRqstMsg->in_frame_fmt, tileHwin, tileVwin);

	//Write Client Configuration for Luma
	hRes_CA = CEIL(pstRqstMsg->active.width, hwParams->cisHwin) * hwParams->cisHwin;
	vRes_CA = CEIL(pstRqstMsg->active.height, hwParams->cisVwin) * hwParams->cisVwin;

	//nWords = (hRes_CA * vRes_CA)/4;
	nWords = (hRes_CA * vRes_CA);
	packSel = ((tileHwin == 8) && (tileVwin == 8)) ? 0 : 1;
	ISPSS_BE_DEWARP_WriteClient(pstRqstMsg, dwpObj, 0, nWords, packSel);

	//Write Client Configuration for Chroma
	vResD = vRes_CA / 2;
	nWords = (ISPSS_BE_DEWARP_GetYUVMode(pstRqstMsg->in_frame_fmt) == MODE_YUV420) ?
					(hRes_CA * vResD) : (hRes_CA * vRes_CA);
	ISPSS_BE_DEWARP_WriteClient(pstRqstMsg, dwpObj, 1, nWords, packSel);

}


/***********************************************
 * FUNCTION: Invoke write client for RS path
 * PARAMS: pstRqstMsg - Request message
 * RETURN: None
 ***********************************************/
static void ISPSS_BE_DEWARP_WrClient_RS(struct ISP_BE_RQST_MSG *pstRqstMsg,
					struct DEWARP_OBJ *dwpObj)
{
	UINT32 vRes_CA, hRes_CA, vResD;
	UINT32 lineBytes, maxLineBytes, nWords;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
		&g_dewarp.lut_ctx.dwp_lgdc[pstRqstMsg->sensorID][active_lut_idx].lutCfg;

	struct ISP_BE_DWP_HW_CONFIG *hwParams = &(dwpLutCfg->hwParams);
	//struct ISP_BE_RQST_MSG *prvRqstMsg = &(g_dewarp.prvDwpRqst.prvRqstMsg);

	DWPLOGT("%s:%d hRes=%d vRes=%d cisH=%d cisV=%d output_bpp=%d\n", __func__, __LINE__,
		pstRqstMsg->active.width, pstRqstMsg->active.height, hwParams->cisHwin,
		hwParams->cisVwin, ISPSS_BE_DEWARP_GetBitDepth(pstRqstMsg->out_frame_bit_depth));
	//Write Client Configuration for Luma
	hRes_CA = CEIL(pstRqstMsg->active.width, hwParams->cisHwin) * hwParams->cisHwin;
	vRes_CA = CEIL(pstRqstMsg->active.height, hwParams->cisVwin) * hwParams->cisVwin;

	lineBytes = (hRes_CA * ISPSS_BE_DEWARP_GetBitDepth(pstRqstMsg->out_frame_bit_depth))/8;
	//dHub_burst_sz 256
	maxLineBytes = CEIL(lineBytes, ISPSS_BE_DEWARP_DBUB_BURST_SZ) *
			ISPSS_BE_DEWARP_DBUB_BURST_SZ;
	nWords = (maxLineBytes * vRes_CA)/4;
	DWPLOGT("%s:%d:Luma size=%x\n", __func__, __LINE__, nWords);
	ISPSS_BE_DEWARP_WriteClient(pstRqstMsg, dwpObj, 0, nWords, 2);

	//Write Client Configuration for Chroma
	vResD = vRes_CA / 2;
	nWords = (ISPSS_BE_DEWARP_GetYUVMode(pstRqstMsg->in_frame_fmt) == MODE_YUV420) ?
					(maxLineBytes * vResD)/4 : (maxLineBytes * vRes_CA)/4;
	DWPLOGT("%s:%d:Chroma size=%x\n", __func__, __LINE__, nWords);
	ISPSS_BE_DEWARP_WriteClient(pstRqstMsg, dwpObj, 1, nWords, 2);

}


/***********************************************
 * FUNCTION: Configure LDI Status registers
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 ***********************************************/
INT ISPSS_BE_DEWARP_ConfigLDI_Status(struct ISP_BE_RQST_MSG *pstRqstMsg,
					struct DEWARP_OBJ *dwpObj)
{
	UINT32 RegAddr = 0, vRes_CA, hRes_CA;
	T32LDI_CFGC5 cfgc5;
	T32LDI_CFGC6 cfgc6;
	//T32LDI_CFGCRS cfgcrs;
	T32LDI_STATUS status;
	struct ISP_BE_BCM *pBcmBuf;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
		&g_dewarp.lut_ctx.dwp_lgdc[pstRqstMsg->sensorID][active_lut_idx].lutCfg;

	struct ISP_BE_DWP_HW_CONFIG *hwParams = &(dwpLutCfg->hwParams);
	struct ISP_BE_DWP_HW_CONFIG *prvHwParams = &(g_dewarp.prvDwpRqst.prvDwpLutCfg->hwParams);
	struct ISP_BE_RQST_MSG *prvRqstMsg = &(g_dewarp.prvDwpRqst.prvRqstMsg);

	pBcmBuf = ISPSS_GET_BCMBUF(pstRqstMsg);

	if (!g_dewarp.enablePrevRqstComparision ||
			(pstRqstMsg->active.height != prvRqstMsg->active.height ||
				hwParams->cisVwin != prvHwParams->cisVwin ||
				hwParams->cisHwin != prvHwParams->cisHwin)) {

		RegAddr = DEWARP_LDI_BASE;

		hRes_CA = CEIL(pstRqstMsg->active.width, hwParams->cisHwin) * hwParams->cisHwin;
		vRes_CA = CEIL(pstRqstMsg->active.height, hwParams->cisVwin) * hwParams->cisVwin;

		cfgc5.u32 = 0x0;
		cfgc5.uCFGC5_nPixCispStride = hRes_CA * hwParams->cisVwin;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGC5, cfgc5.u32);

		cfgc6.u32 = 0x0;
		cfgc6.uCFGC6_nCispStrides = vRes_CA / hwParams->cisVwin;
		ISPSS_BCMBUF_Write(pBcmBuf->bcm_buf, RegAddr + RA_LDI_CFGC6, cfgc6.u32);
	}

	//TODO: Check if this being cleared
	//Clear shortStrideWin, ShortFrameWin
	status.u32 = 0;
	status.uSTATUS_ShortStrideWin = 1;
	status.uSTATUS_ShortFrameWin = 1;
	ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf, RegAddr + RA_LDI_STATUS, status.u32);
	status.uSTATUS_ShortStrideWin = 0;
	status.uSTATUS_ShortFrameWin = 0;
	ISPSS_BCMBUF_Write(pBcmBuf->clear_bcm_buf, RegAddr + RA_LDI_STATUS, status.u32);

	return ISPSS_OK;
}


/***********************************************
 * FUNCTION: Configure LDI
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 ***********************************************/
INT ISPSS_BE_DEWARP_Config_LDI(struct ISP_BE_RQST_MSG *pstRqstMsg, struct DEWARP_OBJ *dwpObj)
{
	//INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	struct ISP_BE_DEWARP_RQST_MSG *dwpParam = &pstRqstMsg->dwp;

	ISPSS_BE_DEWARP_ReadClient(pstRqstMsg, dwpObj);

	ISPSS_BE_DEWARP_DIS_PixRd_Cfg_CmdLane(pstRqstMsg, dwpObj);

	ISPSS_BE_DEWARP_DIS_PixRd_Cfg_DataLane(pstRqstMsg, dwpObj);

	//Section 3.4.8 LDI status
	ISPSS_BE_DEWARP_ConfigLDI_Status(pstRqstMsg, dwpObj);
	//Tile Path
	if (dwpParam->dwp_out_path == CISP_TILE_PATH) {
		DWPLOGT("DEWARP Tile Output Path\n");
		ISPSS_BE_DEWARP_CIS_TilePixWr_Cfg_CmdLane(pstRqstMsg, dwpObj);
		//DIF Config for Luma DIF Configuration for Chroma
		ISPSS_BE_DEWARP_CIS_TilePixWr_Cfg_DataLane(pstRqstMsg, dwpObj);
		//Write Client Configuration for Luma, Chroma
		ISPSS_BE_DEWARP_WrClient_Tile(pstRqstMsg, dwpObj);
	} else
		//RS Path
		if (dwpParam->dwp_out_path == CISP_RS_PATH) {
			DWPLOGT("DEWARP RS Output Path\n");
			ISPSS_BE_DEWARP_CIS_RSPixWr_Cfg_CmdLane(pstRqstMsg, dwpObj);
			//Write Client Configuration for Luma, Chroma
			ISPSS_BE_DEWARP_WrClient_RS(pstRqstMsg, dwpObj);
		}

	return ISPSS_OK;
}


/***********************************************
 * FUNCTION: Check LDI status
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 ***********************************************/
INT ISPSS_BE_DEWARP_Chk_LDI_status(struct ISP_BE_RQST_MSG *pstRqstMsg,
					struct DEWARP_OBJ *dwpObj)
{
	HRESULT Ret = ISPSS_OK;
	UINT32 RegAddr = 0;
	T32LDI_STATUS status;

	RegAddr = DEWARP_LDI_BASE;
	status.u32 = 0;
	ISPSS_REG_READ32(RegAddr + RA_LDI_STATUS, &status.u32);

	switch (status.u32 & 0x3) {
	case 1:
		Ret = ISPSS_DWP_SHORTSTRIDE_WIN;
		break;
	case 2:
		Ret = ISPSS_DWP_SHORTFRAME_WIN;
		break;
	case 3:
		Ret = ISPSS_DWP_SHORT_STRIDE_FRAME_WIN;
		break;
	default:
		Ret = ISPSS_OK;
	}
	return Ret;
}


/***********************************************
 * FUNCTION: Get interrupt status
 * PARAMS: pstRqstMsg - Request message
 * RETURN: 0 - No interrupt
 *         1 - Interrupt
 ***********************************************/
INT ISPSS_BE_DEWARP_FrameInterrupt(struct ISP_BE_RQST_MSG *pstRqstMsg,
					struct DEWARP_OBJ *dwpObj)
{
	int instat;
	struct HDL_semaphore *pSemHandle = dhub_semaphore(&ISPSS_FWR_dhubHandle.dhub);

	DWPLOGT("%s:%d\n", __func__, __LINE__);

	/* check dwp interrupt status to figure out which CPCB interrupt */
	instat = semaphore_chk_full(pSemHandle, -1); //Debug:Uncomment this
	//instat = *(UINT32 *)0xf913040c; //Debug:enable above line

	if (bTST(instat, ispDhubSemMap_FWR_dwrp_intr))
		return 1;
	else
		return 0;
}

/*********************************************************
 * FUNCTION: Generate DIF LUT for DIS
 Reference for this is diflut.pl from DV team
 * PARAMS: pstRqstMsg - Request message
 dif_disLut - pointer to LUT
 * RETURN: dlutcnt - Number of 32 bit words in LUT
 *********************************************************/
INT ISPSS_BE_DEWARP_Create_DIFDisLut(struct ISP_BE_RQST_MSG *pstRqstMsg,
					SHM_HANDLE *hShm, UINT32 **dif_disLut)
{
	int m_range, l_range, k_range, j_range, i_range;
	int m_coeff, l_coeff, k_coeff, j_coeff, i_coeff;
	int i, j, k, l, m;
	int dlutcnt = 0;
	UINT32 disHwin, disVwin, tileHwin, tileVwin; //, value = 0;
	INT iResult = 0;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
		&g_dewarp.lut_ctx.dwp_lgdc[pstRqstMsg->sensorID][active_lut_idx].lutCfg;

	struct ISP_BE_DWP_HW_CONFIG *hwParams = &(dwpLutCfg->hwParams);

	disHwin = hwParams->disHwin;
	disVwin = hwParams->disVwin;
	ISPBE_GET_TILE_SIZE(pstRqstMsg->in_frame_fmt, tileHwin, tileVwin);

	if (tileVwin == 8 && tileHwin == 8) {
		m_range = 1;
		l_range = 1;
		k_range = 1;
		j_range = tileVwin / 2;
		i_range = disHwin / 4;

		m_coeff = 0;
		l_coeff = 0;
		k_coeff = 0;
		j_coeff = 1;
		i_coeff = tileVwin / 2;
	} else if (tileVwin == 6 && tileHwin == 8) {
		m_range = 1;
		l_range = 1;
		k_range = 1;
		j_range = tileVwin / 2;
		i_range = disHwin / 4;

		m_coeff = 0;
		l_coeff = 0;
		k_coeff = 0;
		j_coeff = 1;
		i_coeff = tileVwin / 2;
	} else { //v8h6
		m_range = 1;
		l_range = 1;
		k_range = 1;
		j_range = tileVwin / 4;
		i_range = disHwin / 2;

		m_coeff = 0;
		l_coeff = 0;
		k_coeff = 0;
		j_coeff = 1;
		i_coeff = tileVwin / 4;

	}
	//Allocate memory for LUT//FIXME:
	iResult = ispSS_SHM_Allocate(SHM_NONSECURE, (disHwin * tileVwin * sizeof(UINT32)),
					1024, hShm, SHM_NONSECURE_CONTIG);
	if (iResult != ISPSS_OK) {
		DWPLOGE("DeWarp: memory Allocation Failed\n");
		goto e_AllocateBcmStruct;
	}

	ispSS_SHM_GetVirtualAddress(*hShm, 0, (void **)dif_disLut);

	DWPLOGT("Generating Difdis_lut[%d x %d]\n", disHwin, tileVwin);

	for (m = 0;  m < m_range;  m++) {
		for (l = 0;  l < l_range;  l++) {
			for (k = 0;  k < k_range;  k++) {
				for (j = 0;  j < j_range;  j++) {
					for (i = 0;  i < i_range;  i++) {
						*(*dif_disLut + dlutcnt) =
							(i*i_coeff + j*j_coeff + k*k_coeff +
							 l*l_coeff + m*m_coeff);
						dlutcnt++;
					}
				}
			}
		}
	}
e_AllocateBcmStruct:

	return dlutcnt;
}

/*********************************************************
 * FUNCTION: Generate DIF LUT for CIS
 Reference for this is diflut.pl from DV team
 * PARAMS: pstRqstMsg - Request message
 dif_cisLut - pointer to LUT
 * RETURN: dlutcnt - Number of 32 bit words in LUT
 *********************************************************/
INT ISPSS_BE_DEWARP_Create_DIFCisLut(struct ISP_BE_RQST_MSG *pstRqstMsg,
					SHM_HANDLE *hShm, UINT32 **dif_cisLut)
{
	int k_range, j_range, i_range;
	int k_coeff, j_coeff, i_coeff;
	int dlutcnt = 0;
	int i, j, k;
	UINT32 cisHwin, cisVwin, tileHwin, tileVwin;
	INT iResult = 0;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pstRqstMsg->sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
		&g_dewarp.lut_ctx.dwp_lgdc[pstRqstMsg->sensorID][active_lut_idx].lutCfg;

	struct ISP_BE_DWP_HW_CONFIG *hwParams = &(dwpLutCfg->hwParams);

	cisHwin = hwParams->cisHwin;
	cisVwin = hwParams->cisVwin;
	ISPBE_GET_TILE_SIZE(pstRqstMsg->in_frame_fmt, tileHwin, tileVwin);

	k_range = 1;
	j_range = cisHwin;
	i_range = tileVwin;

	k_coeff = 0;
	j_coeff = 1;
	i_coeff = cisHwin;

	//Allocate memory for LUT
	iResult = ispSS_SHM_Allocate(SHM_NONSECURE, (cisHwin * tileVwin * sizeof(UINT32)),
					1024, hShm, SHM_NONSECURE_CONTIG);
	if (iResult != ISPSS_OK) {
		DWPLOGE("DeWarp: memory Allocation Failed\n");
		goto e_AllocateBcmStruct;
	}

	ispSS_SHM_GetVirtualAddress(*hShm, 0, (void **)dif_cisLut);

	DWPLOGT("Generating Difcis_lut[%d x %d]\n", cisHwin, tileVwin);

	for (k = 0;  k < k_range;  k++) {
		for (j = 0;  j < j_range;  j++) {
			for (i = 0;  i < i_range;  i++) {
				*(*dif_cisLut + dlutcnt) = (i*i_coeff + j*j_coeff + k*k_coeff);
				dlutcnt++;
			}
		}
	}

e_AllocateBcmStruct:

	return dlutcnt;
}
