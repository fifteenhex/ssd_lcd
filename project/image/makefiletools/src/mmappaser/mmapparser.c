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

// headers of itself
#include "mmapparser.h"

// headers of standard C libs
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MMAP_PARSER_ERR(fmt, arg...)          printf((char *)fmt, ##arg)
#define MMAP_PARSER_DBG(fmt, arg...)          //printf((char *)fmt, ##arg)
MMAP_BOOL AddMMIPid(const char*, MMAPParserHandle*);
MMAP_BOOL AddToTable(const char*, MMAP_U32, MMAPParserHandle*);
MMAP_U32 GetValueFromTable(const char*, MMAPParserHandle*);
MMAP_U32 DoCalculate_Logic_Or(char*, MMAPParserHandle*);
MMAP_U32 DoCalculate_Shift(const char*);

MMAP_S32 MMAPFileParser(const char* filePath, MMAPParserHandle* handle,char* content)
{

    PARSER_Err errno;

    char tempStr[128];
    char* sym_s = NULL;
    MMAP_BOOL bSecureMMAP=MMAP_TRUE;

    if(!(handle->pu8MMAPParsedItemCount && handle->pstMMAPParsedItems))
    {
        MMAP_PARSER_ERR("Error, input point is null... [%s %s %d]\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
        errno = ERR_NULL_POINTER;
        return errno;
    }

    if((content==NULL)||(strlen(content)==0))
    {
        FILE* fp=NULL;
        fp = fopen(filePath, "r");
        if(!fp)
        {
            MMAP_PARSER_ERR("Warning:Open the MMAP [%s] header file failed... [%s %s %d]\n", filePath, __FILE__, __PRETTY_FUNCTION__, __LINE__);
            errno = ERR_NONEXIST_FILE;
            return errno;
        }
        while(fgets(tempStr, sizeof(tempStr), fp))
        {
            sym_s = strstr(tempStr, "#define");
            if(NULL != sym_s)
            {
                bSecureMMAP=MMAP_FALSE;
                break;
            }

        }
        fclose(fp);
        if(bSecureMMAP==MMAP_TRUE)
        {
            MMAP_PARSER_ERR("**********************************************************************\n");
            MMAP_PARSER_ERR("Need Call mapi_secure::DoMmapFileDecryption\n");
            MMAP_PARSER_ERR("MMAP File content is %s!!!\n",content);
            MMAP_PARSER_ERR("**********************************************************************\n");
            assert(0);
        }
    }
    if((content!=NULL)&&(strstr(content, "#define")!=NULL)) //Encrypt MMAP
    {
        char*  cStr=NULL;
        int count=0,i;
        char mmapItems[MAX_ITEM_COUNT][128];
        cStr=strtok(content,"\n");
        while(cStr!=NULL)
        {

            memcpy(tempStr,cStr,sizeof(tempStr));
            sym_s = strstr(tempStr, "#define");
             if(NULL != sym_s)
            {
                memcpy(mmapItems[count],tempStr,sizeof(tempStr));
                count++;
            }
            cStr=strtok(NULL,"\n");
        }
        for (i=0; i<count; i++)
        {
            memset(tempStr,0,sizeof(tempStr));
            memcpy(tempStr,mmapItems[i],sizeof(mmapItems[i]));
            Parser(handle,tempStr);
        }
    }
    else  //Un Encrypt MMAP
    {
        FILE* fp=NULL;
        fp = fopen(filePath, "r");
        if(!fp)
        {
            MMAP_PARSER_ERR("Warning:Open the MMAP [%s] header file failed... [%s %s %d]\n", filePath, __FILE__, __PRETTY_FUNCTION__, __LINE__);
            errno = ERR_NONEXIST_FILE;
            return errno;
        }
        while(fgets(tempStr, sizeof(tempStr), fp))
        {
            sym_s = strstr(tempStr, "#define");
            //not define case, then continue
            if(NULL == sym_s)
            {
                continue;
            }
            Parser(handle,tempStr);
        }
        fclose(fp);
    }

    return MMAP_TRUE;
}
MMAP_S32 Parser(MMAPParserHandle* handle,const char* Str)
{
    //symbol_start_ptr
    char* sym_s = NULL;
    //symbol_end_ptr
    char* sym_e = NULL;
    //value_ptr
    char* value = NULL;

    char tempStr[128];

    char *align_value=NULL;
    char align_name[80]="";
    PARSER_Err errno;

    memcpy(tempStr,Str,sizeof(tempStr));
    sym_s = strstr(tempStr, "#define");
    //remove "define "
    if (((MMAP_S32)(sym_s + strlen("#define") - tempStr) >= (MMAP_S32)sizeof(tempStr)) || ((MMAP_S32)(sym_s + strlen("#define") - tempStr) < 0))
    {
        MMAP_PARSER_ERR("Error, parser pointer overflow\n");
        errno = ERR_POINTER_OVERFLOW;
        return errno;
    }
    else
    {
        if (((sym_s + strlen("#define")) >= tempStr) && ((sym_s + strlen("#define")) < (tempStr + sizeof(tempStr))))
        {
            sym_s += strlen("#define");
        }
        else
        {
            MMAP_PARSER_ERR("Error, parser pointer overflow\n");
            errno = ERR_POINTER_OVERFLOW;
            return errno;
        }
    }

    if (((MMAP_S32)(sym_s + strspn(sym_s, " \t") - tempStr) >= (MMAP_S32)sizeof(tempStr)) || ((MMAP_S32)(sym_s + strspn(sym_s, " \t") - tempStr) < 0))
    {
        MMAP_PARSER_ERR("Error, parser pointer overflow\n");
        errno = ERR_POINTER_OVERFLOW;
        return errno;
    }
    else
    {
        sym_s += strspn(sym_s, " \t");
    }

    if (((MMAP_S32)(sym_s + strcspn(sym_s, " \t()") - tempStr) >= (MMAP_S32)sizeof(tempStr)) || ((MMAP_S32)(sym_s + strcspn(sym_s, " \t()") - tempStr) < 0))
    {
        MMAP_PARSER_ERR("Error, parser pointer overflow\n");
        errno = ERR_POINTER_OVERFLOW;
        return errno;
    }
    else
    {
        sym_e = sym_s + strcspn(sym_s, " \t()");
        if ((sym_e >= tempStr) && (sym_e < (tempStr + sizeof(tempStr))))
        {
            memcpy(sym_e, "\0", 1);
        }
        else
        {
            MMAP_PARSER_ERR("Error, parser pointer overflow\n");
            errno = ERR_POINTER_OVERFLOW;
            return errno;
        }
    }

    if (((MMAP_S32)((sym_e + 1) + strspn(sym_e + 1, " \t") - tempStr) >= (MMAP_S32)sizeof(tempStr)) || ((MMAP_S32)((sym_e + 1) + strspn(sym_e + 1, " \t") - tempStr) < 0))
    {
        MMAP_PARSER_ERR("Error, parser pointer overflow\n");
        errno = ERR_POINTER_OVERFLOW;
        return errno;
    }
    else
    {
        value = (sym_e + 1) + strspn(sym_e + 1, " \t");
    }

    if ((sym_s >= tempStr) && (sym_s < (tempStr + sizeof(tempStr))))
    {
        if(('\0' == *sym_s) || ('\0' == *value))
        {
            errno = ERR_NULL_POINTER;
            return errno;
        }
    }
    else
    {
        MMAP_PARSER_ERR("Error, parser pointer overflow\n");
        errno = ERR_POINTER_OVERFLOW;
        return errno;
    }

    char *brackets_left = strchr(value, '(');
    char *brackets_right = strchr(value, ')');
    if((brackets_left == NULL) && (brackets_right == NULL))
    {
        //remove "//....."
        char *tempChar;
        if (((MMAP_S32)(value + strcspn(value, " \t/") - tempStr) >= (MMAP_S32)sizeof(tempStr)) || ((MMAP_S32)(value + strcspn(value, " \t/") - tempStr) < 0))
        {
            MMAP_PARSER_ERR("Error, parser pointer overflow\n");
            errno = ERR_POINTER_OVERFLOW;
            return errno;
        }
        else
        {
            tempChar = value + strcspn(value, " \t/");
            if(tempChar != value)
            {
                if(strstr(tempChar, "Alignment")!=NULL)
                {
                        char*  rest_of_str = NULL;
                        align_value=strtok_r(tempChar,"//Alignment ",&rest_of_str);
                }
                *tempChar = '\0';
            }
        }
    }
    else
    {
        //remove "(" & ")"
        value = brackets_left + 1;
        *brackets_right = '\0';
    }

    if(Is_StrFullCmp("MIU_DRAM_LEN", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("MIU_DRAM_LEN0", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("MIU_DRAM_LEN1", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("MIU_DRAM_LEN2", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("ENABLE_MIU_1", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("ENABLE_MIU_2", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("MIU_INTERVAL", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("MIU_INTERVAL2", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("MIU0", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("MIU1", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("MIU2", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("TYPE_NONE", sym_s))
    {
        AddToTable(sym_s, DoCalculate_Shift(value), handle);
    }
    else if(Is_StrFullCmp("UNCACHE", sym_s))
    {
        AddToTable(sym_s, DoCalculate_Shift(value), handle);
    }
    else if(Is_StrFullCmp("CACHE", sym_s))
    {
        AddToTable(sym_s, DoCalculate_Shift(value), handle);
    }
    else if(Is_StrFullCmp("REMAPPING_TO_USER_SPACE", sym_s))
    {
        AddToTable(sym_s, DoCalculate_Shift(value), handle);
    }
    else if(Is_StrFullCmp("CMA", sym_s))
    {
        AddToTable(sym_s, DoCalculate_Shift(value), handle);
    }
    else if(Is_StrFullCmp("MIU0_END_ADR", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("MIU1_END_ADR", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("MIU2_END_ADR", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("MMAP_COUNT", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(Is_StrFullCmp("TEST_4K_ALIGN", sym_s))
    {
        AddToTable(sym_s, strtoul(value, 0, 0), handle);
    }
    else if(strstr(sym_s,MMAP_M_GROUP_SELMIU))
    {
        char* GroupValue=NULL;
        char count[2]={0};
        MMAP_S32 i=0;
            char*  rest_of_str = NULL;
            GroupValue=strtok_r(value,":",&rest_of_str);
        while(GroupValue!=NULL)
        {
            char GroupID[MMAP_MAX_NAME_LEN]={0};
            count[0]='\0';
            strncpy(GroupID, sym_s, strlen(sym_s));
            sprintf(count, "%d", i);
            strncat(GroupID, count, 1);
            AddToTable(GroupID, strtoul(GroupValue, 0, 16), handle);

            i++;
                GroupValue = strtok_r(rest_of_str,":",&rest_of_str);
        }
    }
    else
    {
        if(0 == memcmp("_LAYER", sym_e - 6, 6))
        {
            AddToTable(sym_s, strtoul(value, 0, 0), handle);
        }
        else if(0 == memcmp("_ADR", sym_e - 4, 4))
        {
            strncpy(align_name, sym_s, strlen(sym_s)-4 );
            strcat(align_name, "_ALIGN");
            AddToTable(align_name, strtoul(align_value, 0, 0), handle);
            AddToTable(sym_s, strtoul(value, 0, 0), handle);
        }
        else if(0 == memcmp("_LEN", sym_e - 4, 4))
        {
            MMAP_U32 tempvalue = strtoul(value, 0, 0);
            AddToTable(sym_s, tempvalue, handle);

            if ( 0 <= tempvalue )
            {
                MMAP_U32 templength = (sym_e - 4)-sym_s+1;
                if ( templength < MAX_NAME_LEN )
                {
                    char tempname[MAX_NAME_LEN];
                    memset(tempname, 0, MAX_NAME_LEN);
                    memcpy(tempname, sym_s, templength-1);
                    AddMMIPid(tempname, handle);
                }
            }
        }
        else if(0 == memcmp("_MEMORY_TYPE", sym_e - 12, 12))
        {
            AddToTable(sym_s, DoCalculate_Logic_Or(value, handle), handle);
        }
        else if(0 == memcmp("_CMA_HID", sym_e - 8, 8))
        {
            AddToTable(sym_s, strtoul(value, 0, 0), handle);
        }
    }
    return MMAP_TRUE;
}
MMAP_BOOL AddMMIPid(const char *name, MMAPParserHandle* handle)
{
    if(*(handle->pu8MMAPidCount) < MAX_ITEM_COUNT)
    {
        memcpy((handle->pstMMAPids + *(handle->pu8MMAPidCount))->name, name, strlen(name));
        (handle->pstMMAPids + *(handle->pu8MMAPidCount))->value = 0;
        ++(*(handle->pu8MMAPidCount));
        return MMAP_TRUE;
    }
    return MMAP_FALSE;
}

MMAP_BOOL AddToTable(const char *name, MMAP_U32 value, MMAPParserHandle* handle)
{
    if(*(handle->pu8MMAPParsedItemCount) < MAX_ITEM_COUNT)
    {
        memcpy((handle->pstMMAPParsedItems + *(handle->pu8MMAPParsedItemCount))->name, name, strlen(name));
        memcpy(&((handle->pstMMAPParsedItems + *(handle->pu8MMAPParsedItemCount))->value), &value, sizeof(value));
        ++(*(handle->pu8MMAPParsedItemCount));
        return MMAP_TRUE;
    }
    else   //pu8MMAPidCount  >= MAX_ITEM_COUNT
    {
        printf("**********************************************************************\n");
        printf("**********************************************************************\n");
        printf("MAX_ITEM_COUNT size = %d. It is not enough!! \n",MAX_ITEM_COUNT);
        printf("Please increase MAX_ITEM_COUNT size in Supernova/core/MMAPInfo/inc/MMAPInfoDataType.h\n");
        printf("%s cannot add to table!!!\n",name);
        assert(0);
    }
    return MMAP_FALSE;
}

MMAP_U32 GetValueFromTable(const char* pStr, MMAPParserHandle* handle)
{
    MMAP_S32 i;
    for(i = 0; i < *(handle->pu8MMAPParsedItemCount); i++)
    {
        if(Is_StrFullCmp(pStr, (handle->pstMMAPParsedItems + i)->name))
        {
            return (handle->pstMMAPParsedItems + i)->value;
        }
    }
    MMAP_PARSER_ERR("Error, not search in table... [%s %s %d]\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    return MMAP_FALSE;
}

MMAP_U32 DoCalculate_Shift(const char* pCommand)
{
    MMAP_U32 value_1 = 0;
    MMAP_U32 value_2 = 0;
    char op[10];
    memset(op, 0, sizeof(op));
    sscanf(pCommand, "%x %s %x", &value_1, op, &value_2);

    if(Is_StrFullCmp(">>", op))
    {
        return (value_1 >> value_2);
    }
    else if(Is_StrFullCmp("<<", op))
    {
        return (value_1 << value_2);
    }
    MMAP_PARSER_ERR("Error, not define case... [%s %s %d]\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);
    return MMAP_FALSE;
}

MMAP_U32 DoCalculate_Logic_Or(char* pCommand, MMAPParserHandle* handle)
{
    MMAP_U32 value = 0;
    char*  rest_of_str = NULL;
    char *pStr = strtok_r(pCommand, " |",&rest_of_str);

    while(pStr != NULL)
    {
        value |= GetValueFromTable(pStr, handle);
        pStr = strtok_r(rest_of_str, " |",&rest_of_str);
    }

    return value;
}

MMAP_BOOL Is_StrFullCmp(const char *pStr_1, const char *pStr_2)
{
    if(strlen(pStr_1) == strlen(pStr_2))
    {
        if(0 == memcmp(pStr_1, pStr_2, strlen(pStr_1)))
        {
            return MMAP_TRUE;
        }
    }
    return MMAP_FALSE;
}
