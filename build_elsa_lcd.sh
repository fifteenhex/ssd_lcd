#!/bin/bash

DATE=$(date +%m%d)
#RELEASEDIR=ReleaseToCus_${DATE}
RELEASEDIR=$(pwd)/
#release source code
#find ./boot/ | grep -v boot/.git | cpio -pdm ${RELEASEDIR}/
#find ./project/ | grep -v project/.git | cpio -pdm ${RELEASEDIR}/
#find ./kernel/ | grep -v kernel/.git | cpio -pdm ${RELEASEDIR}/
#find ./sdk/ | grep sdk/verify | grep -v sdk/verify/application/smarttalk | cpio -pdm ${RELEASEDIR}/

#save code version
#repo manifest -o snapshot.xml -r
#cp snapshot.xml ${RELEASEDIR}/sdk_version.xml

# build uboot
function build_boot() {
	cd ${RELEASEDIR}/boot
	declare -x ARCH="arm"
	declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
	make pioneer3_dualenv_spinand_defconfig
	make clean
	make -j16

	#mkdir -p ${RELEASEDIR}project/board/p3/boot/spinand/uboot
	cp -vf ${RELEASEDIR}boot/u-boot_spinand.xz.img.bin ${RELEASEDIR}project/board/p3/boot/spinand/uboot/u-boot_dualenv_spinand.xz.img.bin
}
#build kernel
function build_kernel() {
	cd ${RELEASEDIR}/kernel
	declare -x ARCH="arm"
	declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
    make pioneer3_ssc020a_s01a_spinand_demo_camera_defconfig
	make clean
	make -j8
}
#build project
function build_project() {
	cd ${RELEASEDIR}/project
    make dispcam_p3_spinand.glibc-9.1.0-s01a.128.qfn128.demo_camera_defconfig
	make clean
	make image -j16
    #make image-fast
	#make image-nocheck
}
echo $1
case $1 in
	all)
	  build_boot
	  build_kernel
	  build_project
	  ;;
	boot)
	  build_boot
	  ;;
	kernel)
	  build_kernel
	;;
	project)
	  build_project
	;;
	clean)
	  cd ${RELEASEDIR}/boot
	  make clean
	  cd ${RELEASEDIR}/kernel
	  make clean
	  cd ${RELEASEDIR}/project
	  make clean
	;;
	*)
	  build_project
	  ;;
esac