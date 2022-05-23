INC  += $(DB_BUILD_TOP)/../common/live555/UsageEnvironment/include
INC  += $(DB_BUILD_TOP)/../common/live555/groupsock/include
INC  += $(DB_BUILD_TOP)/../common/live555/liveMedia/include
INC  += $(DB_BUILD_TOP)/../common/live555/BasicUsageEnvironment/include
INC  += $(DB_BUILD_TOP)/../common/live555/mediaServer/include

ST_DEP := common vpe venc vif uvc uac
MODE := $(findstring fastboot, $(BOARD))

MODE:=fastboot
ifeq ($(MODE), fastboot)
LIBS += -lmi_vif -lmi_vpe -lmi_venc -lmi_iqserver -lfbc_decode -lmi_ao -lmi_ai
CODEDEFINE += -DFASTBOOT_MODE=1
else
ST_DEP += common vpe venc vif uvc rgn onvif live555
LIBS += -lmi_vif -lmi_vpe -lmi_venc -lmi_rgn -lmi_divp -lmi_iqserver -lmi_vdf \
		-lmi_shadow -lOD_LINUX -lMD_LINUX -lVG_LINUX -lmi_ive -lmi_ao
CODEDEFINE += -DFASTBOOT_MODE=0
endif
