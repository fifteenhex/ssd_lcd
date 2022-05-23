#!/usr/bin/python

import sys, os, struct
import binascii
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("input_dir", help="directory to pack image")
parser.add_argument("output_bin", help="output image path")
parser.add_argument("-p", "--part_size", help="parttion size (Byte)", dest="part_size", type=str)
parser.add_argument("-t", "--flash_type", help="flash type", dest="flash_type", type=str)
parser.add_argument("-g", "--page_size", help="page size", dest="page_size", type=str, default="0x800")
parser.add_argument("-b", "--block_size", help="block size", dest="block_size", type=str, default="0x20000")
parser.add_argument("-l", "--linux_flash_type", help="linux flash type", dest="linux_flash_type", type=str, default="none")

def get_all_files(dir):
    files_ = []
    list_ = os.listdir(dir)
    for i in range(0, len(list_)):
        path = os.path.join(dir, list_[i])
        if os.path.isdir(path):
            files_.extend(get_all_files(path))
        if os.path.isfile(path):
            files_.append(path)
    return files_

def get_all_subdirs(dir):
    dirs_ = []
    list_ = os.listdir(dir)
    for i in range(0, len(list_)):
        path = os.path.join(dir, list_[i])
        if os.path.isdir(path):
            dirs_.append(path)
            dirs_.extend(get_all_subdirs(path))
    return dirs_

def main():
    block_size = 131072;
    subblock_size = 32768;
    subblock_reserved_size = 16;
    cache_size = 32768;
    cache_pool_size = 2;
    file_cache_size = 32768;
    read_size = 2048;
    prog_size = 2048;
    cycle_count = 500;
    lookadhead_size = 8;
    inline_file_size = 1022;
    compaction = 1

    args = parser.parse_args()
    part_size  = int(args.part_size, 16);
    flash_type  = args.flash_type;
    linux_flash_type = args.linux_flash_type
    image_name = args.output_bin
    page_size  = int(args.page_size, 16);
    image_size = 0;
    subblock_cnt = 0;
    file_block_cnt = 0;
    total_block_cnt = 0;

    if (flash_type == "spinand"):
        read_size = page_size;
        prog_size = page_size;
        block_size = int(args.block_size, 16);

    if (flash_type == "nor"):
        block_size = 65536

    if (flash_type == "sdmmc"):
        block_size = 131072
        read_size = 2048
        prog_size = 2048

    if (linux_flash_type == "emmc"):
        block_size = 131072
        read_size = 2048
        prog_size = 2048

    subblock_per_block = block_size / subblock_size;

    entries = get_all_files(args.input_dir)
    entries.sort(key=lambda v: (v.upper(), v[0].islower()))

    dentries = get_all_subdirs(args.input_dir)
    dentries.sort(key=lambda v: (v.upper(), v[0].islower()))

    #Calculate all file block cnt
    for entry in entries:
        file_len = os.path.getsize(entry)
        file_subblock = file_len / (subblock_size - subblock_reserved_size)
        file_remain = file_len % (subblock_size - subblock_reserved_size)

        subblock_cnt += file_subblock
        if (file_len > inline_file_size and file_remain > 0):
            subblock_cnt += 1

    file_block_cnt += subblock_cnt / subblock_per_block
    subblock_remain = subblock_cnt % subblock_per_block
    if (subblock_remain > 0):
        file_block_cnt += 1

    total_block_cnt += file_block_cnt

    #Calculate all dir block cnt
    for entry in dentries:
        total_block_cnt += 2

    #Add superblock count
    total_block_cnt += 2

    image_size = total_block_cnt * block_size;
    if (part_size < image_size):
        print('###################mkfwfs fail, image size exceed patition size!###################')
        return

    #mkfwfs command
    mkfwfs_bin = sys.path[0] + '/../../build/mkfwfs'
    make_image_command = mkfwfs_bin + ' -c ' + str(args.input_dir) + ' --block ' + str(block_size) + ' --subblock ' + str(subblock_size) \
                    + ' --fcache ' + str(file_cache_size) + ' --pool ' + str(cache_pool_size) \
                    + ' --size ' + str(image_size) + ' --compaction ' + str(compaction) \
                    + ' --read ' + str(read_size) + ' --prog ' + str(prog_size) + ' --cache ' + str(cache_size) \
                    + ' --cycle ' + str(cycle_count) + ' --ahead ' + str(lookadhead_size) + ' ' + image_name


    print (make_image_command)
    os.system(make_image_command)
    print ('image size %d' %image_size)

if __name__ == '__main__':
    main()
