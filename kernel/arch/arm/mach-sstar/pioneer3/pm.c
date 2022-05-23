/*
* pm.c- Sigmastar
*
* Copyright (c) [2019~2020] SigmaStar Technology.
*
*
* This software is licensed under the terms of the GNU General Public
* License version 2, as published by the Free Software Foundation, and
* may be copied, distributed, and modified under those terms.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License version 2 for more details.
*
*/
#include <linux/suspend.h>
#include <linux/io.h>
#include <asm/suspend.h>
#include <asm/fncpy.h>
#include <asm/cacheflush.h>
#include "ms_platform.h"
#include "gpi-irqs.h"
#include "registers.h"


#define FIN     printk(KERN_ERR"[%s]+++\n",__FUNCTION__)
#define FOUT    printk(KERN_ERR"[%s]---\n",__FUNCTION__)
#define HERE    printk(KERN_ERR"%s: %d\n",__FILE__,__LINE__)
#define SUSPEND_WAKEUP 0
#define SUSPEND_SLEEP  1
#define STR_PASSWORD   0x5A5A55AA
#define IN_SRAM 1

#define GET_REG8_ADDR(x, y)     (x+(y)*2-((y)&1))
#define GET_REG16_ADDR(x, y)    (x+(y)*4)



#if IN_SRAM
#define SUSPENDINFO_ADDRESS 0xA000B000
#else
#define SUSPENDINFO_ADDRESS 0x20000000
#endif
//----0xA0000000~0xA000A000---IPL.rom
//----0xA000C000~0xA000D000---IPL.ram
//----0xA0001000~0xA0002000---suspend code

typedef struct {
    char magic[8];
    unsigned int resume_entry;
    unsigned int count;
    unsigned int status;
    unsigned int password;
#ifdef CONFIG_SS_STRDEBUG
    unsigned int checksum;
#endif
} suspend_keep_info;

extern void sram_suspend_imi(void)__attribute__((aligned(16)));
extern void cpu_suspend_abort(void);

static void (*mstar_suspend_imi_fn)(void);
void (*mstar_resume_imi_fn)(void);
void (sram_resume_imi)(void)__attribute__((aligned(8)));


static void __iomem *suspend_imi_vbase;
void __iomem *resume_imi_vbase;
void __iomem *resume_imistack_vbase;

suspend_keep_info *pStr_info;
int suspend_status = SUSPEND_WAKEUP;
extern void recode_timestamp(int mark, const char* name);

typedef enum
{
    E_MIUPLL_100M = 0,
    E_MIUPLL_133M ,
    E_MIUPLL_150M ,
    E_MIUPLL_166M ,
    E_MIUPLL_200M ,
    E_MIUPLL_216M ,
    E_MIUPLL_233M ,
    E_MIUPLL_266M ,
    E_MIUPLL_400M ,
    E_MIUPLL_468M ,
    E_MIUPLL_MAX,
}MIUPLL_TYPE_E;

#define BOND_SSD222Q_QFN128_2Gb_DDR3_Nanya      0x14
#define BOND_SSD222D_QFN128_1Gb_DDR3_Nanya      0x17
#define BOND_SSD222D_QFN128_1Gb_DDR3_WB         0x13
#define BOND_SSD222__QFN128_512Mb_DDR2_WB       0x16
#define BOND_SSD222__QFN128_512Mb_DDR2_ESMT     0x12
#define BOND_SSD221__QFN68__512Mb_DDR2_WB       0x06
#define BOND_SSD221__QFN68__512Mb_DDR2_ESMT     0x02
#define BOND_SSD210F_QFN68__128Mb_DDR2_WB       0x04
#define BOND_SSD211B_BGA__128Mb_DDR2_WB       0x0C
#define BOND_SSD211AM_QFN68__128Mb_DDR2_WB       0x05

#define RIU_BASE_ADDR (0x1F000000)
#define RIU_BASE_ADDR_VA (0xFD000000)
#define REG_ADDR_BASE_TIMER0          GET_REG8_ADDR( RIU_BASE_ADDR_VA, 0x3020 )
#define REG_ADDR_BASE_TIMER1          GET_REG8_ADDR( RIU_BASE_ADDR_VA, 0x3040 )
#define REG_ADDR_BASE_TIMER2          GET_REG8_ADDR( RIU_BASE_ADDR_VA, 0x3060 )

#define REG_ADDR_BASE_PADTOP          GET_REG8_ADDR( RIU_BASE_ADDR, 0x103C00 )

#define TIMER_BASE_ADDR     REG_ADDR_BASE_TIMER0
#define TIMER1_BASE_ADDR     REG_ADDR_BASE_TIMER1
#define TIMER2_BASE_ADDR     REG_ADDR_BASE_TIMER2


#define TIMER_CTRL_ADDR     GET_REG16_ADDR(TIMER_BASE_ADDR, 0x00)
#define TIMER_HIT_ADDR      GET_REG16_ADDR(TIMER_BASE_ADDR, 0x01)
#define TIMER_MAX_L_ADDR    GET_REG16_ADDR(TIMER_BASE_ADDR, 0x02)
#define TIMER_MAX_H_ADDR    GET_REG16_ADDR(TIMER_BASE_ADDR, 0x03)
#define TIMER_CAPCNT_L_ADDR GET_REG16_ADDR(TIMER_BASE_ADDR, 0x04)
#define TIMER_CAPCNT_H_ADDR GET_REG16_ADDR(TIMER_BASE_ADDR, 0x05)
#define TIMER_DIVIDE_ADDR   GET_REG16_ADDR(TIMER_BASE_ADDR, 0x06)
#define TIMER1_DIVIDE_ADDR   GET_REG16_ADDR(TIMER1_BASE_ADDR, 0x06)
#define TIMER2_DIVIDE_ADDR   GET_REG16_ADDR(TIMER2_BASE_ADDR, 0x06)
#define TIMER_OSC           12000000
#define TIMER_CLOCK           TIMER_OSC           // 1 sec = TIMER_CLOCK ticks
#define TIMER_DELAY_1s        (TIMER_OSC / 1)
#define TIMER_DELAY_500ms     (TIMER_OSC / 2)
#define TIMER_DELAY_250ms     (TIMER_OSC / 4)
#define TIMER_DELAY_100ms     (TIMER_OSC / 10)
#define TIMER_DELAY_10ms      (TIMER_OSC / 100)
#define TIMER_DELAY_5ms       (TIMER_OSC / 200)
#define TIMER_DELAY_2500us    (TIMER_OSC / 400)
#define TIMER_DELAY_2ms       (TIMER_OSC / 500)
#define TIMER_DELAY_1ms       (TIMER_OSC / 1000)
#define TIMER_DELAY_500us     (TIMER_OSC / 2000)
#define TIMER_DELAY_250us     (TIMER_OSC / 4000)
#define TIMER_DELAY_100us     (TIMER_OSC / 10000)
#define TIMER_DELAY_10us      (TIMER_OSC / 100000)
#define TIMER_DELAY_5us       (TIMER_OSC / 200000)
#define TIMER_DELAY_2us       (TIMER_OSC / 500000)
#define TIMER_DELAY_1us       (TIMER_OSC / 1000000)

#define REG(Reg_Addr)           (*(volatile unsigned short *)(Reg_Addr))

#define REG_ADDR_DEBUG            (0xFD200800)

/* read register by byte */
#define ms_readb(a) (*(volatile unsigned char *)IO_ADDRESS(a))

/* read register by word */
#define ms_readw(a) (*(volatile unsigned short *)IO_ADDRESS(a))

/* read register by long */
#define ms_readl(a) (*(volatile unsigned int *)IO_ADDRESS(a))

/* write register by byte */
#define ms_writeb(v,a) (*(volatile unsigned char *)IO_ADDRESS(a) = (v))

/* write register by word */
#define ms_writew(v,a) (*(volatile unsigned short *)IO_ADDRESS(a) = (v))

/* write register by long */
#define ms_writel(v,a) (*(volatile unsigned int *)IO_ADDRESS(a) = (v))
#define INREG8(x)           ms_readb(x)
#define INSREG8(addr, mask, val) OUTREG8(addr, ((INREG8(addr)&(~(mask))) | val))

#define INREG16(x)              ms_readw(x)
#define OUTREG16(x, y)          ms_writew((u16)(y), x)
#define SETREG16(x, y)          OUTREG16(x, INREG16(x)|(y))
#define CLRREG16(x, y)          OUTREG16(x, INREG16(x)&~(y))
#define INREGMSK16(x, y)        (INREG16(x) & (y))
#define OUTREGMSK16(x, y, z)    OUTREG16(x, ((INREG16(x)&~(z))|((y)&(z))))

#define INSREG16(addr, mask, val) OUTREG16(addr, ((INREG16(addr)&(~(mask))) | val))

#define SetDebugFlag(x) (REG(REG_ADDR_DEBUG)=x)

#define FLAG_PRE_MAIN (0x7000)
#define FLAG_PRE_PLL (0x7001)
#define FLAG_PRE_PLLDONE (0x7002)
#define FLAG_MAIN_ID (0x7003)
#define FLAG_INI_MIU_CLOCK (0x7004)
#define FLAG_INI_SPI_CLOCK (0x7005)
#define FLAG_MAIN_DONE (0x7006)
#define FLAG_RESUME_DONE (0x7007)
#define FLAG_MIU_FAIL (0x7008)

#define FLAG_RESUME_FAIL (0x7777)

#define _MIUPLL_Getloop_div_second(enType) ((enType==E_MIUPLL_133M) ? 0x1C :\
                                                (enType==E_MIUPLL_150M) ? 0x1F :\
                                                (enType==E_MIUPLL_166M) ? 0x1C :\
                                                (enType==E_MIUPLL_200M) ? 0x19 :\
                                                (enType==E_MIUPLL_216M) ? 0x1B :\
                                                (enType==E_MIUPLL_233M) ? 0x1E :\
                                                (enType==E_MIUPLL_266M) ? 0x2C :\
                                                (enType==E_MIUPLL_400M) ? 0x32 : 0x27)

#define _MIUPLL_Getu8output_div_first(enType) ((enType==E_MIUPLL_133M) ? 0x03 :\
                                                (enType==E_MIUPLL_150M) ? 0x03 :\
                                                (enType==E_MIUPLL_166M) ? 0x02 :\
                                                (enType==E_MIUPLL_200M) ? 0x01 :\
                                                (enType==E_MIUPLL_216M) ? 0x01 :\
                                                (enType==E_MIUPLL_233M) ? 0x01 :\
                                                (enType==E_MIUPLL_266M) ? 0x02 :\
                                                (enType==E_MIUPLL_400M) ? 0x01 : 0x0)

static inline void timer_set_divide(int divide)
{
    REG(TIMER_DIVIDE_ADDR)=divide-1;
    REG(TIMER1_DIVIDE_ADDR)=divide-1;
    REG(TIMER2_DIVIDE_ADDR)=divide-1;
}

static inline U32 timer_get_count(void)
{
    // 1 tick = 1/OSC1 sec
    U32 val=0;
    val = (REG(TIMER_CAPCNT_L_ADDR)& 0xFFFF);
    val +=  (REG(TIMER_CAPCNT_H_ADDR)<<16);

    return val;
}

static inline U32 timer_get_max(void)
{
    // 1 tick = 1/OSC1 sec
    U32 val=0;

    val = (REG(TIMER_MAX_L_ADDR)& 0xFFFF);
    val +=  (REG(TIMER_MAX_H_ADDR)<<16);

    return val;
}

static inline void loop_delay_timer(unsigned long count)
{
    U32 st0=0;
    U32 tick_count=0;
    U32 st1=0;

    st0=timer_get_count();

    if(0==count)
    {
        return;
    }

    while(1)
    {
        st1=timer_get_count();
        if(st1<st0)
        {
            tick_count+=((timer_get_max()-st0) + st1);
        }
        else
        {
            tick_count+=(st1-st0);
        }
        st0=st1;

        if(tick_count>=count)
        {
            break;
        }
    }
}


#ifdef CONFIG_SS_STRDEBUG
#include <asm-generic/sections.h>

int calc_checksum(void *buf, int size)
{
    int i;
    int sum = 0;

    for (i = 0; size > i; i += 4)
        sum += *(volatile int*)(buf + i);
    return sum;
}
#endif
static inline void miu_self_refresh(U8 bEnter)
{
    if(bEnter)
    {
        //enter self refresh
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012E0), 0x0000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101246), 0xFFFE);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101266), 0xFFFF);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101286), 0xFFFF);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012A6), 0xFFFF);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011E6), 0x00FE);
        loop_delay_timer(TIMER_DELAY_1us);

        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101218), 0x0400);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101200), 0x30, 0x20);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x1E, 0x04);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x01, 0x00);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x01, 0x01);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x01, 0x00);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x1E, 0x02);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x01, 0x00);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x01, 0x01);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x01, 0x00);
        loop_delay_timer(TIMER_DELAY_1us);


        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101200), 0x202E);
        loop_delay_timer(TIMER_DELAY_1us);

        //power down
        //INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101203), 0xF0, 0xF0);
        //INSREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101000), 0x18, 0x18);
        //OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101054), 0xC070);
        //OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101008), 0x0000);
    }
    else
    {
        //power up
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101000), 0x10, 0x00); //Not sel gpio mode
        loop_delay_timer(TIMER_DELAY_1us);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101203), 0xF0, 0x00); //Disable OENZ
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101054), 0x70, 0x00); //ATOP_PD on
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101008), 0x3F, 0x3F); //rx enable
        // DQSM RST
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101E), 0x0005);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101E), 0x000F);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101E), 0x0005);
        //exit self refresh
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x20, 0x00);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011e6), 0x00FE);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x1E, 0x02);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x01, 0x00);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x01, 0x01);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101201), 0x01, 0x00);
        INSREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101200), 0x30, 0x00);
        loop_delay_timer(TIMER_DELAY_1us);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10121e), 0x8c08);
        // CLEAR above mark
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101246), 0x0000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101266), 0x0000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101286), 0x0000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012A6), 0x0000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011e6), 0x0000);
    }
}

static inline void _IPLResume(U8 u8Resume)
{
    if(u8Resume)
    {
//        U32 u32Paddr = 0xC00114D5;
//        unsigned int SIG1, SIG2; // SIG_STR
        SetDebugFlag(0x5555);
        {
            miu_self_refresh(1);
            miu_self_refresh(0);
//            ERR_MSG("exit miu_self_refresh!\r\n");

//            pStr_info->status = SUSPEND_WAKEUP;
            SetDebugFlag(FLAG_RESUME_DONE);
//            asm volatile("bx %0" : : "r" (u32Paddr));
        }

    }
}


static inline void MiuBootClkSelect(U8 bMpll)
{
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x00103840), bMpll ? 0x04 : 0);//switch clk_miu_sc_gp from clk_boot_p(12MHz) to clk_miu
}
static inline void board_init(U8 bondingID)
{
    switch (bondingID)
    {
        default :
        SetDebugFlag(0x9999);
        // VID1 : PAD_SAR_GPIO1=L: 0.9V
        CLRREG16(0x1F203C48, BIT5|BIT4|BIT1|BIT0); //  reg_test_in_mode reg_test_out_mode
        CLRREG16(0x1F002844, BIT5|BIT4|BIT3|BIT2|BIT1|BIT0); // reg_sar_aisel
        CLRREG16(0x1F002844, BIT9);
        CLRREG16(0x1F002848, BIT1);
         SetDebugFlag(0x9998);
        break;
    }
    switch (bondingID)
    {
        case BOND_SSD221__QFN68__512Mb_DDR2_WB:
        case BOND_SSD221__QFN68__512Mb_DDR2_ESMT:
        case BOND_SSD210F_QFN68__128Mb_DDR2_WB:
            // set reg_ej_mode , mode3=PAD_TTL12~15
            INSREG16( GET_REG16_ADDR(REG_ADDR_BASE_PADTOP, 0x60), BIT8|BIT9|BIT10,BIT10);
            break;
        default:
            // set reg_ej_mode , mode3=PAD_KEY10~13
            INSREG16( GET_REG16_ADDR(REG_ADDR_BASE_PADTOP, 0x60), BIT8|BIT9|BIT10,BIT8|BIT9);
            break;
    }
}
static inline void _cpu_dvfs(U32 u32TargetSet)
{
    OUTREG16(RIU_BASE_ADDR + (0x1032A4 << 1), u32TargetSet&0xFFFF); //set target freq to LPF high
    OUTREG16(RIU_BASE_ADDR + (0x1032A6 << 1), (u32TargetSet>>16)&0xFFFF);
    OUTREG16(RIU_BASE_ADDR + (0x1032B0 << 1), 0x0001); //switch to LPF control
    OUTREG16(RIU_BASE_ADDR + (0x1032AA << 1), 0x0006); //mu[2:0]
    OUTREG16(RIU_BASE_ADDR + (0x1032AE << 1), 0x0008); //lpf_update_cnt[7:0]
    SETREG16(RIU_BASE_ADDR + (0x1032B2 << 1), BIT12);  //from low to high
    OUTREG16(RIU_BASE_ADDR + (0x1032A8 << 1), 0x0000); //toggle LPF enable
    OUTREG16(RIU_BASE_ADDR + (0x1032A8 << 1), 0x0001);
    while( !(INREG16(RIU_BASE_ADDR + (0x1032BA << 1))&BIT0) ); //polling done
    OUTREG16(RIU_BASE_ADDR + (0x1032A8 << 1), 0x0000);
    OUTREG16(RIU_BASE_ADDR + (0x1032A0 << 1), u32TargetSet&0xFFFF);  //store freq to LPF low
    OUTREG16(RIU_BASE_ADDR + (0x1032A2 << 1), (u32TargetSet>>16)&0xFFFF);
}

static inline void usbpll_init(void)
{
    unsigned char pll_en = 0;

    pll_en = INREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x142000));
//    if((pll_en & 0x18) == 0)
//    {
//        return;
//    }

    //upll_init.txt
    // enable UPLL & UTMI; wait around 100us for PLL to be stable
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x142000), 0xC0);    // enable 320MHz clock
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x14200E), 0x11);    // enable 384MHz/480MHz clock

    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x142002), 0x98);
    OUTREG16(RIU_BASE_ADDR + (0x142004 << 1), 0x0110);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x14200e), 0x11);


    loop_delay_timer(TIMER_DELAY_100us);
}

static inline void cpupll_init(void)
{
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103224), 0x88);//reg_mipspll_loop_div[7:0]=0x88, It's default setting.
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103225), 0x00);//reg_mipspll_en_post=0x0, It's default setting.
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103223), 0x01);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x001032C2), 0x45); //pll 800MHz  432/50 x 2^19 = 4529848 = h0x451EB8
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x001032C1), 0x1E);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x001032C0), 0xB8);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x001032C4), 0x01);//Update ARM frequency synthesizer N.f setting
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103223), 0x00);//ARM PLL PD (Power-on)
    OUTREG16(RIU_BASE_ADDR + (0x1032A0 << 1), 0x1EB8);  //store freq to LPF low
    OUTREG16(RIU_BASE_ADDR + (0x1032A2 << 1), 0x0045);
    loop_delay_timer(TIMER_DELAY_100us); //TIMER_DELAY_100us not enough
    _cpu_dvfs(0x451EB9);
}

static inline void lpll_init(void)
{
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103390), 0xA3);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103391), 0x8B);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103392), 0x2E);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103393), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103380), 0x01);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103381), 0x22);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103382), 0x20);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103383), 0x04);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103384), 0x43);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103385), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103386), 0x02);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103387), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103388), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103389), 0x05);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103394), 0x01);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103395), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103396), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103397), 0x00);
}

static inline void _IPLGetResume(U32 *pc_addr, U8 reset)
{
    reset = reset;
    {
        unsigned int SIG1, SIG2; // SIG_STR

//        suspend_keep_info *str_info = (suspend_keep_info *)SUSPENDINFO_ADDRESS;
        SIG1= pStr_info->magic[0];
        SIG2= pStr_info->magic[4];
        if ((pStr_info->password == STR_PASSWORD) &&
            (SIG1 == 0x5F474953) && (SIG2 == 0x00525453) &&
            (pStr_info->status == SUSPEND_SLEEP))
        {
            *pc_addr = pStr_info->resume_entry;
        }
        SetDebugFlag(0xA);
    }

}
#ifndef UNUSED_VAR
#define UNUSED_VAR(v) v = v
#endif

static inline void miu_init_DDR_all(U8 bondingID, U8 bResume)
{

    UNUSED_VAR(bondingID);
    bResume=bResume;

    #if 1
        /*0423_mantis 0000062(GPIO/GPIO_DFT input mode, en_mask, pd_atop, pd_LDO, rx_en)  */
        // wriu -w 0x10108b 0x02 0x00 // turn on tree LDO
//             INSREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10108b),  0x20, 0x00);
        SetDebugFlag(0x6664);
        // wriu -w 0x101088 0xff 0x11 // turn on RX LDO
        INSREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101088),  0xff, 0x11);
        SetDebugFlag(0x6663);
        // wriu -w 0x10100e 0x01 0x01 // en_mask on
        INSREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10100e),  0x01, 0x01);
        // wriu -w 0x101019 0x60 0x00 // GPIO_DFT off
        SetDebugFlag(0x6665);
//             INSREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101019),  0x60, 0x00);
    #endif

    //swch 4
    //================================================
    // Start MIU init !!!
    //================================================
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10121e),  0x0c00);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10121e),  0x0c00);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10121e),  0x0c00);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10121e),  0x0c01);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101246),  0xfffe);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101266),  0xffff);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101286),  0xffff);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012a6),  0xffff);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101106),  0xffff);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101126),  0xffff);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011e6),  0xfffe);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101078),  0x0001);
    loop_delay_timer(TIMER_DELAY_1ms); //ori is (DELAY_MIU_INIT << 2) //wait 4ms
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101024),  0x1000);
    loop_delay_timer(TIMER_DELAY_1ms);      //wait 1ms
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101024),  0x0000);
    loop_delay_timer(TIMER_DELAY_1ms);      //wait 1ms
    //----------------------
    //set DDRPLL0 = 1333MHZ or 1866MHZ
    //----------------------
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101036),  0x0400);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101034),  0x2004);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10108a),  0x0001);
    if((bondingID == BOND_SSD222Q_QFN128_2Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101030),  0x8f5c);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101032),  0x001e);
    }
    else if(bondingID == BOND_SSD222__QFN128_512Mb_DDR2_ESMT)
    {
        // MantisBT: http://172.30.60.53/mantisbt/view.php?id=586
        // DDR clock 1200MHz while miu init, then switch to 1333MHz
        //ERR_MSG("Set DDRPLL0 1200Mhz\r\n");
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101030),  0x147b);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101032),  0x002e);
    }
    else
    {
    #if defined(POWERDOWN)
//		//set 1200
//        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101030),  0x147B);
//        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101032),  0x002e);
        //set 1000
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101030),	0x4bc7);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101032),	0x0037);

        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010FC),  0xFFFF);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010FE),  0xEFEF);
    #else
        if(bondingID == BOND_SSD210F_QFN68__128Mb_DDR2_WB || bondingID == BOND_SSD211B_BGA__128Mb_DDR2_WB || bondingID == BOND_SSD211AM_QFN68__128Mb_DDR2_WB)
        {
            OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101030),  0x147b);
            OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101032),  0x002e);
        }
        else
        {
            OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101030),  0x8000);
            OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101032),  0x0029);
        }
	#endif
    }
    loop_delay_timer(TIMER_DELAY_1ms); //ori is (TIMER_DELAY_10ms * 3); //wait 30ms
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101022),  0x0004);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10102c),  0x0114);
    //---------------------------
    //set DDR2_16_4X_CL9_1333/1866
    //---------------------------
    if((bondingID == BOND_SSD222Q_QFN128_2Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101202),  0x02a3);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101204),  0x0053);

        if((bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_WB))
            OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101206),  0x1538);
        if((bondingID == BOND_SSD222Q_QFN128_2Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_Nanya))
            OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101206),  0x1570);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101208),  0x20dd);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10120a),  0x2d76);

        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10120c),  0xe7e9);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10120e),  0x4096);
    }
    else if((bondingID == BOND_SSD210F_QFN68__128Mb_DDR2_WB) || (bondingID == BOND_SSD211B_BGA__128Mb_DDR2_WB) || (bondingID == BOND_SSD211AM_QFN68__128Mb_DDR2_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101202),  0x0252);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101204),  0x0051);  //rd timing

        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101206),  0x1b50);  //[11]csz=0 //[10:8]tCKE; [7:0]tREFI
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101208),  0x1e99);  //[13:8]tRAS; [7:4]tRP; [3:0]tRCD
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10120a),  0x2777);  //[14:8]tRC; [7:4]tRTP; [3:0]tRRD

        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10120c),  0x95a8);  //[15:12]tRTW; [11:8]tWTR; [7:4]tWR; [3:0]tWL
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10120e),  0x4036);  //[14:12]tCCD; [10:0]tRFC
    }
    else
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101202),  (bResume)?0xf292:0x0292);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101204),  0x0051);  //rd timing

        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101206),  0x1b50);  //[11]csz=0 //[10:8]tCKE; [7:0]tREFI
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101208),  0x1e99);  //[13:8]tRAS; [7:4]tRP; [3:0]tRCD
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10120a),  0x2777);  //[14:8]tRC; [7:4]tRTP; [3:0]tRRD

        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10120c),  0x95a8);  //[15:12]tRTW; [11:8]tWTR; [7:4]tWR; [3:0]tWL
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10120e),  0x404c);  //[14:12]tCCD; [10:0]tRFC
    }

    if((bondingID == BOND_SSD222Q_QFN128_2Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101210),  0x1f14);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101212),  0x4004);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101214),  0x8020);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101216),  0xc000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101228),  0x00b0);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101322),  0x0003);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101340),  0x0d0d);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101342),  0x0620);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101344),  0x2d07);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101346),  0x0e09);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101348),  0x0e07);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10134a),  0x0504);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10134c),  0x0528);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10134e),  0x0096);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101350),  0xe000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101352),  0x0000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101354),  0x0d00);
    }
    else
    {
        if((bondingID == BOND_SSD222__QFN128_512Mb_DDR2_ESMT) || (bondingID == BOND_SSD221__QFN68__512Mb_DDR2_ESMT))
        {
            OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101210),  0x0203);  //MR0
        }
        else
        {
            OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101210),  0x0003);  //MR0
        }

        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101212),  0x4004);  //MR1
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101214),  0x8000);  //MR2
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101216),  0xc000);  //MR3
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101228),  0x0070);  //[8:4]odtm_dly
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012d2),  0x6000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101322),  0x0003);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10133e),  0x0000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101340),  0x0909);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101342),  0x071e);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101344),  0x2707);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101346),  0x0908);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101348),  0x0905);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10134a),  0x0304);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10134c),  0x0528);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10134e),  0x0046);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101350),  0xe000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101352),  0x0000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101354),  0x0900);
    }
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101380),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101386),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1013fe),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101160),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101162),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101164),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101166),  0x0030);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101168),  0x5000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101002),  0xaaaa);
    if((bondingID == BOND_SSD222Q_QFN128_2Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101004),  0x0008);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10100a),  0x2200);  //change phase  F6t
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10100e),  0x0097);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10102e),  0x1122);  //change phase  F6
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101038),  0x0077);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10103a),  0x7076);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10103c),  0x9422);  //change phase  F6t
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10103e),  0xA044);  //change phase  F6t
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101048),  0x0077);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10104a),  0x7070);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10104c),  0x0044);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10104e),  0x0044);  //change phase  F6t
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101050),  0x1111);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101052),  0x000b);
    }
    else if((bondingID == BOND_SSD210F_QFN68__128Mb_DDR2_WB) || (bondingID == BOND_SSD211B_BGA__128Mb_DDR2_WB) || (bondingID == BOND_SSD211AM_QFN68__128Mb_DDR2_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101004),  0x0000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10100a),  0x2200); //[14:12]cke1_skew; [10:8]cke0_skew; [7:4]clkph_cke1; [3:0]clkph_cke0
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10100e),  0x008f);  //[5:1]dqsm_dly
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10102e),  0x1122);  //ddrpll_region_sel
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101038),  0x0077);  //clkph_dq for I5
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10103a),  0x8080);  //clkph_cmd/dqsm/clk
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10103c),  0xa222);  //[11:8]dqs_skew; [7:4]dqsm1_skew; [3:0]dqsm0_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10103e),  0x2222);  //[7:4]dq_skew; [3:0]oen_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101048),  0x0077);  //clkph_dq for I5
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10104a),  0x8080);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10104c),  0x0022);  //dqsm23_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10104e),  0x0022);  //dqsm01_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101050),  0x1111);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101052),  0x0000);
    }
    else
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101004),  0x0000);
//        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10100a),  0x1100);
        /////No need to change, avoid resume failure///
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10100a),  0x0000); //[14:12]cke1_skew; [10:8]cke0_skew; [7:4]clkph_cke1; [3:0]clkph_cke0
        ////////////////////////////////////////
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10100e),  0x008f);  //[5:1]dqsm_dly
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10102e),  0x1122);  //ddrpll_region_sel
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101038),  0x0077);  //clkph_dq for I5
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10103a),  0x7070);  //clkph_cmd/dqsm/clk
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10103c),  0x9111);  //[11:8]dqs_skew; [7:4]dqsm1_skew; [3:0]dqsm0_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10103e),  0x1111);  //[7:4]dq_skew; [3:0]oen_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101048),  0x0077);  //clkph_dq for I5
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10104a),  0x7070);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10104c),  0x0011);  //dqsm23_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10104e),  0x0011);  //dqsm01_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101050),  0x1111);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101052),  0x0000);
    }
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10106c),  0x0808);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10106e),  0x0808);

    if((bondingID == BOND_SSD222Q_QFN128_2Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101074),  0x0404);  // TrigLvl
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101076),  0x0404);  // TrigLvl
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101094),  0x1413);  //change phase	F6 1/2 kcode

        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a0),  0x4441);  //DQ dskew 3/2/1/0
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a2),  0x4342);  //DQ dskew 7/6/5/4
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a4),  0x6655);  //DQ dskew 11/10/9/8
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a6),  0x7666);  //DQ dskew 15/14/13/12
    }
    else if((bondingID == BOND_SSD222__QFN128_512Mb_DDR2_WB) || (bondingID == BOND_SSD222__QFN128_512Mb_DDR2_ESMT))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101074),  0x0404);  // TrigLvl
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101076),  0x0404);  // TrigLvl
        if((bondingID == BOND_SSD222__QFN128_512Mb_DDR2_ESMT))
        {
            OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101094),  0x1818);  //change phase	F6 1/2 kcode
        }
        else
        {
            OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101094),  0x1616);  //change phase	F6 1/2 kcode
        }

        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a0),  0x6788);  //DQ dskew 3/2/1/0
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a2),  0x79a6);  //DQ dskew 7/6/5/4
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a4),  0x2122);  //DQ dskew 11/10/9/8
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a6),  0x4321);  //DQ dskew 15/14/13/12
    }
    else if((bondingID == BOND_SSD221__QFN68__512Mb_DDR2_WB) || (bondingID == BOND_SSD221__QFN68__512Mb_DDR2_ESMT))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101074),  0x0404);  // TrigLvl
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101076),  0x0404);  // TrigLvl
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101094),  0x1516);  //change phase	F6 1/2 kcode

        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a0),  0xbcdc);  //DQ dskew 3/2/1/0
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a2),  0xcddc);  //DQ dskew 7/6/5/4
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a4),  0x3232);  //DQ dskew 11/10/9/8
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a6),  0x4311);  //DQ dskew 15/14/13/12
    }
    else if((bondingID == BOND_SSD210F_QFN68__128Mb_DDR2_WB) || (bondingID == BOND_SSD211B_BGA__128Mb_DDR2_WB) || (bondingID == BOND_SSD211AM_QFN68__128Mb_DDR2_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101074),  0x0404);  // TrigLvl
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101076),  0x0404);  // TrigLvl
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101094),  0x1715);  //change phase	F6 1/2 kcode

        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a0),  0xadbf);  //DQ dskew 3/2/1/0
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a2),  0xffdc);  //DQ dskew 7/6/5/4
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a4),  0x7372);  //DQ dskew 11/10/9/8
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a6),  0x4846);  //DQ dskew 15/14/13/12
    }

    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010a8),  0x1111);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010aa),  0x1111);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010ac),  0x1111);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010ae),  0x1111);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010b6),  0x0000);
    if((bondingID == BOND_SSD222Q_QFN128_2Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010b8),  0x4444);  //reg_oen_dqx_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010ba),  0x0444);  //reg_oenX_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010bc),  0x0444);  //reg_dqsX_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010be),  0x0444);  //reg_dqX_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d0),  0x4444);  //sel_mclk1x_skew_0_dqX
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d2),  0x4444);  //change phase
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d4),  0x5555);  //change phase  F6
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d6),  0x5555);  //change phase  F6
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d8),  0x0054);  //change phase  F6
    }
    else if((bondingID == BOND_SSD210F_QFN68__128Mb_DDR2_WB) || (bondingID == BOND_SSD211B_BGA__128Mb_DDR2_WB) || (bondingID == BOND_SSD211AM_QFN68__128Mb_DDR2_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010b8),  0x2222);  //reg_oen_dqx_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010ba),  0x0222);  //reg_oenX_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010bc),  0x0222);  //reg_dqsX_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010be),  0x0222);  //reg_dqX_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d0),  0x5555);  //sel_mclk1x_skew_0_dqX
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d2),  0x5555);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d4),  0x5555);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d6),  0x5555);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d8),  0x0055);
    }
    else
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010b8),  0x1111);  //reg_oen_dqx_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010ba),  0x0111);  //reg_oenX_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010bc),  0x0111);  //reg_dqsX_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010be),  0x0111);  //reg_dqX_skew
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d0),  0x4444);  //sel_mclk1x_skew_0_dqX
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d2),  0x4444);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d4),  0x4444);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d6),  0x4444);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010d8),  0x0044);
    }

    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010e0),  0x5555);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010e2),  0x5555);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010e4),  0x5555);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010e6),  0x5555);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1010e8),  0x0055);
    //=====================================================================
    // inivec setting start
    //=====================================================================
    //-----------------
    //program DLL
    //-----------------
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101062),  0x007f);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101064),  0xf000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101060),  0x00cb);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101060),  0x00cf);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101060),  0x00cb);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101060),  0x00c3);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101060),  0x00cb);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101060),  0x00c3);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101060),  0x00cb);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101060),  0x00c2);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101060),  0x00c0);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101060),  0x33c8);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101070),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101098),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10109a),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101090),  0xf0f1);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101070),  0x0800);
    //---------------------------
    //program dig general setting
    //---------------------------
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10122c),  0x8021);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012fc),  0x951a);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101252),  0xffff);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101272),  0xffff);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101292),  0xffff);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012b2),  0x0000);//Disable VPA protect
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101112),  0xffff);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101132),  0xffff);
    //---------------------
    // Default BW setting
    //---------------------
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101240),  0x8015);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101260),  0x8015);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101280),  0x8015);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012a0),  0x8015);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101100),  0x8015);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101120),  0x8015);
    //---------------------------
    //program ana general setting
    //---------------------------
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10108a),  0x0001);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101070),  0x0800);
    if((bondingID == BOND_SSD222Q_QFN128_2Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101058),  0x0a0a);  //change phase  F6
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10105a),  0xaaaa);  //change phase  F6
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10105c),  0xaaaa);  //change phase  F6
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10105e),  0xaaaa);  //change phase  F6
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101a),  0x8000);
    }
    else if((bondingID == BOND_SSD210F_QFN68__128Mb_DDR2_WB) || (bondingID == BOND_SSD211B_BGA__128Mb_DDR2_WB) || (bondingID == BOND_SSD211AM_QFN68__128Mb_DDR2_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101058),  0x0505);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10105a),  0x5555);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10105c),  0x5577);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10105e),  0x5555);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101a),  0x8000);
    }
    else
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101058),  0x0a0a);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10105a),  0xaaaa);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10105c),  0xaaaa);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10105e),  0xaaaa);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101a),  0x8000);
    }
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101c),  0x0020);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101008),  (bResume)?0x0000:0x003F);

    //-----------
    // DQSM RST
    //-----------
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101e),  0x0005);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101e),  0x000f);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101e),  0x0005);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10121e),  0x8c01);
    //---------------------------
    //Release miu SW reset
    //---------------------------
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10121e),  0x8c00);
    //---------------------------
    //disable GPIO
    //---------------------------
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101000), (bResume)?0x2018:0x2010);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101000), (bResume)?0x0008:0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101018),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10107c),  0x0000);

    if((bondingID == BOND_SSD222Q_QFN128_2Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101054),  0xc000);
    }
    else
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101054), (bResume)?0x4070:0x4000);  //1T cmd mode
    }

    //-----------
    // DQSM RST
    //-----------
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101e),  0x0005);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101e),  0x000f);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101e),  0x0005);
    //---------------------------
    //select Mapping
    //---------------------------
    if((bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_Nanya) || (bondingID == BOND_SSD222D_QFN128_1Gb_DDR3_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101000),  0x0002);
    }
    else if(bondingID == BOND_SSD222Q_QFN128_2Gb_DDR3_Nanya)
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101000),  0x0003);
    }
    else if((bondingID == BOND_SSD210F_QFN68__128Mb_DDR2_WB) || (bondingID == BOND_SSD211B_BGA__128Mb_DDR2_WB) || (bondingID == BOND_SSD211AM_QFN68__128Mb_DDR2_WB))
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101000),  0x0004);
    }
    else
    {
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101000),  (bResume)?0x0009:0x0001);
    }

    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101200),  0x0000);
    //---------------------
    // begin init DRAM
    //---------------------
    //-----Wait 200us, (keep DRAM RESET = 0)-----
    loop_delay_timer(TIMER_DELAY_1us);  //wait 1ms
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101200),  0x0008);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101200),  0x000c);
    //-----Wait 500us, (keep DRAM RESET = 1 , CKE = 0)-----
    loop_delay_timer(TIMER_DELAY_1us);  //wait 1ms
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101200),  0x000e);
    //-----Wait tXPR=400ns-----
    loop_delay_timer(TIMER_DELAY_1us);  //wait 1ms
    //--------Initial DRAM start here!!!
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101200),  0x000f);
    //-----Wait init done-----
    loop_delay_timer(TIMER_DELAY_100us);  //wait 1ms
    //--------Initial Done
    //--------DDR2 wait 400ns for tXPR(?), DDR3 wait 512T for tZQinit
    loop_delay_timer(TIMER_DELAY_1ms); //ori is (TIMER_DELAY_10ms);     //wait 10ms
    //--------wait 3.9us for a REF to clean RX FIFO wptr
    loop_delay_timer(TIMER_DELAY_1ms); //ori is (TIMER_DELAY_10ms);     //wait 10ms

    // MantisBT: http://172.30.60.53/mantisbt/view.php?id=586
    // DDR clock 1200MHz while miu init, then switch to 1333MHz
    if(bondingID == BOND_SSD222__QFN128_512Mb_DDR2_ESMT)
    {
        //ERR_MSG("Set DDRPLL0 1333Mhz\r\n");
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101030),  0x8000);
        OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101032),  0x0029);
    }

    //-----------
    // DQSM RST
    //-----------
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101e),  0x0005);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101e),  0x000f);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10101e),  0x0005);

    //-------------------------
    //Enable BIST setting
    //-------------------------
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101246),  0x7ffe);  //unmask 0f
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011e6),  0xfffa);  //unmask g128
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012fe),  0xa0e1);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012fe),  0x80e1);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012f0),  0x0000);
}


static inline void miu_init(U8 bondingID, U8 u8Resume)
{
    switch (bondingID)
    {
        case BOND_SSD222__QFN128_512Mb_DDR2_WB:
        case BOND_SSD222__QFN128_512Mb_DDR2_ESMT:
        case BOND_SSD221__QFN68__512Mb_DDR2_WB:
        case BOND_SSD221__QFN68__512Mb_DDR2_ESMT:
        case BOND_SSD222D_QFN128_1Gb_DDR3_Nanya:
        case BOND_SSD222D_QFN128_1Gb_DDR3_WB:
        case BOND_SSD210F_QFN68__128Mb_DDR2_WB:
        case BOND_SSD222Q_QFN128_2Gb_DDR3_Nanya:
            miu_init_DDR_all(bondingID, u8Resume);
            break;
        default:
            miu_init_DDR_all(bondingID, u8Resume);
            SetDebugFlag(FLAG_MIU_FAIL);
            asm("b .");
            break;
    }
}
static inline void miupll_init(MIUPLL_TYPE_E enType)
{
    U8 u8loop_div_second = _MIUPLL_Getloop_div_second(enType);
    U8 u8output_div_first = _MIUPLL_Getu8output_div_first(enType);

    //OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103103), 0x00);// reg1[8] : reg_miu_128bus_pll_pd = 1'b0
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103104), 0x00);//// reg2[5:4] : reg_miu_128bus_pll_input_div_first[1:0] = 2'd0
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103105), 0x00);// reg2[10:8] : reg_miu_128bus_pll_loop_div_first[2:0] = 3'd0
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103106), u8loop_div_second);  // reg3[7:0] : reg_miu_128bus_pll_loop_div_second = 8'd44
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103107), u8output_div_first); // reg3[15:8] : reg_miu_128bus_pll_output_div_first[7:0] = 8'd1
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103108), 0x10);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103109), 0x00);

    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x00103103), 0x00);// reg1[8] : reg_miu_128bus_pll_pd = 1'b0
    loop_delay_timer(TIMER_DELAY_250us);

}

static inline void miu_bw_set(U8 bondingID)
{
    UNUSED_VAR(bondingID);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101240), 0x15);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101241), 0x80);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101242), 0x08);  //group0 member limitation
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101243), 0x20);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101244), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101245), 0x04);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101248), 0xFF);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101249), 0xFF);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10124A), 0x10);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10124B), 0x32);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10124C), 0x54);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10124D), 0x76);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10124E), 0x98);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10124F), 0xBA);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101250), 0xDC);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101251), 0xFE);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10125C), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10125D), 0x00);

    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101260), 0x15);  //group_en, member en
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101261), 0x80);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101262), 0x08);  //group1 member limitation
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101263), 0x20);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101264), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101265), 0x04);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101268), 0xFF);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101269), 0xFF);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10126A), 0x10);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10126B), 0x32);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10126C), 0x54);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10126D), 0x76);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10126E), 0x98);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10126F), 0xBA);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101270), 0xDC);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101271), 0xFE);  //Same mark

    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101280), 0x15);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101281), 0x80);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101282), 0x08);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101283), 0x20);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101284), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101285), 0x04);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101288), 0xFF);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101289), 0xFF);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10128A), 0x10);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10128B), 0x32);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10128C), 0x54);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10128D), 0x76);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10128E), 0x98);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10128F), 0xBA);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101290), 0xDC);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101291), 0xFE);  //Same mark

    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012A0), 0x15);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012A1), 0x80);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012A2), 0x08);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012A3), 0x20);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012A4), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012A5), 0x04);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012A8), 0xFF);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012A9), 0xFF);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012AA), 0x10);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012AB), 0x32);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012AC), 0x54);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012AD), 0x76);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012AE), 0x98);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012AF), 0xBA);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012B0), 0xDC);  //Same mark
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012B1), 0xFE);  //Same mark

    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012FE), 0xE1);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012FF), 0x80);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011E0), 0x02);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011E1), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011E2), 0x1E);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011E3), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011E8), 0x18);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011E9), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011EA), 0x08);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011EB), 0x40);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011EC), 0x02);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011ED), 0x02);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011F8), 0xE1);  //group_priority0_id_ctrl_arb
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011F9), 0xFF);
}


static inline MIUPLL_TYPE_E miuGetmiupll(U8 bondingID)
{
    MIUPLL_TYPE_E enType = E_MIUPLL_266M;
    UNUSED_VAR(bondingID);

    enType = E_MIUPLL_200M;
    return enType;
}
static inline void MiuClkSrcSelect(U8 u8Src)
{
    CLRREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x0010382E), 0x10);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x0010382E), u8Src);//switch MIU clock
    SETREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x0010382E), 0x10);//switch MIU clock
}

static inline void clk_init(void)
{
    //clock_initial.txt
 #if defined(POWERDOWN)
//    ERR_MSG("miu set to 216Mhz\r\n");
    MiuClkSrcSelect(0xC);//switch MIU clock from xtal(12MHz) to MIU PLL
#else
    MiuClkSrcSelect(0x8);//switch MIU clock from xtal(12MHz) to MIU PLL
#endif
    SetDebugFlag(0x56);

    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x00103840), 0x04);//switch clk_miu_sc_gp from clk_boot_p(12MHz) to clk_miu
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x001020F8), 0x01);//switch ARM clock from xtal(12MHz) to ARM PLL
    SetDebugFlag(0x57);

#if defined(POWERDOWN)
//    ERR_MSG("Disabled miupll\r\n");
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x00103103), 0x01);
#endif
    //copy from i6
    //switch L3 bridge clock to MIU2X clock
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x00102202), 0x84);
    //loop_delay_timer(TIMER_DELAY_100ms); //ori is open
    SetDebugFlag(0x58);

    //Speed FAST RIU WRITE (Check with Philips)
    //It P3 new functino to speedup riu write cycle
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x001001A4), 0x01);//riu_bridge fast_w_enable
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x001001A2), 0x00);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x00102275), 0x80);//L3 bridge , bypass_w_cyc_riu

    /*turn on clock for IPL using*/
    //ckg_uart0 = 172M
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x103862), 0x00);
    //ckg_bdma = ckg_miu [4]BDMA clock source
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1038C0), 0x10);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x0E41), 0x10);  //1C:108M, 18:86M 14:72M 10:54M 4:27M 8:36M C:43MH
    SETREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x0E41), BIT6);  //glitch mux
    SetDebugFlag(0x59);


 #if defined(POWERDOWN)
 	//non-pm ckg_spi = clk_216
 	OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x103864), 0x00);  //100:clk_miu
 #else
 	//non-pm ckg_spi = clk_miu
 	OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x103864), 0x10);  //100:clk_miu
 #endif
    SETREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x103864), BIT5);  //glitch mux
    //SPI mode select = fast read mode
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x17E4), 0x01);
    SetDebugFlag(0x60);

    //PLL source auto gating function
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1038E2), 0x03);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1038E3), 0x00);



//    ERR_MSG("RTOS clk_init done \r\n");
}
static inline void miu_unmaskall(void)
{
    //-------------------------                   ),        );
    // unmask all                                 ),        );
    //-------------------------                   ),        );
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101246),  0x0000/*0x7dfe*/);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101266),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101286),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1012a6),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101126),  0x0000);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x1011e6),  0x0000/*0xfff2*/);
    OUTREG16(GET_REG8_ADDR(RIU_BASE_ADDR, 0x10121e),  0x8c08);
}
static inline void _IPLInitAllPllClk(U8 bondingID)
{
//    U32 u32pc_addr=0;
    MIUPLL_TYPE_E enType;

//    board_init(bondingID); //24.5us
    SetDebugFlag(1);
//////5.2ms//////
    cpupll_init();
    SetDebugFlag(2);
    lpll_init();
    SetDebugFlag(3);
    usbpll_init();
    SetDebugFlag(4);
//    _IPLGetResume(&u32pc_addr, 1);
    SetDebugFlag(5);
    miu_init(bondingID, 1);//~5ms
    SetDebugFlag(6);
    enType = miuGetmiupll(bondingID);
    SetDebugFlag(7);
    miupll_init(enType);
    SetDebugFlag(8);
    miu_bw_set(bondingID);
    SetDebugFlag(0x119);
    clk_init(); //TODO

    SetDebugFlag(0xA);
    miu_unmaskall();
    SetDebugFlag(0xB);
//////5.2ms//////
    _IPLResume(1);
}

static inline void _IPLGetBondingId(U8 *u8Res)
{
    *u8Res = INREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x101E90));
}

static inline void IPL_main(void)
{
    U8 bondingID;
    _IPLGetBondingId(&bondingID);
    SetDebugFlag(FLAG_MAIN_ID);
    _IPLInitAllPllClk(bondingID);
    SetDebugFlag(FLAG_MAIN_DONE);
}
static inline void McuClkSrcSelect(void)
{   /*EDIE*/
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x00103802), 0x30);//switch clk_mcu from xtal(12MHz) to MPLL(432MHz)
    timer_set_divide(0x24);//timer0~2
}

static inline void ClkEarlyInit(void)
{
    SetDebugFlag(FLAG_INI_MIU_CLOCK);
    MiuBootClkSelect(1);//switch clk_miu_sc_gp from clk_boot_p(12MHz) to clk_miu
    McuClkSrcSelect();
//    BdmaClkInit();
    SetDebugFlag(FLAG_INI_SPI_CLOCK);
//    SpiClkInit();
}

static inline void pll_init(void)
{
    //mpll.txt
    SetDebugFlag(FLAG_PRE_PLL);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR,   0x00111B12), 0x00); // turn on xtal HV
    loop_delay_timer(TIMER_DELAY_100us);
    SetDebugFlag(0x1);
    OUTREG8(GET_REG8_ADDR(RIU_BASE_ADDR, 0x103003), 0x00); //turn on miupll, wait around 100us for PLL to be stable
    SetDebugFlag(0x2);
    loop_delay_timer(TIMER_DELAY_250us); //TIMER_DELAY_100us not enough
    SetDebugFlag(0x3);
}

static inline void IPL_PrecursorMain(void)
{
    SetDebugFlag(FLAG_PRE_MAIN);

    pll_init(); //0.394ms
    SetDebugFlag(FLAG_PRE_PLLDONE);//1us

    ClkEarlyInit(); //11us
//    clearGpi();
    IPL_main();
}

static u32 CurTask_SP = 0;
static u32 Reglr;

void sram_resume_imi(void)
{
    int j, i, final_status, cascade_irq=999;
    *(unsigned short volatile *) 0xFD200800 = 0x7777;
    /*Log gpi status*/
    for (j=0; j<=GPI_FIQ_NUM/16; j++)
    {
        final_status = INREG16(BASE_REG_GPI_INT_PA + REG_ID_30 + j*4);
        for(i=0; i<16 && final_status!=0; i++)
        {
            if(0 !=(final_status & (1<<i)))
            {
                cascade_irq = i + j*16;
            }
        }
    }
    *(unsigned short volatile *) 0xFD20080C = cascade_irq;

#ifndef CONFIG_SS_DIRECT_RESUME
    //    /*SW reset*/r
    *(unsigned short volatile *) 0xFD001CB8 = 0x79;
#endif

    *(unsigned short volatile *) 0xFD200800 = 0x7779;
    IPL_PrecursorMain();
    *(unsigned short volatile *) 0xFD200800 = 0x7788;

    if (pStr_info) {
        pStr_info->status = SUSPEND_WAKEUP;
    }

    asm volatile("mov lr, %0" : :"r"(Reglr));
    asm volatile("mov sp, %0" : :"r"(CurTask_SP));
//	asm("blx cpu_suspend_abort");
    asm volatile("ldmia   sp!, {r1 - r3}");//@ pop phys pgd, virt SP, phys resume fn
    asm volatile("teq r0, #0");
    asm volatile("moveq   r0, #1");     //e force non-zero value
    asm volatile("mov sp, r2");
    asm volatile("ldmfd   sp!, {r4 - r11, pc}");

//    /*SW reset*/
//    *(unsigned short volatile *) 0xFD001CB8 = 0x79;
}

static int mstar_suspend_ready(unsigned long ret)
{
    int resume_fn, stack,dummy;
    *(unsigned short volatile *) 0xFD200800 = 0x2222;

    mstar_suspend_imi_fn = fncpy(suspend_imi_vbase, (void*)&sram_suspend_imi, 0x1000);
    mstar_resume_imi_fn = fncpy(resume_imi_vbase, (void*)&sram_resume_imi, 0x2000);

    resume_fn=(u32)mstar_resume_imi_fn;
    stack=(u32)resume_imistack_vbase+0x1000;
    suspend_status = SUSPEND_SLEEP;

    //resume info
    if (pStr_info) {
        pStr_info->count++;
        pStr_info->status = SUSPEND_SLEEP;
        pStr_info->password = STR_PASSWORD;
    }
    //flush cache to ensure memory is updated before self-refresh
//    __cpuc_flush_kern_all();
    //flush L1~L3
    Chip_Flush_CacheAll();
    //flush tlb to ensure following translation is all in tlb
    local_flush_tlb_all();
    // avoid TLB misses
    dummy = *(unsigned short volatile *) stack;
    dummy = *(unsigned short volatile *) resume_fn;
    dummy = *(unsigned short volatile *) resume_fn+0x1000;

#ifdef CONFIG_SS_STRDEBUG
    pStr_info->checksum = calc_checksum(_text, __init_begin-1-_text);
#endif
    #if 0
    for(i=PAD_GPIO2; i<=PAD_GPIO12; i++){
        MDrv_GPIO_PadVal_Set(i, 0x0);
        MDrv_GPIO_Set_Low(i);
    }

    for(i=PAD_GPIO14; i<=PAD_SATA_GPIO; i++){
        MDrv_GPIO_PadVal_Set(i, 0x0);
        MDrv_GPIO_Set_Low(i);
    }

        /*clear gpi*/
    for( gpi_irq = 0 ; gpi_irq < GPI_FIQ_END ; gpi_irq++)
    {
        SETREG16( (BASE_REG_GPI_INT_PA + REG_ID_0A + (gpi_irq/16)*4 ) , (1 << (gpi_irq%16)) );
    }
    #endif
    *(unsigned short volatile *) 0xFD200800 = 0x2224;
    *(unsigned short volatile *) 0xFD200808 = (u32)resume_fn;
    *(unsigned short volatile *) 0xFD200804 = (u32)(resume_fn>>16);
    *(unsigned short volatile *) 0xFD200810 = (u32)stack;
    *(unsigned short volatile *) 0xFD20080C = (u32)(stack>>16);
    *(unsigned short volatile *) 0xFD200800 = 0x2225;

    asm volatile("mov %0, sp" : "=r"(CurTask_SP));
    asm volatile("mov %0, lr" : "=r"(Reglr));


    mstar_suspend_imi_fn(); //test
//    sram_resume_imi();
    return 0;
}

static int mstar_suspend_enter(suspend_state_t state)
{
    FIN;
    switch (state) 
    {
        case PM_SUSPEND_MEM:
            printk(KERN_INFO "state = PM_SUSPEND_MEM\n");
            cpu_suspend(0, mstar_suspend_ready);
            break;
        default:
            return -EINVAL;
    }

    return 0;
}

struct platform_suspend_ops mstar_suspend_ops = {
    .enter    = mstar_suspend_enter,
    .valid    = suspend_valid_only_mem,
};

int __init mstar_pm_init(void)
{
    unsigned int resume_pbase = virt_to_phys(cpu_resume);
    static void __iomem * suspend_info_vbase = (void *)SUSPENDINFO_ADDRESS;
    suspend_imi_vbase = __arm_ioremap_exec(0xA0001000, 0x1000, false);  //put suspend code at IMI offset 64K;
    resume_imi_vbase = __arm_ioremap_exec(0xA0002000, 0x2000, false);  //put suspend code at IMI offset 64K;
    suspend_info_vbase =  __arm_ioremap_exec(SUSPENDINFO_ADDRESS, 0x1000, false);

    resume_imistack_vbase = __arm_ioremap_exec(0xA0007000, 0x1100, false);  //put stack at IMI 0xA0008000

    pStr_info = (suspend_keep_info *)(suspend_info_vbase);
    memset(pStr_info, 0, sizeof(suspend_keep_info));
    strcpy(pStr_info->magic, "SIG_STR");
    pStr_info->resume_entry = resume_pbase;

    suspend_set_ops(&mstar_suspend_ops);
    printk(KERN_INFO "[%s] resume_pbase=0x%08X, suspend_imi_vbase=0x%08X\n", __func__, (unsigned int)resume_pbase, (unsigned int)suspend_imi_vbase);

    return 0;
}

