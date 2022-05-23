#
# Makefile for SigmaStar camdriver.

#-------------------------------------------------------------------------------
#   Description of some variables owned by the library
#-------------------------------------------------------------------------------
# Library module (lib) or Binary module (bin)
PROCESS = lib
ifeq ($(BB_CHIP_ID),infinity6)
    PP_OPT_COMMON += CONFIG_ARCH_INFINITY6
else ifeq ($(BB_CHIP_ID),infinity5)
    PP_OPT_COMMON += CONFIG_ARCH_INFINITY5
endif

#======================================
#  include sensordriver's mak
#======================================
include $(PATH_sensordriver)/drv/rtk_sensor.mak

#-------------------------------------------------------------------------------
#   List of source files of the library or executable to generate
#-------------------------------------------------------------------------------
