OUTPUTDIR:=$(CURDIR)/output
IMAGEDIR:=$(CURDIR)/output/images
RESOUCE:=_resource
FSTYPE:=_fstype
PATSIZE:=_patsize
MOUNTPT:=_mountpoint
MOUNTTG:=_mounttarget
OPTIONS:=_options
BOOTENV:=_bootenv
MTDPART:=_mtdpart
BOOTTAB0:=_boottab0
BOOTTAB1:=_boottab1
SYSTAB:=_systab
CISSIZE:=_cissize
BOOTADDR:=_bootaddr
BOOTCMD:=_bootcmd
BOOTREC:=_bootrec
COPIES:=_copies
BLKSIZE:=_blksize
DATASIZE:=_datasize
PGSIZE:=_pgsize
OTABLK:=_otablk
BBMCFG:=_bbmcfg
BLKENV:=_blkenv

include ../configs/current.configs
ifneq ($(IMAGE_CONFIG),)
include configs/$(CHIP)/$(IMAGE_CONFIG)
endif
PARTS_TARGETS:=rootfs_recovery

define filesize
$(shell printf 0x%x $(shell stat -c "%s" $(1)))
endef

define sum
$(shell val=0;$(foreach n,$(1),val=$$[$${val}+$(n)];)printf "0x%x" $${val})
endef

define multiplyhex
$(shell printf "0x%x" $(shell echo $$[$(1)*$(2)]))
endef

define alignup
$(shell printf "0x%x" $(shell echo $$[$$[$(1)+$(2)-1]&$$[~$$[$(2)-1]]]))
endef

ifeq ($(VERBOSE),1)
  Q =
else
  Q = @
endif

PRODUCT_NEAT:=$(patsubst %-fastboot,%,$(PRODUCT))

include configs/$(CHIP)/script_$(PRODUCT_NEAT).mk
ifeq ($(patsubst $(PRODUCT_NEAT)%,%,$(PRODUCT)),)
include configs/$(CHIP)/rootfs_$(PRODUCT_NEAT).mk
PARTS_TARGETS+=$(foreach nod, $(USR_MOUNT_BLOCKS),$(nod)_$(PRODUCT_NEAT))
else
PARTS_TARGETS+=
include configs/$(CHIP)/rootfs_fastboot_$(PRODUCT_NEAT).mk
endif

## Fake Object to be occupied
$(PARTS_TARGETS):

PARTS_TARGETS_MK=$(wildcard $(foreach n,$(PARTS_TARGETS),configs/$(CHIP)/$(n).mk))
PARTS_TARGETS_MK_DEFAULT=$(patsubst %_$(PRODUCT_NEAT).mk, %.mk , $(filter-out $(PARTS_TARGETS_MK),$(foreach n,$(PARTS_TARGETS),configs/$(CHIP)/$(n).mk)))

-include $(PARTS_TARGETS_MK)

-include configs/$(CHIP)/ramdisk.mk
include image.mk
include ota.mk
-include $(PROJ_ROOT)/intre_apps/app.mk

PERF:=$(patsubst %,tar xf build/%.tar.gz -C $(OUTPUTDIR)/rootfs,$(PERF_NAME))
STAP:=$(patsubst %,tar xf build/%.tar.gz -C $(OUTPUTDIR)/rootfs,$(STAP_NAME))

prepare:
	@mkdir -p $(OUTPUTDIR)
	@mkdir -p $(IMAGEDIR)
	@mkdir -p $(IMAGEDIR)/boot
	@mkdir -p $(foreach n,$(USR_MOUNT_BLOCKS),$($(n)$(RESOUCE)))

all: prepare
	$(MAKE) intre_apps
	$(MAKE) rootfs
	$(MAKE) ramdisk
	$(MAKE) $(PARTS_TARGETS)
	$(MAKE) images
	$(MAKE) scripts

clean:
	rm -rf $(OUTPUTDIR)
	$(MAKE) intre_apps_clean

