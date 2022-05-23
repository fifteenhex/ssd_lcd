mkdir /var/tmp
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/customer/lib:/lib:/config/lib:/config/wifi
cd /customer
chmod 777 usbCamera
./usbCamera &
