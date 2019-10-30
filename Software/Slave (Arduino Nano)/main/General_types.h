/******************************************************************************************************************************************************************************************************************************************************** 
 * main.h
 * 
 * ToDo[PS]-  Need to insert comments
*********************************************************************************************************************************************************************************************************************************************************/

#include "Config.h"

/******************************************************************************************************************************************************************************************************************************************************** 
    ### Defines 
*********************************************************************************************************************************************************************************************************************************************************/

#define _MODULE_TYPE_LIGTH /* Choice a module type: { _MODULE_TYPE_PLUGS, _MODULE_TYPE_LIGTH } */

/* Defines to plugs module */
#ifdef _MODULE_TYPE_PLUGS
    /* Referencias do módulo PLUGS */
    #define _CONFIG_MODULE_TYPE       (0x02)
    #define _CONFIG_MODULE_DATA_SIZE  (0x10)
    /* Referencias dos pinos configurados como DIGITAL OUTPUT que controlam o estado dos plugs */
    #define _STATE_CTRL_PLUG_1        (4)
    #define _STATE_CTRL_PLUG_2        (5)
    #define _STATE_CTRL_PLUG_3        (6)
    #define _STATE_CTRL_PLUG_4        (7)
    /* Referencias dos pinos configurados como ANALOG INPUT que lêem o sensor de corrente dos plugs */
    #define _CURRENT_PLUG_1           (A0)
    #define _CURRENT_PLUG_2           (A1)
    #define _CURRENT_PLUG_3           (A2)
    #define _CURRENT_PLUG_4           (A4)
    /* Referencias dos indíces de cada parâmetro de configuração do primeiro plug no RX_BUFFER do frame de configuração (SID = 0x03)*/
    #define _PLUG_ID               (7)
    #define _PLUG_MODE             (8)
    #define _PLUG_ON_OFF           (9)
    #define _PLUG_SENSOR_ID        (10)
    #define _PLUG_MIN_MAX          (11)
    #define _PLUG_SET_POINT        (12)
    #define _PLUG_HOUR_ON_MSB      (13)
    #define _PLUG_HOUR_ON_LSB      (14)
    #define _PLUG_HOUR_OFF_MSB     (15)
    #define _PLUG_HOUR_OFF_LSB     (16)
    /* Referencias dos valores possíveis para cada parâmetro de configuração dos plugs */
    /* RX_BUFFER[_PLUG_ID + k] sendo k:{1, 7, 13, 19}; Equivale ao frame.Data[k] sendo k:{1, 7, 13, 19} */
    #define _PLUG_1      (0x00)
    #define _PLUG_2      (0x01)
    #define _PLUG_3      (0x02)
    #define _PLUG_4      (0x03)
    /* RX_BUFFER[_PLUG_MODE + k] sendo k:{0, 6, 12, 18}; Equivale ao frame.Data[k] sendo k:{0, 6, 12, 18} */
    #define _MANUAL            (0x00)
    #define _AUTO_SENSOR       (0x02)
    #define _AUTO_TIME         (0x03)
    /* RX_BUFFER[_PLUG_SENSOR_REF + k] sendo k:{1, 7, 13, 19}; Equivale ao frame.Data[k] sendo k:{1, 7, 13, 19} */
    #define _TEMPERATURA (0x00)
    #define _SALINIDADE  (0x01)
    #define _NIVEL       (0x02)
    #define _PH          (0x03)
    #define _VAZAO       (0x04)
    /* RX_BUFFER[_PLUG_LEVEL_TYPE + k] sendo k:{2, 8, 14, 20}; Equivale ao frame.Data[k] sendo k:{2, 8, 14, 20} */
    #define _MIN         (0x00)
    #define _MAX         (0x01)
    /* RX_BUFFER[_PLUG_ON_OFF + k] sendo k:{5, 11, 17, 23}; Equivale ao frame.Data[k] sendo k:{5, 11, 17, 23} */
    #define _OFF         (0x00)
    #define _ON          (0x01) 
    
    typedef struct{
        byte id;        /* ID do plug = {0x01 plug_1 | 0x02 plug_2 | 0x03 plug_3 | 0x03 plug_3} */
        byte mode;      /* Modo de operação = {0x00 Manual | 0x02 Auto_por_sensor | 0x03 Auto_por_hora} */
        byte on_off;
        byte sensor_ref;
        byte min_max;
        byte set_point;
        byte hour_on_msb;
        byte hour_on_lsb;
        byte hour_off_msb;
        byte hour_off_lsb;
        byte potency;
    }Plug_t;
    
    typedef struct{
        Plug_t plug_1;
        Plug_t plug_2;
        Plug_t plug_3;
        Plug_t plug_4;
    }Plugs_t;    

    typedef struct{
        byte temperatura;
        int  salinidade;
        byte nivel;
        byte ph;
        byte vazao;
    }Sensors_t;

    typedef struct{
        byte hour_msb;
        byte hour_lsb;
        int  hour;  /* Em minutos */
    }Time_t;
    
#endif

/* Defines to ligth module */
#ifdef _MODULE_TYPE_LIGTH
    /* Referencias do módulo LIGHT */
    #define _CONFIG_MODULE_TYPE      (0x03) 
    #define _CONFIG_MODULE_DATA_SIZE (0x0E)
    /* Referencias dos pinos configurados como DIGITAL OUTPUT que controlam os PWMs das barras de led */
    /* white led */
    #define _CTRL_LIGTH_4   (4) /* Enable A*/
    #define _CTRL_LIGTH_5   (5) /*  */
    #define _CTRL_LIGTH_6   (6)
    /* Blue led */
    #define _CTRL_LIGTH_7   (7) /* Enable B */
    #define _CTRL_LIGTH_9   (9)
    #define _CTRL_LIGTH_10   (10)
    /* Referencias dos indíces de cada parâmetro de configuração das barras de led no RX_BUFFER do frame de configuração (SID = 0x03)*/
    #define _LIGHT_START_HOUR  (7)
    #define _LIGHT_START_MIN   (8)
    #define _LIGHT_DAY_HOUR    (9)
    #define _LIGHT_DAY_MIN     (10)
    #define _LIGHT_NIGHT_HOUR  (11)
    #define _LIGHT_NIGHT_MIN   (12)
    #define _LIGHT_SWEEP_TIME  (13)
    #define _LIGHT_WHITE_DAY   (14)
    #define _LIGHT_WHITE_NIGHT (15)
    #define _LIGHT_BLUE_DAY    (16)
    #define _LIGHT_BLUE_NIGHT  (17)
    #define _LIGHT_RED_DAY     (18)
    #define _LIGHT_RED_NIGHT   (19)
    /* Referencias dos valores possíveis para cada parâmetro de configuração dos plugs */
    /* RX_BUFFER[_LIGHT_MODE]; Equivale ao frame.Data[0] */
    #define _MANUAL      (0x00)
    #define _AUTO_SENSOR (0x02)
    #define _AUTO_TIME   (0x03)

    typedef struct{
        byte day;
        byte night;
    }Light_t;
    
    typedef struct{
        byte mode;
        byte start_hour;
        byte start_min;
        byte day_hour;
        byte day_min;
        byte night_hour;
        byte night_min;
        byte sweep_time;
        Light_t led_white;
        Light_t led_blue;
        Light_t led_red;
    }Lights_t;

    typedef struct{
        byte temperatura;
        int  salinidade;
        byte nivel;
        byte ph;
        byte vazao;
    }Sensors_t;

    typedef struct{
        byte hour_msb;
        byte hour_lsb;
        int  hour;  /* Em minutos */
    }Time_t;
#endif


#ifdef _MODULE_TYPE_PLUGS
    extern Plugs_t set_module;          /* Estrutura usada para transmitir informações do módulo Comm_appl para o módulo Control_appl */
    extern Plugs_t status_module;       /* Estrutura usada para transmitir informações do módulo Control_appl para o módulo Comm_appl */
    extern Sensors_t sensor;
    extern Time_t time_now;
#endif
#ifdef _MODULE_TYPE_LIGTH
    extern Lights_t set_module;
    extern Lights_t status_module;
    extern Sensors_t sensor;
    extern Time_t time_now;
#endif
