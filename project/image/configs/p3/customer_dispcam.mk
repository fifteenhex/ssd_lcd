-include $(PROJ_ROOT)/../sdk/verify/application/app.mk

ifeq ($(BENCH),yes)
	include configs/$(CHIP)/bench.mk
endif

.PHONY: customer

ifneq (VERIFY_UI_DEMO, y)
app:
endif

customer_$(PRODUCT):
customer:customer_$(PRODUCT)

customer_$(PRODUCT): app
	@echo =============================pkg customer app ==========================
	@echo =============================XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX ==========================
	@echo
	if [ $(BENCH) = "yes" ]; then \
		$(MAKE) bench; \
	fi;
