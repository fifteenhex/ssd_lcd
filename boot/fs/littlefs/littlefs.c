/*
* littlefs.c- Sigmastar
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


/*
 * littlefs.c
 */
#include <stdlib.h>
#include <lfs.h>
#include <littlefs.h>
#include <spi_flash.h>
#include <linux/mtd/mtd.h>
#if defined(CONFIG_MS_SPINAND)
#include <common.h>
#include <nand.h>
#endif

#if !defined(CONFIG_MS_SPINAND)
static struct spi_flash *flash;
#endif

#if defined(CONFIG_FS_LITTLEFS)

typedef struct
{
    lfs_t lfs;
    struct lfs_config cfg;
} LittleFsMntHandle;

typedef struct
{
    lfs_t *lfs_file;
    lfs_file_t lfs_fd;
} LittleFsFileHandle;


extern int printf(const char *fmt, ...);
#define LITTLEFS_CACHE_SIZE                     2048
#define LITTLEFS_READ_SIZE                      2048
#define LITTLEFS_BLK_SIZE                       32768
#define LITTLEFS_READ_BUFFER_CNT                4

extern int get_mtdpart_range(char *partition, u64 *offset, u64 *size);

static LittleFsMntHandle _gLfsMntHandle;

//block device operations
static int block_device_read(const struct lfs_config *c, lfs_block_t block,
                             lfs_off_t off, void *buffer, lfs_size_t size)
{
    U32 u32Addr;

    u32Addr = c->block_size * (c->block_offset + block) + off;
#if defined(CONFIG_MS_SPINAND)
    nand_info_t *nand;
    lfs_size_t retlen;

    nand = &nand_info[0];
    return mtd_read(nand, u32Addr, size, &retlen, buffer);
#else
    return spi_flash_read(flash, u32Addr, size, (u_char *)buffer);
#endif
}

static int get_parts_info(char *partition,lfs_partition_t *lfs_partition)
{
    u64 part_offset;
    u64 part_size;

    if(!lfs_partition)
        return -1;

#if defined(CONFIG_MS_SPINAND)
    nand_info_t *nand;

    nand = &nand_info[0];
    lfs_partition->u32_PageSize = nand->writesize;
    lfs_partition->u32_BlkSize = nand->erasesize;
#else
    flash = spi_flash_probe(CONFIG_SF_DEFAULT_BUS, CONFIG_SF_DEFAULT_CS,
                            CONFIG_SF_DEFAULT_SPEED, CONFIG_SF_DEFAULT_MODE);
    if (!flash)
    {
        printf("Failed to get nor flash info\n");
        return -1;
    }

    lfs_partition->u32_PageSize = flash->page_size;
    lfs_partition->u32_BlkSize = flash->sector_size;
#endif

    lfs_partition->u32_BlkSize = LITTLEFS_BLK_SIZE;
    if (!get_mtdpart_range(partition, &part_offset, &part_size))
    {
        printf("failed to get mtdparts info\n");
        return -1;
    }
    lfs_partition->u16_BlkCnt = ((u32)part_size)/lfs_partition->u32_BlkSize;
    lfs_partition->u16_StartBlk = ((u32)part_offset)/lfs_partition->u32_BlkSize;

    return 0;
}

S32  littlefs_mount(char *partition, char *mnt_path)
{
    int err = 0;

    memset(&_gLfsMntHandle.cfg.lfs_partition, 0x0, sizeof(_gLfsMntHandle.cfg.lfs_partition));
    err = get_parts_info(partition,&_gLfsMntHandle.cfg.lfs_partition);
    if (err < 0)
        return err;

    //block device operations
    _gLfsMntHandle.cfg.read = block_device_read;

    //block device configuration
     //spinad need set flash page size, value is 2048, spinor not limit
    _gLfsMntHandle.cfg.page_size = _gLfsMntHandle.cfg.lfs_partition.u32_PageSize;
    // block size is a multiple of cache size
    _gLfsMntHandle.cfg.block_size = _gLfsMntHandle.cfg.lfs_partition.u32_BlkSize;
    _gLfsMntHandle.cfg.block_count = _gLfsMntHandle.cfg.lfs_partition.u16_BlkCnt;
    _gLfsMntHandle.cfg.block_offset = _gLfsMntHandle.cfg.lfs_partition.u16_StartBlk;

    _gLfsMntHandle.cfg.read_size = LITTLEFS_READ_SIZE;
    _gLfsMntHandle.cfg.read_buffer_cnt = LITTLEFS_READ_BUFFER_CNT;
    //cache size is a multiple of read sizes
    _gLfsMntHandle.cfg.cache_size = LITTLEFS_CACHE_SIZE;

    // mount the filesystem
    err = lfs_mount(&_gLfsMntHandle.lfs, &_gLfsMntHandle.cfg);

    return err;
}

void littlefs_unmount()
{
    lfs_unmount(&_gLfsMntHandle.lfs);
}

void* littlefs_open(char *filename, U32 flags, U32 mode)
{
    int err;
    int flag;
    void *ret = NULL;
    LittleFsFileHandle *fd = NULL;

    fd = (LittleFsFileHandle *)calloc(sizeof(LittleFsFileHandle), 1);
    if (!fd)
    {
        printf("%s: alloc fd fail\n", __FUNCTION__);
        goto littlefs_open_end;
    }

    switch(flags)
    {
        case O_RDONLY:
            flag = LFS_O_RDONLY;
            break;
        case O_WRONLY:
            flag = LFS_O_WRONLY;
            break;
        case O_RDWR:
            flag = LFS_O_RDWR;
            break;
        default:
            flag = LFS_O_RDONLY;
            break;
    }

    err = lfs_file_open(&_gLfsMntHandle.lfs, &fd->lfs_fd, filename, flag);
    if (err)
    {
        printf("%s: open %s fail(%d)\n", __FUNCTION__, filename, err);
        free((void*)fd);
        goto littlefs_open_end;
    }

    fd->lfs_file = &_gLfsMntHandle.lfs;
    ret = fd;

littlefs_open_end:
    return ret;
}

S32 littlefs_close(void* fd)
{
    int ret = 0;
    LittleFsFileHandle *file = (LittleFsFileHandle *)fd;
    if (fd)
    {
        ret = lfs_file_close(file->lfs_file, &file->lfs_fd);
        if(ret)
        {
            printf("%s: close fail(%d)\n", __FUNCTION__, ret);
        }
        free(fd);
        fd = NULL;
    }
    return ret;
}

S32 littlefs_read(void* fd, void *buf, U32 count)
{
    LittleFsFileHandle *file = (LittleFsFileHandle *)fd;

    return lfs_file_read(file->lfs_file, &file->lfs_fd, buf, count);
}

S32 littlefs_write(void* fd, void *buf, U32 count)
{
    //TODO
    return 0;
}

S32 littlefs_lseek(void* fd, S32 offset, S32 whence)
{
    LittleFsFileHandle *file = (LittleFsFileHandle *)fd;
    u32 ret;

    switch (whence)
    {
        case SEEK_SET:
            ret = 0;   // Do nothing
            break;
        case SEEK_CUR:
            ret = 0;   // Do nothing
            break;
        case SEEK_END:
            ret = lfs_file_size(file->lfs_file, &file->lfs_fd);
            break;
        default:
            ret = 0;
            break;
    }

    return ret;
}
#endif
