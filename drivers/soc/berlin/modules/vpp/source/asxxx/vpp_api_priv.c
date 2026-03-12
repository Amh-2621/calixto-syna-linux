// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2025 Synaptics Incorporated
 *
 */

#include "vpp_api.h"
#include "hal_vpp_wrap.h"

int wrap_VPP_Init_Recovery(VPP_MEM_LIST *shm_list,
			int is_ampless_boot, vpp_config_params vpp_config_param)
{
	int res = -1;
	VPP_MIPI_CONFIG_PARAMS *pResCfg = vpp_config_param.mipi_resinfo_params;

	/* Always assume ampless boot and NTZ version flow for ASxxx family.
	 * Configure LCDC-0 and enable interrupt
	 */
	if (vpp_config_param.lcdc_config_params) {
		res = wrap_MV_VPP_LoadConfigTable(VOUT_TFT, SYNA_LCDC_1, vpp_config_param.lcdc_config_params);
		if (res) {
			pr_err("Failed Load For LCDC0\n");
			goto exit;
		}
	}

	/* Configure LCDC-1 and enable interrupt */
	if (pResCfg) {
		res = wrap_MV_VPP_LoadConfigTable(VOUT_TFT, SYNA_LCDC_2, pResCfg);
		if (res)
			pr_err("Failed Load CFG For LCDC1\n");
	}

exit:
	return res;
}
