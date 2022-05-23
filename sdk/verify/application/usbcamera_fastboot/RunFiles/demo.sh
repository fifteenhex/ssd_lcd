mkdir /var/tmp
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/customer_app/lib:/lib:/config/lib:/config/wifi
cd /customer_app
chmod 777 usbCamera
./usbCamera &
