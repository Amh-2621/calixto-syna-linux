// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2023 Synaptics Incorporated */

#ifndef _VPP_CFG_H_
#define _VPP_CFG_H_

typedef void (*CallbackHandler)(void* data);

typedef struct vpp_config_params {
	u32 frame_rate;
	u32 frame_size_ndx;
	int disp1_res_id;
	int disp1_out_type;
	int disp1_bit_depth;
	int disp1_colorformat;
	int disp2_res_id;
	int disp2_out_type;
	int display_mode;
	int sw_partition;
	int devnum;
	int hw_partition;
	// 0:buildin, 1:fastlogo
	u32 logo_enable;
	u32 fb_count;
	int active_planes, open_planes;
	int enable_frame_buf_copy;
	CallbackHandler callback;
	void *data;
	void *lcdc_config_params;
	void *mipi_lcdc_config_params;
	void *mipi_config_params;
	void *mipi_resinfo_params;
	VPP_MEM vpp_dsi_info_shm_handle;
	VPP_MEM vpp_resinfo_shm_handle;
	VPP_MEM vpp_cmdinfo_shm_handle;
	bool hdmitx_enable;
} vpp_config_params;
#endif
