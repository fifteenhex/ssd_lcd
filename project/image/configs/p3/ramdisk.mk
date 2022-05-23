.PHONY: ramdisk

LIB_DIR_PATH:=$(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/common/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)

RAMDISK_DIR?=$(OUTPUTDIR)/tmprd
RAMDISK_IMG?=$(ramdisk$(RESOUCE))

spinand_ramdisk:

nor_ramdisk:
	@echo =====no support!=====

ifneq ($(ramdisk$(RESOUCE)),)
ramdisk:$(FLASH_TYPE)_ramdisk
else
ramdisk:
	@echo NO RAMDISK RES!
endif

