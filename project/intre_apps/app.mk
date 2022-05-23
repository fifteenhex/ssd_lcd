include $(PROJ_ROOT)/configs/current.configs

intre_apps:
	$(MAKE) -C $(PROJ_ROOT)/intre_apps/elsa_lcd
	@cp -rf $(PROJ_ROOT)/intre_apps/elsa_lcd/elsa_main $(IMAGE_INSTALL_DIR)/customer
	@cp -rf $(PROJ_ROOT)/intre_apps/elsa_lcd/*.gif $(IMAGE_INSTALL_DIR)/customer
intre_apps_clean:
	$(MAKE) -C $(PROJ_ROOT)/intre_apps/elsa_lcd clean
