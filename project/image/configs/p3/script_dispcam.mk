TFTPDOWNLOADADDR?=0x21000000
TFTPDOWNLOADADDR_PART_PNI?=0x21800000
KERNELBOOTADDR?=0x22000000
INITRAMFSLOADADDR?=0x23000000

ifeq ($(FLASH_TYPE), nor)
FLASH_INIT = "tftp $(TFTPDOWNLOADADDR) boot/flash_list.nri\\nsf probe $(TFTPDOWNLOADADDR)"
FLASH_PROBE = "sf probe 0"
FLASH_WRITE = "sf write"
FLASH_ERASE_PART = "sf erase"
FLASH_WRITE_PART = "sf write"
FLASH_READ = "sf read"
FLASH_INIT_SCRIPT = set_partition
else
ifeq ($(FLASH_TYPE), spinand)
FLASH_INIT = "tftp $(TFTPDOWNLOADADDR) boot/flash_list.sni\\nnand probe $(TFTPDOWNLOADADDR)"
FLASH_PROBE = ""
FLASH_WRITE = "nand write"
FLASH_ERASE_PART = "nand erase.part"
FLASH_WRITE_PART = "nand write.e"
FLASH_READ = "nand read.e"
FLASH_INIT_SCRIPT = set_partition
else
ifeq ($(FLASH_TYPE), sdmmc)
FLASH_INIT = ""
FLASH_PROBE = ""
FLASH_WRITE = "fatwrite mmc 0:1 "
FLASH_ERASE_PART = "fdisk -e 0:"
FLASH_WRITE_PART = "fdisk -w 0:"
FLASH_READ = "fdisk -r 0:"
FLASH_INIT_SCRIPT = sdmmc_partition_init
PAT_FAT_LIST = $(patsubst %_FAT32,%,$(filter, _FAT32, $(foreach n,$(USER_PART_LIST), $(n)_$($(n)TYPE))))
endif
endif
endif

ifeq ($(LINUX_FLASH_TYPE), emmc)
TARGET_SCRIPT:=$(LINUX_FLASH_TYPE)_init_script $(foreach n,$(LINUX_IMAGE_LIST),$(n)_$(LINUX_FLASH_TYPE)_$($(n)$(FSTYPE))_script) $(LINUX_FLASH_TYPE)_config_script
TARGET_SCRIPT+=$(foreach n,$(IMAGE_LIST),$(n)_$(FLASH_TYPE)_$($(n)$(FSTYPE))_script)
else
TARGET_SCRIPT:=$(foreach n,$(IMAGE_LIST),$(n)_$(FLASH_TYPE)_$($(n)$(FSTYPE))_script) $(FLASH_TYPE)_config_script
endif
TARGET_FS:=$(filter-out $(patsubst %_fs__,%,$(filter %_fs__, $(foreach n,$(IMAGE_LIST),$(n)_fs_$($(n)$(FSTYPE))_))), $(IMAGE_LIST))
TARGET_UBIFS := $(patsubst %_fs_ubifs_, %, $(filter %_fs_ubifs_, $(foreach n,$(TARGET_FS),$(n)_fs_$($(n)$(FSTYPE))_)))
TARGET_SQUAFS := $(patsubst %_fs_squashfs_, %,$(filter %_fs_squashfs_, $(foreach n,$(TARGET_FS),$(n)_fs_$($(n)$(FSTYPE))_)))
TARGET_RAMFS := $(patsubst %_fs_ramfs_, %,$(filter %_fs_ramfs_, $(foreach n,$(TARGET_FS),$(n)_fs_$($(n)$(FSTYPE))_)))
TARGET_JIFFS2 := $(patsubst %_fs_jffs2_, %, $(filter %_fs_jffs2_, $(foreach n,$(TARGET_FS),$(n)_fs_$($(n)$(FSTYPE))_)))
TARGET_NONEFS := $(filter-out $(TARGET_FS), $(filter-out $(patsubst %_fs__sz__, %, $(filter %_fs__sz__, $(foreach n,$(IMAGE_LIST),$(n)_fs_$($(n)$(FSTYPE))_sz_$($(n)$(PATSIZE))_))), $(IMAGE_LIST)))
TARGET_SYS_SDMMC_SCRIPT := $(foreach n,$(SYS_IMAGE_LIST),$(n)_$(FLASH_TYPE)_dos_script)
SCRIPTDIR:=$(IMAGEDIR)/scripts

scripts:
	mkdir -p $(SCRIPTDIR)
	$(MAKE) $(FLASH_INIT_SCRIPT)
	$(MAKE) $(TARGET_SCRIPT)
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(IMAGEDIR)/auto_update.txt
	@echo estar scripts/[[$(FLASH_INIT_SCRIPT).es >> $(IMAGEDIR)/auto_update.txt
	@$(foreach n,$(IMAGE_LIST),echo "estar scripts/[[$(n).es" >> $(IMAGEDIR)/auto_update.txt;)
	if [ $(LINUX_FLASH_TYPE) == "emmc" ]; then \
		echo estar scripts/[[init_emmc >> $(IMAGEDIR)/auto_update.txt; \
		$(foreach n,$(LINUX_IMAGE_LIST),echo "estar scripts/[[$(n)" >> $(IMAGEDIR)/auto_update.txt;)	\
	fi;
	@echo estar scripts/set_config >> $(IMAGEDIR)/auto_update.txt
	@echo saveenv >> $(IMAGEDIR)/auto_update.txt
	@echo printenv >> $(IMAGEDIR)/auto_update.txt
	@echo reset >> $(IMAGEDIR)/auto_update.txt
	@echo "% <- this is end of file symbol" >> $(IMAGEDIR)/auto_update.txt
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(IMAGEDIR)/auto_update_bin.txt
	@echo estar scripts_bin/[[set_partition.es >> $(IMAGEDIR)/auto_update_bin.txt
	@$(foreach n,$(IMAGE_LIST),echo "estar scripts_bin/[[$(n).es" >> $(IMAGEDIR)/auto_update_bin.txt;)
	@echo estar scripts_bin/set_config >> $(IMAGEDIR)/auto_update_bin.txt
	@echo saveenv >> $(IMAGEDIR)/auto_update_bin.txt
	@echo printenv >> $(IMAGEDIR)/auto_update_bin.txt
	@echo reset >> $(IMAGEDIR)/auto_update_bin.txt
	@echo "% <- this is end of file symbol" >> $(IMAGEDIR)/auto_update_bin.txt

set_partition:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[set_partition.es
	@echo -e $(FLASH_INIT) >> $(SCRIPTDIR)/[[set_partition.es
ifeq ($(FLASH_TYPE), nor)
	@echo setenv mtdids nor0\=nor0 >> $(SCRIPTDIR)/[[set_partition.es
else
ifeq ($(FLASH_TYPE), spinand)
	@echo setenv mtdids nand0\=nand0 >> $(SCRIPTDIR)/[[set_partition.es
endif
endif
	@echo setenv mtdparts \' $(MTDPARTS) >> $(SCRIPTDIR)/[[set_partition.es
	@echo saveenv >> $(SCRIPTDIR)/[[set_partition.es
ifeq ($(FLASH_TYPE), spinand)
	@echo $(FLASH_ERASE_PART) UBI >> $(SCRIPTDIR)/[[set_partition.es
	@echo ubi part UBI >> $(SCRIPTDIR)/[[set_partition.es
endif
	@echo -e $(foreach n,$(TARGET_UBIFS),ubi create $(n) $($(n)$(PATSIZE))\\n) >> $(SCRIPTDIR)/[[set_partition.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[set_partition.es

cis_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[cis.es
	@echo tftp $(TFTPDOWNLOADADDR) cis.bin >> $(SCRIPTDIR)/[[cis.es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[cis.es
	@echo $(FLASH_ERASE_PART) CIS >> $(SCRIPTDIR)/[[cis.es
	@echo $(FLASH_WRITE) $(TFTPDOWNLOADADDR) $(shell printf 0x%X $(shell expr 0 \* $(shell printf %d $(FLASH_BLK_SIZE)))) \$${filesize} >> $(SCRIPTDIR)/[[cis.es
	@echo $(FLASH_WRITE) $(TFTPDOWNLOADADDR) $(shell printf 0x%X $(shell expr 1 \* $(shell printf %d $(FLASH_BLK_SIZE)))) \$${filesize} >> $(SCRIPTDIR)/[[cis.es
	@echo $(FLASH_WRITE) $(TFTPDOWNLOADADDR) $(shell printf 0x%X $(shell expr 2 \* $(shell printf %d $(FLASH_BLK_SIZE)))) \$${filesize} >> $(SCRIPTDIR)/[[cis.es
	@echo $(FLASH_WRITE) $(TFTPDOWNLOADADDR) $(shell printf 0x%X $(shell expr 3 \* $(shell printf %d $(FLASH_BLK_SIZE)))) \$${filesize} >> $(SCRIPTDIR)/[[cis.es
	@echo $(FLASH_WRITE) $(TFTPDOWNLOADADDR) $(shell printf 0x%X $(shell expr 4 \* $(shell printf %d $(FLASH_BLK_SIZE)))) \$${filesize} >> $(SCRIPTDIR)/[[cis.es
	@echo $(FLASH_WRITE) $(TFTPDOWNLOADADDR) $(shell printf 0x%X $(shell expr 5 \* $(shell printf %d $(FLASH_BLK_SIZE)))) \$${filesize} >> $(SCRIPTDIR)/[[cis.es
	@echo $(FLASH_WRITE) $(TFTPDOWNLOADADDR) $(shell printf 0x%X $(shell expr 6 \* $(shell printf %d $(FLASH_BLK_SIZE)))) \$${filesize} >> $(SCRIPTDIR)/[[cis.es
	@echo $(FLASH_WRITE) $(TFTPDOWNLOADADDR) $(shell printf 0x%X $(shell expr 7 \* $(shell printf %d $(FLASH_BLK_SIZE)))) \$${filesize} >> $(SCRIPTDIR)/[[cis.es
	@echo $(FLASH_WRITE) $(TFTPDOWNLOADADDR) $(shell printf 0x%X $(shell expr 8 \* $(shell printf %d $(FLASH_BLK_SIZE)))) \$${filesize} >> $(SCRIPTDIR)/[[cis.es
	@echo $(FLASH_WRITE) $(TFTPDOWNLOADADDR) $(shell printf 0x%X $(shell expr 9 \* $(shell printf %d $(FLASH_BLK_SIZE)))) \$${filesize} >> $(SCRIPTDIR)/[[cis.es
	@echo mtdparts del CIS >> $(SCRIPTDIR)/[[cis.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[cis.es

ipl_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[ipl.es
	@echo tftp $(TFTPDOWNLOADADDR) ipl_s.bin >> $(SCRIPTDIR)/[[ipl.es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[ipl.es
	@echo $(FLASH_ERASE_PART) IPL0 >> $(SCRIPTDIR)/[[ipl.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) IPL0 \$${filesize} >> $(SCRIPTDIR)/[[ipl.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[ipl.es

ipl_cust_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[ipl_cust.es
	@echo tftp $(TFTPDOWNLOADADDR) ipl_cust_s.bin > $(SCRIPTDIR)/[[ipl_cust.es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[ipl_cust.es
	@echo $(FLASH_ERASE_PART) IPL_CUST0 >> $(SCRIPTDIR)/[[ipl_cust.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) IPL_CUST0 \$${filesize} >> $(SCRIPTDIR)/[[ipl_cust.es
	@echo $(FLASH_ERASE_PART) IPL_CUST1 >> $(SCRIPTDIR)/[[ipl_cust.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) IPL_CUST1 \$${filesize} >> $(SCRIPTDIR)/[[ipl_cust.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[ipl_cust.es

misc_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / isp & iqfile total file size must be less than 1MB in MISC partition" > $(SCRIPTDIR)/[[misc.es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[misc.es
	@echo $(FLASH_ERASE_PART) MISC >> $(SCRIPTDIR)/[[misc.es
	@echo tftp $(TFTPDOWNLOADADDR) misc.bin >> $(SCRIPTDIR)/[[misc.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) MISC \$${filesize} >> $(SCRIPTDIR)/[[misc.es
	@if [ -n "$(misc$(BLKENV))" ]; then	\
		echo -e "$(misc$(BLKENV))" | sed 's/^/setenv /g' >> $(SCRIPTDIR)/[[misc.es;	\
		echo saveenv >> $(SCRIPTDIR)/[[misc.es;	\
	fi;
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[misc.es

kernel_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[kernel.es
	@echo tftp $(TFTPDOWNLOADADDR) kernel >> $(SCRIPTDIR)/[[kernel.es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[kernel.es
	@echo $(FLASH_ERASE_PART) KERNEL >> $(SCRIPTDIR)/[[kernel.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) KERNEL \$${filesize} >> $(SCRIPTDIR)/[[kernel.es
ifeq ($(FLASH_TYPE), spinand)
ifeq ($(PRODUCT), ipc-rtos)
	@echo tftp $(TFTPDOWNLOADADDR) initramfs.gz >> $(SCRIPTDIR)/[[kernel.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) $(LINUX_RAMDISK_ADDR) \$${filesize} >> $(SCRIPTDIR)/[[kernel.es
else ifeq ($(PRODUCT), ipc-rtos-smplh)
	@echo tftp $(TFTPDOWNLOADADDR) initramfs.gz >> $(SCRIPTDIR)/[[kernel.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) $(LINUX_RAMDISK_ADDR) \$${filesize} >> $(SCRIPTDIR)/[[kernel.es
else ifeq ($(PRODUCT), usbcam-rtos)
	@echo tftp $(TFTPDOWNLOADADDR) initramfs.gz >> $(SCRIPTDIR)/[[kernel.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) $(LINUX_RAMDISK_ADDR) \$${filesize} >> $(SCRIPTDIR)/[[kernel.es	
else
	@echo $(FLASH_ERASE_PART) RECOVERY >> $(SCRIPTDIR)/[[kernel.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) RECOVERY \$${filesize} >> $(SCRIPTDIR)/[[kernel.es
endif
endif
	@if [ -n "$(kernel$(BLKENV))" ]; then	\
		echo -e "$(kernel$(BLKENV))" | sed 's/^/setenv /g' >> $(SCRIPTDIR)/[[kernel.es;	\
		echo saveenv >> $(SCRIPTDIR)/[[kernel.es;	\
	fi;
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[kernel.es
	@echo kernel-image done!!!

ramdisk_$(FLASH_TYPE)_gz_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[ramdisk.es
	@echo tftp $(TFTPDOWNLOADADDR) kernel >> $(SCRIPTDIR)/[[ramdisk.es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[ramdisk.es
	@echo $(FLASH_ERASE_PART) RECOVERY >> $(SCRIPTDIR)/[[ramdisk.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) RECOVERY \$${filesize} >> $(SCRIPTDIR)/[[ramdisk.es
	@echo tftp $(TFTPDOWNLOADADDR) initramfs.gz >> $(SCRIPTDIR)/[[ramdisk.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) $(RECOVERY_RAMDISK_ADDR) \$${filesize} >> $(SCRIPTDIR)/[[ramdisk.es
	@echo setenv $(patsubst %_$(FLASH_TYPE)_gz_script,%,$@)_file_size 0x\$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_gz_script,%,$@).es
	@echo setenv initrd_high $(INITRAMFSLOADADDR) >> $(SCRIPTDIR)/[[ramdisk.es
	@echo setenv initrd_size \$${$(patsubst %_$(FLASH_TYPE)_gz_script,%,$@)_file_size} >> $(SCRIPTDIR)/[[ramdisk.es
	@echo saveenv >> $(SCRIPTDIR)/[[ramdisk.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[ramdisk.es

rtos_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[rtos.es
	@echo tftp $(TFTPDOWNLOADADDR) rtos >> $(SCRIPTDIR)/[[rtos.es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[rtos.es
	@echo $(FLASH_ERASE_PART) RTOS >> $(SCRIPTDIR)/[[rtos.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) RTOS \$${filesize} >> $(SCRIPTDIR)/[[rtos.es
ifeq ($(FLASH_TYPE), spinand)
	@echo $(FLASH_ERASE_PART) RTOS_BACKUP >> $(SCRIPTDIR)/[[rtos.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) RTOS_BACKUP \$${filesize} >> $(SCRIPTDIR)/[[rtos.es
endif
	@if [ -n "$(rtos$(BLKENV))" ]; then	\
		echo -e "$(rtos$(BLKENV))" | sed 's/^/setenv /g' >> $(SCRIPTDIR)/[[rtos.es;	\
		echo saveenv >> $(SCRIPTDIR)/[[rtos.es;	\
	fi;
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[rtos.es

uboot_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[uboot.es
	@echo tftp $(TFTPDOWNLOADADDR) uboot_s.bin >> $(SCRIPTDIR)/[[uboot.es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[uboot.es
	@echo $(FLASH_ERASE_PART) UBOOT0 >> $(SCRIPTDIR)/[[uboot.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) UBOOT0 \$${filesize} >> $(SCRIPTDIR)/[[uboot.es
	@echo $(FLASH_ERASE_PART) UBOOT1 >> $(SCRIPTDIR)/[[uboot.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) UBOOT1 \$${filesize} >> $(SCRIPTDIR)/[[uboot.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[uboot.es

boot_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[boot.es
	@echo tftp $(TFTPDOWNLOADADDR) boot.bin >> $(SCRIPTDIR)/[[boot.es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[boot.es
	@echo $(FLASH_ERASE_PART) BOOT0 >> $(SCRIPTDIR)/[[boot.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) BOOT0 \$${filesize} >> $(SCRIPTDIR)/[[boot.es
ifneq ($(cis$(BOOTTAB1)), )
	@echo $(FLASH_ERASE_PART) BOOT1 >> $(SCRIPTDIR)/[[boot.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) BOOT1 \$${filesize} >> $(SCRIPTDIR)/[[boot.es
endif
	@if [ -n "$(boot$(BLKENV))" ]; then	\
		echo -e "$(boot$(BLKENV))" | sed 's/^/setenv /g' >> $(SCRIPTDIR)/[[boot.es;\
		echo saveenv >> $(SCRIPTDIR)/[[boot.es;\
	fi;
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[boot.es

logo_$(FLASH_TYPE)__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[logo.es
	@echo  tftp $(TFTPDOWNLOADADDR) logo >> $(SCRIPTDIR)/[[logo.es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[logo.es
	@echo $(FLASH_ERASE_PART) LOGO >> $(SCRIPTDIR)/[[logo.es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) LOGO \$${filesize} >> $(SCRIPTDIR)/[[logo.es
	@if [ -n "$(logo$(BLKENV))" ]; then	\
		echo -e "$(logo$(BLKENV))" | sed 's/^/setenv /g' >> $(SCRIPTDIR)/[[logo.es;	\
		echo saveenv >> $(SCRIPTDIR)/[[logo.es;	\
	fi;
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[logo.es

%_$(FLASH_TYPE)_lfs_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@).es
	@echo tftp $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@).lfs >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@).es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@).es
	@echo $(FLASH_ERASE_PART) $(shell echo $(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@) |tr a-z A-Z) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@).es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) $(shell echo $(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@) |tr a-z A-Z) \$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@).es
	@if [ -n "$($(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@)$(BLKENV))" ]; then \
		echo -e "$($(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@)$(BLKENV))" | sed 's/^/setenv /g' >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@).es;	\
		echo saveenv >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@).es;	\
	fi;
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_lfs_script,%,$@).es

%_$(FLASH_TYPE)_fwfs_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@).es
	@echo tftp $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@).fwfs >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@).es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@).es
	@echo $(FLASH_ERASE_PART) $(shell echo $(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@) |tr a-z A-Z) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@).es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) $(shell echo $(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@) |tr a-z A-Z) \$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@).es
	@if [ -n "$($(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@)$(BLKENV))" ]; then \
		echo -e "$($(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@)$(BLKENV))" | sed 's/^/setenv /g' >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@).es;	\
		echo saveenv >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@).es;	\
	fi;
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_fwfs_script,%,$@).es

%_$(FLASH_TYPE)_squashfs_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es
	@echo tftp $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).sqfs >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es
	@echo $(FLASH_ERASE_PART) $(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@) \$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es
	@if [ -n "$($(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@)$(BLKENV))" ]; then \
		echo -e "$($(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@)$(BLKENV))" | sed 's/^/setenv /g' >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es;	\
		echo saveenv >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es;	\
	fi;
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_squashfs_script,%,$@).es

%_$(FLASH_TYPE)_ramfs_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@).es
	@echo tftp $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@).ramfs >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@).es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@).es
	@echo $(FLASH_ERASE_PART) $(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@).es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@) \$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@).es
	@echo setenv $(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@)_file_size 0x\$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@).es
	@echo setenv initrd_high ${INITRAMFSLOADADDR} >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@).es
	@echo setenv initrd_size \$${$(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@)_file_size} >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@).es
	@echo setenv initrd_block 110 >> $(SCRIPTDIR)/[[rootfs.es
	@echo saveenv >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@).es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ramfs_script,%,$@).es

%_$(FLASH_TYPE)_ramfs_nocompress_script:%_$(FLASH_TYPE)_ramfs_script
	@echo done

%_$(FLASH_TYPE)_jffs2_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_jffs2_script,%,$@).es
	@echo tftp $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_jffs2_script,%,$@).jffs2 >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_jffs2_script,%,$@).es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_jffs2_script,%,$@).es
	@echo $(FLASH_ERASE_PART) $(patsubst %_$(FLASH_TYPE)_jffs2_script,%,$@) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_jffs2_script,%,$@).es
	@echo $(FLASH_WRITE_PART) $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_jffs2_script,%,$@) \$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_jffs2_script,%,$@).es
	@echo  "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_jffs2_script,%,$@).es

%_$(FLASH_TYPE)_ubifs_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo $(FLASH_PROBE) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo ubi part UBI >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	#@echo -e $(foreach n,$(TARGET_FS),ubi create $(n) $($(n)$(PATSIZE))\\n) >> $(SCRIPTDIR)/[[ipl.es
	#@echo ubi create $(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@) $($(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@)$(PATSIZE)) >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo tftp $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).ubifs >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo ubi write $(TFTPDOWNLOADADDR) $(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@) \$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo "% <- this is end of file symbol" >>  $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es

bootconfig_$(FLASH_TYPE)_ubifs_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo printenv >> $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es
	@echo "% <- this is end of file symbol" >>  $(SCRIPTDIR)/[[$(patsubst %_$(FLASH_TYPE)_ubifs_script,%,$@).es

$(FLASH_TYPE)_partition_script:

$(FLASH_TYPE)_config_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/set_config
	@echo setenv bootargs $(rootfs$(BOOTENV)) $(kernel$(BOOTENV)) $(EXBOOTARGS) \$${mtdparts} >> $(SCRIPTDIR)/set_config
ifeq ($(DUAL_OS), on)
ifeq ($(FLASH_TYPE), spinand)
	@echo setenv bootcmd \' $(FLASH_READ) $(RTOS_LOAD_ADDR) RTOS $(rtos$(PATSIZE)) \; dcache off \; go $(RTOS_LOAD_ADDR) >> $(SCRIPTDIR)/set_config
else
	@echo setenv bootcmd \' $(FLASH_PROBE) \; $(FLASH_READ) $(RTOS_LOAD_ADDR) RTOS $(rtos$(PATSIZE)) \; dcache off \; go $(RTOS_LOAD_ADDR) >> $(SCRIPTDIR)/set_config
endif
else
	@echo setenv bootcmd \' $(kernel$(BOOTCMD)) $(rootfs$(BOOTCMD)) dcache on \; $(bootlogocmd) bootm $(KERNELBOOTADDR)\;$(kernel$(BOOTREC)) $(rootfs$(BOOTREC)) dcache on \; bootm $(KERNELBOOTADDR) >> $(SCRIPTDIR)/set_config
endif
	@echo setenv autoestart 0 >> $(SCRIPTDIR)/set_config
	@echo setenv sstar_bbm off >> $(SCRIPTDIR)/set_config
	if [ -a ../parser_IPL.sh ] ; \
	then \
		sh ../parser_IPL.sh $($(patsubst %_nofsimage,%,ipl)$(RESOUCE)) $(SCRIPTDIR) ;\
	fi;
	@echo saveenv >> $(SCRIPTDIR)/set_config
	@echo reset >> $(SCRIPTDIR)/set_config
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/set_config

emmc_init_script:
	@echo "# <- this is for comment / create emmc partition" > $(SCRIPTDIR)/[[init_emmc
	@echo emmc erase >>  $(SCRIPTDIR)/[[init_emmc
	@echo -e $(foreach n,$(USER_PART_LIST),emmc create $(n) $($(n)SIZE)\\n) >> $(SCRIPTDIR)/[[init_emmc
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[init_emmc

kernel_emmc__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@)
	if [ $(SYSTEM) != "" ]; then \
		echo emmc erase.p $(KERNEL_A_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@); \
		echo tftp $(TFTPDOWNLOADADDR) $(patsubst %_emmc__script,%,$@) >> $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@); \
		echo emmc write.p $(TFTPDOWNLOADADDR) $(KERNEL_A_PAT_NAME) \$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@); \
		if [ $(SYSTEM) == "double" ]; then \
			echo emmc erase.p $(KERNEL_B_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@); \
			echo emmc write.p $(TFTPDOWNLOADADDR) $(KERNEL_B_PAT_NAME) \$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@); \
			echo mmc erase $(RECOVERY_KERNEL_OFFSET) $(RECOVERY_KERNEL_SIZE) >> $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@); \
			echo mmc write $(TFTPDOWNLOADADDR) $(RECOVERY_KERNEL_OFFSET) \$${filesize} >> $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@); \
		fi; \
	else \
		echo tftp $(TFTPDOWNLOADADDR) $(patsubst %_emmc__script,%,$@) >> $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@); \
		echo mmc write $(TFTPDOWNLOADADDR) 0x800 0x1800 >> $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@); \
		echo mmc dev 0 1 >> $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@); \
		echo setenv bootcmd \' mmc read 0x21000000 0x800 0x1800 \; bootm 0x21000000\; >> $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@); \
	fi;
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[$(patsubst %_emmc__script,%,$@)

%_emmc_ext4_script:
	@echo "# <- this is for comment /" > $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@)
	@IMAGE_SIZE="`stat --format=%s $(IMAGEDIR)/$(patsubst %_emmc_ext4_script,%,$@).ext4`"; \
	IMAGE_SIZE_16="0x`echo "obase=16;$$IMAGE_SIZE"|bc`"; \
	IMAGE_BLK_SIZE=$$(($$IMAGE_SIZE/512 + 1)); \
	IMAGE_BLK_SIZE_16="0x`echo "obase=16;$$IMAGE_BLK_SIZE"|bc`"; \
	if [ $$IMAGE_SIZE -gt $(SPLIT_EACH_FILE_SIZE) ]; then \
		split -b $(SPLIT_EACH_FILE_SIZE) $(IMAGEDIR)/$(patsubst %_emmc_ext4_script,%,$@).ext4 $(IMAGEDIR)/$(patsubst %_emmc_ext4_script,%,$@).ext4_; \
		if [ $(patsubst %_emmc_ext4_script,%,$@) = miservice ]; then \
			echo emmc erase.p $(USER_A_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			if [ $(SYSTEM) == "double" ]; then \
				echo emmc erase.p $(USER_B_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				echo mmc erase $(RECOVERY_USER_OFFSET) $(RECOVERY_USER_SIZE) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				offset_rec=`printf %d $(RECOVERY_USER_OFFSET)`; \
			fi; \
		elif [ $(patsubst %_emmc_ext4_script,%,$@) = appconfigs ]; then \
			echo emmc erase.p $(APPCONFIGS_A_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
		elif [ $(patsubst %_emmc_ext4_script,%,$@) = customer ]; then \
			echo emmc erase.p $(DATA_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
		else \
			echo emmc erase.p $(ROOTFS_A_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			if [ $(SYSTEM) == "double" ]; then \
				echo emmc erase.p $(ROOTFS_B_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				echo mmc erase $(RECOVERY_ROOTFS_OFFSET) $(RECOVERY_ROOTFS_SIZE) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				offset_rec=`printf %d $(RECOVERY_ROOTFS_OFFSET)`; \
			fi; \
		fi; \
		offset_blk=0; \
		SPLIT_IMAGE_SIZE_16="0x`echo "obase=16;$(SPLIT_EACH_FILE_SIZE)"|bc`"; \
		SPLIT_IMAGE_BLK_SIZE=$$(($(SPLIT_EACH_FILE_SIZE)/512)); \
		SPLIT_IMAGE_BLK_SIZE_16="0x`echo "obase=16;$$SPLIT_IMAGE_BLK_SIZE"|bc`"; \
		for i in `ls $(IMAGEDIR)/|grep $(patsubst %_emmc_ext4_script,%,$@).ext4_`; do \
			offset_blk_16=0x`echo "obase=16;$$offset_blk"|bc`; \
			if [ $(SYSTEM) == "double" ]; then \
				offset_rec_16=0x`echo "obase=16;$$offset_rec"|bc`; \
			fi; \
			echo tftp $(TFTPDOWNLOADADDR) $$i >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			if [ $(patsubst %_emmc_ext4_script,%,$@) = miservice ]; then \
				echo emmc write.p.continue $(TFTPDOWNLOADADDR) $(USER_A_PAT_NAME) $$offset_blk_16 $$SPLIT_IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				if [ $(SYSTEM) == "double" ]; then \
					echo emmc write.p.continue $(TFTPDOWNLOADADDR) $(USER_B_PAT_NAME) $$offset_blk_16 $$SPLIT_IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
					echo mmc write $(TFTPDOWNLOADADDR) $$offset_rec_16 $$SPLIT_IMAGE_BLK_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				fi; \
			elif [ $(patsubst %_emmc_ext4_script,%,$@) = appconfigs ]; then \
				echo emmc write.p.continue $(TFTPDOWNLOADADDR) $(APPCONFIGS_A_PAT_NAME) $$offset_blk_16 $$SPLIT_IMAGE_SIZE_16>> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			elif [ $(patsubst %_emmc_ext4_script,%,$@) = customer ]; then \
				echo emmc write.p.continue $(TFTPDOWNLOADADDR) $(DATA_PAT_NAME) $$offset_blk_16 $$SPLIT_IMAGE_SIZE_16>> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			else \
				echo emmc write.p.continue $(TFTPDOWNLOADADDR) $(ROOTFS_A_PAT_NAME) $$offset_blk_16 $$SPLIT_IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				if [ $(SYSTEM) == "double" ]; then \
					echo emmc write.p.continue $(TFTPDOWNLOADADDR) $(ROOTFS_B_PAT_NAME) $$offset_blk_16 $$SPLIT_IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
					echo mmc write $(TFTPDOWNLOADADDR) $$offset_rec_16 $$SPLIT_IMAGE_BLK_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				fi; \
			fi; \
			offset_blk=$$(($$offset_blk + $$SPLIT_IMAGE_BLK_SIZE)); \
			if [ $(SYSTEM) == "double" ]; then \
				offset_rec=$$(($$offset_rec + $$SPLIT_IMAGE_BLK_SIZE)); \
			fi; \
		done; \
	else \
		echo tftp $(TFTPDOWNLOADADDR) $(patsubst %_emmc_ext4_script,%,$@).ext4 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
		if [ $(patsubst %_emmc_ext4_script,%,$@) = miservice ]; then \
			echo emmc erase.p $(USER_A_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			echo emmc write.p $(TFTPDOWNLOADADDR) $(USER_A_PAT_NAME) $$IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			if [ $(SYSTEM) == "double" ]; then \
				echo emmc erase.p $(USER_B_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				echo emmc write.p $(TFTPDOWNLOADADDR) $(USER_B_PAT_NAME) $$IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				echo mmc erase $(RECOVERY_USER_OFFSET) $(RECOVERY_USER_SIZE) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				echo mmc write $(TFTPDOWNLOADADDR) $(RECOVERY_USER_OFFSET) $$IMAGE_BLK_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			fi; \
		elif [ $(patsubst %_emmc_ext4_script,%,$@) = customer ]; then\
			echo emmc erase.p $(DATA_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			echo emmc write.p $(TFTPDOWNLOADADDR) $(DATA_PAT_NAME) $$IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
		elif [ $(patsubst %_emmc_ext4_script,%,$@) = appconfigs ]; then\
			echo emmc erase.p $(APPCONFIGS_A_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			echo emmc write.p $(TFTPDOWNLOADADDR) $(APPCONFIGS_A_PAT_NAME) $$IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
		else \
			echo emmc erase.p $(ROOTFS_A_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			echo emmc write.p $(TFTPDOWNLOADADDR) $(ROOTFS_A_PAT_NAME) $$IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			if [ $(SYSTEM) == "double" ]; then \
				echo emmc erase.p $(ROOTFS_B_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				echo emmc write.p $(TFTPDOWNLOADADDR) $(ROOTFS_B_PAT_NAME) $$IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				echo mmc erase $(RECOVERY_ROOTFS_OFFSET) $(RECOVERY_ROOTFS_SIZE) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
				echo mmc write $(TFTPDOWNLOADADDR) $(RECOVERY_ROOTFS_OFFSET) $$IMAGE_BLK_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@); \
			fi; \
		fi; \
	fi;
	@echo  "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[$(patsubst %_emmc_ext4_script,%,$@)

%_emmc_fwfs_script:
	@echo "# <- this is for comment /" > $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@)
	@IMAGE_SIZE="`stat --format=%s $(IMAGEDIR)/$(patsubst %_emmc_fwfs_script,%,$@).fwfs`"; \
	IMAGE_SIZE_16="0x`echo "obase=16;$$IMAGE_SIZE"|bc`"; \
	IMAGE_BLK_SIZE=$$(($$IMAGE_SIZE/512 + 1)); \
	IMAGE_BLK_SIZE_16="0x`echo "obase=16;$$IMAGE_BLK_SIZE"|bc`"; \
	if [ $$IMAGE_SIZE -gt $(SPLIT_EACH_FILE_SIZE) ]; then \
		split -b $(SPLIT_EACH_FILE_SIZE) $(IMAGEDIR)/$(patsubst %_emmc_fwfs_script,%,$@).fwfs $(IMAGEDIR)/$(patsubst %_emmc_fwfs_script,%,$@).fwfs_; \
		echo emmc erase.p $(MISC_A_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
		if [ $(SYSTEM) == "double" ]; then \
			echo emmc erase.p $(MISC_B_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
			echo mmc erase $(RECOVERY_MISC_OFFSET) $(RECOVERY_MISC_SIZE) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
			offset_rec=`printf %d $(RECOVERY_MISC_OFFSET)`; \
		fi; \
		offset_blk=0; \
		SPLIT_IMAGE_SIZE_16="0x`echo "obase=16;$(SPLIT_EACH_FILE_SIZE)"|bc`"; \
		SPLIT_IMAGE_BLK_SIZE=$$(($(SPLIT_EACH_FILE_SIZE)/512)); \
		SPLIT_IMAGE_BLK_SIZE_16="0x`echo "obase=16;$$SPLIT_IMAGE_BLK_SIZE"|bc`"; \
		for i in `ls $(IMAGEDIR)/|grep $(patsubst %_emmc_fwfs_script,%,$@).fwfs_`; do \
			offset_blk_16=0x`echo "obase=16;$$offset_blk"|bc`; \
			if [ $(SYSTEM) == "double" ]; then \
				offset_rec_16=0x`echo "obase=16;$$offset_rec"|bc`; \
			fi; \
			echo tftp $(TFTPDOWNLOADADDR) $$i >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
			echo emmc write.p.continue $(TFTPDOWNLOADADDR) $(MISC_A_PAT_NAME) $$offset_blk_16 $$SPLIT_IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
			if [ $(SYSTEM) == "double" ]; then \
				echo emmc write.p.continue $(TFTPDOWNLOADADDR) $(MISC_B_PAT_NAME) $$offset_blk_16 $$SPLIT_IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
				echo mmc write $(TFTPDOWNLOADADDR) $$offset_rec_16 $$SPLIT_IMAGE_BLK_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
			fi; \
			offset_blk=$$(($$offset_blk + $$SPLIT_IMAGE_BLK_SIZE)); \
			if [ $(SYSTEM) == "double" ]; then \
				offset_rec=$$(($$offset_rec + $$SPLIT_IMAGE_BLK_SIZE)); \
			fi; \
		done; \
	else \
		echo tftp $(TFTPDOWNLOADADDR) $(patsubst %_emmc_fwfs_script,%,$@).fwfs >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
		echo emmc erase.p $(MISC_A_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
		echo emmc write.p $(TFTPDOWNLOADADDR) $(MISC_A_PAT_NAME) $$IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
		if [ $(SYSTEM) == "double" ]; then \
			echo emmc erase.p $(MISC_B_PAT_NAME) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
			echo emmc write.p $(TFTPDOWNLOADADDR) $(MISC_B_PAT_NAME) $$IMAGE_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
			echo mmc erase $(RECOVERY_MISC_OFFSET) $(RECOVERY_MISC_SIZE) >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
			echo mmc write $(TFTPDOWNLOADADDR) $(RECOVERY_MISC_OFFSET) $$IMAGE_BLK_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@); \
		fi; \
	fi;
	@echo  "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[$(patsubst %_emmc_fwfs_script,%,$@)

emmc_config_script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/set_config
	if [ $(SYSTEM) != "" ]; then \
		KERNEL_IMAGE_SIZE="`stat --format=%s $(IMAGEDIR)/kernel`"; \
		KERNEL_IMAGE_SIZE_16="0x`echo "obase=16;$$KERNEL_IMAGE_SIZE"|bc`"; \
		ROOTFS_IMAGE_SIZE="`stat --format=%s $(IMAGEDIR)/rootfs.ext4`"; \
		ROOTFS_IMAGE_SIZE_16="0x`echo "obase=16;$$ROOTFS_IMAGE_SIZE"|bc`"; \
		USER_IMAGE_SIZE="`stat --format=%s $(IMAGEDIR)/miservice.ext4`"; \
		USER_IMAGE_SIZE_16="0x`echo "obase=16;$$USER_IMAGE_SIZE"|bc`"; \
		echo setenv bootargs $(ROOTFS_A_PAT_BOOTENV) $(KERNEL_A_PAT_BOOTENV) $(EXBOOTARGS) \$${mtdparts} >> $(SCRIPTDIR)/set_config; \
		echo setenv bootcmd \' emmc read.p $(TFTPDOWNLOADADDR) $(KERNEL_A_PAT_NAME) $($(KERNEL_A_PAT_NAME)SIZE) \;$(bootlogocmd) bootm $(TFTPDOWNLOADADDR)\; >> $(SCRIPTDIR)/set_config; \
		if [ $(SYSTEM) == "double" ]; then \
			echo setenv bootargsbp $(ROOTFS_B_PAT_BOOTENV) $(KERNEL_B_PAT_BOOTENV) $(EXBOOTARGS)  \$${mtdparts} >> $(SCRIPTDIR)/set_config; \
			echo setenv bootcmdbp \' emmc read.p $(TFTPDOWNLOADADDR) $(KERNEL_B_PAT_NAME) $($(KERNEL_B_PAT_NAME)SIZE) \;$(bootlogocmd) bootm $(TFTPDOWNLOADADDR)\; >> $(SCRIPTDIR)/set_config; \
			#echo setenv reckey \' pin=$(RECOVERY_KEY_PIN),level=$(RECOVERY_KEY_LEVEL) >> $(SCRIPTDIR)/set_config; \
			echo setenv recargs  kernel=$(RECOVERY_KERNEL_OFFSET),$$KERNEL_IMAGE_SIZE_16,$(KERNEL_A_PAT_NAME),$(KERNEL_B_PAT_NAME)\;rootfs=$(RECOVERY_ROOTFS_OFFSET),$$ROOTFS_IMAGE_SIZE_16,$(ROOTFS_A_PAT_NAME),$(ROOTFS_B_PAT_NAME)\;user=$(RECOVERY_USER_OFFSET),$$USER_IMAGE_SIZE_16,$(USER_A_PAT_NAME),$(USER_B_PAT_NAME) >> $(SCRIPTDIR)/set_config; \
		fi; \
	else \
		echo mmc dev 0 1 >> $(SCRIPTDIR)/set_config; \
		echo mmc bootbus 0 1 0 0 >> $(SCRIPTDIR)/set_config; \
		echo mmc partconf 0 1 1 0 >> $(SCRIPTDIR)/set_config; \
		echo setenv bootargs  console=ttyS0,115200 root=/dev/mmcblk0boot1 rootwait rootfstype=squashfs ro init=/linuxrc cma=64M >> $(SCRIPTDIR)/set_config; \
	fi;
	if [ -a ../parser_IPL.sh ] ; \
	then \
		sh ../parser_IPL.sh $($(patsubst %_nofsimage,%,ipl)$(RESOUCE)) $(SCRIPTDIR) ;\
	fi;
	@echo saveenv >> $(SCRIPTDIR)/set_config
	@echo  "% <- this is end of file symbol" >> $(SCRIPTDIR)/set_config

sdmmc_partition_init:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[sdmmc_partition_init.es
	@echo -e mmc rescan >> $(SCRIPTDIR)/[[sdmmc_partition_init.es
	@echo -e fdisk -d 0 >> $(SCRIPTDIR)/[[sdmmc_partition_init.es
	@echo -e $(foreach n,$(SYSTEM_PART_LIST),fdisk -c 0 $($(n)$(CNT))\\n) >> $(SCRIPTDIR)/[[sdmmc_partition_init.es
	@PART_TOTAL_CNT=0; \
	for i in $(foreach n,$(SYSTEM_PART_LIST),$($(n)$(CNT))); do \
		PART_TOTAL_CNT=$$(($$PART_TOTAL_CNT+$$i)); \
	done; \
	if [ $$PART_TOTAL_CNT -gt $$(($(FLASH_BLK_CNT))) ]; then \
		echo -e "\033[41;37m Warning: partition takes capacity($$PART_TOTAL_CNT) is greater than total capacity($(FLASH_BLK_CNT)) \033[0m"; \
	fi;
	@echo -e mmc rescan >> $(SCRIPTDIR)/[[sdmmc_partition_init.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[sdmmc_partition_init.es

fat_sdmmc__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[fat.es
	@echo -e fatformat mmc 0:1 >> $(SCRIPTDIR)/[[fat.es
	@echo -e tftp $(TFTPDOWNLOADADDR) boot/IPL >> $(SCRIPTDIR)/[[fat.es
	@echo -e $(FLASH_WRITE) $(TFTPDOWNLOADADDR) IPL \$${filesize} >> $(SCRIPTDIR)/[[fat.es
	@echo -e tftp $(TFTPDOWNLOADADDR) boot/IPL_CUST >> $(SCRIPTDIR)/[[fat.es
	@echo -e $(FLASH_WRITE) $(TFTPDOWNLOADADDR) IPL_CUST \$${filesize} >> $(SCRIPTDIR)/[[fat.es
	@echo -e tftp $(TFTPDOWNLOADADDR) boot/UBOOT >> $(SCRIPTDIR)/[[fat.es
	@echo -e $(FLASH_WRITE) $(TFTPDOWNLOADADDR) UBOOT \$${filesize} >> $(SCRIPTDIR)/[[fat.es
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[fat.es

system_sdmmc__script:
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(SCRIPTDIR)/[[system.es
	$(MAKE)	$(TARGET_SYS_SDMMC_SCRIPT)
	@$(foreach n,$(SYS_IMAGE_LIST),echo "estar scripts/[[$(n)" >> $(SCRIPTDIR)/[[system.es;)
	@echo  "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[system.es

# sd boot partition -- kernel, rootfs, miservice, customer, misc
%_sdmmc_dos_script:
	@echo "# <- this is for comment /" > $(SCRIPTDIR)/[[$(patsubst %_sdmmc_dos_script,%,$@)
	@IMAGE_NAME=$(patsubst %_sdmmc_dos_script,%,$@);\
	IMAGE_INDEX=$($(patsubst %_sdmmc_dos_script,%,$@)$(INDEX)); \
	if [ "$($(patsubst %_sdmmc_dos_script,%,$@)$(FSTYPE))" == "" ]; then \
		IMAGE_PATH=$(IMAGEDIR)/$$IMAGE_NAME; \
	else \
		IMAGE_PATH=$(IMAGEDIR)/$$IMAGE_NAME.$($(patsubst %_sdmmc_dos_script,%,$@)$(FSTYPE)); \
		IMAGE_NAME=$$IMAGE_NAME.$($(patsubst %_sdmmc_dos_script,%,$@)$(FSTYPE)); \
	fi; \
	IMAGE_SIZE="`stat --format=%s $$IMAGE_PATH`"; \
	IMAGE_SIZE_16="0x`echo "obase=16;$$IMAGE_SIZE"|bc`"; \
	IMAGE_ALIGN=$$(($$IMAGE_SIZE%512)); \
	IMAGE_BLK_SIZE=$$(($$IMAGE_SIZE/512)); \
	if [ $$IMAGE_ALIGN -gt 0 ]; then \
		IMAGE_BLK_SIZE=$$(($$IMAGE_BLK_SIZE+1)); \
	fi; \
	IMAGE_BLK_SIZE_16="0x`echo "obase=16;$$IMAGE_BLK_SIZE"|bc`"; \
	if [ $$IMAGE_SIZE -gt $(SPLIT_EACH_FILE_SIZE) ]; then \
		split -b $(SPLIT_EACH_FILE_SIZE) $$IMAGE_PATH $${IMAGE_PATH}_; \
		echo $(FLASH_ERASE_PART)$$IMAGE_INDEX >> $(SCRIPTDIR)/[[$(patsubst %_sdmmc_dos_script,%,$@); \
		offset_blk=0; \
		SPLIT_IMAGE_SIZE_16="0x`echo "obase=16;$(SPLIT_EACH_FILE_SIZE)"|bc`"; \
		SPLIT_IMAGE_ALIGN=$$(($(SPLIT_EACH_FILE_SIZE)%512)); \
		SPLIT_IMAGE_BLK_SIZE=$$(($(SPLIT_EACH_FILE_SIZE)/512)); \
		if [ $$SPLIT_IMAGE_ALIGN -gt 0 ]; then \
			SPLIT_IMAGE_BLK_SIZE=$$(($$SPLIT_IMAGE_BLK_SIZE+1)); \
		fi; \
		SPLIT_IMAGE_BLK_SIZE_16="0x`echo "obase=16;$$SPLIT_IMAGE_BLK_SIZE"|bc`"; \
		for i in `ls $(IMAGEDIR)/|grep $${IMAGE_NAME}_`; do \
			offset_blk_16=0x`echo "obase=16;$$offset_blk"|bc`; \
			echo tftp $(TFTPDOWNLOADADDR) $$i >> $(SCRIPTDIR)/[[$(patsubst %_sdmmc_dos_script,%,$@); \
			echo $(FLASH_WRITE_PART)$$IMAGE_INDEX $(TFTPDOWNLOADADDR) $$offset_blk_16 $$SPLIT_IMAGE_BLK_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_sdmmc_dos_script,%,$@); \
			offset_blk=$$(($$offset_blk + $$SPLIT_IMAGE_BLK_SIZE)); \
		done; \
	else \
		echo $(FLASH_ERASE_PART)$$IMAGE_INDEX >> $(SCRIPTDIR)/[[$(patsubst %_sdmmc_dos_script,%,$@); \
		echo tftp $(TFTPDOWNLOADADDR) $$IMAGE_NAME >> $(SCRIPTDIR)/[[$(patsubst %_sdmmc_dos_script,%,$@); \
		echo $(FLASH_WRITE_PART)$$IMAGE_INDEX $(TFTPDOWNLOADADDR) 0 $$IMAGE_BLK_SIZE_16 >> $(SCRIPTDIR)/[[$(patsubst %_sdmmc_dos_script,%,$@); \
	fi;\
	if [ "$(patsubst %_sdmmc_dos_script,%,$@)"x == "misc"x ]; then \
		echo setenv $($(patsubst %_sdmmc_dos_script,%,$@)$(MISCENV)) >> $(SCRIPTDIR)/[[$(patsubst %_sdmmc_dos_script,%,$@); \
		echo saveenv >> $(SCRIPTDIR)/[[$(patsubst %_sdmmc_dos_script,%,$@); \
	fi;
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/[[$(patsubst %_sdmmc_dos_script,%,$@)
