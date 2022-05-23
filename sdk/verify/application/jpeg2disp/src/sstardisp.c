#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "mi_sys.h"
#include "sstardisp.h"

#include "mi_panel_datatype.h"
#include "mi_panel.h"
#include "mi_disp_datatype.h"
#include "mi_disp.h"


#if defined(__cplusplus)||defined(c_plusplus)
extern "C"{
#endif
int sstar_disp_init(MI_DISP_PubAttr_t *pstDispPubAttr)
{
	MI_DISP_DEV DispDev = 0;
	MI_DISP_LAYER DispLayer = 0;
    MI_PANEL_IntfType_e eIntfType;
	MI_PANEL_ParamConfig_t pstParamCfg;
    MI_DISP_InputPortAttr_t stInputPortAttr;
	MI_DISP_PubAttr_t stPubAttr;
	MI_DISP_VideoLayerAttr_t stLayerAttr;
	MI_DISP_RotateConfig_t stRotateConfig;

    memset(&stInputPortAttr, 0, sizeof(stInputPortAttr));

    MI_SYS_Init();
	printf("eIntfType=%d\n", pstDispPubAttr->eIntfType);
	MI_DISP_SetPubAttr(DispDev, pstDispPubAttr);
    MI_DISP_Enable(DispDev);

    eIntfType = pstDispPubAttr->eIntfType == E_MI_DISP_INTF_MIPIDSI ? E_MI_PNL_INTF_MIPI_DSI :
                pstDispPubAttr->eIntfType == E_MI_DISP_INTF_TTL_SPI_IF ? E_MI_PNL_INTF_TTL_SPI_IF :
                E_MI_PNL_INTF_TTL;
	
	MI_PANEL_Init(eIntfType);
	MI_PANEL_GetPanelParam(eIntfType, &pstParamCfg);

    memset(&stPubAttr, 0, sizeof(MI_DISP_PubAttr_t));
	memset(&stRotateConfig, 0, sizeof(MI_DISP_RotateConfig_t));

    stPubAttr.eIntfType = eIntfType;
    
    MI_DISP_GetPubAttr(DispDev,&stPubAttr);
    memset(&stLayerAttr, 0, sizeof(MI_DISP_VideoLayerAttr_t));
    stLayerAttr.stVidLayerDispWin.u16X = 0;
    stLayerAttr.stVidLayerDispWin.u16Y = 0;
    stLayerAttr.stVidLayerDispWin.u16Width = stPubAttr.stSyncInfo.u16Hact;
    stLayerAttr.stVidLayerDispWin.u16Height = stPubAttr.stSyncInfo.u16Vact;
    MI_DISP_BindVideoLayer(DispLayer, DispDev);
    MI_DISP_SetVideoLayerAttr(DispLayer, &stLayerAttr);
    MI_DISP_EnableVideoLayer(DispLayer);

    stRotateConfig.eRotateMode = E_MI_DISP_ROTATE_NONE;
    MI_DISP_SetVideoLayerRotateMode(DispLayer, &stRotateConfig);

    memset(&stInputPortAttr, 0, sizeof(MI_DISP_InputPortAttr_t));
    stInputPortAttr.u16SrcWidth = stPubAttr.stSyncInfo.u16Hact;
    stInputPortAttr.u16SrcHeight = stPubAttr.stSyncInfo.u16Vact;
    stInputPortAttr.stDispWin.u16X = 0;
    stInputPortAttr.stDispWin.u16Y = 0;
    stInputPortAttr.stDispWin.u16Width = stPubAttr.stSyncInfo.u16Hact;
    stInputPortAttr.stDispWin.u16Height = stPubAttr.stSyncInfo.u16Vact;
    MI_DISP_SetInputPortAttr(DispLayer, 0, &stInputPortAttr);
    MI_DISP_EnableInputPort(DispLayer, 0);

    
    return 0;
}
int sstar_disp_Deinit(MI_DISP_PubAttr_t *pstDispPubAttr)
{

    MI_DISP_DisableInputPort(0, 0);
    MI_DISP_DisableVideoLayer(0);
    MI_DISP_UnBindVideoLayer(0, 0);
    MI_DISP_Disable(0);

    switch(pstDispPubAttr->eIntfType) {
        case E_MI_DISP_INTF_HDMI:
            break;

        case E_MI_DISP_INTF_VGA:
            break;

        case E_MI_DISP_INTF_LCD:
        default:
            MI_PANEL_DeInit();

    }

    MI_SYS_Exit();
    printf("sstar_disp_Deinit...\n");

    return 0;
}

#if defined(__cplusplus)||defined(c_plusplus)
}
#endif

