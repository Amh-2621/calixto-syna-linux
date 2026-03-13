// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 */
 #ifndef __SYNA_LCDC_DEV_H_
 #define __SYNA_LCDC_DEV_H_

 #define SYNA_LCDC_INTF_TYPE_DPI    (SYNA_LCDC_TYPE_DPI_MCU | SYNA_LCDC_TYPE_DPI_RGB)
 #define SYNA_LCDC_INTF_TYPE_DSI    (SYNA_LCDC_TYPE_DSI_CMD)

 enum syna_lcdc_pix_fmt {
	SYNA_LCDC_PIXFMT_ARGB32,
	SYNA_LCDC_PIXFMT_ARGB32_PM,
	SYNA_LCDC_PIXFMT_RGB565,
	SYNA_LCDC_PIXFMT_RGB888,
	SYNA_LCDC_PIXFMT_MAX
};

enum syna_lcdc_pix_order {
	SYNA_LCDC_PIXORDER_XRGB = 0,
	SYNA_LCDC_PIXORDER_XBGR = 1,
	SYNA_LCDC_PIXORDER_RGBX = 2,
	SYNA_LCDC_PIXORDER_BGRX = 3,
	SYNA_LCDC_PIXORDER_MAX
};

typedef enum syna_lcdc_interface_t_ {
	SYNA_LCDC_TYPE_DPI_RGB   = 0x1, /*TFT + DSI_VIDEO*/
	SYNA_LCDC_TYPE_DPI_MCU   = 0x2,
	SYNA_LCDC_TYPE_DSI_CMD   = 0x4
} syna_lcdc_interface_t;

typedef enum syna_lcdc_output_mode_t_ {
	SYNA_LCDC_MODE_0    = 0x0,
	SYNA_LCDC_MODE_1    = 0x1,
	SYNA_LCDC_MODE_2    = 0x2,
	SYNA_LCDC_MODE_3    = 0x3
} syna_lcdc_output_mode_t;

enum SYNA_LCDC_NUM {
	SYNA_LCDC_1 = 0,
	SYNA_LCDC_2 = 1,
	SYNA_LCDC_MAX = 2
};
#endif