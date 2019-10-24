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
#include "Config.h"


/******************************************************************************************************************************************************************************************************************************************************** 
  @Brief: Setting GPIOs 
*********************************************************************************************************************************************************************************************************************************************************/
#ifdef _MODULE_TYPE_PLUGS
void Config_configGPIO()    /* Initial hardware configuration */
{
    /* Digital inputs */
    pinMode(COMM_ENALE_IN, INPUT);          /* D2 board pin */
    /* Analog inputs */
    pinMode(_CURRENT_PLUG_1, INPUT);
    pinMode(_CURRENT_PLUG_2, INPUT);
    pinMode(_CURRENT_PLUG_3, INPUT);
    pinMode(_CURRENT_PLUG_4, INPUT);
    /* Digital outputs */
    pinMode(_STATE_CTRL_PLUG_1, OUTPUT);    /* D4 board pin */
    pinMode(_STATE_CTRL_PLUG_2, OUTPUT);    /* D5 board pin */
    pinMode(_STATE_CTRL_PLUG_3, OUTPUT);    /* D6 board pin */
    pinMode(_STATE_CTRL_PLUG_4, OUTPUT);    /* D7 board pin */
    pinMode(LED_ON_BOARD, OUTPUT);          /* GPIO 13 used to led blink */
    pinMode(COMM_ENABLE_OUT, OUTPUT);       /* D3 board pin */
    
    /* Setting initial values*/
    digitalWrite(COMM_ENABLE_OUT, HIGH);
    digitalWrite(LED_ON_BOARD, LOW);
    digitalWrite(_STATE_CTRL_PLUG_1, LOW);
    digitalWrite(_STATE_CTRL_PLUG_2, LOW);
    digitalWrite(_STATE_CTRL_PLUG_3, LOW);
    digitalWrite(_STATE_CTRL_PLUG_4, LOW);
}
#endif

#ifdef _MODULE_TYPE_LIGTH
void Config_configGPIO()
{
    /* Digital inputs */
    pinMode(COMM_ENALE_IN, INPUT);          /* D2 board pin */
    /* Digital outputs */
    pinMode(COMM_ENABLE_OUT, OUTPUT);       /* D3 board pin */
    pinMode(LED_ON_BOARD, OUTPUT);          /* GPIO 13 used to led blink */
    pinMode(_CTRL_LIGTH_1, OUTPUT);         /* D4 board pin */
    pinMode(_CTRL_LIGTH_2, OUTPUT);         /* D5 board pin */
    pinMode(_CTRL_LIGTH_3, OUTPUT);         /* D6 board pin */
    
    /* Setting initial values*/
    digitalWrite(COMM_ENABLE_OUT, HIGH);
    digitalWrite(LED_ON_BOARD, LOW);
    digitalWrite(_CTRL_LIGTH_1, LOW);
    digitalWrite(_CTRL_LIGTH_2, LOW);
    digitalWrite(_CTRL_LIGTH_3, LOW);
}
#endif


/******************************************************************************************************************************************************************************************************************************************************** 
  @Brief: Setting UARTs
*********************************************************************************************************************************************************************************************************************************************************/
void Config_configUART()
{  
  /* UART0 - Used to serial communication whith master module */ 
  Serial.begin(USART_BAUDRATE);
  PORTD &= 0xFE;   /* Disable pull up in PORTD0 (RxD pin) */
}


/******************************************************************************************************************************************************************************************************************************************************** 
  @Brief: Setting TIMERs 
*********************************************************************************************************************************************************************************************************************************************************/
void Config_configTIMER()
{ 
  /* TIMER1 - Used to define base time (tick) of 10ms */
  TCCR1A = 0;                        /* Confira TIMER1 (16-Bits) para operação normal pinos OC1A e OC1B desconectados */
  TCCR1B = 0;                        /* Limpa registrador */
  TCCR1B |= (1<<CS10)|(1 << CS12);   /* Configura o prescaler para 1024: CS12 = 1 e CS10 = 1 */
  TCNT1 = 65380;                     /* Configura o contador do TIMER1 para chamar a ISR a cada 10ms (100Hz)   >>   65536-(16MHz/1024/100Hz) = 65380 */
  TIMSK1 |= (1 << TOIE1);            /* Habilita a interrupção do TIMER1 */
}
