#!/bin/sh

if [ "$#" != "1" ]; then
    echo "[ERR]usage: $0 configs/defconfigs/\$(PRODUCT)_\$(CHIP)_\$(CONFIG)_defconfig"
    exit -1
fi

defconfig=$1
if [[ ${defconfig:0-9} == "defconfig" ]]; then
    echo "make $(basename $defconfig)"
    make $(basename $defconfig)
else
    echo "[ERR]defconfig name format: \$(PRODUCT)_\$(CHIP)_\$(CONFIG)_defconfig"
    exit -1
fi
