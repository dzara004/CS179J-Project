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

//Global variables for ultrasonic sensor
unsigned long timer0Counter = 0x00;
unsigned char timerOn = 0x01;
unsigned long distance = 0x00;
unsigned long input = 0x00;

//Global variables for stepper motor
unsigned long phaseCount;
unsigned char steps[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C,0x08,0x09};
unsigned int counter;
unsigned char forwardCount;
unsigned char backwardCount;
unsigned char rotate;
unsigned char i;

//States for stepper motor
enum Motor{ready, rotate} motor;
enum Rotation {clockwise, counterClockwise} route;

//States for bluetooth communication
enum BTStates{wait, readyD1, sendD1, readyA1, sendA1, readyD2, sendD2, readyA2, sendA2, readyM, sendM} BTState;
unsigned char select = 101;
unsigned char motion = 0x00;

//Bluetooth communication state machine
void BTTick() {
	// Transitions
	switch(BTState) {
		case wait:
			if (select == 101) {
				BTState = readyD1;
			} else if (select == 102) {
				BTState = readyA1;
			} else if (select == 103) {
				BTState = readyD2;
			} else if (select == 104) {
				BTState = readyA2;
			} else if (select == 105) {
				BTState = readyM;
			}
			break;
		case readyD1:
			BTState = sendD1;
			break;
		case sendD1:
			BTState = wait;
			break;
		case readyA1:
			BTState = sendA1;
			break;
		case sendA1:
			BTState = wait;
			break;
		case readyD2:
			BTState = sendD2;
			break;
		case sendD2:
			BTState = wait;
			break;
		case readyA2:
			BTState = sendA2;
			break;
		case sendA2:
			BTState = wait;
			break;
		case readyM:
			BTState = sendM;
			break;
		case sendM:
			BTState = wait;
			break;
	}

	// Actions
	switch(BTState){
		case wait:
			++select;
			if (select == 106) {
				select = 101;
			}
			break;
		case readyD1:
			if (USART_IsSendReady(0)) {
				USART_Send(101, 0);
				while (!(USART_HasTransmitted(0))) {}
			}
			break;
		case sendD1:
			if (USART_IsSendReady(0)) {
				USART_Send(distance, 0);
				while (!(USART_HasTransmitted(0))) {}
			}
			break;
		case readyA1:
			if (USART_IsSendReady(0)) {
				USART_Send(102, 0);
				while (!(USART_HasTransmitted(0))) {}
			}
			break;
		case sendA1:
			if (USART_IsSendReady(0)) {
				if (direction == forward) {
					USART_Send(i * 10, 0);
				} else {
					USART_Send(180 - (i * 10), 0);
				}
				while (!USART_HasTransmitted(0)) {}
			}
			break;
		case readyD2:
			if (USART_IsSendReady(0)) {
				USART_Send(103, 0);
				while (!(USART_HasTransmitted(0))) {}
			}
			break;
		case sendD2:
			input = PINA;
			if (USART_IsSendReady(0)) {
				USART_Send(input, 0);
				while (!(USART_HasTransmitted(0))) {}
			}
			break;
		case readyA2:
			if (USART_IsSendReady(0)) {
				USART_Send(104, 0);
				while (!(USART_HasTransmitted(0))) {}
			}
			break;
		case sendA2:
			if (USART_IsSendReady(0)) {
				if (direction == backward) {
					USART_Send(180 - (i * 10), 0);
				} else {
					USART_Send(i * 10, 0);
				}
				while (!USART_HasTransmitted(0)) {}
			}
			break;
		case readyM:
			if (USART_IsSendReady(0)) {
				USART_Send(105, 0);
				while (!(USART_HasTransmitted(0))) {}
			}
			break;
		case sendM:
			motion = PINB & 0x10;
			if (motion != 0x00) {
				PORTB |= 0x01;
			} else {
				PORTB &= 0xFE;
			}
			if (USART_IsSendReady(0)) {
				USART_Send(motion, 0);
				while (!(USART_HasTransmitted(0))) {}
			}
			break;
	}
}

//State machine code for stepper motor
void MotorTick() {
	//Transitions
	switch(motor) {
		case ready:
			if (rotate == 0x02) {
				route = clockwise;
				phaseCount = (10 / 5.625) * 64;	//Rotate 10 degrees clockwise
				motor = rotate;
				forwardCount = 0;
				backwardCount = 7;
				counter = 0;
				++i;
			} else if (rotate == 0x01) {
				route = counterClockwise;
				phaseCount = (10 / 5.625) * 64;	//Rotate 10 degrees counter clockwise
				motor = rotate;
				forwardCount = 0;
				backwardCount = 7;
				counter = 0;
				++i;
			}
			break;
		case rotate:
			if (counter < phaseCount )
				motor = rotate;
			else {
				motor = ready;
			}
			break;
	}


	// Actions
	switch(motor) {
		case ready:
			if (i == 18 && rotate == 0x02) {
				rotate = 0x01;
				i = 0;
			} else if (i == 18 && rotate == 0x01) {
				rotate = 0x02;
				i = 0;
			}
			break;
		case rotate:
			if (route == clockwise) {
				PORTC = steps[forwardCount];	//Perform the stepper motor pattern
				if (forwardCount < 7)
					forwardCount++;
				else
					forwardCount = 0;
			} else if (route == counterClockwise) {
				PORTC = steps[backwardCount];	//Perform the stepper motor pattern
				if(backwardCount > 0)
					backwardCount--;
				else
					backwardCount = 7;
			}
			counter++;
			break;
	}
}



//Function used to retrieve distance
void getDistance() {
	timerOn = 0x01;
	_delay_us(10);						  //Wait 10 us
	PORTD = 0x10;						  //Set trig HIGH
	_delay_us(10);						  //Wait 10 us
	PORTD = 0x00;						  //Set trig LOW
	_delay_us(10);						  //Set 10 us
	
	EICRA |= (1 << ISC11) | (1 << ISC10); //Trigger on rising edge of INT1
	EIMSK |= 1 << INT1;					  //Enable interrupts on INT1
	_delay_ms(10);
	
	EIMSK = 0x00;						  //Disable interrupts on INT1
	EICRA = 0x00;						  //Reset triggering event
	timerOn = 0x00;						  //Stop timer variable
	EICRA |= (1 << ISC11) | (0 << ISC10); //Trigger on falling edge of INT1
	//EIFR |= 1 << INTF1;				  //Reset flag as required by datasheet
	EIMSK |= 1 << INT1;					  //Enable interrupts on INT1 again
	//_delay_ms(10);						  //Delay time to "catch" falling edge, else falling edge doesn't get picked up
}

int main(void)
{
	DDRA = 0x00; PORTA = 0x00;	//Other distance
	DDRB = 0x0F; PORTB = 0x00;	//Testing LED and PIR motion sensor
	DDRC = 0xFF; PORTC = 0x00;	//Stepper motor
	DDRD = 0x12; PORTD = 0x00;	//For Ultrasonic Sensor (ECHO on PD3; TRIG on PD4)

	//Initializations
	LCD_init();
	initUSART(0);
	TimerSet(3);
	TimerOn();
	
	motor = ready;
	route = clockwise;
	BTState = wait;
	
	//Counting variable for timing the 'tick' functions
	unsigned char motorCount = 0;
	unsigned char btCount = 0;
	
	while (1) {
		//Every ~75ms
		if (motorCount == 17) {
			BTTick();
			getDistance();
			motorCount = 0;
		} else {
			MotorTick();
			++motorCount;
			++btCount;
		}
		
		while (!TimerFlag) {}
		TimerFlag = 0;
	}
}

//Interrupt service routine to capture rising edges on INT1
ISR(INT1_vect) {
	if (timerOn) {
		TCCR0B |= 1 << CS00;		//Start counter with no prescaler value
		TIMSK0 |= 1 << TOIE0;		//Enable overflow interrupts
	}
	else {
		TCCR0B = 0x00;				//Stop timer
		TIMSK0 = 0x00;				//Disable overflow interrupts 
		distance = (timer0Counter * 255) + TCNT0; //Calculate distance
		distance = distance / 920;				  //Convert to inches (Rough approximation)
	}
	timer0Counter = 0x00;
}

//Interrupt service routine to capture timer overflows
ISR(TIMER0_OVF_vect) {
	timer0Counter++;
	TCNT0 = 0;
}
