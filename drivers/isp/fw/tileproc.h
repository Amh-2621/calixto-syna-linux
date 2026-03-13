#ifndef tileproc_h
#define tileproc_h (){}
#include "ctypes.h"
#pragma pack(1)
#ifdef __cplusplus
extern "C" {
#endif
#ifndef _DOCC_H_BITOPS_
#define _DOCC_H_BITOPS_ (){}
#define _bSETMASK_(b)				((b)<32 ? (1<<((b)&31)) : 0)
#define _NSETMASK_(msb,lsb)			(_bSETMASK_((msb)+1)-_bSETMASK_(lsb))
#define _bCLRMASK_(b)				(~_bSETMASK_(b))
#define _NCLRMASK_(msb,lsb)			(~_NSETMASK_(msb,lsb))
#define _BFGET_(r,msb,lsb)			(_NSETMASK_((msb)-(lsb),0)&((r)>>(lsb)))
#define _BFSET_(r,msb,lsb,v)		do{ (r)&=_NCLRMASK_(msb,lsb); \
					(r)|=_NSETMASK_(msb,lsb)&((v)<<(lsb)); }while(0)
#endif
#ifndef h_TG_PL
#define h_TG_PL (){}
#define     RA_TG_PL_X                                     0x0000
#define     RA_TG_PL_Y                                     0x0004
	typedef struct SIE_TG_PL {
#define     w32TG_PL_X                                     {\
	UNSG32 uX_start                                    : 13;\
	UNSG32 uX_end                                      : 13;\
	UNSG32 RSVDx0_b26                                  :  6;\
}
	union { UNSG32 u32TG_PL_X;
		struct w32TG_PL_X;
	};
#define     w32TG_PL_Y                                     {\
	UNSG32 uY_start                                    : 12;\
	UNSG32 uY_end                                      : 12;\
	UNSG32 RSVDx4_b24                                  :  8;\
}
union { UNSG32 u32TG_PL_Y;
	struct w32TG_PL_Y;
};
} SIE_TG_PL;
typedef union  T32TG_PL_X
{ UNSG32 u32;
	struct w32TG_PL_X;
} T32TG_PL_X;
typedef union  T32TG_PL_Y
{ UNSG32 u32;
	struct w32TG_PL_Y;
} T32TG_PL_Y;
typedef union  TTG_PL_X
{ UNSG32 u32[1];
	struct {
		struct w32TG_PL_X;
	};
} TTG_PL_X;
typedef union  TTG_PL_Y
{ UNSG32 u32[1];
	struct {
		struct w32TG_PL_Y;
	};
} TTG_PL_Y;
SIGN32 TG_PL_drvrd(SIE_TG_PL *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 TG_PL_drvwr(SIE_TG_PL *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void TG_PL_reset(SIE_TG_PL *p);
SIGN32 TG_PL_cmp(SIE_TG_PL *p, SIE_TG_PL *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define TG_PL_check(p,pie,pfx,hLOG) TG_PL_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define TG_PL_print(p,    pfx,hLOG) TG_PL_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_TG_PRG
#define h_TG_PRG (){}
#define     RA_TG_PRG_CTRL                                 0x0000
#define     RA_TG_PRG_CTRL1                                0x0004
#define     RA_TG_PRG_Total                                0x0008
#define     RA_TG_PRG_Initial                              0x000C
#define     RA_TG_PRG_HSYNC                                0x0010
#define     RA_TG_PRG_VSYNC                                0x0014
#define     RA_TG_PRG_VS                                   0x0018
#define     RA_TG_PRG_FT                                   0x001C
#define     RA_TG_PRG_VX                                   0x0020
typedef struct SIE_TG_PRG {
#define     w32TG_PRG_CTRL                                 {\
	UNSG32 uCTRL_mode                                  :  2;\
	UNSG32 uCTRL_lwin                                  :  8;\
	UNSG32 uCTRL_frst                                  : 12;\
	UNSG32 uCTRL_freeze                                : 10;\
}
union { UNSG32 u32TG_PRG_CTRL;
	struct w32TG_PRG_CTRL;
};
#define     w32TG_PRG_CTRL1                                {\
	UNSG32 uCTRL1_sync_ctrl                            :  2;\
	UNSG32 uCTRL1_res_change_en                        :  9;\
	UNSG32 RSVDx4_b11                                  : 21;\
}
union { UNSG32 u32TG_PRG_CTRL1;
	struct w32TG_PRG_CTRL1;
};
#define     w32TG_PRG_Total                                {\
	UNSG32 uTotal_vertical                             : 12;\
	UNSG32 uTotal_horizontal                           : 13;\
	UNSG32 RSVDx8_b25                                  :  7;\
}
union { UNSG32 u32TG_PRG_Total;
	struct w32TG_PRG_Total;
};
#define     w32TG_PRG_Initial                              {\
	UNSG32 uInitial_xi                                 : 13;\
	UNSG32 uInitial_yi                                 : 12;\
	UNSG32 RSVDxC_b25                                  :  7;\
}
union { UNSG32 u32TG_PRG_Initial;
	struct w32TG_PRG_Initial;
};
#define     w32TG_PRG_HSYNC                                {\
	UNSG32 uHSYNC_h_start                              : 13;\
	UNSG32 uHSYNC_h_end                                : 13;\
	UNSG32 RSVDx10_b26                                 :  6;\
}
union { UNSG32 u32TG_PRG_HSYNC;
	struct w32TG_PRG_HSYNC;
};
#define     w32TG_PRG_VSYNC                                {\
	UNSG32 uVSYNC_v_start                              : 12;\
	UNSG32 uVSYNC_v_end                                : 12;\
	UNSG32 RSVDx14_b24                                 :  8;\
}
union { UNSG32 u32TG_PRG_VSYNC;
	struct w32TG_PRG_VSYNC;
};
#define     w32TG_PRG_VS                                   {\
	UNSG32 uVS_h_start                                 : 13;\
	UNSG32 uVS_h_end                                   : 13;\
	UNSG32 RSVDx18_b26                                 :  6;\
}
union { UNSG32 u32TG_PRG_VS;
	struct w32TG_PRG_VS;
};
#define     w32TG_PRG_FT                                   {\
	UNSG32 uFT_frame                                   :  8;\
	UNSG32 RSVDx1C_b8                                  : 24;\
}
union { UNSG32 u32TG_PRG_FT;
	struct w32TG_PRG_FT;
};
#define     w32TG_PRG_VX                                   {\
	UNSG32 uVX_vx                                      : 13;\
	UNSG32 RSVDx20_b13                                 : 19;\
}
union { UNSG32 u32TG_PRG_VX;
	struct w32TG_PRG_VX;
};
} SIE_TG_PRG;
typedef union  T32TG_PRG_CTRL
{ UNSG32 u32;
	struct w32TG_PRG_CTRL;
} T32TG_PRG_CTRL;
typedef union  T32TG_PRG_CTRL1
{ UNSG32 u32;
	struct w32TG_PRG_CTRL1;
} T32TG_PRG_CTRL1;
typedef union  T32TG_PRG_Total
{ UNSG32 u32;
	struct w32TG_PRG_Total;
} T32TG_PRG_Total;
typedef union  T32TG_PRG_Initial
{ UNSG32 u32;
	struct w32TG_PRG_Initial;
} T32TG_PRG_Initial;
typedef union  T32TG_PRG_HSYNC
{ UNSG32 u32;
	struct w32TG_PRG_HSYNC;
} T32TG_PRG_HSYNC;
typedef union  T32TG_PRG_VSYNC
{ UNSG32 u32;
	struct w32TG_PRG_VSYNC;
} T32TG_PRG_VSYNC;
typedef union  T32TG_PRG_VS
{ UNSG32 u32;
	struct w32TG_PRG_VS;
} T32TG_PRG_VS;
typedef union  T32TG_PRG_FT
{ UNSG32 u32;
	struct w32TG_PRG_FT;
} T32TG_PRG_FT;
typedef union  T32TG_PRG_VX
{ UNSG32 u32;
	struct w32TG_PRG_VX;
} T32TG_PRG_VX;
typedef union  TTG_PRG_CTRL
{ UNSG32 u32[1];
	struct {
		struct w32TG_PRG_CTRL;
	};
} TTG_PRG_CTRL;
typedef union  TTG_PRG_CTRL1
{ UNSG32 u32[1];
	struct {
		struct w32TG_PRG_CTRL1;
	};
} TTG_PRG_CTRL1;
typedef union  TTG_PRG_Total
{ UNSG32 u32[1];
	struct {
		struct w32TG_PRG_Total;
	};
} TTG_PRG_Total;
typedef union  TTG_PRG_Initial
{ UNSG32 u32[1];
	struct {
		struct w32TG_PRG_Initial;
	};
} TTG_PRG_Initial;
typedef union  TTG_PRG_HSYNC
{ UNSG32 u32[1];
	struct {
		struct w32TG_PRG_HSYNC;
	};
} TTG_PRG_HSYNC;
typedef union  TTG_PRG_VSYNC
{ UNSG32 u32[1];
	struct {
		struct w32TG_PRG_VSYNC;
	};
} TTG_PRG_VSYNC;
typedef union  TTG_PRG_VS
{ UNSG32 u32[1];
	struct {
		struct w32TG_PRG_VS;
	};
} TTG_PRG_VS;
typedef union  TTG_PRG_FT
{ UNSG32 u32[1];
	struct {
		struct w32TG_PRG_FT;
	};
} TTG_PRG_FT;
typedef union  TTG_PRG_VX
{ UNSG32 u32[1];
	struct {
		struct w32TG_PRG_VX;
	};
} TTG_PRG_VX;
SIGN32 TG_PRG_drvrd(SIE_TG_PRG *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 TG_PRG_drvwr(SIE_TG_PRG *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void TG_PRG_reset(SIE_TG_PRG *p);
SIGN32 TG_PRG_cmp(SIE_TG_PRG *p, SIE_TG_PRG *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define TG_PRG_check(p,pie,pfx,hLOG) TG_PRG_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define TG_PRG_print(p,    pfx,hLOG) TG_PRG_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_TG
#define h_TG (){}
#define     RA_TG_INIT                                     0x0000
#define     RA_TG_SIZE                                     0x0004
#define     RA_TG_HS                                       0x0008
#define     RA_TG_HB                                       0x000C
#define     RA_TG_HB_CR                                    0x0010
#define     RA_TG_HB_CR2                                   0x0014
#define     RA_TG_VS0                                      0x0018
#define     RA_TG_VS1                                      0x001C
#define     RA_TG_VB0                                      0x0020
#define     RA_TG_VB0_CR                                   0x0024
#define     RA_TG_VB0_CR2                                  0x0028
#define     RA_TG_VB1                                      0x002C
#define     RA_TG_SCAN                                     0x0030
#define        TG_SCAN_MODE_PROG                                        0x0
#define        TG_SCAN_MODE_INTER                                       0x1
#define     RA_TG_INTPOS                                   0x0034
#define     RA_TG_MODE                                     0x0038
#define        TG_MODE_EN_MASTER                                        0x0
#define        TG_MODE_EN_SLAVE                                         0x1
#define     RA_TG_HVREF                                    0x003C
#define        TG_HVREF_SEL_SYNC                                        0x0
#define        TG_HVREF_SEL_BLANK                                       0x1
#define        TG_HVREF_POL_NEG_PULSE                                   0x0
#define        TG_HVREF_POL_POS_PULSE                                   0x1
typedef struct SIE_TG {
#define     w32TG_INIT                                     {\
	UNSG32 uINIT_Y                                     : 16;\
	UNSG32 uINIT_X                                     : 16;\
}
union { UNSG32 u32TG_INIT;
	struct w32TG_INIT;
};
#define     w32TG_SIZE                                     {\
	UNSG32 uSIZE_Y                                     : 16;\
	UNSG32 uSIZE_X                                     : 16;\
}
union { UNSG32 u32TG_SIZE;
	struct w32TG_SIZE;
};
#define     w32TG_HS                                       {\
	UNSG32 uHS_FE                                      : 16;\
	UNSG32 uHS_BE                                      : 16;\
}
union { UNSG32 u32TG_HS;
	struct w32TG_HS;
};
#define     w32TG_HB                                       {\
	UNSG32 uHB_FE                                      : 16;\
	UNSG32 uHB_BE                                      : 16;\
}
union { UNSG32 u32TG_HB;
	struct w32TG_HB;
};
#define     w32TG_HB_CR                                    {\
	UNSG32 uHB_CR_FE                                   : 16;\
	UNSG32 uHB_CR_BE                                   : 16;\
}
union { UNSG32 u32TG_HB_CR;
	struct w32TG_HB_CR;
};
#define     w32TG_HB_CR2                                   {\
	UNSG32 uHB_CR2_FE                                  : 16;\
	UNSG32 uHB_CR2_BE                                  : 16;\
}
union { UNSG32 u32TG_HB_CR2;
	struct w32TG_HB_CR2;
};
#define     w32TG_VS0                                      {\
	UNSG32 uVS0_FE                                     : 16;\
	UNSG32 uVS0_BE                                     : 16;\
}
union { UNSG32 u32TG_VS0;
	struct w32TG_VS0;
};
#define     w32TG_VS1                                      {\
	UNSG32 uVS1_FE                                     : 16;\
	UNSG32 uVS1_BE                                     : 16;\
}
union { UNSG32 u32TG_VS1;
	struct w32TG_VS1;
};
#define     w32TG_VB0                                      {\
	UNSG32 uVB0_FE                                     : 16;\
	UNSG32 uVB0_BE                                     : 16;\
}
union { UNSG32 u32TG_VB0;
	struct w32TG_VB0;
};
#define     w32TG_VB0_CR                                   {\
	UNSG32 uVB0_CR_FE                                  : 16;\
	UNSG32 uVB0_CR_BE                                  : 16;\
}
union { UNSG32 u32TG_VB0_CR;
	struct w32TG_VB0_CR;
};
#define     w32TG_VB0_CR2                                  {\
	UNSG32 uVB0_CR2_FE                                 : 16;\
	UNSG32 uVB0_CR2_BE                                 : 16;\
}
union { UNSG32 u32TG_VB0_CR2;
	struct w32TG_VB0_CR2;
};
#define     w32TG_VB1                                      {\
	UNSG32 uVB1_FE                                     : 16;\
	UNSG32 uVB1_BE                                     : 16;\
}
union { UNSG32 u32TG_VB1;
	struct w32TG_VB1;
};
#define     w32TG_SCAN                                     {\
	UNSG32 uSCAN_MODE                                  :  1;\
	UNSG32 RSVDx30_b1                                  : 31;\
}
union { UNSG32 u32TG_SCAN;
	struct w32TG_SCAN;
};
#define     w32TG_INTPOS                                   {\
	UNSG32 uINTPOS_FRAME                               : 16;\
	UNSG32 uINTPOS_FIELD                               : 16;\
}
union { UNSG32 u32TG_INTPOS;
	struct w32TG_INTPOS;
};
#define     w32TG_MODE                                     {\
	UNSG32 uMODE_EN                                    :  1;\
	UNSG32 RSVDx38_b1                                  : 31;\
}
union { UNSG32 u32TG_MODE;
	struct w32TG_MODE;
};
#define     w32TG_HVREF                                    {\
	UNSG32 uHVREF_SEL                                  :  1;\
	UNSG32 uHVREF_POL                                  :  1;\
	UNSG32 RSVDx3C_b2                                  : 30;\
}
union { UNSG32 u32TG_HVREF;
	struct w32TG_HVREF;
};
} SIE_TG;
typedef union  T32TG_INIT
{ UNSG32 u32;
	struct w32TG_INIT;
} T32TG_INIT;
typedef union  T32TG_SIZE
{ UNSG32 u32;
	struct w32TG_SIZE;
} T32TG_SIZE;
typedef union  T32TG_HS
{ UNSG32 u32;
	struct w32TG_HS;
} T32TG_HS;
typedef union  T32TG_HB
{ UNSG32 u32;
	struct w32TG_HB;
} T32TG_HB;
typedef union  T32TG_HB_CR
{ UNSG32 u32;
	struct w32TG_HB_CR;
} T32TG_HB_CR;
typedef union  T32TG_HB_CR2
{ UNSG32 u32;
	struct w32TG_HB_CR2;
} T32TG_HB_CR2;
typedef union  T32TG_VS0
{ UNSG32 u32;
	struct w32TG_VS0;
} T32TG_VS0;
typedef union  T32TG_VS1
{ UNSG32 u32;
	struct w32TG_VS1;
} T32TG_VS1;
typedef union  T32TG_VB0
{ UNSG32 u32;
	struct w32TG_VB0;
} T32TG_VB0;
typedef union  T32TG_VB0_CR
{ UNSG32 u32;
	struct w32TG_VB0_CR;
} T32TG_VB0_CR;
typedef union  T32TG_VB0_CR2
{ UNSG32 u32;
	struct w32TG_VB0_CR2;
} T32TG_VB0_CR2;
typedef union  T32TG_VB1
{ UNSG32 u32;
	struct w32TG_VB1;
} T32TG_VB1;
typedef union  T32TG_SCAN
{ UNSG32 u32;
	struct w32TG_SCAN;
} T32TG_SCAN;
typedef union  T32TG_INTPOS
{ UNSG32 u32;
	struct w32TG_INTPOS;
} T32TG_INTPOS;
typedef union  T32TG_MODE
{ UNSG32 u32;
	struct w32TG_MODE;
} T32TG_MODE;
typedef union  T32TG_HVREF
{ UNSG32 u32;
	struct w32TG_HVREF;
} T32TG_HVREF;
typedef union  TTG_INIT
{ UNSG32 u32[1];
	struct {
		struct w32TG_INIT;
	};
} TTG_INIT;
typedef union  TTG_SIZE
{ UNSG32 u32[1];
	struct {
		struct w32TG_SIZE;
	};
} TTG_SIZE;
typedef union  TTG_HS
{ UNSG32 u32[1];
	struct {
		struct w32TG_HS;
	};
} TTG_HS;
typedef union  TTG_HB
{ UNSG32 u32[1];
	struct {
		struct w32TG_HB;
	};
} TTG_HB;
typedef union  TTG_HB_CR
{ UNSG32 u32[1];
	struct {
		struct w32TG_HB_CR;
	};
} TTG_HB_CR;
typedef union  TTG_HB_CR2
{ UNSG32 u32[1];
	struct {
		struct w32TG_HB_CR2;
	};
} TTG_HB_CR2;
typedef union  TTG_VS0
{ UNSG32 u32[1];
	struct {
		struct w32TG_VS0;
	};
} TTG_VS0;
typedef union  TTG_VS1
{ UNSG32 u32[1];
	struct {
		struct w32TG_VS1;
	};
} TTG_VS1;
typedef union  TTG_VB0
{ UNSG32 u32[1];
	struct {
		struct w32TG_VB0;
	};
} TTG_VB0;
typedef union  TTG_VB0_CR
{ UNSG32 u32[1];
	struct {
		struct w32TG_VB0_CR;
	};
} TTG_VB0_CR;
typedef union  TTG_VB0_CR2
{ UNSG32 u32[1];
	struct {
		struct w32TG_VB0_CR2;
	};
} TTG_VB0_CR2;
typedef union  TTG_VB1
{ UNSG32 u32[1];
	struct {
		struct w32TG_VB1;
	};
} TTG_VB1;
typedef union  TTG_SCAN
{ UNSG32 u32[1];
	struct {
		struct w32TG_SCAN;
	};
} TTG_SCAN;
typedef union  TTG_INTPOS
{ UNSG32 u32[1];
	struct {
		struct w32TG_INTPOS;
	};
} TTG_INTPOS;
typedef union  TTG_MODE
{ UNSG32 u32[1];
	struct {
		struct w32TG_MODE;
	};
} TTG_MODE;
typedef union  TTG_HVREF
{ UNSG32 u32[1];
	struct {
		struct w32TG_HVREF;
	};
} TTG_HVREF;
SIGN32 TG_drvrd(SIE_TG *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 TG_drvwr(SIE_TG *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void TG_reset(SIE_TG *p);
SIGN32 TG_cmp  (SIE_TG *p, SIE_TG *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define TG_check(p,pie,pfx,hLOG) TG_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define TG_print(p,    pfx,hLOG) TG_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_TG_MAIN
#define h_TG_MAIN (){}
#define     RA_TG_MAIN_INIT                                0x0000
#define     RA_TG_MAIN_SIZE                                0x0004
#define     RA_TG_MAIN_HS                                  0x0008
#define     RA_TG_MAIN_HB                                  0x000C
#define     RA_TG_MAIN_HB_Y                                0x0010
#define     RA_TG_MAIN_HB_C                                0x0014
#define     RA_TG_MAIN_HB_OUT                              0x0018
#define     RA_TG_MAIN_VS0                                 0x001C
#define     RA_TG_MAIN_VS1                                 0x0020
#define     RA_TG_MAIN_VB0                                 0x0024
#define     RA_TG_MAIN_VB0_Y                               0x0028
#define     RA_TG_MAIN_VB0_C                               0x002C
#define     RA_TG_MAIN_VB0_OUT                             0x0030
#define     RA_TG_MAIN_VB1                                 0x0034
#define     RA_TG_MAIN_SCAN                                0x0038
#define        TG_MAIN_SCAN_MODE_PROG                                   0x0
#define        TG_MAIN_SCAN_MODE_INTER                                  0x1
#define     RA_TG_MAIN_INTPOS                              0x003C
#define     RA_TG_MAIN_MODE                                0x0040
#define        TG_MAIN_MODE_EN_MASTER                                   0x0
#define        TG_MAIN_MODE_EN_SLAVE                                    0x1
#define     RA_TG_MAIN_HVREF                               0x0044
#define        TG_MAIN_HVREF_SEL_SYNC                                   0x0
#define        TG_MAIN_HVREF_SEL_BLANK                                  0x1
#define        TG_MAIN_HVREF_POL_NEG_PULSE                              0x0
#define        TG_MAIN_HVREF_POL_POS_PULSE                              0x1
typedef struct SIE_TG_MAIN {
#define     w32TG_MAIN_INIT                                {\
	UNSG32 uINIT_Y                                     : 16;\
	UNSG32 uINIT_X                                     : 16;\
}
union { UNSG32 u32TG_MAIN_INIT;
	struct w32TG_MAIN_INIT;
};
#define     w32TG_MAIN_SIZE                                {\
	UNSG32 uSIZE_Y                                     : 16;\
	UNSG32 uSIZE_X                                     : 16;\
}
union { UNSG32 u32TG_MAIN_SIZE;
	struct w32TG_MAIN_SIZE;
};
#define     w32TG_MAIN_HS                                  {\
	UNSG32 uHS_FE                                      : 16;\
	UNSG32 uHS_BE                                      : 16;\
}
union { UNSG32 u32TG_MAIN_HS;
	struct w32TG_MAIN_HS;
};
#define     w32TG_MAIN_HB                                  {\
	UNSG32 uHB_FE                                      : 16;\
	UNSG32 uHB_BE                                      : 16;\
}
union { UNSG32 u32TG_MAIN_HB;
	struct w32TG_MAIN_HB;
};
#define     w32TG_MAIN_HB_Y                                {\
	UNSG32 uHB_Y_FE                                    : 16;\
	UNSG32 uHB_Y_BE                                    : 16;\
}
union { UNSG32 u32TG_MAIN_HB_Y;
	struct w32TG_MAIN_HB_Y;
};
#define     w32TG_MAIN_HB_C                                {\
	UNSG32 uHB_C_FE                                    : 16;\
	UNSG32 uHB_C_BE                                    : 16;\
}
union { UNSG32 u32TG_MAIN_HB_C;
	struct w32TG_MAIN_HB_C;
};
#define     w32TG_MAIN_HB_OUT                              {\
	UNSG32 uHB_OUT_FE                                  : 16;\
	UNSG32 uHB_OUT_BE                                  : 16;\
}
union { UNSG32 u32TG_MAIN_HB_OUT;
	struct w32TG_MAIN_HB_OUT;
};
#define     w32TG_MAIN_VS0                                 {\
	UNSG32 uVS0_FE                                     : 16;\
	UNSG32 uVS0_BE                                     : 16;\
}
union { UNSG32 u32TG_MAIN_VS0;
	struct w32TG_MAIN_VS0;
};
#define     w32TG_MAIN_VS1                                 {\
	UNSG32 uVS1_FE                                     : 16;\
	UNSG32 uVS1_BE                                     : 16;\
}
union { UNSG32 u32TG_MAIN_VS1;
	struct w32TG_MAIN_VS1;
};
#define     w32TG_MAIN_VB0                                 {\
	UNSG32 uVB0_FE                                     : 16;\
	UNSG32 uVB0_BE                                     : 16;\
}
union { UNSG32 u32TG_MAIN_VB0;
	struct w32TG_MAIN_VB0;
};
#define     w32TG_MAIN_VB0_Y                               {\
	UNSG32 uVB0_Y_FE                                   : 16;\
	UNSG32 uVB0_Y_BE                                   : 16;\
}
union { UNSG32 u32TG_MAIN_VB0_Y;
	struct w32TG_MAIN_VB0_Y;
};
#define     w32TG_MAIN_VB0_C                               {\
	UNSG32 uVB0_C_FE                                   : 16;\
	UNSG32 uVB0_C_BE                                   : 16;\
}
union { UNSG32 u32TG_MAIN_VB0_C;
	struct w32TG_MAIN_VB0_C;
};
#define     w32TG_MAIN_VB0_OUT                             {\
	UNSG32 uVB0_OUT_FE                                 : 16;\
	UNSG32 uVB0_OUT_BE                                 : 16;\
}
union { UNSG32 u32TG_MAIN_VB0_OUT;
	struct w32TG_MAIN_VB0_OUT;
};
#define     w32TG_MAIN_VB1                                 {\
	UNSG32 uVB1_FE                                     : 16;\
	UNSG32 uVB1_BE                                     : 16;\
}
union { UNSG32 u32TG_MAIN_VB1;
	struct w32TG_MAIN_VB1;
};
#define     w32TG_MAIN_SCAN                                {\
	UNSG32 uSCAN_MODE                                  :  1;\
	UNSG32 RSVDx38_b1                                  : 31;\
}
union { UNSG32 u32TG_MAIN_SCAN;
	struct w32TG_MAIN_SCAN;
};
#define     w32TG_MAIN_INTPOS                              {\
	UNSG32 uINTPOS_FRAME                               : 16;\
	UNSG32 uINTPOS_FIELD                               : 16;\
}
union { UNSG32 u32TG_MAIN_INTPOS;
	struct w32TG_MAIN_INTPOS;
};
#define     w32TG_MAIN_MODE                                {\
	UNSG32 uMODE_EN                                    :  1;\
	UNSG32 RSVDx40_b1                                  : 31;\
}
union { UNSG32 u32TG_MAIN_MODE;
	struct w32TG_MAIN_MODE;
};
#define     w32TG_MAIN_HVREF                               {\
	UNSG32 uHVREF_SEL                                  :  1;\
	UNSG32 uHVREF_POL                                  :  1;\
	UNSG32 RSVDx44_b2                                  : 30;\
}
union { UNSG32 u32TG_MAIN_HVREF;
	struct w32TG_MAIN_HVREF;
};
} SIE_TG_MAIN;
typedef union  T32TG_MAIN_INIT
{ UNSG32 u32;
	struct w32TG_MAIN_INIT;
} T32TG_MAIN_INIT;
typedef union  T32TG_MAIN_SIZE
{ UNSG32 u32;
	struct w32TG_MAIN_SIZE;
} T32TG_MAIN_SIZE;
typedef union  T32TG_MAIN_HS
{ UNSG32 u32;
	struct w32TG_MAIN_HS;
} T32TG_MAIN_HS;
typedef union  T32TG_MAIN_HB
{ UNSG32 u32;
	struct w32TG_MAIN_HB;
} T32TG_MAIN_HB;
typedef union  T32TG_MAIN_HB_Y
{ UNSG32 u32;
	struct w32TG_MAIN_HB_Y;
} T32TG_MAIN_HB_Y;
typedef union  T32TG_MAIN_HB_C
{ UNSG32 u32;
	struct w32TG_MAIN_HB_C;
} T32TG_MAIN_HB_C;
typedef union  T32TG_MAIN_HB_OUT
{ UNSG32 u32;
	struct w32TG_MAIN_HB_OUT;
} T32TG_MAIN_HB_OUT;
typedef union  T32TG_MAIN_VS0
{ UNSG32 u32;
	struct w32TG_MAIN_VS0;
} T32TG_MAIN_VS0;
typedef union  T32TG_MAIN_VS1
{ UNSG32 u32;
	struct w32TG_MAIN_VS1;
} T32TG_MAIN_VS1;
typedef union  T32TG_MAIN_VB0
{ UNSG32 u32;
	struct w32TG_MAIN_VB0;
} T32TG_MAIN_VB0;
typedef union  T32TG_MAIN_VB0_Y
{ UNSG32 u32;
	struct w32TG_MAIN_VB0_Y;
} T32TG_MAIN_VB0_Y;
typedef union  T32TG_MAIN_VB0_C
{ UNSG32 u32;
	struct w32TG_MAIN_VB0_C;
} T32TG_MAIN_VB0_C;
typedef union  T32TG_MAIN_VB0_OUT
{ UNSG32 u32;
	struct w32TG_MAIN_VB0_OUT;
} T32TG_MAIN_VB0_OUT;
typedef union  T32TG_MAIN_VB1
{ UNSG32 u32;
	struct w32TG_MAIN_VB1;
} T32TG_MAIN_VB1;
typedef union  T32TG_MAIN_SCAN
{ UNSG32 u32;
	struct w32TG_MAIN_SCAN;
} T32TG_MAIN_SCAN;
typedef union  T32TG_MAIN_INTPOS
{ UNSG32 u32;
	struct w32TG_MAIN_INTPOS;
} T32TG_MAIN_INTPOS;
typedef union  T32TG_MAIN_MODE
{ UNSG32 u32;
	struct w32TG_MAIN_MODE;
} T32TG_MAIN_MODE;
typedef union  T32TG_MAIN_HVREF
{ UNSG32 u32;
	struct w32TG_MAIN_HVREF;
} T32TG_MAIN_HVREF;
typedef union  TTG_MAIN_INIT
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_INIT;
	};
} TTG_MAIN_INIT;
typedef union  TTG_MAIN_SIZE
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_SIZE;
	};
} TTG_MAIN_SIZE;
typedef union  TTG_MAIN_HS
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_HS;
	};
} TTG_MAIN_HS;
typedef union  TTG_MAIN_HB
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_HB;
	};
} TTG_MAIN_HB;
typedef union  TTG_MAIN_HB_Y
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_HB_Y;
	};
} TTG_MAIN_HB_Y;
typedef union  TTG_MAIN_HB_C
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_HB_C;
	};
} TTG_MAIN_HB_C;
typedef union  TTG_MAIN_HB_OUT
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_HB_OUT;
	};
} TTG_MAIN_HB_OUT;
typedef union  TTG_MAIN_VS0
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_VS0;
	};
} TTG_MAIN_VS0;
typedef union  TTG_MAIN_VS1
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_VS1;
	};
} TTG_MAIN_VS1;
typedef union  TTG_MAIN_VB0
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_VB0;
	};
} TTG_MAIN_VB0;
typedef union  TTG_MAIN_VB0_Y
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_VB0_Y;
	};
} TTG_MAIN_VB0_Y;
typedef union  TTG_MAIN_VB0_C
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_VB0_C;
	};
} TTG_MAIN_VB0_C;
typedef union  TTG_MAIN_VB0_OUT
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_VB0_OUT;
	};
} TTG_MAIN_VB0_OUT;
typedef union  TTG_MAIN_VB1
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_VB1;
	};
} TTG_MAIN_VB1;
typedef union  TTG_MAIN_SCAN
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_SCAN;
	};
} TTG_MAIN_SCAN;
typedef union  TTG_MAIN_INTPOS
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_INTPOS;
	};
} TTG_MAIN_INTPOS;
typedef union  TTG_MAIN_MODE
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_MODE;
	};
} TTG_MAIN_MODE;
typedef union  TTG_MAIN_HVREF
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAIN_HVREF;
	};
} TTG_MAIN_HVREF;
SIGN32 TG_MAIN_drvrd(SIE_TG_MAIN *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 TG_MAIN_drvwr(SIE_TG_MAIN *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void TG_MAIN_reset(SIE_TG_MAIN *p);
SIGN32 TG_MAIN_cmp(SIE_TG_MAIN *p, SIE_TG_MAIN *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define TG_MAIN_check(p,pie,pfx,hLOG) TG_MAIN_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define TG_MAIN_print(p,    pfx,hLOG) TG_MAIN_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_BITMAP40
#define h_BITMAP40 (){}
#define     RA_BITMAP40_SEL                                0x0000
#define     RA_BITMAP40_SEL1                               0x0004
#define     RA_BITMAP40_SEL2                               0x0008
#define     RA_BITMAP40_SEL3                               0x000C
#define     RA_BITMAP40_SEL4                               0x0010
#define     RA_BITMAP40_SEL5                               0x0014
#define     RA_BITMAP40_SEL6                               0x0018
#define     RA_BITMAP40_SEL7                               0x001C
typedef struct SIE_BITMAP40 {
#define     w32BITMAP40_SEL                                {\
	UNSG32 uSEL_BIT_POS0                               :  6;\
	UNSG32 uSEL_BIT_POS1                               :  6;\
	UNSG32 uSEL_BIT_POS2                               :  6;\
	UNSG32 uSEL_BIT_POS3                               :  6;\
	UNSG32 uSEL_BIT_POS4                               :  6;\
	UNSG32 RSVDx0_b30                                  :  2;\
}
union { UNSG32 u32BITMAP40_SEL;
	struct w32BITMAP40_SEL;
};
#define     w32BITMAP40_SEL1                               {\
	UNSG32 uSEL_BIT_POS5                               :  6;\
	UNSG32 uSEL_BIT_POS6                               :  6;\
	UNSG32 uSEL_BIT_POS7                               :  6;\
	UNSG32 uSEL_BIT_POS8                               :  6;\
	UNSG32 uSEL_BIT_POS9                               :  6;\
	UNSG32 RSVDx4_b30                                  :  2;\
}
union { UNSG32 u32BITMAP40_SEL1;
	struct w32BITMAP40_SEL1;
};
#define     w32BITMAP40_SEL2                               {\
	UNSG32 uSEL_BIT_POS10                              :  6;\
	UNSG32 uSEL_BIT_POS11                              :  6;\
	UNSG32 uSEL_BIT_POS12                              :  6;\
	UNSG32 uSEL_BIT_POS13                              :  6;\
	UNSG32 uSEL_BIT_POS14                              :  6;\
	UNSG32 RSVDx8_b30                                  :  2;\
}
union { UNSG32 u32BITMAP40_SEL2;
	struct w32BITMAP40_SEL2;
};
#define     w32BITMAP40_SEL3                               {\
	UNSG32 uSEL_BIT_POS15                              :  6;\
	UNSG32 uSEL_BIT_POS16                              :  6;\
	UNSG32 uSEL_BIT_POS17                              :  6;\
	UNSG32 uSEL_BIT_POS18                              :  6;\
	UNSG32 uSEL_BIT_POS19                              :  6;\
	UNSG32 RSVDxC_b30                                  :  2;\
}
union { UNSG32 u32BITMAP40_SEL3;
	struct w32BITMAP40_SEL3;
};
#define     w32BITMAP40_SEL4                               {\
	UNSG32 uSEL_BIT_POS20                              :  6;\
	UNSG32 uSEL_BIT_POS21                              :  6;\
	UNSG32 uSEL_BIT_POS22                              :  6;\
	UNSG32 uSEL_BIT_POS23                              :  6;\
	UNSG32 uSEL_BIT_POS24                              :  6;\
	UNSG32 RSVDx10_b30                                 :  2;\
}
union { UNSG32 u32BITMAP40_SEL4;
	struct w32BITMAP40_SEL4;
};
#define     w32BITMAP40_SEL5                               {\
	UNSG32 uSEL_BIT_POS25                              :  6;\
	UNSG32 uSEL_BIT_POS26                              :  6;\
	UNSG32 uSEL_BIT_POS27                              :  6;\
	UNSG32 uSEL_BIT_POS28                              :  6;\
	UNSG32 uSEL_BIT_POS29                              :  6;\
	UNSG32 RSVDx14_b30                                 :  2;\
}
union { UNSG32 u32BITMAP40_SEL5;
	struct w32BITMAP40_SEL5;
};
#define     w32BITMAP40_SEL6                               {\
	UNSG32 uSEL_BIT_POS30                              :  6;\
	UNSG32 uSEL_BIT_POS31                              :  6;\
	UNSG32 uSEL_BIT_POS32                              :  6;\
	UNSG32 uSEL_BIT_POS33                              :  6;\
	UNSG32 uSEL_BIT_POS34                              :  6;\
	UNSG32 RSVDx18_b30                                 :  2;\
}
union { UNSG32 u32BITMAP40_SEL6;
	struct w32BITMAP40_SEL6;
};
#define     w32BITMAP40_SEL7                               {\
	UNSG32 uSEL_BIT_POS35                              :  6;\
	UNSG32 uSEL_BIT_POS36                              :  6;\
	UNSG32 uSEL_BIT_POS37                              :  6;\
	UNSG32 uSEL_BIT_POS38                              :  6;\
	UNSG32 uSEL_BIT_POS39                              :  6;\
	UNSG32 RSVDx1C_b30                                 :  2;\
}
union { UNSG32 u32BITMAP40_SEL7;
	struct w32BITMAP40_SEL7;
};
} SIE_BITMAP40;
typedef union  T32BITMAP40_SEL
{ UNSG32 u32;
	struct w32BITMAP40_SEL;
} T32BITMAP40_SEL;
typedef union  T32BITMAP40_SEL1
{ UNSG32 u32;
	struct w32BITMAP40_SEL1;
} T32BITMAP40_SEL1;
typedef union  T32BITMAP40_SEL2
{ UNSG32 u32;
	struct w32BITMAP40_SEL2;
} T32BITMAP40_SEL2;
typedef union  T32BITMAP40_SEL3
{ UNSG32 u32;
	struct w32BITMAP40_SEL3;
} T32BITMAP40_SEL3;
typedef union  T32BITMAP40_SEL4
{ UNSG32 u32;
	struct w32BITMAP40_SEL4;
} T32BITMAP40_SEL4;
typedef union  T32BITMAP40_SEL5
{ UNSG32 u32;
	struct w32BITMAP40_SEL5;
} T32BITMAP40_SEL5;
typedef union  T32BITMAP40_SEL6
{ UNSG32 u32;
	struct w32BITMAP40_SEL6;
} T32BITMAP40_SEL6;
typedef union  T32BITMAP40_SEL7
{ UNSG32 u32;
	struct w32BITMAP40_SEL7;
} T32BITMAP40_SEL7;
typedef union  TBITMAP40_SEL
{ UNSG32 u32[8];
	struct {
		struct w32BITMAP40_SEL;
		struct w32BITMAP40_SEL1;
		struct w32BITMAP40_SEL2;
		struct w32BITMAP40_SEL3;
		struct w32BITMAP40_SEL4;
		struct w32BITMAP40_SEL5;
		struct w32BITMAP40_SEL6;
		struct w32BITMAP40_SEL7;
	};
} TBITMAP40_SEL;
SIGN32 BITMAP40_drvrd(SIE_BITMAP40 *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 BITMAP40_drvwr(SIE_BITMAP40 *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void BITMAP40_reset(SIE_BITMAP40 *p);
SIGN32 BITMAP40_cmp(SIE_BITMAP40 *p, SIE_BITMAP40 *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define BITMAP40_check(p,pie,pfx,hLOG) BITMAP40_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define BITMAP40_print(p,    pfx,hLOG) BITMAP40_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_BITMAP20
#define h_BITMAP20 (){}
#define     RA_BITMAP20_SEL                                0x0000
#define     RA_BITMAP20_SEL1                               0x0004
#define     RA_BITMAP20_SEL2                               0x0008
#define     RA_BITMAP20_SEL3                               0x000C
typedef struct SIE_BITMAP20 {
#define     w32BITMAP20_SEL                                {\
	UNSG32 uSEL_BIT_POS0                               :  5;\
	UNSG32 uSEL_BIT_POS1                               :  5;\
	UNSG32 uSEL_BIT_POS2                               :  5;\
	UNSG32 uSEL_BIT_POS3                               :  5;\
	UNSG32 uSEL_BIT_POS4                               :  5;\
	UNSG32 uSEL_BIT_POS5                               :  5;\
	UNSG32 RSVDx0_b30                                  :  2;\
}
union { UNSG32 u32BITMAP20_SEL;
	struct w32BITMAP20_SEL;
};
#define     w32BITMAP20_SEL1                               {\
	UNSG32 uSEL_BIT_POS6                               :  5;\
	UNSG32 uSEL_BIT_POS7                               :  5;\
	UNSG32 uSEL_BIT_POS8                               :  5;\
	UNSG32 uSEL_BIT_POS9                               :  5;\
	UNSG32 uSEL_BIT_POS10                              :  5;\
	UNSG32 uSEL_BIT_POS11                              :  5;\
	UNSG32 RSVDx4_b30                                  :  2;\
}
union { UNSG32 u32BITMAP20_SEL1;
	struct w32BITMAP20_SEL1;
};
#define     w32BITMAP20_SEL2                               {\
	UNSG32 uSEL_BIT_POS12                              :  5;\
	UNSG32 uSEL_BIT_POS13                              :  5;\
	UNSG32 uSEL_BIT_POS14                              :  5;\
	UNSG32 uSEL_BIT_POS15                              :  5;\
	UNSG32 uSEL_BIT_POS16                              :  5;\
	UNSG32 uSEL_BIT_POS17                              :  5;\
	UNSG32 RSVDx8_b30                                  :  2;\
}
union { UNSG32 u32BITMAP20_SEL2;
	struct w32BITMAP20_SEL2;
};
#define     w32BITMAP20_SEL3                               {\
	UNSG32 uSEL_BIT_POS18                              :  5;\
	UNSG32 uSEL_BIT_POS19                              :  5;\
	UNSG32 RSVDxC_b10                                  : 22;\
}
union { UNSG32 u32BITMAP20_SEL3;
	struct w32BITMAP20_SEL3;
};
} SIE_BITMAP20;
typedef union  T32BITMAP20_SEL
{ UNSG32 u32;
	struct w32BITMAP20_SEL;
} T32BITMAP20_SEL;
typedef union  T32BITMAP20_SEL1
{ UNSG32 u32;
	struct w32BITMAP20_SEL1;
} T32BITMAP20_SEL1;
typedef union  T32BITMAP20_SEL2
{ UNSG32 u32;
	struct w32BITMAP20_SEL2;
} T32BITMAP20_SEL2;
typedef union  T32BITMAP20_SEL3
{ UNSG32 u32;
	struct w32BITMAP20_SEL3;
} T32BITMAP20_SEL3;
typedef union  TBITMAP20_SEL
{ UNSG32 u32[4];
	struct {
		struct w32BITMAP20_SEL;
		struct w32BITMAP20_SEL1;
		struct w32BITMAP20_SEL2;
		struct w32BITMAP20_SEL3;
	};
} TBITMAP20_SEL;
SIGN32 BITMAP20_drvrd(SIE_BITMAP20 *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 BITMAP20_drvwr(SIE_BITMAP20 *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void BITMAP20_reset(SIE_BITMAP20 *p);
SIGN32 BITMAP20_cmp(SIE_BITMAP20 *p, SIE_BITMAP20 *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define BITMAP20_check(p,pie,pfx,hLOG) BITMAP20_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define BITMAP20_print(p,    pfx,hLOG) BITMAP20_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_BITMAP32
#define h_BITMAP32 (){}
#define     RA_BITMAP32_SEL                                0x0000
#define     RA_BITMAP32_SEL1                               0x0004
#define     RA_BITMAP32_SEL2                               0x0008
#define     RA_BITMAP32_SEL3                               0x000C
#define     RA_BITMAP32_SEL4                               0x0010
#define     RA_BITMAP32_SEL5                               0x0014
typedef struct SIE_BITMAP32 {
#define     w32BITMAP32_SEL                                {\
	UNSG32 uSEL_BIT_POS0                               :  5;\
	UNSG32 uSEL_BIT_POS1                               :  5;\
	UNSG32 uSEL_BIT_POS2                               :  5;\
	UNSG32 uSEL_BIT_POS3                               :  5;\
	UNSG32 uSEL_BIT_POS4                               :  5;\
	UNSG32 uSEL_BIT_POS5                               :  5;\
	UNSG32 RSVDx0_b30                                  :  2;\
}
union { UNSG32 u32BITMAP32_SEL;
	struct w32BITMAP32_SEL;
};
#define     w32BITMAP32_SEL1                               {\
	UNSG32 uSEL_BIT_POS6                               :  5;\
	UNSG32 uSEL_BIT_POS7                               :  5;\
	UNSG32 uSEL_BIT_POS8                               :  5;\
	UNSG32 uSEL_BIT_POS9                               :  5;\
	UNSG32 uSEL_BIT_POS10                              :  5;\
	UNSG32 uSEL_BIT_POS11                              :  5;\
	UNSG32 RSVDx4_b30                                  :  2;\
}
union { UNSG32 u32BITMAP32_SEL1;
	struct w32BITMAP32_SEL1;
};
#define     w32BITMAP32_SEL2                               {\
	UNSG32 uSEL_BIT_POS12                              :  5;\
	UNSG32 uSEL_BIT_POS13                              :  5;\
	UNSG32 uSEL_BIT_POS14                              :  5;\
	UNSG32 uSEL_BIT_POS15                              :  5;\
	UNSG32 uSEL_BIT_POS16                              :  5;\
	UNSG32 uSEL_BIT_POS17                              :  5;\
	UNSG32 RSVDx8_b30                                  :  2;\
}
union { UNSG32 u32BITMAP32_SEL2;
	struct w32BITMAP32_SEL2;
};
#define     w32BITMAP32_SEL3                               {\
	UNSG32 uSEL_BIT_POS18                              :  5;\
	UNSG32 uSEL_BIT_POS19                              :  5;\
	UNSG32 uSEL_BIT_POS20                              :  5;\
	UNSG32 uSEL_BIT_POS21                              :  5;\
	UNSG32 uSEL_BIT_POS22                              :  5;\
	UNSG32 uSEL_BIT_POS23                              :  5;\
	UNSG32 RSVDxC_b30                                  :  2;\
}
union { UNSG32 u32BITMAP32_SEL3;
	struct w32BITMAP32_SEL3;
};
#define     w32BITMAP32_SEL4                               {\
	UNSG32 uSEL_BIT_POS24                              :  5;\
	UNSG32 uSEL_BIT_POS25                              :  5;\
	UNSG32 uSEL_BIT_POS26                              :  5;\
	UNSG32 uSEL_BIT_POS27                              :  5;\
	UNSG32 uSEL_BIT_POS28                              :  5;\
	UNSG32 uSEL_BIT_POS29                              :  5;\
	UNSG32 RSVDx10_b30                                 :  2;\
}
union { UNSG32 u32BITMAP32_SEL4;
	struct w32BITMAP32_SEL4;
};
#define     w32BITMAP32_SEL5                               {\
	UNSG32 uSEL_BIT_POS30                              :  5;\
	UNSG32 uSEL_BIT_POS31                              :  5;\
	UNSG32 RSVDx14_b10                                 : 22;\
}
union { UNSG32 u32BITMAP32_SEL5;
	struct w32BITMAP32_SEL5;
};
} SIE_BITMAP32;
typedef union  T32BITMAP32_SEL
{ UNSG32 u32;
	struct w32BITMAP32_SEL;
} T32BITMAP32_SEL;
typedef union  T32BITMAP32_SEL1
{ UNSG32 u32;
	struct w32BITMAP32_SEL1;
} T32BITMAP32_SEL1;
typedef union  T32BITMAP32_SEL2
{ UNSG32 u32;
	struct w32BITMAP32_SEL2;
} T32BITMAP32_SEL2;
typedef union  T32BITMAP32_SEL3
{ UNSG32 u32;
	struct w32BITMAP32_SEL3;
} T32BITMAP32_SEL3;
typedef union  T32BITMAP32_SEL4
{ UNSG32 u32;
	struct w32BITMAP32_SEL4;
} T32BITMAP32_SEL4;
typedef union  T32BITMAP32_SEL5
{ UNSG32 u32;
	struct w32BITMAP32_SEL5;
} T32BITMAP32_SEL5;
typedef union  TBITMAP32_SEL
{ UNSG32 u32[6];
	struct {
		struct w32BITMAP32_SEL;
		struct w32BITMAP32_SEL1;
		struct w32BITMAP32_SEL2;
		struct w32BITMAP32_SEL3;
		struct w32BITMAP32_SEL4;
		struct w32BITMAP32_SEL5;
	};
} TBITMAP32_SEL;
SIGN32 BITMAP32_drvrd(SIE_BITMAP32 *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 BITMAP32_drvwr(SIE_BITMAP32 *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void BITMAP32_reset(SIE_BITMAP32 *p);
SIGN32 BITMAP32_cmp(SIE_BITMAP32 *p, SIE_BITMAP32 *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define BITMAP32_check(p,pie,pfx,hLOG) BITMAP32_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define BITMAP32_print(p,    pfx,hLOG) BITMAP32_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_BITMAP16
#define h_BITMAP16 (){}
#define     RA_BITMAP16_SEL                                0x0000
#define     RA_BITMAP16_SEL1                               0x0004
typedef struct SIE_BITMAP16 {
#define     w32BITMAP16_SEL                                {\
	UNSG32 uSEL_BIT_POS0                               :  4;\
	UNSG32 uSEL_BIT_POS1                               :  4;\
	UNSG32 uSEL_BIT_POS2                               :  4;\
	UNSG32 uSEL_BIT_POS3                               :  4;\
	UNSG32 uSEL_BIT_POS4                               :  4;\
	UNSG32 uSEL_BIT_POS5                               :  4;\
	UNSG32 uSEL_BIT_POS6                               :  4;\
	UNSG32 uSEL_BIT_POS7                               :  4;\
}
union { UNSG32 u32BITMAP16_SEL;
	struct w32BITMAP16_SEL;
};
#define     w32BITMAP16_SEL1                               {\
	UNSG32 uSEL_BIT_POS8                               :  4;\
	UNSG32 uSEL_BIT_POS9                               :  4;\
	UNSG32 uSEL_BIT_POS10                              :  4;\
	UNSG32 uSEL_BIT_POS11                              :  4;\
	UNSG32 uSEL_BIT_POS12                              :  4;\
	UNSG32 uSEL_BIT_POS13                              :  4;\
	UNSG32 uSEL_BIT_POS14                              :  4;\
	UNSG32 uSEL_BIT_POS15                              :  4;\
}
union { UNSG32 u32BITMAP16_SEL1;
	struct w32BITMAP16_SEL1;
};
} SIE_BITMAP16;
typedef union  T32BITMAP16_SEL
{ UNSG32 u32;
	struct w32BITMAP16_SEL;
} T32BITMAP16_SEL;
typedef union  T32BITMAP16_SEL1
{ UNSG32 u32;
	struct w32BITMAP16_SEL1;
} T32BITMAP16_SEL1;
typedef union  TBITMAP16_SEL
{ UNSG32 u32[2];
	struct {
		struct w32BITMAP16_SEL;
		struct w32BITMAP16_SEL1;
	};
} TBITMAP16_SEL;
SIGN32 BITMAP16_drvrd(SIE_BITMAP16 *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 BITMAP16_drvwr(SIE_BITMAP16 *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void BITMAP16_reset(SIE_BITMAP16 *p);
SIGN32 BITMAP16_cmp(SIE_BITMAP16 *p, SIE_BITMAP16 *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define BITMAP16_check(p,pie,pfx,hLOG) BITMAP16_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define BITMAP16_print(p,    pfx,hLOG) BITMAP16_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_BITMAP12
#define h_BITMAP12 (){}
#define     RA_BITMAP12_SEL                                0x0000
#define     RA_BITMAP12_SEL1                               0x0004
typedef struct SIE_BITMAP12 {
#define     w32BITMAP12_SEL                                {\
	UNSG32 uSEL_BIT_POS0                               :  4;\
	UNSG32 uSEL_BIT_POS1                               :  4;\
	UNSG32 uSEL_BIT_POS2                               :  4;\
	UNSG32 uSEL_BIT_POS3                               :  4;\
	UNSG32 uSEL_BIT_POS4                               :  4;\
	UNSG32 uSEL_BIT_POS5                               :  4;\
	UNSG32 uSEL_BIT_POS6                               :  4;\
	UNSG32 uSEL_BIT_POS7                               :  4;\
}
union { UNSG32 u32BITMAP12_SEL;
	struct w32BITMAP12_SEL;
};
#define     w32BITMAP12_SEL1                               {\
	UNSG32 uSEL_BIT_POS8                               :  4;\
	UNSG32 uSEL_BIT_POS9                               :  4;\
	UNSG32 uSEL_BIT_POS10                              :  4;\
	UNSG32 uSEL_BIT_POS11                              :  4;\
	UNSG32 RSVDx4_b16                                  : 16;\
}
union { UNSG32 u32BITMAP12_SEL1;
	struct w32BITMAP12_SEL1;
};
} SIE_BITMAP12;
typedef union  T32BITMAP12_SEL
{ UNSG32 u32;
	struct w32BITMAP12_SEL;
} T32BITMAP12_SEL;
typedef union  T32BITMAP12_SEL1
{ UNSG32 u32;
	struct w32BITMAP12_SEL1;
} T32BITMAP12_SEL1;
typedef union  TBITMAP12_SEL
{ UNSG32 u32[2];
	struct {
		struct w32BITMAP12_SEL;
		struct w32BITMAP12_SEL1;
	};
} TBITMAP12_SEL;
SIGN32 BITMAP12_drvrd(SIE_BITMAP12 *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 BITMAP12_drvwr(SIE_BITMAP12 *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void BITMAP12_reset(SIE_BITMAP12 *p);
SIGN32 BITMAP12_cmp(SIE_BITMAP12 *p, SIE_BITMAP12 *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define BITMAP12_check(p,pie,pfx,hLOG) BITMAP12_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define BITMAP12_print(p,    pfx,hLOG) BITMAP12_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_ReadClient
#define h_ReadClient (){}
#define     RA_ReadClient_Rd                               0x0000
#define     RA_ReadClient_Word                             0x0004
#define     RA_ReadClient_NonStdRes                        0x0008
#define     RA_ReadClient_pack                             0x000C
typedef struct SIE_ReadClient {
#define     w32ReadClient_Rd                               {\
	UNSG32 uRd_start                                   :  1;\
	UNSG32 uRd_clear                                   :  1;\
	UNSG32 RSVDx0_b2                                   : 30;\
}
union { UNSG32 u32ReadClient_Rd;
	struct w32ReadClient_Rd;
};
#define     w32ReadClient_Word                             {\
	UNSG32 uWord_tot                                   : 32;\
}
union { UNSG32 u32ReadClient_Word;
	struct w32ReadClient_Word;
};
#define     w32ReadClient_NonStdRes                        {\
	UNSG32 uNonStdRes_enable                           :  1;\
	UNSG32 uNonStdRes_pixlineTot                       : 13;\
	UNSG32 uNonStdRes_flushCnt                         :  4;\
	UNSG32 RSVDx8_b18                                  : 14;\
}
union { UNSG32 u32ReadClient_NonStdRes;
	struct w32ReadClient_NonStdRes;
};
#define     w32ReadClient_pack                             {\
	UNSG32 upack_Sel                                   :  4;\
	UNSG32 RSVDxC_b4                                   : 28;\
}
union { UNSG32 u32ReadClient_pack;
	struct w32ReadClient_pack;
};
} SIE_ReadClient;
typedef union  T32ReadClient_Rd
{ UNSG32 u32;
	struct w32ReadClient_Rd;
} T32ReadClient_Rd;
typedef union  T32ReadClient_Word
{ UNSG32 u32;
	struct w32ReadClient_Word;
} T32ReadClient_Word;
typedef union  T32ReadClient_NonStdRes
{ UNSG32 u32;
	struct w32ReadClient_NonStdRes;
} T32ReadClient_NonStdRes;
typedef union  T32ReadClient_pack
{ UNSG32 u32;
	struct w32ReadClient_pack;
} T32ReadClient_pack;
typedef union  TReadClient_Rd
{ UNSG32 u32[1];
	struct {
		struct w32ReadClient_Rd;
	};
} TReadClient_Rd;
typedef union  TReadClient_Word
{ UNSG32 u32[1];
	struct {
		struct w32ReadClient_Word;
	};
} TReadClient_Word;
typedef union  TReadClient_NonStdRes
{ UNSG32 u32[1];
	struct {
		struct w32ReadClient_NonStdRes;
	};
} TReadClient_NonStdRes;
typedef union  TReadClient_pack
{ UNSG32 u32[1];
	struct {
		struct w32ReadClient_pack;
	};
} TReadClient_pack;
SIGN32 ReadClient_drvrd(SIE_ReadClient *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 ReadClient_drvwr(SIE_ReadClient *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void ReadClient_reset(SIE_ReadClient *p);
SIGN32 ReadClient_cmp(SIE_ReadClient *p, SIE_ReadClient *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define ReadClient_check(p,pie,pfx,hLOG) ReadClient_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define ReadClient_print(p,    pfx,hLOG) ReadClient_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_WriteClient
#define h_WriteClient (){}
#define     RA_WriteClient_Wr                              0x0000
#define     RA_WriteClient_pix                             0x0004
#define     RA_WriteClient_NonStdRes                       0x0008
#define     RA_WriteClient_pack                            0x000C
typedef struct SIE_WriteClient {
#define     w32WriteClient_Wr                              {\
	UNSG32 uWr_start                                   :  1;\
	UNSG32 uWr_clear                                   :  1;\
	UNSG32 RSVDx0_b2                                   : 30;\
}
union { UNSG32 u32WriteClient_Wr;
	struct w32WriteClient_Wr;
};
#define     w32WriteClient_pix                             {\
	UNSG32 upix_tot                                    : 32;\
}
union { UNSG32 u32WriteClient_pix;
	struct w32WriteClient_pix;
};
#define     w32WriteClient_NonStdRes                       {\
	UNSG32 uNonStdRes_enable                           :  1;\
	UNSG32 uNonStdRes_pixlineTot                       : 13;\
	UNSG32 RSVDx8_b14                                  : 18;\
}
union { UNSG32 u32WriteClient_NonStdRes;
	struct w32WriteClient_NonStdRes;
};
#define     w32WriteClient_pack                            {\
	UNSG32 upack_Sel                                   :  4;\
	UNSG32 RSVDxC_b4                                   : 28;\
}
union { UNSG32 u32WriteClient_pack;
	struct w32WriteClient_pack;
};
} SIE_WriteClient;
typedef union  T32WriteClient_Wr
{ UNSG32 u32;
	struct w32WriteClient_Wr;
} T32WriteClient_Wr;
typedef union  T32WriteClient_pix
{ UNSG32 u32;
	struct w32WriteClient_pix;
} T32WriteClient_pix;
typedef union  T32WriteClient_NonStdRes
{ UNSG32 u32;
	struct w32WriteClient_NonStdRes;
} T32WriteClient_NonStdRes;
typedef union  T32WriteClient_pack
{ UNSG32 u32;
	struct w32WriteClient_pack;
} T32WriteClient_pack;
typedef union  TWriteClient_Wr
{ UNSG32 u32[1];
	struct {
		struct w32WriteClient_Wr;
	};
} TWriteClient_Wr;
typedef union  TWriteClient_pix
{ UNSG32 u32[1];
	struct {
		struct w32WriteClient_pix;
	};
} TWriteClient_pix;
typedef union  TWriteClient_NonStdRes
{ UNSG32 u32[1];
	struct {
		struct w32WriteClient_NonStdRes;
	};
} TWriteClient_NonStdRes;
typedef union  TWriteClient_pack
{ UNSG32 u32[1];
	struct {
		struct w32WriteClient_pack;
	};
} TWriteClient_pack;
SIGN32 WriteClient_drvrd(SIE_WriteClient *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 WriteClient_drvwr(SIE_WriteClient *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void WriteClient_reset(SIE_WriteClient *p);
SIGN32 WriteClient_cmp (SIE_WriteClient *p, SIE_WriteClient *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define WriteClient_check(p,pie,pfx,hLOG) WriteClient_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define WriteClient_print(p,    pfx,hLOG) WriteClient_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_ClientIF
#define h_ClientIF (){}
#define     RA_ClientIF_MR0                                0x0000
#define     RA_ClientIF_MR0_word                           0x0004
#define     RA_ClientIF_MR1                                0x0008
#define     RA_ClientIF_MR1_word                           0x000C
#define     RA_ClientIF_CTRL0                              0x0010
#define     RA_ClientIF_DUMMY                              0x0014
#define     RA_ClientIF_CTRL2                              0x0018
#define     RA_ClientIF_CTRL3                              0x001C
#define     RA_ClientIF_RdClientVmxVm                      0x0020
typedef struct SIE_ClientIF {
#define     w32ClientIF_MR0                                {\
	UNSG32 uMR0_start                                  :  1;\
	UNSG32 uMR0_clear                                  :  1;\
	UNSG32 RSVDx0_b2                                   : 30;\
}
union { UNSG32 u32ClientIF_MR0;
	struct w32ClientIF_MR0;
};
#define     w32ClientIF_MR0_word                           {\
	UNSG32 uMR0_word_tot                               : 32;\
}
union { UNSG32 u32ClientIF_MR0_word;
	struct w32ClientIF_MR0_word;
};
#define     w32ClientIF_MR1                                {\
	UNSG32 uMR1_start                                  :  1;\
	UNSG32 uMR1_clear                                  :  1;\
	UNSG32 RSVDx8_b2                                   : 30;\
}
union { UNSG32 u32ClientIF_MR1;
	struct w32ClientIF_MR1;
};
#define     w32ClientIF_MR1_word                           {\
	UNSG32 uMR1_word_tot                               : 32;\
}
union { UNSG32 u32ClientIF_MR1_word;
	struct w32ClientIF_MR1_word;
};
#define     w32ClientIF_CTRL0                              {\
	UNSG32 uCTRL0_CLKEN_Main0                          :  1;\
	UNSG32 uCTRL0_CLKEN_Main1                          :  1;\
	UNSG32 uCTRL0_rdmain_initval0                      :  1;\
	UNSG32 uCTRL0_rdmain_initval1                      :  1;\
	UNSG32 uCTRL0_rdm_mask_sftrst                      :  1;\
	UNSG32 uCTRL0_packSel_MR0                          :  4;\
	UNSG32 uCTRL0_packSel_MR1                          :  2;\
	UNSG32 uCTRL0_ups420_idataSelM                     :  1;\
	UNSG32 uCTRL0_read_sel_420SP                       :  1;\
	UNSG32 uCTRL0_ups420_idat_ctrl                     :  3;\
	UNSG32 RSVDx10_b16                                 : 16;\
}
union { UNSG32 u32ClientIF_CTRL0;
	struct w32ClientIF_CTRL0;
};
#define     w32ClientIF_DUMMY                              {\
	UNSG32 uDUMMY_dummy                                : 32;\
}
union { UNSG32 u32ClientIF_DUMMY;
	struct w32ClientIF_DUMMY;
};
#define     w32ClientIF_CTRL2                              {\
	UNSG32 uCTRL2_nonStdResEn_MR0                      :  1;\
	UNSG32 uCTRL2_pixlineTot_MR0                       : 13;\
	UNSG32 uCTRL2_flushCnt_MR0                         :  4;\
	UNSG32 RSVDx18_b18                                 : 14;\
}
union { UNSG32 u32ClientIF_CTRL2;
	struct w32ClientIF_CTRL2;
};
#define     w32ClientIF_CTRL3                              {\
	UNSG32 uCTRL3_nonStdResEn_MR1                      :  1;\
	UNSG32 uCTRL3_pixlineTot_MR1                       : 13;\
	UNSG32 uCTRL3_flushCnt_MR1                         :  4;\
	UNSG32 RSVDx1C_b18                                 : 14;\
}
union { UNSG32 u32ClientIF_CTRL3;
	struct w32ClientIF_CTRL3;
};
SIE_ReadClient                                   ie_RdClientVmxVm;
} SIE_ClientIF;
typedef union  T32ClientIF_MR0
{ UNSG32 u32;
	struct w32ClientIF_MR0;
} T32ClientIF_MR0;
typedef union  T32ClientIF_MR0_word
{ UNSG32 u32;
	struct w32ClientIF_MR0_word;
} T32ClientIF_MR0_word;
typedef union  T32ClientIF_MR1
{ UNSG32 u32;
	struct w32ClientIF_MR1;
} T32ClientIF_MR1;
typedef union  T32ClientIF_MR1_word
{ UNSG32 u32;
	struct w32ClientIF_MR1_word;
} T32ClientIF_MR1_word;
typedef union  T32ClientIF_CTRL0
{ UNSG32 u32;
	struct w32ClientIF_CTRL0;
} T32ClientIF_CTRL0;
typedef union  T32ClientIF_DUMMY
{ UNSG32 u32;
	struct w32ClientIF_DUMMY;
} T32ClientIF_DUMMY;
typedef union  T32ClientIF_CTRL2
{ UNSG32 u32;
	struct w32ClientIF_CTRL2;
} T32ClientIF_CTRL2;
typedef union  T32ClientIF_CTRL3
{ UNSG32 u32;
	struct w32ClientIF_CTRL3;
} T32ClientIF_CTRL3;
typedef union  TClientIF_MR0
{ UNSG32 u32[1];
	struct {
		struct w32ClientIF_MR0;
	};
} TClientIF_MR0;
typedef union  TClientIF_MR0_word
{ UNSG32 u32[1];
	struct {
		struct w32ClientIF_MR0_word;
	};
} TClientIF_MR0_word;
typedef union  TClientIF_MR1
{ UNSG32 u32[1];
	struct {
		struct w32ClientIF_MR1;
	};
} TClientIF_MR1;
typedef union  TClientIF_MR1_word
{ UNSG32 u32[1];
	struct {
		struct w32ClientIF_MR1_word;
	};
} TClientIF_MR1_word;
typedef union  TClientIF_CTRL0
{ UNSG32 u32[1];
	struct {
		struct w32ClientIF_CTRL0;
	};
} TClientIF_CTRL0;
typedef union  TClientIF_DUMMY
{ UNSG32 u32[1];
	struct {
		struct w32ClientIF_DUMMY;
	};
} TClientIF_DUMMY;
typedef union  TClientIF_CTRL2
{ UNSG32 u32[1];
	struct {
		struct w32ClientIF_CTRL2;
	};
} TClientIF_CTRL2;
typedef union  TClientIF_CTRL3
{ UNSG32 u32[1];
	struct {
		struct w32ClientIF_CTRL3;
	};
} TClientIF_CTRL3;
SIGN32 ClientIF_drvrd(SIE_ClientIF *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 ClientIF_drvwr(SIE_ClientIF *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void ClientIF_reset(SIE_ClientIF *p);
SIGN32 ClientIF_cmp  (SIE_ClientIF *p, SIE_ClientIF *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define ClientIF_check(p,pie,pfx,hLOG) ClientIF_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define ClientIF_print(p,    pfx,hLOG) ClientIF_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_BETG
#define h_BETG (){}
#define     RA_BETG_PL0                                    0x0000
#define     RA_BETG_PL1                                    0x0008
#define     RA_BETG_PL2                                    0x0010
#define     RA_BETG_PL3                                    0x0018
#define     RA_BETG_PL4                                    0x0020
#define     RA_BETG_PL5                                    0x0028
#define     RA_BETG_PL6                                    0x0030
#define     RA_BETG_PL7                                    0x0038
#define     RA_BETG_PL8                                    0x0040
#define     RA_BETG_PL1_CR                                 0x0048
#define     RA_BETG_PL2_CR                                 0x0050
#define     RA_BETG_PL3_CR                                 0x0058
#define     RA_BETG_PL4_CR                                 0x0060
#define     RA_BETG_PL5_CR                                 0x0068
#define     RA_BETG_PL6_CR                                 0x0070
#define     RA_BETG_PL7_CR                                 0x0078
#define     RA_BETG_PL8_CR                                 0x0080
#define     RA_BETG_PL_FLD                                 0x0088
#define     RA_BETG_TG_PRG                                 0x0090
typedef struct SIE_BETG {
	SIE_TG_PL                                        ie_PL0;
	SIE_TG_PL                                        ie_PL1;
	SIE_TG_PL                                        ie_PL2;
	SIE_TG_PL                                        ie_PL3;
	SIE_TG_PL                                        ie_PL4;
	SIE_TG_PL                                        ie_PL5;
	SIE_TG_PL                                        ie_PL6;
	SIE_TG_PL                                        ie_PL7;
	SIE_TG_PL                                        ie_PL8;
	SIE_TG_PL                                        ie_PL1_CR;
	SIE_TG_PL                                        ie_PL2_CR;
	SIE_TG_PL                                        ie_PL3_CR;
	SIE_TG_PL                                        ie_PL4_CR;
	SIE_TG_PL                                        ie_PL5_CR;
	SIE_TG_PL                                        ie_PL6_CR;
	SIE_TG_PL                                        ie_PL7_CR;
	SIE_TG_PL                                        ie_PL8_CR;
	SIE_TG_PL                                        ie_PL_FLD;
	SIE_TG_PRG                                       ie_TG_PRG;
} SIE_BETG;
SIGN32 BETG_drvrd(SIE_BETG *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 BETG_drvwr(SIE_BETG *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void BETG_reset(SIE_BETG *p);
SIGN32 BETG_cmp  (SIE_BETG *p, SIE_BETG *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define BETG_check(p,pie,pfx,hLOG) BETG_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define BETG_print(p,    pfx,hLOG) BETG_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_UPS_420_422
#define h_UPS_420_422 (){}
#define     RA_UPS_420_422_CFG0                            0x0000
#define     RA_UPS_420_422_CFG1                            0x0004
#define     RA_UPS_420_422_CFG2                            0x0008
#define     RA_UPS_420_422_CFG3                            0x000C
#define     RA_UPS_420_422_CFG4                            0x0010
#define     RA_UPS_420_422_CFG5                            0x0014
#define     RA_UPS_420_422_CFG6                            0x0018
#define     RA_UPS_420_422_CFG7                            0x001C
#define     RA_UPS_420_422_CFG8                            0x0020
#define     RA_UPS_420_422_CFG9                            0x0024
#define     RA_UPS_420_422_CFG10                           0x0028
typedef struct SIE_UPS_420_422 {
#define     w32UPS_420_422_CFG0                            {\
	UNSG32 uCFG0_enable                                :  1;\
	UNSG32 uCFG0_msbswap                               :  1;\
	UNSG32 uCFG0_cswap_regs                            :  1;\
	UNSG32 uCFG0_yshift_regs                           :  1;\
	UNSG32 uCFG0_cshift_regs                           :  1;\
	UNSG32 uCFG0_sp_en                                 :  1;\
	UNSG32 uCFG0_dat8_en                               :  1;\
	UNSG32 uCFG0_sft_rst                               :  1;\
	UNSG32 uCFG0_use_blank                             :  1;\
	UNSG32 uCFG0_opt_malloc                            :  1;\
	UNSG32 uCFG0_rsv0                                  :  9;\
	UNSG32 uCFG0_rsv1                                  :  9;\
	UNSG32 uCFG0_vfilter                               :  1;\
	UNSG32 uCFG0_out422                                :  1;\
	UNSG32 uCFG0_crop_blank                            :  1;\
	UNSG32 uCFG0_luma_rpt                              :  1;\
}
union { UNSG32 u32UPS_420_422_CFG0;
	struct w32UPS_420_422_CFG0;
};
#define     w32UPS_420_422_CFG1                            {\
	UNSG32 uCFG1_ups_c0                                : 13;\
	UNSG32 uCFG1_ups_c1                                : 13;\
	UNSG32 RSVDx4_b26                                  :  6;\
}
union { UNSG32 u32UPS_420_422_CFG1;
	struct w32UPS_420_422_CFG1;
};
#define     w32UPS_420_422_CFG2                            {\
	UNSG32 uCFG2_ups_c2                                : 13;\
	UNSG32 uCFG2_ups_c3                                : 13;\
	UNSG32 RSVDx8_b26                                  :  6;\
}
union { UNSG32 u32UPS_420_422_CFG2;
	struct w32UPS_420_422_CFG2;
};
#define     w32UPS_420_422_CFG3                            {\
	UNSG32 uCFG3_ups_c4                                : 13;\
	UNSG32 uCFG3_ups_c5                                : 13;\
	UNSG32 RSVDxC_b26                                  :  6;\
}
union { UNSG32 u32UPS_420_422_CFG3;
	struct w32UPS_420_422_CFG3;
};
#define     w32UPS_420_422_CFG4                            {\
	UNSG32 uCFG4_ups_c6                                : 13;\
	UNSG32 RSVDx10_b13                                 : 19;\
}
union { UNSG32 u32UPS_420_422_CFG4;
	struct w32UPS_420_422_CFG4;
};
#define     w32UPS_420_422_CFG5                            {\
	UNSG32 uCFG5_hres_regs                             : 13;\
	UNSG32 uCFG5_htot_regs                             : 13;\
	UNSG32 uCFG5_auto_pixcnt                           :  1;\
	UNSG32 uCFG5_power_redn                            :  1;\
	UNSG32 RSVDx14_b28                                 :  4;\
}
union { UNSG32 u32UPS_420_422_CFG5;
	struct w32UPS_420_422_CFG5;
};
#define     w32UPS_420_422_CFG6                            {\
	UNSG32 uCFG6_Rsv0                                  :  8;\
	UNSG32 uCFG6_top_crop                              :  6;\
	UNSG32 uCFG6_bot_crop                              :  6;\
	UNSG32 uCFG6_left_crop                             :  6;\
	UNSG32 uCFG6_right_crop                            :  6;\
}
union { UNSG32 u32UPS_420_422_CFG6;
	struct w32UPS_420_422_CFG6;
};
#define     w32UPS_420_422_CFG7                            {\
	UNSG32 uCFG7_vres_regs                             : 13;\
	UNSG32 RSVDx1C_b13                                 : 19;\
}
union { UNSG32 u32UPS_420_422_CFG7;
	struct w32UPS_420_422_CFG7;
};
#define     w32UPS_420_422_CFG8                            {\
	UNSG32 uCFG8_pdwn_regs                             :  1;\
	UNSG32 uCFG8_pdlvmc_regs                           :  1;\
	UNSG32 uCFG8_pdfvssm_regs                          :  1;\
	UNSG32 uCFG8_dpwr_regs                             :  2;\
	UNSG32 RSVDx20_b5                                  : 27;\
}
union { UNSG32 u32UPS_420_422_CFG8;
	struct w32UPS_420_422_CFG8;
};
#define     w32UPS_420_422_CFG9                            {\
	UNSG32 uCFG9_yblank_regs                           : 12;\
	UNSG32 uCFG9_cblank_regs                           : 12;\
	UNSG32 RSVDx24_b24                                 :  8;\
}
union { UNSG32 u32UPS_420_422_CFG9;
	struct w32UPS_420_422_CFG9;
};
#define     w32UPS_420_422_CFG10                           {\
	UNSG32 uCFG10_tile_en                              :  1;\
	UNSG32 uCFG10_rsv0                                 :  3;\
	UNSG32 uCFG10_tile_depth                           :  8;\
	UNSG32 uCFG10_tile_width                           :  8;\
	UNSG32 RSVDx28_b20                                 : 12;\
}
union { UNSG32 u32UPS_420_422_CFG10;
	struct w32UPS_420_422_CFG10;
};
} SIE_UPS_420_422;
typedef union  T32UPS_420_422_CFG0
{ UNSG32 u32;
	struct w32UPS_420_422_CFG0;
} T32UPS_420_422_CFG0;
typedef union  T32UPS_420_422_CFG1
{ UNSG32 u32;
	struct w32UPS_420_422_CFG1;
} T32UPS_420_422_CFG1;
typedef union  T32UPS_420_422_CFG2
{ UNSG32 u32;
	struct w32UPS_420_422_CFG2;
} T32UPS_420_422_CFG2;
typedef union  T32UPS_420_422_CFG3
{ UNSG32 u32;
	struct w32UPS_420_422_CFG3;
} T32UPS_420_422_CFG3;
typedef union  T32UPS_420_422_CFG4
{ UNSG32 u32;
	struct w32UPS_420_422_CFG4;
} T32UPS_420_422_CFG4;
typedef union  T32UPS_420_422_CFG5
{ UNSG32 u32;
	struct w32UPS_420_422_CFG5;
} T32UPS_420_422_CFG5;
typedef union  T32UPS_420_422_CFG6
{ UNSG32 u32;
	struct w32UPS_420_422_CFG6;
} T32UPS_420_422_CFG6;
typedef union  T32UPS_420_422_CFG7
{ UNSG32 u32;
	struct w32UPS_420_422_CFG7;
} T32UPS_420_422_CFG7;
typedef union  T32UPS_420_422_CFG8
{ UNSG32 u32;
	struct w32UPS_420_422_CFG8;
} T32UPS_420_422_CFG8;
typedef union  T32UPS_420_422_CFG9
{ UNSG32 u32;
	struct w32UPS_420_422_CFG9;
} T32UPS_420_422_CFG9;
typedef union  T32UPS_420_422_CFG10
{ UNSG32 u32;
	struct w32UPS_420_422_CFG10;
} T32UPS_420_422_CFG10;
typedef union  TUPS_420_422_CFG0
{ UNSG32 u32[1];
	struct {
		struct w32UPS_420_422_CFG0;
	};
} TUPS_420_422_CFG0;
typedef union  TUPS_420_422_CFG1
{ UNSG32 u32[1];
	struct {
		struct w32UPS_420_422_CFG1;
	};
} TUPS_420_422_CFG1;
typedef union  TUPS_420_422_CFG2
{ UNSG32 u32[1];
	struct {
		struct w32UPS_420_422_CFG2;
	};
} TUPS_420_422_CFG2;
typedef union  TUPS_420_422_CFG3
{ UNSG32 u32[1];
	struct {
		struct w32UPS_420_422_CFG3;
	};
} TUPS_420_422_CFG3;
typedef union  TUPS_420_422_CFG4
{ UNSG32 u32[1];
	struct {
		struct w32UPS_420_422_CFG4;
	};
} TUPS_420_422_CFG4;
typedef union  TUPS_420_422_CFG5
{ UNSG32 u32[1];
	struct {
		struct w32UPS_420_422_CFG5;
	};
} TUPS_420_422_CFG5;
typedef union  TUPS_420_422_CFG6
{ UNSG32 u32[1];
	struct {
		struct w32UPS_420_422_CFG6;
	};
} TUPS_420_422_CFG6;
typedef union  TUPS_420_422_CFG7
{ UNSG32 u32[1];
	struct {
		struct w32UPS_420_422_CFG7;
	};
} TUPS_420_422_CFG7;
typedef union  TUPS_420_422_CFG8
{ UNSG32 u32[1];
	struct {
		struct w32UPS_420_422_CFG8;
	};
} TUPS_420_422_CFG8;
typedef union  TUPS_420_422_CFG9
{ UNSG32 u32[1];
	struct {
		struct w32UPS_420_422_CFG9;
	};
} TUPS_420_422_CFG9;
typedef union  TUPS_420_422_CFG10
{ UNSG32 u32[1];
	struct {
		struct w32UPS_420_422_CFG10;
	};
} TUPS_420_422_CFG10;
SIGN32 UPS_420_422_drvrd(SIE_UPS_420_422 *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 UPS_420_422_drvwr(SIE_UPS_420_422 *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void UPS_420_422_reset(SIE_UPS_420_422 *p);
SIGN32 UPS_420_422_cmp  (SIE_UPS_420_422 *p, SIE_UPS_420_422 *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define UPS_420_422_check(p,pie,pfx,hLOG) UPS_420_422_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define UPS_420_422_print(p,    pfx,hLOG) UPS_420_422_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_DIFLUT0
#define h_DIFLUT0 (){}
#define     RA_DIFLUT0_entry                               0x0000
typedef struct SIE_DIFLUT0 {
#define     w32DIFLUT0_entry                               {\
	UNSG32 uentry_DATA                                 : 32;\
}
union { UNSG32 u32DIFLUT0_entry;
	struct w32DIFLUT0_entry;
};
} SIE_DIFLUT0;
typedef union  T32DIFLUT0_entry
{ UNSG32 u32;
	struct w32DIFLUT0_entry;
} T32DIFLUT0_entry;
typedef union  TDIFLUT0_entry
{ UNSG32 u32[1];
	struct {
		struct w32DIFLUT0_entry;
	};
} TDIFLUT0_entry;
SIGN32 DIFLUT0_drvrd(SIE_DIFLUT0 *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 DIFLUT0_drvwr(SIE_DIFLUT0 *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void DIFLUT0_reset(SIE_DIFLUT0 *p);
SIGN32 DIFLUT0_cmp (SIE_DIFLUT0 *p, SIE_DIFLUT0 *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define DIFLUT0_check(p,pie,pfx,hLOG) DIFLUT0_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define DIFLUT0_print(p,    pfx,hLOG) DIFLUT0_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_DIF128_LUT
#define h_DIF128_LUT (){}
#define     RA_DIF128_LUT_DIFLUT0                          0x0000
typedef struct SIE_DIF128_LUT {
	SIE_DIFLUT0                                      ie_DIFLUT0[128];
} SIE_DIF128_LUT;
SIGN32 DIF128_LUT_drvrd(SIE_DIF128_LUT *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 DIF128_LUT_drvwr(SIE_DIF128_LUT *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void DIF128_LUT_reset(SIE_DIF128_LUT *p);
SIGN32 DIF128_LUT_cmp  (SIE_DIF128_LUT *p, SIE_DIF128_LUT *pie,
				char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define DIF128_LUT_check(p,pie,pfx,hLOG) DIF128_LUT_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define DIF128_LUT_print(p,    pfx,hLOG) DIF128_LUT_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_DIF_128
#define h_DIF_128 (){}
#define     RA_DIF_128_CFG0                                0x0000
#define     RA_DIF_128_DIFLUT                              0x0200
typedef struct SIE_DIF_128 {
#define     w32DIF_128_CFG0                                {\
	UNSG32 uCFG0_sft_rst                               :  1;\
	UNSG32 uCFG0_tile_size                             :  9;\
	UNSG32 uCFG0_PDWN0                                 :  1;\
	UNSG32 uCFG0_PDLVMC0                               :  1;\
	UNSG32 uCFG0_PDFVSSM0                              :  1;\
	UNSG32 uCFG0_PDWN1                                 :  1;\
	UNSG32 uCFG0_PDLVMC1                               :  1;\
	UNSG32 uCFG0_PDFVSSM1                              :  1;\
	UNSG32 uCFG0_PDWN2                                 :  1;\
	UNSG32 uCFG0_PDLVMC2                               :  1;\
	UNSG32 uCFG0_PDFVSSM2                              :  1;\
	UNSG32 RSVDx0_b19                                  : 13;\
}
union { UNSG32 u32DIF_128_CFG0;
	struct w32DIF_128_CFG0;
};
UNSG8 RSVDx4                                      [508];
SIE_DIF128_LUT                                   ie_DIFLUT;
} SIE_DIF_128;
typedef union  T32DIF_128_CFG0
{ UNSG32 u32;
	struct w32DIF_128_CFG0;
} T32DIF_128_CFG0;
typedef union  TDIF_128_CFG0
{ UNSG32 u32[1];
	struct {
		struct w32DIF_128_CFG0;
	};
} TDIF_128_CFG0;
SIGN32 DIF_128_drvrd(SIE_DIF_128 *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 DIF_128_drvwr(SIE_DIF_128 *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void DIF_128_reset(SIE_DIF_128 *p);
SIGN32 DIF_128_cmp (SIE_DIF_128 *p, SIE_DIF_128 *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define DIF_128_check(p,pie,pfx,hLOG) DIF_128_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define DIF_128_print(p,    pfx,hLOG) DIF_128_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_SRAMPWR
#define h_SRAMPWR (){}
#define     RA_SRAMPWR_ctrl                                0x0000
#define        SRAMPWR_ctrl_SD_ON                                       0x0
#define        SRAMPWR_ctrl_SD_SHUTDWN                                  0x1
#define        SRAMPWR_ctrl_DSLP_ON                                     0x0
#define        SRAMPWR_ctrl_DSLP_DEEPSLP                                0x1
#define        SRAMPWR_ctrl_SLP_ON                                      0x0
#define        SRAMPWR_ctrl_SLP_SLEEP                                   0x1
typedef struct SIE_SRAMPWR {
#define     w32SRAMPWR_ctrl                                {\
	UNSG32 uctrl_SD                                    :  1;\
	UNSG32 uctrl_DSLP                                  :  1;\
	UNSG32 uctrl_SLP                                   :  1;\
	UNSG32 RSVDx0_b3                                   : 29;\
}
union { UNSG32 u32SRAMPWR_ctrl;
	struct w32SRAMPWR_ctrl;
};
} SIE_SRAMPWR;
typedef union  T32SRAMPWR_ctrl
{ UNSG32 u32;
	struct w32SRAMPWR_ctrl;
} T32SRAMPWR_ctrl;
typedef union  TSRAMPWR_ctrl
{ UNSG32 u32[1];
	struct {
		struct w32SRAMPWR_ctrl;
	};
} TSRAMPWR_ctrl;
SIGN32 SRAMPWR_drvrd(SIE_SRAMPWR *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 SRAMPWR_drvwr(SIE_SRAMPWR *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void SRAMPWR_reset(SIE_SRAMPWR *p);
SIGN32 SRAMPWR_cmp (SIE_SRAMPWR *p, SIE_SRAMPWR *pie, char *pfx,
			void *hLOG, SIGN32 mem, SIGN32 tst);
#define SRAMPWR_check(p,pie,pfx,hLOG) SRAMPWR_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define SRAMPWR_print(p,    pfx,hLOG) SRAMPWR_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_SRAMRWTC
#define h_SRAMRWTC (){}
#define     RA_SRAMRWTC_ctrl0                              0x0000
#define     RA_SRAMRWTC_ctrl1                              0x0004
#define     RA_SRAMRWTC_ctrl2                              0x0008
typedef struct SIE_SRAMRWTC {
#define     w32SRAMRWTC_ctrl0                              {\
	UNSG32 uctrl0_RF1P                                 :  4;\
	UNSG32 uctrl0_UHDRF1P                              :  4;\
	UNSG32 uctrl0_RF2P                                 :  8;\
	UNSG32 uctrl0_UHDRF2P                              :  8;\
	UNSG32 uctrl0_UHDRF2P_ULVT                         :  8;\
}
union { UNSG32 u32SRAMRWTC_ctrl0;
	struct w32SRAMRWTC_ctrl0;
};
#define     w32SRAMRWTC_ctrl1                              {\
	UNSG32 uctrl1_SHDMBSR1P                            :  4;\
	UNSG32 uctrl1_SHDSBSR1P                            :  4;\
	UNSG32 uctrl1_SHCMBSR1P_SSEG                       :  4;\
	UNSG32 uctrl1_SHCMBSR1P_USEG                       :  4;\
	UNSG32 uctrl1_SHCSBSR1P                            :  4;\
	UNSG32 uctrl1_SHCSBSR1P_CUSTM                      :  4;\
	UNSG32 uctrl1_SPSRAM_WT0                           :  4;\
	UNSG32 uctrl1_SPSRAM_WT1                           :  4;\
}
union { UNSG32 u32SRAMRWTC_ctrl1;
	struct w32SRAMRWTC_ctrl1;
};
#define     w32SRAMRWTC_ctrl2                              {\
	UNSG32 uctrl2_L1CACHE                              :  4;\
	UNSG32 uctrl2_DPSR2P                               :  4;\
	UNSG32 uctrl2_ROM                                  :  8;\
	UNSG32 RSVDx8_b16                                  : 16;\
}
union { UNSG32 u32SRAMRWTC_ctrl2;
	struct w32SRAMRWTC_ctrl2;
};
} SIE_SRAMRWTC;
typedef union  T32SRAMRWTC_ctrl0
{ UNSG32 u32;
	struct w32SRAMRWTC_ctrl0;
} T32SRAMRWTC_ctrl0;
typedef union  T32SRAMRWTC_ctrl1
{ UNSG32 u32;
	struct w32SRAMRWTC_ctrl1;
} T32SRAMRWTC_ctrl1;
typedef union  T32SRAMRWTC_ctrl2
{ UNSG32 u32;
	struct w32SRAMRWTC_ctrl2;
} T32SRAMRWTC_ctrl2;
typedef union  TSRAMRWTC_ctrl0
{ UNSG32 u32[1];
	struct {
		struct w32SRAMRWTC_ctrl0;
	};
} TSRAMRWTC_ctrl0;
typedef union  TSRAMRWTC_ctrl1
{ UNSG32 u32[1];
	struct {
		struct w32SRAMRWTC_ctrl1;
	};
} TSRAMRWTC_ctrl1;
typedef union  TSRAMRWTC_ctrl2
{ UNSG32 u32[1];
	struct {
		struct w32SRAMRWTC_ctrl2;
	};
} TSRAMRWTC_ctrl2;
SIGN32 SRAMRWTC_drvrd(SIE_SRAMRWTC *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 SRAMRWTC_drvwr(SIE_SRAMRWTC *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void SRAMRWTC_reset(SIE_SRAMRWTC *p);
SIGN32 SRAMRWTC_cmp  (SIE_SRAMRWTC *p, SIE_SRAMRWTC *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define SRAMRWTC_check(p,pie,pfx,hLOG) SRAMRWTC_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define SRAMRWTC_print(p,    pfx,hLOG) SRAMRWTC_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_DTILE
#define h_DTILE (){}
#define     RA_DTILE_CTRL0                                 0x0000
#define     RA_DTILE_CTRL1                                 0x0004
#define     RA_DTILE_CTRL2                                 0x0008
#define     RA_DTILE_CTRL4                                 0x000C
#define     RA_DTILE_INVSCAN                               0x0010
#define     RA_DTILE_CTRL7                                 0x0014
typedef struct SIE_DTILE {
#define     w32DTILE_CTRL0                                 {\
	UNSG32 uCTRL0_tgEn_rstEn                           :  1;\
	UNSG32 uCTRL0_crop_de2_en                          :  1;\
	UNSG32 uCTRL0_cr_line_cnt                          :  3;\
	UNSG32 uCTRL0_tgclken_sel                          :  1;\
	UNSG32 uCTRL0_memlbUPS420_cgEnb                    :  1;\
	UNSG32 uCTRL0_auto_flush                           :  1;\
	UNSG32 RSVDx0_b8                                   : 24;\
}
union { UNSG32 u32DTILE_CTRL0;
	struct w32DTILE_CTRL0;
};
#define     w32DTILE_CTRL1                                 {\
	UNSG32 uCTRL1_lineCnt_R1_rstval0                   :  2;\
	UNSG32 uCTRL1_lineCnt_R1_rstval1                   :  2;\
	UNSG32 RSVDx4_b4                                   : 28;\
}
union { UNSG32 u32DTILE_CTRL1;
	struct w32DTILE_CTRL1;
};
#define     w32DTILE_CTRL2                                 {\
	UNSG32 uCTRL2_read_sel_422SP                       :  1;\
	UNSG32 uCTRL2_wr_ctrl1                             :  1;\
	UNSG32 uCTRL2_wr_ctrl2                             :  1;\
	UNSG32 RSVDx8_b3                                   : 29;\
}
union { UNSG32 u32DTILE_CTRL2;
	struct w32DTILE_CTRL2;
};
#define     w32DTILE_CTRL4                                 {\
	UNSG32 uCTRL4_UPS420_4K_bypass                     :  1;\
	UNSG32 uCTRL4_bitmap40_bypass                      :  1;\
	UNSG32 uCTRL4_dif_blk_en                           :  1;\
	UNSG32 RSVDxC_b3                                   : 29;\
}
union { UNSG32 u32DTILE_CTRL4;
	struct w32DTILE_CTRL4;
};
#define     w32DTILE_INVSCAN                               {\
	UNSG32 uINVSCAN_bitmap128_Rd0_en                   :  1;\
	UNSG32 uINVSCAN_bitmap128_Rd1_en                   :  1;\
	UNSG32 RSVDx10_b2                                  : 30;\
}
union { UNSG32 u32DTILE_INVSCAN;
	struct w32DTILE_INVSCAN;
};
#define     w32DTILE_CTRL7                                 {\
	UNSG32 uCTRL7_crop_de1_en                          :  1;\
	UNSG32 uCTRL7_sp_dwa_10b_normal_mode               :  1;\
	UNSG32 uCTRL7_sp_dwa_10b_inverse_mode              :  1;\
	UNSG32 uCTRL7_uv_mask_val0                         :  1;\
	UNSG32 uCTRL7_uv_mask_val1                         :  1;\
	UNSG32 uCTRL7_uv_mask_disable                      :  1;\
	UNSG32 RSVDx14_b6                                  : 26;\
}
union { UNSG32 u32DTILE_CTRL7;
	struct w32DTILE_CTRL7;
};
} SIE_DTILE;
typedef union  T32DTILE_CTRL0
{ UNSG32 u32;
	struct w32DTILE_CTRL0;
} T32DTILE_CTRL0;
typedef union  T32DTILE_CTRL1
{ UNSG32 u32;
	struct w32DTILE_CTRL1;
} T32DTILE_CTRL1;
typedef union  T32DTILE_CTRL2
{ UNSG32 u32;
	struct w32DTILE_CTRL2;
} T32DTILE_CTRL2;
typedef union  T32DTILE_CTRL4
{ UNSG32 u32;
	struct w32DTILE_CTRL4;
} T32DTILE_CTRL4;
typedef union  T32DTILE_INVSCAN
{ UNSG32 u32;
	struct w32DTILE_INVSCAN;
} T32DTILE_INVSCAN;
typedef union  T32DTILE_CTRL7
{ UNSG32 u32;
	struct w32DTILE_CTRL7;
} T32DTILE_CTRL7;
typedef union  TDTILE_CTRL0
{ UNSG32 u32[1];
	struct {
		struct w32DTILE_CTRL0;
	};
} TDTILE_CTRL0;
typedef union  TDTILE_CTRL1
{ UNSG32 u32[1];
	struct {
		struct w32DTILE_CTRL1;
	};
} TDTILE_CTRL1;
typedef union  TDTILE_CTRL2
{ UNSG32 u32[1];
	struct {
		struct w32DTILE_CTRL2;
	};
} TDTILE_CTRL2;
typedef union  TDTILE_CTRL4
{ UNSG32 u32[1];
	struct {
		struct w32DTILE_CTRL4;
	};
} TDTILE_CTRL4;
typedef union  TDTILE_INVSCAN
{ UNSG32 u32[1];
	struct {
		struct w32DTILE_INVSCAN;
	};
} TDTILE_INVSCAN;
typedef union  TDTILE_CTRL7
{ UNSG32 u32[1];
	struct {
		struct w32DTILE_CTRL7;
	};
} TDTILE_CTRL7;
SIGN32 DTILE_drvrd(SIE_DTILE *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 DTILE_drvwr(SIE_DTILE *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void DTILE_reset(SIE_DTILE *p);
SIGN32 DTILE_cmp  (SIE_DTILE *p, SIE_DTILE *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define DTILE_check(p,pie,pfx,hLOG) DTILE_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define DTILE_print(p,    pfx,hLOG) DTILE_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_TILE
#define h_TILE (){}
#define     RA_TILE_CTRL0                                  0x0000
#define     RA_TILE_CTRL1                                  0x0004
#define     RA_TILE_CTRL3                                  0x0008
#define     RA_TILE_CTRL4                                  0x000C
#define     RA_TILE_rst                                    0x0010
#define     RA_TILE_status                                 0x0014
typedef struct SIE_TILE {
#define     w32TILE_CTRL0                                  {\
	UNSG32 uCTRL0_isp_start                            :  1;\
	UNSG32 uCTRL0_isp_clear                            :  1;\
	UNSG32 uCTRL0_isp_fifo_clear0                      :  1;\
	UNSG32 uCTRL0_isp_fifo_clear1                      :  1;\
	UNSG32 uCTRL0_start_mask_reset                     :  1;\
	UNSG32 uCTRL0_tiler_en                             :  1;\
	UNSG32 RSVDx0_b6                                   : 26;\
}
union { UNSG32 u32TILE_CTRL0;
	struct w32TILE_CTRL0;
};
#define     w32TILE_CTRL1                                  {\
	UNSG32 uCTRL1_clken_ctrl0                          :  1;\
	UNSG32 uCTRL1_clken_ctrl1                          :  1;\
	UNSG32 uCTRL1_clken_ctrl2                          :  1;\
	UNSG32 uCTRL1_idata_sel                            :  3;\
	UNSG32 uCTRL1_idata_fifo_sel                       :  3;\
	UNSG32 uCTRL1_read_en_422SP                        :  1;\
	UNSG32 uCTRL1_rd_initval0                          :  1;\
	UNSG32 uCTRL1_rd_initval1                          :  1;\
	UNSG32 uCTRL1_rd_mask_sftrst                       :  1;\
	UNSG32 uCTRL1_read_sel                             :  1;\
	UNSG32 uCTRL1_crop_de_en                           :  1;\
	UNSG32 uCTRL1_bitmap30_en                          :  1;\
	UNSG32 uCTRL1_tile_width                           :  4;\
	UNSG32 uCTRL1_tile_depth                           :  4;\
	UNSG32 uCTRL1_vsync_cnt_val                        :  5;\
	UNSG32 uCTRL1_tgEof_en                             :  1;\
	UNSG32 RSVDx4_b30                                  :  2;\
}
union { UNSG32 u32TILE_CTRL1;
	struct w32TILE_CTRL1;
};
#define     w32TILE_CTRL3                                  {\
	UNSG32 uCTRL3_inPix_tot                            : 32;\
}
union { UNSG32 u32TILE_CTRL3;
	struct w32TILE_CTRL3;
};
#define     w32TILE_CTRL4                                  {\
	UNSG32 uCTRL4_xres                                 : 12;\
	UNSG32 uCTRL4_yres                                 : 12;\
	UNSG32 uCTRL4_uv_mask_disable                      :  1;\
	UNSG32 uCTRL4_uv_mask_val0                         :  1;\
	UNSG32 uCTRL4_uv_mask_val1                         :  1;\
	UNSG32 uCTRL4_uv_swap_reg                          :  1;\
	UNSG32 RSVDxC_b28                                  :  4;\
}
union { UNSG32 u32TILE_CTRL4;
	struct w32TILE_CTRL4;
};
#define     w32TILE_rst                                    {\
	UNSG32 urst_tile                                   :  1;\
	UNSG32 urst_memlb                                  :  1;\
	UNSG32 RSVDx10_b2                                  : 30;\
}
union { UNSG32 u32TILE_rst;
	struct w32TILE_rst;
};
#define     w32TILE_status                                 {\
	UNSG32 ustatus_isp_fifo_underflow                  :  1;\
	UNSG32 ustatus_isp_fifo_overflow                   :  1;\
	UNSG32 RSVDx14_b2                                  : 30;\
}
union { UNSG32 u32TILE_status;
	struct w32TILE_status;
};
} SIE_TILE;
typedef union  T32TILE_CTRL0
{ UNSG32 u32;
	struct w32TILE_CTRL0;
} T32TILE_CTRL0;
typedef union  T32TILE_CTRL1
{ UNSG32 u32;
	struct w32TILE_CTRL1;
} T32TILE_CTRL1;
typedef union  T32TILE_CTRL3
{ UNSG32 u32;
	struct w32TILE_CTRL3;
} T32TILE_CTRL3;
typedef union  T32TILE_CTRL4
{ UNSG32 u32;
	struct w32TILE_CTRL4;
} T32TILE_CTRL4;
typedef union  T32TILE_rst
{ UNSG32 u32;
	struct w32TILE_rst;
} T32TILE_rst;
typedef union  T32TILE_status
{ UNSG32 u32;
	struct w32TILE_status;
} T32TILE_status;
typedef union  TTILE_CTRL0
{ UNSG32 u32[1];
	struct {
		struct w32TILE_CTRL0;
	};
} TTILE_CTRL0;
typedef union  TTILE_CTRL1
{ UNSG32 u32[1];
	struct {
		struct w32TILE_CTRL1;
	};
} TTILE_CTRL1;
typedef union  TTILE_CTRL3
{ UNSG32 u32[1];
	struct {
		struct w32TILE_CTRL3;
	};
} TTILE_CTRL3;
typedef union  TTILE_CTRL4
{ UNSG32 u32[1];
	struct {
		struct w32TILE_CTRL4;
	};
} TTILE_CTRL4;
typedef union  TTILE_rst
{ UNSG32 u32[1];
	struct {
		struct w32TILE_rst;
	};
} TTILE_rst;
typedef union  TTILE_status
{ UNSG32 u32[1];
	struct {
		struct w32TILE_status;
	};
} TTILE_status;
SIGN32 TILE_drvrd(SIE_TILE *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 TILE_drvwr(SIE_TILE *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void TILE_reset(SIE_TILE *p);
SIGN32 TILE_cmp  (SIE_TILE *p, SIE_TILE *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define TILE_check(p,pie,pfx,hLOG) TILE_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define TILE_print(p,    pfx,hLOG) TILE_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_WriteCmd
#define h_WriteCmd (){}
#define     RA_WriteCmd_CTRL0                              0x0000
#define     RA_WriteCmd_CTRL1                              0x0004
#define     RA_WriteCmd_CTRL2                              0x0008
#define     RA_WriteCmd_CTRL3                              0x000C
#define     RA_WriteCmd_CTRL4                              0x0010
#define     RA_WriteCmd_CTRL5                              0x0014
#define     RA_WriteCmd_CTRL6                              0x0018
typedef struct SIE_WriteCmd {
#define     w32WriteCmd_CTRL0                              {\
	UNSG32 uCTRL0_RS_Mode_en                           :  1;\
	UNSG32 uCTRL0_burst_size                           :  9;\
	UNSG32 uCTRL0_dHubIrqEnY                           :  1;\
	UNSG32 uCTRL0_dHubIrqEnC                           :  1;\
	UNSG32 uCTRL0_YUVmode                              :  2;\
	UNSG32 uCTRL0_nDhubCmds_tl                         :  4;\
	UNSG32 uCTRL0_dhubWr_losMsk                        :  1;\
	UNSG32 uCTRL0_dhubWr_lgdcLosMsk                    :  1;\
	UNSG32 RSVDx0_b20                                  : 12;\
}
union { UNSG32 u32WriteCmd_CTRL0;
	struct w32WriteCmd_CTRL0;
};
#define     w32WriteCmd_CTRL1                              {\
	UNSG32 uCTRL1_bytLineTot                           : 16;\
	UNSG32 uCTRL1_TileHwidth                           :  9;\
	UNSG32 uCTRL1_TileVdepth                           :  4;\
	UNSG32 RSVDx4_b29                                  :  3;\
}
union { UNSG32 u32WriteCmd_CTRL1;
	struct w32WriteCmd_CTRL1;
};
#define     w32WriteCmd_CTRL2                              {\
	UNSG32 uCTRL2_BaseAddrY                            : 32;\
}
union { UNSG32 u32WriteCmd_CTRL2;
	struct w32WriteCmd_CTRL2;
};
#define     w32WriteCmd_CTRL3                              {\
	UNSG32 uCTRL3_BaseAddrC                            : 32;\
}
union { UNSG32 u32WriteCmd_CTRL3;
	struct w32WriteCmd_CTRL3;
};
#define     w32WriteCmd_CTRL4                              {\
	UNSG32 uCTRL4_Stride                               : 20;\
	UNSG32 RSVDx10_b20                                 : 12;\
}
union { UNSG32 u32WriteCmd_CTRL4;
	struct w32WriteCmd_CTRL4;
};
#define     w32WriteCmd_CTRL5                              {\
	UNSG32 uCTRL5_StrideJump                           : 20;\
	UNSG32 RSVDx14_b20                                 : 12;\
}
union { UNSG32 u32WriteCmd_CTRL5;
	struct w32WriteCmd_CTRL5;
};
#define     w32WriteCmd_CTRL6                              {\
	UNSG32 uCTRL6_nStrides                             : 16;\
	UNSG32 uCTRL6_Chroma_nStrides                      : 16;\
}
union { UNSG32 u32WriteCmd_CTRL6;
	struct w32WriteCmd_CTRL6;
};
} SIE_WriteCmd;
typedef union  T32WriteCmd_CTRL0
{ UNSG32 u32;
	struct w32WriteCmd_CTRL0;
} T32WriteCmd_CTRL0;
typedef union  T32WriteCmd_CTRL1
{ UNSG32 u32;
	struct w32WriteCmd_CTRL1;
} T32WriteCmd_CTRL1;
typedef union  T32WriteCmd_CTRL2
{ UNSG32 u32;
	struct w32WriteCmd_CTRL2;
} T32WriteCmd_CTRL2;
typedef union  T32WriteCmd_CTRL3
{ UNSG32 u32;
	struct w32WriteCmd_CTRL3;
} T32WriteCmd_CTRL3;
typedef union  T32WriteCmd_CTRL4
{ UNSG32 u32;
	struct w32WriteCmd_CTRL4;
} T32WriteCmd_CTRL4;
typedef union  T32WriteCmd_CTRL5
{ UNSG32 u32;
	struct w32WriteCmd_CTRL5;
} T32WriteCmd_CTRL5;
typedef union  T32WriteCmd_CTRL6
{ UNSG32 u32;
	struct w32WriteCmd_CTRL6;
} T32WriteCmd_CTRL6;
typedef union  TWriteCmd_CTRL0
{ UNSG32 u32[1];
	struct {
		struct w32WriteCmd_CTRL0;
	};
} TWriteCmd_CTRL0;
typedef union  TWriteCmd_CTRL1
{ UNSG32 u32[1];
	struct {
		struct w32WriteCmd_CTRL1;
	};
} TWriteCmd_CTRL1;
typedef union  TWriteCmd_CTRL2
{ UNSG32 u32[1];
	struct {
		struct w32WriteCmd_CTRL2;
	};
} TWriteCmd_CTRL2;
typedef union  TWriteCmd_CTRL3
{ UNSG32 u32[1];
	struct {
		struct w32WriteCmd_CTRL3;
	};
} TWriteCmd_CTRL3;
typedef union  TWriteCmd_CTRL4
{ UNSG32 u32[1];
	struct {
		struct w32WriteCmd_CTRL4;
	};
} TWriteCmd_CTRL4;
typedef union  TWriteCmd_CTRL5
{ UNSG32 u32[1];
	struct {
		struct w32WriteCmd_CTRL5;
	};
} TWriteCmd_CTRL5;
typedef union  TWriteCmd_CTRL6
{ UNSG32 u32[1];
	struct {
		struct w32WriteCmd_CTRL6;
	};
} TWriteCmd_CTRL6;
SIGN32 WriteCmd_drvrd(SIE_WriteCmd *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 WriteCmd_drvwr(SIE_WriteCmd *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void WriteCmd_reset(SIE_WriteCmd *p);
SIGN32 WriteCmd_cmp  (SIE_WriteCmd *p, SIE_WriteCmd *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define WriteCmd_check(p,pie,pfx,hLOG) WriteCmd_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define WriteCmd_print(p,    pfx,hLOG) WriteCmd_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_TILEWRAP
#define h_TILEWRAP (){}
#define     RA_TILEWRAP_CTRL                               0x0000
#define     RA_TILEWRAP_WR0                                0x0004
#define     RA_TILEWRAP_WR1                                0x0014
#define     RA_TILEWRAP_WRCMD                              0x0024
typedef struct SIE_TILEWRAP {
#define     w32TILEWRAP_CTRL                               {\
	UNSG32 uCTRL_wr0_start                             :  1;\
	UNSG32 uCTRL_wr1_start                             :  1;\
	UNSG32 uCTRL_wr0_clear                             :  1;\
	UNSG32 uCTRL_wr1_clear                             :  1;\
	UNSG32 uCTRL_chroma_data_sel                       :  1;\
	UNSG32 uCTRL_luma_data_sel                         :  3;\
	UNSG32 uCTRL_420sp_wrbk                            :  1;\
	UNSG32 uCTRL_422sp_wren                            :  1;\
	UNSG32 uCTRL_420sp_dwa10b_en                       :  1;\
	UNSG32 RSVDx0_b11                                  : 21;\
}
union { UNSG32 u32TILEWRAP_CTRL;
	struct w32TILEWRAP_CTRL;
};
SIE_WriteClient                                  ie_WR0;
SIE_WriteClient                                  ie_WR1;
SIE_WriteCmd                                     ie_WRCMD;
} SIE_TILEWRAP;
typedef union  T32TILEWRAP_CTRL
{ UNSG32 u32;
	struct w32TILEWRAP_CTRL;
} T32TILEWRAP_CTRL;
typedef union  TTILEWRAP_CTRL
{ UNSG32 u32[1];
	struct {
		struct w32TILEWRAP_CTRL;
	};
} TTILEWRAP_CTRL;
SIGN32 TILEWRAP_drvrd(SIE_TILEWRAP *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 TILEWRAP_drvwr(SIE_TILEWRAP *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void TILEWRAP_reset(SIE_TILEWRAP *p);
SIGN32 TILEWRAP_cmp  (SIE_TILEWRAP *p, SIE_TILEWRAP *pie,
				char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define TILEWRAP_check(p,pie,pfx,hLOG) TILEWRAP_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define TILEWRAP_print(p,    pfx,hLOG) TILEWRAP_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_TG_MAINB
#define h_TG_MAINB (){}
#define     RA_TG_MAINB_INIT                               0x0000
#define     RA_TG_MAINB_SIZE                               0x0004
#define     RA_TG_MAINB_HS                                 0x0008
#define     RA_TG_MAINB_HB                                 0x000C
#define     RA_TG_MAINB_HB_Y                               0x0010
#define     RA_TG_MAINB_HB_C                               0x0014
#define     RA_TG_MAINB_HB_OUT                             0x0018
#define     RA_TG_MAINB_VS0                                0x001C
#define     RA_TG_MAINB_VS1                                0x0020
#define     RA_TG_MAINB_VB0                                0x0024
#define     RA_TG_MAINB_VB0_Y                              0x0028
#define     RA_TG_MAINB_VB0_C                              0x002C
#define     RA_TG_MAINB_VB0_OUT                            0x0030
#define     RA_TG_MAINB_VB1                                0x0034
#define     RA_TG_MAINB_SCAN                               0x0038
#define        TG_MAINB_SCAN_MODE_PROG                                  0x0
#define        TG_MAINB_SCAN_MODE_INTER                                 0x1
#define     RA_TG_MAINB_INTPOS                             0x003C
#define     RA_TG_MAINB_MODE                               0x0040
#define        TG_MAINB_MODE_EN_MASTER                                  0x0
#define        TG_MAINB_MODE_EN_SLAVE                                   0x1
#define     RA_TG_MAINB_HVREF                              0x0044
#define        TG_MAINB_HVREF_SEL_SYNC                                  0x0
#define        TG_MAINB_HVREF_SEL_BLANK                                 0x1
#define        TG_MAINB_HVREF_POL_NEG_PULSE                             0x0
#define        TG_MAINB_HVREF_POL_POS_PULSE                             0x1
#define     RA_TG_MAINB_BURST                              0x0048
#define     RA_TG_MAINB_BURSTPOS                           0x004C
typedef struct SIE_TG_MAINB {
#define     w32TG_MAINB_INIT                               {\
	UNSG32 uINIT_Y                                     : 16;\
	UNSG32 uINIT_X                                     : 16;\
}
union { UNSG32 u32TG_MAINB_INIT;
	struct w32TG_MAINB_INIT;
};
#define     w32TG_MAINB_SIZE                               {\
	UNSG32 uSIZE_Y                                     : 16;\
	UNSG32 uSIZE_X                                     : 16;\
}
union { UNSG32 u32TG_MAINB_SIZE;
	struct w32TG_MAINB_SIZE;
};
#define     w32TG_MAINB_HS                                 {\
	UNSG32 uHS_FE                                      : 16;\
	UNSG32 uHS_BE                                      : 16;\
}
union { UNSG32 u32TG_MAINB_HS;
	struct w32TG_MAINB_HS;
};
#define     w32TG_MAINB_HB                                 {\
	UNSG32 uHB_FE                                      : 16;\
	UNSG32 uHB_BE                                      : 16;\
}
union { UNSG32 u32TG_MAINB_HB;
	struct w32TG_MAINB_HB;
};
#define     w32TG_MAINB_HB_Y                               {\
	UNSG32 uHB_Y_FE                                    : 16;\
	UNSG32 uHB_Y_BE                                    : 16;\
}
union { UNSG32 u32TG_MAINB_HB_Y;
	struct w32TG_MAINB_HB_Y;
};
#define     w32TG_MAINB_HB_C                               {\
	UNSG32 uHB_C_FE                                    : 16;\
	UNSG32 uHB_C_BE                                    : 16;\
}
union { UNSG32 u32TG_MAINB_HB_C;
	struct w32TG_MAINB_HB_C;
};
#define     w32TG_MAINB_HB_OUT                             {\
	UNSG32 uHB_OUT_FE                                  : 16;\
	UNSG32 uHB_OUT_BE                                  : 16;\
}
union { UNSG32 u32TG_MAINB_HB_OUT;
	struct w32TG_MAINB_HB_OUT;
};
#define     w32TG_MAINB_VS0                                {\
	UNSG32 uVS0_FE                                     : 16;\
	UNSG32 uVS0_BE                                     : 16;\
}
union { UNSG32 u32TG_MAINB_VS0;
	struct w32TG_MAINB_VS0;
};
#define     w32TG_MAINB_VS1                                {\
	UNSG32 uVS1_FE                                     : 16;\
	UNSG32 uVS1_BE                                     : 16;\
}
union { UNSG32 u32TG_MAINB_VS1;
	struct w32TG_MAINB_VS1;
};
#define     w32TG_MAINB_VB0                                {\
	UNSG32 uVB0_FE                                     : 16;\
	UNSG32 uVB0_BE                                     : 16;\
}
union { UNSG32 u32TG_MAINB_VB0;
	struct w32TG_MAINB_VB0;
};
#define     w32TG_MAINB_VB0_Y                              {\
	UNSG32 uVB0_Y_FE                                   : 16;\
	UNSG32 uVB0_Y_BE                                   : 16;\
}
union { UNSG32 u32TG_MAINB_VB0_Y;
	struct w32TG_MAINB_VB0_Y;
};
#define     w32TG_MAINB_VB0_C                              {\
	UNSG32 uVB0_C_FE                                   : 16;\
	UNSG32 uVB0_C_BE                                   : 16;\
}
union { UNSG32 u32TG_MAINB_VB0_C;
	struct w32TG_MAINB_VB0_C;
};
#define     w32TG_MAINB_VB0_OUT                            {\
	UNSG32 uVB0_OUT_FE                                 : 16;\
	UNSG32 uVB0_OUT_BE                                 : 16;\
}
union { UNSG32 u32TG_MAINB_VB0_OUT;
	struct w32TG_MAINB_VB0_OUT;
};
#define     w32TG_MAINB_VB1                                {\
	UNSG32 uVB1_FE                                     : 16;\
	UNSG32 uVB1_BE                                     : 16;\
}
union { UNSG32 u32TG_MAINB_VB1;
	struct w32TG_MAINB_VB1;
};
#define     w32TG_MAINB_SCAN                               {\
	UNSG32 uSCAN_MODE                                  :  1;\
	UNSG32 RSVDx38_b1                                  : 31;\
}
union { UNSG32 u32TG_MAINB_SCAN;
	struct w32TG_MAINB_SCAN;
};
#define     w32TG_MAINB_INTPOS                             {\
	UNSG32 uINTPOS_FRAME                               : 16;\
	UNSG32 uINTPOS_FIELD                               : 16;\
}
union { UNSG32 u32TG_MAINB_INTPOS;
	struct w32TG_MAINB_INTPOS;
};
#define     w32TG_MAINB_MODE                               {\
	UNSG32 uMODE_EN                                    :  1;\
	UNSG32 uMODE_BURST                                 :  1;\
	UNSG32 RSVDx40_b2                                  : 30;\
}
union { UNSG32 u32TG_MAINB_MODE;
	struct w32TG_MAINB_MODE;
};
#define     w32TG_MAINB_HVREF                              {\
	UNSG32 uHVREF_SEL                                  :  1;\
	UNSG32 uHVREF_POL                                  :  1;\
	UNSG32 RSVDx44_b2                                  : 30;\
}
union { UNSG32 u32TG_MAINB_HVREF;
	struct w32TG_MAINB_HVREF;
};
#define     w32TG_MAINB_BURST                              {\
	UNSG32 uBURST_EN                                   :  1;\
	UNSG32 uBURST_MODE64                               :  1;\
	UNSG32 uBURST_SIZE                                 : 10;\
	UNSG32 uBURST_FE                                   : 10;\
	UNSG32 uBURST_BE                                   : 10;\
}
union { UNSG32 u32TG_MAINB_BURST;
	struct w32TG_MAINB_BURST;
};
#define     w32TG_MAINB_BURSTPOS                           {\
	UNSG32 uBURSTPOS_START                             : 16;\
	UNSG32 uBURSTPOS_END                               : 16;\
}
union { UNSG32 u32TG_MAINB_BURSTPOS;
	struct w32TG_MAINB_BURSTPOS;
};
} SIE_TG_MAINB;
typedef union  T32TG_MAINB_INIT
{ UNSG32 u32;
	struct w32TG_MAINB_INIT;
} T32TG_MAINB_INIT;
typedef union  T32TG_MAINB_SIZE
{ UNSG32 u32;
	struct w32TG_MAINB_SIZE;
} T32TG_MAINB_SIZE;
typedef union  T32TG_MAINB_HS
{ UNSG32 u32;
	struct w32TG_MAINB_HS;
} T32TG_MAINB_HS;
typedef union  T32TG_MAINB_HB
{ UNSG32 u32;
	struct w32TG_MAINB_HB;
} T32TG_MAINB_HB;
typedef union  T32TG_MAINB_HB_Y
{ UNSG32 u32;
	struct w32TG_MAINB_HB_Y;
} T32TG_MAINB_HB_Y;
typedef union  T32TG_MAINB_HB_C
{ UNSG32 u32;
	struct w32TG_MAINB_HB_C;
} T32TG_MAINB_HB_C;
typedef union  T32TG_MAINB_HB_OUT
{ UNSG32 u32;
	struct w32TG_MAINB_HB_OUT;
} T32TG_MAINB_HB_OUT;
typedef union  T32TG_MAINB_VS0
{ UNSG32 u32;
	struct w32TG_MAINB_VS0;
} T32TG_MAINB_VS0;
typedef union  T32TG_MAINB_VS1
{ UNSG32 u32;
	struct w32TG_MAINB_VS1;
} T32TG_MAINB_VS1;
typedef union  T32TG_MAINB_VB0
{ UNSG32 u32;
	struct w32TG_MAINB_VB0;
} T32TG_MAINB_VB0;
typedef union  T32TG_MAINB_VB0_Y
{ UNSG32 u32;
	struct w32TG_MAINB_VB0_Y;
} T32TG_MAINB_VB0_Y;
typedef union  T32TG_MAINB_VB0_C
{ UNSG32 u32;
	struct w32TG_MAINB_VB0_C;
} T32TG_MAINB_VB0_C;
typedef union  T32TG_MAINB_VB0_OUT
{ UNSG32 u32;
	struct w32TG_MAINB_VB0_OUT;
} T32TG_MAINB_VB0_OUT;
typedef union  T32TG_MAINB_VB1
{ UNSG32 u32;
	struct w32TG_MAINB_VB1;
} T32TG_MAINB_VB1;
typedef union  T32TG_MAINB_SCAN
{ UNSG32 u32;
	struct w32TG_MAINB_SCAN;
} T32TG_MAINB_SCAN;
typedef union  T32TG_MAINB_INTPOS
{ UNSG32 u32;
	struct w32TG_MAINB_INTPOS;
} T32TG_MAINB_INTPOS;
typedef union  T32TG_MAINB_MODE
{ UNSG32 u32;
	struct w32TG_MAINB_MODE;
} T32TG_MAINB_MODE;
typedef union  T32TG_MAINB_HVREF
{ UNSG32 u32;
	struct w32TG_MAINB_HVREF;
} T32TG_MAINB_HVREF;
typedef union  T32TG_MAINB_BURST
{ UNSG32 u32;
	struct w32TG_MAINB_BURST;
} T32TG_MAINB_BURST;
typedef union  T32TG_MAINB_BURSTPOS
{ UNSG32 u32;
	struct w32TG_MAINB_BURSTPOS;
} T32TG_MAINB_BURSTPOS;
typedef union  TTG_MAINB_INIT
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_INIT;
	};
} TTG_MAINB_INIT;
typedef union  TTG_MAINB_SIZE
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_SIZE;
	};
} TTG_MAINB_SIZE;
typedef union  TTG_MAINB_HS
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_HS;
	};
} TTG_MAINB_HS;
typedef union  TTG_MAINB_HB
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_HB;
	};
} TTG_MAINB_HB;
typedef union  TTG_MAINB_HB_Y
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_HB_Y;
	};
} TTG_MAINB_HB_Y;
typedef union  TTG_MAINB_HB_C
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_HB_C;
	};
} TTG_MAINB_HB_C;
typedef union  TTG_MAINB_HB_OUT
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_HB_OUT;
	};
} TTG_MAINB_HB_OUT;
typedef union  TTG_MAINB_VS0
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_VS0;
	};
} TTG_MAINB_VS0;
typedef union  TTG_MAINB_VS1
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_VS1;
	};
} TTG_MAINB_VS1;
typedef union  TTG_MAINB_VB0
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_VB0;
	};
} TTG_MAINB_VB0;
typedef union  TTG_MAINB_VB0_Y
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_VB0_Y;
	};
} TTG_MAINB_VB0_Y;
typedef union  TTG_MAINB_VB0_C
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_VB0_C;
	};
} TTG_MAINB_VB0_C;
typedef union  TTG_MAINB_VB0_OUT
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_VB0_OUT;
	};
} TTG_MAINB_VB0_OUT;
typedef union  TTG_MAINB_VB1
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_VB1;
	};
} TTG_MAINB_VB1;
typedef union  TTG_MAINB_SCAN
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_SCAN;
	};
} TTG_MAINB_SCAN;
typedef union  TTG_MAINB_INTPOS
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_INTPOS;
	};
} TTG_MAINB_INTPOS;
typedef union  TTG_MAINB_MODE
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_MODE;
	};
} TTG_MAINB_MODE;
typedef union  TTG_MAINB_HVREF
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_HVREF;
	};
} TTG_MAINB_HVREF;
typedef union  TTG_MAINB_BURST
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_BURST;
	};
} TTG_MAINB_BURST;
typedef union  TTG_MAINB_BURSTPOS
{ UNSG32 u32[1];
	struct {
		struct w32TG_MAINB_BURSTPOS;
	};
} TTG_MAINB_BURSTPOS;
SIGN32 TG_MAINB_drvrd(SIE_TG_MAINB *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 TG_MAINB_drvwr(SIE_TG_MAINB *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void TG_MAINB_reset(SIE_TG_MAINB *p);
SIGN32 TG_MAINB_cmp  (SIE_TG_MAINB *p, SIE_TG_MAINB *pie,
			char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
#define TG_MAINB_check(p,pie,pfx,hLOG) TG_MAINB_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define TG_MAINB_print(p,    pfx,hLOG) TG_MAINB_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_TILEPROC
#define h_TILEPROC (){}
#define     RA_TILEPROC_VP_CLKEN_CTRL                      0x0000
#define     RA_TILEPROC_DUMMY0                             0x0004
#define     RA_TILEPROC_VCNTSTS0                           0x0008
#define     RA_TILEPROC_rst                                0x000C
#define     RA_TILEPROC_ClientIFm                          0x0010
#define     RA_TILEPROC_UPS420_422Main_Y                   0x0040
#define     RA_TILEPROC_UPS420_422Main_C                   0x006C
#define     RA_TILEPROC_DTILE                              0x0098
#define     RA_TILEPROC_feTG                               0x00B0
#define     RA_TILEPROC_TILE                               0x0100
#define     RA_TILEPROC_bitmap40                           0x0118
#define     RA_TILEPROC_bitmap32                           0x0138
#define     RA_TILEPROC_DIF_Y                              0x0200
#define     RA_TILEPROC_DIF_C                              0x0600
#define     RA_TILEPROC_TILEWRAP                           0x0A00
#define     RA_TILEPROC_SRAMPWR_LINE_BUF                   0x0A40
#define     RA_TILEPROC_SRAMPWR_DIF_MEM                    0x0A44
typedef struct SIE_TILEPROC {
#define     w32TILEPROC_VP_CLKEN_CTRL                      {\
	UNSG32 uVP_CLKEN_CTRL_toggle                       :  1;\
	UNSG32 RSVDx0_b1                                   : 31;\
}
union { UNSG32 u32TILEPROC_VP_CLKEN_CTRL;
	struct w32TILEPROC_VP_CLKEN_CTRL;
};
#define     w32TILEPROC_DUMMY0                             {\
	UNSG32 uDUMMY0_ctrl                                : 32;\
}
union { UNSG32 u32TILEPROC_DUMMY0;
	struct w32TILEPROC_DUMMY0;
};
#define     w32TILEPROC_VCNTSTS0                           {\
	UNSG32 uVCNTSTS0_VCNTSTS_fetg                      : 16;\
	UNSG32 RSVDx8_b16                                  : 16;\
}
union { UNSG32 u32TILEPROC_VCNTSTS0;
	struct w32TILEPROC_VCNTSTS0;
};
#define     w32TILEPROC_rst                                {\
	UNSG32 urst_detile                                 :  1;\
	UNSG32 urst_memlb_ups420                           :  1;\
	UNSG32 urst_isp                                    :  1;\
	UNSG32 RSVDxC_b3                                   : 29;\
}
union { UNSG32 u32TILEPROC_rst;
	struct w32TILEPROC_rst;
};
SIE_ClientIF                                     ie_ClientIFm;
SIE_UPS_420_422                                  ie_UPS420_422Main_Y;
SIE_UPS_420_422                                  ie_UPS420_422Main_C;
SIE_DTILE                                        ie_DTILE;
SIE_TG_MAINB                                     ie_feTG;
SIE_TILE                                         ie_TILE;
SIE_BITMAP40                                     ie_bitmap40;
SIE_BITMAP32                                     ie_bitmap32;
UNSG8 RSVDx150                                    [176];
SIE_DIF_128                                      ie_DIF_Y;
SIE_DIF_128                                      ie_DIF_C;
SIE_TILEWRAP                                     ie_TILEWRAP;
SIE_SRAMPWR                                      ie_SRAMPWR_LINE_BUF;
SIE_SRAMPWR                                      ie_SRAMPWR_DIF_MEM;
UNSG8 RSVDxA48                                    [440];
} SIE_TILEPROC;
typedef union  T32TILEPROC_VP_CLKEN_CTRL
{ UNSG32 u32;
	struct w32TILEPROC_VP_CLKEN_CTRL;
} T32TILEPROC_VP_CLKEN_CTRL;
typedef union  T32TILEPROC_DUMMY0
{ UNSG32 u32;
	struct w32TILEPROC_DUMMY0;
} T32TILEPROC_DUMMY0;
typedef union  T32TILEPROC_VCNTSTS0
{ UNSG32 u32;
	struct w32TILEPROC_VCNTSTS0;
} T32TILEPROC_VCNTSTS0;
typedef union  T32TILEPROC_rst
{ UNSG32 u32;
	struct w32TILEPROC_rst;
} T32TILEPROC_rst;
typedef union  TTILEPROC_VP_CLKEN_CTRL
{ UNSG32 u32[1];
	struct {
		struct w32TILEPROC_VP_CLKEN_CTRL;
	};
} TTILEPROC_VP_CLKEN_CTRL;
typedef union  TTILEPROC_DUMMY0
{ UNSG32 u32[1];
	struct {
		struct w32TILEPROC_DUMMY0;
	};
} TTILEPROC_DUMMY0;
typedef union  TTILEPROC_VCNTSTS0
{ UNSG32 u32[1];
	struct {
		struct w32TILEPROC_VCNTSTS0;
	};
} TTILEPROC_VCNTSTS0;
typedef union  TTILEPROC_rst
{ UNSG32 u32[1];
	struct {
		struct w32TILEPROC_rst;
	};
} TTILEPROC_rst;
SIGN32 TILEPROC_drvrd(SIE_TILEPROC *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
SIGN32 TILEPROC_drvwr(SIE_TILEPROC *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
void TILEPROC_reset(SIE_TILEPROC *p);
SIGN32 TILEPROC_cmp  (SIE_TILEPROC *p, SIE_TILEPROC *pie, char *pfx,
			void *hLOG, SIGN32 mem, SIGN32 tst);
#define TILEPROC_check(p,pie,pfx,hLOG) TILEPROC_cmp(p,pie,pfx,(void*)(hLOG),0,0)
#define TILEPROC_print(p,    pfx,hLOG) TILEPROC_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifdef __cplusplus
}
#endif
#pragma  pack()
#endif
