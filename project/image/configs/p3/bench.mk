include $(PROJ_ROOT)/configs/current.configs

.PHONY: bench

bench:
	if [ $(BENCH) = "yes" ]; then \
		git clone http://hcgit04:9080/a/mstar/bench ; \
		cp -rf bench $(miservice$(RESOUCE)) ; \
		echo "cd bench" >> $(OUTPUTDIR)/customer/demo.sh ; \
		echo ./RunUnittest.sh >> $(OUTPUTDIR)/customer/demo.sh ; \
	fi;
