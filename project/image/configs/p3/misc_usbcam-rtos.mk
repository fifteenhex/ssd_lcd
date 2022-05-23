.PHONY: misc

misc_$(PRODUCT):
misc: misc_$(PRODUCT)

misc_$(PRODUCT):
	@echo [================= $@ ==============]
	if [ "$(DUAL_OS)" = "on" ]; then \
		if [ "$(CHIP)" = "i6e" ] && [ "$(PRODUCT)" = "usbcam-rtos" ]; then \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/PreloadSetting_uvc_bus_pwr_en_hiber.txt $(misc$(RESOUCE))/PreloadSetting.txt ; \
		else \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/PreloadSetting.txt $(misc$(RESOUCE)) ; \
		fi;\
		if [ "$(SENSOR_TYPE0)" != "" ]; then \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(SENSOR_TYPE0)/$(SENSOR_TYPE0)_api.bin $(misc$(RESOUCE)) ; \
			cp -f $(misc$(RESOUCE))/$(SENSOR_TYPE0)_api.bin $(misc$(RESOUCE))/$(ISP_API_COLORNIGHT0) ; \
			cp -f $(misc$(RESOUCE))/$(SENSOR_TYPE0)_api.bin $(misc$(RESOUCE))/$(ISP_API_DAY0) ; \
			cp -f $(misc$(RESOUCE))/$(SENSOR_TYPE0)_api.bin $(misc$(RESOUCE))/$(ISP_API_NIGHT0) ; \
			rm -f $(misc$(RESOUCE))/$(SENSOR_TYPE0)_api.bin ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(SENSOR_TYPE0)/$(SENSOR_TYPE0)_iqfile.bin $(misc$(RESOUCE)) ; \
			mv $(misc$(RESOUCE))/$(SENSOR_TYPE0)_iqfile.bin $(misc$(RESOUCE))/$(ISP_IQ0) ; \
			if [ "$(CHIP)" = "i6e" ]; then \
				cp -f $(misc$(RESOUCE))/$(SENSOR_TYPE0)_api_night.bin $(misc$(RESOUCE))/$(ISP_API_NIGHT0) ; \
				rm -f $(misc$(RESOUCE))/$(SENSOR_TYPE0)_api_night.bin ; \
			fi;\
			if [ "$(ISP_AWB0)" != "" ]; then \
			       cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(SENSOR_TYPE0)/$(SENSOR_TYPE0)_awb_cali.data $(misc$(RESOUCE)) ; \
				mv $(misc$(RESOUCE))/$(SENSOR_TYPE0)_awb_cali.data $(misc$(RESOUCE))/$(ISP_AWB0) ; \
			fi;\
			if [ "$(ISP_ALSC0)" != "" ]; then \
			       cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(SENSOR_TYPE0)/$(SENSOR_TYPE0)_alsc_cali.data $(misc$(RESOUCE)) ; \
				mv $(misc$(RESOUCE))/$(SENSOR_TYPE0)_alsc_cali.data $(misc$(RESOUCE))/$(ISP_ALSC0) ; \
			fi;\
		fi;\
		if [ "$(SENSOR_TYPE1)" != "" ]; then \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(SENSOR_TYPE1)/$(SENSOR_TYPE1)_api.bin $(misc$(RESOUCE)) ; \
			cp -f $(misc$(RESOUCE))/$(SENSOR_TYPE1)_api.bin $(misc$(RESOUCE))/$(ISP_API_COLORNIGHT1) ; \
			cp -f $(misc$(RESOUCE))/$(SENSOR_TYPE1)_api.bin $(misc$(RESOUCE))/$(ISP_API_DAY1) ; \
			cp -f $(misc$(RESOUCE))/$(SENSOR_TYPE1)_api.bin $(misc$(RESOUCE))/$(ISP_API_NIGHT1) ; \
			rm -f $(misc$(RESOUCE))/$(SENSOR_TYPE1)_api.bin ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(SENSOR_TYPE1)/$(SENSOR_TYPE1)_iqfile.bin $(misc$(RESOUCE)) ; \
			mv $(misc$(RESOUCE))/$(SENSOR_TYPE1)_iqfile.bin $(misc$(RESOUCE))/$(ISP_IQ1) ; \
			if [ "$(CHIP)" = "i6e" ]; then \
				cp -f $(misc$(RESOUCE))/$(SENSOR_TYPE1)_api_night.bin $(misc$(RESOUCE))/$(ISP_API_NIGHT1) ; \
				rm -f $(misc$(RESOUCE))/$(SENSOR_TYPE1)_api_night.bin ; \
			fi;\
			if [ "$(ISP_AWB1)" != "" ]; then \
			       cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(SENSOR_TYPE1)/$(SENSOR_TYPE1)_awb_cali.data $(misc$(RESOUCE)) ; \
				mv $(misc$(RESOUCE))/$(SENSOR_TYPE1)_awb_cali.data $(misc$(RESOUCE))/$(ISP_AWB1) ; \
			fi;\
			if [ "$(ISP_ALSC1)" != "" ]; then \
			       cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(SENSOR_TYPE1)/$(SENSOR_TYPE1)_alsc_cali.data $(misc$(RESOUCE)) ; \
				mv $(misc$(RESOUCE))/$(SENSOR_TYPE1)_alsc_cali.data $(misc$(RESOUCE))/$(ISP_ALSC1) ; \
			fi;\
		fi;\
		if [ "$(DLA_FIRMWARE)" != "" ]; then \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/dla_file/* $(misc$(RESOUCE))/ ; \
		fi;\
		if [ "$(PRODUCT)" != "usbcam-rtos" ]; then \
		    cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/ascii_8x16 $(misc$(RESOUCE))/ ; \
		    cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/hanzi_16x16 $(misc$(RESOUCE))/ ; \
		    cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/200X131.argb1555 $(misc$(RESOUCE))/200X131.argb ; \
		    cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/200X133.bmp $(misc$(RESOUCE))/ ; \
		fi;\
		if [ "$(CHIP)" = "i6e" ] && [ "$(PRODUCT)" != "usbcam-rtos" ]; then \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/ldc_chn0_view0_table.bin $(misc$(RESOUCE))/ ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/new_AMTK_1R_cm_fhd_rtos_bin.cfg $(misc$(RESOUCE))/ ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/atan2_Q15.bin $(misc$(RESOUCE))/ ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/Cali_LDCpoly.bin $(misc$(RESOUCE))/ ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/sin_Q15.bin $(misc$(RESOUCE))/ ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/trdu0to180.bin $(misc$(RESOUCE))/ ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/trdu180to220.bin $(misc$(RESOUCE))/ ; \
		fi;\
	fi;\
	if [ "$(PRODUCT)" != "usbcam-rtos" ]; then \
	    cp -rf $(PROJ_ROOT)/board/ini/misc/sigmastar1024_600.jpg $(misc$(RESOUCE))/ ; \
	    cp -rf $(PROJ_ROOT)/board/ini/misc/upgrade.jpg $(misc$(RESOUCE))/ ; \
	fi;
	cp -rf $(PROJ_ROOT)/board/ini/misc/config.ini $(misc$(RESOUCE))/ ; \
	cp -rf $(PROJ_ROOT)/board/$(CHIP)/$(BOARD_NAME)/config/fbdev.ini $(misc$(RESOUCE));
