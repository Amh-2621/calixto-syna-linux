// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 */

#if !defined(__SYNA_VPP_FLINFO__)
#define __SYNA_VPP_FLINFO__

#define FASTLOGO_PREPEND_GENX_HEADER	400
#define FASTLOGO_HEADER	1024
#define FASTLOGO_FILE	"/dev/mmcblk0p"

typedef struct {
	unsigned int offset;
	unsigned int width;
	unsigned int height;
	unsigned int stride;
} fastlogo_info_t;

typedef struct {
	unsigned int versionNum;
	unsigned int logoNum;
	fastlogo_info_t info[];
} fastlogo_header_t;
#endif //__SYNA_VPP_FLINFO__
