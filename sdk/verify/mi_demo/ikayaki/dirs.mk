LINK_TYPE ?= static

INC  += $(PROJ_ROOT)/release/include
INC  += $(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)/include/uapi/mstar
INC  += $(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)/drivers/sstar/include

INC  += $(DB_BUILD_TOP)/internal/common
LIBS += -lrt -lpthread -lm -ldl -lcam_fs_wrapper -lcam_os_wrapper
LIBS += -lmi_sys -lmi_common -lmi_vpe -lmi_isp -lmi_sensor -lmi_venc -lmi_divp -lFD_FR_ARM -lcus3a -lispalgo
#
LIBS += -L$(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/common/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/mi_libs/$(LINK_TYPE)
LIBS += -L$(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/common/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/ex_libs/$(LINK_TYPE)

LIBS += -L./lib

ifeq ($(DUAL_OS), on)
CODEDEFINE += -DLINUX_FLOW_ON_DUAL_OS
endif
