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
else
ifeq ($(FLASH_TYPE), spinand)
FLASH_INIT = "tftp $(TFTPDOWNLOADADDR) boot/flash_list.sni\\nnand probe $(TFTPDOWNLOADADDR)"
FLASH_PROBE = ""
FLASH_WRITE = "nand write"
FLASH_ERASE_PART = "nand erase.part"
FLASH_WRITE_PART = "nand write.e"
FLASH_READ = "nand read.e"
endif
endif

TARGET_SCRIPT:=$(foreach n,$(IMAGE_LIST),$(n)_$(FLASH_TYPE)_$($(n)$(FSTYPE))_script) $(FLASH_TYPE)_config_script
TARGET_FS:=$(filter-out $(patsubst %_fs__,%,$(filter %_fs__, $(foreach n,$(IMAGE_LIST),$(n)_fs_$($(n)$(FSTYPE))_))), $(IMAGE_LIST))
TARGET_UBIFS := $(patsubst %_fs_ubifs_, %, $(filter %_fs_ubifs_, $(foreach n,$(TARGET_FS),$(n)_fs_$($(n)$(FSTYPE))_)))
TARGET_SQUAFS := $(patsubst %_fs_squashfs_, %,$(filter %_fs_squashfs_, $(foreach n,$(TARGET_FS),$(n)_fs_$($(n)$(FSTYPE))_)))
TARGET_RAMFS := $(patsubst %_fs_ramfs_, %,$(filter %_fs_ramfs_, $(foreach n,$(TARGET_FS),$(n)_fs_$($(n)$(FSTYPE))_)))
TARGET_JIFFS2 := $(patsubst %_fs_jffs2_, %, $(filter %_fs_jffs2_, $(foreach n,$(TARGET_FS),$(n)_fs_$($(n)$(FSTYPE))_)))
TARGET_NONEFS := $(filter-out $(TARGET_FS), $(filter-out $(patsubst %_fs__sz__, %, $(filter %_fs__sz__, $(foreach n,$(IMAGE_LIST),$(n)_fs_$($(n)$(FSTYPE))_sz_$($(n)$(PATSIZE))_))), $(IMAGE_LIST)))
SCRIPTDIR:=$(IMAGEDIR)/scripts

scripts:
	mkdir -p $(SCRIPTDIR)
	$(MAKE) set_partition
	$(MAKE) $(TARGET_SCRIPT)
	@echo "# <- this is for comment / total file size must be less than 4KB" > $(IMAGEDIR)/auto_update.txt
	@echo estar scripts/[[set_partition.es >> $(IMAGEDIR)/auto_update.txt
	@$(foreach n,$(IMAGE_LIST),echo "estar scripts/[[$(n).es" >> $(IMAGEDIR)/auto_update.txt;)
	@echo estar scripts/set_config >> $(IMAGEDIR)/auto_update.txt
	@echo saveenv >> $(IMAGEDIR)/auto_update.txt
	@echo printenv >> $(IMAGEDIR)/auto_update.txt
	@echo reset >> $(IMAGEDIR)/auto_update.txt
	@echo "% <- this is end of file symbol" >> $(IMAGEDIR)/auto_update.txt

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
	@echo setenv bootargs  $(rootfs$(BOOTENV)) $(kernel$(BOOTENV)) $(EXBOOTARGS) \$${mtdparts} >> $(SCRIPTDIR)/set_config
ifeq ($(DUAL_OS), on)
ifeq ($(FLASH_TYPE), spinand)
	@echo setenv bootcmd \' $(FLASH_READ) $(RTOS_LOAD_ADDR) RTOS $(rtos$(PATSIZE)) \; dcache off \; go $(RTOS_LOAD_ADDR) >> $(SCRIPTDIR)/set_config
else
	@echo setenv bootcmd \' $(FLASH_PROBE) \; $(FLASH_READ) $(RTOS_LOAD_ADDR) RTOS $(rtos$(PATSIZE)) \; dcache off \; go $(RTOS_LOAD_ADDR) >> $(SCRIPTDIR)/set_config
endif
else
	@echo setenv bootcmd \' $(kernel$(BOOTCMD)) $(rootfs$(BOOTCMD)) bootm $(KERNELBOOTADDR)\;$(kernel$(BOOTREC)) $(rootfs$(BOOTREC)) dcache on \; bootm $(KERNELBOOTADDR) >> $(SCRIPTDIR)/set_config
endif
	if [ -a ../parser_IPL.sh ] ; \
	then \
		sh ../parser_IPL.sh $($(patsubst %_nofsimage,%,ipl)$(RESOUCE)) $(SCRIPTDIR) ;\
	fi;
	@echo saveenv >> $(SCRIPTDIR)/set_config
	@echo reset >> $(SCRIPTDIR)/set_config
	@echo "% <- this is end of file symbol" >> $(SCRIPTDIR)/set_config

