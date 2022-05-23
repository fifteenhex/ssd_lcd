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
#include <unistd.h>
#include "fb_common.h"

int main(int argc, char *argv[])
{
    char *devfile = "/dev/fb0";

    if (argc >= 2)
        devfile = argv[1];

    struct fb_var_screeninfo vinfo;
    fb_Tc_Init(devfile, 1);

    fb_Tc_Print_Fix_Info();
    fb_Tc_Print_Var_Info();
    fb_Tc_Get_Var_Info(&vinfo);

    int click = 1;

    int i;
    for (i = 1; i < vinfo.xres / 10; ++i)
    {
        fb_Tc_Begin_Batch_Draw();
        fb_Tc_Fill_Rect(10 * i, 0, 10, vinfo.yres, 0xffff0000);
        fb_Tc_Fill_Rect(10 * i - 10, 0, 10, vinfo.yres, 0xffffffff);
        fb_Tc_End_Batch_Draw();
        usleep(30000);
    }
    fb_Tc_Fill_Rect(10 * i - 10, 0, 10, vinfo.yres, 0xffffffff);

    for (i = 1; i < vinfo.yres / 10; ++i)
    {
        fb_Tc_Begin_Batch_Draw();
        fb_Tc_Fill_Rect(0, 10 * i, vinfo.xres ,10, 0xff00ff00);
        fb_Tc_Fill_Rect(0, 10 * i - 10, vinfo.xres, 10, 0xffffffff);
        fb_Tc_End_Batch_Draw();
        usleep(30000);
    }
    fb_Tc_Fill_Rect(0, 10 * i - 10, vinfo.xres, 10, 0xffffffff);

    fb_Tc_Deinit();
    return 0;
}

