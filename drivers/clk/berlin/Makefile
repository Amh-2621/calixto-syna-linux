# SPDX-License-Identifier: GPL-2.0-only
ifneq ($(filter y,$(CONFIG_MACH_BERLIN_BG2) $(CONFIG_MACH_BERLIN_BG2CD) $(CONFIG_MACH_BERLIN_BG2Q)),)
obj-y += berlin2-avpll.o berlin2-pll.o berlin2-div.o
endif

obj-$(CONFIG_SYNA_AS370_PLL)	+= as370-pll.o
obj-$(CONFIG_SYNA_DOLPHIN_PLL)	+= dolphin-pll.o
obj-$(CONFIG_SYNA_CLK_BASE)	+= syna-clk.o
syna-clk-y := clk.o gate.o
obj-$(CONFIG_SYNA_MYNA2_CLK)	+= myna2-clks.o
obj-$(CONFIG_SYNA_PLATYPUS_CLK)	+= platypus-clks.o
obj-$(CONFIG_SYNA_DOLPHIN_CLK)	+= dolphin-clks.o
obj-$(CONFIG_MACH_BERLIN_BG2)	+= bg2.o
obj-$(CONFIG_MACH_BERLIN_BG2CD)	+= bg2.o
obj-$(CONFIG_MACH_BERLIN_BG2Q)	+= bg2q.o
