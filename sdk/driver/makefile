include $(CURDIR)/../../project/configs/current.configs

DRIVER_EXCLUE_FILE := Makefile makefile
DRIVER_MODULES:= $(filter-out $(DRIVER_EXCLUE_FILE),$(shell ls))

DRIVER_ENABLED:=$(patsubst %_enable_,%,$(patsubst %__,%,$(filter-out %_disable_, $(foreach n,$(DRIVER_MODULES),$(n)_$(driver_$(n))_))))
DRIVER_DISABLED:=$(filter-out $(INTERFACE_ENABLED),$(INTERFACE_MODULES))
DRIVER_MODULES_CLEAN := $(foreach n,$(DRIVER_ENABLED),$(n)_clean)
DRIVER_MODULES_INSTALL := $(foreach n,$(DRIVER_ENABLED),$(n)_install)

.PHONY: all clean install $(DRIVER_MODULES_CLEAN) $(DRIVER_MODULES_INSTALL) $(DRIVER_ENABLED)

all: $(DRIVER_ENABLED)
clean: $(DRIVER_MODULES_CLEAN)
install: $(DRIVER_MODULES_INSTALL)

$(DRIVER_ENABLED):
	$(MAKE) -C $@ all

$(DRIVER_MODULES_CLEAN):
	$(MAKE) -C $(patsubst %_clean,%,$@) clean

$(DRIVER_MODULES_INSTALL):
	$(MAKE) -C $(patsubst %_install,%,$@) install
