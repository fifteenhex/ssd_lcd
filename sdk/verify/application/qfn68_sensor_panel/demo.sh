mkdir /var/tmp
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/customer/lib:/lib:/config/lib
cd /customer
chmod 777 prog_panel
./prog_panel &
