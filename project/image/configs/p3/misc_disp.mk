.PHONY: misc

misc_$(PRODUCT):
misc: misc_$(PRODUCT)

misc_$(PRODUCT):
	@echo [================= $@ ==============]
	if [ "$(DUAL_OS)" = "on" ]; then \
		cp $(PROJ_ROOT)/board/$(CHIP)/rtos/PreloadSetting.txt $(misc$(RESOUCE)) ; \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/iqfile/$(SENSOR_TYPE)/* $(misc$(RESOUCE)) ; \
		cp -f $(misc$(RESOUCE))/$(SENSOR_TYPE)_api.bin $(misc$(RESOUCE))/$(ISP_API_COLORNIGHT) ; \
		cp -f $(misc$(RESOUCE))/$(SENSOR_TYPE)_api.bin $(misc$(RESOUCE))/$(ISP_API_DAY) ; \
		cp -f $(misc$(RESOUCE))/$(SENSOR_TYPE)_api.bin $(misc$(RESOUCE))/$(ISP_API_NIGHT) ; \
		if [ "$(CHIP)" = "i6e" ]; then \
			cp -f $(misc$(RESOUCE))/$(SENSOR_TYPE)_api_night.bin $(misc$(RESOUCE))/$(ISP_API_NIGHT) ; \
			rm -f $(misc$(RESOUCE))/$(SENSOR_TYPE)_api_night.bin ; \
		fi;\
		rm -f $(misc$(RESOUCE))/$(SENSOR_TYPE)_api.bin ; \
		mv $(misc$(RESOUCE))/$(SENSOR_TYPE)_iqfile.bin $(misc$(RESOUCE))/$(ISP_IQ) ; \
		if [ "$(ISP_AWB)" != "" ]; then \
			mv $(misc$(RESOUCE))/$(SENSOR_TYPE)_alsc_cali.data $(misc$(RESOUCE))/$(ISP_AWB) ; \
		fi;\
		if [ "$(ISP_ALSC)" != "" ]; then \
			mv $(misc$(RESOUCE))/$(SENSOR_TYPE)_awb_cali.data $(misc$(RESOUCE))/$(ISP_ALSC) ; \
		fi;\
		if [ "$(DLA_FIRMWARE)" != "" ]; then \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/dla_file/* $(misc$(RESOUCE))/ ; \
		fi;\
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/PreloadSetting.txt $(misc$(RESOUCE))/ ; \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/ascii_8x16 $(misc$(RESOUCE))/ ; \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/hanzi_16x16 $(misc$(RESOUCE))/ ; \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/200X131.argb1555 $(misc$(RESOUCE))/200X131.argb ; \
		cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/200X133.bmp $(misc$(RESOUCE))/ ; \
		if [ "$(CHIP)" = "i6e" ]; then \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/ldc_chn0_view0_table.bin $(misc$(RESOUCE))/ ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/new_AMTK_1R_cm_fhd_rtos_bin.cfg $(misc$(RESOUCE))/ ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/atan2_Q15.bin $(misc$(RESOUCE))/ ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/Cali_LDCpoly.bin $(misc$(RESOUCE))/ ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/sin_Q15.bin $(misc$(RESOUCE))/ ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/trdu0to180.bin $(misc$(RESOUCE))/ ; \
			cp -rf $(PROJ_ROOT)/board/$(CHIP)/rtos/trdu180to220.bin $(misc$(RESOUCE))/ ; \
		fi;\
	fi;
	cp -rf $(PROJ_ROOT)/board/ini/misc/config.ini $(misc$(RESOUCE))/config.ini ;\
	if [ $(IS_DEMO_BOARD) = "1" ]; then \
		sed -i 's/m_pnlList = {SAT070AT50H18BH,}/m_pnlList = {SAT070AT50H18BH_Demo1,}/g' $(misc$(RESOUCE))/config.ini ;\
		sed -i 's/m_sParaTarget = SAT070AT50H18BH/m_sParaTarget = SAT070AT50H18BH_Demo1/g' $(misc$(RESOUCE))/config.ini ;\
	elif [ $(IS_DEMO_BOARD) = "2" ]; then \
		sed -i 's/m_pnlList = {SAT070AT50H18BH,}/m_pnlList = {SAT070AT50H18BH_Demo2,}/g' $(misc$(RESOUCE))/config.ini ;\
		sed -i 's/m_sParaTarget = SAT070AT50H18BH/m_sParaTarget = SAT070AT50H18BH_Demo2/g' $(misc$(RESOUCE))/config.ini ;\
	elif [ $(IS_DEMO_BOARD) = "3" ]; then \
		sed -i 's/m_pnlList = {SAT070AT50H18BH,}/m_pnlList = {SAT070AT50H18BH_Demo3,}/g' $(misc$(RESOUCE))/config.ini ;\
		sed -i 's/m_sParaTarget = SAT070AT50H18BH/m_sParaTarget = SAT070AT50H18BH_Demo3/g' $(misc$(RESOUCE))/config.ini ;\
	else	\
		sed -i 's/m_pnlList = {SAT070AT50H18BH,}/m_pnlList = {SAT070AT50H18BH,}/g' $(misc$(RESOUCE))/config.ini ;\
		sed -i 's/m_sParaTarget = SAT070AT50H18BH/m_sParaTarget = SAT070AT50H18BH/g' $(misc$(RESOUCE))/config.ini ;\
	fi;
	cp -rf $(PROJ_ROOT)/board/ini/misc/sigmastar1024_600.jpg $(misc$(RESOUCE))/
	cp -rf $(PROJ_ROOT)/board/ini/misc/upgrade.jpg $(misc$(RESOUCE))/
