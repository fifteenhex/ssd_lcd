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
#include <time.h>

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

struct timespec ts1, ts2;
double posix_dur;
#define JDEC_PERF(ts1, ts2,cnt)                                                                                \
    {                                                                                                      \
        static int count = cnt;                                                                            \
        static double dur = 0.0;                                                                           \
        clock_gettime(CLOCK_MONOTONIC, &ts2);                                                     \
        posix_dur = 1000.0 * ts2.tv_sec + 1e-6 * ts2.tv_nsec - (1000.0 * ts1.tv_sec + 1e-6 * ts1.tv_nsec); \
        if (unlikely(dur - 0.0 < 0.001))                                                                   \
        {                                                                                                  \
            dur = posix_dur;                                                                               \
        }                                                                                                  \
        else                                                                                               \
        {                                                                                                  \
            dur = (dur + posix_dur) / 2;                                                                   \
        }                                                                                                  \
        count--;                                                                                           \
        if (unlikely(count == 0))                                                                          \
        {                                                                                                  \
            printf("CPU time used (%s->%d): %.2f ms\n", __FUNCTION__,__LINE__, dur);                       \
            count = cnt;                                                                                   \
            dur = 0.0;                                                                                     \
        }                                                                                                  \
        clock_gettime(CLOCK_MONOTONIC, &ts1);                                                     \
    }

