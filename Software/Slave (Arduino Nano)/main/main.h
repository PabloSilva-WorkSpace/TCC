/******************************************************************************************************************************************************************************************************************************************************** 
 * main.h
 * 
 * ToDo[PS]-  Need to insert comments
*********************************************************************************************************************************************************************************************************************************************************/


#ifndef _MAIN_H
#define _MAIN_H


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Headers includes 
*********************************************************************************************************************************************************************************************************************************************************/
#include "Comm_appl.h"
#include "Control_appl.h"

/******************************************************************************************************************************************************************************************************************************************************** 
    ### Libraries includes 
*********************************************************************************************************************************************************************************************************************************************************/


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Defines 
*********************************************************************************************************************************************************************************************************************************************************/
#define T10ms (1)
#define T500ms (50)
#define T1000ms (100)


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Data Types 
*********************************************************************************************************************************************************************************************************************************************************/
typedef struct {
    Uart_t uart;
    Control_t control;
}MainData_t;


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Constants
*********************************************************************************************************************************************************************************************************************************************************/


/******************************************************************************************************************************************************************************************************************************************************** 
    ### Functions Prototypes 
*********************************************************************************************************************************************************************************************************************************************************/
void Task10ms(void);
void Task500ms(void);
void Task1000ms(void);

#endif
