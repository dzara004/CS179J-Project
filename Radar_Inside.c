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
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFA; PORTD = 0x00;	//For Ultrasonic Sensor (ECHO on PD3; TRIG on PD4)

	//Initializations
	PWM_on();
	//set_PWM(500);
	//PWM_off();
	initGraphic();
	clearScreen();
	drawAxes();
	_delay_ms(5000);
	
	LCD_init();
	LCD_ClearScreen();
	initUSART(0);
	initUSART(1);
	//540 fine without PIR motion sensor
	//600 find with PIR motion sensor
	//620 is the best by far, only bug occurring every ~5 passes
	//630 is even better than 620 with one bug occurring every ~20-25 passes
	//250 is testing
	//189 because 75x2.53
	TimerSet(129);
	TimerOn();
	
	BTState = wait;
	
	double angleOne;
	double angleTwo;
	unsigned char plotTimer = 0;
	unsigned char normalCount = 0;
	
	while (1) {
		BTTick();
		angleOne = a1 * (M_PI / 180);
		angleTwo = a2 * (M_PI / 180);
		
		//Integration portion
		if (USART_IsSendReady(1)) {
			USART_Send(m, 1);
			while (!USART_HasTransmitted(1)) {}
		}
		
		//if (((d1 < 12) && (m == 0)) || ((d2 < 12) & (m == 0))) {
			//++normalCount;
			//if (normalCount == 10) {
				//plotLeft(d1, angleOne, m);
				//plotRight(d2, angleTwo, m);
				//plotTimer = 0;
				//normalCount = 0;
			//}
		//} else if (plotTimer == 20) {
			//plotLeft(d1, angleOne, m);
			//plotRight(d2, angleTwo, m);
			//plotTimer = 0;
		//}
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
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
}
