/* Fish Tank automation project - TCC -  main file
 * Developer: Pablo
 * 
 * ToDo[PENS] - need to improve the comments
*/


/* Headers includes */  
#include "main.h"
#include "Config.h"


/* Main Data of Module */
MainData mainData;


/* Variables into this scope (this file *.c) */
static int Counter1Of10ms = 0;
static int Counter2Of10ms = 0;
static int Counter3Of10ms = 0;


/* CallBack Timer1 runs every 10ms */ 
ISR(TIMER1_OVF_vect)
{
  /* Simple instructions */
  Counter1Of10ms++;
  Counter2Of10ms++;
  Counter3Of10ms++;
  /* Instructions to Timer1 peripheral */
  TCNT1 = 65380;  /* Reconfigura o contador do TIMER1 para chamar a ISR a cada 10ms */
}


void setup()
{
  configHardware();
}


void loop()
{
  if(Counter1Of10ms >= T10ms){
    Counter1Of10ms = 0;
    Task10ms();  /* call Task10ms() */
  }
  if(Counter2Of10ms >= T500ms){
    Counter2Of10ms = 0;
    Task500ms();  /* call Task500ms() */
  }
  if(Counter3Of10ms >= T1000ms){
    Counter3Of10ms = 0;
    Task1000ms();  /* call Task1000ms() */
  }
}


void Task10ms(void)
{
  Comm_appl_FSM(&mainData);
  Comm_appl_FRM(&mainData);
  Comm_appl_RHM(&mainData);
}


void Task500ms(void)
{
  digitalWrite(LED_ON_BOARD, !digitalRead(LED_ON_BOARD));
  //Comm_appl_Request_ChangeOf_FSM_State(&mainData, FSM_State_Send);
}


void Task1000ms(void)
{
  
}















/*
 Datasheet: http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-7810-Automotive-Microcontrollers-ATmega328P_Datasheet.pdf
 Manual: https://data2.manualslib.com/pdf5/121/12021/1202096-atmel/atmega48a.pdf?e996624a392c69ca68be012dd76f1410
 
 Fóruns:
   https://www.avrfreaks.net/forum/atmega328p-serial-communication-interrupt
   https://stackoverflow.com/questions/47017860/interrupt-driven-usart-using-udre-with-atmega328p
*/


/*

int incomming, i;
  if(Serial.available() > 0){
    incomming = Serial.available();
    Serial.print("incomming: ");
    Serial.println(incomming, DEC);
    //incomming = Serial.read();
    i = Serial.readBytes(RxBuffer, incomming);
    Serial.print("data: ");
    Serial.println(i, DEC);
    Serial.println(RxBuffer);
  }

*/
