.PHONY: symbol_link symbol_link_clean linux-kernel linux-kernel_clean
-include configs/current.configs
ifneq ($(IMAGE_CONFIG),)
include image/configs/$(CHIP)/$(IMAGE_CONFIG)
endif

KERNEL_ROOT= $(PROJ_ROOT)/../kernel
KERNEL_DIR= $(KERNEL_ROOT)/arch/arm/configs
KERNEL_MK= $(KERNEL_ROOT)/Makefile

VERSION = $(shell cat $(KERNEL_MK) | awk 'NR==1' | tr -cd "[0-9]")
PATCHLEVEL = $(shell cat $(KERNEL_MK) | awk 'NR==2' | tr -cd "[0-9]")
SUBLEVEL = $(shell cat $(KERNEL_MK) | awk 'NR==3' | tr -cd "[0-9]")
PARSE_KERNEL_VERSION = $(VERSION).$(PATCHLEVEL).$(SUBLEVEL)

KBUILD_ROOT = $(PROJ_ROOT)/kbuild/$(PARSE_KERNEL_VERSION)

linux-kernel:
	@echo "build kernel: MAKEFLAGS <$(MAKEFLAGS)>"
	@if [ -f $(KERNEL_DIR)/$(KERNEL_CONFIG) ]; then \
		echo "linux-config: \"$(KERNEL_CONFIG)\"";\
		$(MAKE) -C $(KERNEL_ROOT) $(KERNEL_CONFIG);\
		$(MAKE) -C $(KERNEL_ROOT);\
	else \
		echo "ignore linux-config: \"$(KERNEL_CONFIG)\" is unknown"; \
	fi

linux-kernel_clean:
	@if [ -f $(KERNEL_DIR)/$(KERNEL_CONFIG) ]; then \
		$(MAKE) -C $(KERNEL_ROOT) clean; \
	else \
		echo "ignore linux-config: \"$(KERNEL_CONFIG)\" is unknown"; \
	fi

$(KBUILD_ROOT):
	@if [ "$(KERNEL_VERSION)" == "" ]; then \
		exit 0 ; \
	elif [ "$(KERNEL_VERSION)" == "$(PARSE_KERNEL_VERSION)" ]; then \
		mkdir -p $(KBUILD_ROOT); \
	else \
		echo "ERROR: kernel config version:$(KERNEL_VERSIN) , current version: $(PARSE_KERNEL_VERSION)"; \
		exit 1; \
	fi

symbol_link: $(KBUILD_ROOT)
	ln -sf $(KERNEL_ROOT)/modules $(KBUILD_ROOT)/
	ln -sf $(KERNEL_ROOT)/arch $(KBUILD_ROOT)/
	ln -sf $(KERNEL_ROOT)/drivers $(KBUILD_ROOT)/
	ln -sf $(KERNEL_ROOT)/include $(KBUILD_ROOT)/
	ln -sf $(KERNEL_ROOT)/scripts $(KBUILD_ROOT)/
	ln -sf $(KERNEL_ROOT)/Makefile $(KBUILD_ROOT)/
	ln -sf $(KERNEL_ROOT)/Module.symvers $(KBUILD_ROOT)/
	ln -sf $(KERNEL_ROOT)/.sstar_chip.txt $(KBUILD_ROOT)/
	ln -sf $(KERNEL_ROOT)/.config $(KBUILD_ROOT)/
	ln -sf $(PROJ_ROOT)/kbuild/tools/usr $(KBUILD_ROOT)/
	ln -sf $(PROJ_ROOT)/kbuild/customize/$(KERNEL_VERSION)/$(CHIP)/$(PRODUCT) $(KBUILD_ROOT)/customize

symbol_link_clean:
	rm $(KBUILD_ROOT) -rf
