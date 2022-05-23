执行prog_vif前需要先运行如下两个cmd，关闭fbc mode
# echo 0 > /sys/module/mi_vif/parameters/bFBCMode
# echo FBC 0 > /proc/vpe/vpe_debug
