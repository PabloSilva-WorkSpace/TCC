/******************************************************************************************************************************************************************************************************************************************************** 
 * Fish Tank automation project - TCC -  Config file
 * Developer: Pablo
 * 
 * ToDo[PS] - need to improve the comments
*********************************************************************************************************************************************************************************************************************************************************/


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/
#include "General_types.h"
#include "Control_appl.h"


#ifdef _MODULE_TYPE_PLUGS
void Control_appl_SMC( Control_t *pControl ){
    switch (pControl->SMC_State){
        case SMC_State_Idle:
        {
            //A inicialização do módulo pode ser feita aqui, pois este estado será chamado apena na primeira chamada desta máquina de estados
            Control_appl_Request_ChangeOf_SMC_State(pControl, SMC_State_Set_Module);
            break;
        }
        case SMC_State_Set_Module:   /* Controlling of module based on struct set_module */
        {
            Control_appl_Set_Module();
            Control_appl_Request_ChangeOf_SMC_State(pControl, SMC_State_Read_Module);
            break;
        }
        case SMC_State_Read_Module:   /* Return status of module on struct status_module */
        {
            Control_appl_Get_Module_Status();
            Control_appl_Request_ChangeOf_SMC_State(pControl, SMC_State_Set_Module);
            break;
        }
        case SMC_State_Error:
        {
            /* ToDo[PENS] error handler */
            Control_appl_Request_ChangeOf_SMC_State(pControl, SMC_State_Set_Module);
            break;
        } 
        default:
        {
            /* ToDo[PENS] - default handler */
            Control_appl_Request_ChangeOf_SMC_State(pControl, SMC_State_Set_Module);
        }
    }
}


void Control_appl_Request_ChangeOf_SMC_State(Control_t *pControl, SMC_States_t nextState)
{
    pControl->SMC_State = nextState;
}


void Control_appl_Set_Module( void )
{
    /* Setting module based on struct module */ 
    if(module.plug_1.mode == _MANUAL && module.plug_1.on_off == _ON){
        digitalWrite(_STATE_CTRL_PLUG_1, HIGH);
        digitalWrite(_STATE_CTRL_PLUG_2, HIGH);
        digitalWrite(_STATE_CTRL_PLUG_3, HIGH);
        digitalWrite(_STATE_CTRL_PLUG_4, HIGH);
    }
    if(module.plug_1.mode == _MANUAL && module.plug_1.on_off == _OFF){
        digitalWrite(_STATE_CTRL_PLUG_1, LOW);
        digitalWrite(_STATE_CTRL_PLUG_2, LOW);
        digitalWrite(_STATE_CTRL_PLUG_3, LOW);
        digitalWrite(_STATE_CTRL_PLUG_4, LOW);
    }
}


void Control_appl_Get_Module_Status( void )
{
    /* Returning module status on struct status_module. This values will are send on UART */
    status_module.plug_1.on_off  = digitalRead(_STATE_CTRL_PLUG_1);
    status_module.plug_2.on_off  = digitalRead(_STATE_CTRL_PLUG_1);
    status_module.plug_3.on_off  = digitalRead(_STATE_CTRL_PLUG_1);
    status_module.plug_4.on_off  = digitalRead(_STATE_CTRL_PLUG_1);
    status_module.plug_1.potency = analogRead(_CURRENT_PLUG_1)*127/4;
    status_module.plug_2.potency = analogRead(_CURRENT_PLUG_2)*127/4;
    status_module.plug_3.potency = analogRead(_CURRENT_PLUG_3)*127/4;
    status_module.plug_4.potency = analogRead(_CURRENT_PLUG_4)*127/4;
}
#endif

#ifdef _MODULE_TYPE_LIGTH
void Control_appl_SMC( void ){

}
#endif
