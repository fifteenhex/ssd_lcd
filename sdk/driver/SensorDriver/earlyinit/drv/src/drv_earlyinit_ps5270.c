/* SigmaStar trade secret */
/* Copyright (c) [2019~2020] SigmaStar Technology.
All rights reserved.

Unless otherwise stipulated in writing, any and all information contained
herein regardless in any format shall remain the sole proprietary of
SigmaStar and be kept in strict confidence
(SigmaStar Confidential Information) by the recipient.
Any unauthorized act including without limitation unauthorized disclosure,
copying, use, reproduction, sale, distribution, modification, disassembling,
reverse engineering and compiling of the contents of SigmaStar Confidential
Information is unlawful and strictly prohibited. SigmaStar hereby reserves the
rights to any and all damages, losses, costs and expenses resulting therefrom.
*/

#include <drv_sensor_earlyinit_common.h>
#include <drv_sensor_earlyinit_method_1.h>
#include "PS5270_MIPI_init_table.h"         /* Sensor initial table */

/* Sensor EarlyInit implementation */
SENSOR_EARLYINIY_ENTRY_IMPL_BEGIN(PS5270)

typedef struct {
    unsigned int total_gain;
    unsigned short reg_val;
} Gain_ARRAY;

static EarlyInitSensorInfo_t _gPS5270Info =
{
    .eBayerID       = E_EARLYINIT_SNR_BAYER_BG,
    .ePixelDepth    = EARLYINIT_DATAPRECISION_14,
    .u32FpsX1000    = 25000,
    .u32Width       = 1536,
    .u32Height      = 1536,
    .u32GainX1024   = 1024,
    .u32ShutterUs   = 8000,
    .u32ShutterShortUs = 0,
    .u32GainShortX1024 = 0,
    .u8ResId           = 0,
    .u8HdrMode         = 0,
};

static Gain_ARRAY gain_table[]={
    {10000 , 0   },
    {10220 , 1   },
    {10444 , 2   },
    {10672 , 3   },
    {10905 , 4   },
    {11143 , 5   },
    {11387 , 6   },
    {11636 , 7   },
    {11893 , 8   },
    {12151 , 9   },
    {12420 , 10  },
    {12689 , 11  },
    {12970 , 12  },
    {13251 , 13  },
    {13540 , 14  },
    {13838 , 15  },
    {14144 , 16  },
    {14453 , 17  },
    {14766 , 18  },
    {15092 , 19  },
    {15422 , 20  },
    {15760 , 21  },
    {16107 , 22  },
    {16456 , 23  },
    {16821 , 24  },
    {17188 , 25  },
    {17564 , 26  },
    {17949 , 27  },
    {18343 , 28  },
    {18737 , 29  },
    {19149 , 30  },
    {19570 , 31  },
    {20000 , 32  },
    {20439 , 33  },
    {20887 , 34  },
    {21344 , 35  },
    {21810 , 36  },
    {22285 , 37  },
    {22781 , 38  },
    {23273 , 39  },
    {23786 , 40  },
    {24309 , 41  },
    {24839 , 42  },
    {25378 , 43  },
    {25940 , 44  },
    {26511 , 45  },
    {27090 , 46  },
    {27676 , 47  },
    {28287 , 48  },
    {28906 , 49  },
    {29531 , 50  },
    {30184 , 51  },
    {30843 , 52  },
    {31508 , 53  },
    {32201 , 54  },
    {32926 , 55  },
    {33629 , 56  },
    {34362 , 57  },
    {35129 , 58  },
    {35898 , 59  },
    {36670 , 60  },
    {37475 , 61  },
    {38316 , 62  },
    {39159 , 63  },
    {40000 , 64  },
    {40878 , 65  },
    {41775 , 66  },
    {42689 , 67  },
    {43621 , 68  },
    {44570 , 69  },
    {45549 , 70  },
    {46545 , 71  },
    {47573 , 72  },
    {48603 , 73  },
    {49679 , 74  },
    {50756 , 75  },
    {51881 , 76  },
    {53005 , 77  },
    {54162 , 78  },
    {55351 , 79  },
    {56575 , 80  },
    {57812 , 81  },
    {59063 , 82  },
    {60368 , 83  },
    {61687 , 84  },
    {63040 , 85  },
    {64428 , 86  },
    {65826 , 87  },
    {67285 , 88  },
    {68754 , 89  },
    {70257 , 90  },
    {71797 , 91  },
    {73372 , 92  },
    {74950 , 93  },
    {76597 , 94  },
    {78280 , 95  },
    {80000 , 96  },
    {81756 , 97  },
    {83549 , 98  },
    {85378 , 99  },
    {87242 , 100 },
    {89140 , 101 },
    {91123 , 102 },
    {93091 , 103 },
    {95145 , 104 },
    {97234 , 105 },
    {99357 , 106 },
    {101512  ,107},
    {103762  ,108},
    {106045  ,109},
    {108360  ,110},
    {110703  ,111},
    {113149  ,112},
    {115625  ,113},
    {118125  ,114},
    {120737  ,115},
    {123373  ,116},
    {126031  ,117},
    {128805  ,118},
    {131704  ,119},
    {134516  ,120},
    {137450  ,121},
    {140515  ,122},
    {143593  ,123},
    {146679  ,124},
    {149899  ,125},
    {153265  ,126},
    {156635  ,127},
    {160000  ,128},
    {163513  ,129},
    {167013  ,130},
    {170667  ,131},
    {174483  ,132},
    {178281  ,133},
    {182247  ,134},
    {186182  ,135},
    {190290  ,136},
    {194353  ,137},
    {198594  ,138},
    {203024  ,139},
    {207392  ,140},
    {211953  ,141},
    {216720  ,142},
    {221405  ,143},
    {226298  ,144},
    {231086  ,145},
    {236421  ,146},
    {241296  ,147},
    {246747  ,148},
    {252062  ,149},
    {257610  ,150},
    {263408  ,151},
    {269031  ,152},
    {274899  ,153},
    {281029  ,154},
    {286935  ,155},
    {293620  ,156},
    {300073  ,157},
    {306243  ,158},
    {313270  ,159},
    {320000  ,160},
    {327026  ,161},
    {334367  ,162},
    {341333  ,163},
    {348596  ,164},
    {359298  ,165},
    {364089  ,166},
    {372364  ,167},
    {380139  ,168},
    {389169  ,169},
    {397670  ,170},
    {406551  ,171},
    {414785  ,172},
    {424456  ,173},
    {433439  ,174},
    {442811  ,175},
    {452597  ,176},
    {462825  ,177},
    {472161  ,178},
    {483304  ,179},
    {493494  ,180},
    {504123  ,181},
    {515220  ,182},
    {526817  ,183},
    {538947  ,184},
    {549799  ,185},
    {561096  ,186},
    {574877  ,187},
    {587240  ,188},
    {600147  ,189},
    {613633  ,190},
    {625344  ,191},
    {640000  ,192},
    {652749  ,193},
    {668735  ,194},
    {682667  ,195},
    {697191  ,196},
    {712348  ,197},
    {728178  ,198},
    {744727  ,199},
    {762047  ,200},
    {776493  ,201},
    {795340  ,202},
    {811089  ,203},
    {831675  ,204},
    {848912  ,205},
    {866878  ,206},
    {885622  ,207},
    {905193  ,208},
    {925650  ,209},
    {947052  ,210},
    {963765  ,211},
    {986988  ,212},
    {1011358 ,213},
    {1030440 ,214},
    {1050256 ,215},
    {1077895 ,216},
    {1099597 ,217},
    {1122192 ,218},
    {1145734 ,219},
    {1170286 ,220},
    {1195912 ,221},
    {1222687 ,222},
    {1250687 ,223},
    {1280000 ,224},
};

#define BANK1_0X0E (0x01)
#define BANK1_0X0F (0xF4)
#define BANK1_0X5F (0x2C)
#define BANK1_0X60 (0xC2)

#define PREVIEW_LINE_PERIOD 24535 //Line per frame = Lpf+1 , line period = (1/30)/1125
#define VTS_25FPS  1630
#define SENSOR_MAXGAIN      128

#define NE_PATCH_LB     (30)
#define NE_PATCH_UB     (300)
#define NE_PATCH_SCALE	(40)

/** @brief Convert shutter to sensor register setting
@param[in] nShutterUs target shutter in us
@param[in] nFps x  target shutter in us
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/
static unsigned int PS5270EarlyInitShutterAndFps( unsigned int nFpsX1000, unsigned int nShutterUs, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
    /*ToDO: Parsing shutter to sensor i2c setting*/
    unsigned char n;
    unsigned int lines = 0;
    unsigned int vts = 0;
    unsigned int ny, /*ne, ne_patch, */line_fix;
    //unsigned int sns_const1;
    int nNumRegs = 0;
    I2cCfg_t expo_reg[] = {
        //{0xEF, 0x01},
        {0x0C, 0x00},
        {0x0D, 0x02},
        //{0x0E, BANK1_0X0E},	//[3] Cmd_OffNe1[11:8]
        //{0x0F, BANK1_0X0F},	//[4] Cmd_OffNe1[7:0]
        //{0x5F, BANK1_0X5F},	//[5]
        //{0x60, BANK1_0X60},	//[6]
        {0x0A, 0x06},
        {0x0B, 0x5D},
    };

    //sns_const1 = ( (expo_reg[3].u16Data<<8)|expo_reg[4].u16Data ) + ( (expo_reg[5].u16Data<<8)|expo_reg[6].u16Data );
    //sns_const1 = ((BANK1_0X0E<<8)|BANK1_0X0F) + ((BANK1_0X5F<<8)|BANK1_0X60);


    lines=(1000*nShutterUs)/PREVIEW_LINE_PERIOD;

    if(nFpsX1000<1000) {    //for old method

        if (nFpsX1000 > 25)
            nFpsX1000 = 25;

        vts = (VTS_25FPS*25)/nFpsX1000;
    } else {    //new method, fps is 1000 based
        if (nFpsX1000 > 25*1000)
            nFpsX1000 = 25*1000;
        vts = (VTS_25FPS*25*1000)/nFpsX1000;
    }

    if (lines > vts-2) {
        vts = lines +3;
    }

    /*patch 05/15/2019 james.yang primesensor*/
    line_fix = ((vts - 4) > lines)?((3 < lines)?lines:3):(vts - 4);	// 4 <= Ny <= LPF-3
    ny = vts - line_fix;

    // ne_patch update.
#if 0
    ne_patch = NE_PATCH_LB + ((ny*NE_PATCH_SCALE)>>8);
    if (ne_patch > NE_PATCH_UB)
        ne_patch = NE_PATCH_UB;

    ne = sns_const1 - ne_patch;
#endif
    expo_reg[0].u16Data = (ny>>8) & 0x00ff;
    expo_reg[1].u16Data = (ny>>0) & 0x00ff;
    //expo_reg[2].u16Data = (ne>>8) & 0x00ff;
    //expo_reg[3].u16Data = (ne>>0) & 0x00ff;
    //expo_reg[4].u16Data = (ne_patch>>8) & 0x00ff;
    //expo_reg[5].u16Data = (ne_patch>>0) & 0x00ff;
    expo_reg[2].u16Data = (vts >> 8) & 0x00ff;
    expo_reg[3].u16Data = (vts >> 0) & 0x00ff;

    /*copy result*/
    //memcpy(pRegs,expo_reg,sizeof(expo_reg));
    for(n=0;n<sizeof(expo_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)expo_reg)[n];
    }
    nNumRegs = sizeof(expo_reg)/sizeof(expo_reg[0]);

    _gPS5270Info.u32ShutterUs = nShutterUs;
    _gPS5270Info.u32FpsX1000 = nFpsX1000;

    return nNumRegs; //Return number of sensor registers to write
}

/** @brief Convert gain to sensor register setting
@param[in] nGainX1024 target sensor gain x 1024
@param[out] pRegs I2C data buffer
@param[in] nMaxNumRegs pRegs buffer length
@retval Return the number of I2C data in pRegs
*/
static unsigned int PS5270EarlyInitGain( unsigned int u32GainX1024, I2cCfg_t *pRegs, unsigned int nMaxNumRegs)
{
    /*TODO: Parsing gain to sensor i2c setting*/
    unsigned char n;
    unsigned int u32Gain;
    int nNumRegs = 0;
    int nId = 0;
    char sghd_flag = 0;
    int nGainTableSize= sizeof(gain_table)/sizeof(gain_table[0]);
    I2cCfg_t gain_reg[] = {
        //{0xEF, 0x01},
        {0x83, 0x04}, //analog gain suggest from 1.25X to 32X
        {0x18, 0x00},
    };

    u32Gain = (u32GainX1024)*10000/1024;
    u32Gain = (u32Gain > SENSOR_MAXGAIN*10000) ? SENSOR_MAXGAIN*10000:u32Gain; //limit max gain at 128X

    for(nId = 6; nId < nGainTableSize; nId++)
    {
        if(u32Gain <= gain_table[nId].total_gain)   break;
    }

    if (nId == nGainTableSize) {
        return 0;
    }

#define AG_HS_NODE  (6*10000)    // 6.0x (U.9)
#define AG_LS_NODE  (5*10000)    // 5.0x (U.9)

    if(gain_table[nId].total_gain >= AG_HS_NODE)
    {
        sghd_flag = 0;    //HS mode , 4X
    }
    else if(gain_table[nId].total_gain <= AG_LS_NODE)
    {
        sghd_flag = 1;    //LS mode , 1x
    }

    if (sghd_flag == 0) nId -= 64; //HS mode, gain_table[gain_idx].total_gain /= 4

    gain_reg[0].u16Data = nId&0xFF;
    gain_reg[1].u16Data = sghd_flag;

    /*copy result*/
    //memcpy(pRegs,gain_reg,sizeof(gain_reg));
    for(n=0;n<sizeof(gain_reg);++n)
    {
        ((unsigned char*)pRegs)[n] = ((unsigned char*)gain_reg)[n];
    }
    nNumRegs = sizeof(gain_reg)/sizeof(gain_reg[0]);

    _gPS5270Info.u32GainX1024 = u32GainX1024;

    return nNumRegs;
}

static unsigned int PS5270EarlyInitGetSensorInfo(EarlyInitSensorInfo_t* pSnrInfo)
{
    if(pSnrInfo)
    {
        pSnrInfo->eBayerID      = E_EARLYINIT_SNR_BAYER_BG;
        pSnrInfo->ePixelDepth   = EARLYINIT_DATAPRECISION_14;
        pSnrInfo->u32FpsX1000   = _gPS5270Info.u32FpsX1000;
        pSnrInfo->u32Width      = _gPS5270Info.u32Width;
        pSnrInfo->u32Height     = _gPS5270Info.u32Height;
        pSnrInfo->u32GainX1024  = _gPS5270Info.u32GainX1024;
        pSnrInfo->u32ShutterUs  = _gPS5270Info.u32ShutterUs;
        pSnrInfo->u32ShutterShortUs = _gPS5270Info.u32ShutterShortUs;
        pSnrInfo->u32GainShortX1024 = _gPS5270Info.u32GainShortX1024;
        pSnrInfo->u8ResId       = _gPS5270Info.u8ResId;
        pSnrInfo->u8HdrMode     = _gPS5270Info.u8HdrMode;
        pSnrInfo->u32TimeoutMs  = 300;
        //CamOsPrintf("PS5250 , FPS=%u, Shutter=%u, Gain=%u \n", pSnrInfo->u32FpsX1000, pSnrInfo->u32ShutterUs, pSnrInfo->u32GainX1024);
    }
    return 0;
}

/* Sensor EarlyInit implementation end*/
SENSOR_EARLYINIY_ENTRY_IMPL_END( PS5270,
                                 Sensor_init_table,
                                 PS5270EarlyInitShutterAndFps,
                                 PS5270EarlyInitGain,
                                 PS5270EarlyInitGetSensorInfo
                                );
