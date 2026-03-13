/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __TILE_APIFUNCS_H__
#define __TILE_APIFUNCS_H__

/*the offset plus with input resolution to set dummy TG size*/
#define ISPSS_BE_TILE_HBLANK 24
#define ISPSS_BE_TILE_VBLANK 10

//Values are taken from ROT
/*dummy TG HSync*/
#define ISPSS_BE_DUMMY_TG_HS_FE 1
#define ISPSS_BE_DUMMY_TG_HS_BE 3

/*dummy TG HBlank*/
#define ISPSS_BE_DUMMY_TG_HB_FE_OFF 8 //ISPSS_BE_DUMMY_TG_HB_BE + 1
#define ISPSS_BE_DUMMY_TG_HB_BE     7 //ISPSS_BE_DUMMY_TG_HS_BE + 4

/*dummy TG VSync0*/
#define ISPSS_BE_DUMMY_TG_VS0_FE 1
#define ISPSS_BE_DUMMY_TG_VS0_BE 2

/*dummy TG VBlank*/
#define ISPSS_BE_DUMMY_TG_VB0_FE_OFF 1//ISPSS_BE_DUMMY_TG_VS0_BE + 1
#define ISPSS_BE_DUMMY_TG_VB0_BE     0 //ISPSS_BE_DUMMY_TG_VS0_BE

enum TILE_OBJ_STATUS_T {
	ISP_BE_TILE_OBJ_STATUS_INACTIVE,
	ISP_BE_TILE_OBJ_STATUS_ACTIVE,
};

enum TILE_HW_STATUS_T {
	TILE_HW_STATUS_IDLE,
	TILE_HW_STATUS_RUNNING,
};

INT ISPSS_BE_TILE_ConfigDummyTG(struct ISP_BE_TILE_OBJ *tile_obj, struct ISP_BE_RQST_MSG *rqstBuf);
INT ISPSS_BE_TILE_ConfigTile(struct ISP_BE_TILE_OBJ *tile_obj, struct ISP_BE_RQST_MSG *rqstBuf);
INT ISPSS_BE_TILE_ConfigWriteClient(struct ISP_BE_TILE_OBJ *tile_obj,
					struct ISP_BE_RQST_MSG *rqstBuf);
INT ISPSS_BE_TILE_Start(struct ISP_BE_TILE_OBJ *tile_obj, struct ISP_BE_RQST_MSG *rqstBuf);
INT ISPSS_BE_TILE_SubmitHW(struct ISP_BE_TILE_OBJ *tile_obj,
					struct ISP_BE_RQST_MSG *rqstBuf, INT dbgLevel);
INT ISPSS_BE_TILE_FrameInterrupt(struct ISP_BE_TILE_OBJ *tile_obj,
					struct ISP_BE_RQST_MSG *rqstBuf);
INT ISPSS_BE_TILE_UnderflowOverflow_status(struct ISP_BE_TILE_OBJ *tile_obj, INT clrFlag);
INT ISPSS_BE_TILE_Reset(struct ISP_BE_RQST_MSG *rqstBuf);
#endif
