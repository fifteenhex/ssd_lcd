.PHONY: miservice

KBUILD_ROOT:=$(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)
KBUILD_CUST:=$(KBUILD_ROOT)/customize
KERN_MODS_PATH:=$(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)/$(CHIP)/configs/$(PRODUCT)/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/modules
LIB_DIR_PATH:=$(PROJ_ROOT)/release/$(PRODUCT)/$(CHIP)/common/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)

miservice_$(PRODUCT):
miservice: miservice_$(PRODUCT)

miservice_$(PRODUCT):
	@echo [================= $@ ==============]

	mkdir -p $(miservice$(RESOUCE))/lib
	cp $(LIB_DIR_PATH)/mi_libs/dynamic/* $(miservice$(RESOUCE))/lib/
	cp $(LIB_DIR_PATH)/ex_libs/dynamic/* $(miservice$(RESOUCE))/lib/
	cp -rf $(PROJ_ROOT)/board/$(CHIP)/$(BOARD_NAME)/config/* $(miservice$(RESOUCE))
	cp -vf $(PROJ_ROOT)/board/$(CHIP)/mmap/$(MMAP) $(miservice$(RESOUCE))/mmap.ini
	cp -rvf $(LIB_DIR_PATH)/bin/config_tool/* $(miservice$(RESOUCE))
	cd $(miservice$(RESOUCE)); chmod +x config_tool; ln -sf config_tool dump_config; ln -sf config_tool load_config; ln -sf config_tool dump_mmap

	mkdir -p $(miservice$(RESOUCE))/iqfile/

	cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/isp_api.xml $(miservice$(RESOUCE))/iqfile/ -vf
	if [ "$(IQ0)" != "" ]; then \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(IQ0) $(miservice$(RESOUCE))/iqfile/ -vf; \
		cd $(miservice$(RESOUCE))/iqfile; chmod +x $(shell echo $(IQ0) | awk -F'/' '{print $$NF}'); ln -sf $(shell echo $(IQ0) | awk -F'/' '{print $$NF}') iqfile0.bin; cd -; \
	fi;
	if [ "$(IQ1)" != "" ]; then \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(IQ1) $(miservice$(RESOUCE))/iqfile/ -vf; \
		cd $(miservice$(RESOUCE))/iqfile; chmod +x $(shell echo $(IQ1) | awk -F'/' '{print $$NF}'); ln -sf $(shell echo $(IQ1) | awk -F'/' '{print $$NF}') iqfile1.bin; cd -; \
	fi;
	if [ "$(IQ2)" != "" ]; then \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(IQ2) $(miservice$(RESOUCE))/iqfile/ -vf; \
		cd $(miservice$(RESOUCE))/iqfile; chmod +x $(shell echo $(IQ2) | awk -F'/' '{print $$NF}'); ln -sf $(shell echo $(IQ2) | awk -F'/' '{print $$NF}') iqfile2.bin; cd -; \
	fi;
	if [ "$(IQ3)" != "" ]; then \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(IQ3) $(miservice$(RESOUCE))/iqfile/ -vf; \
		cd $(miservice$(RESOUCE))/iqfile; chmod +x $(shell echo $(IQ3) | awk -F'/' '{print $$NF}'); ln -sf $(shell echo $(IQ3) | awk -F'/' '{print $$NF}') iqfile3.bin; cd -; \
	fi;
	if [ -d $(PROJ_ROOT)/board/$(CHIP)/venc_fw ]; then \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/venc_fw $(miservice$(RESOUCE)); \
	fi;
	if [ -d $(PROJ_ROOT)/board/$(CHIP)/dla_file ]; then \
		mkdir $(miservice$(RESOUCE))/dla; \
		cp $(PROJ_ROOT)/board/$(CHIP)/dla_file/ipu_firmware.bin $(miservice$(RESOUCE))/dla; \
	fi;

	mkdir -p $(miservice$(RESOUCE))/modules/$(KERNEL_VERSION)

	if [ -f "$(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/misc_mod_list" ]; then \
		cat $(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/misc_mod_list | sed 's#\(.*\).ko\(.*\)#$(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/\1.ko#' | xargs -i cp -rvf {} $(miservice$(RESOUCE))/modules/$(KERNEL_VERSION); \
	fi;
	if [ -f "$(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/.mods_depend" ]; then \
		cat $(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/.mods_depend | sed 's#\(.*\)#$(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/\1.ko#' | xargs -i cp -rvf {} $(miservice$(RESOUCE))/modules/$(KERNEL_VERSION); \
	fi;
	if [ -f "$(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/misc_mod_list_late" ]; then \
		cat $(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/misc_mod_list_late | sed 's#\(.*\).ko\(.*\)#$(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/\1.ko#' | xargs -i cp -rvf {} $(miservice$(RESOUCE))/modules/$(KERNEL_VERSION); \
	fi;

	if [ -f "$(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)/$(CHIP)/configs/$(PRODUCT)/$(BOARD)/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/$(FLASH_TYPE)/modules/kernel_mod_list_late" ]; then \
		cat $(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)/$(CHIP)/configs/$(PRODUCT)/$(BOARD)/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/$(FLASH_TYPE)/modules/kernel_mod_list_late | sed 's#\(.*\).ko\(.*\)#$(PROJ_ROOT)/kbuild/$(KERNEL_VERSION)/$(CHIP)/configs/$(PRODUCT)/$(BOARD)/$(TOOLCHAIN)/$(TOOLCHAIN_VERSION)/$(FLASH_TYPE)/modules/\1.ko#' | xargs -i cp -rvf {} $(miservice$(RESOUCE))/modules/$(KERNEL_VERSION); \
	fi;

	if [ "$(SENSOR_LIST)" != "" ]; then \
		cp -rvf $(foreach n,$(SENSOR_LIST),$(LIB_DIR_PATH)/modules/$(KERNEL_VERSION)/$(n)) $(miservice$(RESOUCE))/modules/$(KERNEL_VERSION); \
	fi; \

	# creat insmod ko scrpit
	-if [ -f "$(KBUILD_CUST)/kernel_mod_list" ]; then \
		cat $(KBUILD_CUST)/kernel_mod_list | sed 's#\(.*\).ko\(.*\)#$(KERN_MODS_PATH)/\1.ko#' | xargs -i cp -rvf {} $(miservice$(RESOUCE))/modules/$(KERNEL_VERSION); \
	fi;

	find $(miservice$(RESOUCE))/modules/$(KERNEL_VERSION) -name "*.ko" | xargs $(STRIP) --strip-unneeded;
