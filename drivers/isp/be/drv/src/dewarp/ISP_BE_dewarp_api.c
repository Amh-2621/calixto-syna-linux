// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/delay.h>
#include <linux/firmware.h>

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
#include "ISP_BE_dewarp_Lut_main.h"
#include "ISP_BE_dewarp_helper_apis.h"

#define baseAddr_DEWARP     (MEMMAP_ISPSS_REG_BASE + ISPSS_MEMMAP_DEWARP_REG_BASE)

#define ISPSS_BE_DEWARP_BLOCKING_ISR_TIMEOUT_IN_100MS   10
#define GET_DWP_OBJ(index) ((struct DEWARP_OBJ *)(g_dewarp.m_pDewarp_objs[index]))

struct DEWARP g_dewarp;

INT ISPSS_BE_DEWARP_FreeBcmStruct(struct ISP_BE_BCM *pBcmBuf)
{
	INT i;

	for (i = 0; i < 4; i++)
		ispSS_SHM_Release(pBcmBuf->shmHandle[i].hShm);

	return ISPSS_OK;
}


/***********************************************
 * FUNCTION: Release the BCM buffers
 * PARAMS: pstdewarpRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_ENOMEM - no memory
 *
 ***********************************************/
INT ISPSS_BE_DEWARP_ReleaseBCM(struct ISP_BE_RQST_MSG *rqstBuf)
{
	struct ISP_BE_BCM *pBcmBuf = NULL;

	pBcmBuf = ISPSS_GET_BCMBUF(rqstBuf);

	ISPSS_BCMBUF_Destroy(pBcmBuf->bcm_buf);
	ISPSS_BCMBUF_Destroy(pBcmBuf->clear_bcm_buf);
	ISPSS_CFGQ_Destroy(pBcmBuf->dma_cfgQ);
	ISPSS_CFGQ_Destroy(pBcmBuf->final_bcm_cfgQ);
	ISPSS_BE_DEWARP_FreeBcmStruct(pBcmBuf);

	return ISPSS_OK;
}


/***********************************************************
 * FUNCTION: initialize DWP module. Create DeWarp objects
 * PARAMS: none
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EBADCALL - function called previously
 *
 **********************************************************/
INT ISPSS_BE_DEWARP_Init(void)
{
	INT i;
	HRESULT Ret = ISPSS_OK;

	if (g_dewarp.magic == DEWARP_MAGIC) {
		Ret = ISPSS_EBADCALL;
		goto e_ISPSS_BE_DEWARP_Init;
	}

	g_dewarp.magic = DEWARP_MAGIC;
	g_dewarp.m_dwpNoOfObjs = MAX_DEWARP_OBJECTS;

	//Mark status of DEWARP as inactive.
	g_dewarp.m_dwpHWStatus = DEWARP_HW_STATUS_IDLE;
	isp_rqstq_reset(&(g_dewarp.m_OutFrameQ));
	isp_rqstq_reset(&(g_dewarp.m_ProcessFrameQ));
	g_dewarp.IntCnt = 0;
	g_dewarp.commit_QId = DEWARP_COMMIT_QUEUE_ID_12;

	mutex_init(&(g_dewarp.m_OutFrameQ.isp_rqstq_lock));
	mutex_init(&(g_dewarp.m_ProcessFrameQ.isp_rqstq_lock));
	mutex_init(&g_dewarp.isr_lock);

	//Create multiple DeWarp objects
	for (i = 0; i < MAX_DEWARP_OBJECTS; i++) {

		Ret = ispSS_SHM_Allocate(SHM_NONSECURE, sizeof(struct DEWARP_OBJ), 1024,
			&g_dewarp.shmHandle[i], SHM_NONSECURE_CONTIG);

		if (Ret != ISPSS_OK) {
			DWPLOGE("Failed to Allocate memory sizeof struct DEWARP_OBJ = %ld\n",
					sizeof(struct DEWARP_OBJ));
			DWPLOGE("ispSS_SHM_Allocate Fails\n");
			Ret = ISPSS_ENOMEM;
			goto e_ISPSS_BE_DEWARP_Init;
		}
		ispSS_SHM_GetVirtualAddress(g_dewarp.shmHandle[i], 0,
			(void **)&g_dewarp.m_pDewarp_objs[i]);
		g_dewarp.m_pDewarp_objs[i]->m_dwpClientId = -1;
		g_dewarp.m_pDewarp_objs[i]->m_dwpPriority = -1;
		g_dewarp.m_pDewarp_objs[i]->m_iValid = 1;
		g_dewarp.m_pDewarp_objs[i]->m_queue.InputQCnt = 0;
		g_dewarp.m_pDewarp_objs[i]->m_objStatus = DEWARP_OBJ_STATUS_INACTIVE;
		g_dewarp.m_pDewarp_objs[i]->base_addr = baseAddr_DEWARP;
		g_dewarp.enablePrevRqstComparision = 0;

		g_dewarp.m_pDewarp_objs[i]->m_queue.OutRqstQ = &(g_dewarp.m_OutFrameQ);
		g_dewarp.m_pDewarp_objs[i]->m_queue.ProcessRqstQ = &(g_dewarp.m_ProcessFrameQ);
		isp_rqstq_reset(&(g_dewarp.m_pDewarp_objs[i]->m_queue.InRqstQ));
		isp_rqstq_reset(&(g_dewarp.m_pDewarp_objs[i]->m_bcmbuf_list));

		mutex_init(&(g_dewarp.m_pDewarp_objs[i]->m_queue.InRqstQ.isp_rqstq_lock));
	}
	memset(&(g_dewarp.prvDwpRqst), -1, sizeof(struct ISP_BE_RQST_MSG));

	g_dewarp.bcmbufQ.queueCfg.maxQueueLen = 2;
	g_dewarp.bcmbufQ.queueCfg.maxBcmBuf = 2;
	g_dewarp.bcmbufQ.queueCfg.bcmBufSize = BCM_BUFFER_SIZE;
	g_dewarp.bcmbufQ.queueCfg.maxDhubCfgQ = 2;
	g_dewarp.bcmbufQ.queueCfg.dmaCmdSize = DMA_CMD_BUFFER_SIZE;
	ISPSS_BCMBUF_QUEUE_Reset(&g_dewarp.bcmbufQ);
	ISPSS_BCMBUF_QUEUE_Create(&g_dewarp.bcmbufQ);
	g_dewarp.autopush_user_config = 0;
	g_dewarp.autopush_framecount = 0;

	Ret = ISPSS_BE_Lut_Init();

e_ISPSS_BE_DEWARP_Init:
	if (Ret == ISPSS_OK)
		DWPLOGE("DEWARP Init Successful. %d Objects Created\n", MAX_DEWARP_OBJECTS);
	else
		DWPLOGE("DEWARP Init Failed\n");

	return Ret;
}


/**************************************************
 * FUNCTION: Destroy all DeWarp Objects
 * PARAMS: None
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_EHARDWAREBUSY - Hardware Busy
 *
 **************************************************/
INT ISPSS_BE_DEWARP_Destroy(void)
{
	HRESULT Ret = ISPSS_OK;
	struct DEWARP_OBJ *dwpTmpObj;
	INT i;

	/* If H/W is still processing the frame can't destroy the object now */
	if (g_dewarp.m_dwpHWStatus == DEWARP_HW_STATUS_RUNNING) {
		Ret = ISPSS_EHARDWAREBUSY;
		goto e_DEWARP_Destroy;
	}

	for (i = 0; i < MAX_DEWARP_OBJECTS; i++) {
		dwpTmpObj = GET_DWP_OBJ(i);
		/*
		 * If closing client has any of its buffers pending to be processed in
		 * any of the Queues don't allow to close
		 */
		if (dwpTmpObj->m_queue.InputQCnt || dwpTmpObj->m_queue.ProcessQCnt ||
					dwpTmpObj->m_queue.OutputQCnt) {
			Ret = ISPSS_EQNOTEMPTY;
			goto e_DEWARP_Destroy;
		}
		if (dwpTmpObj->m_objStatus == DEWARP_OBJ_STATUS_ACTIVE) {
			DWPLOGE("DEWARP Client is not closed - %d\n", i);
			Ret = ISPSS_EQNOTEMPTY;
			goto e_DEWARP_Destroy;
		}
	}
	Ret = ISPSS_BE_Lut_Destroy();
	if (Ret != ISPSS_OK)
		goto e_DEWARP_Destroy;

	//All clients are closed, so release client memory
	for (i = 0; i < MAX_DEWARP_OBJECTS; i++) {
		mutex_destroy(&(g_dewarp.m_pDewarp_objs[i]->m_queue.InRqstQ.isp_rqstq_lock));
		ispSS_SHM_Release(g_dewarp.shmHandle[i]);
		g_dewarp.m_pDewarp_objs[i] = NULL;
	}

	mutex_destroy(&g_dewarp.isr_lock);
	mutex_destroy(&(g_dewarp.m_OutFrameQ.isp_rqstq_lock));
	mutex_destroy(&(g_dewarp.m_ProcessFrameQ.isp_rqstq_lock));

	ISPSS_BCMBUF_QUEUE_Destroy(&g_dewarp.bcmbufQ);

	g_dewarp.magic = 0;
	/* TODO:Release the matrix buffer when Algo comes in */

e_DEWARP_Destroy:
	if (Ret == ISPSS_OK)
		DWPLOGE("DEWARP Destroy Successful\n");
	else
		DWPLOGE("DEWARP Destroy Failed\n");

	return Ret;
}


/*****************************************************************
 * FUNCTION: Open DeWarp Object and return client ID
 * PARAMS: iClientId - ID of the Client.
 *         iPriority - Client priority
 * RETURN: ISPSS_OK - succeed
 *
 *****************************************************************/
INT ISPSS_BE_DEWARP_Open(INT *iClientId, INT iPriority)
{
	HRESULT Ret = ISPSS_OK;
	INT index;

	for (index = 0; index < MAX_DEWARP_OBJECTS; index++) {
		if (g_dewarp.m_pDewarp_objs[index]->m_iValid) {
			if (g_dewarp.m_pDewarp_objs[index]->m_objStatus ==
				DEWARP_OBJ_STATUS_INACTIVE) {
				*iClientId = index;
				g_dewarp.m_pDewarp_objs[index]->m_dwpClientId = index;
				g_dewarp.m_pDewarp_objs[index]->m_dwpPriority = iPriority;
				g_dewarp.m_pDewarp_objs[index]->m_iValid = 1;
				g_dewarp.m_pDewarp_objs[index]->m_queue.InputQCnt = 0;
				g_dewarp.m_pDewarp_objs[index]->m_queue.ProcessQCnt = 0;
				g_dewarp.m_pDewarp_objs[index]->m_queue.OutputQCnt = 0;
				g_dewarp.m_pDewarp_objs[index]->m_objStatus =
				    DEWARP_OBJ_STATUS_ACTIVE;
				break;
			}
		}
	}

	if (index >= MAX_DEWARP_OBJECTS) {
		Ret = ISPSS_ENODEV;
		DWPLOGE("DEWARP Open Failed\n");
		*iClientId = -1;
	}

	return Ret;
}


/**************************************************
 * FUNCTION: Close DeWarp Object
 * PARAMS: None
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_ENODEV - No device
 *         ISPSS_EHARDWAREBUSY - Hardware busy
 *
 **************************************************/
INT ISPSS_BE_DEWARP_Close(INT iClientId)
{
	HRESULT Ret = ISPSS_OK;
	struct DEWARP_OBJ *dwpTmpObj;

	if (iClientId > MAX_DEWARP_OBJECTS || iClientId < 0) {
		Ret = ISPSS_ENODEV;
		goto e_DEWARP_Close;
	}

	dwpTmpObj = GET_DWP_OBJ(iClientId);
	if (dwpTmpObj->m_objStatus == DEWARP_OBJ_STATUS_INACTIVE) {
		Ret = ISPSS_ENODEV;
		goto e_DEWARP_Close;
	}

	/* If H/W is still processing the frame can't close the object now */
	if (dwpTmpObj->m_queue.ProcessQCnt) {
		pr_debug("ProcessQCnt: %d\n", dwpTmpObj->m_queue.ProcessQCnt);
		Ret = ISPSS_EHARDWAREBUSY;
		goto e_DEWARP_Close;
	}

	/*
	 * If closing client has any of its buffers pending to be processed in
	 * any of the Queues don't allow to close
	 */
	if (dwpTmpObj->m_queue.InputQCnt || dwpTmpObj->m_queue.OutputQCnt) {
		Ret = ISPSS_EQNOTEMPTY;
		goto e_DEWARP_Close;
	}

	dwpTmpObj->m_dwpClientId = -1;
	dwpTmpObj->m_dwpPriority = -1;
	dwpTmpObj->m_iValid = 1;
	dwpTmpObj->m_objStatus = DEWARP_OBJ_STATUS_INACTIVE;

e_DEWARP_Close:
	if (Ret == ISPSS_OK)
		DWPLOGE("DEWARP Object %d Closed\n", iClientId);
	else
		DWPLOGE("DEWARP Error in Closing Object %d. Error = %d\n", iClientId, Ret);

	return Ret;
}


/****************************************************************
 * FUNCTION: Initialize the DISIdx CIS Mat in
 dwp_lgdc of g_dewarp
 * PARAMS: sensorID - Sensor ID 0/1
 shmHandle - Handle to Index, Matrix buffer
 If NULL, use internal algo to generate
 disidx_cismat_sz - Buffer size
 * RETURN: ISPSS_OK - succeed
 *
 ****************************************************************/
INT ISPSS_BE_DEWARP_LGDC_Config(INT sensorID, SHM_HANDLE *shmHandle,
	INT disidx_cismat_sz, struct ISPBE_DEWARP_LUT_CFG *lutCFG)
{
	HRESULT Ret = ISPSS_OK;
	SHM_HANDLE hPrevShm;
	INT active_lut_idx, free_lut_idx;
	struct ISP_BE_DEWARP_LGDC *active_lut = NULL;

	if (sensorID != 0 && sensorID != 1) {
		Ret = ISPSS_DWP_INVALID_SENSOR;
		goto e_ISP_BE_LGDC_Config;
	}
	if (shmHandle == NULL) {
		DWPLOGE("DEWARP shmHandle is NULL\n");
		//TODO: Call the internal algo to generate Idx/Mat. Also accept camera parameters
	} else {
		/*
		 * TODO:Once Algo is available remove below assignment.
		 * Algo generator should set this
		 */
		active_lut_idx = g_dewarp.lut_ctx.active_lut[sensorID];
		free_lut_idx = DEWARP_GET_FREE_LUT_SLOT(active_lut_idx);

		/*
		 * Make sure active lut is currently not in use,
		 * because we are going to release the memory of the lut/matrix buffer
		 * on return of this call. shmHandle = hPrevShm
		 */
		if (g_dewarp.m_dwpHWStatus == DEWARP_HW_STATUS_RUNNING) {
			Ret = ISPSS_EHARDWAREBUSY;
			goto e_ISP_BE_LGDC_Config;
		} else if (g_dewarp.lut_ctx.dwpSwAlgoStatus == DEWARP_SW_ALGO_STATUS_RUNNING) {
			Ret = ISPSS_DWP_LUTGEN_INPROGRESS;
			goto e_ISP_BE_LGDC_Config;
		}
		active_lut = &g_dewarp.lut_ctx.dwp_lgdc[sensorID][free_lut_idx];
		if (active_lut_idx > -1)
			hPrevShm = active_lut->hShm;
		else
			hPrevShm = 0;

		/* Make free lut as new active lut */
		g_dewarp.lut_ctx.active_lut[sensorID] = free_lut_idx;
		active_lut = &g_dewarp.lut_ctx.dwp_lgdc[sensorID][free_lut_idx];
		active_lut->hShm = *shmHandle;
		active_lut->dwpDisCis_sz = disidx_cismat_sz;

		ispSS_SHM_GetVirtualAddress(*shmHandle, 0, &active_lut->addr);
		ispSS_SHM_GetPhysicalAddress(*shmHandle, 0, &active_lut->phy_addr);
		*shmHandle = hPrevShm;
		/*
		 * Copy the default Lut CFG for perticular sensor.
		 * Default cfg should be loaded from external cfg file
		 */
		memcpy(&(active_lut->lutCfg), lutCFG, sizeof(struct ISPBE_DEWARP_LUT_CFG));
		active_lut->isLutCfgAvailable = 1;
	}

e_ISP_BE_LGDC_Config:
	return Ret;
}

/*********************************************************
 * FUNCTION: Ge the number of frame waiting in the input Q
 * PARAMS: iClientId - Client ID
 *         puiFramesWaiting - No of frames waiting in input Q
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_ECLIENTID - Invalid client
 *
 ********************************************************/
INT ISPSS_BE_DEWARP_GetNoOfFramesWaiting(INT iClientId, UINT32 *puiFramesWaiting)
{
	HRESULT iResult = ISPSS_OK;

	if (iClientId >= 0 && iClientId < MAX_DEWARP_OBJECTS &&
					g_dewarp.m_pDewarp_objs[iClientId]->m_iValid) {
		mutex_lock(&g_dewarp.isr_lock);
		*puiFramesWaiting = g_dewarp.m_pDewarp_objs[iClientId]->m_queue.InputQCnt;
		mutex_unlock(&g_dewarp.isr_lock);
	} else {
		iResult = ISPSS_ECLIENTID;
	}
	return iResult;
}


/*********************************************************
 * FUNCTION: Ge the number of frame waiting in the queues
 * PARAMS: frameWaitCnt - wait count struct
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_ECLIENTID - Invalid client
 *
 ********************************************************/
INT ISPSS_BE_DEWARP_GetNoOfFramesWaiting_ClientQ(struct ISPBE_GET_FRAMEQ_WAIT_CNT *frameWaitCnt)
{
	HRESULT iResult = ISPSS_OK;
	INT index;
	INT iClientId = frameWaitCnt->iClientId;

	frameWaitCnt->noOfFramesWaitingInputQ = 0;
	frameWaitCnt->noOfFramesWaitingProcessQ = 0;
	frameWaitCnt->noOfFramesWaitingOutputQ = 0;

	mutex_lock(&g_dewarp.isr_lock);

	if (iClientId >= 0 && iClientId < MAX_DEWARP_OBJECTS) {
		if (g_dewarp.m_pDewarp_objs[iClientId]->m_iValid) {
			frameWaitCnt->noOfFramesWaitingInputQ =
					g_dewarp.m_pDewarp_objs[iClientId]->m_queue.InputQCnt;
			frameWaitCnt->noOfFramesWaitingProcessQ =
					g_dewarp.m_pDewarp_objs[iClientId]->m_queue.ProcessQCnt;
			frameWaitCnt->noOfFramesWaitingOutputQ =
					g_dewarp.m_pDewarp_objs[iClientId]->m_queue.OutputQCnt;
		} else {
			iResult = ISPSS_ECLIENTID;
		}
	} else {
		/* clientID is invlaid get sum of all client count */
		for (index = 0; index < MAX_DEWARP_OBJECTS; index++) {
			if (g_dewarp.m_pDewarp_objs[index]->m_iValid == 1) {
				frameWaitCnt->noOfFramesWaitingInputQ +=
					g_dewarp.m_pDewarp_objs[index]->m_queue.InputQCnt;
				frameWaitCnt->noOfFramesWaitingProcessQ +=
					g_dewarp.m_pDewarp_objs[index]->m_queue.ProcessQCnt;
				frameWaitCnt->noOfFramesWaitingOutputQ +=
					g_dewarp.m_pDewarp_objs[index]->m_queue.OutputQCnt;
			}
		}
	}

	mutex_unlock(&g_dewarp.isr_lock);

	return iResult;
}

/***********************************************
 * FUNCTION: Allocate memory for bcm buffers
 * PARAMS: pBcmBuf - BCM structure
 * RETURN: ISPSS_OK - succeed
 *
 ***********************************************/
INT ISPSS_BE_DEWARP_AllocateBcmStruct(struct ISP_BE_BCM *pBcmBuf)
{
	HRESULT iResult = ISPSS_OK;
	int i;

	iResult = ispSS_SHM_Allocate(SHM_NONSECURE, sizeof(struct BCMBUF), 1024,
			&pBcmBuf->shmHandle[0].hShm, SHM_NONSECURE_CONTIG);
	if (iResult != ISPSS_OK)
		goto e_AllocateBcmStruct;

	iResult = ispSS_SHM_Allocate(SHM_NONSECURE, sizeof(struct BCMBUF), 1024,
			&pBcmBuf->shmHandle[1].hShm, SHM_NONSECURE_CONTIG);
	if (iResult != ISPSS_OK)
		goto e_AllocateBcmStruct1;

	iResult = ispSS_SHM_Allocate(SHM_NONSECURE, sizeof(struct DHUB_CFGQ), 1024,
			&pBcmBuf->shmHandle[2].hShm, SHM_NONSECURE_CONTIG);
	if (iResult != ISPSS_OK)
		goto e_AllocateBcmStruct2;

	iResult = ispSS_SHM_Allocate(SHM_NONSECURE, sizeof(struct DHUB_CFGQ), 1024,
			&pBcmBuf->shmHandle[3].hShm, SHM_NONSECURE_CONTIG);
	if (iResult != ISPSS_OK)
		goto e_AllocateBcmStruct3;

	for (i = 0; i < 4; i++) {
		ispSS_SHM_GetVirtualAddress(pBcmBuf->shmHandle[i].hShm, 0,
			&pBcmBuf->shmHandle[i].addr);
		ispSS_SHM_GetPhysicalAddress(pBcmBuf->shmHandle[i].hShm, 0,
			&pBcmBuf->shmHandle[i].phy_addr);
	}

	pBcmBuf->bcm_buf = pBcmBuf->shmHandle[0].addr;
	pBcmBuf->clear_bcm_buf = pBcmBuf->shmHandle[1].addr;
	pBcmBuf->dma_cfgQ = pBcmBuf->shmHandle[2].addr;
	pBcmBuf->final_bcm_cfgQ = pBcmBuf->shmHandle[3].addr;

	memset(pBcmBuf->bcm_buf, 0, sizeof(struct BCMBUF));
	memset(pBcmBuf->clear_bcm_buf, 0, sizeof(struct BCMBUF));
	memset(pBcmBuf->dma_cfgQ, 0, sizeof(struct DHUB_CFGQ));
	memset(pBcmBuf->final_bcm_cfgQ, 0, sizeof(struct DHUB_CFGQ));

	return iResult;

e_AllocateBcmStruct3:
	ispSS_SHM_Release(pBcmBuf->shmHandle[2].hShm);
e_AllocateBcmStruct2:
	ispSS_SHM_Release(pBcmBuf->shmHandle[1].hShm);
e_AllocateBcmStruct1:
	ispSS_SHM_Release(pBcmBuf->shmHandle[0].hShm);
e_AllocateBcmStruct:
	return iResult;
}

/*******************************************************************
 * FUNCTION: Check if the current request is same as previous
 * PARAMS: pstdewarpRqstMsg - Request message
 * RETURN: ISPSS_OK - succeed
 *
 ********************************************************************/
INT ISPSS_BE_DEWARP_IsSameRequest(struct ISP_BE_RQST_MSG *pRqstMsg)
{
	INT isSameRqst = 1;
	struct ISP_BE_RQST_MSG *prvRqstMsg = &g_dewarp.prvDwpRqst.prvRqstMsg;

	if ((pRqstMsg->sensorID != prvRqstMsg->sensorID) ||
		(pRqstMsg->in_frame_fmt) != (prvRqstMsg->in_frame_fmt) ||
		(pRqstMsg->in_frame_bit_depth != prvRqstMsg->in_frame_bit_depth) ||
		(pRqstMsg->active.width != prvRqstMsg->active.width) ||
		(pRqstMsg->active.height != prvRqstMsg->active.height)) {
		isSameRqst = 0;
	}

	return isSameRqst;
}

/*******************************************************************
 * FUNCTION: Copy BCM buffer from BCMQ
 * PARAMS: pBcmBuf - BCM buffer
 *         pBcmBufQ - BCM Queue
 * RETURN: ISPSS_OK - succeed
 *
 ********************************************************************/
static void ISPSS_BE_DEWARP_CopyFromBcmBufQ(struct ISP_BE_BCM *pBcmBuf,
	struct ISPSS_BCMBUF_ITEM *pBcmBufQ)
{
	pBcmBuf->bcm_buf = &pBcmBufQ->pBcmBuf[ISPSS_BCMBUF_ITEM_PROGRAM_BCM_NDX];
	pBcmBuf->clear_bcm_buf =  &pBcmBufQ->pBcmBuf[ISPSS_BCMBUF_ITEM_CLEAR_BCM_NDX];
	pBcmBuf->dma_cfgQ = &pBcmBufQ->pCfgQ[ISPSS_CFGQ_ITEM_DHUB_CFGQ_NDX];
	pBcmBuf->final_bcm_cfgQ =  &pBcmBufQ->pCfgQ[ISPSS_CFGQ_ITEM_FINAL_CGQ_NDX];
}

/*******************************************************************
 * FUNCTION: Prepare request buffer when there is not input
 *           frame properties are changed
 * PARAMS: pendRqstMsg - Request buffer
 *         dwpObj - Dewarp Object
 * RETURN: ISPSS_OK - succeed
 *
 ********************************************************************/
INT ISPSS_BE_DEWARP_PrepareRequestForNoConfigChange(struct ISP_BE_RQST_MSG *pendRqstMsg,
	struct DEWARP_OBJ *dwpObj)
{
	ISPSS_BCMBUF_QUEUE_Element_Select(&g_dewarp.bcmbufQ, pendRqstMsg->pCurrBcmBuf, CPCB_1);
	ISPSS_BE_DEWARP_CopyFromBcmBufQ(pendRqstMsg->pBcmBuf, pendRqstMsg->pCurrBcmBuf);

	ISPSS_BE_DEWARP_FunctionalReset(pendRqstMsg);

	ISPSS_BE_DEWARP_ReadClient(pendRqstMsg, dwpObj);
	/* Start dewarping sequence */
	ISPSS_BE_DEWARP_Start(pendRqstMsg, dwpObj);
	pendRqstMsg->FrameState = 0; //Actual H/W processing not done yet
	pendRqstMsg->bcm_prepared = 1;  //BCM preparation done

	return 0;
}

/*******************************************************************
 * FUNCTION: Prepare BCM buffer
 * PARAMS: pendRqstMsg - Request buffer
 * PARAMS: dwpObj - DEWARP Object
 * RETURN: ISPSS_OK - succeed
 *
 ********************************************************************/
INT ISPSS_BE_DEWARP_PrepareRequest(struct ISP_BE_RQST_MSG *pendRqstMsg,
	struct DEWARP_OBJ *dwpObj)
{
	HRESULT Ret = ISPSS_OK;
	INT sensorID = pendRqstMsg->sensorID;
	INT lutBufferId = g_dewarp.lut_ctx.active_lut[sensorID];

	/* if BCM BCM is already prepared and previous request is same request */
	if (!pendRqstMsg->bcm_prepared) {
		/*
		 * If algo is under process of generating the LUT
		 * then request has to be pushed to inputQ
		 */
		Ret = ISPSS_BE_DEWARP_CheckLutStatus(pendRqstMsg);

		if (Ret == DEWARP_SW_ALGO_STATUS_RUNNING)
			return ISPSS_DWP_LUTGEN_INPROGRESS;

		/* If not same request, generate the lut */
		if (Ret == ISPSS_DWP_LUT_NOTPRESENT)
			pr_err("%s: SHOULD NOT LAND HERE !!\n", __func__);

		/* keep track the number of requests utilizing the active lut */
		/*
		 * If swapping out of LUT is required then you have to make sure that
		 * active lut has 0 rqstPreparedCnt
		 */
		mutex_lock(&g_dewarp.lut_ctx.dwp_lgdc[sensorID][lutBufferId].active_rqst_lock);
		g_dewarp.lut_ctx.dwp_lgdc[sensorID][lutBufferId].rqstPreparedCnt++;
		mutex_unlock(&g_dewarp.lut_ctx.dwp_lgdc[sensorID][lutBufferId].active_rqst_lock);

		ISPSS_BCMBUF_QUEUE_Element_Select(&g_dewarp.bcmbufQ,
					pendRqstMsg->pCurrBcmBuf, CPCB_1);
		ISPSS_BE_DEWARP_CopyFromBcmBufQ(pendRqstMsg->pBcmBuf, pendRqstMsg->pCurrBcmBuf);

		ISPSS_BE_DEWARP_FunctionalReset(pendRqstMsg);

		ISPSS_BE_DEWARP_Config_LGDC_TG(pendRqstMsg, dwpObj);
		ISPSS_BE_DEWARP_Config_LGDC(pendRqstMsg, dwpObj);

		ISPSS_BE_DEWARP_Config_LDI(pendRqstMsg, dwpObj);

		/* Start dewarping sequence */
		ISPSS_BE_DEWARP_Start(pendRqstMsg, dwpObj);

		//Generate cfgQ
		ISPSS_BCMBUF_To_CFGQ(pendRqstMsg->pBcmBuf->clear_bcm_buf,
					pendRqstMsg->pBcmBuf->final_bcm_cfgQ);
		ISPSS_BCMBUF_To_CFGQ(pendRqstMsg->pBcmBuf->bcm_buf,
					pendRqstMsg->pBcmBuf->final_bcm_cfgQ);
		ISPSS_CFGQ_To_CFGQ(pendRqstMsg->pBcmBuf->dma_cfgQ,
					pendRqstMsg->pBcmBuf->final_bcm_cfgQ);

		pendRqstMsg->FrameState = 0; //Actual H/W processing not done yet
		pendRqstMsg->bcm_prepared = 1;  //BCM preparation done
	}

	return ISPSS_OK;
}


/*******************************************************************
 * FUNCTION: Clear the cache for bcm buffers
 * PARAMS: pstdwpRqstMsg - Request message
 *
 * RETURN: NONE.
 *
 ********************************************************************/
void ISPSS_BE_DEWARP_BCM_ClearCache(struct ISP_BE_RQST_MSG *pstdwpRqstMsg)
{
	struct ISP_BE_BCM *pBcmBuf = NULL;

	pBcmBuf = ISPSS_GET_BCMBUF(pstdwpRqstMsg);

	ispSS_SHM_CleanCache(pBcmBuf->shmHandle[0].hShm, 0, sizeof(struct BCMBUF));
	ispSS_SHM_CleanCache(pBcmBuf->shmHandle[1].hShm, 0, sizeof(struct BCMBUF));
	ispSS_SHM_CleanCache(pBcmBuf->shmHandle[2].hShm, 0, sizeof(struct DHUB_CFGQ));
	ispSS_SHM_CleanCache(pBcmBuf->shmHandle[3].hShm, 0, sizeof(struct DHUB_CFGQ));
}


/*******************************************************************
 * FUNCTION: Submit the request message to hardware
 *           If dwp_WaitforIntr is set then this function will pole
 *           for H/W semaphore to be set.
 * PARAMS: pstdewarpRqstMsg - Request message
 * dwpObj - Dewarp object
 * RETURN: ISPSS_OK - succeed
 *
 ********************************************************************/
INT ISPSS_BE_DEWARP_SubmitToHardWare(struct ISP_BE_RQST_MSG *pstRqstMsg,
	struct DEWARP_OBJ *dwpObj, int qid)
{
	HRESULT Ret = ISPSS_OK;

	g_dewarp.m_dwpHWStatus = DEWARP_HW_STATUS_RUNNING;

	ISPSS_BE_DEWARP_SubmitHW(pstRqstMsg, dwpObj, qid);

	//Wait for the interrupt
	if (pstRqstMsg->WaitforIntr) {
		static int timeoutCount = ISPSS_BE_DEWARP_BLOCKING_ISR_TIMEOUT_IN_100MS;

		while (timeoutCount) {
			DWPLOGI("Waiting for interrupt\n");
			if (ISPSS_BE_DEWARP_FrameInterrupt(pstRqstMsg, dwpObj)) {
				/*
				 * Check the LDI status, in case of failure following
				 * status bits of LDI will be set
				 */
				Ret = ISPSS_BE_DEWARP_Chk_LDI_status(pstRqstMsg, dwpObj);
				if (Ret != ISPSS_OK)
					DWPLOGE("DEWARP LDI Failure. Err Code=%x\n", Ret);
				else
					pstRqstMsg->FrameState = 1;

				break;
			} else {
				//wait for 100ms before polling again
				msleep(100);
			}
			timeoutCount--;
		}
	}

	return ISPSS_OK;
}


/*******************************************************************
 * FUNCTION: Prepare the BCM buffer and Submit the buffer to hardware
 * PARAMS: pendRqstMsg - Request message
 *         dwpObj - DeWarp Object
 *         qid - BCM Q id
 * RETURN: NONE.
 *
 ********************************************************************/
static INT ISPSS_BE_DEWARP_PrepareAndSubmitToBcm(struct ISP_BE_RQST_MSG *pendRqstMsg,
	struct DEWARP_OBJ *dwpObj, int qid)
{
	HRESULT Ret = ISPSS_OK;
	INT active_lut_idx = g_dewarp.lut_ctx.active_lut[pendRqstMsg->sensorID];
	struct ISPBE_DEWARP_LUT_CFG *dwpLutCfg =
		&g_dewarp.lut_ctx.dwp_lgdc[pendRqstMsg->sensorID][active_lut_idx].lutCfg;

	//1. If BCM is not prepared then prepare it now
	if (pendRqstMsg->bcm_prepared == 0) {
		Ret = ISPSS_BE_DEWARP_PrepareRequest(pendRqstMsg, dwpObj);
		if (Ret != ISPSS_OK)
			goto e_prepareAndSubmit;
	}

	//2. Only if dwp_SubmitHw is set BCM buffers are submitted to hardware
	if (pendRqstMsg->SubmitHw) {
		ISPSS_BE_DEWARP_SubmitToHardWare(pendRqstMsg, dwpObj, qid);

		/*
		 * Keep a copy of the processed frame
		 * This will be used to comare with current frame properties
		 * which inturn useful to reduce the BCM writes
		 */
		memcpy(&g_dewarp.prvDwpRqst.prvRqstMsg, pendRqstMsg,
			sizeof(struct ISP_BE_RQST_MSG));
		g_dewarp.prvDwpRqst.prvDwpLutCfg = dwpLutCfg;

		//Move to processed queue and move to out queue in next ISR
		isp_rqstq_push(&g_dewarp.m_ProcessFrameQ, pendRqstMsg);
		dwpObj->m_queue.ProcessQCnt++;
	}
e_prepareAndSubmit:
	return Ret;
}

/*******************************************************************
 * FUNCTION: Get next request from the input Queue
 * PARAMS: pdwpObj - DeWarp object
 * RiETURN: ISP_BE_DEWARP_RQST - Request message
 *
 ********************************************************************/
static struct ISP_BE_RQST_MSG *ISPSS_BE_DEWARP_GetNextRequest(struct DEWARP_OBJ **pdwpObj)
{
	INT index;
	struct ISP_BE_RQST_MSG *pendRqstMsg = NULL;
	struct DEWARP_OBJ *dwpObj = NULL;

	//get the next request from the highest priority client
	for (index = 0; index < MAX_DEWARP_OBJECTS; index++) {
		dwpObj = g_dewarp.m_pDewarp_objs[index];
		if (dwpObj->m_iValid) {
			if (dwpObj->m_objStatus == DEWARP_OBJ_STATUS_ACTIVE &&
					dwpObj->m_queue.InputQCnt >= 1) {
				isp_rqstq_pop(&dwpObj->m_queue.InRqstQ, (void **)&pendRqstMsg);
				break;
			}
		}
	}
	*pdwpObj = dwpObj;

	return pendRqstMsg;
}

/****************************************************************************
 * FUNCTION: Process request message
 *           This function commits the previously proccessed frame from
 *           proccess Queue to Output Queue. Prepare BCM buffer for the new
 *           request in the input Queue.
 * PARAMS: pendRqstMsg - Request message
 *         dwpObj - Object
 * RETURN: ISPSS_OK - succeed
 *
 ****************************************************************************/
INT ISPSS_BE_DEWARP_ProcessRequest(struct ISP_BE_RQST_MSG *pendRqstMsg,
	struct DEWARP_OBJ *dwpObj)
{
	HRESULT Ret = ISPSS_OK;
	struct ISP_BE_RQST_MSG *processedRqstMsg = NULL;
	INT sensorID  = 0;
	INT lutBufferId = g_dewarp.lut_ctx.active_lut[sensorID];
	//0. Commit the previously processed frame to outputQ
	isp_rqstq_pop(&g_dewarp.m_ProcessFrameQ, (void **)&processedRqstMsg);
	isp_rqstq_pop_commit(&g_dewarp.m_ProcessFrameQ);
	if (processedRqstMsg) {
		sensorID = processedRqstMsg->sensorID;

		isp_rqstq_push(&g_dewarp.m_OutFrameQ, processedRqstMsg);
		g_dewarp.m_pDewarp_objs[processedRqstMsg->m_BuffID]->m_queue.ProcessQCnt--;
		g_dewarp.m_pDewarp_objs[processedRqstMsg->m_BuffID]->m_queue.OutputQCnt++;

		/* Decrement the number frames using active_lut */
		mutex_lock(&g_dewarp.lut_ctx.dwp_lgdc[sensorID][lutBufferId].active_rqst_lock);
		g_dewarp.lut_ctx.dwp_lgdc[sensorID][lutBufferId].rqstPreparedCnt--;
		mutex_unlock(&g_dewarp.lut_ctx.dwp_lgdc[sensorID][lutBufferId].active_rqst_lock);
	}

	//1. Get the next request to be processed
	if ((pendRqstMsg == NULL) || (dwpObj == NULL)) {
		//get the next request from the highest priority client
		pendRqstMsg = ISPSS_BE_DEWARP_GetNextRequest(&dwpObj);
		Ret = ISPSS_BE_DEWARP_CheckLutStatus(pendRqstMsg);

		if (pendRqstMsg && (Ret == ISPSS_DWP_LUT_PRESENT &&
						Ret != ISPSS_DWP_LUTGEN_INPROGRESS)) {
			isp_rqstq_pop_commit(&dwpObj->m_queue.InRqstQ);
			dwpObj->m_queue.InputQCnt--;
		}
	}

	if (pendRqstMsg) {
		if (pendRqstMsg->pCurrBcmBuf == NULL &&
			(processedRqstMsg && processedRqstMsg->pCurrBcmBuf)) {
			/*
			 * If BCMBUF is not preallocated,
			 * the reuse the one released in this interrupt
			 */
			pendRqstMsg->pCurrBcmBuf = processedRqstMsg->pCurrBcmBuf;
			processedRqstMsg->pCurrBcmBuf = NULL;
		}

		//2. If BCM is not prepared then prepare it now
		//3. Only if dwp_SubmitHw is set BCM buffers are submitted to hardware
		if (g_dewarp.autopush_user_config == 1) {
			if (g_dewarp.autopush_framecount == 0) {
				DWPLOGI("%s:%d: Enabling autopush\n", __func__, __LINE__);
				ISPSS_BCMDHUB_AutoPush(0, BCM_SCHED_Q9, 1);
			}
			Ret = ISPSS_BE_DEWARP_PrepareAndSubmitToBcm(pendRqstMsg,
						dwpObj, BCM_SCHED_Q9);
			if (Ret != ISPSS_OK)
				goto e_ProcessRequest;
			g_dewarp.autopush_framecount++;
		}
		if (g_dewarp.autopush_framecount == 1 || g_dewarp.autopush_user_config == 0) {
			Ret = ISPSS_BE_DEWARP_PrepareAndSubmitToBcm(pendRqstMsg,
					dwpObj, g_dewarp.commit_QId);
			if (Ret != ISPSS_OK)
				goto e_ProcessRequest;
		}
	} else {
		//If no more frame to process, then move to IDLE state
		g_dewarp.m_dwpHWStatus = DEWARP_HW_STATUS_IDLE;
	}

	if (processedRqstMsg && processedRqstMsg->pCurrBcmBuf) {
		//If the current BCMBUF is not required, then recycle it
		ISPSS_BCMBUF_QUEUE_push(&g_dewarp.bcmbufQ, processedRqstMsg->pCurrBcmBuf);
		//Ensure that internal BCMBUF is not leaked out to the caller
		processedRqstMsg->pCurrBcmBuf = NULL;
	}

	if (!(g_dewarp.commit_QId == DEWARP_COMMIT_QUEUE_ID_12 ||
		g_dewarp.commit_QId == DEWARP_COMMIT_QUEUE_ID_13)) {
		//Submit as much s possible (Max 2 since BCMQ depth is 2) to BCMQ
		while (1) {
			int bcmQFullStatus = 0;

			//get the next request from the highest priority client
			pendRqstMsg = ISPSS_BE_DEWARP_GetNextRequest(&dwpObj);
			Ret = ISPSS_BE_DEWARP_CheckLutStatus(pendRqstMsg);
			if (pendRqstMsg && (Ret == ISPSS_DWP_LUT_PRESENT &&
					Ret != ISPSS_DWP_LUTGEN_INPROGRESS)) {
				isp_rqstq_pop_commit(&dwpObj->m_queue.InRqstQ);
				dwpObj->m_queue.InputQCnt--;
			}

			//get the free BCMBUF
			if (pendRqstMsg && !pendRqstMsg->pCurrBcmBuf)
				ISPSS_BCMBUF_QUEUE_pop_and_commit(&g_dewarp.bcmbufQ,
					(void **)&pendRqstMsg->pCurrBcmBuf);

			//Check hardware BCMBQ status
			BCM_SCHED_GetFullSts(g_dewarp.commit_QId, &bcmQFullStatus);

			//Prepare & Submit only if valid next request, BCM and BCMQ is avialable
			if (!bcmQFullStatus && pendRqstMsg && pendRqstMsg->pCurrBcmBuf) {
				Ret = ISPSS_BE_DEWARP_PrepareAndSubmitToBcm(pendRqstMsg,
					dwpObj, g_dewarp.commit_QId);
				if (Ret != ISPSS_OK)
					goto e_ProcessRequest;
			} else {
				//Cannot proceed further to submit
				break;
			}
		}
	}
e_ProcessRequest:
	return Ret;
}


/******************************************************************************
 * FUNCTION: Interrupt routine
 *           If there is any other request in the input Queue process that too.
 *           Call the callback registered from application. If there is nothing
 *           to be processed in the ProcessQ, turn off the TG
 * PARAMS: intrNum - interrupt number
 *         pArgs - argumnets
 * RETURN: ISPSS_OK - succeed
 *
 *******************************************************************************/
int ISPSS_BE_DEWARP_ProcessISR(UINT32 intrNum, void *pArgs)
{

	//Reset DeWarp TG if there is no frame in the queue
	if (!isp_rqstq_get_count(&g_dewarp.m_ProcessFrameQ))
		ISPSS_BE_DEWARP_Reset();

	mutex_lock(&g_dewarp.isr_lock);

	//Process the next available request
	ISPSS_BE_DEWARP_ProcessRequest(NULL, NULL);

	mutex_unlock(&g_dewarp.isr_lock);

	return ISPSS_OK;
}

/***********************************************
 * FUNCTION: Push a frame for DeWarping
 * PARAMS: pstdewarpRqstMsg - Request message
 iClientId - Index to the object, if multiple objects exist
 * RETURN: ISPSS_OK - succeed
 *         ISPSS_ENOMEM - no memory
 ***********************************************/
INT ISPSS_BE_DEWARP_PushRequest(INT iClientId, struct ISP_BE_RQST_MSG *pstRqstMsg)
{
	HRESULT Ret = ISPSS_OK;
	struct DEWARP_OBJ *dwpObj;
	struct ISP_BE_RQST_MSG *pendRqstMsg = NULL;

	mutex_lock(&g_dewarp.isr_lock);

	// Fetch the object from object pool - TBD:
	// create the memory required by the client dynamically here
	dwpObj = g_dewarp.m_pDewarp_objs[iClientId];
	if (pstRqstMsg->m_BuffID != dwpObj->m_dwpClientId) {
		Ret = ISPSS_ECLIENTID;
		goto e_PushRequest;
	}

	//1. Create BCMBUF required to process the request - TBD:
	// introduce interface, so that application can preallocate buffer
	pstRqstMsg->pBcmBuf = &(pstRqstMsg->bcmBuf); //Else Copy from pCurrBcmBuf fails

	pstRqstMsg->bcm_prepared = 0;
	pstRqstMsg->pCurrBcmBuf = NULL;
	ISPSS_BCMBUF_QUEUE_pop_and_commit(&g_dewarp.bcmbufQ,
		(void **)&pstRqstMsg->pCurrBcmBuf);

	//2. If no change w.r.t the g_dewarp.prvStDewarpRqst,
	// then better prepare the BCMBUF here itself
	if (pstRqstMsg->pCurrBcmBuf) {
		if (ISPSS_BE_DEWARP_IsSameRequest(pstRqstMsg) && pstRqstMsg->bcm_prepared) {
#ifdef ISPSS_BE_DEWARP_PREPARE_NOCONFIG_CHANGE
			/*
			 * If same request, then we shall exersize
			 * ISPSS_BE_DEWARP_PrepareRequestForNoConfigChange()
			 * for min register programming
			 */
			Ret = ISPSS_BE_DEWARP_PrepareRequestForNoConfigChange(pstRqstMsg, dwpObj);
#else
			Ret = ISPSS_BE_DEWARP_PrepareRequest(pstRqstMsg, dwpObj);
#endif
		} else {
			Ret = ISPSS_BE_DEWARP_PrepareRequest(pstRqstMsg, dwpObj);
		}
	}

	/*
	 * 3. If H/W is still processing then push the frame into
	 * client queue and return immediately
	 */
	if (!pstRqstMsg->bcm_prepared || g_dewarp.m_dwpHWStatus == DEWARP_HW_STATUS_RUNNING ||
		Ret == ISPSS_DWP_LUTGEN_INPROGRESS) {
		//Push the Rqst msg to input Queue
		//DWPLOGI("DeWarp State RUNNING: Pushing to the Queue. - state:%d, bcmbuf:%x\n",
		//			g_dewarp.m_dwpHWStatus, pstRqstMsg->pCurrBcmBuf);
		isp_rqstq_push(&dwpObj->m_queue.InRqstQ, pstRqstMsg);

		dwpObj->m_queue.InputQCnt++;

		if (g_dewarp.m_dwpHWStatus == DEWARP_HW_STATUS_RUNNING)
			Ret = ISPSS_EHARDWAREBUSY;
		goto e_PushRequest;
	} else {
		pendRqstMsg = pstRqstMsg;
	}

	//4. Only if dwp_SubmitHw is set, then ProcessRequest = Prepare + submit
	if (pendRqstMsg->SubmitHw)
		Ret = ISPSS_BE_DEWARP_ProcessRequest(pendRqstMsg, dwpObj);

e_PushRequest:
	mutex_unlock(&g_dewarp.isr_lock);

	pstRqstMsg->ErrorCode = Ret;

	return Ret;
}


/***********************************************
 * FUNCTION: Pop a frame whose DeWarping is completed
 * PARAMS: pstdewarpRqstMsg - Request message processed successfully
 * RETURN: valid pointer if sucessfuly poped
 *         null pointer, no more request to pop
 ***********************************************/
INT ISPSS_BE_DEWARP_PopRequest(struct ISP_BE_RQST_MSG **pstRqstMsg)
{
	struct ISP_BE_RQST_MSG *rqstMsg = NULL;
	struct DEWARP_OBJ *dwpObj = NULL;

	if (isp_rqstq_pop(&g_dewarp.m_OutFrameQ, (void **)&rqstMsg)) {
		mutex_lock(&g_dewarp.isr_lock);

		isp_rqstq_pop_commit(&g_dewarp.m_OutFrameQ);
		dwpObj = GET_DWP_OBJ(rqstMsg->m_BuffID);
		dwpObj->m_queue.OutputQCnt--;

		mutex_unlock(&g_dewarp.isr_lock);
	}
	*pstRqstMsg = rqstMsg;

	return ISPSS_OK;
}

INT ISPSS_BE_DEWARP_IOCTL(enum ISP_BE_IOCTL_CMD ioctl_cmd, void *param)
{
	HRESULT Ret = ISPSS_OK;
	struct ISPBE_GET_FRAMEQ_WAIT_CNT *frameWaitCnt;
	struct ISP_BE_DEWARP_LGDC_CFG *lutParam;
	struct ISP_BE_DWP_LUT_FRAME_PARAM *frameParam;
	INT sensorID;
	SHM_HANDLE shmHandle;
	INT disidx_cismat_sz;

	switch (ioctl_cmd) {
	case ISP_BE_IOCTL_CMD_CONFIG:
		if (param != NULL) {
			lutParam = (struct ISP_BE_DEWARP_LGDC_CFG *)param;
			sensorID = lutParam->sensorId;
			shmHandle = lutParam->shmHandle;
			disidx_cismat_sz = lutParam->disidx_cismat_sz;
			ISPSS_BE_DEWARP_LGDC_Config(sensorID, &shmHandle,
							disidx_cismat_sz, &lutParam->lutCFG);
			lutParam->shmHandle = shmHandle;
		}
		break;
	case ISP_BE_IOCTL_CMD_GET_ALL_FRAMEQ_WAIT_CNT:
		if (param != NULL) {
			frameWaitCnt = (struct ISPBE_GET_FRAMEQ_WAIT_CNT *)param;
			Ret = ISPSS_BE_DEWARP_GetNoOfFramesWaiting_ClientQ(frameWaitCnt);
		}
		break;
	case ISP_BE_IOCTL_CMD_GET_FB_SIZE:
		if (param != NULL) {
			frameParam = (struct ISP_BE_DWP_LUT_FRAME_PARAM *)param;
			ISPSS_BE_DEWARP_GetFBSize(frameParam);
		}
		break;
	default:
		DWPLOGE("DEWARP: Invalid ioctl command\n");
	}

	return Ret;
}

static int ISPBE_LoadLut(struct ISP_BE_DEWARP_LGDC_CFG *cfg, char *lut_file)
{
	int result = 0;
	void *addr;
	const struct firmware *fw;

	result = request_firmware(&fw, lut_file, NULL);
	if (result) {
		pr_err("%s: ret=%d\n", __func__, result);
		goto exit;
	}

	if (ispSS_SHM_Allocate(SHM_NONSECURE, fw->size, 1024,
						&cfg->shmHandle, SHM_NONSECURE_CONTIG)) {
		pr_err("%s: there is error while allocating memory\n", __func__);
		result = -ENOMEM;
		goto free_fw;
	}
	ispSS_SHM_GetVirtualAddress(cfg->shmHandle, 0, &addr);

	memcpy(addr, fw->data, fw->size);
	ispSS_SHM_CleanCache(cfg->shmHandle, 0, fw->size);
	cfg->disidx_cismat_sz = fw->size;

free_fw:
	release_firmware(fw);
exit:
	return result;
}

void IPSBE_FreeLut(struct ISP_BE_DEWARP_LGDC_CFG *cfg)
{
	/* If LUT loading is successful free up the LUT */
	if (cfg->shmHandle) {
		ispSS_SHM_Release(cfg->shmHandle);
		cfg->shmHandle = 0;
	}
}

INT ISPBE_CA_LoadMatrix(int sensorId, char* lut_file,
			struct ISPBE_DEWARP_LUT_CFG *lutCFG)
{
	HRESULT Ret = S_OK;
	struct ISP_BE_DEWARP_LGDC_CFG cfg;

	cfg.sensorId = sensorId;

	memcpy(&cfg.lutCFG, lutCFG, sizeof(struct ISPBE_DEWARP_LUT_CFG));

	Ret = ISPBE_LoadLut(&cfg, lut_file);
	if (Ret != 0) {
		pr_err("%s ISPBE_LoadLut failed !!\n", __func__);
		return Ret;
	}

	if (cfg.disidx_cismat_sz && cfg.shmHandle) {
		Ret = ISPBE_MODULE_Ioctl(ISPBE_MODULE_DEWARP, ISP_BE_IOCTL_CMD_CONFIG, &cfg);
		if (Ret == S_OK)
			pr_err("%s:%d: pass lut to dwp engine done\n", __func__, __LINE__);
	}

	IPSBE_FreeLut(&cfg);

	return Ret;
}

void ISPSS_BE_DEWARP_Probe(struct ISPBE_CA_DRV_CTX *drv_ctx)
{
	drv_ctx->fops.module_init = ISPSS_BE_DEWARP_Init;
	drv_ctx->fops.module_destroy = ISPSS_BE_DEWARP_Destroy;
	drv_ctx->fops.module_open = ISPSS_BE_DEWARP_Open;
	drv_ctx->fops.module_close = ISPSS_BE_DEWARP_Close;
	drv_ctx->fops.module_ioctl = ISPSS_BE_DEWARP_IOCTL;
	drv_ctx->fops.module_GetNoOfFramesWaiting = ISPSS_BE_DEWARP_GetNoOfFramesWaiting;
	drv_ctx->fops.module_ProcessISR = ISPSS_BE_DEWARP_ProcessISR;
	drv_ctx->fops.module_PushRequest = ISPSS_BE_DEWARP_PushRequest;
	drv_ctx->fops.module_PopRequest = ISPSS_BE_DEWARP_PopRequest;
	drv_ctx->sem_id = ISP_DHUBSEM_TSB_dwrp_intr;
}
