#!/bin/sh

#/config/riu_w e 30 11
#/config/riu_w 103c 8 00
#sleep 0.01 
#/config/riu_w 103c 8 10

#mkdir -p /etc/
#touch /etc/hosts
touch /appconfigs/hosts
mkdir -p /tmp/wifi/run
chmod 777 /tmp/wifi/run
mkdir -p /appconfigs/misc/wifi/
mkdir -p /var/wifi/misc/
mkdir -p /var/lib/misc/
mkdir -p /var/run/hostapd/
insmod /config/wifi/sigma_wifi_ssw10xb.ko Sstar_printk_mask=0
mdev -s
wlan0=`ifconfig -a | grep wlan0`
trial=0
maxtrycnt=50
while [ -z "$wlan0" ] && [ $trial -le $maxtrycnt ]
do 
    sleep 0.2
    #echo currect try $trial...
    trial=$(($trial + 1 ))
    wlan0=`ifconfig -a | grep wlan0`
done
if [ $trial -le $maxtrycnt ]; then
    echo try $trial times
fi
if [ $trial -gt $maxtrycnt ];then
    echo wlan0 not found
    exit -1
fi
#echo LOG_LMAC=ON > /sys/module/sigma_wifi_ssw10xb/Sstarfs/Sstar_printk_mask
#echo LOG_BH=ON > /sys/module/sigma_wifi_ssw10xb/Sstarfs/Sstar_printk_mask
#echo LOG_AP=ON > /sys/module/sigma_wifi_ssw10xb/Sstarfs/Sstar_printk_mask
#echo LOG_STA=ON > /sys/module/sigma_wifi_ssw10xb/Sstarfs/Sstar_printk_mask
#echo LOG_SCAN=ON > /sys/module/sigma_wifi_ssw10xb/Sstarfs/Sstar_printk_mask
exit 0







