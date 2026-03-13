/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2023 Synaptics Incorporated
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef __DEWARP_SYSFS_H__
#define __DEWARP_SYSFS_H__

int dewarp_create_sysfs_dev_files(struct dewarp_subdev_dev *dw_dev);
void dewarp_remove_sysfs_dev_files(struct dewarp_subdev_dev *dw_dev);

#endif

