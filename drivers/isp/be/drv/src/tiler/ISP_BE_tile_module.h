/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __TILE_MODULE_H__
#define __TILE_MODULE_H__

#include "com_type.h"
#include "ispSS_rqstq.h"
#include "tileproc.h"

#define ISPSS_BE_TILE_ENABLE_BCMBUF_Q
#ifdef ISPSS_BE_TILE_ENABLE_BCMBUF_Q
#include "ispSS_bcmbuf_queue.h"
#endif //ISPSS_BE_TILE_ENABLE_BCMBUF_Q

#define MAX_TILE_OBJECTS  1

#define ISPSS_BE_TILE_IS_BCMBUF_OPT_ENABLE() (g_tile.enablePrevRqstComparision)
//TILE_DEBUG_DumpRegs(int base_addr);
//struct ISP_BE_RQST_MSG_T;

struct ISP_BE_TILE_OBJ {
	UINT   base_addr;
	INT    m_objStatus;             //Status of object : Active/In-active.
	INT    m_iValid;               //Whether this object is valid or not
};


struct ISP_BE_TILE_T {
	INT     m_tileNoOfObjs;    //Number of Tile objects created.
	INT     m_tileHWStatus;          //Status of TILE engine, IDLE/RUNNING
	UINT    handle;
	INT     enablePrevRqstComparision;
	//Flag indicating that tiler hw/sw restarted, reset after resubmitting rqst
	INT     restartFlag;

	//Save the previous frame. This can be compared with current one to reduce BCM writes
	struct ISP_BE_RQST_MSG prvStTileRqst;
	struct ISP_REQUEST_QUEUE  m_OutFrameQ; //Frames processed

	ISPSS_INTR_HANDLER IntrHandler;
	void *IntrHandlerArgs;
	int IntCnt;

	SHM_HANDLE  shmHandle[MAX_TILE_OBJECTS];
	struct ISP_BE_TILE_OBJ *m_pTile_obj[MAX_TILE_OBJECTS];

	INT autopush_user_config;
	INT autopush_framecount;
#ifdef ISPSS_BE_TILE_ENABLE_BCMBUF_Q
	struct ISPSS_BCMBUF_QUEUE bcmbufQ;
#endif //ISPSS_BE_TILE_ENABLE_BCMBUF_Q

};

#endif
