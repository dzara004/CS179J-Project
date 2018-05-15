/*
 * ST7920_test.c
 *
 * Created: 4/24/2018 2:19:04 PM
 * Author : mikem
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>

/*Note: Custom LCD functions will be placed into its own library after further testing*/

void initGraphic(void) {
	PORTB &= 0xFB;	//Clear enable
	_delay_us(72);
	PORTB &= 0xFD;	//Clear RW
	_delay_us(72);
	PORTB &= 0xFE;	//Clear RS
	_delay_us(72);
	
	PORTA = 0x30;   //Set as 8-bit interface
	PORTB |= 0x04;	//Enable change
	_delay_us(72);  //Stabilization time as required by the datasheet
	PORTB &= ~0x04;	//Clear enable
	_delay_us(72);
	
	_delay_ms(1);
	
	PORTA = 0x36;   //Set to graphics mode
	PORTB |= 0x04;	//''
	_delay_us(72);  //''
	PORTB &= 0xFB;	//''
	_delay_us(72);
	
	PORTA = 0x0c;   //Turn on display and leave cursor and blink off
	PORTB |= 0x04;	//''
	_delay_us(72);  //''
	PORTB &= 0xFB;	//''
	_delay_us(72);
	
	PORTA = 0x01;   //Clear the display and reset the address
	PORTB |= 0x04;	//''
	_delay_us(72);  //''
	PORTB &= 0xFB;	//''
	_delay_us(72);
	
	PORTA = 0x06;   //Turn on display with no cursor
	PORTB |= 0x04;	//''
	_delay_us(72);  //''
	PORTB &= 0xFB;	//''
	_delay_us(72);
}

void setPixel(unsigned char xPos, unsigned char yPos, unsigned long pixel) {
	PORTB &= 0xFB;	//Clear enable
	_delay_us(72);
	PORTB &= 0xFD;	//Clear RW
	_delay_us(72);
	PORTB &= 0xFE;	//Clear RS
	_delay_us(72);
	
	yPos |= 0x80;	//Set X and Y coordinates as shown below
	PORTA = yPos;	
	PORTB |= 0x04;	//Set enable
	_delay_us(72);  //Stabilization time as required by the datasheet
	PORTB &= 0xFB;	//Clear enable
	_delay_us(72);
	
	xPos |= 0x80;   
	PORTA = xPos;   
	PORTB |= 0x04;	//''
	_delay_us(72);  //''
	PORTB &= 0xFB;	//''
	_delay_us(72);
	
	PORTB |= 0x01;	//Set RS to signal for write
	_delay_us(72);
	
	PORTA = (pixel >> 8);	//Set/Clear pixel as shown below
	PORTB |= 0x04;	//''
	_delay_us(72);  //''
	PORTB &= 0xFB;	//''
	_delay_us(72);
	
	PORTA = pixel;  
	PORTB |= 0x04;	//''
	_delay_us(72);  //''
	PORTB &= 0xFB;	//''
	_delay_us(72);
}

void clearScreen() {
	unsigned char row = 0x00;
	unsigned char column = 0x00;
	
	for (row = 0; row < 16; ++row) {
		for (column = 0; column < 33; ++column) {
			setPixel(row, column, 0x0000);
		}
	}
}

void faceTest() {
	//Left eye
	setPixel(12, 17, 0x18);
	_delay_ms(100);
	setPixel(12, 16, 0x3C);
	_delay_ms(100);
	setPixel(12, 15, 0xFF);
	_delay_ms(100);
	setPixel(12, 14, 0x3C);
	_delay_ms(100);
	setPixel(12, 13, 0x18);
	_delay_ms(100);
	
	//Right eye
	setPixel(10, 17, 0x18);
	_delay_ms(100);
	setPixel(10, 16, 0x3C);
	_delay_ms(100);
	setPixel(10, 15, 0xFF);
	_delay_ms(100);
	setPixel(10, 14, 0x3C);
	_delay_ms(100);
	setPixel(10, 13, 0x18);
	_delay_ms(100);
	
	//Mouth
	setPixel(3, 16, 0x81);
	_delay_ms(100);
	setPixel(3, 15, 0xFF);
	_delay_ms(100);
	
	//Nose
	setPixel(11, 0, 0x66);
	_delay_ms(100);
}

void drawAxes() {
	//Set y-axis
	for (unsigned y = 0; y < 32; ++y) {
		setPixel(11, y, 0x0001);
		setPixel(3, y, 0x0001);
	}
	
	//Set x-axis
	for (unsigned char x = 8; x < 16; ++x) {
		setPixel(x, 0, 0xFFFF);
	}
}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;	//Set data port to output, initialize to 0
	DDRB = 0xFF; PORTB = 0x00;	//Set command port to output, initialize to 0
	DDRD = 0xF2; PORTD = 0x00;	//Testing LED
	
	_delay_ms(100);	//Stabilization period as required by the ST7920 datasheet
	
	initGraphic();	//Initialize as graphic display
	clearScreen();	//Clears LCD screen
	drawAxes();		//Draw axes
	
	//setPixel(7, 13, 0x03C0);
	//setPixel(7, 12, 0x03C0);
	//setPixel(7, 11, 0x03C0);
	//setPixel(7, 10, 0x03C0);
	
	//faceTest();
	
	/*Sample code for plotting
	double x = 3.14;
	unsigned char y = 55;
	if (y > 31) {
		setPixel(round(x) + 8, y - 32, 0x0FF0);
	}
	setPixel(4, 55, 0x0FF0);
	*/
	
	/*Testing code for integration
	double x = cos(30);
	x = ceil(x);
	int xPos = x;
	*/
	
	/*Testing to see if syntax if correct
	PORTD |= 1 << PD7;
	_delay_ms(1000);
	PORTD &= 0 << PD7;
	_delay_ms(1000);
	PORTD |= 1 << PD7;
	_delay_ms(1000);
	PORTD &= 0 << PD7;
	*/
	
	/*Testing code
	PORTD = 0x80;
	_delay_ms(1000);
	PORTD = 0x00;
	_delay_ms(1000);
	*/
}
