#!/usr/bin/python

import sys, os, struct
import binascii
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("device_name", help="mount device")
parser.add_argument("dir_name", help="mount directory")
parser.add_argument("-p", "--part_size", help="parttion size (Byte)", dest="part_size", type=str)
parser.add_argument("-r", "--rcs_dir", help="rcS directory", dest="rcs_dir", type=str)


def main():
    args = parser.parse_args()
    block_size = 32768;
    cache_size = 32768;
    read_size = 2048;
    prog_size = 2048;
    cycle_count = 500;
    lookahead_size = 8;
    part_size  = int(args.part_size, 16);
    
    block_count = part_size / block_size;
  
    lfs_mount_command = 'lfs' + ' --block_size=' + str(block_size) + ' --block_cycles=' + str(cycle_count) + ' --read_size=' + str(read_size) \
                    + ' --prog_size=' + str(prog_size) + ' --cache_size=' + str(cache_size) + ' --block_count=' + str(block_count) \
                    + ' --lookahead_size=' + str(lookahead_size) + ' ' + str(args.device_name) + ' ' + str(args.dir_name)
    
    
    echo_rcS_command = 'echo' + ' ' + lfs_mount_command + ' ' + '>> ' + args.rcs_dir
    
    os.system(echo_rcS_command)
    print ('lfs_mount_command is %s' %echo_rcS_command)

if __name__ == '__main__':
    main()