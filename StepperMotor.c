#include <avr/io.h>
#include <avr/interrupt.h>
#include "timer.h"

unsigned long numPhases;
unsigned char stepOut[8] = {0x01, 0x03, 0x02, 0x06, 0x04, 0x0C,0x08,0x09};
unsigned int maxCnt = 0;
unsigned int cnt = 0;

unsigned char forCnt = 0;
unsigned char bckCnt = 7;
unsigned char clockwise = 0x01;

enum buttonStates{buttonWait,buttonPush,buttonGo} buttonState;
enum Direction {forward, backward} direction;
unsigned char button = 0x02;
unsigned char i = 0x00;

void Tick() {
	// Transitions for button pushed
	switch(buttonState) {
		case buttonWait:
			if(!button){
				buttonState = buttonWait;
			}
			else if (button == 0x02) {
				direction = forward;
				numPhases = (30 / 5.625) * 64;
				maxCnt = numPhases;
				buttonState = buttonGo;
				forCnt = 0;
				bckCnt = 7;
				cnt = 0;
				++i;
			} else if (button == 0x01) {
				direction = backward;
				numPhases = (30 / 5.625) * 64;
				maxCnt = numPhases;
				buttonState = buttonGo;
				forCnt = 0;
				bckCnt = 7;
				cnt = 0;
				++i;
			}
			break;
		case buttonGo:
			if(cnt < maxCnt )
				buttonState = buttonGo;
			else{
				buttonState = buttonWait;
			}
			break;
	}

	// Actions
	switch(buttonState){
		case buttonWait:
			if (i == 6 && button == 0x02) {
				button = 0x01;
				i = 0;
			} else if (i == 6 && button == 0x01) {
				button = 0x02;
				i = 0;
			}
			break;
		case buttonGo:
			if(direction == forward){
				PORTA = stepOut[forCnt];
				if(forCnt < 7)
					forCnt++;
				else
					forCnt=0;
			}
			else if(direction == backward){
				PORTA = stepOut[bckCnt];
				if(bckCnt > 0)
					bckCnt--;
				else
					bckCnt=7;
			}
			cnt++;
			break;
	}
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00; // Setting DDRA to output
    DDRB = 0x00; PORTB = 0xFF; // Setting DDRB to input

	TimerSet(3);
	TimerOn();
	buttonState = buttonWait;
    while (1)
    {
		Tick();
		while(!TimerFlag) {}
		TimerFlag = 0;
    }
}
