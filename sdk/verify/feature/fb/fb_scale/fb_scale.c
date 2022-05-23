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
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <linux/kd.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <string.h>
#include <errno.h>

#include "fb_common.h"

void Wait(int val)
{
    if (val <= 0)
    {
        printf("Press 'Enter' to continue...");
        getchar();
    }
    else
    {
        printf("Action will continue after %ds\n", val);
        sleep(val);
    }
}

int main(int argc, char *argv[])
{
    int fd;
    //char buf[1024*600*4];
    char buf[800*480*4];

    fb_Tc_Init("/dev/fb0", 0);

    fd = open("pic_800x480_argb8888.raw", O_RDONLY);

    read(fd, buf, sizeof(buf));
    close(fd);

    fb_Tc_Fill_Buffer(buf, 800, 480);

    fb_Tc_Deinit();
    return 0;
}

