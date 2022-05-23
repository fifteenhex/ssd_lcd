.PHONY: customer

LIB_DIR_PATH:=$(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/common/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)

KBUILD_ROOT:=$(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)
KBUILD_CUST:=$(KBUILD_ROOT)/customize
KERN_MODS_PATH:=$(KBUILD_ROOT)/modules
LIB_DIR_PATH:=$(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/common/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)

customer_$(PRODUCT):
customer:customer_$(PRODUCT)

customer_$(PRODUCT):
	@echo ====================== $@ ==========================
	@echo
	mkdir -p $(OUTPUTDIR)/customer
	cp -rf $(PROJ_ROOT)/board/ini/* $(OUTPUTDIR)/customer

	if [ $(CONFIG_verify_mixer) = "enable" ]; then \
		if [ $(FLASH_TYPE) = "nor" ]; then \
			if [ "$(FLASH_SIZE)" != "8M" ]; then \
				cp -rf $(LIB_DIR_PATH)/bin/mixer/* $(OUTPUTDIR)/customer/ ; \
				ln -sf ./mixer/font $(OUTPUTDIR)/customer/font ; \
				if [ -f "$(LIB_DIR_PATH)/bin/mi_demo/prog_rtsp" ]; then \
					cp -rf $(LIB_DIR_PATH)/bin/mi_demo/prog_rtsp $(OUTPUTDIR)/customer/; \
				fi;	\
			fi; \
		else \
			if [ -f "$(LIB_DIR_PATH)/bin/mixer" ]; then \
				cp -rf $(LIB_DIR_PATH)/bin/mixer/* $(OUTPUTDIR)/customer/ ; \
				ln -sf ./mixer/font $(OUTPUTDIR)/customer/font ; \
			fi;	\
			if [ -f "$(LIB_DIR_PATH)/bin/mi_demo" ]; then \
				cp -rf $(LIB_DIR_PATH)/bin/mi_demo/ $(OUTPUTDIR)/customer/ ; \
			fi;	\
		fi;    \
	fi;

	-if [ "$(FLASH_TYPE)"x != "nor"x ]; then \
		chmod 755 $(LIB_DIR_PATH)/bin/debug/*	;\
		cp -rf $(LIB_DIR_PATH)/bin/debug/* $(OUTPUTDIR)/customer/	;\
	else	\
		if [ -f "$(LIB_DIR_PATH)/bin/debug/i2c_read_write" ]; then \
			chmod 755 $(LIB_DIR_PATH)/bin/debug/i2c_read_write  ;\
			cp -rf $(LIB_DIR_PATH)/bin/debug/i2c_read_write $(OUTPUTDIR)/customer/   ;\
		fi;	\
		if [ -f "$(LIB_DIR_PATH)/bin/debug/riu_r" ]; then \
			chmod 755 $(LIB_DIR_PATH)/bin/debug/riu_r  ;\
			cp -rf $(LIB_DIR_PATH)/bin/debug/riu_r $(OUTPUTDIR)/customer/   ;\
		fi;	\
		if [ -f "$(LIB_DIR_PATH)/bin/debug/riu_w" ]; then \
			chmod 755 $(LIB_DIR_PATH)/bin/debug/riu_w  ;\
			cp -rf $(LIB_DIR_PATH)/bin/debug/riu_w $(OUTPUTDIR)/customer/   ;\
		fi;	\
	fi;

	#add:  remove sshd in nor flash default
	if [[ "$(FLASH_TYPE)"x = "nor"x ]] && [[ -d "$(OUTPUTDIR)/customer/ssh" ]]; then \
		rm -rf $(OUTPUTDIR)/customer/ssh; \
	fi;
	#end add

	if [ -f "$(OUTPUTDIR)/customer/demo.sh" ]; then \
		rm  $(OUTPUTDIR)/customer/demo.sh; \
	fi;
	touch $(OUTPUTDIR)/customer/demo.sh
	chmod 755 $(OUTPUTDIR)/customer/demo.sh

	# change linux printk level to 4 
	echo 'echo 4 > /proc/sys/kernel/printk' >> $(OUTPUTDIR)/customer/demo.sh
	echo >> $(OUTPUTDIR)/customer/demo.sh

	# creat insmod ko scrpit
	if [ -f "$(KBUILD_CUST)/kernel_mod_list" ]; then \
		cat $(KBUILD_CUST)/kernel_mod_list | sed 's#\(.*\).ko#insmod /config/modules/$(KERNEL_VERSION)/\1.ko#' >> $(OUTPUTDIR)/customer/demo.sh; \
		echo "#kernel_mod_list" >> $(OUTPUTDIR)/customer/demo.sh; \
	fi;

	if [ -f "$(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/misc_mod_list" ]; then \
		cat $(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/misc_mod_list | sed 's#\(.*\).ko#insmod /config/modules/$(KERNEL_VERSION)/\1.ko#' >> $(OUTPUTDIR)/customer/demo.sh; \
		echo "#misc_mod_list" >> $(OUTPUTDIR)/customer/demo.sh; \
	fi;
	if [ -f "$(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/.mods_depend" ]; then \
		cat $(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/.mods_depend | sed '2,20s#\(.*\)#insmod /config/modules/$(KERNEL_VERSION)/\1.ko#' >> $(OUTPUTDIR)/customer/demo.sh; \
		echo "#mi module" >> $(OUTPUTDIR)/customer/demo.sh; \
	fi;
	if [ -f "$(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/misc_mod_list_late" ]; then \
		cat $(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/misc_mod_list_late | sed 's#\(.*\).ko#insmod /config/modules/$(KERNEL_VERSION)/\1.ko#' >> $(OUTPUTDIR)/customer/demo.sh; \
		echo "#misc_mod_list_late" >> $(OUTPUTDIR)/customer/demo.sh; \
	fi;

	if [ -f "$(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)/$(CHIP)/configs/$(PRODUCT)/$(BOARD)/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/$(FLASH_TYPE)/modules/kernel_mod_list_late" ]; then \
		cat $(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)/$(CHIP)/configs/$(PRODUCT)/$(BOARD)/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/$(FLASH_TYPE)/modules/kernel_mod_list_late | sed 's#\(.*\).ko#insmod /config/modules/$(KERNEL_VERSION)/\1.ko#' >> $(OUTPUTDIR)/customer/demo.sh; \
		echo "#kernel_mod_list_late" >> $(OUTPUTDIR)/customer/demo.sh; \
	fi;

	if [ "$(DUAL_OS)" != "on" ]; then \
		if [ "$(SENSOR0)" != "" ]; then \
			echo insmod /config/modules/$(KERNEL_VERSION)/$(SENSOR0) $(SENSOR0_OPT) >> $(OUTPUTDIR)/customer/demo.sh; \
		fi; \
		if [ "$(SENSOR1)" != "" ]; then \
			echo insmod /config/modules/$(KERNEL_VERSION)/$(SENSOR1) $(SENSOR1_OPT) >> $(OUTPUTDIR)/customer/demo.sh; \
		fi; \
		if [ "$(SENSOR2)" != "" ]; then \
			echo insmod /config/modules/$(KERNEL_VERSION)/$(SENSOR2) $(SENSOR2_OPT) >> $(OUTPUTDIR)/customer/demo.sh; \
		fi;	\
		sed -i 's/mi_sys.ko/mi_sys.ko cmdQBufSize=768 logBufSize=256 config_ini_path=\"\/customer\/config.ini\"/g' $(OUTPUTDIR)/customer/demo.sh; \
		sed -i '/mi_iqserver.ko/,+4d' $(OUTPUTDIR)/customer/demo.sh;\
		sed -i '/mi_isp.ko/,+4d' $(OUTPUTDIR)/customer/demo.sh;\
		sed -i 's/mi_common/insmod \/config\/modules\/$(KERNEL_VERSION)\/mi_common.ko/g' $(OUTPUTDIR)/customer/demo.sh; \
		sed -i '/#mi module/a	major=`cat /proc/devices | busybox awk "\\\\$$2==\\""mi_poll"\\" {print \\\\$$1}"`\nbusybox mknod \/dev\/mi_poll c $$major 0' $(OUTPUTDIR)/customer/demo.sh; \
	fi;

	if [ $(IS_DEMO_BOARD) = "0" ]; then \
		echo "Not need pannel config!" ;\
	elif [ $(IS_DEMO_BOARD) = "1" ]; then \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/$(BOARD_NAME)/config/config_demo.ini $(OUTPUTDIR)/customer/config.ini ;\
	elif [ $(IS_DEMO_BOARD) = "2" ]; then \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/$(BOARD_NAME)/config/config_demo_camera.ini $(OUTPUTDIR)/customer/config.ini ;\
	else	\
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/$(BOARD_NAME)/config/config.ini $(OUTPUTDIR)/customer/config.ini ;\
	fi;
	echo mdev -s >> $(OUTPUTDIR)/customer/demo.sh
