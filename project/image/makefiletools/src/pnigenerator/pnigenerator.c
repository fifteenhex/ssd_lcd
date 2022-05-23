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

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

static char **p_opt_argv_strings = NULL;
static u32 opt_strings_count = 0;

typedef struct SPI_NAND_PARTITION_RECORD_T
{
    u8 szPartName[16];
    u8 u8_Trunk;
    u8 u8_BackTrunk;
    u8 u8_Active;
    u8 u8_Group;
    u32 u32_Offset;
    u32 u32_Size;
    u32 u32_Reserved;
} SPI_NAND_PARTITION_RECORD_t;

typedef struct SpinandPartitionInfo_T
{
    u8 szMagic[16];
    u32 u32_Checksum;
    u32 u32_size;
    SPI_NAND_PARTITION_RECORD_t records[62];
} SpinandPartitionInfo_t;
static u32 blk_size = 0x20000;
static u16 u16BlkCnt = 1024;
static u32 _checkSum(u8 *pData, u16 nByteCnt)
{
    u32 nSum = 0;

    while(nByteCnt--)
        nSum += *pData++;

    return nSum;
}
static u8 grab_bbm_config(const char *partition_name)
{
    char bbm_config_on_str[32];
    char bbm_config_off_str[32];
    u32 i = 0;

    snprintf(bbm_config_on_str, 32, "%s_bbm=on", partition_name);
    snprintf(bbm_config_off_str, 32, "%s_bbm=off", partition_name);
    for (i = 0; i < opt_strings_count; i++)
    {
        if (!strcasecmp(bbm_config_on_str, p_opt_argv_strings[i]))
            return 1;
        if (!strcasecmp(bbm_config_off_str, p_opt_argv_strings[i]))
            return 0;
    }

    return 0;
}
static u32 parse_partition_section(const char *section, u32 start_partion_addr, void *usr_data, u32 *p_i, u8 u8Tunnk, u8 u8BackTrunk, u8 bActive, u8 bCheck)
{
    char partition_name[30];
    u32 partition_size = 0;
    char *temp_pos = NULL;
    u32 start_addr = 0;
    SpinandPartitionInfo_t *pstPni = (SpinandPartitionInfo_t *)usr_data;

    if (*p_i == 62)
        return start_partion_addr;

    /*Exp: "0x140000(CIS),0x20000@0x140000(IPL0)2,0x20000(IPL1)2,0x20000(IPL_CUST0)2,0x20000(IPL_CUST1)2,0x40000(UBOOT0)1,0x40000(UBOOT1)1,0x60000(ENV)"

           "0x20000(KEY_CUST),0x500000(KERNEL),0x500000(RECOVERY),0x600000(rootfs),0x60000(MISC),-(UBI)"*/

    memset(partition_name, 0, 30);
    start_addr = start_partion_addr;
    if (*section == '-')
    {
        partition_size = u16BlkCnt * blk_size - start_addr;
        section += 2;
        temp_pos = strstr(section, ")");
        if (temp_pos != NULL)
        {
            *temp_pos = 0;
        }
        strcpy(partition_name, section);
    }
    else
    {
        temp_pos = strstr(section, "@");
        if (temp_pos)
        {
            temp_pos++;
            sscanf(section, "0x%x@0x%x(%s)", &partition_size, &start_addr, partition_name);
            //printf("SIZE 0x%x ADDR 0x%x Name %s\n", partition_size, start_addr, partition_name);
        }
        else
        {
            sscanf(section, "0x%x(%s)", &partition_size, partition_name);
        }
        temp_pos = strstr(partition_name, ")");
        if (temp_pos != NULL)
        {
            *temp_pos = 0;
        }
    }
    if (start_addr >= u16BlkCnt * blk_size)
    {
        return start_addr;
    }
    if (bCheck && partition_size % blk_size != 0)
    {
        fprintf(stderr, "\033[5;41;33m[%s]Block Size 0x%X Not Align to 0x%X!\033[0m\n", partition_name, partition_size, blk_size);
        exit(1);
    }
    pstPni->records[*p_i].u8_Trunk = u8Tunnk;
    pstPni->records[*p_i].u8_BackTrunk = u8BackTrunk;
    pstPni->records[*p_i].u8_Active = bActive;
    pstPni->records[*p_i].u32_Offset = start_addr;
    pstPni->records[*p_i].u32_Size = partition_size;
    pstPni->records[*p_i].u32_Reserved = !grab_bbm_config(partition_name);
    memcpy(pstPni->records[*p_i].szPartName, partition_name, 16);
    //printf("name: %s start 0x%x size 0x%x idx %d\n",partition_name, start_partion_addr, partition_size, *p_i);
    (*p_i)++;

    return partition_size + start_addr;
}
static u32 parse_partition(const char *string, u32 start_partion_addr, void *usr_data, u32 *p_i, u8 u8Tunnk, u8 u8BackTrunk, u8 bActive, u8 bCheck)
{
    char partition_section[50];
    const char *p_section_start = NULL;
    const char *p_section_end = NULL;

    if (!string)
    {
        return start_partion_addr;
    }
    p_section_start = string;
    while(1)
    {
        memset(partition_section, 0, 50);
        p_section_end = strstr(p_section_start, ",");
        if (p_section_end == NULL)
        {
            strcpy(partition_section, p_section_start);
            start_partion_addr = parse_partition_section(partition_section, start_partion_addr, usr_data, p_i, u8Tunnk, u8BackTrunk, bActive, bCheck);
            break;
        }
        strncpy(partition_section, p_section_start, p_section_end - p_section_start);
        start_partion_addr = parse_partition_section(partition_section, start_partion_addr, usr_data, p_i, u8Tunnk, u8BackTrunk, bActive, bCheck);
        p_section_start = p_section_end + 1;
    }

    return  start_partion_addr;
}
u32 easy_atoi(const char *pStr)
{
    u32 intStrLen = strlen(pStr);
    u16 bUseHex = 0;
    u32 intRetNumber = 0;

    if (pStr == NULL)
    {
        return 0xFFFFFFFF;
    }

    if (intStrLen > 2)
    {
        if (pStr[0] == '0' &&(pStr[1] == 'X' || pStr[1] == 'x'))
        {
            bUseHex = 1;
            pStr += 2;
        }
    }
    if (bUseHex == 1)
    {
        for (int i = 0; i < intStrLen - 2; i++)
        {
            if ((pStr[i] > '9' || pStr[i] < '0')    \
                && (pStr[i] > 'f' || pStr[i] < 'a') \
                && (pStr[i] > 'F' || pStr[i] < 'A'))
            {
                return 0xFFFFFFFF;
            }
        }
        sscanf(pStr, "%x", &intRetNumber);
    }
    else
    {
        for (int i = 0; i < intStrLen; i++)
        {
            if (pStr[i] > '9' || pStr[i] < '0')
            {
                return 0xFFFFFFFF;
            }
        }
        intRetNumber =  atoi(pStr);
    }
    return intRetNumber;
}
u8 SstarMagic[16] = {0x53, 0x53, 0x54, 0x41, 0x52, 0x53, 0x45, 0x4D, 0x49, 0x43, 0x49, 0x53, 0x30, 0x30, 0x30, 0x31};
int main(int argc, char **argv)
{
    SpinandPartitionInfo_t stPni;
    u32 i = 0;
    int fd = 0;
    int result;
    u8 is_write = 0;
    u8 is_read = 0;
    char *file_name = NULL;
    char *boot_part0 = NULL;
    char *boot_part1 = NULL;
    char *sys_part = NULL;
    u32 start_partion_addr = 0;
    u32 u32CheckSum = 0;

    memset(&stPni, 0, sizeof(SpinandPartitionInfo_t));

    while ((result = getopt(argc, argv, "a:b:c:r:o:s:t:")) != -1)
    {
        switch (result)
        {
            case 'c':
            {
                u16BlkCnt = (u16)easy_atoi(optarg);
            }
            break;
            case 's':
            {
                blk_size = easy_atoi(optarg);
            }
            break;
            case 'a':
            {
                if (strlen(optarg) != 0)
                {
                    boot_part0 = (char *)malloc(strlen(optarg) + 1);
                    assert(boot_part0);
                    strcpy(boot_part0, optarg);
                    printf("BOOT0: %s\n", boot_part0);
                    is_write = 1;
                }
            }
            break;
            case 'b':
            {
                if (strlen(optarg) != 0)
                {
                    boot_part1 = (char *)malloc(strlen(optarg) + 1);
                    assert(boot_part1);
                    strcpy(boot_part1, optarg);
                    printf("BOOT1: %s\n", boot_part1);
                    is_write = 1;
                }
            }
            break;
            case 't':
            {
                if (strlen(optarg) != 0)
                {
                    sys_part = (char *)malloc(strlen(optarg) + 1);
                    assert(sys_part);
                    strcpy(sys_part, optarg);
                    printf("SYS: %s\n", sys_part);
                    is_write = 1;
                }
            }
            break;
            case 'r':
            {

                file_name = (char *)malloc(strlen(optarg) + 1);
                assert(file_name);
                strcpy(file_name, optarg);
                is_read = 1;
            }
            break;
            case 'o':
            {
                file_name = (char *)malloc(strlen(optarg) + 1);
                assert(file_name);
                strcpy(file_name, optarg);
                is_write = 1;
            }
            break;
            default:
                break;
        }
    }
    opt_strings_count = argc - optind;
    if (opt_strings_count)
        p_opt_argv_strings = &argv[optind];

    if (is_read && is_write && (!strlen(file_name)))
    {
        printf("ERROR OPT!\n");

        goto ERR_NOFD;
    }
    if (is_write && file_name)
    {
        fd = open(file_name, O_CREAT | O_RDWR | O_TRUNC, 0777);
        if (-1 == fd)
        {
            perror("open");
            goto ERR_NOFD;
        }
        if (boot_part0)
            start_partion_addr = parse_partition(boot_part0, start_partion_addr, (void *)&stPni, &i, 0, (boot_part1 != NULL), 1, 0);
        if (boot_part1)
            start_partion_addr = parse_partition(boot_part1, start_partion_addr, (void *)&stPni, &i, 1, 0, 0, 0);
        if (sys_part)
        {
            start_partion_addr = parse_partition(sys_part, start_partion_addr, (void *)&stPni, &i, 0, 0, 1, 1);
        }
        memcpy(stPni.szMagic, SstarMagic, 16);
        stPni.u32_size = sizeof(SPI_NAND_PARTITION_RECORD_t) * i;
        stPni.u32_Checksum = _checkSum((u8*)&(stPni.u32_size), sizeof(u32));
        stPni.u32_Checksum += _checkSum((u8*)stPni.records, stPni.u32_size);
        write(fd, &stPni, sizeof(SpinandPartitionInfo_t));
        printf("FLASH HAS USED 0x%xKB\n", start_partion_addr);
    }
    else if (is_read && file_name)
    {
        fd = open(file_name, O_RDONLY);
        if (-1 == fd)
        {
            perror("open");
            goto ERR_NOFD;
        }
        read(fd, &stPni, sizeof(SpinandPartitionInfo_t));
    }
    printf("ChkSum       : %d\n", stPni.u32_Checksum);
    printf("Magic        : ");
    for (i = 0; i < 16; i++)
        putchar(stPni.szMagic[i]);
    putchar('\n');
    u32CheckSum = _checkSum((u8*)&(stPni.u32_size), sizeof(u32));
    u32CheckSum += _checkSum((u8*)stPni.records, stPni.u32_size);
    if (u32CheckSum == stPni.u32_Checksum)
    {
        printf("Checksum ok!!\n");
    }
    else
    {
        printf("Checksum error!\n");

        goto ERR;
    }
    printf("IDX: %17s %17s %18s %11s %8s %10s\n", "StartBlk:", "BlkCnt:", "Trunk/BkTrunk:", "Active:", "BBM:", "Name:");
    for (i = 0; i < stPni.u32_size / sizeof(SPI_NAND_PARTITION_RECORD_t); i++)
    {
        printf("%3d: %4d,(%#010X) %4d,(%#010X) %16d/%d %11d %8s %10s\n", i,
                stPni.records[i].u32_Offset / blk_size, stPni.records[i].u32_Offset, stPni.records[i].u32_Size / blk_size, stPni.records[i].u32_Size,
                stPni.records[i].u8_Trunk, stPni.records[i].u8_BackTrunk, stPni.records[i].u8_Active, stPni.records[i].u32_Reserved ? "OFF": "ON", stPni.records[i].szPartName);
    }

ERR:
    close(fd);
ERR_NOFD:
    if (boot_part0)
        free(boot_part0);
    if (boot_part1)
        free(boot_part1);
    if (sys_part)
        free(sys_part);
    if (file_name)
        free(file_name);

    return 0;
}
