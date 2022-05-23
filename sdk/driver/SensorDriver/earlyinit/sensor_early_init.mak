#
# Makefile for SigmaStar camdriver.

#-------------------------------------------------------------------------------
#   Description of some variables owned by the library
#-------------------------------------------------------------------------------
# Library module (lib) or Binary module (bin)
PROCESS = lib
#PATH_sensor_earlyinit_hal = $(PATH_sensor_earlyinit)/hal/$(BB_CHIP_ID)

PATH_C +=\
    $(PATH_sensordriver)/drv/src\
    $(PATH_sensordriver)/earlyinit/drv/src

PATH_H +=\
    $(PATH_cam_os_wrapper)/pub\
    $(PATH_sensordriver)/drv/inc\
    $(PATH_sensordriver)/earlyinit/drv/pub\
    $(PATH_camdriver)/earlyinit/drv/pub\
    $(PATH_camdriver)/earlyinit/drv/inc\
    $(PATH_camdriver)/sensorif/drv/pub\
    $(PATH_camdriver)/common \
    $(PATH_camdriver)/cmdq_service/drv/pub

#-------------------------------------------------------------------------------
#   List of source files of the library or executable to generate
#-------------------------------------------------------------------------------

ifeq ($(call FIND_COMPILER_OPTION, _SENSOR_EARLYINIT_), TRUE)
#  SRC_C_LIST += 
endif

ifeq ($(call FIND_COMPILER_OPTION, _SENSOR_EARLYINIT_), FALSE)
  SRC_C_LIST += drv_earlyinit_dummy_sensor.c
else

ifneq (,$(filter _SENSOR_PS5520_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_ps5520.c
endif

ifneq (,$(filter _SENSOR_IMX323_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_imx323.c
endif

ifneq (,$(filter _SENSOR_PS5250_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_ps5250.c
endif

ifneq (,$(filter _SENSOR_PS5250_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_ps5250.c
endif

ifneq (,$(filter _SENSOR_PS5270_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_ps5270.c
endif

ifneq (,$(filter _SENSOR_PS5268_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_ps5268.c
endif

ifneq (,$(filter _SENSOR_IMX307_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_imx307.c
endif

ifneq (,$(filter _SENSOR_SC4238_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_sc4238.c
endif

ifneq (,$(filter _SENSOR_IMX415_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_imx415.c
endif

ifneq (,$(filter _SENSOR_F32_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_f32.c
endif

ifneq (,$(filter _SENSOR_PS5258_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_ps5258.c
endif

ifneq (,$(filter _SENSOR_PS5260_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_ps5260.c
endif
ifneq (,$(filter _SENSOR_SC210IOT_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_sc210iot.c
endif

ifneq (,$(filter _SENSOR_H66_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_h66.c
endif
ifneq (,$(filter _SENSOR_GC1054_,$(SENSOR_SET)))
  SRC_C_LIST += drv_earlyinit_gc1054.c
endif

endif

