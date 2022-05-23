APPLICATION_PATH=$(PROJ_ROOT)/../sdk/verify/application
include $(PROJ_ROOT)/configs/current.configs
ifeq ($(verify_smarttalk),enable)
	include $(APPLICATION_PATH)/smarttalk/image.mk
endif
ifeq ($(verify_ssplayer),enable)
	include $(APPLICATION_PATH)/ssplayer/image.mk
endif
ifeq ($(verify_smarthome),enable)
	include $(APPLICATION_PATH)/smarthome/image.mk
endif
ifeq ($(verify_zk_bootup),enable)
	include $(APPLICATION_PATH)/zk_bootup/image.mk
endif
ifeq ($(verify_zk_mini),enable)
	include $(APPLICATION_PATH)/zk_mini/image.mk
endif
ifeq ($(verify_zk_full),enable)
	include $(APPLICATION_PATH)/zk_full/image.mk
endif
ifeq ($(verify_zk_mini_nosensor),enable)
	include $(APPLICATION_PATH)/zk_mini_nosensor/image.mk
endif
ifeq ($(verify_zk_mini_fastboot),enable)
	include $(APPLICATION_PATH)/zk_mini_fastboot/image.mk
endif
ifeq ($(verify_zk_mini_nosensor_fastboot),enable)
	include $(APPLICATION_PATH)/zk_mini_nosensor_fastboot/image.mk
endif
ifeq ($(verify_zk_full_fastboot),enable)
	include $(APPLICATION_PATH)/zk_full_fastboot/image.mk
endif
ifeq ($(verify_jpeg2disp),enable)
	include $(APPLICATION_PATH)/jpeg2disp/image.mk
endif
ifeq ($(verify_qfn68_sensor_panel),enable)
	include $(APPLICATION_PATH)/qfn68_sensor_panel/image.mk
endif
ifeq ($(verify_barcode),enable)
	include $(APPLICATION_PATH)/barcode/image.mk
endif
ifeq ($(verify_disp_pic_fastboot),enable)
	include $(APPLICATION_PATH)/disp_pic_fastboot/image.mk
endif
ifeq ($(verify_usbcamera_fastboot),enable)
	include $(APPLICATION_PATH)/usbcamera_fastboot/image.mk
endif
ifeq ($(verify_usbcamera),enable)
	include $(APPLICATION_PATH)/usbcamera/image.mk
endif
ifeq ($(verify_barcodeYuyan),enable)
	include $(APPLICATION_PATH)/barcodeYuyan/image.mk
endif

