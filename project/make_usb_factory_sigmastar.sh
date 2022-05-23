#!/bin/bash
#----------------------------------------
# PATH Define
#----------------------------------------

TARGET_DIR=./image/output/images
AUTO_UPDATE_SCRIPT=$TARGET_DIR/auto_update.txt
TMP_UPGRADE_FILE=TMP_SigmastarUpgrade.bin
PADDED_BIN=$TARGET_DIR/padded.bin
PAD_DUMMY_BIN=$TARGET_DIR/dummy_pad
BUILD_TIME=`date '+%Y%m%d%H%M'`
UPGRADE_FILE=$TARGET_DIR/SstarUsbImage_${BUILD_TIME}.bin
SCRIPT_FILE=$TARGET_DIR/sstar_usb_factory_script.txt
USB_UPDATER_LOAD_FILE=$TARGET_DIR/usb_updater_ipl.bin
U_BOOT_LOAD_FILE=$TARGET_DIR/usb_updater_boot.bin
#lunch `print_lunch_menu | grep aosp_$TARGET_DEVICE-userdebug | cut -d. -f1`

#----------------------------------------
# Globe Value Define
#----------------------------------------
SECURE_UPGRADE=0
USB_CRC_CHECK=0
SPILT_SIZE=16384
SCRIPT_SIZE=0x1000 #4KB
PAD_DUMMY_SIZE=10240 #10KB
currentImageOffset=0
MAGIC_STRING="SSTAR_USB_IMAGE"
FullUpgrade=
#CRC_SEGMENT_SIZE should align to 0x1000
CRC_SEGMENT_SIZE=0x200000 #2MB

function func_process_main_script()
{
    BUILD_PATH=`pwd`
    setpartition=0

    #confirm each image is upgrade or not.
    mainScript=""
    tmp2="
    "

    if [ ! -f "$AUTO_UPDATE_SCRIPT" ] ; then
        echo "auto_update script is not exist, can not generate bin, please build image!!!"
        exit
    fi

    #tmpScript=$(grep "estar" $AUTO_UPDATE_SCRIPT | grep "\[\[")
    tmpScript=$(cat $AUTO_UPDATE_SCRIPT)
    for mainContent in $tmpScript
    do
        if [ "$(echo $mainContent | awk '{print $1}')" == "estar" ];then
            imageName=$(echo $mainContent | awk '{print $2}' | cut -d '/' -f 2)
            if [ "$FullUpgrade" == "Y" ] || [ "$FullUpgrade" == "y" ] || [ -z "$FullUpgrade" ]; then
                mainScript=$mainScript$mainContent$tmp2
            else
                read -p "Optional Upgrade $imageName? (Y/N)" temp
                if [ "$temp" == "Y" ] || [ "$temp" == "y" ] || [ -z "$temp" ]; then
                    mainScript=$mainScript$mainContent$tmp2
                fi
            fi
        else
            mainScript=$mainScript$mainContent$tmp2
        fi
    done

    echo "USB Facotry Image Generating....."
    # pad mangic to $UPGRADE_FILE
    printf "#%s\n" $MAGIC_STRING > $SCRIPT_FILE
    printf "#GENERATED TIME:%s\n" $BUILD_TIME  >> $SCRIPT_FILE
    printf "SCRIPT 0x0 0x%x\n" $SCRIPT_SIZE  >> $SCRIPT_FILE
    currentImageOffset=$(($currentImageOffset+$SCRIPT_SIZE))
    func_process_load_file_to_script
    for mainContent in $mainScript
    do
        if [ "$(echo $mainContent | awk '{print $1}')" == "estar" ];then
            func_process_sub_script $(echo $mainContent|awk '{print $2}')
        else
            if [ "$mainContent" != "reset" ] && [ "$mainContent" != "printenv" ] && [ "$mainContent" != "% <- this is end of file symbol" ];then
                echo $mainContent >> $SCRIPT_FILE
            fi
        fi
    done
}

function func_process_load_file_to_script()
{
    if [ ! -f "$USB_UPDATER_LOAD_FILE" ] ||  [  ! -f "$U_BOOT_LOAD_FILE" ]; then
        echo "usb updater or u-boot is not exist!!!"
        exit
    fi
    # usb_updater
    usb_updater_size=$(stat -c%s $USB_UPDATER_LOAD_FILE)
    printf "USB_UPDATER 0x%x 0x%x\n" $currentImageOffset $usb_updater_size  >> $SCRIPT_FILE
    cat $USB_UPDATER_LOAD_FILE >> $TMP_UPGRADE_FILE
    # align image to 0x1000(4K)
    needAlignSize=0
    not_align_size=$(($usb_updater_size & 0xfff))
    if [ $not_align_size != 0 ]; then
        needAlignSize=$((0x1000-$not_align_size))
        for ((i=0; i<$needAlignSize; i++))
        do
            printf "\xff" >>$PADDED_BIN
        done

        cat $PADDED_BIN >>$TMP_UPGRADE_FILE
        rm $PADDED_BIN
    fi
    currentImageOffset=$(($currentImageOffset+$usb_updater_size+$needAlignSize))

    #u-boot
    needAlignSize=0
    u_boot_size=$(stat -c%s $U_BOOT_LOAD_FILE)
    printf "U-BOOT 0x%x 0x%x\n" $currentImageOffset $u_boot_size >> $SCRIPT_FILE
    cat $U_BOOT_LOAD_FILE >> $TMP_UPGRADE_FILE
    # align image to 0x1000(4K)
    not_align_size=$(($u_boot_size & 0xfff))
    if [ $not_align_size != 0 ]; then
        needAlignSize=$((0x1000-$not_align_size))
        for ((i=0; i<$needAlignSize; i++))
        do
            printf "\xff" >>$PADDED_BIN
        done

        cat $PADDED_BIN >>$TMP_UPGRADE_FILE
        rm $PADDED_BIN
    fi
    currentImageOffset=$(($currentImageOffset+$u_boot_size+$needAlignSize))
}

function func_process_sub_script()
{
    filePath=$1
    fileName=$(echo $1 | cut -d '/' -f 2 | cut -d '[' -f 3)
    echo "# File Partition: "$fileName >> $SCRIPT_FILE
    filecontent=$(grep -Ev "^\s*$|#" $TARGET_DIR/$filePath)
    for subContent in $filecontent
    do
        if [ "$subContent" != "% <- this is end of file symbol" ] && [ "$subContent" != "sync_mmap" ]; then
            subContent_prefix=$(echo $subContent | awk '{print $1}')
            if [ "$subContent_prefix" == "estar" ]; then
                func_process_sub_script $(echo $subContent|awk '{print $2}')
            elif [ "$subContent_prefix" == "tftp" ]; then
                DRAM_BUF_ADDR=$(echo $subContent | awk '{print $2}')
                imagePath=$(echo $subContent | awk '{print $3}')
                if [ ! -f "$TARGET_DIR/$imagePath" ] ; then
                    echo "error:image:$TARGET_DIR/$imagePath is not exist!!!"
                    rm -f $SCRIPT_FILE
                    exit
                fi
                imageSize=$(stat -c%s $TARGET_DIR/$imagePath)
                printf "usbload %s 0x%x 0x%x\n" $DRAM_BUF_ADDR $currentImageOffset $imageSize >> $SCRIPT_FILE
                cat $TARGET_DIR/$imagePath >> $TMP_UPGRADE_FILE

                # align image to 0x1000(4K)
                ImageAlignSize=0
                NOT_ALAIN_IMAGE_SIZE=$(($imageSize & 0xfff))
                if [ $NOT_ALAIN_IMAGE_SIZE != 0 ]; then
                    ImageAlignSize=$((0x1000-$NOT_ALAIN_IMAGE_SIZE))
                    for ((i=0; i<$ImageAlignSize; i++))
                    do
                        printf "\xff" >>$PADDED_BIN
                    done

                    cat $PADDED_BIN >>$TMP_UPGRADE_FILE
                    rm $PADDED_BIN
                fi
                currentImageOffset=$(($currentImageOffset+$imageSize+$ImageAlignSize))
            else
                #fix ubi write 0x20400000 tvcustomer ${filesize} to ubi write 0x20400000 tvcustomer 0xfffff
                #Begin
                CMD=$(echo $subContent | awk '{print $2}')
                if [ "$CMD" == "write.p" ] || [ "$CMD" == "write.boot" ] || [ "$CMD" == "unlzo" ] || [ "$CMD" == "unlzo.cont" ] || [ "$CMD" == "write.e" ] || [ "$CMD" == "write" ]; then
                    #change 10 mechanism to 16 mechanism
                    imageSize_16=0x$(echo "obase=16; $imageSize" | bc)
                    imageAddr=$(echo "$DRAM_BUF_ADDR")
                    subContent=${subContent/'$(filesize)'/$imageSize_16}
                    subContent=${subContent/'${filesize}'/$imageSize_16}
                    subContent=${subContent/'${fileaddr}'/$imageAddr}
                fi
                #fix nand write.e ${fileaddr} recovery $(filesize) to nand write.e xxx recovery xxx
                #fix setenv recoverycmd nand read.e 0x25000000 recovery $(filesize) to setenv recoverycmd nand read.e 0x25000000 recovery xxx
                CMD_NAND=$(echo $subContent | awk '{print $1}')
                if [ "$CMD_NAND" == "ncishash" ] || [ "$CMD_NAND" == "setenv" ]; then
                    imageSize_16=0x$(echo "obase=16; $imageSize" | bc)
                    subContent=${subContent/'$(filesize)'/$imageSize_16}
                fi
                #End
                subContent=$(echo $subContent | tr -d '\r\n')
                echo $subContent >> $SCRIPT_FILE
            fi
        fi
    done
}

function func_init()
{
    # delete related file
    rm -f $SCRIPT_FILE
    rm -f $TMP_UPGRADE_FILE
    #ANDROID_BUILD_TOP=`pwd`
    #export PATH=$PATH:$ANDROID_BUILD_TOP/prebuilts/tools/linux-x86/crc
    dos2unix $AUTO_UPDATE_SCRIPT $TARGET_DIR/scripts/* 1>/dev/null 2>&1
}

function func_generate_script_file()
{

    echo "% <- this is end of script symbol" >>$SCRIPT_FILE

    # pad script to script_file size
    SCRIPT_FILE_SIZE=$(stat -c%s $SCRIPT_FILE)
    PADDED_SIZE=$(($SCRIPT_SIZE-$SCRIPT_FILE_SIZE))

    printf "\xff" >$PAD_DUMMY_BIN
    for ((i=1; i<$PAD_DUMMY_SIZE; i++))
    do
        printf "\xff" >>$PAD_DUMMY_BIN
    done

    while [ $PADDED_SIZE -gt $PAD_DUMMY_SIZE ]
    do
        cat $PAD_DUMMY_BIN >> $SCRIPT_FILE
        PADDED_SIZE=$(($PADDED_SIZE-$PAD_DUMMY_SIZE))
    done

    if [ $PADDED_SIZE != 0 ]; then
        printf "\xff" >$PADDED_BIN
        for ((i=1; i<$PADDED_SIZE; i++))
        do
            printf "\xff" >>$PADDED_BIN
        done
        cat $PADDED_BIN >> $SCRIPT_FILE
        rm $PADDED_BIN
    fi
}

function func_generate_upgrade_file()
{
    # generate $UPGRADE_FILE
    cat $SCRIPT_FILE > $UPGRADE_FILE
    rm -f $SCRIPT_FILE
    cat $TMP_UPGRADE_FILE >> $UPGRADE_FILE
    rm $TMP_UPGRADE_FILE

    echo "success, usb factory image have generate:"
    echo "      path:$UPGRADE_FILE"
    echo "      size:$(stat -c%s $UPGRADE_FILE) byte"
    md5="$(md5sum $UPGRADE_FILE  | cut -d" " -f1)"
    echo "      md5sum:$md5"
    printf "$md5" >> $UPGRADE_FILE

}

function parsing_para()
{
    read -p "Full or Optional Upgrade ? (Y/N)" FullUpgrade
}

#-----------------------------------------------
# Main():Generate Upgrade Image  Script
#-----------------------------------------------
IFS="
"
parsing_para;
func_init;
func_process_main_script;
func_generate_script_file;
func_generate_upgrade_file;
