// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2025 Synaptics Incorporated */

#ifndef _VPP_BOOT_CONFIG_H_
#define _VPP_BOOT_CONFIG_H_

#include "vpp_config.h"

/**
 * VPP Boot Configuration Module Header
 *
 * This module provides functionality to read boot configuration files
 * and update VPP display parameters accordingly.
 */

/**
 * MV_VPP_ReadBootConfig - Interface to read boot configuration parameters
 * @config: Pointer to vpp_config_params
 * Reads /boot/res.txt file to get multiple display configuration parameters including
 * resolution, display mode, bits per pixel, and output format.
 * Returns: 0 on success, negative error code on failure
 */
int MV_VPP_ReadBootConfig(vpp_config_params *config);

/**
 * MV_VPP_WriteBootConfig - Interface to write boot configuration parameters
 * @config: Pointer to vpp_config_params containing parameters to write
 * Writes multiple display configuration parameters to /boot/res.txt file including
 * resolution, display mode, bits per pixel, and output format.
 * Returns: 0 on success, negative error code on failure
 */
int MV_VPP_WriteBootConfig(const vpp_config_params *config);

#endif /* _VPP_BOOT_CONFIG_H_ */
