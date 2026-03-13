/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __DEWARP_MODULE_H__
#define __DEWARP_MODULE_H__

#include "com_type.h"
#include "ispSS_rqstq.h"
#include "ispBE_module_common_priv.h"
#include "drv_msg.h"
#include "ispSS_bcmbuf_queue.h"

//DEWARP module magic number
#define DEWARP_MAGIC    0x1180FACE

#define MAX_DEWARP_OBJECTS 1
#define MAX_CAM_SENSORS 2
#define MAX_LUT_BUFFERS 2


#define ISPSS_BE_DEWARP_IS_BCMBUF_OPT_ENABLE() (g_dewarp.enablePrevRqstComparision)

#define DEWARP_COMMIT_QUEUE_ID_12    (12)
#define DEWARP_COMMIT_QUEUE_ID_13    (13)

#define DEWARP_GET_FREE_LUT_SLOT(active_idx) ((active_idx + 1) % MAX_LUT_BUFFERS)

struct ISP_BE_DEWARP_RQST_T;

struct DEWARP_OBJ {
	INT    base_addr;       // DEWARP hardware base address
	UINT   m_dwpPriority;
	INT    m_dwpClientId;       //To which client this object belongs to
	UINT   m_lgdc_base;         //TODO: Use this base address

	INT    m_objStatus;             //Status of object : Active/In-active.
	INT    m_iValid;               //Whether this object is valid or not

	struct ISPBE_CLIENT_QUEUE m_queue;

	struct ISP_REQUEST_QUEUE m_bcmbuf_list;
	struct task_struct *dwp_isr_task;

};

struct ISP_BE_DEWARP_LGDC {
	UINT dwpSensorId; //Current sensor under use
	/*
	 * Track the number of requests under processing using the active LUT.
	 * If count is not 0, don't swap out active LUT
	 */
	INT rqstPreparedCnt;
	INT isLutCfgAvailable;
	struct mutex active_rqst_lock;

	SHM_HANDLE hShm;
	void *addr;
	void *phy_addr;
	UINT dwpDisCis_sz; //Number of bytes in DisCIS matrix
	struct ISPBE_DEWARP_LUT_CFG lutCfg;
};

/* To store previous frame information */
struct ISP_BE_DEWARP_PRV_REQ {
	struct ISP_BE_RQST_MSG prvRqstMsg;
	struct ISPBE_DEWARP_LUT_CFG *prvDwpLutCfg;
};

struct ISP_BE_DWP_LUT_CTX {
	INT dwpSwAlgoStatus; //Algo status
	//There can be MAX_CAM_SENSORS availbale
	struct ISP_BE_DEWARP_LGDC dwp_lgdc[MAX_CAM_SENSORS][MAX_LUT_BUFFERS];

	INT active_lut[MAX_CAM_SENSORS]; //-1:no lut/matrix is active, 0/1:active lut index
	struct mutex active_lut_lock;

	AMPMsgQ_t dwp_lut_trigger_msgQ_handle;
	AMPMsgQ_t dwp_process_rqst_msgQ_handle;
	struct task_struct *dwp_Algo_Task;
	struct task_struct *dwp_ProcessRequest_Trigger_Task;
	INT dwpAlgoTaskExit;
	INT dwpProcessReqstTriggerTaskExit;
};

struct DEWARP {
	INT     magic;
	INT     m_dwpNoOfObjs;      //Number of DEWARP objects created.
	INT     m_dwpCurrObjUnderExec;  //Current object under execution
	INT     m_dwpHWStatus;          //Status of DEWARP engine, IDLE/RUNNING
	UINT    handle;
	INT     enablePrevRqstComparision;

	ISPSS_DWP_INTR_HANDLER IntrHandler;
	VOID *IntrHandlerArgs;
	INT IntCnt;

	//Save the previous frame. This can be compared with current one to reduce BCM writes
	struct ISP_BE_DEWARP_PRV_REQ prvDwpRqst;
	struct ISP_REQUEST_QUEUE  m_ProcessFrameQ; //Frames being processed - submitted to BCM
	struct ISP_REQUEST_QUEUE  m_OutFrameQ; //Frames processed
	struct mutex isr_lock;

	struct ISP_BE_DWP_LUT_CTX lut_ctx; /* Dewarp DisLut CisMatrix related data structure */

	SHM_HANDLE  shmHandle[MAX_DEWARP_OBJECTS];
	struct DEWARP_OBJ *m_pDewarp_objs[MAX_DEWARP_OBJECTS];

	INT autopush_user_config;
	INT autopush_framecount;
	INT commit_QId;
	struct ISPSS_BCMBUF_QUEUE bcmbufQ;

};

extern struct DEWARP g_dewarp;
#endif
