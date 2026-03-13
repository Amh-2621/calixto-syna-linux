#ifndef deWarp_h
#define deWarp_h (){}
#include "ctypes.h"
#pragma pack(1)
#ifdef __cplusplus
  extern "C" {
#endif
#ifndef _DOCC_H_BITOPS_
#define _DOCC_H_BITOPS_ (){}
    #define _bSETMASK_(b)                                      ((b)<32 ? (1<<((b)&31)) : 0)
    #define _NSETMASK_(msb,lsb)                                (_bSETMASK_((msb)+1)-_bSETMASK_(lsb))
    #define _bCLRMASK_(b)                                      (~_bSETMASK_(b))
    #define _NCLRMASK_(msb,lsb)                                (~_NSETMASK_(msb,lsb))
    #define _BFGET_(r,msb,lsb)                                 (_NSETMASK_((msb)-(lsb),0)&((r)>>(lsb)))
    #define _BFSET_(r,msb,lsb,v)                               do{ (r)&=_NCLRMASK_(msb,lsb); (r)|=_NSETMASK_(msb,lsb)&((v)<<(lsb)); }while(0)
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
     SIGN32 TG_PL_cmp  (SIE_TG_PL *p, SIE_TG_PL *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
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
     SIGN32 TG_PRG_cmp  (SIE_TG_PRG *p, SIE_TG_PRG *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
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
     SIGN32 TG_MAIN_cmp  (SIE_TG_MAIN *p, SIE_TG_MAIN *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
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
     SIGN32 BITMAP40_cmp  (SIE_BITMAP40 *p, SIE_BITMAP40 *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
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
     SIGN32 BITMAP20_cmp  (SIE_BITMAP20 *p, SIE_BITMAP20 *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
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
     SIGN32 BITMAP32_cmp  (SIE_BITMAP32 *p, SIE_BITMAP32 *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
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
     SIGN32 BITMAP16_cmp  (SIE_BITMAP16 *p, SIE_BITMAP16 *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
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
     SIGN32 BITMAP12_cmp  (SIE_BITMAP12 *p, SIE_BITMAP12 *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
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
     SIGN32 ReadClient_cmp  (SIE_ReadClient *p, SIE_ReadClient *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
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
     SIGN32 WriteClient_cmp  (SIE_WriteClient *p, SIE_WriteClient *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
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
     SIGN32 ClientIF_cmp  (SIE_ClientIF *p, SIE_ClientIF *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
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
     SIGN32 DIFLUT0_cmp  (SIE_DIFLUT0 *p, SIE_DIFLUT0 *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
    #define DIFLUT0_check(p,pie,pfx,hLOG) DIFLUT0_cmp(p,pie,pfx,(void*)(hLOG),0,0)
    #define DIFLUT0_print(p,    pfx,hLOG) DIFLUT0_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_DIF64_LUT
#define h_DIF64_LUT (){}
    #define     RA_DIF64_LUT_DIFLUT0                           0x0000
    typedef struct SIE_DIF64_LUT {
              SIE_DIFLUT0                                      ie_DIFLUT0[64];
    } SIE_DIF64_LUT;
     SIGN32 DIF64_LUT_drvrd(SIE_DIF64_LUT *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
     SIGN32 DIF64_LUT_drvwr(SIE_DIF64_LUT *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
       void DIF64_LUT_reset(SIE_DIF64_LUT *p);
     SIGN32 DIF64_LUT_cmp  (SIE_DIF64_LUT *p, SIE_DIF64_LUT *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
    #define DIF64_LUT_check(p,pie,pfx,hLOG) DIF64_LUT_cmp(p,pie,pfx,(void*)(hLOG),0,0)
    #define DIF64_LUT_print(p,    pfx,hLOG) DIF64_LUT_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_DIF64
#define h_DIF64 (){}
    #define     RA_DIF64_CFG0                                  0x0000
    #define     RA_DIF64_DIFLUT                                0x0100
    typedef struct SIE_DIF64 {
    #define     w32DIF64_CFG0                                  {\
            UNSG32 uCFG0_sft_rst                               :  1;\
            UNSG32 uCFG0_tile_size                             : 11;\
            UNSG32 uCFG0_PDWN0                                 :  1;\
            UNSG32 uCFG0_PDLVMC0                               :  1;\
            UNSG32 uCFG0_PDFVSSM0                              :  1;\
            UNSG32 uCFG0_PDWN1                                 :  1;\
            UNSG32 uCFG0_PDLVMC1                               :  1;\
            UNSG32 uCFG0_PDFVSSM1                              :  1;\
            UNSG32 uCFG0_PDWN2                                 :  1;\
            UNSG32 uCFG0_PDLVMC2                               :  1;\
            UNSG32 uCFG0_PDFVSSM2                              :  1;\
            UNSG32 RSVDx0_b21                                  : 11;\
          }
    union { UNSG32 u32DIF64_CFG0;
            struct w32DIF64_CFG0;
          };
             UNSG8 RSVDx4                                      [252];
              SIE_DIF64_LUT                                    ie_DIFLUT;
    } SIE_DIF64;
    typedef union  T32DIF64_CFG0
          { UNSG32 u32;
            struct w32DIF64_CFG0;
                 } T32DIF64_CFG0;
    typedef union  TDIF64_CFG0
          { UNSG32 u32[1];
            struct {
            struct w32DIF64_CFG0;
                   };
                 } TDIF64_CFG0;
     SIGN32 DIF64_drvrd(SIE_DIF64 *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
     SIGN32 DIF64_drvwr(SIE_DIF64 *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
       void DIF64_reset(SIE_DIF64 *p);
     SIGN32 DIF64_cmp  (SIE_DIF64 *p, SIE_DIF64 *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
    #define DIF64_check(p,pie,pfx,hLOG) DIF64_cmp(p,pie,pfx,(void*)(hLOG),0,0)
    #define DIF64_print(p,    pfx,hLOG) DIF64_cmp(p,0,  pfx,(void*)(hLOG),0,0)
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
     SIGN32 DIF128_LUT_cmp  (SIE_DIF128_LUT *p, SIE_DIF128_LUT *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
    #define DIF128_LUT_check(p,pie,pfx,hLOG) DIF128_LUT_cmp(p,pie,pfx,(void*)(hLOG),0,0)
    #define DIF128_LUT_print(p,    pfx,hLOG) DIF128_LUT_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_DIF128
#define h_DIF128 (){}
    #define     RA_DIF128_CFG0                                 0x0000
    #define     RA_DIF128_DIFLUT                               0x0200
    typedef struct SIE_DIF128 {
    #define     w32DIF128_CFG0                                 {\
            UNSG32 uCFG0_sft_rst                               :  1;\
            UNSG32 uCFG0_tile_size                             : 11;\
            UNSG32 uCFG0_PDWN0                                 :  1;\
            UNSG32 uCFG0_PDLVMC0                               :  1;\
            UNSG32 uCFG0_PDFVSSM0                              :  1;\
            UNSG32 uCFG0_PDWN1                                 :  1;\
            UNSG32 uCFG0_PDLVMC1                               :  1;\
            UNSG32 uCFG0_PDFVSSM1                              :  1;\
            UNSG32 uCFG0_PDWN2                                 :  1;\
            UNSG32 uCFG0_PDLVMC2                               :  1;\
            UNSG32 uCFG0_PDFVSSM2                              :  1;\
            UNSG32 RSVDx0_b21                                  : 11;\
          }
    union { UNSG32 u32DIF128_CFG0;
            struct w32DIF128_CFG0;
          };
             UNSG8 RSVDx4                                      [508];
              SIE_DIF128_LUT                                   ie_DIFLUT;
    } SIE_DIF128;
    typedef union  T32DIF128_CFG0
          { UNSG32 u32;
            struct w32DIF128_CFG0;
                 } T32DIF128_CFG0;
    typedef union  TDIF128_CFG0
          { UNSG32 u32[1];
            struct {
            struct w32DIF128_CFG0;
                   };
                 } TDIF128_CFG0;
     SIGN32 DIF128_drvrd(SIE_DIF128 *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
     SIGN32 DIF128_drvwr(SIE_DIF128 *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
       void DIF128_reset(SIE_DIF128 *p);
     SIGN32 DIF128_cmp  (SIE_DIF128 *p, SIE_DIF128 *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
    #define DIF128_check(p,pie,pfx,hLOG) DIF128_cmp(p,pie,pfx,(void*)(hLOG),0,0)
    #define DIF128_print(p,    pfx,hLOG) DIF128_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_LDI
#define h_LDI (){}
    #define     RA_LDI_DIF64DY                                 0x0000
    #define     RA_LDI_DIF64DC                                 0x0200
    #define     RA_LDI_DIF128CY                                0x0400
    #define     RA_LDI_DIF128CC                                0x0800
    #define     RA_LDI_RCCmDo                                  0x0C00
    #define     RA_LDI_WCCispY                                 0x0C10
    #define     RA_LDI_WCCispC                                 0x0C20
    #define     RA_LDI_CFGD0                                   0x0C30
    #define     RA_LDI_CFGD1                                   0x0C34
    #define     RA_LDI_CFGD2                                   0x0C38
    #define     RA_LDI_CFGC0                                   0x0C3C
    #define     RA_LDI_CFGC1                                   0x0C40
    #define     RA_LDI_CFGC2                                   0x0C44
    #define     RA_LDI_CFGC7                                   0x0C48
    #define     RA_LDI_CFGC3                                   0x0C4C
    #define     RA_LDI_CFGC4                                   0x0C50
    #define     RA_LDI_CFGC5                                   0x0C54
    #define     RA_LDI_CFGC6                                   0x0C58
    #define     RA_LDI_CFGCRS                                  0x0C5C
    #define     RA_LDI_CFGCRSP                                 0x0C60
    #define     RA_LDI_STATUS                                  0x0C64
    #define     RA_LDI_DHUB_IRQEN                              0x0C68
    typedef struct SIE_LDI {
              SIE_DIF64                                        ie_DIF64DY;
              SIE_DIF64                                        ie_DIF64DC;
              SIE_DIF128                                       ie_DIF128CY;
              SIE_DIF128                                       ie_DIF128CC;
              SIE_ReadClient                                   ie_RCCmDo;
              SIE_WriteClient                                  ie_WCCispY;
              SIE_WriteClient                                  ie_WCCispC;
    #define     w32LDI_CFGD0                                   {\
            UNSG32 uCFGD0_BaseAddrDY                           : 32;\
          }
    union { UNSG32 u32LDI_CFGD0;
            struct w32LDI_CFGD0;
          };
    #define     w32LDI_CFGD1                                   {\
            UNSG32 uCFGD1_BaseAddrDC                           : 32;\
          }
    union { UNSG32 u32LDI_CFGD1;
            struct w32LDI_CFGD1;
          };
    #define     w32LDI_CFGD2                                   {\
            UNSG32 uCFGD2_StrideDisp                           : 20;\
            UNSG32 RSVDxC38_b20                                : 12;\
          }
    union { UNSG32 u32LDI_CFGD2;
            struct w32LDI_CFGD2;
          };
    #define     w32LDI_CFGC0                                   {\
            UNSG32 uCFGC0_BaseAddrCY                           : 32;\
          }
    union { UNSG32 u32LDI_CFGC0;
            struct w32LDI_CFGC0;
          };
    #define     w32LDI_CFGC1                                   {\
            UNSG32 uCFGC1_BaseAddrCC                           : 32;\
          }
    union { UNSG32 u32LDI_CFGC1;
            struct w32LDI_CFGC1;
          };
    #define     w32LDI_CFGC2                                   {\
            UNSG32 uCFGC2_StrideCisp                           : 20;\
            UNSG32 RSVDxC44_b20                                : 12;\
          }
    union { UNSG32 u32LDI_CFGC2;
            struct w32LDI_CFGC2;
          };
    #define     w32LDI_CFGC7                                   {\
            UNSG32 uCFGC7_JumpStrideCisp                       : 20;\
            UNSG32 RSVDxC48_b20                                : 12;\
          }
    union { UNSG32 u32LDI_CFGC7;
            struct w32LDI_CFGC7;
          };
    #define     w32LDI_CFGC3                                   {\
            UNSG32 uCFGC3_nStridesCisp                         : 16;\
            UNSG32 RSVDxC4C_b16                                : 16;\
          }
    union { UNSG32 u32LDI_CFGC3;
            struct w32LDI_CFGC3;
          };
    #define     w32LDI_CFGC4                                   {\
            UNSG32 uCFGC4_lgdcLosMsk                           :  1;\
            UNSG32 uCFGC4_losMsk                               :  1;\
            UNSG32 RSVDxC50_b2                                 : 30;\
          }
    union { UNSG32 u32LDI_CFGC4;
            struct w32LDI_CFGC4;
          };
    #define     w32LDI_CFGC5                                   {\
            UNSG32 uCFGC5_nPixCispStride                       : 32;\
          }
    union { UNSG32 u32LDI_CFGC5;
            struct w32LDI_CFGC5;
          };
    #define     w32LDI_CFGC6                                   {\
            UNSG32 uCFGC6_nCispStrides                         : 16;\
            UNSG32 RSVDxC58_b16                                : 16;\
          }
    union { UNSG32 u32LDI_CFGC6;
            struct w32LDI_CFGC6;
          };
    #define     w32LDI_CFGCRS                                  {\
            UNSG32 uCFGCRS_CispRsEn                            :  1;\
            UNSG32 uCFGCRS_CispRsNob                           :  9;\
            UNSG32 uCFGCRS_CispRsPixLineTot                    : 20;\
            UNSG32 RSVDxC5C_b30                                :  2;\
          }
    union { UNSG32 u32LDI_CFGCRS;
            struct w32LDI_CFGCRS;
          };
    #define     w32LDI_CFGCRSP                                 {\
            UNSG32 uCFGCRSP_CispRsPackEn                       :  1;\
            UNSG32 uCFGCRSP_YCSwap                             :  1;\
            UNSG32 uCFGCRSP_UVSwap                             :  1;\
            UNSG32 RSVDxC60_b3                                 : 29;\
          }
    union { UNSG32 u32LDI_CFGCRSP;
            struct w32LDI_CFGCRSP;
          };
    #define     w32LDI_STATUS                                  {\
            UNSG32 uSTATUS_ShortStrideWin                      :  1;\
            UNSG32 uSTATUS_ShortFrameWin                       :  1;\
            UNSG32 RSVDxC64_b2                                 : 30;\
          }
    union { UNSG32 u32LDI_STATUS;
            struct w32LDI_STATUS;
          };
    #define     w32LDI_DHUB_IRQEN                              {\
            UNSG32 uDHUB_IRQEN_DIS_Y                           :  1;\
            UNSG32 uDHUB_IRQEN_DIS_C                           :  1;\
            UNSG32 uDHUB_IRQEN_CIS_Y                           :  1;\
            UNSG32 uDHUB_IRQEN_CIS_C                           :  1;\
            UNSG32 RSVDxC68_b4                                 : 28;\
          }
    union { UNSG32 u32LDI_DHUB_IRQEN;
            struct w32LDI_DHUB_IRQEN;
          };
             UNSG8 RSVDxC6C                                    [404];
    } SIE_LDI;
    typedef union  T32LDI_CFGD0
          { UNSG32 u32;
            struct w32LDI_CFGD0;
                 } T32LDI_CFGD0;
    typedef union  T32LDI_CFGD1
          { UNSG32 u32;
            struct w32LDI_CFGD1;
                 } T32LDI_CFGD1;
    typedef union  T32LDI_CFGD2
          { UNSG32 u32;
            struct w32LDI_CFGD2;
                 } T32LDI_CFGD2;
    typedef union  T32LDI_CFGC0
          { UNSG32 u32;
            struct w32LDI_CFGC0;
                 } T32LDI_CFGC0;
    typedef union  T32LDI_CFGC1
          { UNSG32 u32;
            struct w32LDI_CFGC1;
                 } T32LDI_CFGC1;
    typedef union  T32LDI_CFGC2
          { UNSG32 u32;
            struct w32LDI_CFGC2;
                 } T32LDI_CFGC2;
    typedef union  T32LDI_CFGC7
          { UNSG32 u32;
            struct w32LDI_CFGC7;
                 } T32LDI_CFGC7;
    typedef union  T32LDI_CFGC3
          { UNSG32 u32;
            struct w32LDI_CFGC3;
                 } T32LDI_CFGC3;
    typedef union  T32LDI_CFGC4
          { UNSG32 u32;
            struct w32LDI_CFGC4;
                 } T32LDI_CFGC4;
    typedef union  T32LDI_CFGC5
          { UNSG32 u32;
            struct w32LDI_CFGC5;
                 } T32LDI_CFGC5;
    typedef union  T32LDI_CFGC6
          { UNSG32 u32;
            struct w32LDI_CFGC6;
                 } T32LDI_CFGC6;
    typedef union  T32LDI_CFGCRS
          { UNSG32 u32;
            struct w32LDI_CFGCRS;
                 } T32LDI_CFGCRS;
    typedef union  T32LDI_CFGCRSP
          { UNSG32 u32;
            struct w32LDI_CFGCRSP;
                 } T32LDI_CFGCRSP;
    typedef union  T32LDI_STATUS
          { UNSG32 u32;
            struct w32LDI_STATUS;
                 } T32LDI_STATUS;
    typedef union  T32LDI_DHUB_IRQEN
          { UNSG32 u32;
            struct w32LDI_DHUB_IRQEN;
                 } T32LDI_DHUB_IRQEN;
    typedef union  TLDI_CFGD0
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGD0;
                   };
                 } TLDI_CFGD0;
    typedef union  TLDI_CFGD1
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGD1;
                   };
                 } TLDI_CFGD1;
    typedef union  TLDI_CFGD2
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGD2;
                   };
                 } TLDI_CFGD2;
    typedef union  TLDI_CFGC0
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGC0;
                   };
                 } TLDI_CFGC0;
    typedef union  TLDI_CFGC1
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGC1;
                   };
                 } TLDI_CFGC1;
    typedef union  TLDI_CFGC2
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGC2;
                   };
                 } TLDI_CFGC2;
    typedef union  TLDI_CFGC7
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGC7;
                   };
                 } TLDI_CFGC7;
    typedef union  TLDI_CFGC3
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGC3;
                   };
                 } TLDI_CFGC3;
    typedef union  TLDI_CFGC4
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGC4;
                   };
                 } TLDI_CFGC4;
    typedef union  TLDI_CFGC5
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGC5;
                   };
                 } TLDI_CFGC5;
    typedef union  TLDI_CFGC6
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGC6;
                   };
                 } TLDI_CFGC6;
    typedef union  TLDI_CFGCRS
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGCRS;
                   };
                 } TLDI_CFGCRS;
    typedef union  TLDI_CFGCRSP
          { UNSG32 u32[1];
            struct {
            struct w32LDI_CFGCRSP;
                   };
                 } TLDI_CFGCRSP;
    typedef union  TLDI_STATUS
          { UNSG32 u32[1];
            struct {
            struct w32LDI_STATUS;
                   };
                 } TLDI_STATUS;
    typedef union  TLDI_DHUB_IRQEN
          { UNSG32 u32[1];
            struct {
            struct w32LDI_DHUB_IRQEN;
                   };
                 } TLDI_DHUB_IRQEN;
     SIGN32 LDI_drvrd(SIE_LDI *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
     SIGN32 LDI_drvwr(SIE_LDI *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
       void LDI_reset(SIE_LDI *p);
     SIGN32 LDI_cmp  (SIE_LDI *p, SIE_LDI *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
    #define LDI_check(p,pie,pfx,hLOG) LDI_cmp(p,pie,pfx,(void*)(hLOG),0,0)
    #define LDI_print(p,    pfx,hLOG) LDI_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_DWTG
#define h_DWTG (){}
    #define     RA_DWTG_INIT                                   0x0000
    #define     RA_DWTG_SIZE                                   0x0004
    #define     RA_DWTG_HS                                     0x0008
    #define     RA_DWTG_HB                                     0x000C
    #define     RA_DWTG_HB_CR                                  0x0010
    #define     RA_DWTG_HB_CR2                                 0x0014
    #define     RA_DWTG_VS0                                    0x0018
    #define     RA_DWTG_VS1                                    0x001C
    #define     RA_DWTG_VB0                                    0x0020
    #define     RA_DWTG_VB0_CR                                 0x0024
    #define     RA_DWTG_VB0_CR2                                0x0028
    #define     RA_DWTG_VB1                                    0x002C
    #define     RA_DWTG_SCAN                                   0x0030
    #define        DWTG_SCAN_MODE_PROG                                      0x0
    #define        DWTG_SCAN_MODE_INTER                                     0x1
    #define     RA_DWTG_INTPOS                                 0x0034
    #define     RA_DWTG_MODE                                   0x0038
    #define        DWTG_MODE_EN_MASTER                                      0x0
    #define        DWTG_MODE_EN_SLAVE                                       0x1
    #define     RA_DWTG_HVREF                                  0x003C
    #define        DWTG_HVREF_SEL_SYNC                                      0x0
    #define        DWTG_HVREF_SEL_BLANK                                     0x1
    #define        DWTG_HVREF_POL_NEG_PULSE                                 0x0
    #define        DWTG_HVREF_POL_POS_PULSE                                 0x1
    #define     RA_DWTG_STRPOS                                 0x0040
    #define     RA_DWTG_STRVS                                  0x0044
    #define     RA_DWTG_FRSTPOS                                0x0048
    #define     RA_DWTG_INTRPOS                                0x004C
    typedef struct SIE_DWTG {
    #define     w32DWTG_INIT                                   {\
            UNSG32 uINIT_Y                                     : 16;\
            UNSG32 uINIT_X                                     : 16;\
          }
    union { UNSG32 u32DWTG_INIT;
            struct w32DWTG_INIT;
          };
    #define     w32DWTG_SIZE                                   {\
            UNSG32 uSIZE_Y                                     : 16;\
            UNSG32 uSIZE_X                                     : 16;\
          }
    union { UNSG32 u32DWTG_SIZE;
            struct w32DWTG_SIZE;
          };
    #define     w32DWTG_HS                                     {\
            UNSG32 uHS_FE                                      : 16;\
            UNSG32 uHS_BE                                      : 16;\
          }
    union { UNSG32 u32DWTG_HS;
            struct w32DWTG_HS;
          };
    #define     w32DWTG_HB                                     {\
            UNSG32 uHB_FE                                      : 16;\
            UNSG32 uHB_BE                                      : 16;\
          }
    union { UNSG32 u32DWTG_HB;
            struct w32DWTG_HB;
          };
    #define     w32DWTG_HB_CR                                  {\
            UNSG32 uHB_CR_FE                                   : 16;\
            UNSG32 uHB_CR_BE                                   : 16;\
          }
    union { UNSG32 u32DWTG_HB_CR;
            struct w32DWTG_HB_CR;
          };
    #define     w32DWTG_HB_CR2                                 {\
            UNSG32 uHB_CR2_FE                                  : 16;\
            UNSG32 uHB_CR2_BE                                  : 16;\
          }
    union { UNSG32 u32DWTG_HB_CR2;
            struct w32DWTG_HB_CR2;
          };
    #define     w32DWTG_VS0                                    {\
            UNSG32 uVS0_FE                                     : 16;\
            UNSG32 uVS0_BE                                     : 16;\
          }
    union { UNSG32 u32DWTG_VS0;
            struct w32DWTG_VS0;
          };
    #define     w32DWTG_VS1                                    {\
            UNSG32 uVS1_FE                                     : 16;\
            UNSG32 uVS1_BE                                     : 16;\
          }
    union { UNSG32 u32DWTG_VS1;
            struct w32DWTG_VS1;
          };
    #define     w32DWTG_VB0                                    {\
            UNSG32 uVB0_FE                                     : 16;\
            UNSG32 uVB0_BE                                     : 16;\
          }
    union { UNSG32 u32DWTG_VB0;
            struct w32DWTG_VB0;
          };
    #define     w32DWTG_VB0_CR                                 {\
            UNSG32 uVB0_CR_FE                                  : 16;\
            UNSG32 uVB0_CR_BE                                  : 16;\
          }
    union { UNSG32 u32DWTG_VB0_CR;
            struct w32DWTG_VB0_CR;
          };
    #define     w32DWTG_VB0_CR2                                {\
            UNSG32 uVB0_CR2_FE                                 : 16;\
            UNSG32 uVB0_CR2_BE                                 : 16;\
          }
    union { UNSG32 u32DWTG_VB0_CR2;
            struct w32DWTG_VB0_CR2;
          };
    #define     w32DWTG_VB1                                    {\
            UNSG32 uVB1_FE                                     : 16;\
            UNSG32 uVB1_BE                                     : 16;\
          }
    union { UNSG32 u32DWTG_VB1;
            struct w32DWTG_VB1;
          };
    #define     w32DWTG_SCAN                                   {\
            UNSG32 uSCAN_MODE                                  :  1;\
            UNSG32 RSVDx30_b1                                  : 31;\
          }
    union { UNSG32 u32DWTG_SCAN;
            struct w32DWTG_SCAN;
          };
    #define     w32DWTG_INTPOS                                 {\
            UNSG32 uINTPOS_FRAME                               : 16;\
            UNSG32 uINTPOS_FIELD                               : 16;\
          }
    union { UNSG32 u32DWTG_INTPOS;
            struct w32DWTG_INTPOS;
          };
    #define     w32DWTG_MODE                                   {\
            UNSG32 uMODE_EN                                    :  1;\
            UNSG32 RSVDx38_b1                                  : 31;\
          }
    union { UNSG32 u32DWTG_MODE;
            struct w32DWTG_MODE;
          };
    #define     w32DWTG_HVREF                                  {\
            UNSG32 uHVREF_SEL                                  :  1;\
            UNSG32 uHVREF_POL                                  :  1;\
            UNSG32 RSVDx3C_b2                                  : 30;\
          }
    union { UNSG32 u32DWTG_HVREF;
            struct w32DWTG_HVREF;
          };
    #define     w32DWTG_STRPOS                                 {\
            UNSG32 uSTRPOS_StrdHstart                          : 16;\
            UNSG32 uSTRPOS_StrdVstart                          : 16;\
          }
    union { UNSG32 u32DWTG_STRPOS;
            struct w32DWTG_STRPOS;
          };
    #define     w32DWTG_STRVS                                  {\
            UNSG32 uSTRVS_StrdVend                             : 16;\
            UNSG32 uSTRVS_StrdVsize                            :  8;\
            UNSG32 RSVDx44_b24                                 :  8;\
          }
    union { UNSG32 u32DWTG_STRVS;
            struct w32DWTG_STRVS;
          };
    #define     w32DWTG_FRSTPOS                                {\
            UNSG32 uFRSTPOS_FrstHstart                         : 16;\
            UNSG32 uFRSTPOS_FrstVstart                         : 16;\
          }
    union { UNSG32 u32DWTG_FRSTPOS;
            struct w32DWTG_FRSTPOS;
          };
    #define     w32DWTG_INTRPOS                                {\
            UNSG32 uINTRPOS_IntrHstart                         : 16;\
            UNSG32 uINTRPOS_IntrVstart                         : 16;\
          }
    union { UNSG32 u32DWTG_INTRPOS;
            struct w32DWTG_INTRPOS;
          };
    } SIE_DWTG;
    typedef union  T32DWTG_INIT
          { UNSG32 u32;
            struct w32DWTG_INIT;
                 } T32DWTG_INIT;
    typedef union  T32DWTG_SIZE
          { UNSG32 u32;
            struct w32DWTG_SIZE;
                 } T32DWTG_SIZE;
    typedef union  T32DWTG_HS
          { UNSG32 u32;
            struct w32DWTG_HS;
                 } T32DWTG_HS;
    typedef union  T32DWTG_HB
          { UNSG32 u32;
            struct w32DWTG_HB;
                 } T32DWTG_HB;
    typedef union  T32DWTG_HB_CR
          { UNSG32 u32;
            struct w32DWTG_HB_CR;
                 } T32DWTG_HB_CR;
    typedef union  T32DWTG_HB_CR2
          { UNSG32 u32;
            struct w32DWTG_HB_CR2;
                 } T32DWTG_HB_CR2;
    typedef union  T32DWTG_VS0
          { UNSG32 u32;
            struct w32DWTG_VS0;
                 } T32DWTG_VS0;
    typedef union  T32DWTG_VS1
          { UNSG32 u32;
            struct w32DWTG_VS1;
                 } T32DWTG_VS1;
    typedef union  T32DWTG_VB0
          { UNSG32 u32;
            struct w32DWTG_VB0;
                 } T32DWTG_VB0;
    typedef union  T32DWTG_VB0_CR
          { UNSG32 u32;
            struct w32DWTG_VB0_CR;
                 } T32DWTG_VB0_CR;
    typedef union  T32DWTG_VB0_CR2
          { UNSG32 u32;
            struct w32DWTG_VB0_CR2;
                 } T32DWTG_VB0_CR2;
    typedef union  T32DWTG_VB1
          { UNSG32 u32;
            struct w32DWTG_VB1;
                 } T32DWTG_VB1;
    typedef union  T32DWTG_SCAN
          { UNSG32 u32;
            struct w32DWTG_SCAN;
                 } T32DWTG_SCAN;
    typedef union  T32DWTG_INTPOS
          { UNSG32 u32;
            struct w32DWTG_INTPOS;
                 } T32DWTG_INTPOS;
    typedef union  T32DWTG_MODE
          { UNSG32 u32;
            struct w32DWTG_MODE;
                 } T32DWTG_MODE;
    typedef union  T32DWTG_HVREF
          { UNSG32 u32;
            struct w32DWTG_HVREF;
                 } T32DWTG_HVREF;
    typedef union  T32DWTG_STRPOS
          { UNSG32 u32;
            struct w32DWTG_STRPOS;
                 } T32DWTG_STRPOS;
    typedef union  T32DWTG_STRVS
          { UNSG32 u32;
            struct w32DWTG_STRVS;
                 } T32DWTG_STRVS;
    typedef union  T32DWTG_FRSTPOS
          { UNSG32 u32;
            struct w32DWTG_FRSTPOS;
                 } T32DWTG_FRSTPOS;
    typedef union  T32DWTG_INTRPOS
          { UNSG32 u32;
            struct w32DWTG_INTRPOS;
                 } T32DWTG_INTRPOS;
    typedef union  TDWTG_INIT
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_INIT;
                   };
                 } TDWTG_INIT;
    typedef union  TDWTG_SIZE
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_SIZE;
                   };
                 } TDWTG_SIZE;
    typedef union  TDWTG_HS
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_HS;
                   };
                 } TDWTG_HS;
    typedef union  TDWTG_HB
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_HB;
                   };
                 } TDWTG_HB;
    typedef union  TDWTG_HB_CR
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_HB_CR;
                   };
                 } TDWTG_HB_CR;
    typedef union  TDWTG_HB_CR2
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_HB_CR2;
                   };
                 } TDWTG_HB_CR2;
    typedef union  TDWTG_VS0
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_VS0;
                   };
                 } TDWTG_VS0;
    typedef union  TDWTG_VS1
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_VS1;
                   };
                 } TDWTG_VS1;
    typedef union  TDWTG_VB0
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_VB0;
                   };
                 } TDWTG_VB0;
    typedef union  TDWTG_VB0_CR
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_VB0_CR;
                   };
                 } TDWTG_VB0_CR;
    typedef union  TDWTG_VB0_CR2
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_VB0_CR2;
                   };
                 } TDWTG_VB0_CR2;
    typedef union  TDWTG_VB1
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_VB1;
                   };
                 } TDWTG_VB1;
    typedef union  TDWTG_SCAN
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_SCAN;
                   };
                 } TDWTG_SCAN;
    typedef union  TDWTG_INTPOS
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_INTPOS;
                   };
                 } TDWTG_INTPOS;
    typedef union  TDWTG_MODE
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_MODE;
                   };
                 } TDWTG_MODE;
    typedef union  TDWTG_HVREF
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_HVREF;
                   };
                 } TDWTG_HVREF;
    typedef union  TDWTG_STRPOS
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_STRPOS;
                   };
                 } TDWTG_STRPOS;
    typedef union  TDWTG_STRVS
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_STRVS;
                   };
                 } TDWTG_STRVS;
    typedef union  TDWTG_FRSTPOS
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_FRSTPOS;
                   };
                 } TDWTG_FRSTPOS;
    typedef union  TDWTG_INTRPOS
          { UNSG32 u32[1];
            struct {
            struct w32DWTG_INTRPOS;
                   };
                 } TDWTG_INTRPOS;
     SIGN32 DWTG_drvrd(SIE_DWTG *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
     SIGN32 DWTG_drvwr(SIE_DWTG *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
       void DWTG_reset(SIE_DWTG *p);
     SIGN32 DWTG_cmp  (SIE_DWTG *p, SIE_DWTG *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
    #define DWTG_check(p,pie,pfx,hLOG) DWTG_cmp(p,pie,pfx,(void*)(hLOG),0,0)
    #define DWTG_print(p,    pfx,hLOG) DWTG_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_LGDC
#define h_LGDC (){}
    #define     RA_LGDC_CFG0                                   0x0000
    #define     RA_LGDC_CFG1                                   0x0004
    #define     RA_LGDC_CFG2                                   0x0008
    #define     RA_LGDC_CFG3                                   0x000C
    #define     RA_LGDC_CFG4                                   0x0010
    #define     RA_LGDC_CFG5                                   0x0014
    #define     RA_LGDC_CFG6                                   0x0018
    #define     RA_LGDC_CFG7                                   0x001C
    #define     RA_LGDC_CFG8                                   0x0020
    #define     RA_LGDC_CFG9                                   0x0024
    #define     RA_LGDC_CFTG                                   0x0028
    #define     RA_LGDC_TG                                     0x002C
    #define     RA_LGDC_DBG0                                   0x007C
    #define     RA_LGDC_DBG1                                   0x0080
    #define     RA_LGDC_DBG2                                   0x0084
    #define     RA_LGDC_DBG3                                   0x0088
    typedef struct SIE_LGDC {
    #define     w32LGDC_CFG0                                   {\
            UNSG32 uCFG0_CisHwin                               :  8;\
            UNSG32 uCFG0_CisVwin                               :  8;\
            UNSG32 uCFG0_DisHwin                               :  8;\
            UNSG32 uCFG0_DisVwin                               :  8;\
          }
    union { UNSG32 u32LGDC_CFG0;
            struct w32LGDC_CFG0;
          };
    #define     w32LGDC_CFG1                                   {\
            UNSG32 uCFG1_DisHgrid                              :  5;\
            UNSG32 uCFG1_DisVgrid                              :  5;\
            UNSG32 uCFG1_TileHwin                              :  5;\
            UNSG32 uCFG1_TileVwin                              :  5;\
            UNSG32 RSVDx4_b20                                  : 12;\
          }
    union { UNSG32 u32LGDC_CFG1;
            struct w32LGDC_CFG1;
          };
    #define     w32LGDC_CFG2                                   {\
            UNSG32 uCFG2_CisMatSize                            : 10;\
            UNSG32 uCFG2_CisBlkHtotal                          : 10;\
            UNSG32 uCFG2_CisBlkVtotal                          : 10;\
            UNSG32 RSVDx8_b30                                  :  2;\
          }
    union { UNSG32 u32LGDC_CFG2;
            struct w32LGDC_CFG2;
          };
    #define     w32LGDC_CFG3                                   {\
            UNSG32 uCFG3_Hdimension                            : 12;\
            UNSG32 uCFG3_Vdimension                            : 12;\
            UNSG32 RSVDxC_b24                                  :  8;\
          }
    union { UNSG32 u32LGDC_CFG3;
            struct w32LGDC_CFG3;
          };
    #define     w32LGDC_CFG4                                   {\
            UNSG32 uCFG4_HRES                                  : 12;\
            UNSG32 uCFG4_VRES                                  : 12;\
            UNSG32 RSVDx10_b24                                 :  8;\
          }
    union { UNSG32 u32LGDC_CFG4;
            struct w32LGDC_CFG4;
          };
    #define     w32LGDC_CFG5                                   {\
            UNSG32 uCFG5_CisBsize                              :  9;\
            UNSG32 uCFG5_MaxDisBlocks                          : 10;\
            UNSG32 uCFG5_YUVmode                               :  2;\
            UNSG32 uCFG5_DisV4H2scan                           :  1;\
            UNSG32 uCFG5_LimitCbuff                            :  1;\
            UNSG32 uCFG5_BicubicOff                            :  2;\
            UNSG32 RSVDx14_b25                                 :  7;\
          }
    union { UNSG32 u32LGDC_CFG5;
            struct w32LGDC_CFG5;
          };
    #define     w32LGDC_CFG6                                   {\
            UNSG32 uCFG6_DisLumaTiles                          :  7;\
            UNSG32 uCFG6_DisChromaTiles                        :  7;\
            UNSG32 RSVDx18_b14                                 : 18;\
          }
    union { UNSG32 u32LGDC_CFG6;
            struct w32LGDC_CFG6;
          };
    #define     w32LGDC_CFG7                                   {\
            UNSG32 uCFG7_DisBlkSize                            : 13;\
            UNSG32 uCFG7_DisBlkChromaSize                      : 13;\
            UNSG32 RSVDx1C_b26                                 :  6;\
          }
    union { UNSG32 u32LGDC_CFG7;
            struct w32LGDC_CFG7;
          };
    #define     w32LGDC_CFG8                                   {\
            UNSG32 uCFG8_BlankLuma                             : 10;\
            UNSG32 uCFG8_BlankChroma                           : 10;\
            UNSG32 uCFG8_Source8bits                           :  1;\
            UNSG32 RSVDx20_b21                                 : 11;\
          }
    union { UNSG32 u32LGDC_CFG8;
            struct w32LGDC_CFG8;
          };
    #define     w32LGDC_CFG9                                   {\
            UNSG32 uCFG9_IHRES                                 : 12;\
            UNSG32 uCFG9_IVRES                                 : 12;\
            UNSG32 RSVDx24_b24                                 :  8;\
          }
    union { UNSG32 u32LGDC_CFG9;
            struct w32LGDC_CFG9;
          };
    #define     w32LGDC_CFTG                                   {\
            UNSG32 uCFTG_start                                 :  1;\
            UNSG32 uCFTG_clear                                 :  1;\
            UNSG32 uCFTG_clken_ctrl                            :  1;\
            UNSG32 uCFTG_clken_stall                           :  2;\
            UNSG32 uCFTG_exctrl                                : 27;\
          }
    union { UNSG32 u32LGDC_CFTG;
            struct w32LGDC_CFTG;
          };
              SIE_DWTG                                         ie_TG;
    #define     w32LGDC_DBG0                                   {\
            UNSG32 uDBG0_FLAG                                  :  4;\
            UNSG32 uDBG0_TGERR                                 :  4;\
            UNSG32 RSVDx7C_b8                                  : 24;\
          }
    union { UNSG32 u32LGDC_DBG0;
            struct w32LGDC_DBG0;
          };
    #define     w32LGDC_DBG1                                   {\
            UNSG32 uDBG1_FrameStrides                          :  9;\
            UNSG32 uDBG1_MinCisBlks                            :  9;\
            UNSG32 uDBG1_MaxCisBlks                            :  9;\
            UNSG32 RSVDx80_b27                                 :  5;\
          }
    union { UNSG32 u32LGDC_DBG1;
            struct w32LGDC_DBG1;
          };
    #define     w32LGDC_DBG2                                   {\
            UNSG32 uDBG2_FrameCisBlks                          : 16;\
            UNSG32 uDBG2_FrameDisBlks                          : 16;\
          }
    union { UNSG32 u32LGDC_DBG2;
            struct w32LGDC_DBG2;
          };
    #define     w32LGDC_DBG3                                   {\
            UNSG32 uDBG3_MinDisBlks                            :  9;\
            UNSG32 uDBG3_MaxDisBlks                            :  9;\
            UNSG32 uDBG3_OthDbg                                : 14;\
          }
    union { UNSG32 u32LGDC_DBG3;
            struct w32LGDC_DBG3;
          };
    } SIE_LGDC;
    typedef union  T32LGDC_CFG0
          { UNSG32 u32;
            struct w32LGDC_CFG0;
                 } T32LGDC_CFG0;
    typedef union  T32LGDC_CFG1
          { UNSG32 u32;
            struct w32LGDC_CFG1;
                 } T32LGDC_CFG1;
    typedef union  T32LGDC_CFG2
          { UNSG32 u32;
            struct w32LGDC_CFG2;
                 } T32LGDC_CFG2;
    typedef union  T32LGDC_CFG3
          { UNSG32 u32;
            struct w32LGDC_CFG3;
                 } T32LGDC_CFG3;
    typedef union  T32LGDC_CFG4
          { UNSG32 u32;
            struct w32LGDC_CFG4;
                 } T32LGDC_CFG4;
    typedef union  T32LGDC_CFG5
          { UNSG32 u32;
            struct w32LGDC_CFG5;
                 } T32LGDC_CFG5;
    typedef union  T32LGDC_CFG6
          { UNSG32 u32;
            struct w32LGDC_CFG6;
                 } T32LGDC_CFG6;
    typedef union  T32LGDC_CFG7
          { UNSG32 u32;
            struct w32LGDC_CFG7;
                 } T32LGDC_CFG7;
    typedef union  T32LGDC_CFG8
          { UNSG32 u32;
            struct w32LGDC_CFG8;
                 } T32LGDC_CFG8;
    typedef union  T32LGDC_CFG9
          { UNSG32 u32;
            struct w32LGDC_CFG9;
                 } T32LGDC_CFG9;
    typedef union  T32LGDC_CFTG
          { UNSG32 u32;
            struct w32LGDC_CFTG;
                 } T32LGDC_CFTG;
    typedef union  T32LGDC_DBG0
          { UNSG32 u32;
            struct w32LGDC_DBG0;
                 } T32LGDC_DBG0;
    typedef union  T32LGDC_DBG1
          { UNSG32 u32;
            struct w32LGDC_DBG1;
                 } T32LGDC_DBG1;
    typedef union  T32LGDC_DBG2
          { UNSG32 u32;
            struct w32LGDC_DBG2;
                 } T32LGDC_DBG2;
    typedef union  T32LGDC_DBG3
          { UNSG32 u32;
            struct w32LGDC_DBG3;
                 } T32LGDC_DBG3;
    typedef union  TLGDC_CFG0
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_CFG0;
                   };
                 } TLGDC_CFG0;
    typedef union  TLGDC_CFG1
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_CFG1;
                   };
                 } TLGDC_CFG1;
    typedef union  TLGDC_CFG2
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_CFG2;
                   };
                 } TLGDC_CFG2;
    typedef union  TLGDC_CFG3
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_CFG3;
                   };
                 } TLGDC_CFG3;
    typedef union  TLGDC_CFG4
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_CFG4;
                   };
                 } TLGDC_CFG4;
    typedef union  TLGDC_CFG5
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_CFG5;
                   };
                 } TLGDC_CFG5;
    typedef union  TLGDC_CFG6
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_CFG6;
                   };
                 } TLGDC_CFG6;
    typedef union  TLGDC_CFG7
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_CFG7;
                   };
                 } TLGDC_CFG7;
    typedef union  TLGDC_CFG8
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_CFG8;
                   };
                 } TLGDC_CFG8;
    typedef union  TLGDC_CFG9
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_CFG9;
                   };
                 } TLGDC_CFG9;
    typedef union  TLGDC_CFTG
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_CFTG;
                   };
                 } TLGDC_CFTG;
    typedef union  TLGDC_DBG0
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_DBG0;
                   };
                 } TLGDC_DBG0;
    typedef union  TLGDC_DBG1
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_DBG1;
                   };
                 } TLGDC_DBG1;
    typedef union  TLGDC_DBG2
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_DBG2;
                   };
                 } TLGDC_DBG2;
    typedef union  TLGDC_DBG3
          { UNSG32 u32[1];
            struct {
            struct w32LGDC_DBG3;
                   };
                 } TLGDC_DBG3;
     SIGN32 LGDC_drvrd(SIE_LGDC *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
     SIGN32 LGDC_drvwr(SIE_LGDC *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
       void LGDC_reset(SIE_LGDC *p);
     SIGN32 LGDC_cmp  (SIE_LGDC *p, SIE_LGDC *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
    #define LGDC_check(p,pie,pfx,hLOG) LGDC_cmp(p,pie,pfx,(void*)(hLOG),0,0)
    #define LGDC_print(p,    pfx,hLOG) LGDC_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifndef h_DEWARP
#define h_DEWARP (){}
    #define     RA_DEWARP_LDI                                  0x0000
    #define     RA_DEWARP_LGDC                                 0x0E00
    typedef struct SIE_DEWARP {
              SIE_LDI                                          ie_LDI;
              SIE_LGDC                                         ie_LGDC;
             UNSG8 RSVDxE8C                                    [372];
    } SIE_DEWARP;
     SIGN32 DEWARP_drvrd(SIE_DEWARP *p, UNSG32 base, SIGN32 mem, SIGN32 tst);
     SIGN32 DEWARP_drvwr(SIE_DEWARP *p, UNSG32 base, SIGN32 mem, SIGN32 tst, UNSG32 *pcmd);
       void DEWARP_reset(SIE_DEWARP *p);
     SIGN32 DEWARP_cmp  (SIE_DEWARP *p, SIE_DEWARP *pie, char *pfx, void *hLOG, SIGN32 mem, SIGN32 tst);
    #define DEWARP_check(p,pie,pfx,hLOG) DEWARP_cmp(p,pie,pfx,(void*)(hLOG),0,0)
    #define DEWARP_print(p,    pfx,hLOG) DEWARP_cmp(p,0,  pfx,(void*)(hLOG),0,0)
#endif
#ifdef __cplusplus
  }
#endif
#pragma  pack()
#endif
