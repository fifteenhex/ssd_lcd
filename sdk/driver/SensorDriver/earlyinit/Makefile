SS_ARCH_NAME := $(subst ",,$(CONFIG_SSTAR_CHIP_NAME))

EXTRA_CFLAGS      +=  -Idrivers/sstar/camdriver/sensordriver/drv/inc
EXTRA_CFLAGS      +=  -Idrivers/sstar/camdriver/sensordriver/drv/pub
EXTRA_CFLAGS      +=  -Idrivers/sstar/camdriver/common
EXTRA_CFLAGS      +=  -Idrivers/sstar/cam_os_wrapper/pub
EXTRA_CFLAGS      +=  -Idrivers/sstar/include
EXTRA_CFLAGS      +=  -Idrivers/sstar/camdriver/sensordriver/earlyinit/drv/pub
EXTRA_CFLAGS      +=  -Idrivers/sstar/camdriver/sensordriver/earlyinit/drv/inc
EXTRA_CFLAGS      +=  -Idrivers/sstar/camdriver/sensordriver/earlyinit/hal/$(SS_ARCH_NAME)/inc
EXTRA_CFLAGS      +=  -Idrivers/sstar/camdriver/sensordriver/earlyinit/hal/$(SS_ARCH_NAME)/pub

MVXV_START := MVX3
MVXV_LIB_TYPE := I2
MS_PLATFORM_ID := __
COMMITNUMBER := g$(shell git log --format=%h -n 1 2> /dev/null)$(shell git diff --quiet || echo -dirty )
COMP_ID := [sensor_module]
MVXV_EXT := [$(shell git rev-parse --abbrev-ref HEAD 2> /dev/null | sed -e 's/\//_/g') build.$(shell date +%Y%m%d%H%M)
MVXV_END := ]XVM
EXTRA_CFLAGS += -DSENSOR_MODULE_VERSION="$(MVXV_START)$(MVXV_LIB_TYPE)$(MS_PLATFORM_ID)$(COMMITNUMBER)$(COMP_ID)$(MVXV_EXT)$(MVXV_END)"

EXTRA_CFLAGS      += -D_SENSOR_EARLYINIT_

obj-$(CONFIG_MS_SENSOR_EARLYINIT) +=  sensor_earlyinit.o
sensor_earlyinit-y := drv/src/drv_earlyinit_ps5250.o \
					drv/src/drv_light_sensor.o \
					drv/src/drv_sensor_earlyinit.o \
					drv/src/earlyinit_ut.o \
					drv/src/linux/earlyinit_module.o \
					drv/src/linux/earlyinit_mem_api.o \
					hal/$(SS_ARCH_NAME)/src/hal_earlyinit_metadata_to_cmdq.o\
					hal/$(SS_ARCH_NAME)/src/hal_earlyinit.o\
