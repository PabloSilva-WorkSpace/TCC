/******************************************************************************************************************************************************************************************************************************************************** 
 * main.h
 * 
 * ToDo[PS]-  Need to insert comments
*********************************************************************************************************************************************************************************************************************************************************/

#include "Config.h"

/******************************************************************************************************************************************************************************************************************************************************** 
    ### Defines 
*********************************************************************************************************************************************************************************************************************************************************/

#define _MODULE_TYPE_PLUGS /* Choice a module type: { _MODULE_TYPE_PLUGS, _MODULE_TYPE_LIGTH } */

/* Defines to plugs module */
#ifdef _MODULE_TYPE_PLUGS
    #define _CONFIG_MODULE_TYPE       (0x02)
    #define _CONFIG_MODULE_DATA_SIZE  (0x14)
    #define _STATE_CTRL_PLUG_1        (4)
    #define _STATE_CTRL_PLUG_2        (5)
    #define _STATE_CTRL_PLUG_3        (6)
    #define _STATE_CTRL_PLUG_4        (7)
    #define _CURRENT_PLUG_1           (A0)
    #define _CURRENT_PLUG_2           (A1)
    #define _CURRENT_PLUG_3           (A2)
    #define _CURRENT_PLUG_4           (A4)

    #define _PLUG_MODE        (7)
    #define _PLUG_SENSOR_REF  (8)
    #define _PLUG_LEVEL_TYPE  (9)
    #define _PLUG_SET_POINT   (10)
    #define _PLUG_POTENCY     (11)

    #define _MANUAL (0x00)
    
    typedef struct{
        byte mode;
        byte sensor_ref;
        byte level_type;
        byte set_point;
        byte potency;
    }Plug_t;
    
    typedef struct{
        Plug_t plug_1;
        Plug_t plug_2;
        Plug_t plug_3;
        Plug_t plug_4;
    }Plugs_t;
    
#endif
/* Defines to ligth module */
#ifdef _MODULE_TYPE_LIGTH
    #define _CONFIG_MODULE_TYPE (0x03) 
    #define _CONFIG_MODULE_DATA_SIZE (0x0E)
    #define _CTRL_LIGTH_1 (4)
    #define _CTRL_LIGTH_2 (5)
    #define _CTRL_LIGTH_3 (6)

    typedef struct{
        byte mode;
    }Ligths_t;
    
#endif


#ifdef _MODULE_TYPE_PLUGS
    extern Plugs_t module;
#endif
#ifdef _MODULE_TYPE_LIGTH
    extern Ligths_t module;
#endif
