IMAGE_INSTALL_DIR:=$(OUTPUTDIR)

.PHONY: rootfs

RAMDISK_DIR?=$(OUTPUTDIR)/tmprd
RAMDISK_IMG?=$(ramdisk$(RESOUCE))

KBUILD_ROOT:=$(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)
KBUILD_CUST:=$(KBUILD_ROOT)/customize
KERN_MODS_PATH:=$(KBUILD_ROOT)/modules
LIB_DIR_PATH:=$(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/common/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)

LFS_MOUNT_BLK := $(foreach m, $(filter %_lfs, $(foreach block, $(USR_MOUNT_BLOCKS), $(block)_$($(block)$(FSTYPE)))), $(patsubst %_lfs, %, $(m)))
FWFS_MOUNT_BLK := $(foreach m, $(filter %_fwfs, $(foreach block, $(USR_MOUNT_BLOCKS), $(block)_$($(block)$(FSTYPE)))), $(patsubst %_fwfs, %, $(m)))
MOUNT_BLK := $(filter-out $(LFS_MOUNT_BLK), $(USR_MOUNT_BLOCKS))
MOUNT_BLK := $(filter-out $(FWFS_MOUNT_BLK), $(MOUNT_BLK))

ifeq ($(TOOLCHAIN), uclibc)
LIBC=$(strip $(patsubst %.tar.gz, %, $(word 1, $(notdir $(wildcard $(LIB_DIR_PATH)/package/libuclibc-*.tar.gz)))))
else
LIBC=$(strip $(patsubst %.tar.gz, %, $(word 1, $(notdir $(wildcard $(LIB_DIR_PATH)/package/libc-*.tar.gz)))))
endif

rootfs:
	cd rootfs; tar xf rootfs.tar.gz -C $(OUTPUTDIR)
	tar xf busybox/$(BUSYBOX).tar.gz -C $(OUTPUTDIR)/rootfs
	if [ "$(PRODUCT)" != "usbcam-rtos" ]; then \
	tar xf $(LIB_DIR_PATH)/package/$(LIBC).tar.gz -C $(OUTPUTDIR)/rootfs/lib ; \
	fi;

	mkdir -p $(OUTPUTDIR)/rootfs/config
	cp -rf etc/* $(OUTPUTDIR)/rootfs/etc
	if [ "$(appconfigs$(RESOUCE))" != "" ]; then \
		mkdir -p  $(appconfigs$(RESOUCE)); \
		mkdir -p $(OUTPUTDIR)/rootfs/appconfigs;\
	fi;

	if [ $(CONFIG_verify_mixer) = "enable" ]; then \
		if [ $(FLASH_TYPE) = "nor" ]; then \
			if [ "$(FLASH_SIZE)" != "8M" ]; then \
				echo export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:/customer >> ${OUTPUTDIR}/rootfs/etc/profile; \
			fi; \
		else \
			echo export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:/customer >> ${OUTPUTDIR}/rootfs/etc/profile; \
		fi;    \
    fi;

	mkdir -p $(OUTPUTDIR)/rootfs/lib/modules/

	touch ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo mice 0:0 0660 =input/ >> ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo mouse.* 0:0 0660 =input/ >> ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo event.* 0:0 0660 =input/ >> ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo pcm.* 0:0 0660 =snd/ >> ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo control.* 0:0 0660 =snd/ >> ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo timer 0:0 0660 =snd/ >> ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo '$$DEVNAME=bus/usb/([0-9]+)/([0-9]+) 0:0 0660 =bus/usb/%1/%2' >> ${OUTPUTDIR}/rootfs/etc/mdev.conf

	echo export PATH=\$$PATH:/config >> ${OUTPUTDIR}/rootfs/etc/profile
	echo export TERMINFO=/config/terminfo >> ${OUTPUTDIR}/rootfs/etc/profile
	echo export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:/config/lib >> ${OUTPUTDIR}/rootfs/etc/profile
	sed -i '/^mount.*/d' $(OUTPUTDIR)/rootfs/etc/profile
	echo mkdir -p /dev/pts >> ${OUTPUTDIR}/rootfs/etc/init.d/rcS
	echo mount -t sysfs none /sys >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS
	echo mount -t tmpfs mdev /dev >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS
	echo mount -t debugfs none /sys/kernel/debug/ >>  $(OUTPUTDIR)/rootfs/etc/init.d/rcS

	cp -rvf $(PROJ_ROOT)/tools/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/fw_printenv/* $(OUTPUTDIR)/rootfs/etc/
	echo "$(ENV_CFG)" > $(OUTPUTDIR)/rootfs/etc/fw_env.config
	cd $(OUTPUTDIR)/rootfs/etc/;ln -sf fw_printenv fw_setenv
	echo mkdir -p /var/lock >> ${OUTPUTDIR}/rootfs/etc/init.d/rcS

	if [ "$(FLASH_TYPE)"x = "nor"x  ]; then \
		echo mdev -s >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS ;\
	fi;
	echo -e $(foreach block, $(MOUNT_BLK), "mount -t $($(block)$(FSTYPE)) $($(block)$(MOUNTPT)) $($(block)$(MOUNTTG))\n") >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS
	if [ "$(LFS_MOUNT_BLK)" != "" ]; then \
		echo -e mdev -s >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS;	\
		if [ "$(TOOLCHAIN)" = "glibc" ] && [ "$(TOOLCHAIN_VERSION)" = "9.1.0" ]; then \
			tar -vxf $(PROJ_ROOT)/image/fuse/fuse-2.9.9-arm-linux-gnueabihf-glibc-9.1.0.tar.gz -C $(OUTPUTDIR)/rootfs/;	\
			tar -vxf $(PROJ_ROOT)/image/littlefs-fuse/littlefs-fuse-2.2.0-arm-linux-gnueabihf-glibc-9.1.0.tar.gz -C $(OUTPUTDIR)/rootfs/;	\
		fi;	\
		if [ "$(TOOLCHAIN)" = "glibc" ] && [ "$(TOOLCHAIN_VERSION)" = "8.2.1" ]; then \
			tar -vxf $(PROJ_ROOT)/image/fuse/fuse-2.9.9-arm-linux-gnueabihf-glibc-8.2.1.tar.gz -C $(OUTPUTDIR)/rootfs/;	\
			tar -vxf $(PROJ_ROOT)/image/littlefs-fuse/littlefs-fuse-2.2.0-arm-linux-gnueabihf-glibc-8.2.1.tar.gz -C $(OUTPUTDIR)/rootfs/;	\
		fi;	\
		mkdir -p $(OUTPUTDIR)/rootfs/misc;\
		$(foreach block, $(LFS_MOUNT_BLK), $(PROJ_ROOT)/image/makefiletools/script/lfs_mount.py --part_size=$($(block)$(PATSIZE)) --rcs_dir=$(OUTPUTDIR)/rootfs/etc/init.d/rcS $($(block)$(MOUNTPT)) $($(block)$(MOUNTTG));)\
	fi;

	if [ "$(FWFS_MOUNT_BLK)" != "" ]; then \
		echo -e mdev -s >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS;	\
		if [ "$(TOOLCHAIN)" = "glibc" ] && [ "$(TOOLCHAIN_VERSION)" = "9.1.0" ]; then \
			tar -vxf $(PROJ_ROOT)/image/fuse/fuse-2.9.9-arm-linux-gnueabihf-glibc-9.1.0.tar.gz -C $(OUTPUTDIR)/rootfs/;	\
			tar -vxf $(PROJ_ROOT)/image/firmwarefs-fuse/firmwarefs-fuse-2.2.0-arm-linux-gnueabihf-glibc-9.1.0.tar.gz -C $(OUTPUTDIR)/rootfs/;	\
		fi;	\
		if [ "$(TOOLCHAIN)" = "glibc" ] && [ "$(TOOLCHAIN_VERSION)" = "8.2.1" ]; then \
			tar -vxf $(PROJ_ROOT)/image/fuse/fuse-2.9.9-arm-linux-gnueabihf-glibc-8.2.1.tar.gz -C $(OUTPUTDIR)/rootfs/;	\
			tar -vxf $(PROJ_ROOT)/image/firmwarefs-fuse/firmwarefs-fuse-2.2.0-arm-linux-gnueabihf-glibc-8.2.1.tar.gz -C $(OUTPUTDIR)/rootfs/;	\
		fi;	\
		if [ "$(TOOLCHAIN)" = "uclibc" ] && [ "$(TOOLCHAIN_VERSION)" = "9.1.0" ]; then \
			tar -vxf $(PROJ_ROOT)/image/fuse/fuse-2.9.9-arm-sigmastar-linux-uclibcgnueabihf-9.1.0.tar.gz -C $(OUTPUTDIR)/rootfs/;	\
			tar -vxf $(PROJ_ROOT)/image/firmwarefs-fuse/firmwarefs-fuse-2.2.0-arm-sigmastar-linux-uclibcgnueabihf-9.1.0.tar.gz -C $(OUTPUTDIR)/rootfs/;	\
		fi;	\
		mkdir -p $(OUTPUTDIR)/rootfs/misc;\
		$(foreach block, $(FWFS_MOUNT_BLK), $(PROJ_ROOT)/image/makefiletools/script/fwfs_mount.py --flash_type=$(FLASH_TYPE) --block_size=$(FLASH_BLK_SIZE) --page_size=$(FLASH_PG_SIZE) --part_size=$($(block)$(PATSIZE)) --rcs_dir=$(OUTPUTDIR)/rootfs/etc/init.d/rcS $($(block)$(MOUNTPT)) $($(block)$(MOUNTTG));)\
	fi;

	ln -fs /config/modules/$(KERNEL_VERSION) $(OUTPUTDIR)/rootfs/lib/modules/
	find $(OUTPUTDIR)/rootfs/lib/ -name "*.so" | xargs $(STRIP)  --strip-unneeded;
	echo mkdir -p /dev/pts >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS
	echo mount -t devpts devpts /dev/pts >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS
#	echo "busybox telnetd&" >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS
	echo "if [ -e /customer/demo.sh ]; then" >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS
	echo "    /customer/demo.sh" >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS
	echo "fi;" >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS
	#add sshd, default password 1234
	if [[ "$(FLASH_TYPE)"x = "spinand"x ]]; then \
		if [[ $(TOOLCHAIN_VERSION) = "9.1.0" ]] || [[ $(TOOLCHAIN_VERSION) = "8.2.1" ]]; then \
			echo "root::0:0:Linux User,,,:/home/root:/bin/sh" > $(OUTPUTDIR)/rootfs/etc/passwd; \
			echo "sshd:x:74:74:Privilege-separated SSH:/var/empty/sshd:/sbin/nologin" >> $(OUTPUTDIR)/rootfs/etc/passwd; \
			echo "export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:/customer/ssh/lib" >> ${OUTPUTDIR}/rootfs/etc/init.d/rcS; \
			echo "mkdir /var/empty" >> ${OUTPUTDIR}/rootfs/etc/init.d/rcS; \
			echo "/customer/ssh/sbin/sshd -f /customer/ssh/etc/sshd_config" >> ${OUTPUTDIR}/rootfs/etc/init.d/rcS; \
			echo "export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:/customer/ssh/lib" >> ${OUTPUTDIR}/rootfs/etc/profile; \
		fi; \
	fi;
	echo "telnetd&" >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS

	#end add
	# add for usb factory upgrade tool
	cp $(PROJ_ROOT)/board/$(CHIP)/boot/usb/upgrade/usb_updater.bin $(OUTPUTDIR)/images/usb_updater_ipl.bin
	if [ "$(FLASH_TYPE)" = "nor" ]; then \
		if [ "$(ENV_TYPE)" == "dualenv" ]; then \
			cp $(PROJ_ROOT)/board/$(CHIP)/boot/usb/upgrade/u-boot.img_dual.bin $(OUTPUTDIR)/images/usb_updater_boot.bin ; \
		else \
			cp $(PROJ_ROOT)/board/$(CHIP)/boot/usb/upgrade/u-boot.img_dual.bin $(OUTPUTDIR)/images/usb_updater_boot.bin ; \
		fi; \
	else \
		if [ "$(ENV_TYPE)" == "dualenv" ]; then \
			cp $(PROJ_ROOT)/board/$(CHIP)/boot/usb/upgrade/u-boot_spinand.img_dual.bin $(OUTPUTDIR)/images/usb_updater_boot.bin ; \
		else \
			cp $(PROJ_ROOT)/board/$(CHIP)/boot/usb/upgrade/u-boot_spinand.img_single.bin $(OUTPUTDIR)/images/usb_updater_boot.bin ; \
		fi; \
	fi; \
	#end add

	# add for usb factory upgrade tool
	cp $(PROJ_ROOT)/board/$(CHIP)/boot/usb/upgrade/usb_updater.bin $(OUTPUTDIR)/images/usb_updater_ipl.bin
	if [ "$(FLASH_TYPE)" = "nor" ]; then \
		cp $(PROJ_ROOT)/board/$(CHIP)/boot/usb/upgrade/u-boot.img_single.bin $(OUTPUTDIR)/images/usb_updater_boot.bin ; \
	else \
		cp $(PROJ_ROOT)/board/$(CHIP)/boot/usb/upgrade/u-boot_spinand.img_single.bin $(OUTPUTDIR)/images/usb_updater_boot.bin ; \
	fi; \
	#end add

	mkdir -p $(OUTPUTDIR)/vendor
	mkdir -p $(OUTPUTDIR)/customer
	mkdir -p $(OUTPUTDIR)/rootfs/vendor
	mkdir -p $(OUTPUTDIR)/rootfs/customer
	mkdir -p $(OUTPUTDIR)/bootconfig
	mkdir -p $(OUTPUTDIR)/rootfs/bootconfig
