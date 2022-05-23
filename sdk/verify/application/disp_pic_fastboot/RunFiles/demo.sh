mkdir /var/tmp
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/lib:/lib:/config/lib:/config/wifi
cd /customer_app
chmod 777 YuvToPanel
./YuvToPanel YUV420SP_1024_600.yuv 1024 600 1024 600 &
