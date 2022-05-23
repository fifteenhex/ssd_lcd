#!/bin/sh

#param $1:dictory path
function get_dir_files() {
    if test -f $1
    then
        arr_files=(${arr_files[*]} $1)
    else
        for file in `ls $1`
        do
            get_dir_files $1/$file
        done
    fi
}


#param $1:product name
#param $2:chip name
#param $3:config file path
#param $4:template file path
function change_config_into_defconfig() {
    output_dir=configs/defconfigs
    product_name=$1
    chip_name=$2
    config_file=$3
    template_file=$4
    dst_file=$output_dir/${product_name}_${chip_name}_$(basename $config_file)_defconfig
    echo config file:$config_file
    echo template file:$template_file
    echo dst file:$dst_file

    #while read -r line
    #do
    #echo $line
    #done < $config_file

    arr_keys=(`sed '/^$/d; /[:blank:]*#/d' $config_file | tr -s " " | awk -F '=' '{print $1}'`)
    #echo ${arr_keys[@]}
    length_arr_keys=${#arr_keys[@]}
    #echo $length_arr_keys

    cp -f $template_file $dst_file
    for((i=0; i<${length_arr_keys}; i++));
    do
        #echo ${arr_keys[i]}
        arr_value=`sed '/^$/d; /[:blank:]*#/d' $config_file | tr -s " " | grep -w ^${arr_keys[i]} | awk -F '=' '{s="";for(i=2;i<=NF;i++)s=s""(i==NF?$i:$i"=");print s}'`
        arr_value=`echo $arr_value | sed 's/^[ \t]*//g'`
        #echo $arr_value

        if [ "$arr_value" -eq "$arr_value" ] 2> /dev/null
        then
            arr_value=$arr_value
        else
            arr_value="\"${arr_value}\""
        fi

        grep -qw "^CONFIG_${arr_keys[i]}" $dst_file
        if [ $? != 0 ] && ([ ${arr_keys[i]} != "CUSTOMER_OPTIONS" ] && [ ${arr_keys[i]} != "CUSTOMER_TAILOR" ]); then
            echo can not find the config:${arr_keys[i]}
            exit 1
        fi

        sed -i "s#^CONFIG_${arr_keys[i]}=.*#CONFIG_${arr_keys[i]}=${arr_value}#g" $dst_file
    done
}

get_dir_files configs
#echo ${arr_files[@]}

length_arr_files=${#arr_files[@]}
#echo $length_arr_files
for((i=0; i<${length_arr_files}; i++));
do
    if [[ ${arr_files[i]} =~ "configs/Kconfigs" ]] \
        || [[ ${arr_files[i]} =~ "configs/defconfigs" ]] \
        || [[ ${arr_files[i]} =~ "configs/current.configs" ]]; then
        unset arr_files[i]
    fi
done
length_arr_files=${#arr_files[@]}
#echo $length_arr_files

for file in ${arr_files[@]};
do
    if [[ $file =~ "ipc-rtos-smplh" ]] && [[ $file =~ "i6e" ]]; then
        echo $file
        change_config_into_defconfig ipc-rtos-smplh i6e $file configs/defconfigs/ipc-rtos-smplh_i6e_default_defconfig
    elif [[ $file =~ "ipc-rtos" ]] && [[ $file =~ "i6e" ]]; then
        echo $file
        change_config_into_defconfig ipc-rtos i6e $file configs/defconfigs/ipc-rtos_i6e_default_defconfig
    elif [[ $file =~ "ipc-rtos" ]] && [[ $file =~ "i6b0" ]]; then
        echo $file
        change_config_into_defconfig ipc-rtos i6b0 $file configs/defconfigs/ipc-rtos_i6b0_default_defconfig
    elif [[ $file =~ "ipc-rtos" ]] && [[ $file =~ "i6" ]]; then
        echo $file
        change_config_into_defconfig ipc-rtos i6 $file configs/defconfigs/ipc-rtos_i6_default_defconfig
    elif [[ $file =~ "ipc" ]] && [[ $file =~ "i6e" ]]; then
        echo $file
        change_config_into_defconfig ipc i6e $file configs/defconfigs/ipc_i6e_default_defconfig
    elif [[ $file =~ "ipc" ]] && [[ $file =~ "i6b0" ]]; then
        echo $file
        change_config_into_defconfig ipc i6b0 $file configs/defconfigs/ipc_i6b0_default_defconfig
    elif [[ $file =~ "ipc" ]] && [[ $file =~ "i6" ]]; then
        echo $file
        change_config_into_defconfig ipc i6 $file configs/defconfigs/ipc_i6_default_defconfig
    elif [[ $file =~ "ipc" ]] && [[ $file =~ "m6" ]]; then
        echo $file
        change_config_into_defconfig ipc m6 $file configs/defconfigs/ipc_m6_default_defconfig
    elif [[ $file =~ "nvr" ]] && [[ $file =~ "i2m" ]];then
        echo $file
        change_config_into_defconfig nvr i2m $file configs/defconfigs/nvr_i2m_default_defconfig
    elif [[ $file =~ "xvr" ]] && [[ $file =~ "m6" ]];then
        echo $file
        change_config_into_defconfig xvr m6 $file configs/defconfigs/xvr_m6_default_defconfig
    elif [[ $file =~ "cardv" ]] && [[ $file =~ "i6e" ]]; then
        echo $file
        change_config_into_defconfig cardv i6e $file configs/defconfigs/cardv_i6e_default_defconfig
    elif [[ $file =~ "cardv" ]] && [[ $file =~ "i6" ]]; then
        echo $file
        change_config_into_defconfig cardv i6 $file configs/defconfigs/cardv_i6_default_defconfig
    elif [[ $file =~ "usbcam-rtos" ]] && [[ $file =~ "i6e" ]]; then
        echo $file
        change_config_into_defconfig usbcam-rtos i6e $file configs/defconfigs/usbcam-rtos_i6e_default_defconfig
    elif [[ $file =~ "usbcam" ]] && [[ $file =~ "i6e" ]]; then
        echo $file
        change_config_into_defconfig usbcam i6e $file configs/defconfigs/usbcam_i6e_default_defconfig
    elif [[ $file =~ "usbcam" ]] && [[ $file =~ "i6b0" ]]; then
        echo $file
        change_config_into_defconfig usbcam i6b0 $file configs/defconfigs/usbcam_i6b0_default_defconfig
    elif [[ $file =~ "usbcam" ]] && [[ $file =~ "i6" ]]; then
        echo $file
        change_config_into_defconfig usbcam i6 $file configs/defconfigs/usbcam_i6_default_defconfig
    elif [[ $file =~ "dispcam" ]] && [[ $file =~ "p3" ]]; then
        echo $file
        change_config_into_defconfig dispcam p3 $file configs/defconfigs/dispcam_p3_default_defconfig
    fi
done
