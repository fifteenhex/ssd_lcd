#!/bin/bash

while getopts "a:c:" opt; do
  case $opt in
    a)
      alkaid_dir=$OPTARG
      ;;
    c)
      chip=$OPTARG
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done

uboot_dir=$PWD


if [ "${chip}" = "i5" ]; then
  declare -x PATH="/tools/toolchain/arm-linux-gnueabihf-4.8.3-201404/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  make infinity5_defconfig
  make clean; make
  if [ -d $alkaid_dir/project/board/i5/boot/nor/uboot ]; then
    cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/i5/boot/nor/uboot
    cp $uboot_dir/u-boot.mz.img.bin $alkaid_dir/project/board/i5/boot/nor/uboot
  fi

  make infinity5_spinand_defconfig
  make clean; make
  if [ -d $alkaid_dir/project/board/i5/boot/spinand/uboot ]; then
    cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/i5/boot/spinand/uboot
  fi
fi

if [ "${chip}" = "i6" ]; then
  declare -x PATH="/tools/toolchain/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  make infinity6_defconfig
  make clean; make -j4
  if [ -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi

  make infinity6_spinand_defconfig
  make clean; make -j4
  if [ -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  if [ -d $alkaid_dir/project/board/${chip}/boot/spinand-ramdisk/uboot ]; then
    cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand-ramdisk/uboot/u-boot_spinand-ramdisk.xz.img.bin
  fi
fi

if [ "${chip}" = "i6b0" ]; then
  declare -x PATH="/tools/toolchain/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  whereis ${CROSS_COMPILE}gcc
  GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
  echo GCC_VERSION=${GCC_VERSION}

  make infinity6b0_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot

  make infinity6b0_spinand_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot
fi
if [ "${chip}" = "i6e" ]; then
  declare -x PATH="/tools/toolchain/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  whereis ${CROSS_COMPILE}gcc
  GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
  echo GCC_VERSION=${GCC_VERSION}

  make infinity6e_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot

  make infinity6e_spinand_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot
fi
if [ "${chip}" = "i2m" ]; then
  declare -x PATH="/tools/toolchain/gcc-arm-8.2-2018.08-x86_64-arm-linux-gnueabihf/bin":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  whereis ${CROSS_COMPILE}gcc
  GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
  echo GCC_VERSION=${GCC_VERSION}

  make infinity2m_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot

  make infinity2m_spinand_defconfig
  make clean; make -j4
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot
fi
if [ "${chip}" = "p3" ]; then
  declare -x PATH="/tools/toolchain/gcc-sigmastar-9.1.0-2020.07-x86_64_arm-linux-gnueabihf/bin/":$PATH
  declare -x ARCH="arm"
  declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
  whereis ${CROSS_COMPILE}gcc
  GCC_VERSION=$(${CROSS_COMPILE}gcc --version | head -n 1 | sed -e 's/.*\([0-9]\.[0-9]\.[0-9]\).*/\1/')
  echo GCC_VERSION=${GCC_VERSION}

  make pioneer3_defconfig
  make clean; make
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot
  if [ ! -d $alkaid_dir/project/board/${chip}/boot-rtos/nor/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot-rtos/nor/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot-rtos/nor/uboot

  make pioneer3_ufu_defconfig
  make clean;make
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot/u-boot-ufu.xz.img.bin
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot-rtos/nor/uboot/u-boot-ufu.xz.img.bin

  make pioneer3_spinand_defconfig
  make clean; make
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot

  make pioneer3_dualenv_defconfig
  make clean; make
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/nor/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/nor/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/nor/uboot/u-boot_dualenv.xz.img.bin

  make pioneer3_dualenv_spinand_defconfig
  make clean; make
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/spinand/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/spinand/uboot
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/spinand/uboot/u-boot_dualenv_spinand.xz.img.bin

  make pioneer3_nor_emmc_defconfig
  make clean; make
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/emmc/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/emmc/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/emmc/uboot/u-boot_nor.xz.img.bin

  make pioneer3_spinand_emmc_defconfig
  make clean; make
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/emmc/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/emmc/uboot
  fi
  cp $uboot_dir/u-boot_spinand.xz.img.bin $alkaid_dir/project/board/${chip}/boot/emmc/uboot/u-boot_spinand.xz.img.bin

  make pioneer3_sdmmc_defconfig
  make clean; make
  if [ ! -d $alkaid_dir/project/board/${chip}/boot/sdmmc/uboot ]; then
    mkdir $alkaid_dir/project/board/${chip}/boot/sdmmc/uboot
  fi
  cp $uboot_dir/u-boot.xz.img.bin $alkaid_dir/project/board/${chip}/boot/sdmmc/uboot/UBOOT
fi
