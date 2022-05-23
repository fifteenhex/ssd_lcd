PROJ_ROOT ?=../../../project
include $(PROJ_ROOT)/configs/current.configs


SENSOR_HEADER = drv_ms_cus_

SENSOR_SRC_DIR=drv/src
SENSORSRC = $(notdir  $(shell find ./drv -name *.c ))
SENSOR_SRC = $(filter-out %.mod.c,$(SENSORSRC))
SSTAR_SENSORS?=$(patsubst $(SENSOR_HEADER)%,%,$(patsubst %.c,%, $(SENSOR_SRC)))
SSTAR_SENSORS_CLEAN:=$(patsubst %, %_clean, $(SSTAR_SENSORS))
SSTAR_SENSORS_INSTALL := $(patsubst %, %_install, $(SSTAR_SENSORS))

TARGET_MODULES := $(patsubst %, %_module, $(SSTAR_SENSORS))
TARGET_LIBS:= $(patsubst %, %_lib, $(SSTAR_SENSORS))

MOD_PREFIX:=


export PROJ_ROOT CHIP PRODUCT BOARD TOOLCHAIN TOOLCHAIN_VERSION KERNEL_VERSION CUSTOMER_OPTIONS MOD_PREFIX INTERFACE_ENABLED INTERFACE_DISABLED MHAL_ENABLED MHAL_DISABLED


.PHONY: $(SSTAR_SENSORS)

all:build_makefile
	$(MAKE) -C $(SENSOR_SRC_DIR) modules SENSOR_FILES="$(SENSOR_SRC)" SENSOR_HEADER=$(SENSOR_HEADER)
	@echo all done!

clean:build_makefile
	$(MAKE) -C $(SENSOR_SRC_DIR) modules_clean
	@rm -f $(SENSOR_SRC_DIR)/Makefile
	@echo all clean!

install:build_makefile
	$(MAKE) -C $(SENSOR_SRC_DIR) modules_install SENSOR_FILES="$(SENSOR_SRC)" SENSOR_HEADER=$(SENSOR_HEADER)
	@echo all install!

build_makefile:
	@ln -sf $(CURDIR)/sensor.mk $(SENSOR_SRC_DIR)/Makefile

$(SSTAR_SENSORS):%:build_makefile
	@$(MAKE) $@_module

$(SSTAR_SENSORS_INSTALL):%:build_makefile
	$(MAKE) -C $(SENSOR_SRC_DIR) module_install SENSOR_NAME=$(patsubst %_install,%,$@) SENSOR_FILE=$(patsubst %_install,%,$(patsubst %,$(SENSOR_HEADER)%,$@))
	@if [[ "`ls -A $(CURDIR)/drv/pub`" != "" ]]; then \
		if [[ $(CHIP) == "i2" ]]; then \
			cp -rf $(CURDIR)/drv/pub/drv_ms_cus_sensor.h $(PROJ_ROOT)/release/$(PRODUCT)/include/drivers/sensorif; \
			cp -rf $(CURDIR)/drv/pub/sensor_i2c_api.h $(PROJ_ROOT)/release/$(PRODUCT)/include/drivers/sensorif; \
		else \
			cp -rf $(CURDIR)/drv/pub/drv_ms_cus_sensor.h $(PROJ_ROOT)/release/include/drivers/sensorif; \
			cp -rf $(CURDIR)/drv/pub/sensor_i2c_api.h $(PROJ_ROOT)/release/include/drivers/sensorif; \
		fi; \
	fi;

$(TARGET_MODULES): %_module:build_makefile
	$(MAKE) -C $(SENSOR_SRC_DIR) module SENSOR_NAME=$(patsubst %_module,%,$@) SENSOR_FILE=$(patsubst %_module,$(SENSOR_HEADER)%,$@)

$(SSTAR_SENSORS_CLEAN):%:build_makefile
	$(MAKE) -C $(SENSOR_SRC_DIR) module_clean SENSOR_NAME=$(patsubst %_clean,%,$@) SENSOR_FILE=$(patsubst %_clean,%,$(patsubst %,$(SENSOR_HEADER)%,$@))
