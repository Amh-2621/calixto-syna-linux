/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __TILE_API_H__
#define __TILE_API_H__

#include "ispSS_bcmbuf.h"
#include "ispSS_shm.h"

#define ISP_DEBUG_CALLTRACE "ISPDBG:TILE: "

typedef int (*ISPSS_INTR_HANDLER)(int intr, void *pArgs);
struct ISP_BE_TILE_OBJ;

/***********************************************************
 * FUNCTION: initialize TILE module
 * PARAMS: none
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADCALL - function called previously
 **********************************************************/
INT ISPSS_BE_TILE_Init(void);

/**************************************************
 * FUNCTION: Destroy all DeWarp Objects
 * PARAMS: None
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EHARDWAREBUSY - Hardware Busy
 *
 **************************************************/
INT ISPSS_BE_TILE_Destroy(void);

/***********************************************
 * FUNCTION: Push a frame for DeWarping
 * PARAMS: pstdewarpRqstMsg - Request message
 *         handle - Index to the object, if multiple objects exist
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_ENOMEM - no memory
 ***********************************************/
INT ISPSS_BE_TILE_PushRequest(int handle, struct ISP_BE_RQST_MSG *pstRqstMsg);

/*********************************************************************
 * FUNCTION: Dewarp Register dump
 * PARAMS:   None.
 * RETURN:   ISPSS_OK
 ********************************************************************/
//INT ISPSS_BE_TILE_DumpRegs();

int ISPSS_BE_TILE_ProcessISR(UINT32 intrNum, void *pArgs);

void tile_setOBuf(int pOaddr, int rqstNum);

INT ISPSS_BE_TILE_PopRequest(struct ISP_BE_RQST_MSG **tileRqstMsg);

INT ISPSS_BE_TILE_ProcessRequest(struct ISP_BE_RQST_MSG *pendRqstMsg,
		struct ISP_BE_TILE_OBJ *tileObj, int dbgLevel);

int tile_print_rqst_msg(struct ISP_BE_RQST_MSG *pstRqstMsg); //TODO: Debugging remove this

void tile_intrHandler(int intr, void *param);

void ISPSS_BE_TILE_RegisterInterruptCallback(void *pArgs, ISPSS_INTR_HANDLER tile_intrHandler);

/*********************************************************************
 * FUNCTION: Get the underflow/Overflow status, clear the FIFO
 * PARAMS:   clrFlag - 1 Clear the FIFO, 0 Dispaly status
 * RETURN:   ISPSS_OK
 ********************************************************************/
INT ISPSS_BE_TILE_FIFO_Status(INT clrFlag);

INT ISPSS_BE_TILE_Open(int *clientID, int priorit);
INT ISPSS_BE_TILE_Close(int clientID);

#endif//__TILE_API_H__
