//Default libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

//Libraries given in CS120B
#include "lcd.h"
#include "timer.h"
#include "bit.h"
#include "usart_ATmega1284.h"

//Global variables for bluetooth communication
unsigned char select = 105;
unsigned char val = 0x00;
unsigned char d1 = 0x00;
unsigned char a1 = 0x00;
unsigned char d2 = 0x00;
unsigned char a2 = 0x00;
char* data[10];

//States for bluetooth communication
enum BTStates{wait, distance1, angle1, distance2, angle2} BTState;

//Bluetooth communication state machine
void BTTick() {
	// Transitions
	switch(BTState) {
		case wait:
			if (select == 101) {
				BTState = distance1;
			} else if (select == 102) {
				BTState = angle1;
			} else if (select == 103) {
				BTState = distance2;
			} else if (select == 104) {
				BTState = angle2;
			} else if (select == 105) {
				BTState = wait;
			}
			break;
		case distance1:
			if (select == 101) {
				BTState = distance1;
			} else if (select != 101) {
				BTState = wait;
			}
			break;
		case angle1:
			if (select == 102) {
				BTState = angle1;
			} else if (select != 102) {
				BTState = wait;
			}
			break;
		case distance2:
			if (select == 103) {
				BTState = distance2;
			} else if (select != 103) {
				BTState = wait;
			}
			break;
		case angle2:
			if (select == 104) {
				BTState = angle2;
			} else if (select != 104) {
				BTState = wait;
			}
			break;
	}

	// Actions
	switch(BTState) {
		case wait:
			if (USART_HasReceived(0)) {
				val = USART_Receive(0);
			}
			
			if (val == 101) {
				select = 101;
			} else if (val == 102) {
				select = 102;
			} else if (val == 103) {
				select = 103;
			} else if (val == 104) {
				select = 104;
			} else {
				select = 105;
			}
			break;
		case distance1:
			if (USART_HasReceived(0)) {
				d1 = USART_Receive(0);
				USART_Flush(0);
				select = 105;
			}
			itoa(d1, data, 10);
			LCD_ClearScreen();
			LCD_DisplayString(1, "Distance 1:");
			LCD_DisplayString(17, data);
			break;
		case angle1:
		PORTC = 0x01;
		_delay_ms(1000);
		PORTC = 0x00;
		_delay_ms(1000);
			if (USART_HasReceived(0)) {
				a1 = USART_Receive(0);
				USART_Flush(0);
				select = 105;
			}
			itoa(a1, data, 10);
			LCD_ClearScreen();
			LCD_DisplayString(1, "Angle 1:");
			LCD_DisplayString(17, data);
			break;
		case distance2:
			if (USART_HasReceived(0)) {
				d2 = USART_Receive(0);
				USART_Flush(0);
				select = 105;
			}
			itoa(d2, data, 10);
			LCD_ClearScreen();
			LCD_DisplayString(1, "Distance 2:");
			LCD_DisplayString(17, data);
			break;
		case angle2:
			if (USART_HasReceived(0)) {
				a2 = USART_Receive(0);
				USART_Flush(0);
				select = 105;
			}
			itoa(a2, data, 10);
			LCD_ClearScreen();
			LCD_DisplayString(1, "Angle 2:");
			LCD_DisplayString(17, data);
			break;
	}
}

void receiveData() {
	PORTC = 0x01;
	_delay_ms(1000);
	PORTC = 0x00;
	_delay_ms(1000);
	
	if (USART_HasReceived(0)) {
		d1 = USART_Receive(0);
		USART_Flush(0);
	}
	
	char* str[10];
	itoa(d1, str, 10);
	LCD_ClearScreen();
	LCD_DisplayString(1, str);
	
	if (USART_HasReceived(0)) {
		d1 = USART_Receive(0);
		USART_Flush(0);
	}
	
	char* test[10];
	itoa(d1, test, 10);
	LCD_ClearScreen();
	LCD_DisplayString(1, test);
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0x02; PORTD = 0x00;	//For Ultrasonic Sensor (ECHO on PD3; TRIG on PD4)

	//Initializations
	LCD_init();
	LCD_ClearScreen();
	initUSART(0);
	//550 okay (only bug is in angle 1)
	TimerSet(525);
	TimerOn();
	
	BTState = wait;
	
	unsigned char test = 0x00;
	unsigned char pick = 0x01;
	char* str[10];

	while (1) {
		
		//receiveData();
		//itoa(d1, str, 10);
		//LCD_ClearScreen();
		//LCD_DisplayString(1, str);
		
		//Send choice
		//if (USART_IsSendReady(0)) {
			//USART_Send(pick, 0);
			//while (!(USART_HasTransmitted(0))) {}
		//}
		//if (pick == 1) {
			//if(USART_HasReceived(0)){
				////Code is not entering here!
				//PORTC = 0x01;
				//_delay_ms(1000);
				//PORTC = 0x00;
				//_delay_ms(1000);
				//test = USART_Receive(0);
				//USART_Flush(0);
			//}
			//itoa(test, str, 10);
			//LCD_ClearScreen();
			//LCD_DisplayString(1, str);
		//}
		
		/* Testing LED; Put this code in places where you think the error lies
		PORTC = 0x01;
		_delay_ms(1000);
		PORTC = 0x00;
		_delay_ms(1000);
		*/
		
		BTTick();
		
		while (!TimerFlag) {}
		TimerFlag = 0;
		//receiveData();
	}
}
