INC  += $(DB_BUILD_TOP)/../common/live555/mediaServer/include

ST_DEP := common vpe vif
LIBS += -lmi_vif -lmi_vpe -lmi_disp -lmi_panel -lmi_isp -lmi_iqserver -lcus3a -lispalgo
