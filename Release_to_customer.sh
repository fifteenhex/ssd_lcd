#!/bin/bash

flashtype="nand"
project="ssd222"
buildtype="project"
while getopts "f:p:e:" opt; do
  case $opt in
    f)
      flashtype=$OPTARG
      ;;
    p)
      project=$OPTARG
      ;;
	e)
	  buildtype=$OPTARG
	;;
    ?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done
#echo "######flashtype=${flashtype}######"
echo "######Usage:"
echo "            -f nor/nand"
echo "            -p ssd212/ssd222"
echo "            -e boot/kernel/project/all"
echo "example: ./Release_to_customer.sh -f nand -p ssd222 -e all"
#echo "######project=${project}######"
#echo "######fastboot=${fastboot}######"

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
echo 111
# build uboot
function build_boot() {
	cd ${RELEASEDIR}/boot
	declare -x ARCH="arm"
	declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
	if [ "${flashtype}" = "nor" ]; then
		make pioneer3_defconfig
	elif [ "${flashtype}" = "nand" ]; then
		make pioneer3_spinand_defconfig
	else
		flashtype="nor"
		echo "No need make menuconfig"
	fi
	#make clean
	make

	if [ "${flashtype}" = "nor" ]; then
        mkdir -p ${RELEASEDIR}project/board/p3/boot/nor/uboot
		cp -vf ${RELEASEDIR}boot/u-boot.xz.img.bin ${RELEASEDIR}project/board/p3/boot/nor/uboot

	else
        mkdir -p ${RELEASEDIR}project/board/p3/boot/spinand/uboot
		cp -vf ${RELEASEDIR}boot/u-boot_spinand.xz.img.bin ${RELEASEDIR}project/board/p3/boot/spinand/uboot/u-boot_spinand.xz.img.bin
	fi
}
#build kernel
function build_kernel() {
	cd ${RELEASEDIR}/kernel
	declare -x ARCH="arm"
	declare -x CROSS_COMPILE="arm-linux-gnueabihf-"
	if [ "${flashtype}" = "nor" ]; then
		#make pioneer3_ssc020a_s01a_demo_camera_wifi_defconfig #wifi
		make pioneer3_ssc020a_s01a_demo_camera_defconfig
	elif [ "${flashtype}" = "nand" ]; then
		#make pioneer3_ssc020a_s01a_spinand_demo_camera_wifi_defconfig #wifi
		make pioneer3_ssc020a_s01a_spinand_demo_camera_defconfig
	else
		echo "No need make menuconfig"
	fi
	make clean
	make -j8
	#cp -rvf ./arch/arm/boot/uImage* ../project/release/nvr/i2m/011A/glibc/8.2.1/bin/kernel/nor/
	#cp -rvf ./arch/arm/boot/Image ../project/release/nvr/i2m/011A/glibc/8.2.1/bin/kernel/nor/
	#rm -rf ../project/kbuild/4.9.84/i2m/configs/nvr/011A/glibc/8.2.1/nor/modules/*.ko
	#cp -rvf ./modules/*.ko ../project/kbuild/4.9.84/i2m/configs/nvr/011A/glibc/8.2.1/nor/modules/
    #cp -rvf ./arch/arm/boot/uImage.xz ../project/release/dispcam/p3/000A/glibc/9.1.0/bin/kernel/nor/

}
#build project
function build_project() {
	cd ${RELEASEDIR}/project
	if [ "${flashtype}" = "nor" ]; then

		if [ "${project}" = "ssd212" ]; then
			make dispcam_p3_nor.glibc-9.1.0-squashfs.s01a.64.qfn128.demo_defconfig
		fi
		if [ "${project}" = "ssd222" ]; then
			make dispcam_p3_nor.glibc-9.1.0-squashfs.s01a.64.qfn128.demo_camera_defconfig
		fi

	else

		if [ "${project}" = "ssd212" ]; then
			make dispcam_p3_spinand.glibc-9.1.0-s01a.64.qfn128.demo_defconfig
		fi
		if [ "${project}" = "ssd222" ]; then
			#make dispcam_p3_spinand.glibc-9.1.0-s01a.64.qfn128.demo_camera_defconfig
            make dispcam_p3_spinand.glibc-9.1.0-s01a.64.qfn128.demo_camera_defconfig
		fi
	fi
	make clean
	make image -j8
    #make image-fast
	#make image-nocheck
}
echo $buildtype
case $buildtype in
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
	\?)
	  echo "Invalid option: -$OPTARG" >&2
	  ;;
esac