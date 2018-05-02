//Default libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>

//Libraries given in CS120B
#include "lcd.h"
#include "timer.h"
#include "bit.h"
#include "usart_ATmega1284.h"

//Global variables
unsigned long timer0Counter = 0x00;
unsigned char timerOn = 0x01;
unsigned long distance = 0x00;
unsigned long input = 0x00;

int main(void)
{
	DDRA = 0x00; PORTA = 0x00;	//Input from other ATmega1284
	DDRB = 0xFF; PORTB = 0x00;	//LCD data lines
	DDRC = 0xFF; PORTC = 0x00;  //Testing LED
	DDRD = 0x10; PORTD = 0x00;	//For Ultrasonic Sensor (ECHO on PD3; TRIG on PD4)

	LCD_init();
	TimerSet(500);
	TimerOn();
	
	
	
	/* Sample code for initializing interrupts *
	sei();								  //Enable global interrupts
	EICRA |= (1 << ISC01) | (1 << ISC00); //Trigger on rising edge of INT0 (PORTD initially high above)
	EIMSK |= 1 << INT0;					  //Enable interrupts on INT0
	
	EIMSK = 0x00;						  //Disable interrupts on INT0
	EICRA = 0x00;						  //Reset triggering event
	EICRA |= (1 << ISC01) | (0 << ISC00); //Trigger on falling edge of INT0
	EIFR |= 1 << INTF0;					  //Reset flag as required by datasheet
	PORTD = 0x00;						  //PORTD gets cleared to produce falling edge
	EIMSK |= 1 << INT0;					  //Enable interrupts on INT0 again
	_delay_ms(1000);					  //Delay time to "catch" falling edge, else falling edge doesn't get picked up
	cli();								  //Disable global interrupts (maybe unnecessary)
										  //***At this point, you should have seen the green LED blink twice
	/* End sample code */
	
	
	
	/* Sample code for testing itoa function and LCD screen * 
	char* str[10];				//Empty char to put ascii value
	itoa(702, str, 10);			//Convert the integer to ascii
	LCD_DisplayString(1, str);	//Display the ascii value onto position 1 of LCD screen
	//LCD_ClearScreen();
	/* End sample code */
	
	
	
	/* Sample code for testing timer0 function *
	sei();						//Enable global interrupts
	TCNT0 = 0x00;				//Initialize counter value to 0
	TCCR0B |= 1 << CS00;		//Start counter with no prescaler value
	TIMSK0 |= 1 << TOIE0;		//Enable overflow interrupts
	/* End sample code */
	
	
	
	/* Ultrasonic sensor code 
	sei();								  //Enable global interrupts
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
	_delay_ms(10);						  //Delay time to "catch" falling edge, else falling edge doesn't get picked up
	/* End Ultrasonic sensor code */

	while (1) {
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
		//PORTC = 0x01;				//Testing LED
		TCCR0B = 0x00;				//Stop timer
		TIMSK0 = 0x00;				//Disable overflow interrupts 
		char* test[10];	
		char* test2[10];			
		distance = (timer0Counter * 255) + TCNT0; //Calculate distance
		distance = distance / 920;				  //Convert to inches (Rough approximation)
		if (distance <= 60 && distance >= 10) {					  //'if' case is here to avoid bug of distance being over 1000 inches
			LCD_ClearScreen();
			ltoa(distance, test, 10);
			LCD_DisplayString(1, test);		
		}
		else if (distance < 10) {					  //'if' case is here to see if distance is less than 10 (the inner limit)
			LCD_ClearScreen();
			LCD_DisplayString(6, "Too close!");
		}
		
		//_delay_ms(500);
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
	}
	timer0Counter = 0x00;
	
	/* Test to show the ISR is taken (this is assumed to have an LED on PORTC0) *
	PORTC = 0x01;
	_delay_ms(1000);
	PORTC = 0x00;
	_delay_ms(1000);
	/* End test */
}

//Interrupt service routine to capture timer overflows
ISR(TIMER0_OVF_vect) {
	timer0Counter++;
	TCNT0 = 0;
	
	/* Test to show how many times timer0 overflows so it can be captured and calculated *
	char* test[10];
	ltoa(timer0Counter, test, 10);
	LCD_ClearScreen();
	LCD_DisplayString(1, test);
	TIMSK0 = 0x00;						//Disable overflow interrupts
	cli();								//Disable global interrupts
	/* End test */
}
