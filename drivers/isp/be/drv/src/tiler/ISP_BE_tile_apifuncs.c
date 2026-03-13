// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include "ISP_BE_tile_common.h"
#include "ispSS_api_dhub_config.h"
//#include "ispSS_bcmbuf_debug.h"
#include "ispSS_common.h"
#include "ispSS_fe_client.h"
#include "tileproc.h"
#include "ispMISC.h"
#include "ispSS_memmap.h"
#include "Galois_memmap.h"
#include "ispSS_reg.h"
#include "ispBE_common.h"
#include "ispSS_bcmbuf_queue.h"
#include "ispBE_module_common.h"
#include "ISP_BE_tile_api.h"
#include "ISP_BE_tile_module.h"
#include "ISP_BE_tile_apifuncs.h"
#include "ispbe_err.h"
#include "ispbe_debug.h"

#define IS_TILE_FRAME(srcfmt) \
	((srcfmt == ISPSS_SRCFMT_YUV420SP_TILED_V8H8) || \
	 (srcfmt == ISPSS_SRCFMT_YUV420SP_TILED_V8H6) || \
	 (srcfmt == ISPSS_SRCFMT_YUV420SP_TILED_V6H8) || \
	 (srcfmt == ISPSS_SRCFMT_YUV422SP_TILED_V8H8) || \
	 (srcfmt == ISPSS_SRCFMT_YUV422SP_TILED_V8H6) || \
	 (srcfmt == ISPSS_SRCFMT_YUV422SP_TILED_V6H8))

#define IS_YUV420_TILE_FRAME(srcfmt) \
	((srcfmt == ISPSS_SRCFMT_YUV420SP_TILED_V8H8) || \
	 (srcfmt == ISPSS_SRCFMT_YUV420SP_TILED_V8H6) || \
	 (srcfmt == ISPSS_SRCFMT_YUV420SP_TILED_V6H8))

#define IS_YUV422_TILE_FRAME(srcfmt) \
	((srcfmt == ISPSS_SRCFMT_YUV422SP_TILED_V8H8) || \
	 (srcfmt == ISPSS_SRCFMT_YUV422SP_TILED_V8H6) || \
	 (srcfmt == ISPSS_SRCFMT_YUV422SP_TILED_V6H8))

#define ISPSS_BE_TILE_FUNCTION_RESET
#define TILE_BASE (MEMMAP_ISPSS_REG_BASE + ISPSS_MEMMAP_TILE_REG_BASE)
#define ISPMISC_BASE (MEMMAP_ISP_REG_BASE + ISPSS_MEMMAP_GLB_REG_BASE)

/* Start: BCM Configs */
#undef ISPSS_BE_TILE_SUBMIT_BCM_IMMEDIATELY //Submit BCM buffers independent of interrupt

#ifdef ISPSS_BE_TILE_SUBMIT_BCM_IMMEDIATELY
#define ISPSS_BE_TILE_SUBMIT_INDIVIDUAL_BCM_BUFFER //Submit BCM buffers individually
#undef  ISPSS_BE_TILE_SUBMIT_BCM_DIRECT_WRITE //Direct write BCM buffers
#endif

#ifndef ISPSS_BE_TILE_SUBMIT_BCM_IMMEDIATELY
//Define this to write to Tile queue, undefine to write to genric queue
#define ISPSS_BE_TILE_SUBMIT_TILE_QUEUE
#endif
/* End: BCM Configs */

#define ISPSS_BE_TILE_2NDDMA //Undefine to enable 1DDMA

#define ISPSS_BE_TILE_ISR (2)

/* TG Operation modes 0: Fixed window Stride operation 1: Variable window stride operation */
#define ISPSS_BE_TILE_TG_OPMODE (0)
#define H_TOTAL 4400
#define V_TOTAL 2250
//#define HBLANK 560 //20480
//#define VBLANK 90 //16
#define HBLANK 0 //20480 FIXME:Debug purpose only
#define VBLANK 0 //16
#define DIF_LDPH 4
#define DIF_LDPV 2

#define bTST(x, b)                    (((x) >> (b)) & 1)

#define ISP_DEBUG_CALLTRACE "ISPDBG:TILE: "
//#define ISPSS_TILEDBG(...) if (0) pr_err(ISP_DEBUG_CALLTRACE  __VA_ARGS__)
#define ISPSS_TILEDBG(...)  pr_debug(ISP_DEBUG_CALLTRACE  __VA_ARGS__)

T32TILE_CTRL0 tileCtrl0;
T32TILEWRAP_CTRL tileWrapCtrl;
int y_height, uv_height;
/**************************************************************************************
 * FUNCTION: set dummy TG for video.
 * PARAMS: TotRes - input total resolution.
 *                progressive - 1: prgressive, 0: interlaced.
 * RETURN:  ISPSS_OK - succeed.
 *                ISPSS_EBCMBUFFULL -BCM buffer is full.
 ***************************************************************************************/
INT ISPSS_BE_TILE_ConfigDummyTG(struct ISP_BE_TILE_OBJ *tile_obj, struct ISP_BE_RQST_MSG *rqstBuf)
{
	INT RetVal = ISPSS_OK;
	UINT32 RegAddr;
	T32TG_MAIN_SIZE tgMain;
	T32TG_MAIN_HB tgMainHB;
	T32TG_MAIN_VB0 tgMainVB0;
	T32TG_MAIN_HB_Y tgMainY;
	T32TG_MAIN_HB_C tgMainC;
	T32TG_MAIN_HB_OUT tgMainOut;
	T32TG_MAIN_VB0_Y tgMainVB0Y;
	T32TG_MAIN_VB0_C tgMainVB0C;
	T32TG_MAIN_VB0_OUT tgMainVB0OUT;

	ISPSS_TILEDBG("hRes:%x vRes:%x\n", rqstBuf->active.width, rqstBuf->active.height);

	RegAddr = tile_obj->base_addr + RA_TILEPROC_feTG;

	tgMain.uSIZE_Y = rqstBuf->active.height + ISPSS_BE_TILE_VBLANK;
	tgMain.uSIZE_X = rqstBuf->active.width + ISPSS_BE_TILE_HBLANK;
#ifdef SYNA_ISP_BE_A0
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TG_MAINB_SIZE), tgMain.u32);
#else
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TG_MAIN_SIZE), tgMain.u32);
#endif

	/* set the H blank front edge value*/
	tgMainHB.uHB_FE = rqstBuf->active.width + ISPSS_BE_DUMMY_TG_HB_FE_OFF;
	tgMainHB.uHB_BE = ISPSS_BE_DUMMY_TG_HB_BE;
#ifdef SYNA_ISP_BE_A0
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TG_MAINB_HB), tgMainHB.u32);
#else
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TG_MAIN_HB), tgMainHB.u32);
#endif

	tgMainOut.uHB_OUT_FE = rqstBuf->active.width + ISPSS_BE_DUMMY_TG_HB_FE_OFF;
	tgMainOut.uHB_OUT_BE = ISPSS_BE_DUMMY_TG_HB_BE;
#ifdef SYNA_ISP_BE_A0
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf,
			(RegAddr + RA_TG_MAINB_HB_OUT), tgMainOut.u32);
#else
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf,
			(RegAddr + RA_TG_MAIN_HB_OUT), tgMainOut.u32);
#endif

	tgMainY.uHB_Y_FE = rqstBuf->active.width + ISPSS_BE_DUMMY_TG_HB_FE_OFF;
	tgMainY.uHB_Y_BE = ISPSS_BE_DUMMY_TG_HB_BE;
#ifdef SYNA_ISP_BE_A0
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TG_MAINB_HB_Y), tgMainY.u32);
#else
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TG_MAIN_HB_Y), tgMainY.u32);
#endif

	tgMainC.uHB_C_FE = rqstBuf->active.width + ISPSS_BE_DUMMY_TG_HB_FE_OFF;
	tgMainC.uHB_C_BE = ISPSS_BE_DUMMY_TG_HB_BE;
#ifdef SYNA_ISP_BE_A0
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TG_MAINB_HB_C), tgMainC.u32);
#else
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TG_MAIN_HB_C), tgMainC.u32);
#endif

	/* set the V blank front edge value */
	tgMainVB0.uVB0_FE = rqstBuf->active.height + ISPSS_BE_DUMMY_TG_VB0_FE_OFF;
	tgMainVB0.uVB0_BE = ISPSS_BE_DUMMY_TG_VB0_BE;
#ifdef SYNA_ISP_BE_A0
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TG_MAINB_VB0), tgMainVB0.u32);
#else
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TG_MAIN_VB0), tgMainVB0.u32);
#endif

	tgMainVB0Y.uVB0_Y_FE = rqstBuf->active.height + ISPSS_BE_DUMMY_TG_VB0_FE_OFF;
	tgMainVB0Y.uVB0_Y_BE = ISPSS_BE_DUMMY_TG_VB0_BE;
#ifdef SYNA_ISP_BE_A0
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf,
			(RegAddr + RA_TG_MAINB_VB0_Y), tgMainVB0Y.u32);
#else
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf,
			(RegAddr + RA_TG_MAIN_VB0_Y), tgMainVB0Y.u32);
#endif

	tgMainVB0C.uVB0_C_FE = rqstBuf->active.height + ISPSS_BE_DUMMY_TG_VB0_FE_OFF;
	tgMainVB0C.uVB0_C_BE = ISPSS_BE_DUMMY_TG_VB0_BE;
#ifdef SYNA_ISP_BE_A0
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf,
			(RegAddr + RA_TG_MAINB_VB0_C), tgMainVB0C.u32);
#else
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf,
			(RegAddr + RA_TG_MAIN_VB0_C), tgMainVB0C.u32);
#endif

	tgMainVB0OUT.uVB0_OUT_FE = rqstBuf->active.height + ISPSS_BE_DUMMY_TG_VB0_FE_OFF;
	tgMainVB0OUT.uVB0_OUT_BE = ISPSS_BE_DUMMY_TG_VB0_BE;
#ifdef SYNA_ISP_BE_A0
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf,
					(RegAddr + RA_TG_MAINB_VB0_OUT), tgMainVB0OUT.u32);
#else
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf,
					(RegAddr + RA_TG_MAIN_VB0_OUT), tgMainVB0OUT.u32);
#endif

	return RetVal;
}


INT ISPSS_BE_TILE_ConfigTile(struct ISP_BE_TILE_OBJ *tile_obj, struct ISP_BE_RQST_MSG *rqstBuf)
{
	INT RetVal = ISPSS_OK;
	UINT32 RegAddr = 0;
	T32TILE_CTRL1 tileCtrl1;
	T32TILE_CTRL3 tileCtrl3;
	T32TILE_CTRL4 tileCtrl4;
	struct ISP_BE_TILE_RQST_MSG *tileRqst = &(rqstBuf->tile);

	RegAddr = tile_obj->base_addr + RA_TILEPROC_TILE;

	tileCtrl0.u32 = 0;
	/*
	 * CTRL0 is used to start ISP data input and feTg.
	 * Also to enable rs2tile block. Do this in Start function
	 */
	if (rqstBuf->tile.isRsToTile)
		tileCtrl0.uCTRL0_tiler_en = 1;
	else
		tileCtrl0.uCTRL0_tiler_en = 0;

	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TILE_CTRL0), tileCtrl0.u32);

	/* CTRL1 Configurations */
	//idata_fifo_sel
	ISPSS_REG_READ32(RegAddr + RA_TILE_CTRL1, &tileCtrl1.u32);
	if ((rqstBuf->tile.dataAlignment == TILE_LSB_DATA_ALIGN &&
			rqstBuf->in_frame_bit_depth == ISPSS_BIT_DEPTH_8BIT) &&
			(rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV420SP ||
			 rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV422SP ||
			 rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV444P))
		tileCtrl1.uCTRL1_idata_fifo_sel = 0x1;
	else if (rqstBuf->in_frame_bit_depth == ISPSS_BIT_DEPTH_10BIT &&
			(rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV420SP ||
			 rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV422SP))
		tileCtrl1.uCTRL1_idata_fifo_sel = 0x2;
	else if ((rqstBuf->tile.dataAlignment == TILE_MSB_DATA_ALIGN &&
			rqstBuf->in_frame_bit_depth == ISPSS_BIT_DEPTH_8BIT) &&
			(rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV420SP ||
			rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV422SP ||
			rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV444P))
		tileCtrl1.uCTRL1_idata_fifo_sel = 0x3;
	else
		tileCtrl1.uCTRL1_idata_fifo_sel = 0x0;


	//read_en_422SP
	if (rqstBuf->tile.isRsToTile && (rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV420SP))
		tileCtrl1.uCTRL1_read_en_422SP = 0x0;
	else //if no rs2tile or YUV422/YUV444/RGB888
		tileCtrl1.uCTRL1_read_en_422SP = 0x1;

	if (rqstBuf->in_frame_bit_depth == ISPSS_BIT_DEPTH_10BIT)
		tileCtrl1.uCTRL1_idata_sel = 0x2;
	else if (rqstBuf->in_frame_bit_depth == ISPSS_BIT_DEPTH_8BIT)
		tileCtrl1.uCTRL1_idata_sel = 0x3;

	//Keep default values for rd_initval0, rd_initval1, rd_mask_sftrst, read_sel, crop_de_en
	//Currently bitmap30_en disabled

	//tile_width; Number of pixels in each line of a tile
	tileCtrl1.uCTRL1_tile_width = tileRqst->tileHwin; //Supported values are 8 and 6
	//tile_depth; Number of pixels in each line of a tile
	tileCtrl1.uCTRL1_tile_depth = tileRqst->tileVwin; //Supported values are 8 and 6

	//vsync_cnt_val; Number of vsync's at ISP before start receiving the data
	tileCtrl1.uCTRL1_vsync_cnt_val = VBLANK;

	tileCtrl1.uCTRL1_clken_ctrl0 = (tileRqst->TileFalseInt & 1) ? 1 : 0;
	tileCtrl1.uCTRL1_clken_ctrl1 = (tileRqst->TileFalseInt & (1 << 1)) ? 1 : 0;
	tileCtrl1.uCTRL1_clken_ctrl2 = (tileRqst->TileFalseInt & (1 << 2)) ? 1 : 0;
	tileCtrl1.uCTRL1_rd_initval0 = 0;
	tileCtrl1.uCTRL1_rd_initval1 = 1;

	tileCtrl1.uCTRL1_rd_mask_sftrst = 0;
	tileCtrl1.uCTRL1_read_sel = 0;
	if ((rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV420SP) &&
		((rqstBuf->active.height/2) % tileRqst->tileVwin == 0)) {
		//Enable crop or drop of line for 420sp case (drop alternate lines)
		tileCtrl1.uCTRL1_crop_de_en = 1;
	} else {
		tileCtrl1.uCTRL1_crop_de_en = 0;
	}
	tileCtrl1.uCTRL1_bitmap30_en = 0;
	tileCtrl1.uCTRL1_tgEof_en = 0;
	//Default value for tgEof_en; Do not reset start mask with tgEof
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TILE_CTRL1), tileCtrl1.u32);

	/* CTRL3 Configurations */
	/*
	 * inPix_tot;Specifies the total number of
	 * pixels expected at the input of isp_async_fifo0.
	 * should be programmed with (ivres*ihres)
	 */
	tileCtrl3.uCTRL3_inPix_tot = rqstBuf->active.width * rqstBuf->active.height;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TILE_CTRL3), tileCtrl3.u32);

	/* CTRL4 Configurations */
	ISPSS_REG_READ32(RegAddr + RA_TILE_CTRL4, &tileCtrl4.u32);
	//xres; Number of pixels in a frame line
	tileCtrl4.uCTRL4_xres = rqstBuf->active.width;
	//yres;  Number of lines in a frame
	tileCtrl4.uCTRL4_yres = rqstBuf->active.height;
	// uv_mask_disable, uv_mask_val0, uv_mask_val1
	// uv_swap_reg;Control bit to swap U & V during tiling.
	// 1 – Swap U & V, 0 – U & V in same order as in input interface
	tileCtrl4.uCTRL4_uv_swap_reg = (rqstBuf->tile.isSwapEnable) ? 1:0;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TILE_CTRL4), tileCtrl4.u32);

	/*Write Client Config*/
	RegAddr = tile_obj->base_addr + RA_TILEPROC_TILEWRAP + RA_TILEWRAP_CTRL;

	ISPSS_REG_READ32(RegAddr, &tileWrapCtrl.u32);
	if ((rqstBuf->in_frame_bit_depth == ISPSS_BIT_DEPTH_8BIT) &&
			((rqstBuf->out_frame_fmt == ISPSS_SRCFMT_YUV420SP_TILED_V8H8) ||
			(rqstBuf->out_frame_fmt == ISPSS_SRCFMT_YUV422SP_TILED_V8H8))) {
		tileWrapCtrl.uCTRL_chroma_data_sel = 1;
		tileWrapCtrl.uCTRL_luma_data_sel = 1;
	} else {
		tileWrapCtrl.uCTRL_chroma_data_sel = 0;
		tileWrapCtrl.uCTRL_luma_data_sel = 0;
	}
	tileWrapCtrl.uCTRL_420sp_wrbk = 1;

	RetVal = ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, RegAddr, tileWrapCtrl.u32);

	/* Status register use this in status checking */
	return RetVal;
}


/***************************************************************************************
 * FUNCTION: clear all the FIFOs and pipline data inside the channel.
 * PARAMS:
 * RETURN:  ISPSS_OK - succeed.
 *                ISPSS_EBADPARAM - bad input parameters.
 *                ISPSS_EBCMBUFFULL -BCM buffer is full.
 ***************************************************************************************/
INT ISPSS_BE_TILE_WRClearChannel(struct ISP_BE_TILE_OBJ *tile_obj, struct ISP_BE_RQST_MSG *rqstBuf)
{
	UINT32 RegAddr;
	INT RetVal = ISPSS_OK;

	RegAddr = tile_obj->base_addr + RA_TILEPROC_TILEWRAP + RA_TILEWRAP_CTRL;

	tileWrapCtrl.uCTRL_wr0_clear = 1;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->clear_bcm_buf, RegAddr, tileWrapCtrl.u32);
	tileWrapCtrl.uCTRL_wr0_clear = 0;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->clear_bcm_buf, RegAddr, tileWrapCtrl.u32);

	tileWrapCtrl.uCTRL_wr1_clear = 1;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->clear_bcm_buf, RegAddr, tileWrapCtrl.u32);
	tileWrapCtrl.uCTRL_wr1_clear = 0;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->clear_bcm_buf, RegAddr, tileWrapCtrl.u32);

	return RetVal;
}


/***************************************************************************************
 * FUNCTION: enable the channel.
 * PARAMS:  Channel - id of a front-end dataloader channel.
 *                Enable - 1:enable, 0:no effect.
 * RETURN:  ISPSS_OK - succeed.
 *                ISPSS_EBADPARAM - bad input parameters.
 *                ISPSS_EBCMBUFFULL -BCM buffer is full.
 ***************************************************************************************/
INT ISPSS_BE_TILE_WREnableChannel(struct ISP_BE_TILE_OBJ *tile_obj,
			struct ISP_BE_RQST_MSG *rqstBuf)
{
	UINT32 RegAddr;
	INT RetVal = ISPSS_OK;

	RegAddr = tile_obj->base_addr + RA_TILEPROC_TILEWRAP + RA_TILEWRAP_CTRL;

	tileWrapCtrl.uCTRL_wr0_start = 1;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, RegAddr, tileWrapCtrl.u32);
	tileWrapCtrl.uCTRL_wr0_start = 0;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, RegAddr, tileWrapCtrl.u32);

	tileWrapCtrl.uCTRL_wr1_start = 1;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, RegAddr, tileWrapCtrl.u32);
	tileWrapCtrl.uCTRL_wr1_start = 0;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, RegAddr, tileWrapCtrl.u32);

	return RetVal;
}

INT ISPSS_BE_TILE_DhubParams(struct ISPSS_DHUB_CFG_2NDDMA_PARAM *pCfg2NDDMAParam,
			INT outfmt, INT in_frame_bit_depth)
{
	//Dehub parametrs for all supported output formats
	return 0;
}


INT ISPSS_BE_TILE_ConfigWriteClient(struct ISP_BE_TILE_OBJ *tile_obj,
			struct ISP_BE_RQST_MSG *rqstBuf)
{
	INT RetVal = ISPSS_OK;
	struct ISPSS_DHUB_CFG_2NDDMA_PARAM stCfg2NDDMAParam;
	INT *cfgQ = NULL;
	INT *cfgQ_shadow =  (unsigned int *)rqstBuf->pBcmBuf->dma_cfgQ->addr; //FIXME Rohit
	INT *cfgQ_len = &(rqstBuf->pBcmBuf->dma_cfgQ->len);
	int in_frame_bit_depth;
	int WidthInBytes;

	y_height = rqstBuf->active.height;
	if ((rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV420SP) ||
		(rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV422SP)) {
		uv_height = (rqstBuf->in_frame_fmt == ISPSS_SRCFMT_YUV420SP) ?
					rqstBuf->active.height/2 :  rqstBuf->active.height;
		if (rqstBuf->in_frame_bit_depth == ISPSS_BIT_DEPTH_8BIT) {
			in_frame_bit_depth = 8;
			WidthInBytes = (rqstBuf->active.width*in_frame_bit_depth)/8;
		} else {
			in_frame_bit_depth = 10;
			if (rqstBuf->tile.isRsToTile)
				WidthInBytes = ((rqstBuf->active.width*in_frame_bit_depth)/8)*16/15;
			else
				WidthInBytes = ((rqstBuf->active.width*in_frame_bit_depth)/8);
		}
	} else {
		// ISPSS_SRCFMT_YUV444P | TILE_RGB_MODE_RGB888
		uv_height = 0;
		in_frame_bit_depth = 8;
		WidthInBytes = (rqstBuf->active.width*3);
	}
	/*Configure write client*/
	ISPSS_FE_WR_CLIENT_ClientConfig(rqstBuf->pBcmBuf->bcm_buf, ISP_TILE_WR0,
		rqstBuf->out_frame_fmt, rqstBuf->active.width, y_height, in_frame_bit_depth);
	if (uv_height) {
		//uv client or dhub for only yuv-sp format
		ISPSS_FE_WR_CLIENT_ClientConfig(rqstBuf->pBcmBuf->bcm_buf, ISP_TILE_WR1,
		rqstBuf->out_frame_fmt, rqstBuf->active.width, uv_height, in_frame_bit_depth);
	}

	/* Configure Dhub */
	cfgQ = cfgQ_shadow + *cfgQ_len * 2;

	//TODO: Write now configured only for YUV 422/420 format, configure for all the outputs
	//ISPSS_BE_TILE_DhubParams() Use this
	stCfg2NDDMAParam.pBcmBuf = rqstBuf->pBcmBuf->bcm_buf;
	stCfg2NDDMAParam.dhubId = (UINT64)(&ISPSS_TSB_dhubHandle);
	stCfg2NDDMAParam.pdhubHandle = &ISPSS_TSB_dhubHandle;
	stCfg2NDDMAParam.dmaId  = ispDhubChMap_TSB_Tile_W0;
	stCfg2NDDMAParam.addr   = (unsigned long)rqstBuf->OutputFrameAddr_Y;
	stCfg2NDDMAParam.width  = WidthInBytes;
	stCfg2NDDMAParam.stride  = stCfg2NDDMAParam.width;
	stCfg2NDDMAParam.height = y_height;
	stCfg2NDDMAParam.cfgQ   = cfgQ;
	stCfg2NDDMAParam.mtrREn = 0;

	/*Program 2NDDMA For Write Client WR0*/
	*cfgQ_len += ISPSS_DhubCfg2NDDMA(&stCfg2NDDMAParam);
	ISPSS_TILEDBG("addr %lx, width %x, stride %x, y_height %x\n",
			(unsigned long)stCfg2NDDMAParam.addr, stCfg2NDDMAParam.width,
			stCfg2NDDMAParam.stride, stCfg2NDDMAParam.height);
	cfgQ = cfgQ_shadow + *cfgQ_len * 2;

	if (uv_height) {
		//uv client or dhub for only yuv-sp format
		stCfg2NDDMAParam.pBcmBuf = rqstBuf->pBcmBuf->bcm_buf;
		stCfg2NDDMAParam.dhubId = (UINT64)(&ISPSS_TSB_dhubHandle);
		stCfg2NDDMAParam.pdhubHandle = &ISPSS_TSB_dhubHandle;
		stCfg2NDDMAParam.dmaId  = ispDhubChMap_TSB_Tile_W1;
		stCfg2NDDMAParam.addr   = (unsigned long)rqstBuf->OutputFrameAddr_UV;
		stCfg2NDDMAParam.width  = WidthInBytes;
		stCfg2NDDMAParam.stride  = stCfg2NDDMAParam.width;
		stCfg2NDDMAParam.height = uv_height;
		stCfg2NDDMAParam.cfgQ   = cfgQ;
		stCfg2NDDMAParam.mtrREn = 0;

		/*Program 2NDDMA For write Client WR1*/
		*cfgQ_len += ISPSS_DhubCfg2NDDMA(&stCfg2NDDMAParam);
		cfgQ = cfgQ_shadow + *cfgQ_len * 2;
		ISPSS_TILEDBG("addr %lx, width %x, stride %x, uv_height %x\n",
				(unsigned long)stCfg2NDDMAParam.addr, stCfg2NDDMAParam.width,
				stCfg2NDDMAParam.stride, stCfg2NDDMAParam.height);
	}
	/* reset the write clients */
	ISPSS_BE_TILE_WRClearChannel(tile_obj, rqstBuf);

	return RetVal;
}


/***********************************************
 * FUNCTION: Reset Tile
 *
 * PARAMS: psttileRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 ***********************************************/
INT ISPSS_BE_TILE_Reset(struct ISP_BE_RQST_MSG *rqstBuf)
{
	UINT32 RegAddr = 0;

	T32CLKRST_FuncReset funcReset;

	RegAddr = ISPMISC_BASE + RA_IspMISC_clkrst + RA_CLKRST_FuncReset;

#ifdef ISPSS_BE_TILE_FUNCTION_RESET
	/* Tile func reset */
	RegAddr = ISPMISC_BASE + RA_IspMISC_clkrst + RA_CLKRST_FuncReset;
	ISPSS_REG_READ32(RegAddr, &funcReset);

	funcReset.uFuncReset_tileReset = 0x1;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->clear_bcm_buf, RegAddr, funcReset.u32);

	funcReset.uFuncReset_tileReset = 0x0;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->clear_bcm_buf, RegAddr, funcReset.u32);

	//Dummy write to delay:FIXME
	//bcm_buffer_dummy_write(rqstBuf->pBcmBuf->clear_bcm_buf, 98);

#else
	/* Tile func reset */
	ISPSS_REG_READ32(RegAddr, &funcReset);

	funcReset.uFuncReset_tileReset = 0x1;
	ISPSS_REG_WRITE32(RegAddr, funcReset.u32);

	funcReset.uFuncReset_tileReset = 0x0;
	ISPSS_REG_WRITE32(RegAddr, funcReset.u32);
#endif

	return ISPSS_OK;
}


/*********************************************************
 * FUNCTION: Initiation Sequence of Tile
BRIEF: At the time of power-on, apply the
top-level reset of this function
after all the clocks are stabilized.
Then, apply this reset on every frame reset
 * PARAMS: psttileRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADPARAM - Bad parameter
 *********************************************************/
INT ISPSS_BE_TILE_Start(struct ISP_BE_TILE_OBJ *tile_obj, struct ISP_BE_RQST_MSG *rqstBuf)
{
	UINT32 RegAddr = 0;

	RegAddr = tile_obj->base_addr + RA_TILEPROC_TILE;

	//Start the Write Clients
	ISPSS_BE_TILE_WREnableChannel(tile_obj, rqstBuf);

	/* CTRL0 is used to start ISP data input and feTg. Also to enable rs2tile block */
	tileCtrl0.uCTRL0_isp_start = 0x1;
	ISPSS_BCMBUF_Write(rqstBuf->pBcmBuf->bcm_buf, (RegAddr + RA_TILE_CTRL0), tileCtrl0.u32);

	return ISPSS_OK;
}


/********************************************************
 * FUNCTION: Submit the BCM and CFGQ buffers to Hardware
 * PARAMS: psttileRqstMsg - Request message
 * RETURN: None
 ********************************************************/
INT ISPSS_BE_TILE_SubmitHW(struct ISP_BE_TILE_OBJ *tile_obj,
			struct ISP_BE_RQST_MSG *rqstBuf, int dbgLevel)
{
	struct ISP_BE_BCM *pBcmBuf;
	//    BCMBUF dmaBcmBuf, finalBcmBuf;
	pBcmBuf = rqstBuf->pBcmBuf;
	//    INT instat;//Debug:FIXME

	//Generate cfgQ
	ISPSS_BCMBUF_To_CFGQ(pBcmBuf->clear_bcm_buf, pBcmBuf->final_bcm_cfgQ);
	ISPSS_BCMBUF_To_CFGQ(pBcmBuf->bcm_buf, pBcmBuf->final_bcm_cfgQ);
	ISPSS_CFGQ_To_CFGQ(pBcmBuf->dma_cfgQ, pBcmBuf->final_bcm_cfgQ);

#ifdef ISPSS_BE_TILE_SUBMIT_BCM_IMMEDIATELY
#ifdef ISPSS_BE_TILE_SUBMIT_INDIVIDUAL_BCM_BUFFER //Commit to hardware - immediately
#ifndef ISPSS_BE_TILE_SUBMIT_BCM_DIRECT_WRITE //Direct write BCM buffers
	ISPSS_BCMBUF_Create(&dmaBcmBuf, BCM_BUFFER_SIZE);
	ISPSS_BCMBUF_Reset(&dmaBcmBuf);
	ISPSS_BCMBUF_Select(&dmaBcmBuf, CPCB_1);

	if (dbgLevel & 0x1F)//TODO: remove these
		ISPSS_BCMBUF_LogPrint(pBcmBuf->clear_bcm_buf);

	instat = *(UINT32 *)0xF910440C;
	ISPSS_TILEDBG("%s:%d:instat=%x\n", __func__, __LINE__, instat);
	ISPSS_BCMBUF_HardwareTrans(pBcmBuf->clear_bcm_buf, 1);//ISPSS_BCMBUF_DirectWrite
	if (dbgLevel & 0x1F)
		ISPSS_BCMBUF_LogPrint(pBcmBuf->bcm_buf);

	instat = *(UINT32 *)0xF910440C;
	ISPSS_TILEDBG("%s:%d:instat=%x\n", __func__, __LINE__, instat);
	ISPSS_BCMBUF_HardwareTrans(pBcmBuf->bcm_buf, 1);//ISPSS_BCMBUF_DirectWrite
	if (dbgLevel & 0x1F)
		ISPSS_CFGQ_LogPrint(pBcmBuf->dma_cfgQ);

	ISPSS_CFGQ_To_BCMBUF(pBcmBuf->dma_cfgQ, &dmaBcmBuf);
	if (dbgLevel & 0x1F)
		ISPSS_BCMBUF_LogPrint(&dmaBcmBuf);

	instat = *(UINT32 *)0xF910440C;
	ISPSS_TILEDBG("%s:%d:instat=%x\n", __func__, __LINE__, instat);
	ISPSS_BCMBUF_HardwareTrans(&dmaBcmBuf, 1);//ISPSS_BCMBUF_DirectWrite these should come 1st
	ISPSS_BCMBUF_Create(&dmaBcmBuf, BCM_BUFFER_SIZE);
	ISPSS_BCMBUF_Reset(&dmaBcmBuf);
	ISPSS_BCMBUF_Select(&dmaBcmBuf, CPCB_1);

	if (dbgLevel & 0x1F)//TODO: remove these
		ISPSS_BCMBUF_LogPrint(pBcmBuf->clear_bcm_buf);
	ISPSS_BCMBUF_DirectWrite(pBcmBuf->clear_bcm_buf);
	if (dbgLevel & 0x1F)
		ISPSS_BCMBUF_LogPrint(pBcmBuf->bcm_buf);
	ISPSS_BCMBUF_DirectWrite(pBcmBuf->bcm_buf);
	//ISPSS_CFGQ_To_BCMBUF(pBcmBuf->dma_cfgQ, &dmaBcmBuf);
	//ISPSS_BCMBUF_DirectWrite(&dmaBcmBuf);
	if (dbgLevel & 0x1F)
		ISPSS_CFGQ_LogPrint(pBcmBuf->dma_cfgQ);
	ISPSS_CFGQ_DirectWrite(pBcmBuf->dma_cfgQ);
#endif
#else
	//Convert CFGQ to BCMBUF and ISPSS_BCMBUF_HardwareTrans
	ISPSS_CFGQ_To_BCMBUF(pBcmBuf->final_bcm_cfgQ, &finalBcmBuf);
#ifndef ISPSS_BE_TILE_SUBMIT_BCM_DIRECT_WRITE //Direct write BCM buffers
	//Submit Final/Single buffer - and block
	ISPSS_BCMBUF_HardwareTrans(&finalBcmBuf, 1);
#else
	ISPSS_BCMBUF_DirectWrite(&finalBcmBuf);//2nd
#endif
#endif
#else
#ifdef ISPSS_BE_TILE_SUBMIT_TILE_QUEUE
	// commit to the Q associated with TILE interrupt (Qx)
	ISPSS_BCMDHUB_CFGQ_Commit(pBcmBuf->final_bcm_cfgQ, CPCB_1, 12, 1); //12 is generic queue
#else
	ISPSS_BCMDHUB_CFGQ_Commit(psttileRqstMsg->final_bcm_cfgQ, CPCB_1, 13, 1);
#endif
#endif

	return ISPSS_OK;
}

/********************************************************************
 * FUNCTION: Get Underflow/Overflow status. Clear respective registers
 * PARAMS: clrFlag - Clear/Show
 * RETURN: None
 *
 ********************************************************************/
int ISPSS_BE_TILE_UnderflowOverflow_status(struct ISP_BE_TILE_OBJ *tile_obj, INT clrFlag)
{
	static INT underflow_count, overflow_count;
	T32TILE_status status;
	T32TILE_CTRL0 tile_ctrl0;
	UINT32 RegAddr = 0;
	int ret = 0;

	/* Tile func reset */
	RegAddr = tile_obj->base_addr + RA_TILEPROC_TILE;

	ISPSS_REG_READ32(RegAddr + RA_TILE_status, &status.u32);

	if (status.ustatus_isp_fifo_underflow) {
		underflow_count++;
		ret += underflow_count;
		pr_debug("TILE: UF Count=%d\n", underflow_count);
	}
	if (status.ustatus_isp_fifo_overflow) {
		overflow_count++;
		pr_debug("TILE: OF Count=%d\n", overflow_count);
		ret +=  overflow_count;
	}
	if (clrFlag != ISPSS_BE_TILE_ISR)
		pr_debug("TILE: UF Count=%d OF Count=%d\n", underflow_count, overflow_count);

	/* If clrFlag is set, clear the FIFO*/
	if (clrFlag) {
		pr_debug("TILE: Clearing FIFO\n");
		underflow_count = 0;
		overflow_count = 0;
		ISPSS_REG_READ32(RegAddr + RA_TILE_CTRL0, &tile_ctrl0.u32);
		/* Clear the FIFO */
		tile_ctrl0.uCTRL0_isp_fifo_clear1 = 1;
		ISPSS_REG_WRITE32(RegAddr + RA_TILE_CTRL0, tile_ctrl0.u32);

		/* Reset the bit to 0.*/
		tile_ctrl0.uCTRL0_isp_fifo_clear1 = 0;
		ISPSS_REG_WRITE32(RegAddr + RA_TILE_CTRL0, tile_ctrl0.u32);
	}
	return ret;
}


/***********************************************
 * FUNCTION: Get interrupt status
 * PARAMS: psttileRqstMsg - Request message
 * RETURN: 0 - No interrupt
 *         1 - Interrupt
 ***********************************************/
INT ISPSS_BE_TILE_FrameInterrupt(struct ISP_BE_TILE_OBJ *tile_obj, struct ISP_BE_RQST_MSG *rqstBuf)
{
	int instat;
	struct HDL_semaphore *pSemHandle = dhub_semaphore(&ISPSS_TSB_dhubHandle.dhub);
	/* check tile interrupt status to figure out which CPCB interrupt */
	instat = semaphore_chk_full(pSemHandle, -1); //Debug:Uncomment this
	//    instat = *(UINT32 *)0xF910440C;//Debug:enable above line

	ISPSS_BE_TILE_UnderflowOverflow_status(tile_obj, ISPSS_BE_TILE_ISR);

	if (bTST(instat, ispDhubSemMap_TSB_tile_intr))
		return 1;
	else
		return 0;
}

