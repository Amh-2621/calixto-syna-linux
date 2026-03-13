// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>

#define __TILE_API_C__
#include "ispSS_api_dhub_config.h"
#include "ispSS_common.h"
#include "ispSS_dhub_intr.h"
#include "ispSS_reg.h"
#include "ispSS_bcmbuf_queue.h"
#include "ispBE_module_common.h"
#include "ISP_BE_tile_api.h"
#include "ISP_BE_tile_module.h"
#include "ISP_BE_tile_apifuncs.h"
#include "ISP_BE_tile_common.h"
#include "ispbe_err.h"
#include "ispbe_debug.h"

#define ISPMISC_BASE (MEMMAP_ISP_REG_BASE + ISPSS_MEMMAP_GLB_REG_BASE)

#define ISPSS_BE_TILE_BLOCKING_ISR_TIMEOUT_IN_100MS   10
#define ISPSS_TILE_BASE (MEMMAP_ISPSS_REG_BASE + ISPSS_MEMMAP_TILE_REG_BASE)

/*
 * Enable this to do BCM direct register write, this is different from
 * BCM_Direct_Write. For this to work we need to pass NULL buffers
 */
#undef ISPSS_BE_TILE_PRIMITIVE_DIRECT_WRITE

#define GET_TILE_OBJ(index) ((struct ISP_BE_TILE_OBJ *)(g_tile.m_pTile_obj[index]))
int ISPSS_BE_TILE_ProcessISR(UINT32 intrNum, void *pArgs);
struct ISP_BE_TILE_T g_tile;

INT ISPSS_BE_TILE_FreeBcmStruct(struct ISP_BE_BCM *pBcmBuf)
{
	INT i;

	for (i = 0; i < 4; i++)
		ispSS_SHM_Release(pBcmBuf->shmHandle[i].hShm);

	return ISPSS_OK;
}

/*******************************************************************
 * FUNCTION: Clear cache for BCM buffers
 *
 *
 * PARAMS:   pstpstRqstMsg- - Request message
 * RETURN:   SYNA_ISPSS_DNTILE_OK - succeed
 ********************************************************************/
void ISPSS_BE_TILE_BCM_ClearCache(struct ISP_BE_RQST_MSG *rqstBuf)
{
	struct ISP_BE_BCM *pBcmBuf = NULL;

	pBcmBuf = &(rqstBuf->bcmBuf);

	ispSS_SHM_CleanCache(pBcmBuf->shmHandle[0].hShm, 0, sizeof(struct BCMBUF));
	ispSS_SHM_CleanCache(pBcmBuf->shmHandle[1].hShm, 0, sizeof(struct BCMBUF));
	ispSS_SHM_CleanCache(pBcmBuf->shmHandle[2].hShm, 0, sizeof(struct DHUB_CFGQ));
	ispSS_SHM_CleanCache(pBcmBuf->shmHandle[3].hShm, 0, sizeof(struct DHUB_CFGQ));
}

/***********************************************
 * FUNCTION: Release the BCM buffers
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_ENOMEM - no memory
 ***********************************************/
INT ISPSS_BE_TILE_ReleaseBCM(struct ISP_BE_RQST_MSG *rqstBuf)
{
	struct ISP_BE_BCM *pBcmBuf = NULL;

	pBcmBuf = &(rqstBuf->bcmBuf);

	ISPSS_BCMBUF_Destroy(pBcmBuf->bcm_buf);
	ISPSS_BCMBUF_Destroy(pBcmBuf->clear_bcm_buf);
	ISPSS_CFGQ_Destroy(pBcmBuf->dma_cfgQ);
	ISPSS_CFGQ_Destroy(pBcmBuf->final_bcm_cfgQ);
	ISPSS_BE_TILE_FreeBcmStruct(pBcmBuf);

	return ISPSS_OK;
}

/***********************************************************
 * FUNCTION: initialize TILE module. Create Tile objects
 * PARAMS: none
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADCALL - function called previously
 **********************************************************/
INT ISPSS_BE_TILE_Init(void)
{
	HRESULT Ret = ISPSS_OK;

	//Mark status of TILE as inactive.
	g_tile.m_tileHWStatus = TILE_HW_STATUS_IDLE;
	isp_rqstq_reset(&(g_tile.m_OutFrameQ));

#ifdef ISPSS_BE_TILE_ENABLE_BCMBUF_Q
	g_tile.bcmbufQ.queueCfg.maxQueueLen = 3;
	g_tile.bcmbufQ.queueCfg.maxBcmBuf = 2;
	g_tile.bcmbufQ.queueCfg.bcmBufSize = BCM_BUFFER_SIZE;
	g_tile.bcmbufQ.queueCfg.maxDhubCfgQ = 2;
	g_tile.bcmbufQ.queueCfg.dmaCmdSize = DMA_CMD_BUFFER_SIZE;
	ISPSS_BCMBUF_QUEUE_Reset(&g_tile.bcmbufQ);
	ISPSS_BCMBUF_QUEUE_Create(&g_tile.bcmbufQ);
#endif //ISPSS_BE_TILE_ENABLE_BCMBUF_Q

	g_tile.autopush_user_config = 0;
	g_tile.autopush_framecount = 0;

	pr_debug("TILE Init Successful. Object Created\n");

	return Ret;
}

INT ISPSS_BE_TILE_Restart(void)
{
	HRESULT Ret = ISPSS_OK;
	UINT32 RegAddr = 0;
	struct ISP_BE_RQST_MSG *pendRqstMsg;
	int index = 0;
	T32CLKRST_FuncReset funcReset;

	RegAddr = ISPMISC_BASE + RA_IspMISC_clkrst + RA_CLKRST_FuncReset;

	/* Tile func reset */
	ISPSS_REG_READ32(RegAddr, &funcReset);
	funcReset.uFuncReset_tileReset = 0x1;
	ISPSS_REG_WRITE32(RegAddr, funcReset.u32);
	funcReset.uFuncReset_tileReset = 0x0;
	ISPSS_REG_WRITE32(RegAddr, funcReset.u32);

	//optional init
	for (index = 0; index < MAX_TILE_OBJECTS; index++) {
		g_tile.m_pTile_obj[index]->m_iValid = 1;
		g_tile.m_pTile_obj[index]->m_objStatus = ISP_BE_TILE_OBJ_STATUS_ACTIVE;
		g_tile.m_pTile_obj[index]->base_addr = ISPSS_TILE_BASE;
	}
	g_tile.enablePrevRqstComparision = 0;
	//mandatory init
	g_tile.m_tileHWStatus = TILE_HW_STATUS_IDLE;
	g_tile.restartFlag = 0;

	if (isp_rqstq_pop_shadow(&g_tile.m_OutFrameQ, (void **)&pendRqstMsg)) {
		//restart tiler rqst again
		g_tile.restartFlag = 1;
		if (pendRqstMsg->SubmitHw) {
			for (index = 0; index < MAX_TILE_OBJECTS; index++) {
				Ret = ISPSS_BE_TILE_ProcessRequest(pendRqstMsg,
					g_tile.m_pTile_obj[index], pendRqstMsg->debugFlag);
			}
		}
	}

	return Ret;
}

/**************************************************
 * FUNCTION: Destroy all Tile Objects
 * PARAMS: None
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EHARDWAREBUSY - Hardware Busy
 *
 **************************************************/
INT ISPSS_BE_TILE_Destroy(void)
{
	HRESULT Ret = ISPSS_OK;
	struct ISP_BE_RQST_MSG *rqstBuf = NULL;
	struct ISP_BE_TILE_OBJ *ispBeTilerObj;
	int index = 0;

	/* If H/W is still processing the frame can't destroy the object now */
	if (g_tile.m_tileHWStatus == TILE_HW_STATUS_RUNNING) {
		Ret = ISPSS_EHARDWAREBUSY;
		goto e_TILE_Destroy;
	}

	/* Pop all rqst messages from output queue */
	while (isp_rqstq_pop(&g_tile.m_OutFrameQ, (void *)&rqstBuf)) {
		/* Release the BCM buffers */
#ifndef ISPSS_BE_TILE_ENABLE_BCMBUF_Q
		ISPSS_BE_TILE_ReleaseBCM(rqstBuf);
#endif
		isp_rqstq_pop_commit(&g_tile.m_OutFrameQ);
	}

#ifdef ISPSS_BE_TILE_ENABLE_BCMBUF_Q
	ISPSS_BCMBUF_QUEUE_Destroy(&g_tile.bcmbufQ);
#endif //ISPSS_BE_TILE_ENABLE_BCMBUF_Q

	for (index = 0; index < MAX_TILE_OBJECTS; index++) {
		ispBeTilerObj = GET_TILE_OBJ(index);
		if (ispBeTilerObj) {
			if (ispBeTilerObj->m_iValid) {
				pr_debug("Close is not called for id = %d\n", index);
				Ret = ISPSS_EHARDWAREBUSY;
				goto e_TILE_Destroy;
			} else {
				ispSS_SHM_Release(g_tile.shmHandle[index]);
				g_tile.m_pTile_obj[index] = NULL;
			}
		}
	}

e_TILE_Destroy:
	if (Ret == ISPSS_OK)
		pr_err("TILE Destroy Successful\n");
	else
		pr_err("TILE Destroy Failed\n");

	return Ret;
}

/***********************************************
 * FUNCTION: Set BCM buffers to NULL
 * PARAMS: pstRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *
 ***********************************************/
#ifdef ISPSS_BE_TILE_PRIMITIVE_DIRECT_WRITE
static INT ISPSS_BE_TILE_RequestBCM_Null(struct ISP_BE_RQST_MSG *pstRqstMsg)
{
	struct ISP_BE_BCM *pBcmBuf;

	pBcmBuf = &(pstRqstMsg->bcmBuf);

	pBcmBuf->bcm_buf = NULL;
	pBcmBuf->clear_bcm_buf = NULL;
	pBcmBuf->dma_cfgQ = NULL;
	pBcmBuf->final_bcm_cfgQ = NULL;

	return ISPSS_OK;
}
#endif

#ifdef ISPSS_BE_TILE_ENABLE_BCMBUF_Q
/*******************************************************************
 * FUNCTION: Copy BCM buffer from BCMQ
 * PARAMS: pBcmBuf - BCM buffer
 *         pBcmBufQ - BCM Queue
 * RETURN: ISPSS_DWP_OK - succeed
 *
 ********************************************************************/
static void ISPSS_BE_TILE_CopyFromBcmBufQ(struct ISP_BE_BCM *pBcmBuf,
				struct ISPSS_BCMBUF_ITEM *pBcmBufQ)
{
	int i;

	pBcmBuf->bcm_buf = &pBcmBufQ->pBcmBuf[ISPSS_BCMBUF_ITEM_PROGRAM_BCM_NDX];
	pBcmBuf->clear_bcm_buf =  &pBcmBufQ->pBcmBuf[ISPSS_BCMBUF_ITEM_CLEAR_BCM_NDX];
	pBcmBuf->dma_cfgQ = &pBcmBufQ->pCfgQ[ISPSS_CFGQ_ITEM_DHUB_CFGQ_NDX];
	pBcmBuf->final_bcm_cfgQ =  &pBcmBufQ->pCfgQ[ISPSS_CFGQ_ITEM_FINAL_CGQ_NDX];

	pBcmBuf->shmHandle[0].hShm =
		pBcmBufQ->pBcmBuf[ISPSS_BCMBUF_ITEM_CLEAR_BCM_NDX].handle;
	pBcmBuf->shmHandle[1].hShm =
		pBcmBufQ->pBcmBuf[ISPSS_BCMBUF_ITEM_PROGRAM_BCM_NDX].handle;
	pBcmBuf->shmHandle[2].hShm =
		pBcmBufQ->pCfgQ[ISPSS_CFGQ_ITEM_DHUB_CFGQ_NDX].handle;
	pBcmBuf->shmHandle[3].hShm =
		pBcmBufQ->pCfgQ[ISPSS_CFGQ_ITEM_FINAL_CGQ_NDX].handle;
	for (i = 0; i < 4; i++) {
		ispSS_SHM_GetVirtualAddress(pBcmBuf->shmHandle[i].hShm, 0,
				&(pBcmBuf->shmHandle[i].addr));
		ispSS_SHM_GetPhysicalAddress(pBcmBuf->shmHandle[i].hShm, 0,
				&(pBcmBuf->shmHandle[i].phy_addr));
	}
}

#endif //ISPSS_BE_TILE_ENABLE_BCMBUF_Q

INT ISPSS_BE_TILE_PrepareRequestForNoConfigChange(struct ISP_BE_RQST_MSG *pendRqstMsg,
				struct ISP_BE_TILE_OBJ *tileObj)
{
	/* Select sub register programming buffer */
#ifndef ISPSS_BE_TILE_ENABLE_BCMBUF_Q
	ISPSS_BCMBUF_Select(pendRqstMsg->bcmBuf.bcm_buf, CPCB_1);
	ISPSS_BCMBUF_Select(pendRqstMsg->bcmBuf.clear_bcm_buf, CPCB_1);
#else
	ISPSS_BCMBUF_QUEUE_Element_Select(&g_tile.bcmbufQ, pendRqstMsg->pCurrBcmBuf, CPCB_1);
	ISPSS_BE_TILE_CopyFromBcmBufQ(pendRqstMsg->pBcmBuf, pendRqstMsg->pCurrBcmBuf);
#endif

	/* Start tile sequence */
	ISPSS_BE_TILE_Start(tileObj, pendRqstMsg);
	pendRqstMsg->FrameState = 0; //Actual H/W processing not done yet
	pendRqstMsg->bcm_prepared = 1;  //BCM preparation done

	return 0;
}

INT ISPSS_BE_TILE_PrepareRequest(struct ISP_BE_RQST_MSG *pendRqstMsg,
				struct ISP_BE_TILE_OBJ *tileObj)
{
	if (!pendRqstMsg->bcm_prepared) {
#ifndef ISPSS_BE_TILE_ENABLE_BCMBUF_Q
		/* Select sub register programming buffer */
		ISPSS_BCMBUF_Select(pendRqstMsg->bcmBuf.bcm_buf, CPCB_1);
		ISPSS_BCMBUF_Select(pendRqstMsg->bcmBuf.clear_bcm_buf, CPCB_1);
#else
		ISPSS_BCMBUF_QUEUE_Element_Select(&g_tile.bcmbufQ,
				pendRqstMsg->pCurrBcmBuf, CPCB_1);
		ISPSS_BE_TILE_CopyFromBcmBufQ(pendRqstMsg->pBcmBuf, pendRqstMsg->pCurrBcmBuf);
#endif
		ISPSS_BE_TILE_Reset(pendRqstMsg);

		ISPSS_BE_TILE_ConfigDummyTG(tileObj, pendRqstMsg);
		ISPSS_BE_TILE_ConfigTile(tileObj, pendRqstMsg);
		ISPSS_BE_TILE_ConfigWriteClient(tileObj, pendRqstMsg);

		/* Start tileing sequence */
		ISPSS_BE_TILE_Start(tileObj, pendRqstMsg);
		pendRqstMsg->FrameState = 0; //Actual H/W processing not done yet
		pendRqstMsg->bcm_prepared = 1;  //BCM preparation done
	}

	return ISPSS_OK;
}

/*******************************************************************
 * FUNCTION: Submit the request message to hardware
 *           If WaitforIntr is set then this function will pole
 *           for H/W semaphore to be set.
 * PARAMS: pstRqstMsg - Request message
 tileObj - Tile object
 * RETURN: ISPSS_OK - succeed
 ********************************************************************/
INT ISPSS_BE_TILE_SubmitToHardWare(struct ISP_BE_RQST_MSG *pstRqstMsg,
				struct ISP_BE_TILE_OBJ *tileObj, int dbgLevel)
{
	g_tile.m_tileHWStatus = TILE_HW_STATUS_RUNNING;

	//printf("Submitting BCM Buffer to HardWare\n");
	//ISPSS_BE_TILE_BCM_ClearCache(pstRqstMsg);
	ISPSS_BE_TILE_SubmitHW(tileObj, pstRqstMsg, dbgLevel);

	//Wait for the interrupt
	if (pstRqstMsg->WaitforIntr) {
		static int timeoutCount = ISPSS_BE_TILE_BLOCKING_ISR_TIMEOUT_IN_100MS;

		while (timeoutCount) {
			if (ISPSS_BE_TILE_FrameInterrupt(tileObj, pstRqstMsg)) {
				pstRqstMsg->FrameState = 1;
				g_tile.m_tileHWStatus = TILE_HW_STATUS_IDLE;
				break;
			} else {
				//wait for 100ms before polling again
				mdelay(100);
			}
			timeoutCount--;
		}
	}

	return ISPSS_OK;
}

INT ISPSS_BE_TILE_ProcessRequest(struct ISP_BE_RQST_MSG *pendRqstMsg,
				struct ISP_BE_TILE_OBJ *tileObj, int dbgLevel)
{
	struct ISP_BE_RQST_MSG *processedRqstMsg = NULL;
	HRESULT Ret = ISPSS_OK;

	pr_debug("%s %d\n", __func__, __LINE__);
	//0. Commit the previously processed frame to outputQ
	if (!g_tile.restartFlag) {
#ifdef ISPSS_BE_TILE_ENABLE_BCMBUF_Q
		isp_rqstq_pop(&g_tile.m_OutFrameQ, (void **)&processedRqstMsg);
		/* Recycle the BCM buffer */
		if (processedRqstMsg && processedRqstMsg->pCurrBcmBuf) {
			pr_debug("%s %d\n", __func__, __LINE__);
			ISPSS_BCMBUF_QUEUE_push(&g_tile.bcmbufQ, processedRqstMsg->pCurrBcmBuf);
			processedRqstMsg->pCurrBcmBuf = NULL;
			processedRqstMsg->pBcmBuf = NULL;
		}
#endif
		//isp_rqstq_push_shadow_commit(&g_tile.m_OutFrameQ);
	}

	//3. Only if SubmitHw is set BCM buffers are submitted to hardware
	if (pendRqstMsg && pendRqstMsg->SubmitHw) {
		//2. If BCM is not prepared then prepare it now
		if (pendRqstMsg->bcm_prepared == 0) {
#ifdef ISPSS_BE_TILE_ENABLE_BCMBUF_Q
			/* If BCM buffer is not allocated try to allocate */
			if (!pendRqstMsg->pCurrBcmBuf) {
				Ret = ISPSS_BCMBUF_QUEUE_pop_and_commit(&g_tile.bcmbufQ,
				(void **)&pendRqstMsg->pCurrBcmBuf);
				if (Ret == 0) {
					pr_err("%s: ISPSS_BCMBUF_QUEUE_pop_and_commit failed\n",
						__func__);
					return ISPSS_OK;
				}
			}
#endif
			ISPSS_BE_TILE_PrepareRequest(pendRqstMsg, tileObj);
		}
		ISPSS_BE_TILE_SubmitToHardWare(pendRqstMsg, tileObj, dbgLevel);
		//Restart completed after submitting to hw again
		g_tile.restartFlag = 0;

		/*
		 * Keep a copy of the processed frame
		 * This will be used to comare with current frame properties
		 * which inturn useful to reduce the BCM writes
		 */
		memcpy(&g_tile.prvStTileRqst, pendRqstMsg, sizeof(struct ISP_BE_RQST_MSG));
		//Move to output queue but commit in next ISR
		isp_rqstq_push_shadow(&g_tile.m_OutFrameQ, pendRqstMsg);
	} else {
		//If no more frame to process, then move to IDLE state
		if (g_tile.m_OutFrameQ.count == 1)
			g_tile.m_tileHWStatus = TILE_HW_STATUS_IDLE;
	}

	return ISPSS_OK;
}

int ISPSS_BE_TILE_ProcessISR(UINT32 intrNum, void *pArgs)
{

	pr_debug("%s Entry\n", __func__);

	ISPSS_BE_TILE_FIFO_Status(2);
	//printf("Tile ProcessISR\n");
	//2. Process the next avaiable request
	ISPSS_BE_TILE_ProcessRequest(NULL, NULL, 0x0);

#ifdef ISPBE_DIAGS
	if ((g_tile.IntrHandler))
		(*g_tile.IntrHandler)(g_tile.IntCnt++, g_tile.IntrHandlerArgs);
#endif
	pr_debug("%s Exit\n", __func__);
	return ISPSS_OK;
}


INT ISPSS_BE_TILE_PollISR(void)
{
	while (1) {
		if (ISPSS_BE_TILE_FrameInterrupt(NULL, NULL)) {
			//Frame done seen, so process the ISR
			ISPSS_BE_TILE_ProcessISR(ispDhubSemMap_TSB_tile_intr, NULL);
			break;
		}
	}

	return ISPSS_OK;
}

/***********************************************
 * FUNCTION: Push a frame for Tileing
 * PARAMS: pstRqstMsg - Request message
 iClientId - Index to the object, if multiple objects exist
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_ENOMEM - no memory
 ***********************************************/
INT ISPSS_BE_TILE_PushRequest(INT iClientId, struct ISP_BE_RQST_MSG *pstRqstMsg)
{
	HRESULT Ret = ISPSS_OK;
	struct ISP_BE_TILE_OBJ *tileObj;
	struct ISP_BE_RQST_MSG *pendRqstMsg = NULL;

	pr_debug("%s Entry\n", __func__);
	/*
	 * Fetch the object from object pool -
	 * TBD: create the memory required by the client dynamically here
	 */
	tileObj = GET_TILE_OBJ(iClientId);

#ifndef ISPSS_BE_TILE_ENABLE_BCMBUF_Q
	/*
	 * 1. Create BCMBUF required to process the request -
	 * TBD: introduce interface, so that application can preallocate buffer
	 */
	Ret = ISPSS_BE_TILE_RequestBCM(pstRqstMsg);
	if ((Ret != ISPSS_OK)
		goto e_PushRequest;

#else //ISPSS_BE_TILE_ENABLE_BCMBUF_Q
	if (pstRqstMsg->pCurrBcmBuf == NULL) {
		//pr_err("%s %d\n", __func__, __LINE__);
		pstRqstMsg->pBcmBuf = &(pstRqstMsg->bcmBuf); //Else Copy from pCurrBcmBuf fails

		pstRqstMsg->bcm_prepared = 0;
		if (ISPSS_BCMBUF_QUEUE_pop_and_commit(&g_tile.bcmbufQ,
							(void **)&pstRqstMsg->pCurrBcmBuf) == 0) {
			pr_err("%s: ISPSS_BCMBUF_QUEUE_pop_and_commit failed\n", __func__);
			Ret = ISPSS_EBCMBUFFULL;
			goto e_PushRequest;
		}
	}
	if (pstRqstMsg->pCurrBcmBuf)
#endif
	{
		/*2. If no change w.r.t the g_tile.prvStTileRqst,
		 * then better prepare the BCMBUF here itself
		 */
		ISPSS_BE_TILE_PrepareRequest(pstRqstMsg, tileObj);
	}
	pstRqstMsg->debugFlag = 0;
	/*
	 * 3. If H/W is still processing then push the frame
	 * into client queue and return immediately
	 */
#ifdef BCM_IMMEDIATE_WRITE
	if (g_tile.m_tileHWStatus == TILE_HW_STATUS_RUNNING) {
		//pr_err("%s %d\n", __func__, __LINE__);
		Ret = ISPSS_EHARDWAREBUSY;
		goto e_PushRequest;
	} else {
#endif
		pendRqstMsg = pstRqstMsg;
#ifdef BCM_IMMEDIATE_WRITE
	}
#endif

	isp_rqstq_push(&g_tile.m_OutFrameQ, pstRqstMsg);
	//4. Only if SubmitHw is set, then ProcessRequest = Prepare + submit
	if (pendRqstMsg->SubmitHw) {
		//pr_err("%s %d\n", __func__, __LINE__);
		Ret = ISPSS_BE_TILE_ProcessRequest(pendRqstMsg, tileObj, pstRqstMsg->debugFlag);
	} else {
		g_tile.m_tileHWStatus = TILE_HW_STATUS_RUNNING;
	}

e_PushRequest:
	pstRqstMsg->ErrorCode = Ret;

	pr_debug("Exit\n");
	return Ret;
}

/***********************************************
 * FUNCTION: Pop a frame whose Tileing is completed
 * PARAMS: pstRqstMsg - Request message processed sucessfully
 * RETURN: valid pointer if sucessfuly poped
 *         null pointer, no more request to pop
 ***********************************************/
INT ISPSS_BE_TILE_PopRequest(struct ISP_BE_RQST_MSG **pstRqstMsg)
{
	struct ISP_BE_RQST_MSG *rqstMsg = NULL;

	if (isp_rqstq_pop(&g_tile.m_OutFrameQ, (void **)&rqstMsg)) {
		isp_rqstq_pop_commit(&g_tile.m_OutFrameQ);
		*pstRqstMsg = rqstMsg;
	} else {
		*pstRqstMsg =  NULL;
	}

	return ISPSS_OK;
}

/*********************************************************************
 * FUNCTION: Get the underflow/Overflow status, clear the FIFO
 * PARAMS:   clrFlag - 1 Clear the FIFO, 0 Dispaly status
 * RETURN:   ISPSS_OK
 ********************************************************************/
INT ISPSS_BE_TILE_FIFO_Status(INT clrFlag)
{
	int index = 0;

	for (index = 0; index < MAX_TILE_OBJECTS; index++) {
		if (g_tile.m_pTile_obj[index]->m_iValid == 1)
			ISPSS_BE_TILE_UnderflowOverflow_status(g_tile.m_pTile_obj[index], clrFlag);
	}

	return ISPSS_OK;
}

void ISPSS_BE_TILE_RegisterInterruptCallback(void *pArgs, ISPSS_INTR_HANDLER tile_intrHandler)
{

	g_tile.IntrHandler = tile_intrHandler;
	g_tile.IntrHandlerArgs = pArgs;
	g_tile.IntCnt = 0;
}

INT ISPSS_BE_TILER_IOCTL(enum ISP_BE_IOCTL_CMD ioctl_cmd, void *param)
{
	HRESULT Ret = ISPSS_OK;
	int data;

	switch (ioctl_cmd) {
	case ISP_BE_IOCTL_CMD_TILE_RESTART:
		ISPSS_BE_TILE_Restart();
		break;
	case ISP_BE_IOCTL_CMD_TILE_FIFO_STATUS:
		data = *(int *)param;
		ISPSS_BE_TILE_FIFO_Status(data);
		break;
	default:
		pr_err("TILER: Invalid ioctl command\n");
	}

	return Ret;
}

static void ISPSS_BE_TILE_ResetClient(INT index, INT priority)
{
	g_tile.m_pTile_obj[index]->m_iValid = 1;
	g_tile.m_pTile_obj[index]->base_addr = ISPSS_TILE_BASE;
	g_tile.m_pTile_obj[index]->m_objStatus = ISP_BE_TILE_OBJ_STATUS_ACTIVE;

	g_tile.enablePrevRqstComparision = 0;
	g_tile.restartFlag = 0;
	memset(&(g_tile.prvStTileRqst), -1, sizeof(struct ISP_BE_RQST_MSG));//ISPBE_SW_BRINGUP
}

INT ISPSS_BE_TILE_Open(INT *clientID, INT priority)
{
	INT objCnt, ret = ISPSS_FAIL;
	void *tileObjAddr;

	for (objCnt = 0; objCnt < MAX_TILE_OBJECTS; objCnt++) {
		if (g_tile.m_pTile_obj[objCnt] == NULL) {
			ret = ispSS_SHM_Allocate(SHM_NONSECURE, sizeof(struct ISP_BE_TILE_OBJ),
				1024, &g_tile.shmHandle[objCnt], SHM_NONSECURE_CONTIG);
			if (ret != 0) {
				pr_debug("Failed to Allocate memory for TILE\n");
				ret = ISPSS_ENOMEM;
				goto e_ISPSS_BE_TILE_Create;
			}
			ispSS_SHM_GetVirtualAddress(g_tile.shmHandle[objCnt],
					0, &(tileObjAddr));
			g_tile.m_pTile_obj[objCnt] = (struct ISP_BE_TILE_OBJ *) tileObjAddr;
			*clientID = objCnt;
			ISPSS_BE_TILE_ResetClient(objCnt, priority);
			pr_debug("TILE OBJECT CREATED, %d\n", *clientID);
			break;
		} else if (!g_tile.m_pTile_obj[objCnt]->m_iValid) {
			*clientID = objCnt;
			ISPSS_BE_TILE_ResetClient(objCnt, priority);
			pr_debug("RE-USE TILE OBJECT CREATED, %d\n", *clientID);
			break;
		}
	}

	pr_debug("TILE LOAD DEFAULT CONFIG SUCCESS\n");

e_ISPSS_BE_TILE_Create:
	return ret;
}

INT  ISPSS_BE_TILE_Close(int clientID)
{
	struct ISP_BE_TILE_OBJ *m_pTile_obj;
	int Ret = ISPSS_OK;

	if (g_tile.m_tileHWStatus == TILE_HW_STATUS_RUNNING) {
		pr_info("ProcessQCnt: %d\n", g_tile.m_OutFrameQ.count);
		Ret = ISPSS_EHARDWAREBUSY;
		goto e_TILER_Close;
	}

	m_pTile_obj = GET_TILE_OBJ(clientID);
	if (m_pTile_obj == NULL) {
		pr_debug("%s: NO Tiler client found for this\n", __func__);
		Ret = ISPSS_ENODEV;
		goto e_TILER_Close;
	}

	g_tile.m_pTile_obj[clientID]->m_iValid = 0;

	BCM_SCHED_Flush(BCM_SCHED_Q8);
	BCM_SchedSetMux(BCM_SCHED_Q8, 0x1F);

	pr_debug("%s: with instance %d\n", __func__, clientID);

e_TILER_Close:
	return Ret;
}

void ISPSS_BE_TILER_Probe(struct ISPBE_CA_DRV_CTX *drv_ctx)
{
	/* Initialize driver API's calls */
	drv_ctx->fops.module_init = ISPSS_BE_TILE_Init;
	drv_ctx->fops.module_destroy = ISPSS_BE_TILE_Destroy;
	drv_ctx->fops.module_open = ISPSS_BE_TILE_Open;
	drv_ctx->fops.module_close = ISPSS_BE_TILE_Close;
	drv_ctx->fops.module_ioctl = ISPSS_BE_TILER_IOCTL;
	drv_ctx->fops.module_ProcessISR = ISPSS_BE_TILE_ProcessISR;
	drv_ctx->fops.module_PushRequest = ISPSS_BE_TILE_PushRequest;
	drv_ctx->fops.module_PopRequest = ISPSS_BE_TILE_PopRequest;
	drv_ctx->sem_id = ISP_DHUBSEM_TSB_tile_intr;
}
