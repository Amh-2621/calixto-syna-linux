/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __DEWARP_HELPER_APIS_H__
#define __DEWARP_HELPER_APIS_H__

INT ISPSS_BE_DEWARP_GetYUVMode(UINT32 format);
INT ISPSS_BE_DEWARP_GetBitDepth(INT bit_depth_idx);
INT ISPSS_BE_DEWARP_GetInput_Frame_Size(struct ISP_BE_RQST_MSG *rqstMsg);
INT ISPSS_BE_DEWARP_Getoutput_Frame_Size(struct ISP_BE_RQST_MSG *rqstMsg);
void ISPSS_BE_DEWARP_GetFBSize(struct ISP_BE_DWP_LUT_FRAME_PARAM *pstLutParam);

#endif
