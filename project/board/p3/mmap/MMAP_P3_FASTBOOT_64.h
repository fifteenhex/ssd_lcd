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

#define SCA_TOOL_VERSION            "SN SCA V3.0.2 "

////////////////////////////////////////////////////////////////////////////////
// DRAM memory map
//
// Every Module Memory Mapping need 4 define,
// and check code in "msAPI_Memory_DumpMemoryMap"
// 1. XXX_AVAILABLE : For get avaialble memory start address
// 2. XXX_ADR       : Real Address with Alignment
// 3. XXX_GAP_CHK   : For Check Memory Gap, for avoid memory waste
// 4. XXX_LEN       : For the Memory size of this Module usage
////////////////////////////////////////////////////////////////////////////////
#define ENABLE_MIU_1                0
#define ENABLE_MIU_2                0
#define MIU_DRAM_LEN                0x0004000000
#define MIU_DRAM_LEN0               0x0004000000
#define MIU_DRAM_LEN1               0x0000000000
#define MIU_DRAM_LEN2               0x0000000000
#define MIU_INTERVAL                0x0040000000
#define CPU_ALIGN                   0x0000001000

////////////////////////////////////////////////////////////////////////////////
//MIU SETTING
////////////////////////////////////////////////////////////////////////////////
#define MIU0_GROUP_SELMIU                        0000:0000:0000:0000:0000:0000
#define MIU0_GROUP_PRIORITY                        1:0:2:3
#define MIU1_GROUP_SELMIU                        5016:0201:1280:80B8:0004:F61F
#define MIU1_GROUP_PRIORITY                        1:0:2:3
#define MIU2_GROUP_SELMIU                        0000:0000:0000:0000:0000:0000
#define MIU2_GROUP_PRIORITY                        0:0:0:0
////////////////////////////////////////////////////////////////////////////////
//MEMORY TYPE
////////////////////////////////////////////////////////////////////////////////
#define MIU0                        (0x0000)
#define MIU1                        (0x0001)
#define MIU2                        (0x0002)

#define TYPE_NONE                   (0x0000 << 2)

#define UNCACHE                     (0x0001 << 2)
#define REMAPPING_TO_USER_SPACE     (0x0002 << 2)
#define CACHE                       (0x0004 << 2)
#define NONCACHE_BUFFERABLE         (0x0008 << 2)


#define CMA                         (0x0010 << 2)
//MIU_0_START
/* E_LX_MEM   */
#define E_LX_MEM_LAYER                                         0
#define E_LX_MEM_AVAILABLE                                     0x0000000000
#define E_LX_MEM_ADR                                           0x0000000000  //Alignment 0x01000
#define E_LX_MEM_GAP_CHK                                       0x0000000000
#define E_LX_MEM_LEN                                           0x0003FE0000
#define E_LX_MEM_MEMORY_TYPE                                   (MIU0 | TYPE_NONE | UNCACHE | TYPE_NONE)
#define E_LX_MEM_CMA_HID                                       0

/* E_MMAP_ID_DUMMY1   */
#define E_MMAP_ID_DUMMY1_LAYER                                 1
#define E_MMAP_ID_DUMMY1_AVAILABLE                             0x0000000000
#define E_MMAP_ID_DUMMY1_ADR                                   0x0000000000  //Alignment 0
#define E_MMAP_ID_DUMMY1_GAP_CHK                               0x0000000000
#define E_MMAP_ID_DUMMY1_LEN                                   0x0003C00000
#define E_MMAP_ID_DUMMY1_MEMORY_TYPE                           (MIU0 | TYPE_NONE | UNCACHE | TYPE_NONE)
#define E_MMAP_ID_DUMMY1_CMA_HID                               0

/* E_MMAP_ID_EMI   */
#define E_MMAP_ID_EMI_LAYER                                    0
#define E_MMAP_ID_EMI_AVAILABLE                                0x0003FE0000
#define E_MMAP_ID_EMI_ADR                                      0x0003FE0000  //Alignment 0x01000
#define E_MMAP_ID_EMI_GAP_CHK                                  0x0000000000
#define E_MMAP_ID_EMI_LEN                                      0x0000020000
#define E_MMAP_ID_EMI_MEMORY_TYPE                              (MIU0 | TYPE_NONE | UNCACHE | TYPE_NONE)
#define E_MMAP_ID_EMI_CMA_HID                                  0

//MIU_1_START

#define MIU0_END_ADR                                           0x0004000000
#define MMAP_COUNT                                             0x0000000004

#define TEST_4K_ALIGN                   1

/* CHK_VALUE = 3901739293 */
