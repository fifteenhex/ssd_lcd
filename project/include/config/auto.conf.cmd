deps_config := \
	configs/Kconfigs/sdk/mhal/Kconfig \
	configs/Kconfigs/sdk/misc/Kconfig \
	configs/Kconfigs/sdk/interface/Kconfig \
	configs/Kconfigs/sdk/verify/application/Kconfig \
	configs/Kconfigs/sdk/verify/py_ipu/Kconfig \
	configs/Kconfigs/sdk/verify/mixer/Kconfig \
	configs/Kconfigs/sdk/verify/mi_demo/Kconfig \
	configs/Kconfigs/sdk/verify/feature/Kconfig \
	configs/Kconfigs/sdk/verify/Kconfig \
	configs/Kconfigs/sdk/Kconfig \
	configs/Kconfigs/rtos/Kconfig \
	configs/Kconfigs/misc_options/Kconfig \
	configs/Kconfigs/customer_options/Kconfig \
	configs/Kconfigs/sensor/Kconfig \
	configs/Kconfigs/mmap/Kconfig \
	configs/Kconfigs/image/Kconfig \
	configs/Kconfigs/kernel/Kconfig \
	configs/Kconfigs/busybox/Kconfig \
	configs/Kconfigs/toolchain/Kconfig \
	configs/Kconfigs/board/Kconfig \
	configs/Kconfigs/sigma_wifi/Kconfig \
	configs/Kconfigs/product/Kconfig \
	configs/Kconfigs/chip/Kconfig \
	Kconfig

include/config/auto.conf: \
	$(deps_config)


$(deps_config): ;
