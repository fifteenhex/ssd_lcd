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


#ifndef _MMAP_INFO_DATATYPE_H_
#define _MMAP_INFO_DATATYPE_H_

typedef enum
{
    E_MMAP_Original = 0,
    E_MMAP_Security,
    E_MMAP_MAX
} EN_MMAP_Type;

typedef enum
{
  ERR_NONEXIST_FILE = -1,
  ERR_POINTER_OVERFLOW= -2,
  ERR_NULL_POINTER= -3,
}PARSER_Err;

typedef enum
{
  ERR_MMAP_ID = -1,
  ERR_MIU_OVERBOUND= -2,
}MIUNO_Err;

typedef enum
{
    E_MMAPCheck_CommonRule = 0,
    E_MMAPCheck_ProjectRule,
    E_MMAPCheck_MAX
} EN_MMAPCheck_Rule;


#define MMAP_PATH                      "/config/mmap.ini"
#define MMAP_PATH_S                    "/config/mmaps.ini"
#define MMAP_PATH_SECURE               "/config/mmap.aes"     //temp mmap file
#define CONFIG_PATH                    "/config/mmap"         //auto fit mmap path
#define MMAP_M_NEED                    "M_NEED"
#define MMAP_M_CANNOT_COBUFFER         "M_CANNOT_COBUFFER"
#define MMAP_M_SAME_MIU                "M_SAME_MIU"
#define MMAP_M_ORDER_ADJACENT          "M_ORDER_ADJACENT"
#define MMAP_M_ORDER                   "M_ORDER"
#define MMAP_M_LIMIT_SIZE              "M_LIMIT_SIZE"
#define MMAP_M_LIMIT_LOCATE            "M_LIMIT_LOCATE"
#define MMAP_M_GROUP_SELMIU            "GROUP_SELMIU"
#define MMAP_M_NEED_BY_COMPILE_FLAG    "M_NEED_BY_COMPILE_FLAG"
#define MMAP_TRUE 1
#define MMAP_FALSE 0
typedef unsigned char       MMAP_BOOL;
typedef unsigned char       MMAP_U8;    // 1 byte
typedef unsigned short      MMAP_U16;   // 2 bytes
typedef unsigned int        MMAP_U32;   // 4 bytes
typedef unsigned long long  MMAP_U64;   // 8 bytes

typedef signed char         MMAP_S8;    // 1 byte
typedef signed short        MMAP_S16;   // 2 bytes
typedef signed int          MMAP_S32;   // 4 bytes
typedef signed long long    MMAP_S64;   // 8 bytes


/**********************************************************************************/
/* Warning , MMAP static enumerate table only can be added from the end of last element.
 *  Please contact Ted.Chen if you wanna to modify this table
 */

#define MAX_ITEM_COUNT 810
#define MAX_NAME_LEN   100

#define MMAP_MAX      133
#define MMAP_ID_INVALID_ID      0xFFFF
#define MMAP_ID_INVALID_ADDRESS 0xFFFFFFFF

#define MMAP_ID_COUNT 100
#define MMAP_MAX_NAME_LEN 1024
#define CMDLINE_PATH "/proc/cmdline"
#define MMAP_MIU_SELECT_NO 3
#define MMAP_MIU_SELECT_GROUP 7
#define MMAP_MIU_SELECT_BIT 16

typedef struct MMInfo_s
{
    MMAP_U32    total_mem_size;
    MMAP_U32    miu0_mem_size;
    MMAP_U32    miu1_mem_size;
    MMAP_U32    miu2_mem_size;
    MMAP_U32    miu_boundary;
    MMAP_U32    miu_boundary2;
    MMAP_U32    n_mmap_items;
    MMAP_BOOL        b_is_4k_align;
} MMInfo_t;

typedef struct MMapInfo_s
{
    MMAP_U32    u32gID;
    MMAP_U32    u32Addr;
    MMAP_U32    u32Size;
    MMAP_U8     u8Layer;
    MMAP_U32    u32Align;
    MMAP_U32    u32MemoryType;
    MMAP_U32    u32MiuNo;
    MMAP_U32    u32CMAHid; 
} MMapInfo_t;

typedef struct
{
    MMInfo_t m_MMinfo;
    MMapInfo_t *m_MMtable;
} MMAPCfgBlock_t;

typedef struct
{
    char name[MAX_NAME_LEN];
    unsigned int value;
} MMAPItem;


typedef struct
{
    unsigned int key;
    char name[MAX_NAME_LEN];
} MMAPCheckItem;


typedef struct
{
    unsigned short* pu8MMAPParsedItemCount;
    MMAPItem* pstMMAPParsedItems;
    unsigned short* pu8MMAPidCount;
    MMAPItem* pstMMAPids;
} MMAPParserHandle;


#endif // _MMAP_INFO_DATATYPE_H_
