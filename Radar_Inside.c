//Default libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

//Libraries given in CS120B
#include "lcd.h"
#include "timer.h"
#include "bit.h"
#include "usart_ATmega1284.h"

//Libraries developed in CS179J
#include "ST7920.h"

//Global variables for bluetooth communication
unsigned char select = 105;
unsigned char val = 0x00;
unsigned char d1 = 0x00;
unsigned char a1 = 0x00;
unsigned char d2 = 0x00;
unsigned char a2 = 0x00;
unsigned char m = 0x00;
char* data[10];

//States for bluetooth communication
enum BTStates{wait, distance1, angle1, distance2, angle2, motion} BTState;

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
				BTState = motion;
			} else if (select == 106) {
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
		case motion:
			if (select == 105) {
				BTState = motion;
			} else if (select != 105) {
				BTState = wait;
			}
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
			} else if (val == 105) {
				select = 105;
			} else {
				select = 106;
			}
			break;
		case distance1:
			if (USART_HasReceived(0)) {
				d1 = USART_Receive(0);
				USART_Flush(0);
				select = 106;
			}
			itoa(d1, data, 10);
			LCD_ClearScreen();
			LCD_DisplayString(1, "Distance 1:");
			LCD_DisplayString(17, data);
			break;
		case angle1:
			if (USART_HasReceived(0)) {
				a1 = USART_Receive(0);
				USART_Flush(0);
				select = 106;
			}
			if ((a1 != 104) && (a1 != d2)) {
				itoa(a1, data, 10);
				LCD_ClearScreen();
				LCD_DisplayString(1, "Angle 1:");
				LCD_DisplayString(17, data);
			}
			break;
		case distance2:
			if (USART_HasReceived(0)) {
				d2 = USART_Receive(0);
				USART_Flush(0);
				select = 106;
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
				select = 106;
			}
			itoa(a2, data, 10);
			LCD_ClearScreen();
			LCD_DisplayString(1, "Angle 2:");
			LCD_DisplayString(17, data);
			break;
		case motion:
			if (USART_HasReceived(0)) {
				m = USART_Receive(0);
				USART_Flush(0);
				select = 106;
			}
			itoa(m, data, 10);
			LCD_ClearScreen();
			LCD_DisplayString(1, "Motion: ");
			LCD_DisplayString(17, data);
			break;
	}
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xF2; PORTD = 0x00;	//For Ultrasonic Sensor (ECHO on PD3; TRIG on PD4)

	//Initializations
	initGraphic();
	clearScreen();
	drawAxes();
	
	LCD_init();
	LCD_ClearScreen();
	initUSART(0);
	//540 fine without PIR motion sensor
	//600 find with PIR motion sensor
	//620 is the best by far, only bug occurring every ~5 passes
	//630 is even better than 620 with one bug occurring every ~20-25 passes
	TimerSet(630);
	TimerOn();
	
	BTState = wait;
	
	double angleOne;
	double angleTwo;
	
	while (1) {
		BTTick();
		angleOne = a1 * (M_PI / 180);
		angleTwo = a2 * (M_PI / 180);
		plotLeft(d1, angleOne);
		plotRight(d2, angleTwo);
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
}
