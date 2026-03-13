/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef _ISPBE_DEBUG_H
#define _ISPBE_DEBUG_H

#define PRINT_LEVEL_NONE      0
#define PRINT_LEVEL_CRITICAL  1
#define PRINT_LEVEL_ERROR     2
#define PRINT_LEVEL_INFO      3
#define PRINT_LEVEL_DEBUG     4

extern int print_level;
#define print_crit(arg...)        \
do {                        \
	if (print_level >= PRINT_LEVEL_CRITICAL)    \
		pr_err(arg);   \
} while (0)

#define print_err(arg...)        \
do {                        \
	if (print_level >= PRINT_LEVEL_ERROR)    \
		pr_err(arg);   \
} while (0)

#define print_info(arg...)        \
do {                        \
	if (print_level >= PRINT_LEVEL_INFO)    \
		pr_err(arg);   \
} while (0)

#define print_debug(arg...)        \
do {                        \
	if (print_level >= PRINT_LEVEL_DEBUG)    \
		pr_err(arg);   \
} while (0)

#endif
