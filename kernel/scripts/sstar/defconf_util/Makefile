_WD := scripts/sstar/defconf_util
_ROOT_DIR := $(subst $(_WD),,$(shell pwd))
KCONFIG_DIR := $(_ROOT_DIR)scripts/kconfig
MCONF_BIN := $(KCONFIG_DIR)/mconf
ARCH_DIR := $(_ROOT_DIR)/arch/arm/mach-sstar/
KCONFIG := $(_ROOT_DIR)$(_WD)/Kconfig
DEFAULT_CONFIG := $(_ROOT_DIR)$(_WD)/.config
RUN_SCRIPT := $(_ROOT_DIR)$(_WD)/run.sh
CONFIG_DIR := $(_ROOT_DIR)arch/arm/configs/
# enumerate chip names from $(ARCH_DIR)
CHIP_LIST := $(sort $(shell find $(ARCH_DIR) -mindepth 1 -maxdepth 1 -type d -exec basename {} \; | xargs))
DEFCONFIG_LIST := $(sort $(foreach chip,$(CHIP_LIST),$(shell find $(CONFIG_DIR) -iname '$(chip)*_defconfig' | xargs )))

# Kconfig select item template
CHIP_NAME_TEMP := _CHIP_TEMPLATE_
ITEM_TEMPLATE := menuconfig $(CHIP_NAME_TEMP)\n\tbool \"SStar SoC platform $(CHIP_NAME_TEMP)\"\n\tdefault n\n

CONFIG_TEMP := _CONFIG_TEMPLATE_
CONFIG_TEMPLATE := config $(CONFIG_TEMP)\n\tbool \"select to remove $(CONFIG_TEMP)\"\n\tdepends on SSTAR_OPTIONS\n\tdefault n\n\n

# replace template string with chip names
SELECT_ITEMS := $(foreach chip,$(CHIP_LIST),$(subst $(CHIP_NAME_TEMP),CHIP_$(chip),$(ITEM_TEMPLATE)))

OPTION_ITEMS := $(shell cat $(DEFCONFIG_LIST) | sed -s 's/.*CONFIG_\([_0-9a-zA-Z]*\)[ =].*/\1/' | grep -v '\#' | grep ' *' | sort -u | xargs)
CONFIG_ITEMS := $(foreach config,$(OPTION_ITEMS),$(subst $(CONFIG_TEMP),CONFIG_$(config),$(CONFIG_TEMPLATE)))

.PHONY: all debug autogen menuconfig execute 

#all: autogen menuconfig release
all: autogen menuconfig execute
	@echo "------------------------------"
	@echo " Cutomer release done!"
	@echo "------------------------------"

debug:
	@echo "------------------------------"
	@echo " DEBUG"
	@echo "------------------------------"
	@echo $(_WD)
	@echo $(_ROOT_DIR)
	@echo $(MCONF_BIN)
	@echo $(CHIP_LIST)
	@printf "$(SELECT_ITEMS)"
	@printf "$(OPTION_ITEMS)"
	@printf "$(OPTIONS_ITEMS)"
	@printf "$(CONFIG_ITEMS)"

autogen:
	@echo "------------------------------"
	@echo " Auto generate Kconfig..."
	@echo "------------------------------"
	@rm -f $(KCONFIG)
	@rm -f $(DEFAULT_CONFIG)
	@echo "mainmenu \"Customer Release Chips Configuration\"" > $(KCONFIG)
	@printf "$(SELECT_ITEMS)" >> $(KCONFIG)
	@printf "\nconfig SSTAR_OPTIONS\n\tbool \"select config to remove\"\n\tdefault y\n" >> $(KCONFIG)
	@printf "$(CONFIG_ITEMS)" >> $(KCONFIG)
 
	@cat $(KCONFIG)

menuconfig:
	@$(MCONF_BIN) $(KCONFIG)

execute:
	@echo "------------------------------"
	@echo " Execute customer release..."
	@echo "------------------------------"
	$(RUN_SCRIPT) $(shell sed -n 's/CONFIG_CHIP_\([0-9a-zA-Z]*\)=y/\1/p' .config) 

