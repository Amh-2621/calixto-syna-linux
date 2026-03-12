// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2023 Synaptics Incorporated
 *
 */

#ifndef __VPP_PRIV_VS6XXX
#define __VPP_PRIV_VS6XXX

#define VPP_REC_CLK_RATE_BOE	70220000
#define VPP_REC_CLK_RATE_WNC	74784000
#define VPP_REC_CLK_RATE_PANDA	138500000
#define VPP_REC_CLK_RATE_720P	74250000
#define VPP_REC_CLK_RATE_1080P	148500000

#define DISP_RES_HDMI_720P	0x00
#define DISP_RES_HDMI_1080P	0x01
#define DISP_RES_MIPI_BOE	0x02
#define DISP_RES_MIPI_PANDA	0x03
#define DISP_RES_MIPI_WNC	0x04

/* definition of VPP video outputs */
typedef enum {
	FIRST_VOUT     = 0,
	VOUT_HDMI      = 0,
	VOUT_HD        = 1,
	VOUT_SD        = 2,
	VOUT_DSI       = 3,
	MAX_NUM_VOUTS
} ENUM_VOUT_ID;

/* definition of VPP video connectors */
typedef enum {
	FIRST_VOUT_CONNECTOR   = 0,
	VOUT_CONNECTOR_HDMI    = 0,
	VOUT_CONNECTOR_DSI     = 1,
	MAX_NUM_VOUT_CONNECTORS
} ENUM_VOUT_CONNECTOR;

/* definition of VPP CPCB video outputs(for Berlin) */
typedef enum {
	CPCB_INVALID	= -1,
	FIRST_CPCB	= 0,
	CPCB_1		= 0,
#ifdef USE_DOLPHIN
	CPCB_2		= 1,
#endif

	MAX_NUM_CPCBS
} ENUM_CPCB_ID;

/* definition of VPP input planes */
typedef enum {
	FIRST_PLANE  = 0,
	PLANE_MAIN   = 0,
	PLANE_PIP    = 1,
	PLANE_GFX1   = 2,
#ifdef VPP_ALLOW_ALL_PLANES
#ifndef USE_DOLPHIN
	//USE_PLATYPUS
	PLANE_OVP_EL = 3,
#else
	PLANE_GFX2   = 3,
#endif
#ifndef USE_DOLPHIN
	PLANE_VMX     = PLANE_OVP_EL,
#else
	PLANE_AUX     = 4,
	PLANE_OVP_EL  = 5,
	PLANE_MAIN_EL = 6,
	PLANE_VMX     = 7,
#endif
#endif
	MAX_NUM_PLANES
} ENUM_PLANE_ID;


/* definition of scan information */
typedef enum {
	SCAN_DATA_INVALID = -1,
	FIRST_SCAN_DATA   = 0,
	SCAN_DATA_NONE	  = 0,
	OVER_SCAN_DATA	  = 1,
	UNDER_SCAN_DATA   = 2,
	MAX_SCAN_DATA
} ENUM_SCAN_DATA;

/* definition of VPP channels, namely pipelines */
typedef enum {
    CHAN_INVALID = -1,
    FIRST_CHAN   = 0,
    CHAN_MAIN    = 0,
    CHAN_PIP     = 1,
    CHAN_GFX1    = 2,
    CHAN_GFX2    = 3, //AUX channel for DP1->DP2 connection
    CHAN_AUX     = 4, //AUX channel for DP1->DP2 connection
    CHAN_OVP_EL  = 5,
    CHAN_MAIN_EL = 6,
    CHAN_VMX     = 7,
    MAX_NUM_CHANS
}ENUM_CHAN_ID;

#endif