app:
ifeq ($(IMAGE_INSTALL_DIR),)
	@echo "directory of image is not defined"
	@exit 1
endif

	@rm -rf $(IMAGE_INSTALL_DIR)/rootfs/customer_app
	@mkdir -p $(IMAGE_INSTALL_DIR)/rootfs/customer_app

	@cp -rf $(APPLICATION_PATH)/disp_pic_fastboot/RunFiles/bin/* $(IMAGE_INSTALL_DIR)/rootfs/customer_app/
	@chmod 777 $(IMAGE_INSTALL_DIR)/rootfs/customer_app/YuvToPanel
	@$(STRIP) --strip-unneeded $(IMAGE_INSTALL_DIR)/rootfs/customer_app/YuvToPanel
	@cp -rf $(APPLICATION_PATH)/disp_pic_fastboot/RunFiles/etc/* $(IMAGE_INSTALL_DIR)/rootfs/etc/
	@cat $(APPLICATION_PATH)/disp_pic_fastboot/RunFiles/demo.sh >> $(IMAGE_INSTALL_DIR)/rootfs/etc/init.sh
	@cp  $(APPLICATION_PATH)/disp_pic_fastboot/RunFiles/run_later.sh $(IMAGE_INSTALL_DIR)/customer/
	@chmod 777 $(IMAGE_INSTALL_DIR)/customer/run_later.sh


