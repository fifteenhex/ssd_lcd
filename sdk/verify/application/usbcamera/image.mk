app:
ifeq ($(IMAGE_INSTALL_DIR),)
	@echo "directory of image is not defined"
	@exit 1
endif


	@cp -rfd $(APPLICATION_PATH)/usbcamera/lib $(IMAGE_INSTALL_DIR)/customer
	@$(STRIP) --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/lib/*
	@cp -rf $(APPLICATION_PATH)/usbcamera/etc/* $(IMAGE_INSTALL_DIR)/rootfs/etc/
	@cat $(APPLICATION_PATH)/usbcamera/demo.sh >> $(IMAGE_INSTALL_DIR)/customer/demo.sh
	@cp -rf $(APPLICATION_PATH)/usbcamera/bin/usbCamera $(IMAGE_INSTALL_DIR)/customer/usbCamera
	@$(STRIP) --strip-unneeded $(IMAGE_INSTALL_DIR)/customer/usbCamera

