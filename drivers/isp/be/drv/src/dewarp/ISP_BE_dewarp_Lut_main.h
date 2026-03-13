/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __DEWARP_LUT_MAIN_H__
#define __DEWARP_LUT_MAIN_H__

/***********************************************************
 * FUNCTION: initialize DWP Lut specific code.
 * PARAMS: none
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADCALL - function called previously
 *
 **********************************************************/
INT ISPSS_BE_Lut_Init(void);

/***********************************************************
 * FUNCTION: Destroy DWP Lut specific code.
 * PARAMS: none
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADCALL - function called previously
 *
 **********************************************************/
INT ISPSS_BE_Lut_Destroy(void);

void ISPSS_BE_LoadLutCfgData(struct ISP_BE_RQST_MSG *pstRqstMsg);

INT ISPSS_BE_DEWARP_CheckLutStatus(struct ISP_BE_RQST_MSG *pendRqstMsg);

#endif//__DEWARP_LUT_MAIN_H__

