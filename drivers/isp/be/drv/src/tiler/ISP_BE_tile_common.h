/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _DEWARP_COMMON_H_
#define _DEWARP_COMMON_H_
//#include "OSAL_api.h"

/* You can place the DEBUG macros here, which are common to all file */
#define DEWARP_DBG_ERROR(msg) (printf(msg))

enum CISP_PATH {
	CISP_TILE_PATH,
	CISP_RS_PATH
};

enum TILE_DATA_ALIGNEMENT {
	TILE_LSB_DATA_ALIGN,
	TILE_MSB_DATA_ALIGN,
};

#define CEIL(a, b) ((a / b) + ((a % b) != 0))

#endif

