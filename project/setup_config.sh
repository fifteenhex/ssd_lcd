#!/bin/sh

PROJ_ROOT=$PWD

if [ "$#" != "1" ] && [ "$#" != "2" ]; then
    echo "usage: $0 configs/config.chip"
    echo "or $0 configs/config.chip config.out"
    exit -1
fi

INPUT_CONFIG=$1
if [[ ! -e  $INPUT_CONFIG ]]; then
    echo "can't find $INPUT_CONFIG"
    exit -1
fi

if [[ ${INPUT_CONFIG:0-9} == "defconfig" ]]; then
    $PROJ_ROOT/setup_defconfig.sh $INPUT_CONFIG
    exit
fi

OUTPUT_CONFIG=$PROJ_ROOT/configs/current.configs
if [ "$#" == "2" ]; then
    OUTPUT_CONFIG=$2
fi

if [ -e configs ]; then

    setup_chip=$(sed -n "/^CHIP\b/p"  $1 | awk '{print $3}')
    setup_toolchain_v=$(sed -n "/^TOOLCHAIN_VERSION\b/p"  $1 | awk '{print $3}')
    setup_cross_compile=$(sed -n "/^TOOLCHAIN_REL\b/p"  $1 | awk '{print $3}')
    cur_toolchain_v=$(${setup_cross_compile}-gcc -dumpversion)

    if [ "$setup_toolchain_v" != "$cur_toolchain_v" ]; then
        echo "*********************************************************************"
        echo -e "\033[5;41;33m" "TOOLCHAIN VERSION IS NOT MATCHED FOR CHIP [ $setup_chip ]!" "\033[0m"
        echo "config request toolchain version is:[ $setup_toolchain_v ], but server toolchain version is:[ $cur_toolchain_v ]."
        echo "*********************************************************************"
        exit
    fi

    echo PROJ_ROOT = $PROJ_ROOT > $OUTPUT_CONFIG
    echo CONFIG_NAME = config_module_list.mk >> $OUTPUT_CONFIG
    echo KBUILD_MK = kbuild/kbuild.mk >> $OUTPUT_CONFIG
    echo SOURCE_MK = ../sdk/sdk.mk >> $OUTPUT_CONFIG
    echo "KERNEL_MEMADR = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) E_LX_MEM phyaddr)" >> $OUTPUT_CONFIG
    echo "KERNEL_MEMLEN = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) E_LX_MEM size)" >> $OUTPUT_CONFIG
    echo "KERNEL_MEMADR2 = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) E_LX_MEM2 phyaddr)" >> $OUTPUT_CONFIG
    echo "KERNEL_MEMLEN2 = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) E_LX_MEM2 size)" >> $OUTPUT_CONFIG
    echo "KERNEL_MEMADR3 = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) E_LX_MEM3 phyaddr)" >> $OUTPUT_CONFIG
    echo "KERNEL_MEMLEN3 = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) E_LX_MEM3 size)" >> $OUTPUT_CONFIG
    echo "LOGO_ADDR = \$(shell $PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/\$(CHIP)/mmap/\$(MMAP) \$(CHIP) \$(BOOTLOGO_ADDR) miuaddr)" >> $OUTPUT_CONFIG
    cat $1 >> $OUTPUT_CONFIG
    echo "ARCH=arm" >> $OUTPUT_CONFIG
    echo "CROSS_COMPILE=$(sed -n "/TOOLCHAIN_REL\b/p" $OUTPUT_CONFIG | awk '{print $3}')-" >> $OUTPUT_CONFIG

    c=$(sed -n "/^CHIP\b/p" $OUTPUT_CONFIG | awk '{print $3}')

    if [ "$c" = "i5" ]; then
        echo "CHIP_ALIAS = pretzel" >> $OUTPUT_CONFIG
    elif [ "$c" = "i6" ]; then
        echo "CHIP_ALIAS = macaron" >> $OUTPUT_CONFIG
    elif [ "$c" = "i2m" ]; then
        echo "CHIP_ALIAS = taiyaki" >> $OUTPUT_CONFIG
    elif [ "$c" = "p2" ]; then
        echo "CHIP_ALIAS = takoyaki" >> $OUTPUT_CONFIG
    elif [ "$c" = "i6e" ]; then
        echo "CHIP_ALIAS = pudding" >> $OUTPUT_CONFIG
    elif [ "$c" = "i6b0" ]; then
        echo "CHIP_ALIAS = ispahan" >> $OUTPUT_CONFIG
    elif [ "$c" = "m6" ]; then
        echo "CHIP_ALIAS = tiramisu" >> $OUTPUT_CONFIG
    elif [ "$c" = "p3" ]; then
        echo "CHIP_ALIAS = ikayaki" >> $OUTPUT_CONFIG
        echo "CHIP_FULL_NAME = pioneer3" >> $OUTPUT_CONFIG
    fi
    echo "PREFIX =\$(TOOLCHAIN_REL)-" >> $OUTPUT_CONFIG
    echo "AS = \$(PREFIX)as" >> $OUTPUT_CONFIG
    echo "CC = \$(PREFIX)gcc" >> $OUTPUT_CONFIG
    echo "CXX = \$(PREFIX)g++" >> $OUTPUT_CONFIG
    echo "CPP = \$(PREFIX)cpp" >> $OUTPUT_CONFIG
    echo "AR = \$(PREFIX)ar" >> $OUTPUT_CONFIG
    echo "LD = \$(PREFIX)ld" >> $OUTPUT_CONFIG
    echo "STRIP = \$(PREFIX)strip" >> $OUTPUT_CONFIG
    echo "export ARCH CROSS_COMPILE" >> $OUTPUT_CONFIG
else
    echo "can't found configs directory!"
    exit -1
fi

CHIP=`cat $1 | awk '/CHIP/ {print substr($3,$1)}'`
MMAP=`cat $1 | awk '/MMAP/ {print substr($3,$1)}'`
kernel_reserved_env="mmap_reserved="
FBADDR=`$PROJ_ROOT/image/makefiletools/bin/mmapparser $PROJ_ROOT/board/$CHIP/mmap/$MMAP $CHIP E_MMAP_ID_FB phyaddr`
assemble_kernel_reserved_env()
{
	n=$#
	n=$(expr $n \/ 5 \- 1)
	for i in $(seq 0 $n)
	do
		j=$(expr $i \* 5)
		j=$(expr $j \+ 1)
		name=$(eval "echo \${$j}")
		name=$(tr [A-Z] [a-z] <<< $name)
		j=$(expr $j \+ 1)
		miu=$(eval "echo \${$j}")
		j=$(expr $j \+ 1)
		sz=$(eval "echo \${$j}")
		j=$(expr $j \+ 1)
		start=$(eval "echo \${$j}")
		j=$(expr $j \+ 1)
		end=$(eval "echo \${$j}")
		kernel_reserved_env+="$name,miu=$miu,sz=$sz,max_start_off=$start,max_end_off=$end "
		if [ "$name" == "bootlogo" ]; then
            sed -i "s/LOGO_ADDR = .*/LOGO_ADDR = $start/g" $OUTPUT_CONFIG
		fi
		if [ "$FBADDR" != "" ]; then
            sed -i "s/LOGO_ADDR = .*/LOGO_ADDR = $FBADDR/g" $OUTPUT_CONFIG
		fi
	done
}
data=`$PROJ_ROOT/image/makefiletools/bin/reserved $PROJ_ROOT/board/$CHIP/mmap/$MMAP $CHIP `
assemble_kernel_reserved_env $data
if [ $kernel_reserved_env != "mmap_reserved=" ]; then
    sed -i "s/KERNEL_BOOT_ENV.*/& \$(KERNEL_RESERVED_ENV)/g" $OUTPUT_CONFIG
    echo "KERNEL_RESERVED_ENV = $kernel_reserved_env" >> $OUTPUT_CONFIG
fi

cat $OUTPUT_CONFIG
