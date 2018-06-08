//Default libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

//Libraries given in CS120B
#include "lcd.h"
#include "timer.h"
#include "bit.h"

//Library given in CS122A
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

//Global variables for display
unsigned char plotTimer = 0;

//States for output
enum outputStates{display} output;

//Display state machine
void DisplayTick() {
	//Transitions
	switch(output) {
		case display:
			output = display;
			break;
	}
	
	//Actions
	switch(output) {
		case display:
			unsigned char angleOne = a1 * (M_PI / 180);
			unsigned char angleTwo = a2 * (M_PI / 180);
			if (plotTimer == 20) {
				plotLeft(d1, angleOne, m);
				plotRight(d2, angleTwo, m);
				plotTimer = 0;
			}
			if (((m != 0) && (d1 < 12)) || ((m != 0) && (d2 < 12))) {
				set_PWM(500);
			} else {
				set_PWM(0);
			}
			++plotTimer;
			break;
	}
}

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
			break;
		case angle1:
			if (USART_HasReceived(0)) {
				a1 = USART_Receive(0);
				USART_Flush(0);
				select = 106;
			}
			break;
		case distance2:
			if (USART_HasReceived(0)) {
				d2 = USART_Receive(0);
				USART_Flush(0);
				select = 106;
			}
			break;
		case angle2:
			if (USART_HasReceived(0)) {
				a2 = USART_Receive(0);
				USART_Flush(0);
				select = 106;
			}
			break;
		case motion:
			if (USART_HasReceived(0)) {
				m = USART_Receive(0);
				USART_Flush(0);
				select = 106;
			}
			break;
	}
}

//PWM code given in 120B
void set_PWM(double frequency) {
	static double current_frequency; 
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; } 
		else { TCCR0B |= 0x03; }
		if (frequency < 0.954) { OCR0A = 0xFFFF; }
		
		else if (frequency > 31250) { OCR0A = 0x0000; }
		
		else { OCR0A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0; 
		current_frequency = frequency; 
	}
}

void PWM_on() {
	TCCR0A = (1 << COM0A0 | 1 << WGM00);
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;	//LCD data lines
	DDRB = 0xFF; PORTB = 0x00;	//LCD control lines
	DDRD = 0x0A; PORTD = 0x00;	//Bluetooths

	//Initializations
	initGraphic();
	PWM_on();
	
	//Start up radar and communication
	clearScreen();
	drawAxes();
	_delay_ms(5000);
	initUSART(0);
	initUSART(1);
	
	TimerSet(129);
	TimerOn();
	
	BTState = wait;
	output = display;
	
	while (1) {
		//Receive data from bluetooth
		BTTick();
		//Process and plot data
		DisplayTick();
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
}
