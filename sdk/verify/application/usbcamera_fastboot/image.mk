app:
ifeq ($(IMAGE_INSTALL_DIR),)
	@echo "directory of image is not defined"
	@exit 1
endif

	@rm -rf $(IMAGE_INSTALL_DIR)/rootfs/customer_app/lib
	@mkdir -p $(IMAGE_INSTALL_DIR)/rootfs/customer_app/lib

	@cp -rf $(APPLICATION_PATH)/usbcamera_fastboot/RunFiles/bin/usbCamera $(IMAGE_INSTALL_DIR)/rootfs/customer_app/usbCamera
	@chmod 777 $(IMAGE_INSTALL_DIR)/rootfs/customer_app/usbCamera
	@$(STRIP) --strip-unneeded $(IMAGE_INSTALL_DIR)/rootfs/customer_app/usbCamera
	@cp -rfd $(APPLICATION_PATH)/usbcamera_fastboot/RunFiles/lib $(IMAGE_INSTALL_DIR)/rootfs/customer_app/
	@$(STRIP) --strip-unneeded $(IMAGE_INSTALL_DIR)/rootfs/customer_app/lib/*
	@cp -rf $(APPLICATION_PATH)/usbcamera_fastboot/RunFiles/etc/* $(IMAGE_INSTALL_DIR)/rootfs/etc/
	@cat $(APPLICATION_PATH)/usbcamera_fastboot/RunFiles/demo.sh >> $(IMAGE_INSTALL_DIR)/rootfs/etc/init.sh
	@cp  $(APPLICATION_PATH)/usbcamera_fastboot/RunFiles/run_later.sh $(IMAGE_INSTALL_DIR)/customer/
	@chmod 777 $(IMAGE_INSTALL_DIR)/customer/run_later.sh


