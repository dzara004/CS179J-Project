//Default libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <util/delay.h>

//Libraries given in CS120B
#include "lcd.h"
#include "timer.h"
#include "bit.h"
#include "usart_ATmega1284.h"

//PWM code given in 120B
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; } //stops timer/counter
		else { TCCR0B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR0A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR0A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else { OCR0A = (short)(8000000 / (128 * frequency)) - 1; }

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}
}

void PWM_on() {
	TCCR0A = (1 << COM0A0 | 1 << WGM00);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;  //LCD data lines (replaced)
	DDRB = 0xFF; PORTB = 0x00;	//LCD data lines
	DDRC = 0xFF; PORTC = 0x00;  //LCD control lines
	DDRD = 0x02; PORTD = 0x00;	//USART (receiving on PD1)
	
	PWM_on();
	initUSART(0);
	LCD_init();
	TimerSet(750);
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
			if (distance1 < 12) {
				set_PWM(500);
			} else {
				set_PWM(0);
			}
			itoa(distance1, display1, 10);
			//LCD_ClearScreen();
			//LCD_DisplayString(1, "Distance 1 (in):");
			//LCD_DisplayString(17, display1);
		} 
		else if (pick == 1) {
			if(USART_HasReceived(0)){
				angle1 = USART_Receive(0);
				USART_Flush(0);
			}
			LCD_ClearScreen();
			itoa(distance1, display1, 10);
			itoa(angle1, display2, 10);
			LCD_DisplayString(1, "Sensor1(in,deg):");
			//if (distance1 < 10) {
				//LCD_DisplayString(22, "!!!");
			//} else {
				LCD_DisplayString(22, display1);
			//}
			LCD_DisplayString(17, display2);
			_delay_ms(5000);
		} 
		
		if (pick == 2) {
			if(USART_HasReceived(0)){
				distance2 = USART_Receive(0);
				USART_Flush(0);
			}
			if (distance2 < 12) {
				set_PWM(500);
			} else {
				set_PWM(0);
			}
			//if (distance2 < 10) {
				//distance2 = 10;
			//} else if (distance2 > 60) {
				//distance2 = 60;
			//}
			itoa(distance2, display1, 10);
			//LCD_ClearScreen();
			//LCD_DisplayString(1, "Distance 1 (in):");
			//LCD_DisplayString(17, display1);
		}
		else if (pick == 3) {
			if(USART_HasReceived(0)){
				angle2 = USART_Receive(0);
				USART_Flush(0);
			}
			LCD_ClearScreen();
			itoa(distance2, display1, 10);
			itoa(angle2, display2, 10);
			LCD_DisplayString(1, "Sensor2(in,deg):");
			//if (distance2 < 10) {
				//LCD_DisplayString(22, "!!!");
			//} else {
				LCD_DisplayString(22, display1);
			//}
				LCD_DisplayString(17, display2);
			_delay_ms(5000);
		}
		
		++pick;
		if (pick == 4) {
			pick = 0;
		}
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
}
