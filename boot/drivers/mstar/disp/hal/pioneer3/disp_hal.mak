#
# Makefile for SigmaStar camdriver.

#-------------------------------------------------------------------------------
#	Description of some variables owned by the library
#-------------------------------------------------------------------------------
# Library module (lib) or Binary module (bin)
PROCESS = lib

PATH_disp_hal = $(PATH_camdriver)/disp/hal

PATH_C += \
    $(PATH_disp_hal)/$(BB_CHIP_ID)/src \

PATH_H += \
    $(PATH_disp_hal)/$(BB_CHIP_ID)/inc \
    $(PATH_disp_hal)/$(BB_CHIP_ID)/pub \
    $(PATH_disp_hal)/pub \
    $(PATH_cam_os_wrapper)/pub \
    $(PATH_cam_os_wrapper)/inc \

#-------------------------------------------------------------------------------
#	List of source files of the library or executable to generate
#-------------------------------------------------------------------------------
SRC_C_LIST += \
    hal_disp_if.c \
    hal_disp_irq.c \
    hal_disp.c \
#    abi_headers_disp_hal.c \

