#!/bin/sh

pwd=$PWD

mkdir output

tar -xzf zlib-1.2.11.tar.gz
cd zlib-1.2.11
export CHOST=arm-linux-gnueabihf
./configure --prefix=$pwd/output/zlib
make clean
make -j32
make install
cd -
cp -rf output/zlib/lib/libz.so* ../lib
cp -rf output/zlib/include/*.h ../inc


tar -xzf jpegsrc.v9d.tar.gz
mkdir -p $pwd/output/libjpeg
cd jpeg-9d
./configure CC=arm-linux-gnueabihf-gcc --prefix=$pwd/output/libjpeg --host=arm-linux
make clean
make -j32
make install
cd -
cp -rf output/libjpeg/lib/libjpeg.so* ../lib
cp -rf output/libjpeg/include/*.h ../inc

rm -rf zlib-1.2.11
rm -rf jpeg-9d
rm -rf output