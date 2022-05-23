#!/usr/bin/python

import os, struct
import binascii
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("input_dir", help="directory to pack image")
parser.add_argument("output_bin", help="output image path")
parser.add_argument("-a", "--align", help="align unit of file start address (byte)", dest="align_unit", type=int, default=131072)

HEADER_VER = 1
MAGIC_ID = 0x5346574C
PARTITION_HEADER_SIZE = 64
FILE_HEADER_SIZE = 64
COMPRESS_TYPE = 0

def calc_crc32(filename):
    f = open(filename,'rb')
    buf = f.read()
    f.close()
    buf = (binascii.crc32(buf) & 0xFFFFFFFF)
    return buf

def main():
    args = parser.parse_args()

    ALIGN_UNIT_cnt = 0;
    write_ptr = 0

    entries = os.listdir(args.input_dir)
    entries.sort(key=lambda v: (v.upper(), v[0].islower()))

    header_size = len(entries)*FILE_HEADER_SIZE + PARTITION_HEADER_SIZE
    """print ('Partition Header:')
    print ('    magic id    : 0x%08X' %MAGIC_ID)
    print ('    hearer ver  : 0x%08X' %HEADER_VER)
    print ('    header size : %d' %header_size)
    print ('    file number : %d' %len(entries))"""
    write_ptr += header_size + args.align_unit - header_size % args.align_unit

    image_name = args.output_bin
    # Write partition header
    image = open(image_name, 'wb')
    image.write(struct.pack('I', MAGIC_ID))             # magic id
    image.write(struct.pack('I', HEADER_VER))           # header version
    image.write(struct.pack('I', header_size))          # header size
    image.write(struct.pack('I', args.align_unit))      # access align unit
    image.write(struct.pack('I', len(entries)))         # file number
    # Padding partition header to PARTITION_HEADER_SIZE
    for x in range(PARTITION_HEADER_SIZE-image.tell()%PARTITION_HEADER_SIZE):
        image.write(b'\x00')

    for entry in entries:
        file_path = args.input_dir + "/" + entry
        file_len = os.path.getsize(file_path)
        crc32_value = calc_crc32(file_path)
        """print ('File Header:')
        print ('    file name   : %s' %file_path)
        print ('    position    : 0x%08X' %write_ptr)
        print ('    file len    : 0x%08X' %file_len)
        print ('    padding len : 0x%08X' %((args.align_unit - file_len%args.align_unit) if file_len%args.align_unit else 0))
        print ('    crc32       : 0x%08X' %crc32_value)
        print ('    compress    : 0x%08X' %COMPRESS_TYPE)"""

        # Write file header
        image.write(struct.pack('32s', entry.encode())) # file name
        image.write(struct.pack('I', write_ptr))        # file position
        image.write(struct.pack('I', file_len))         # file length
        image.write(struct.pack('I', (args.align_unit - file_len%args.align_unit))) # padding length
        image.write(struct.pack('I', crc32_value))      # crc32
        image.write(struct.pack('I', COMPRESS_TYPE))    # compress type
        # Padding file header to FILE_HEADER_SIZE
        for x in range(PARTITION_HEADER_SIZE-image.tell()%PARTITION_HEADER_SIZE):
            image.write(b'\x00')
        write_ptr += file_len
        if file_len % args.align_unit:
            write_ptr += (args.align_unit - file_len % args.align_unit)

    # Padding header
    for x in range(args.align_unit-image.tell()%args.align_unit):
        image.write(b'\x00')

    # Write file content
    for entry in entries:
        file_path = args.input_dir + "/" + entry
        f = open(file_path, "rb")
        image.write(f.read())
        # Padding file content
        if image.tell()%args.align_unit:
            for x in range(args.align_unit-image.tell()%args.align_unit):
                image.write(b'\x00')
        f.close()

    image.close()

    print ('    %d files packed in %s' % (len(entries), image_name))

if __name__ == '__main__':
    main()
