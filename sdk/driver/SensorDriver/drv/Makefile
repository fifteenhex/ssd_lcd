CONFIG_MSTAR_CHIP_NAME := $(subst ",,$(CONFIG_MSTAR_CHIP_NAME))
EXTRA_CFLAGS      +=  -Idrivers/sstar/camdriver/sensordriver/drv/inc
EXTRA_CFLAGS      +=  -Idrivers/sstar/camdriver/sensordriver/drv/pub
EXTRA_CFLAGS      +=  -Idrivers/sstar/camdriver/sensorif/drv/pub
EXTRA_CFLAGS      +=  -Idrivers/sstar/camdriver/sensorif/drv/inc
EXTRA_CFLAGS      +=  -Idrivers/sstar/cam_os_wrapper/pub


MVXV_START := MVX3
MVXV_LIB_TYPE := I2
MS_PLATFORM_ID := __
COMMITNUMBER := g$(shell git log --format=%h -n 1 2> /dev/null)$(shell git diff --quiet || echo -dirty )
COMP_ID := [sensor_module]
MVXV_EXT := [$(shell git rev-parse --abbrev-ref HEAD 2> /dev/null | sed -e 's/\//_/g') build.$(shell date +%Y%m%d%H%M)
MVXV_END := ]XVM
EXTRA_CFLAGS += -DSENSOR_MODULE_VERSION="$(MVXV_START)$(MVXV_LIB_TYPE)$(MS_PLATFORM_ID)$(COMMITNUMBER)$(COMP_ID)$(MVXV_EXT)$(MVXV_END)"


obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_imx323.o
sensor_imx323-y := src/drv_ms_cus_imx323.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_imx327.o
sensor_imx327-y := src/drv_ms_cus_imx327_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_imx317_hdr.o
sensor_imx317_hdr-y := src/drv_ms_cus_imx317_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_nvp6124b.o
sensor_nvp6124b-y := src/drv_ms_cus_nvp6124b.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_imx291.o
sensor_imx291-y := src/drv_ms_cus_imx291_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_imx307_hdr.o
sensor_imx307_hdr-y := src/drv_ms_cus_imx307_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_os08a10.o
sensor_os08a10-y := src/drv_ms_cus_OS08A10_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_ar0237_rgbir.o
sensor_ar0237_rgbir-y := src/drv_ms_cus_ar0237_RGBIR.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_imx230.o
sensor_imx230-y := src/drv_ms_cus_imx230_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_os05a10.o
sensor_os05a10-y := src/drv_ms_cus_OS05A10_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_c2390.o
sensor_c2390-y := src/drv_ms_cus_C2390_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) += sensor_imx274_hdr.o
sensor_imx274_hdr-y := src/drv_ms_cus_imx274_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) += sensor_ps5260.o
sensor_ps5260-y := src/drv_ms_cus_PS5260_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) += sensor_ps5268.o
sensor_ps5268-y := src/drv_ms_cus_PS5268_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) += sensor_SC4238.o
sensor_SC4238-y := src/drv_ms_cus_SC4238_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) += sensor_sc2315.o
sensor_sc2315-y := src/drv_ms_cus_sc2315_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) += sensor_ps5250.o
sensor_ps5250-y := src/drv_ms_cus_PS5250_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) += sensor_ps5520.o
sensor_ps5520-y := src/drv_ms_cus_PS5520_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_imx415_hdr.o
sensor_imx415_hdr-y := src/drv_ms_cus_imx415_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_imx477.o
sensor_imx477-y := src/drv_ms_cus_imx477_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_ov2718.o
sensor_ov2718-y := src/drv_ms_cus_ov2718_MIPI.o

obj-$(CONFIG_MS_SENSORDRIVER_INFINITY) +=  sensor_rn6852.o
sensor_rn6852-y := src/drv_ms_cus_rn6852_MIPI.o

