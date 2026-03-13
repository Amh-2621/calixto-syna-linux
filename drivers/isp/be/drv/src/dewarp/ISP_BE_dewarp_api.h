/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __DEWARP_API_H__
#define __DEWARP_API_H__

#include "ispSS_bcmbuf.h"
#include "ispSS_bcmbuf_queue.h"
#include "ispBE_common.h"
#include "ispBE_module_common.h"

extern int DwpDbg;
extern int DwpDbg_ct;

#define DWPLOGI(...) pr_err(__VA_ARGS__)
#define DWPLOGE(...) pr_err(__VA_ARGS__)
//#define DWPLOGT(...) pr_err(__VA_ARGS__)
#define DWPLOGT(...)

#ifndef ISPSS_BE_DEWARP_DBUB_BURST_SZ
#define ISPSS_BE_DEWARP_DBUB_BURST_SZ (128)
#endif

enum CISP_PATH {
	CISP_TILE_PATH,
	CISP_RS_PATH
};

enum YUV_MODE {
	MODE_YUV422 = 0,
	MODE_YUV420,
};

#define CEIL(a, b) ((a / b) + ((a % b) != 0))

typedef int (*ISPSS_DWP_INTR_HANDLER)(int intr, void *pArgs);

/* Parametres to be sent through ioctl for ISPSS_BE_DEWARP_LGDC_Config */
struct ISP_BE_DEWARP_LGDC_CFG {
	INT sensorId;
	SHM_HANDLE shmHandle;
	INT disidx_cismat_sz;
	SHM_HANDLE shmHandleCfg;
	INT cfg_sz;
	struct ISPBE_DEWARP_LUT_CFG lutCFG;
};

/***********************************************************
 * FUNCTION: initialize DWP module
 * PARAMS: none
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADCALL - function called previously
 **********************************************************/
INT ISPSS_BE_DEWARP_Init(void);

/**************************************************
 * FUNCTION: Destroy all DeWarp Objects
 * PARAMS: None
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EHARDWAREBUSY - Hardware Busy
 *
 **************************************************/
INT ISPSS_BE_DEWARP_Destroy(void);

/*****************************************************************
 * FUNCTION: Open DeWarp Object and return client ID
 * PARAMS: iClientId - ID of the Client.
 *         iPriority - Client priority
 * RETURN: ISPSS_OK - succeed
 *
 *****************************************************************/
INT ISPSS_BE_DEWARP_Open(INT *iClientId, INT iPriority);

/**************************************************
 * FUNCTION: Close DeWarp Object
 * PARAMS: None
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_ENODEV - No device
 *         ISPSS_EHARDWAREBUSY - Hardware busy
 **************************************************/
INT ISPSS_BE_DEWARP_Close(INT iClientId);

/***********************************************
 * FUNCTION: Push a frame for DeWarping
 * PARAMS: pstRqstMsg - Request message
 handle - Index to the object, if multiple objects exist
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_ENOMEM - no memory
 ***********************************************/
INT ISPSS_BE_DEWARP_PushRequest(int handle, struct ISP_BE_RQST_MSG *pstRqstMsg);

/*********************************************************************
 * FUNCTION: Dewarp Register dump
 * PARAMS:   None.
 * RETURN:   ISPSS_OK
 ********************************************************************/
INT ISPSS_BE_DEWARP_DumpRegs(void);

/****************************************************************
 * FUNCTION: Initialize the DISIdx CIS Mat in
 dwp_lgdc of g_dewarp
 * PARAMS: sensorId - Sensor ID 0/1
 shmHandle - Handle to Index, Matrix buffer
 If NULL, use internal algo to generate
 disidx_cismat_sz - Buffer size
 * RETURN: ISPSS_OK - succeed
 ****************************************************************/
INT ISPSS_BE_DEWARP_LGDC_Config(INT sensorId, SHM_HANDLE *shmHandle,
	INT disidx_cismat_sz, struct ISPBE_DEWARP_LUT_CFG *lutCFG);

/***********************************************
 * FUNCTION: Pop a frame whose DeWarping is completed
 * PARAMS: pstRqstMsg - Request message processed successfully
 * RETURN: valid pointer if sucessfuly poped
 *         null pointer, no more request to pop
 ***********************************************/
INT ISPSS_BE_DEWARP_PopRequest(struct ISP_BE_RQST_MSG **pstRqstMsg);

/***********************************************
 * FUNCTION: Handle dewarp ISR callback
 * PARAMS: intrNum The interrupt number, pArgs callback data
 * RETURN: ISPSS_OK - succeed
 ***********************************************/
INT ISPSS_BE_DEWARP_ProcessISR(UINT32 intrNum, void *pArgs);

/***********************************************
 * FUNCTION: Print dewarp request message structure
 * PARAMS: pstRqstMsg - Request message processed successfully
 * RETURN: ISPSS_OK - succeed
 ***********************************************/
INT dwp_print_rqst_msg(struct ISP_BE_RQST_MSG *rqstMsg);

/***********************************************
 * FUNCTION: ISPBe api to load Lut and configure acocording to sensor id
 * PARAMS: sensorId: sensor Id, Lut file lutCFG for LUT configuration
 * RETURN: ISPSS_OK - succeed
 ***********************************************/
INT ISPBE_CA_LoadMatrix(int sensorId, char *lut_file, struct ISPBE_DEWARP_LUT_CFG *lutCFG);

#endif//__DEWARP_API_H__
