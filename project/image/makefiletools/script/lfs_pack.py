#!/usr/bin/python

import sys, os, struct
import binascii
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("input_dir", help="directory to pack image")
parser.add_argument("output_bin", help="output image path")
parser.add_argument("-p", "--part_size", help="parttion size (Byte)", dest="part_size", type=str)

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

def main():
    block_size = 32768;
    cache_size = 32768;
    read_size = 2048;
    prog_size = 2048;
    cycle_count = 500;
    lookadhead_size = 8;
    
    args = parser.parse_args()
    part_size  = int(args.part_size, 16);
    image_name = args.output_bin
    image_size = 0;
    total_block_cnt = 0;
    
    entries = get_all_files(args.input_dir)
    entries.sort(key=lambda v: (v.upper(), v[0].islower()))
    
    #Calculate all file block cnt
    for entry in entries:
        file_len = os.path.getsize(entry)
        file_block = file_len / block_size
        file_remain = file_len % block_size
        
        total_block_cnt += file_block
        if (file_remain > 0):
            total_block_cnt+=1

    #Add superblock count
    total_block_cnt += 2
    image_size = total_block_cnt * block_size;
    if (part_size < image_size):
        print('###################mklittlefs fail, image size exceed patition size!###################')
        return 
   
    #mklittlefs command
    mklittlefs_bin = sys.path[0] + '/../../build/mklittlefs'
    make_image_command = mklittlefs_bin + ' -c ' + str(args.input_dir) + ' --block ' + str(block_size) + ' --size ' + str(image_size) \
                    + ' --read ' + str(read_size) + ' --prog ' + str(prog_size) + ' --cache ' + str(cache_size) \
                    + ' --cycle ' + str(cycle_count) + ' --ahead ' + str(lookadhead_size) + ' ' + image_name
    
    
    os.system(make_image_command)
    print ('image size %d' %image_size)

if __name__ == '__main__':
    main()
