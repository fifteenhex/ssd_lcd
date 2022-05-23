#
# Makefile for SigmaStar camdriver.

#-------------------------------------------------------------------------------
#   Description of some variables owned by the library
#-------------------------------------------------------------------------------
# Library module (lib) or Binary module (bin)
PROCESS = lib

PATH_C +=\
    $(PATH_sensordriver)/drv/src\
    $(PATH_sensordriver)/rtk

PATH_H +=\
    $(PATH_cam_os_wrapper)/pub\
    $(PATH_sensordriver)/drv/inc\
    $(PATH_sensordriver)/drv/pub\
    $(PATH_camdriver)/sensorif/drv/pub
#-------------------------------------------------------------------------------
#   List of source files of the library or executable to generate
#-------------------------------------------------------------------------------
ifneq ($(ABI_VER),)
SRC_C_LIST += abi_headers_sensordriver.c
endif

SRC_C_LIST += rtk_sensor_module_init.c

ifneq (,$(filter _SENSOR_OS02G10_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_OS02G10_MIPI.c
endif

ifneq (,$(filter _SENSOR_IMX307_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_imx307_MIPI.c
endif

ifneq (,$(filter _SENSOR_IMX327_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_imx327_MIPI.c
endif

ifneq (,$(filter _SENSOR_PS5250_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_PS5250_MIPI.c
endif

ifneq (,$(filter _SENSOR_PS5520_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_PS5520_MIPI.c
endif

ifneq (,$(filter _SENSOR_PS5268_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_PS5268_MIPI.c
endif

ifneq (,$(filter _SENSOR_IMX291_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_imx291_MIPI.c
endif

ifneq (,$(filter _SENSOR_IMX323_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_imx323.c
endif

ifneq (,$(filter _SENSOR_PS5270_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_PS5270_MIPI.c
endif

ifneq (,$(filter _SENSOR_NVP6124B_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_nvp6124b.c
endif

ifneq (,$(filter _SENSOR_SC4238_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_SC4238_MIPI.c
endif

ifneq (,$(filter _SENSOR_IMX415_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_imx415_MIPI.c
endif

ifneq (,$(filter _SENSOR_F32_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_F32_MIPI.c
endif

ifneq (,$(filter _SENSOR_PS5258_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_PS5258_MIPI.c
endif

ifneq (,$(filter _SENSOR_PS5260_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_PS5260_MIPI.c
endif
ifneq (,$(filter _SENSOR_SC210IOT_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_sc210iot_MIPI.c
endif

ifneq (,$(filter _SENSOR_H66_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_H66_MIPI.c
endif

ifneq (,$(filter _SENSOR_GC1054_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_gc1054_MIPI.c
endif
ifneq (,$(filter _SENSOR_GC2053_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_gc2053_MIPI.c
endif
ifneq (,$(filter _SENSOR_GC2053_1LANE_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_gc2053_MIPI_1lane.c
endif

ifneq (,$(filter _SENSOR_GC1054_DUAL_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_gc1054_dual_MIPI.c
endif

ifneq (,$(filter _SENSOR_PLECO_,$(SENSOR_SET)))
  SRC_C_LIST += drv_ms_cus_pleco_MIPI.c
endif
#-------------------------------------------------------------------------------
#   Add VCM source file
#-------------------------------------------------------------------------------
SRC_C_LIST += rtk_vcm_module_init.c

ifneq (,$(filter _VCM_DW9714_,$(VCM_SET)))
  SRC_C_LIST += drv_ms_cus_dw9714.c
endif
