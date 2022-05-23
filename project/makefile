.PHONY: release image
-include configs/current.configs
-include $(KBUILD_MK)
-include $(SOURCE_MK)

image: symbol_link
	$(MAKE) headfile_link
	$(MAKE) linux-kernel
	$(MAKE) release
	$(MAKE) error_message
	$(MAKE) image_install

image-nocheck: symbol_link
	$(MAKE) headfile_link
	$(MAKE) linux-kernel
	$(MAKE) release
	$(MAKE) image_install
	$(MAKE) warning_message

image-release:
	$(MAKE) headfile_link
	$(MAKE) release
	$(MAKE) error_message
	$(MAKE) image_install

image-release-nocheck:
	$(MAKE) headfile_link
	$(MAKE) release
	$(MAKE) image_install
	$(MAKE) warning_message

image-fast:
	$(MAKE) headfile_link
	$(MAKE) error_message
	$(MAKE) image_install

image-fast-nocheck:
	$(MAKE) headfile_link
	$(MAKE) image_install
	$(MAKE) warning_message

image-ota:
	$(MAKE) -C image ota_images

faker:
SOURCE_CLEAN?=faker

clean:
	-$(MAKE) $(SOURCE_CLEAN)
	-$(MAKE) linux-kernel_clean
	$(MAKE) image_clean
	-$(MAKE) symbol_link_clean
	$(MAKE) headfile_link_clean

image_install: image_clean
	$(MAKE) -C image all
	./split_partion.sh
	./make_customer_partition_update.sh

image-release_clean:
	$(MAKE) $(SOURCE_CLEAN)
	$(MAKE) image_clean
	$(MAKE) headfile_link_clean

image_clean:
	$(MAKE) -C image clean

release:
ifneq ($(SOURCE_RELEASE), )
	$(MAKE) sdk
endif

build_verify: $(SOURCE_RELEASE_DEPEND) clean image

headfile_link:
	@header_dirs=`find $(PROJ_ROOT)/release/include/*/$(CHIP_ALIAS) -type d |sed 's/$(CHIP_ALIAS)//'|xargs`; \
		ln -vsf `find -P $${header_dirs} $(PROJ_ROOT)/release/include/*/$(CHIP_ALIAS) -maxdepth 1 -name "*.h"` $(PROJ_ROOT)/release/include/;

headfile_link_clean:
	@find -P $(PROJ_ROOT)/release/include/ -maxdepth 1 -type l -name "*" | xargs rm -f

warning_message:
	@if [ -d ".git" ]; then \
		cd  release/include && \
		git status -s . | \
		awk -F' ' '{a = a "\033[7;34;42m""release/include/"$$2"\033[0m\n"} END {if(a){ \
				print "\033[5;41;33mWARNING!!!! You CAN NOT MODIFY HEAD FILES ,NEED MI OWNER TO REVIEW THE FOLLOWING INCONSISTENT HEAD FILES!!!\033[0m"; \
				print a;}}'; \
	fi;

error_message:warning_message
	@if [ -d ".git" ]; then \
		cd  release/include && \
		git status -s . | \
		awk -F' ' '{a = a "\033[7;34;42m""release/include/"$$2"\033[0m\n"} END {if(a){ \
				print "\033[5;41;33mIF YOU WANT DEBUG ONLY, pelase using command :[[[ make image-nocheck ]]] INSTEADED!!!\033[0m"; \
				exit 127;}}'; \
	fi;


# kbuild system config
srctree = $(PWD)
export srctree

HOSTCC = gcc
HOSTCFLAGS = -Wall -Wmissing-prototypes -Wstrict-prototypes -O2 -fomit-frame-pointer -std=gnu89
export HOSTCC HOSTCFLAGS

KCONFIG_CONFIG ?= .config
MENUCONFIG_CONFIG ?= configs/current.configs

# We need some generic definitions(do not try to remake the file).
scripts/Kbuild.include: ;
-include scripts/Kbuild.include

PHONY += scripts_basic
scripts_basic:
	$(MAKE) $(build)=scripts/basic

scripts/basic/%: scripts_basic ;

silentoldconfig:
	$(MAKE) $(build)=scripts/kconfig $@

config: scripts_basic FORCE
	$(MAKE) $(build)=scripts/kconfig $@
	$(MAKE) silentoldconfig
	$(call post_process_menuconfig)

defconfigs_target=$(foreach n,$(filter %_defconfig,$(wildcard $(PWD)/configs/defconfigs/*)),$(notdir $(n)))
PHONY += $(defconfigs_target)
$(defconfigs_target):scripts_basic FORCE
	$(MAKE) $(build)=scripts/kconfig $@
	$(MAKE) silentoldconfig
	$(call post_process_menuconfig)

menuconfig: scripts_basic FORCE
	$(MAKE) $(build)=scripts/kconfig $@
	$(MAKE) silentoldconfig
	$(call post_process_menuconfig)

# To avoid any implicit rule to kick in, define an empty command
$(KCONFIG_CONFIG) include/config/auto.conf.cmd: ;

# If .config is newer than include/config/auto.conf, someone tinkered
# with it and forgot to run make oldconfig.
# if auto.conf.cmd is missing then we are probably in a cleaned tree so
# we execute the config step to be sure to catch updated Kconfig files
include/config/%.conf: $(KCONFIG_CONFIG) include/config/auto.conf.cmd
	$(MAKE) silentoldconfig

# configs/current.configs:
# remove blank line and comment line
# a=1 ==> a = 1
# remove CONFIG_
# remove "
# remove trailling space
# generate configs/current.configs
# include/generated/autoconf.h:
# remove CONFIG_
# remove KERNEL_VERSION
define post_process_menuconfig
	@sed '/^$$/d; /[[:blank:]]*#/d' $(KCONFIG_CONFIG) > $(MENUCONFIG_CONFIG).in
	@sed -i 's/\([^[:blank:]]\)=\([^[:blank:]]\)/\1 = \2/' $(MENUCONFIG_CONFIG).in
	@sed -i 's/^CONFIG_//g' $(MENUCONFIG_CONFIG).in
	@sed -i 's/"//g' $(MENUCONFIG_CONFIG).in
	@sed -i 's/[[:blank:]]*$$//' $(MENUCONFIG_CONFIG).in
	@$(PWD)/setup_config.sh $(MENUCONFIG_CONFIG).in
	@rm -f $(MENUCONFIG_CONFIG).in
	@sed -i 's/CONFIG_//' $(PWD)/include/generated/autoconf.h
	@sed -i '/ KERNEL_VERSION /d' $(PWD)/include/generated/autoconf.h
endef

PHONY += FORCE
FORCE:

.PHONY: $(PHONY)
