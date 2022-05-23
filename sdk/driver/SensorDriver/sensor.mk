M ?= $(CURDIR)
-include $(PROJ_ROOT)/../sdk/interface/compile_options.mk
ifneq ($(CUSTOMER_OPTIONS),)
-include $(PROJ_ROOT)/release/customer_options/$(CUSTOMER_OPTIONS)
endif
include $(PROJ_ROOT)/configs/current.configs

KDIR?=$(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)

ifeq ($(CHIP), i2)
MSTAR_INC_DIR:= $(PROJ_ROOT)/../sdk/driver/SensorDriver/drv/inc \
				$(PROJ_ROOT)/../sdk/driver/SensorDriver/drv/pub \
				$(PROJ_ROOT)/release/$(PRODUCT)/include \
				$(PROJ_ROOT)/release/$(PRODUCT)/include/drivers/sensorif
else
MSTAR_INC_DIR:= $(PROJ_ROOT)/../sdk/driver/SensorDriver/drv/inc \
				$(PROJ_ROOT)/../sdk/driver/SensorDriver/drv/pub \
				$(PROJ_ROOT)/release/include \
				$(PROJ_ROOT)/release/include/drivers/sensorif
endif

EXTRA_CFLAGS:= -g -Werror -Wall -Wno-unused-result -pipe \
			-DEXTRA_MODULE_NAME=$(MODULE_DIR) $(EXTRA_INCS) \
			-DMI_DBG=$(MI_DBG) \
			-DSUPPORT_HDMI_VGA_DIRECT_MODE=${SUPPORT_HDMI_VGA_DIRECT_MODE} \
			-DSUPPORT_DIVP_USE_GE_SCALING_UP=${SUPPORT_DIVP_USE_GE_SCALING_UP} \
			-DSUPPORT_VIF_USE_GE_FILL_BUF=${SUPPORT_VIF_USE_GE_FILL_BUF} \
			-DSUPPORT_MsOS_MPool_Add_PA2VARange=${SUPPORT_MsOS_MPool_Add_PA2VARange} \
			-DSUPPORT_DISP_ALIGN_UP_OFFSET32=${SUPPORT_DISP_ALIGN_UP_OFFSET32} \
			$(foreach n,$(MSTAR_INC_DIR),-I$(n)) $(foreach n,$(INTERFACE_ENABLED),-DINTERFACE_$(shell tr 'a-z' 'A-Z' <<< $(n))=1) $(foreach n,$(INTERFACE_DISABLED),-DINTERFACE_$(shell tr 'a-z' 'A-Z' <<< $(n))=0) $(foreach n,$(MHAL_ENABLED),-DMHAL_$(shell tr 'a-z' 'A-Z' <<< $(n))=1) $(foreach n,$(MHAL_DISABLED),-DMHAL_$(shell tr 'a-z' 'A-Z' <<< $(n))=0)

ifeq ($(CHIP), i2)
EXTRA_CFLAGS+= -DSUPPORT_VDEC_MULTI_RES=1
EXTRA_CFLAGS+= -DCONFIG_MSTAR_CHIP_I2=1
else ifeq ($(CHIP), k6)
EXTRA_CFLAGS+= -DCONFIG_MSTAR_CHIP_K6=1
else ifeq ($(CHIP), k6l)
EXTRA_CFLAGS+= -DCONFIG_MSTAR_CHIP_K6LITE=1
else ifeq ($(CHIP), i6e)
EXTRA_CFLAGS+= -DCONFIG_SIGMASTAR_CHIP_I6E=1
else ifeq ($(CHIP), i6b0)
EXTRA_CFLAGS+= -DCONFIG_SIGMASTAR_CHIP_I6B0=1
endif

KBUILD_EXTRA_SYMBOLS:=


ifeq ($(KERNEL_VERSION), 3.18.30)
EXTRA_CFLAGS+= -I$(KDIR)/drivers/mstar/include/ -I$(KDIR)/drivers/mstar/cpu/include/ -I$(KDIR)/drivers/mstar/miu/

#$(KDIR)/include $(KDIR)/drivers/mstar/include $(KDIR)/drivers/mstar/cpu/include/ $(KDIR)/drivers/mstar/miu/ \
#$(KDIR)/drivers/mstar/mma_heap/ $(KDIR)/mm \

else
ifeq ($(KERNEL_VERSION), 4.9.84)
EXTRA_CFLAGS+= -I$(KDIR)/drivers/sstar/include/ -I$(KDIR)/drivers/sstar/cpu/include/ -I$(KDIR)/drivers/sstar/miu/
else
EXTRA_CFLAGS+= -I$(KDIR)/mstar2/include/
endif
endif

ifeq ($(PROJ_ROOT)/../sdk/mhal/$(CHIP)/utpa, $(wildcard $(PROJ_ROOT)/../sdk/mhal/$(CHIP)/utpa$))
MSTAR_INC_DIR+=$(PROJ_ROOT)/../sdk/mhal/include/utopia
EXTRA_CFLAGS += -I$(PROJ_ROOT)/../sdk/mhal/include/utopia
endif

MVXV_START := MVX3
MVXV_LIB_TYPE := $(shell echo $(CHIP) | tr a-z A-Z)
MS_PLATFORM_ID := __
COMMITNUMBER_SENSOR := g$(shell cd $(PROJ_ROOT)/../sdk/driver/SensorDriver; git log --format=%h -n 1 2> /dev/null)$(shell git diff --quiet 2> /dev/null || echo -dirty )$(shell cd -;)
COMP_ID_SENSOR := [sensor_module]
MVXV_EXT_SENSOR := [$(shell cd $(PROJ_ROOT)/../sdk/driver/SensorDriver; git rev-parse --abbrev-ref HEAD 2> /dev/null | sed -e 's/\//_/g')$(shell cd -;) build.$(shell date +%Y%m%d%H%M)
MVXV_END := ]XVM
EXTRA_CFLAGS += -DSENSOR_MODULE_VERSION="$(MVXV_START)$(MVXV_LIB_TYPE)$(MS_PLATFORM_ID)$(COMMITNUMBER_SENSOR)$(COMP_ID_SENSOR)$(MVXV_EXT_SENSOR)$(MVXV_END)"

MI_SENSOR_NAME:=$(MOD_PREFIX)$(SENSOR_NAME)

ifneq ($(SENSOR_FILES),)
obj-m := $(patsubst %.c, %.o, $(SENSOR_FILES))
THIS_KO:=$(M)/*.ko
else
obj-m := $(SENSOR_FILE).o
THIS_KO:=$(M)/$(MI_SENSOR_NAME).ko
endif

$(warning obj-m:$(obj-m))

## src file need fix
##$(MI_SENSOR_NAME)-objs+= $(patsubst %.c, %.o, $(WRAPPER_FILE))


modules:
	@echo $(MSTAR_INC_DIR)
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules
	@rename -f 's/$(SENSOR_HEADER)//g' $(THIS_KO)

modules_install:
	$(MAKE) module_install

modules_clean:
	$(MAKE) module_clean

ifeq ($(filter $(SENSOR_FILE).c, $(SOURCE_FILE)),)
module:
	@echo $(MSTAR_INC_DIR)
	$(MAKE) -C $(KDIR) M=$(CURDIR) modules MI_MODULE_NAME=$(MI_SENSOR_NAME)
	@mv $(patsubst %,$(M)/%.ko,$(patsubst %.o,%.c,$(SENSOR_FILE))) $(THIS_KO)
else
module:
	@echo rename source file $(MI_SENSOR_NAME).c
	@exit;
endif

module_install:
ifeq ($(CHIP), i5)
	cp -f $(THIS_KO) $(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/$(BOARD)/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/lib/modules/$(KERNEL_VERSION)
else ifeq ($(CHIP), i2)
	cp -f $(THIS_KO) $(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/$(BOARD)/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/lib/modules/$(KERNEL_VERSION)
else
	cp -f $(THIS_KO) $(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/common/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/modules/$(KERNEL_VERSION)
endif

module_clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean
