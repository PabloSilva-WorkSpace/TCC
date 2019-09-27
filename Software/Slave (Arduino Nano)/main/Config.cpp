/* Fish Tank automation project - TCC -  Config file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
 */


/* Headers includes */  
#include "Config.h"


/************************************************************************************************************ 
  @Brief: Setting GPIOs 
************************************************************************************************************/
void Config_configGPIO()
{
  pinMode(LED_ON_BOARD, OUTPUT);   /* GPIO 13 used to led blink */
}


/************************************************************************************************************ 
  @Brief: Setting UARTs
************************************************************************************************************/
void Config_configUART()
{  
  /* UART0 - Used to serial communication whith master module */ 
  Serial.begin(USART_BAUDRATE);
  PORTD &= 0xFE;   /* Disable pull up in PORTD0 (RxD pin) */
}


/************************************************************************************************************ 
  @Brief: Setting TIMERs 
************************************************************************************************************/
void Config_configTIMER()
{ 
  /* TIMER1 - Used to define base time (tick) of 10ms */
  TCCR1A = 0;                        /* Confira TIMER1 (16-Bits) para operação normal pinos OC1A e OC1B desconectados */
  TCCR1B = 0;                        /* Limpa registrador */
  TCCR1B |= (1<<CS10)|(1 << CS12);   /* Configura o prescaler para 1024: CS12 = 1 e CS10 = 1 */
  TCNT1 = 65380;                     /* Configura o contador do TIMER1 para chamar a ISR a cada 10ms (100Hz)   >>   65536-(16MHz/1024/100Hz) = 65380 */
  TIMSK1 |= (1 << TOIE1);            /* Habilita a interrupção do TIMER1 */
}
