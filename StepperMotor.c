//Default libraries
#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

//Global variables
unsigned long phaseCount;
unsigned char steps[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C,0x08,0x09};
unsigned int counter;
unsigned char forwardCount;
unsigned char backwardCount;
unsigned char rotate;
unsigned char i;

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
	switch (motor) {
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
					forwardCount = 0;
			} else if(route == counterClockwise) {
				PORTA = steps[backwardCount];	//Perform the stepper motor pattern
				if (backwardCount > 0)
					backwardCount--;
				else
					backwardCount = 7;
			}
			counter++;
			break;
	}
}

int main(void)
{
    DDRA = 0xFF; PORTA = 0x00; //Connected to stepper motor
    
    //Initializations
    TimerSet(10);
    TimerOn();
    motor = wait;
    route = clockwise;
	
    unsigned long phaseCount = 0x00;
    unsigned int counter = 0x00;
    unsigned char forwardCount = 0x00;
    unsigned char backwardCount = 0x07;
    unsigned char rotate = 0x01;
    unsigned char i = 0x00;
	
    while (1)
    {
		MotorTick();
		while(!TimerFlag) {}
		TimerFlag = 0;
    }
}
