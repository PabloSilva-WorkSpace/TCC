/********************************************************************************************************************************************************************************************************************************************************
   Fish Tank automation project - TCC -  Config file
   Developer: Pablo

   ToDo[PS] - need to improve the comments
*********************************************************************************************************************************************************************************************************************************************************/


/********************************************************************************************************************************************************************************************************************************************************
    ### Headers includes
*********************************************************************************************************************************************************************************************************************************************************/
#include "General_types.h"
#include "Control_appl.h"


#ifdef _MODULE_TYPE_PLUGS
void Control_appl_SMC( Control_t *pControl ) {
  switch (pControl->SMC_State) {
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
  /* PLUG_1 */
  switch (set_module.plug_1.mode) {
    case _MANUAL: {
        if (set_module.plug_1.on_off == _ON) {
          digitalWrite(_STATE_CTRL_PLUG_1, HIGH);
          status_module.plug_1.on_off = _ON;
        } else if (set_module.plug_1.on_off == _OFF) {
          digitalWrite(_STATE_CTRL_PLUG_1, LOW);
          status_module.plug_1.on_off = _OFF;
        }
        break;
      }
    case _AUTO_SENSOR: {
        if (set_module.plug_1.sensor_ref == _TEMPERATURA) {
          if (set_module.plug_1.min_max == _MIN) {
            if (sensor.temperatura <= set_module.plug_1.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_1, HIGH);
            }
            else if (sensor.temperatura >= set_module.plug_1.set_point + 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_1, LOW);
            }
          }
          else if (set_module.plug_1.min_max == _MAX) {
            if (sensor.temperatura >= set_module.plug_1.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_1, HIGH);
            }
            else if (sensor.temperatura <= set_module.plug_1.set_point - 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_1, LOW);
            }
          }
        }
        else if (set_module.plug_1.sensor_ref == _SALINIDADE) {
          if (set_module.plug_1.min_max == _MIN) {
            if (sensor.salinidade <= set_module.plug_1.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_1, HIGH);
            }
            else if (sensor.salinidade >= set_module.plug_1.set_point + 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_1, LOW);
            }
          }
          else if (set_module.plug_1.min_max == _MAX) {
            if (sensor.salinidade >= set_module.plug_1.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_1, HIGH);
            }
            else if (sensor.salinidade <= set_module.plug_1.set_point - 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_1, LOW);
            }
          }
        }
        else if (set_module.plug_1.sensor_ref == _NIVEL) {
          if (sensor.nivel > set_module.plug_1.set_point) {
            digitalWrite(_STATE_CTRL_PLUG_1, HIGH);
          }
          else if (sensor.nivel <= set_module.plug_1.set_point) {
            digitalWrite(_STATE_CTRL_PLUG_1, LOW);
          }
        }
        else if (set_module.plug_1.sensor_ref == _PH) {
          if (set_module.plug_1.min_max == _MIN) {
            if (sensor.ph <= set_module.plug_1.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_1, HIGH);
            }
            else if (sensor.ph >= set_module.plug_1.set_point + 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_1, LOW);
            }
          }
          else if (set_module.plug_1.min_max == _MAX) {
            if (sensor.ph >= set_module.plug_1.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_1, HIGH);
            }
            else if (sensor.ph <= set_module.plug_1.set_point - 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_1, LOW);
            }
          }
        }
        break;
      }
    case _AUTO_TIME: {
        /* Obter horário do master*/
        /* Qual informação vai carregar o ou os horário(s) de ativação? */
        int hour_on  = ( (set_module.plug_1.hour_on_msb  << 8) | (set_module.plug_1.hour_on_lsb  << 0) );
        int hour_off = ( (set_module.plug_1.hour_off_msb << 8) | (set_module.plug_1.hour_off_lsb << 0) );
        //time_now.hour
        if(hour_on < hour_off ){
          if (hour_on <= time_now.hour && time_now.hour < hour_off){
            digitalWrite(_STATE_CTRL_PLUG_1, HIGH);
          }else{
            digitalWrite(_STATE_CTRL_PLUG_1, LOW);
          }
        }else{
          if (hour_on <= time_now.hour && time_now.hour < hour_off){
            digitalWrite(_STATE_CTRL_PLUG_1, LOW);
          }else{
            digitalWrite(_STATE_CTRL_PLUG_1, HIGH);
          }
        }
        break;
      }
  }

  /* PLUG_2 */
  switch (set_module.plug_2.mode) {
    case _MANUAL: {
        if (set_module.plug_2.on_off == _ON) {
          digitalWrite(_STATE_CTRL_PLUG_2, HIGH);
          status_module.plug_2.on_off = _ON;
        } else if (set_module.plug_2.on_off == _OFF) {
          digitalWrite(_STATE_CTRL_PLUG_2, LOW);
          status_module.plug_2.on_off = _OFF;
        }
        break;
      }
    case _AUTO_SENSOR: {
        if (set_module.plug_2.sensor_ref == _TEMPERATURA) {
          if (set_module.plug_2.min_max == _MIN) {
            if (sensor.temperatura <= set_module.plug_2.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_2, HIGH);
            }
            else if (sensor.temperatura >= set_module.plug_2.set_point + 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_2, LOW);
            }
          }
          else if (set_module.plug_2.min_max == _MAX) {
            if (sensor.temperatura >= set_module.plug_2.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_2, HIGH);
            }
            else if (sensor.temperatura <= set_module.plug_2.set_point - 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_2, LOW);
            }
          }
        }
        else if (set_module.plug_2.sensor_ref == _SALINIDADE) {
          if (set_module.plug_2.min_max == _MIN) {
            if (sensor.salinidade <= set_module.plug_2.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_2, HIGH);
            }
            else if (sensor.salinidade >= set_module.plug_2.set_point + 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_2, LOW);
            }
          }
          else if (set_module.plug_2.min_max == _MAX) {
            if (sensor.salinidade >= set_module.plug_2.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_2, HIGH);
            }
            else if (sensor.salinidade <= set_module.plug_2.set_point - 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_2, LOW);
            }
          }
        }
        else if (set_module.plug_2.sensor_ref == _NIVEL) {
          if (sensor.nivel > set_module.plug_2.set_point) {
            digitalWrite(_STATE_CTRL_PLUG_2, HIGH);
          }
          else if (sensor.nivel <= set_module.plug_2.set_point) {
            digitalWrite(_STATE_CTRL_PLUG_2, LOW);
          }
        }
        else if (set_module.plug_2.sensor_ref == _PH) {
          if (set_module.plug_2.min_max == _MIN) {
            if (sensor.ph <= set_module.plug_2.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_2, HIGH);
            }
            else if (sensor.ph >= set_module.plug_2.set_point + 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_2, LOW);
            }
          }
          else if (set_module.plug_2.min_max == _MAX) {
            if (sensor.ph >= set_module.plug_2.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_2, HIGH);
            }
            else if (sensor.ph <= set_module.plug_2.set_point - 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_2, LOW);
            }
          }
        }
        break;
      }
    case _AUTO_TIME: {
        /* Obter horário do master*/
        /* Qual informação vai carregar o ou os horário(s) de ativação? */
        int hour_on  = ( (set_module.plug_2.hour_on_msb  << 8) | (set_module.plug_2.hour_on_lsb  << 0) );
        int hour_off = ( (set_module.plug_2.hour_off_msb << 8) | (set_module.plug_2.hour_off_lsb << 0) );
        //time_now.hour
        if(hour_on < hour_off ){
          if (hour_on <= time_now.hour && time_now.hour < hour_off){
            digitalWrite(_STATE_CTRL_PLUG_2, HIGH);
          }else{
            digitalWrite(_STATE_CTRL_PLUG_2, LOW);
          }
        }else{
          if (hour_on <= time_now.hour && time_now.hour < hour_off){
            digitalWrite(_STATE_CTRL_PLUG_2, LOW);
          }else{
            digitalWrite(_STATE_CTRL_PLUG_2, HIGH);
          }
        }
        break;
      }
  }

  /* PLUG_3 */
  switch (set_module.plug_3.mode) {
    case _MANUAL: {
        if (set_module.plug_3.on_off == _ON) {
          digitalWrite(_STATE_CTRL_PLUG_3, HIGH);
          status_module.plug_3.on_off = _ON;
        } else if (set_module.plug_3.on_off == _OFF) {
          digitalWrite(_STATE_CTRL_PLUG_3, LOW);
          status_module.plug_3.on_off = _OFF;
        }
        break;
      }
    case _AUTO_SENSOR: {
        if (set_module.plug_3.sensor_ref == _TEMPERATURA) {
          if (set_module.plug_3.min_max == _MIN) {
            if (sensor.temperatura <= set_module.plug_3.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_3, HIGH);
            }
            else if (sensor.temperatura >= set_module.plug_3.set_point + 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_3, LOW);
            }
          }
          else if (set_module.plug_3.min_max == _MAX) {
            if (sensor.temperatura >= set_module.plug_3.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_3, HIGH);
            }
            else if (sensor.temperatura <= set_module.plug_3.set_point - 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_3, LOW);
            }
          }
        }
        else if (set_module.plug_3.sensor_ref == _SALINIDADE) {
          if (set_module.plug_3.min_max == _MIN) {
            if (sensor.salinidade <= set_module.plug_3.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_3, HIGH);
            }
            else if (sensor.salinidade >= set_module.plug_3.set_point + 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_3, LOW);
            }
          }
          else if (set_module.plug_3.min_max == _MAX) {
            if (sensor.salinidade >= set_module.plug_3.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_3, HIGH);
            }
            else if (sensor.salinidade <= set_module.plug_3.set_point - 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_3, LOW);
            }
          }
        }
        else if (set_module.plug_3.sensor_ref == _NIVEL) {
          if (sensor.nivel > set_module.plug_3.set_point) {
            digitalWrite(_STATE_CTRL_PLUG_3, HIGH);
          }
          else if (sensor.nivel <= set_module.plug_3.set_point) {
            digitalWrite(_STATE_CTRL_PLUG_3, LOW);
          }
        }
        else if (set_module.plug_3.sensor_ref == _PH) {
          if (set_module.plug_3.min_max == _MIN) {
            if (sensor.ph <= set_module.plug_3.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_3, HIGH);
            }
            else if (sensor.ph >= set_module.plug_3.set_point + 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_3, LOW);
            }
          }
          else if (set_module.plug_3.min_max == _MAX) {
            if (sensor.ph >= set_module.plug_3.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_3, HIGH);
            }
            else if (sensor.ph <= set_module.plug_3.set_point - 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_3, LOW);
            }
          }
        }
        break;
      }
    case _AUTO_TIME: {
        /* Obter horário do master*/
        /* Qual informação vai carregar o ou os horário(s) de ativação? */
        int hour_on  = ( (set_module.plug_3.hour_on_msb  << 8) | (set_module.plug_3.hour_on_lsb  << 0) );
        int hour_off = ( (set_module.plug_3.hour_off_msb << 8) | (set_module.plug_3.hour_off_lsb << 0) );
        //time_now.hour
        if(hour_on < hour_off ){
          if (hour_on <= time_now.hour && time_now.hour < hour_off){
            digitalWrite(_STATE_CTRL_PLUG_3, HIGH);
          }else{
            digitalWrite(_STATE_CTRL_PLUG_3, LOW);
          }
        }else{
          if (hour_on <= time_now.hour && time_now.hour < hour_off){
            digitalWrite(_STATE_CTRL_PLUG_3, LOW);
          }else{
            digitalWrite(_STATE_CTRL_PLUG_3, HIGH);
          }
        }
        break;
      }
  }

  /* PLUG_4 */
  switch (set_module.plug_4.mode) {
    case _MANUAL: {
        if (set_module.plug_4.on_off == _ON) {
          digitalWrite(_STATE_CTRL_PLUG_4, HIGH);
          status_module.plug_4.on_off = _ON;
        } else if (set_module.plug_4.on_off == _OFF) {
          digitalWrite(_STATE_CTRL_PLUG_4, LOW);
          status_module.plug_4.on_off = _OFF;
        }
        break;
      }
    case _AUTO_SENSOR: {
        if (set_module.plug_4.sensor_ref == _TEMPERATURA) {
          if (set_module.plug_4.min_max == _MIN) {
            if (sensor.temperatura <= set_module.plug_4.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_4, HIGH);
            }
            else if (sensor.temperatura >= set_module.plug_4.set_point + 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_4, LOW);
            }
          }
          else if (set_module.plug_4.min_max == _MAX) {
            if (sensor.temperatura >= set_module.plug_4.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_4, HIGH);
            }
            else if (sensor.temperatura <= set_module.plug_4.set_point - 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_4, LOW);
            }
          }
        }
        else if (set_module.plug_4.sensor_ref == _SALINIDADE) {
          if (set_module.plug_4.min_max == _MIN) {
            if (sensor.salinidade <= set_module.plug_4.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_4, HIGH);
            }
            else if (sensor.salinidade >= set_module.plug_4.set_point + 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_4, LOW);
            }
          }
          else if (set_module.plug_4.min_max == _MAX) {
            if (sensor.salinidade >= set_module.plug_4.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_4, HIGH);
            }
            else if (sensor.salinidade <= set_module.plug_4.set_point - 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_4, LOW);
            }
          }
        }
        else if (set_module.plug_4.sensor_ref == _NIVEL) {
          if (sensor.nivel > set_module.plug_4.set_point) {
            digitalWrite(_STATE_CTRL_PLUG_4, HIGH);
          }
          else if (sensor.nivel <= set_module.plug_4.set_point) {
            digitalWrite(_STATE_CTRL_PLUG_4, LOW);
          }
        }
        else if (set_module.plug_4.sensor_ref == _PH) {
          if (set_module.plug_4.min_max == _MIN) {
            if (sensor.ph <= set_module.plug_4.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_4, HIGH);
            }
            else if (sensor.ph >= set_module.plug_4.set_point + 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_4, LOW);
            }
          }
          else if (set_module.plug_4.min_max == _MAX) {
            if (sensor.ph >= set_module.plug_4.set_point) {
              digitalWrite(_STATE_CTRL_PLUG_4, HIGH);
            }
            else if (sensor.ph <= set_module.plug_4.set_point - 0.5) {
              digitalWrite(_STATE_CTRL_PLUG_4, LOW);
            }
          }
        }
        break;
      }
    case _AUTO_TIME: {
        /* Obter horário do master*/
        /* Qual informação vai carregar o ou os horário(s) de ativação? */
        int hour_on  = ( (set_module.plug_4.hour_on_msb  << 8) | (set_module.plug_4.hour_on_lsb  << 0) );
        int hour_off = ( (set_module.plug_4.hour_off_msb << 8) | (set_module.plug_4.hour_off_lsb << 0) );;
        //time_now.hour
        if(hour_on < hour_off ){
          if (hour_on <= time_now.hour && time_now.hour < hour_off){
            digitalWrite(_STATE_CTRL_PLUG_4, HIGH);
          }else{
            digitalWrite(_STATE_CTRL_PLUG_4, LOW);
          }
        }else{
          if (hour_on <= time_now.hour && time_now.hour < hour_off){
            digitalWrite(_STATE_CTRL_PLUG_4, LOW);
          }else{
            digitalWrite(_STATE_CTRL_PLUG_4, HIGH);
          }
        }
        break;
      }
  }
}

void Control_appl_Get_Module_Status( void )
{
  /* Returning module status on struct status_module. This values will are send on UART */
  /* Plug_1 */
  status_module.plug_1.id           = set_module.plug_1.id;
  status_module.plug_1.mode         = set_module.plug_1.mode;
  status_module.plug_1.on_off       = digitalRead(_STATE_CTRL_PLUG_1);
  status_module.plug_1.sensor_ref   = set_module.plug_1.sensor_ref;
  status_module.plug_1.min_max      = set_module.plug_1.min_max;
  status_module.plug_1.set_point    = set_module.plug_1.set_point;
  status_module.plug_1.hour_on_msb  = set_module.plug_1.hour_on_msb;
  status_module.plug_1.hour_on_lsb  = set_module.plug_1.hour_on_lsb;
  status_module.plug_1.hour_off_msb = set_module.plug_1.hour_off_msb;
  status_module.plug_1.hour_off_lsb = set_module.plug_1.hour_off_lsb;
  status_module.plug_1.potency      = analogRead(_CURRENT_PLUG_1) * 127 / 4;
  /* Plug_2 */
  status_module.plug_2.id           = set_module.plug_2.id;
  status_module.plug_2.mode         = set_module.plug_2.mode;
  status_module.plug_2.on_off       = digitalRead(_STATE_CTRL_PLUG_2);
  status_module.plug_2.sensor_ref   = set_module.plug_2.sensor_ref;
  status_module.plug_2.min_max      = set_module.plug_2.min_max;
  status_module.plug_2.set_point    = set_module.plug_2.set_point;
  status_module.plug_2.hour_on_msb  = set_module.plug_2.hour_on_msb;
  status_module.plug_2.hour_on_lsb  = set_module.plug_2.hour_on_lsb;
  status_module.plug_2.hour_off_msb = set_module.plug_2.hour_off_msb;
  status_module.plug_2.hour_off_lsb = set_module.plug_2.hour_off_lsb;
  status_module.plug_2.potency      = analogRead(_CURRENT_PLUG_2) * 127 / 4;
  /* Plug_3 */
  status_module.plug_3.id           = set_module.plug_3.id;
  status_module.plug_3.mode         = set_module.plug_3.mode;
  status_module.plug_3.on_off       = digitalRead(_STATE_CTRL_PLUG_3);
  status_module.plug_3.sensor_ref   = set_module.plug_3.sensor_ref;
  status_module.plug_3.min_max      = set_module.plug_3.min_max;
  status_module.plug_3.set_point    = set_module.plug_3.set_point;
  status_module.plug_3.hour_on_msb  = set_module.plug_3.hour_on_msb;
  status_module.plug_3.hour_on_lsb  = set_module.plug_3.hour_on_lsb;
  status_module.plug_3.hour_off_msb = set_module.plug_3.hour_off_msb;
  status_module.plug_3.hour_off_lsb = set_module.plug_3.hour_off_lsb;
  status_module.plug_3.potency      = analogRead(_CURRENT_PLUG_3) * 127 / 4;
  /* Plug_4 */
  status_module.plug_4.id           = set_module.plug_4.id;
  status_module.plug_4.mode         = set_module.plug_4.mode;
  status_module.plug_4.on_off       = digitalRead(_STATE_CTRL_PLUG_4);
  status_module.plug_4.sensor_ref   = set_module.plug_4.sensor_ref;
  status_module.plug_4.min_max      = set_module.plug_4.min_max;
  status_module.plug_4.set_point    = set_module.plug_4.set_point;
  status_module.plug_4.hour_on_msb  = set_module.plug_4.hour_on_msb;
  status_module.plug_4.hour_on_lsb  = set_module.plug_4.hour_on_lsb;
  status_module.plug_4.hour_off_msb = set_module.plug_4.hour_off_msb;
  status_module.plug_4.hour_off_lsb = set_module.plug_4.hour_off_lsb;
  status_module.plug_4.potency      = analogRead(_CURRENT_PLUG_4) * 127 / 4;
}
#endif


#ifdef _MODULE_TYPE_LIGTH
void Control_appl_SMC( Control_t *pControl ) {
  switch (pControl->SMC_State) {
    case SMC_State_Idle:
      {
        //A inicialização do módulo pode ser feita aqui, pois este estado será chamado apena na primeira chamada desta máquina de estados
        Control_appl_Request_ChangeOf_SMC_State(pControl, SMC_State_Set_Module);
        break;
      }
    case SMC_State_Set_Module:   /* Controlling of module based on struct set_module */
      {
        Control_appl_Set_Module();
        Control_appl_Request_ChangeOf_SMC_State(pControl, SMC_State_Set_Module);
        break;
      }
    case SMC_State_Read_Module:   /* Return status of module on struct status_module */
      {
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
  /* Control white LED*/

  analogWrite(_CTRL_LIGTH_4, (int)((set_module.led_white.day/100.0)*255));
  analogWrite(_CTRL_LIGTH_5, 0); //analogWrite(_CTRL_LIGTH_5, LOW);
  analogWrite(_CTRL_LIGTH_6, 255); //analogWrite(_CTRL_LIGTH_6, HIGH);
  //  /* Control Blue LED */
  //
  analogWrite(_CTRL_LIGTH_7, (int)((set_module.led_blue.day/100.0)*255));
  analogWrite(_CTRL_LIGTH_9, 0); // analogWrite(_CTRL_LIGTH_9, LOW);
  analogWrite(_CTRL_LIGTH_10, 255); // analogWrite(_CTRL_LIGTH_10, HIGH);


}


void Control_appl_Get_Module_Status( void )
{
  /* Returning module status on struct status_module. This values will are send on UART */
  
}
#endif
