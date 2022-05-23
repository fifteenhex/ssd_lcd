#!/bin/sh

KERNEL_ROOT_DIR=../../..
KERNEL_CONFIG_DIR=../../../arch/arm/configs
DEFCONFIG_FILE=./.config
CONFIG_LIST=$(sed -n 's/CONFIG_\(CONFIG_[_0-9a-zA-Z]*\)=y/\1/p' $DEFCONFIG_FILE)

echo $CONFIG_LIST

for i in $@;do
    echo ">>> Handle $i:"
    for o in $(find $KERNEL_CONFIG_DIR -iname "$i*_defconfig");do
        echo ">>>>>> Handle file $o:" 
        for c in $CONFIG_LIST;do
           echo $c
           grep -v "$c[ =]" $o > .temp
           cp .temp $o
           rm .temp
        done
        echo "<<<<< file $o done"
    done
    echo "<<< done $i"
done

# clear platform specify release blacklist file
find  -iname '*.blacklist' -exec rm {} \;


