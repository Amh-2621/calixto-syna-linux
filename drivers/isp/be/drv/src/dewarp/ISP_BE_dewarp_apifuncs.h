/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __DEWARP_APIFUNCS_H__
#define __DEWARP_APIFUNCS_H__

enum DEWARP_OBJ_STATUS_T {
	DEWARP_OBJ_STATUS_INACTIVE,
	DEWARP_OBJ_STATUS_ACTIVE,
};

enum DEWARP_HW_STATUS_T {
	DEWARP_HW_STATUS_IDLE,
	DEWARP_HW_STATUS_RUNNING,
};

enum DEWARP_SW_ALGO_STATUS_T {
	DEWARP_SW_ALGO_STATUS_IDLE,
	DEWARP_SW_ALGO_STATUS_RUNNING,
};

struct ISP_BE_RQST_MSG_T;
struct ISP_BE_DEWARP_OBJ_T;

INT ISPSS_BE_DEWARP_Config_LGDC_TG(struct ISP_BE_RQST_MSG *pstRqstMsg, struct DEWARP_OBJ *dwpObj);
INT ISPSS_BE_DEWARP_Config_LGDC(struct ISP_BE_RQST_MSG *pstRqstMsg, struct DEWARP_OBJ *dwpObj);
INT ISPSS_BE_DEWARP_Config_LDI(struct ISP_BE_RQST_MSG *pstRqstMsg, struct DEWARP_OBJ *dwpObj);
INT ISPSS_BE_DEWARP_Reset(void);
INT ISPSS_BE_DEWARP_FunctionalReset(struct ISP_BE_RQST_MSG *pstRqstMsg);
INT ISPSS_BE_DEWARP_Start(struct ISP_BE_RQST_MSG *pstRqstMsg, struct DEWARP_OBJ *dwpObj);
INT ISPSS_BE_DEWARP_SubmitHW(struct ISP_BE_RQST_MSG *pstRqstMsg,
					struct DEWARP_OBJ *dwpObj, int qid);
INT ISPSS_BE_DEWARP_Chk_LDI_status(struct ISP_BE_RQST_MSG *pstRqstMsg, struct DEWARP_OBJ *dwpObj);
INT ISPSS_BE_DEWARP_FrameInterrupt(struct ISP_BE_RQST_MSG *pstRqstMsg, struct DEWARP_OBJ *dwpObj);
INT ISPSS_BE_DEWARP_RegDump(struct DEWARP_OBJ *dwp_obj);
void ISPSS_BEDEWARP_ShowRequestMsg(struct ISP_BE_RQST_MSG *pstRqstMsg);

/*********************************************************
 * FUNCTION: Generate DIF LUT for DIS
 * Reference for this is diflut.pl from DV team
 * PARAMS: pstRqstMsg - Request message
 * dif_disLut - pointer to LUT
 * RETURN: dlutcnt - Number of 32 bit words in LUT
 *********************************************************/
INT ISPSS_BE_DEWARP_Create_DIFDisLut(struct ISP_BE_RQST_MSG *pstRqstMsg,
					SHM_HANDLE *hShm, UINT32 **dif_disLut);

/*********************************************************
 * FUNCTION: Generate DIF LUT for CIS
 * Reference for this is diflut.pl from DV team
 * PARAMS: pstRqstMsg - Request message
 * dif_cisLut - pointer to LUT
 * RETURN: dlutcnt - Number of 32 bit words in LUT
 *********************************************************/
INT ISPSS_BE_DEWARP_Create_DIFCisLut(struct ISP_BE_RQST_MSG *pstRqstMsg,
					SHM_HANDLE *hShm, UINT32 **dif_cisLut);

INT ISPSS_BE_DEWARP_ReadClient(struct ISP_BE_RQST_MSG *pstRqstMsg, struct DEWARP_OBJ *dwpObj);

#endif //__DEWARP_APIFUNCS_H__
