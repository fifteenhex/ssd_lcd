include $(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/$(BOARD)/bootscript/$(FLASH_TYPE)/boot_sequence.mk

INIT_FILE=$(OUTPUTDIR)/rootfs/etc/init.sh
LATE_INIT_FILE=$(miservice$(RESOUCE))/demo.sh
KBUILD_ROOT=$(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)/$(CHIP)/configs/$(PRODUCT)/$(BOARD)/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)
RELEASE_ROOT=$(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/common/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)
LIB_DIR_PATH:=$(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/common/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)

ifeq ($(PRODUCT),ipc)
DEMO=$(RELEASE_ROOT)/bin/mixer/mixer
else
ifeq ($(PRODUCT),usbcam)
DEMO=$(RELEASE_ROOT)/bin/mi_demo/prog_uvc
endif
endif

MOD_PATH_KERNEL:=$(KBUILD_ROOT)/$(FLASH_TYPE)/modules/
MOD_PATH_MI:=$(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/
MODULES_PATH:= $(MOD_PATH_KERNEL) $(MOD_PATH_MI)
MODULES_PATH_LIST_FIRST:=$(wildcard $(foreach mod,$(MODULES_1st),$(foreach path, $(MODULES_PATH), $(path)$(mod))))
MODULES_LIST_FIRST:=$(notdir $(wildcard $(foreach mod,$(MODULES_1st),$(foreach path, $(MODULES_PATH), $(path)$(mod)))))
MODULES_PATH_LIST_SECOND:=$(wildcard $(foreach mod,$(MODULES_2nd),$(foreach path, $(MODULES_PATH), $(path)$(mod))))
MODULES_LIST_SECOND:=$(notdir $(wildcard $(foreach mod,$(MODULES_2nd),$(foreach path, $(MODULES_PATH), $(path)$(mod)))))

EX_LIBS=$(RELEASE_ROOT)/ex_libs/dynamic/
MI_LIBS=$(RELEASE_ROOT)/mi_libs/dynamic/
LIBS_PATH:= $(EX_LIBS) $(MI_LIBS)
LIBS_PATH_LIST_FIRST:=$(wildcard $(foreach lib,$(LIBS_1st),$(foreach path, $(LIBS_PATH), $(path)$(lib))))
LIBS_LIST_FIRST:=$(notdir $(wildcard $(foreach lib,$(LIBS_1st),$(foreach path, $(LIBS_PATH), $(path)$(lib)))))
LIBS_PATH_LIST_SECOND:=$(wildcard $(foreach lib,$(LIBS_2nd),$(foreach path, $(LIBS_PATH), $(path)$(lib))))
LIBS_LIST_SECOND:=$(notdir $(wildcard $(foreach lib,$(LIBS_2nd),$(foreach path, $(LIBS_PATH), $(path)$(lib)))))

CONFIG_FILES_DIR=$(OUTPUTDIR)/rootfs/sstar_config

ifeq ($(TOOLCHAIN), uclibc)
LIBC=$(strip $(patsubst %.tar.gz, %, $(word 1, $(notdir $(wildcard $(LIB_DIR_PATH)/package/libuclibc-*.tar.gz)))))
else
LIBC=$(strip $(patsubst %.tar.gz, %, $(word 1, $(notdir $(wildcard $(LIB_DIR_PATH)/package/libc-*.tar.gz)))))
endif

define release_demo
if [ -f $(DEMO) ]; then \
	cp -rf $(DEMO) $(OUTPUTDIR)/customer/; \
	chmod +x $(OUTPUTDIR)/customer/$(notdir $(DEMO)); \
fi;

if [ -f $(DEMO) ]; then \
	$(STRIP)  --strip-unneeded $(OUTPUTDIR)/customer/$(notdir $(DEMO)); \
fi;
endef

.PHONY: rootfs

rootfs:

	cd rootfs; tar xf rootfs.tar.gz -C $(OUTPUTDIR)
	tar xf busybox/$(BUSYBOX).tar.gz -C $(OUTPUTDIR)/rootfs

	## ramdisk/other use /linuxrc , ramfs use /init
	if [ "$(rootfs$(FSTYPE))" = "ramfs" ]; then \
		mv $(OUTPUTDIR)/rootfs/linuxrc $(OUTPUTDIR)/rootfs/init ; \
	fi;

	## /rootfs/config
	mkdir -p $(OUTPUTDIR)/rootfs/config
	mkdir -p $(CONFIG_FILES_DIR)
	mkdir -p $(OUTPUTDIR)/customer

	cp -rf $(PROJ_ROOT)/board/ini/* $(OUTPUTDIR)/customer
	cp -rf $(PROJ_ROOT)/board/$(CHIP)/$(BOARD_NAME)/config/* $(CONFIG_FILES_DIR)
	cp -vf $(PROJ_ROOT)/board/$(CHIP)/mmap/$(MMAP) $(CONFIG_FILES_DIR)/mmap.ini
	cp -rvf $(RELEASE_ROOT)/bin/config_tool/config_tool $(CONFIG_FILES_DIR)
	cd $(CONFIG_FILES_DIR); chmod +x config_tool; ln -sf config_tool dump_config; ln -sf config_tool dump_mmap

	mkdir -p $(CONFIG_FILES_DIR)/iqfile/
	if [ "$(IQ0)" != "" ]; then \
		cp -rvf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(IQ0) $(CONFIG_FILES_DIR)/iqfile/; \
		cd $(CONFIG_FILES_DIR)/iqfile; chmod +x $(notdir $(IQ0)); ln -sf $(notdir $(IQ0)) iqfile0.bin; cd -; \
	fi;
	if [ "$(IQ1)" != "" ]; then \
		cp -rvf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(IQ1) $(CONFIG_FILES_DIR)/iqfile/; \
		cd $(CONFIG_FILES_DIR)/iqfile; chmod +x $(notdir $(IQ1)); ln -sf $(notdir $(IQ1)) iqfile1.bin; cd -; \
	fi;
	if [ "$(IQ2)" != "" ]; then \
		cp -rvf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(IQ2) $(CONFIG_FILES_DIR)/iqfile/; \
		cd $(CONFIG_FILES_DIR)/iqfile; chmod +x $(notdir $(IQ2)); ln -sf $(notdir$ (IQ2)) iqfile2.bin; cd -; \
	fi;
	if [ "$(IQ3)" != "" ]; then \
		cp -rvf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(IQ3) $(CONFIG_FILES_DIR)/iqfile/; \
		cd $(CONFIG_FILES_DIR)/iqfile; chmod +x $(notdir $(IQ3)); ln -sf $(notdir $(IQ3)) iqfile3.bin; cd -; \
	fi;

	if [ -d $(PROJ_ROOT)/board/$(CHIP)/venc_fw ]; then \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/venc_fw $(CONFIG_FILES_DIR); \
	fi;

	## /etc
	cp -rf etc/* $(OUTPUTDIR)/rootfs/etc

	mkdir -p $(OUTPUTDIR)/rootfs/lib/modules/$(KERNEL_VERSION)
	mkdir -p $(miservice$(RESOUCE))/modules/$(KERNEL_VERSION)
	mkdir -p $(miservice$(RESOUCE))/lib

	#### /etc/mdev.conf
	touch ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo mice 0:0 0660 =input/ >> ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo mouse.* 0:0 0660 =input/ >> ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo event.* 0:0 0660 =input/ >> ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo pcm.* 0:0 0660 =snd/ >> ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo control.* 0:0 0660 =snd/ >> ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo timer 0:0 0660 =snd/ >> ${OUTPUTDIR}/rootfs/etc/mdev.conf
	echo '$$DEVNAME=bus/usb/([0-9]+)/([0-9]+) 0:0 0660 =bus/usb/%1/%2' >> ${OUTPUTDIR}/rootfs/etc/mdev.conf

	#### Copy libararies to /lib
	tar xf $(RELEASE_ROOT)/package/$(LIBC).tar.gz -C $(OUTPUTDIR)/rootfs/lib
	-if [ "$(LIBS_PATH_LIST_FIRST)" != "" ];then \
		cp -rvf $(LIBS_PATH_LIST_FIRST) $(OUTPUTDIR)/rootfs/lib/;\
	fi;

	#### Copy libararies to /config/lib
	-if [ "$(LIBS_PATH_LIST_SECOND)" != "" ];then \
		cp -rvf $(LIBS_PATH_LIST_SECOND) $(miservice$(RESOUCE))/lib/;\
	fi;

	#### create /etc/init.sh /config/demo.sh
	if [ -f $(INIT_FILE) ]; then \
		rm  $(INIT_FILE); \
	fi;
	touch $(INIT_FILE)
	chmod 755 $(INIT_FILE)

	if [ -f $(LATE_INIT_FILE) ]; then \
		rm  $(LATE_INIT_FILE); \
	fi;
	touch $(LATE_INIT_FILE)
	chmod 755 $(LATE_INIT_FILE)

	#### /etc/profile
	echo export PATH=\$$PATH:/config:/etc >> ${OUTPUTDIR}/rootfs/etc/profile
	#### echo export TERMINFO=/config/terminfo >> ${OUTPUTDIR}/rootfs/etc/profile
	echo export LD_LIBRARY_PATH=\$$LD_LIBRARY_PATH:/customer\/lib:\/config\/lib >> $(OUTPUTDIR)/rootfs/etc/profile;
	echo "if [ -e /etc/init.sh ]; then" >> $(OUTPUTDIR)/rootfs/etc/profile
	echo "    /etc/init.sh;" >> $(OUTPUTDIR)/rootfs/etc/profile
	echo "fi;" >> $(OUTPUTDIR)/rootfs/etc/profile

	echo mkdir -p /dev/pts >> ${OUTPUTDIR}/rootfs/etc/init.d/rcS
	echo mount -t tmpfs mdev /dev >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS

	#### cp -rvf $(PROJ_ROOT)/tools/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/fw_printenv/* $(OUTPUTDIR)/rootfs/etc/
	#### echo "$(ENV_CFG)" > $(OUTPUTDIR)/rootfs/etc/fw_env.config
	#### cd $(OUTPUTDIR)/rootfs/etc/;ln -sf fw_printenv fw_setenv
	echo mkdir -p /var/lock >> ${OUTPUTDIR}/rootfs/etc/init.d/rcS

	#### mount config block
	echo mdev -s >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS
	echo -e $(foreach block, $(USR_MOUNT_BLOCKS), "mount -t $($(block)$(FSTYPE)) $($(block)$(MOUNTPT)) $($(block)$(MOUNTTG))\n") >> $(OUTPUTDIR)/rootfs/etc/init.d/rcS

	#### Load Necessary Modules Firstly
	-if [ "$(MODULES_LIST_FIRST)" != "" ];then \
		for mod in $(MODULES_LIST_FIRST);do\
			echo "insmod /lib/modules/$(KERNEL_VERSION)/$$mod" >> $(INIT_FILE);\
		done;\
		cp -rvf $(MODULES_PATH_LIST_FIRST) $(OUTPUTDIR)/rootfs/lib/modules/$(KERNEL_VERSION);\
		echo "#mi module" >> $(INIT_FILE); \
	fi;

	if [ "$(SENSOR_LIST)" != "" ]; then \
		cp -rvf $(foreach n,$(SENSOR_LIST),$(RELEASE_ROOT)/modules/$(KERNEL_VERSION)/$(n)) $(OUTPUTDIR)/rootfs/lib/modules/$(KERNEL_VERSION); \
	fi;
	if [ "$(SENSOR0)" != "" ]; then \
		echo insmod /lib/modules/$(KERNEL_VERSION)/$(SENSOR0) $(SENSOR0_OPT) >> $(INIT_FILE); \
	fi;
	if [ "$(SENSOR1)" != "" ]; then \
		echo insmod /lib/modules/$(KERNEL_VERSION)/$(SENSOR1) $(SENSOR1_OPT) >> $(INIT_FILE); \
	fi;
	if [ "$(SENSOR2)" != "" ]; then \
		echo insmod /lib/modules/$(KERNEL_VERSION)/$(SENSOR2) $(SENSOR2_OPT) >> $(INIT_FILE); \
	fi;

	sed -i 's/mi_sys.ko/mi_sys.ko cmdQBufSize=256 logBufSize=256 default_config_path=\/$(notdir $(CONFIG_FILES_DIR)) /g' $(INIT_FILE);
	sed -i 's/mi_venc.ko/mi_venc.ko fw_path=\/$(notdir $(CONFIG_FILES_DIR))\/venc_fw\/chagall.bin /g' $(INIT_FILE);
	sed -i '/#mi module/a	major=`cat /proc/devices | busybox awk "\\\\$$2==\\""mi_poll"\\" {print \\\\$$1}"`\nbusybox mknod \/dev\/mi_poll c $$major 0' $(INIT_FILE);
	echo "echo isproot /$(notdir $(CONFIG_FILES_DIR))/iqfile > /dev/ispmid" >> $(INIT_FILE)
	echo mdev -s >> $(INIT_FILE)

	#### Load Delay Modules Secondly
	-if [ "$(MODULES_LIST_SECOND)" != "" ];then \
		for mod in $(MODULES_LIST_SECOND);do\
			echo "insmod /config/modules/$(KERNEL_VERSION)/$$mod" >> $(LATE_INIT_FILE);\
		done;\
		cp -rvf $(MODULES_PATH_LIST_SECOND) $(miservice$(RESOUCE))/modules/$(KERNEL_VERSION);\
	fi;

	#### Strip Modules and Libraries
	if [ `find $(OUTPUTDIR)/rootfs/lib/modules/$(KERNEL_VERSION) -name "*.ko" | wc -l` -gt 0 ]; then \
		find $(OUTPUTDIR)/rootfs/lib/modules/$(KERNEL_VERSION) -name "*.ko" | xargs $(STRIP) --strip-unneeded; \
	fi;

	if [ `find $(miservice$(RESOUCE))/modules/$(KERNEL_VERSION) -name "*.ko" | wc -l` -gt 0 ]; then \
		find $(miservice$(RESOUCE))/modules/$(KERNEL_VERSION) -name "*.ko" | xargs $(STRIP) --strip-unneeded; \
	fi;

	if [ `find $(OUTPUTDIR)/rootfs/lib/ -name "*.so" | wc -l` -gt 0 ]; then \
		find $(OUTPUTDIR)/rootfs/lib/ -name "*.so" | xargs $(STRIP)  --strip-unneeded; \
	fi

	if [ `find $(miservice$(RESOUCE))/lib/ -name "*.so" | wc -l` -gt 0 ]; then \
		find $(miservice$(RESOUCE))/lib/ -name "*.so" | xargs $(STRIP)  --strip-unneeded; \
	fi

	#### start demo
	$(call release_demo)

	mkdir -p $(OUTPUTDIR)/vendor
	mkdir -p $(OUTPUTDIR)/customer
	mkdir -p $(OUTPUTDIR)/rootfs/vendor
	mkdir -p $(OUTPUTDIR)/rootfs/customer
