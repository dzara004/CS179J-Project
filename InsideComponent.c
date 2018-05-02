//Default libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

//Libraries given in CS120B
#include "lcd.h"
#include "timer.h"
#include "bit.h"
#include "usart_ATmega1284.h"

int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;	//LCD data lines
	DDRC = 0xFF; PORTC = 0x00;  //LCD control lines
	DDRD = 0x02; PORTD = 0x00;	//USART (receiving on PD1)

	initUSART(0);
	LCD_init();
	TimerSet(100);
	TimerOn();
	
	unsigned char distance1 = 0x00;
	unsigned char angle1 = 0x00;
	unsigned char distance2 = 0x00;
	unsigned char angle2 = 0x00;
	unsigned char pick = 0x00;
	char* display1[10];
	char* display2[10];
	
	while (1) {
		//Send choice
		if (USART_IsSendReady(0)) {
			USART_Send(pick, 0);
			while (!USART_HasTransmitted(0));
		}
		
		if (pick == 0) {
			if(USART_HasReceived(0)){
				distance1 = USART_Receive(0);
				USART_Flush(0);
			}
			itoa(distance1, display1, 10);
			LCD_ClearScreen();
			LCD_DisplayString(1, display1);
		} 
		//else if (pick == 1) {
			//if(USART_HasReceived(0)){
				//angle1 = USART_Receive(0);
				//USART_Flush(0);
			//}
			//LCD_ClearScreen();
			//ltoa(distance1, display1, 10);
			//itoa(angle1, display2, 10);
			//LCD_DisplayString(1, "Sensor1(in,deg):");
			//if (distance1 < 10) {
				//LCD_DisplayString(17, "!!!");
			//} else {
				//LCD_DisplayString(17, display1);
			//}
			//LCD_DisplayString(22, angle1);
		//} else if (pick == 2) {
			//if(USART_HasReceived(0)){
				//distance2 = USART_Receive(0);
				//USART_Flush(0);
			//}
		//} else if (pick == 3) {
			//if(USART_HasReceived(0)){
				//angle2 = USART_Receive(0);
				//USART_Flush(0);
			//}
			//LCD_ClearScreen();
			//ltoa(distance2, display1, 10);
			//itoa(angle2, display2, 10);
			//LCD_DisplayString(1, "Sensor2(in,deg):");
			//if (distance2 < 10) {
				//LCD_DisplayString(17, "!!!");
			//} else {
				//LCD_DisplayString(17, display1);
			//}
			//LCD_DisplayString(22, angle2);
		//}
		
		//++pick;
		//if (pick == 4) {
			//pick = 0;
		//}
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
}
