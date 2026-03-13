/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __DEWARP_PROCFS_H__
#define __DEWARP_PROCFS_H__

int dewarp_procfs_register(struct dewarp_subdev_dev *dw_dev,
                            unsigned long *pde);
void dewarp_procfs_unregister(unsigned long pde);

#endif

