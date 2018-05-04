//Default libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

//Libraries given in CS120B/CS122A
#include "lcd.h"
#include "timer.h"
#include "bit.h"
#include "usart_ATmega1284.h"

//Global variables for stepper motor
unsigned long phaseCount;
unsigned char steps[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C,0x08,0x09};
unsigned int counter;
unsigned char forwardCount;
unsigned char backwardCount;
unsigned char rotate;
unsigned char i;

//Global variables for ultrasonic sensor and communication
unsigned long timer0Counter;
unsigned char timerOn;
unsigned long distance;
unsigned long input;
unsigned char count;
unsigned char pick;

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
	count = 0;
}

//States for stepper motor
enum Motor{wait, rotate} motor;
enum Rotation {clockwise, counterClockwise} route;

//State machine code for stepper motor
void MotorTick() {
	
	//Transitions
	switch(motor) {
		case wait:
			if (rotate == 0x02) {
				route = clockwise;
				phaseCount = (30 / 5.625) * 64;	//Rotate 30 degrees clockwise
				motor = rotate;
				forwardCount = 0;
				backwardCount = 7;
				counter = 0;
				++i;
			} else if (rotate == 0x01) {
				route = counterClockwise;
				phaseCount = (30 / 5.625) * 64;	//Rotate 30 degrees counter clockwise
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
			else{
				motor = wait;
			}
			break;
	}

	// Actions
	switch(motor) {
		case wait:
			if (i == 6 && rotate == 0x02) {
				rotate = 0x01;
				i = 0;
			} else if (i == 6 && rotate == 0x01) {
				rotate = 0x02;
				i = 0;
			}
			break;
		case rotate:
			if (route == clockwise) {
				PORTA = steps[forwardCount];	//Perform the stepper motor pattern
				if (forwardCount < 7)
					forwardCount++;
				else
					forwardCount=0;
			} else if (route == counterClockwise) {
				PORTA = steps[backwardCount];	//Perform the stepper motor pattern
				if(backwardCount > 0)
					backwardCount--;
				else
					backwardCount=7;
			}
			counter++;
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0x00;  //Input from other ATmega1284
	DDRB = 0xFF; PORTB = 0x00;  //LCD data lines
	DDRC = 0xFF; PORTC = 0x00;  //LCD control lines
	DDRD = 0x12; PORTD = 0x00;  //For Ultrasonic Sensor (ECHO on PD3; TRIG on PD4)

	initUSART(0);
	LCD_init();
	TimerSet(3);
	TimerOn();
	
	unsigned long timer0Counter = 0x00;
	unsigned char timerOn = 0x01;
	unsigned long distance = 0x00;
	unsigned long input = 0x00;
	unsigned char count = 0x00;
	unsigned char pick = 0x00;
	
	while (1) {
		//If 250ms has passed, get distance. Else, rotate stepper motor
		if (count == 83) {
			getDistance();
		} else {
			Tick();
		}
		++count;
		
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
		char* test[10];	
		char* test2[10];			
		distance = (timer0Counter * 255) + TCNT0; 		//Calculate distance
		distance = distance / 920;				//Convert to inches (Rough approximation)
		
		/* Begin testing case
		if (distance <= 60 && distance >= 10) {					  //'if' case is here to avoid bug of distance being over 1000 inches
			LCD_ClearScreen();
			ltoa(distance, test, 10);
			LCD_DisplayString(1, test);		
		}
		else if (distance < 10) {					  //'if' case is here to see if distance is less than 10 (the inner limit)
			LCD_ClearScreen();
			LCD_DisplayString(6, "Too close!");
		}
		
		input = PINA;
		if (input <= 60 && input >= 10) {					  //'if' case is here to avoid bug of distance being over 1000 inches
			//LCD_ClearScreen();
			ltoa(input, test2, 10);
			LCD_DisplayString(17, test2);
		}
		else if (input < 10) {					  //'if' case is here to see if distance is less than 10 (the inner limit)
			//LCD_ClearScreen();
			LCD_DisplayString(22, "Too close!");
		}
		/* End testing case*/
		
		input = PINA;
		
		//Get choice
		if (USART_HasTransmitted(0)) {
			pick = USART_Receive(0);
			USART_Flush(0);
		}
		if (pick == 0) {
			if(USART_IsSendReady(0)){
				USART_Send(distance, 0);
				while(!USART_HasTransmitted(0)){}
			}
		} else if (pick == 1) {
			if(USART_IsSendReady(0)){
				USART_Send(i * 10, 0);
				while(!USART_HasTransmitted(0)){}
			}
		} else if (pick == 2) {
			if(USART_IsSendReady(0)){
				USART_Send(input, 0);
				while(!USART_HasTransmitted(0)){}
			}
		} else if (pick == 3) {
			if(USART_IsSendReady(0)){
				USART_Send(180 - (i * 10), 0);
				while(!USART_HasTransmitted(0)){}
			}
		}
	}
	timer0Counter = 0x00;
}

//Interrupt service routine to capture timer overflows
ISR(TIMER0_OVF_vect) {
	timer0Counter++;
	TCNT0 = 0;
}
