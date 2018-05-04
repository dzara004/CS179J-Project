//Default libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <util/delay.h>

//Libraries given in CS120B/CS122A
#include "lcd.h"
#include "timer.h"
#include "bit.h"
#include "usart_ATmega1284.h"

//Global variables
unsigned char distance1 = 0x00;
unsigned char angle1 = 0x00;
unsigned char distance2 = 0x00;
unsigned char angle2 = 0x00;
unsigned char pick = 0x00;
char* display1[10];
char* display2[10];

//States for state machine
enum LCDState{display} state;

//State machine for displaying onto 16x2 LCD
void LCDTick() {
	//Transitions
	switch(state) {
		case display:
			state = display;
			break;
	}
	
	//Actions
	switch(state) {
		case display:
			//Send choice
			if (USART_IsSendReady(0)) {
				USART_Send(pick, 0);
				while (!USART_HasTransmitted(0));
			}
			
			//Display data based on choice
			//Get distance one
			if (pick == 0) {
				if(USART_HasReceived(0)){
					distance1 = USART_Receive(0);
					USART_Flush(0);
				}
				if (distance1 < 12) {
					set_PWM(500);
				} else {
					set_PWM(0);
				}
				itoa(distance1, display1, 10);
			} 
			//Get angle one
			else if (pick == 1) {
				if(USART_HasReceived(0)){
					angle1 = USART_Receive(0);
					USART_Flush(0);
				}
				LCD_ClearScreen();
				itoa(angle1, display2, 10);
				LCD_DisplayString(1, "Sensor1(in,deg):");
				LCD_DisplayString(22, display1);
				LCD_DisplayString(17, display2);
				_delay_ms(5000);
			} 
			//Get distance two
			else if (pick == 2) {
				if(USART_HasReceived(0)){
					distance2 = USART_Receive(0);
					USART_Flush(0);
				}
				if (distance2 < 12) {
					set_PWM(500);
				} else {
					set_PWM(0);
				}
				itoa(distance2, display1, 10);
			}
			//Get angle two
			else if (pick == 3) {
			if(USART_HasReceived(0)){
				angle2 = USART_Receive(0);
				USART_Flush(0);
			}
			LCD_ClearScreen();
			itoa(angle2, display2, 10);
			LCD_DisplayString(1, "Sensor2(in,deg):");
			LCD_DisplayString(22, display1);
			LCD_DisplayString(17, display2);
			_delay_ms(5000);
		}
		
		//Reset pick counter and start with distance one again
		++pick;
		if (pick == 4) {
			pick = 0;
		}
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
	DDRA = 0xFF; PORTA = 0x00;  //LCD data lines (replaced)
	DDRB = 0xFF; PORTB = 0x00;  //For speaker
	DDRC = 0xFF; PORTC = 0x00;  //LCD control lines
	DDRD = 0x02; PORTD = 0x00;  //USART (receiving on PD1)
	
	//Initializations
	PWM_on();
	initUSART(0);
	LCD_init();
	TimerSet(750);
	TimerOn();
	
	while (1) {
		LCDTick();
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
}
