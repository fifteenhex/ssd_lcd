#include <../drivers/mstar/gpio/drvGPIO.h>
#include <common.h>
#define SPI_BITS_PER_WORDS    9
#define SPI_PANEL_DEBUG_EN        0
#define ms_delay(x)                   udelay(x*1000)

#if (CONFIG_SPI_CMD_ST7789V3)
#define LSCE_GPIO_PIN   (0)
#define LSCK_GPIO_PIN   (1)
#define LSDA_GPIO_PIN   (2)
#define LSRST_GPIO_PIN   (4)
u16 gu16CmdBuffer[] = {
0x11,0xffff,0x78,
0x36, 0x100,
0x3A, 0x155,
0xB0,0x111,0x1F4,
0xB1,0x1E0,0x104,0x114,
0xB2,0x10c,0x10c,0x100,0x133,0x133,
0xB7,0x175,
0xBB,0x11D,
0xC0,0x12c,
0xC2,0x101,
0xC3,0x10b,
0xC4,0x120,
0xC6,0x10F,
0xD0,0x1A4,0x1A1,
0xD6, 0x1A1,
0xE0,0x1F0,0x105,0x110,0x113,0x114,0x12D,0x138,0x144,0x148,0x10B,0x114,0x111,0x119,0x11C,
0xE1,0x1F0,0x103,0x10B,0x10B,0x10B,0x126,0x138,0x154,0x148,0x13D,0x119,0x119,0x11B,0x11E,
0x29,0xffff,0x05,
0x2c,0xffff,0x78,
};
#else
#define LSCE_GPIO_PIN   (0)
#define LSCK_GPIO_PIN   (1)
#define LSDA_GPIO_PIN   (2)
#define LSRST_GPIO_PIN   (4)
u16 * gu16CmdBuffer = NULL;
#endif

#if (defined (SPI_PANEL_DEBUG_EN) && (SPI_PANEL_DEBUG_EN == 1))
#define SPI_PANEL_DEBUG(_fmt, _args...)   printf(_fmt, ## _args)
#else
#define SPI_PANEL_DEBUG(_fmt, _args...)
#endif

static void _spi_gpio_init(MS_GPIO_NUM gpio_num)
{
    MDrv_GPIO_Pad_Set(gpio_num);
    MDrv_GPIO_Pad_Oen(gpio_num);
    MDrv_GPIO_Pull_High(gpio_num);
    mdrv_gpio_set_high(gpio_num);
}
static void spi_gpio_init()
{
    _spi_gpio_init(LSCE_GPIO_PIN);
    _spi_gpio_init(LSCK_GPIO_PIN);
    _spi_gpio_init(LSDA_GPIO_PIN);
    _spi_gpio_init(LSRST_GPIO_PIN);
}
void _drv_set_RST(unsigned long index)
{
    if(index)
    {
        mdrv_gpio_set_high(LSRST_GPIO_PIN);
    }
    else
    {
        mdrv_gpio_set_low(LSRST_GPIO_PIN);
    }
}
void _drv_set_CS(unsigned long index)
{
    if(index)
    {
        mdrv_gpio_set_high(LSCE_GPIO_PIN);
    }
    else
    {
        mdrv_gpio_set_low(LSCE_GPIO_PIN);
    }
}
void _drv_set_SCL(unsigned long index)
{
    if(index)
    {
        mdrv_gpio_set_high(LSCK_GPIO_PIN);
    }
    else
    {
        mdrv_gpio_set_low(LSCK_GPIO_PIN);
    }
}
void _drv_set_SDI(unsigned long index)
{
    if(index)
    {
        mdrv_gpio_set_high(LSDA_GPIO_PIN);
    }
    else
    {
        mdrv_gpio_set_low(LSDA_GPIO_PIN);
    }
}
static void spi_init()
{
    spi_gpio_init();
    _drv_set_RST(1);
    ms_delay(20);
    _drv_set_RST(0);
    ms_delay(20);
    _drv_set_RST(1);
    ms_delay(120);

    _drv_set_CS(1);
    _drv_set_SCL(0);
    _drv_set_SDI(1);
}
static void spi_deinit()
{
    return ;
}

static void _spi_send_cmd(u16 cmd)
{
    unsigned int i;
    _drv_set_CS(1);
    udelay(1);
    _drv_set_CS(0);
    _drv_set_SDI(1);
    _drv_set_SCL(0);
    udelay(1);
    for(i=0;i<SPI_BITS_PER_WORDS;i++)
    {
        udelay(10);
        if(cmd & (1<<(SPI_BITS_PER_WORDS-1-i)))
            _drv_set_SDI(1);
        else
            _drv_set_SDI(0);
        _drv_set_SCL(0);
        udelay(3);
        _drv_set_SCL(1);
    }
    udelay(10);
    _drv_set_CS(1);
    _drv_set_SCL(0);
    udelay(5);
    return ;
}

void spi_cmd_init()
{
    u32 cmd = 0;
    u32 u32CmdCount = 0;
    u16 delay_time = 0;
    if(gu16CmdBuffer == NULL)
        return;
    u32CmdCount = sizeof(gu16CmdBuffer)/sizeof(gu16CmdBuffer[0]);
    spi_init();
    for(cmd = 0; cmd < u32CmdCount; cmd++)
    {
        if(gu16CmdBuffer[cmd] == 0xffffu)
        {
            delay_time = gu16CmdBuffer[++cmd];
            udelay(delay_time*1000);
            SPI_PANEL_DEBUG("delay:%d ms\n", delay_time);
        }
        else
        {
            _spi_send_cmd(gu16CmdBuffer[cmd]);
            SPI_PANEL_DEBUG("send cmd:0x%x\n", gu16CmdBuffer[cmd]);
        }
    }
    spi_deinit();
    return ;
}
