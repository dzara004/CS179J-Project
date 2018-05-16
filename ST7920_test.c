/*
 * ST7920_test.c
 *
 * Created: 4/24/2018 2:19:04 PM
 * Author : mikem
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include "ST7920.h"

/*Functions to test functions*/
//void faceTest() {
	////Left eye
	//setPixel(12, 17, 0x18);
	//_delay_ms(100);
	//setPixel(12, 16, 0x3C);
	//_delay_ms(100);
	//setPixel(12, 15, 0xFF);
	//_delay_ms(100);
	//setPixel(12, 14, 0x3C);
	//_delay_ms(100);
	//setPixel(12, 13, 0x18);
	//_delay_ms(100);
	//
	////Right eye
	//setPixel(10, 17, 0x18);
	//_delay_ms(100);
	//setPixel(10, 16, 0x3C);
	//_delay_ms(100);
	//setPixel(10, 15, 0xFF);
	//_delay_ms(100);
	//setPixel(10, 14, 0x3C);
	//_delay_ms(100);
	//setPixel(10, 13, 0x18);
	//_delay_ms(100);
	//
	////Mouth
	//setPixel(3, 16, 0x81);
	//_delay_ms(100);
	//setPixel(3, 15, 0xFF);
	//_delay_ms(100);
	//
	////Nose
	//setPixel(11, 0, 0x66);
	//_delay_ms(100);
//}
//
//void pointsTest() {
	//unsigned char leftDistance1 = 15;
	//double leftAngle1 = 150 * (M_PI / 180);
	//plotLeft(leftDistance1, leftAngle1);
	//
	//_delay_ms(1000);
	//
	//unsigned char rightDistance1 = 31;
	//double rightAngle1 = 40 * (M_PI / 180);
	//plotRight(rightDistance1, rightAngle1);
	//
	//_delay_ms(1000);
	//
	//unsigned char leftDistance2 = 9;
	//double leftAngle2 = 10 * (M_PI / 180);
	//plotLeft(leftDistance2, leftAngle2);
	//
	//_delay_ms(1000);
	//
	//unsigned char rightDistance2 = 58;
	//double rightAngle2 = 0 * (M_PI / 180);
	//plotRight(rightDistance2, rightAngle2);
	//
	//_delay_ms(1000);
//}
//
///*Note: Custom LCD functions will be placed into its own library after further testing*/
//
//void initGraphic(void) {
	//PORTB &= 0xFB;	//Clear enable
	//_delay_us(72);
	//PORTB &= 0xFD;	//Clear RW
	//_delay_us(72);
	//PORTB &= 0xFE;	//Clear RS
	//_delay_us(72);
	//
	//PORTA = 0x30;   //Set as 8-bit interface
	//PORTB |= 0x04;	//Enable change
	//_delay_us(72);  //Stabilization time as required by the datasheet
	//PORTB &= ~0x04;	//Clear enable
	//_delay_us(72);
	//
	//_delay_ms(1);
	//
	//PORTA = 0x36;   //Set to graphics mode
	//PORTB |= 0x04;	//''
	//_delay_us(72);  //''
	//PORTB &= 0xFB;	//''
	//_delay_us(72);
	//
	//PORTA = 0x0c;   //Turn on display and leave cursor and blink off
	//PORTB |= 0x04;	//''
	//_delay_us(72);  //''
	//PORTB &= 0xFB;	//''
	//_delay_us(72);
	//
	//PORTA = 0x01;   //Clear the display and reset the address
	//PORTB |= 0x04;	//''
	//_delay_us(72);  //''
	//PORTB &= 0xFB;	//''
	//_delay_us(72);
	//
	//PORTA = 0x06;   //Turn on display with no cursor
	//PORTB |= 0x04;	//''
	//_delay_us(72);  //''
	//PORTB &= 0xFB;	//''
	//_delay_us(72);
//}
//
//void setPixel(unsigned char xPos, unsigned char yPos, unsigned long pixel) {
	//PORTB &= 0xFB;	//Clear enable
	//_delay_us(72);
	//PORTB &= 0xFD;	//Clear RW
	//_delay_us(72);
	//PORTB &= 0xFE;	//Clear RS
	//_delay_us(72);
	//
	//yPos |= 0x80;	//Set X and Y coordinates as shown below
	//PORTA = yPos;	
	//PORTB |= 0x04;	//Set enable
	//_delay_us(72);  //Stabilization time as required by the datasheet
	//PORTB &= 0xFB;	//Clear enable
	//_delay_us(72);
	//
	//xPos |= 0x80;   
	//PORTA = xPos;   
	//PORTB |= 0x04;	//''
	//_delay_us(72);  //''
	//PORTB &= 0xFB;	//''
	//_delay_us(72);
	//
	//PORTB |= 0x01;	//Set RS to signal for write
	//_delay_us(72);
	//
	//PORTA = (pixel >> 8);	//Set/Clear pixel as shown below
	//PORTB |= 0x04;	//''
	//_delay_us(72);  //''
	//PORTB &= 0xFB;	//''
	//_delay_us(72);
	//
	//PORTA = pixel;  
	//PORTB |= 0x04;	//''
	//_delay_us(72);  //''
	//PORTB &= 0xFB;	//''
	//_delay_us(72);
//}
//
//void clearScreen() {
	//unsigned char row = 0x00;
	//unsigned char column = 0x00;
	//
	//for (row = 0; row < 16; ++row) {
		//for (column = 0; column < 33; ++column) {
			//setPixel(row, column, 0x0000);
		//}
	//}
//}
//
//void drawAxes() {
	////Set y-axis
	//for (unsigned y = 0; y < 32; ++y) {
		//setPixel(11, y, 0x0001);
		//setPixel(3, y, 0x0001);
	//}
	//
	////Set x-axis
	//for (unsigned char x = 8; x < 16; ++x) {
		//setPixel(x, 0, 0xFFFF);
	//}
//}
//
//void plotLeft(unsigned char distance, double angle) {
	////Initializations
	//unsigned char xPos = 0x00;
	//unsigned char yPos = 0x00;
	//unsigned long pixel = 0x0000;
	//
	////Calculate X coordinate and pixel
	//double x = fabs(distance * cos(angle));
	//if (x >= 45.0) {
		//xPos = 0;
		//if (x >= 55.0) {
			//pixel = 0xE000;
		//} else if (x >= 50.0) {
			//pixel = 0x0380;
		//} else {
			//pixel = 0x000E;
		//}
	//} else if (x >= 30.0) {
		//xPos = 1;
		//if (x >= 40.0) {
			//pixel = 0xE000;
		//} else if (x >= 35.0) {
			//pixel = 0x0380;
		//} else {
			//pixel = 0x000E;
		//}
	//} else if (x >= 15.0) {
		//xPos = 2;
		//if (x >= 25.0) {
			//pixel = 0xE000;
		//} else if (x >= 20.0) {
			//pixel = 0x0380;
		//} else {
			//pixel = 0x000E;
		//}
	//} else {
		//xPos = 3;
		//if (x >= 10.0) {
			//pixel = 0xE000;
		//} else if (x >= 5.0) {
			//pixel = 0x0380;
		//} else {
			//pixel = 0x000E;
		//}
	//}
	//
	////Calculate Y coordinate
	//double y = distance * sin(angle);
	//if (y > 31.0) {
		//yPos = 31;
	//} else {
		//yPos = round(y);
	//}
	//
	////Plot point
	//double degrees = angle * (180 / M_PI);
	//if (degrees <= 90.0) {
		//setPixel(xPos, 32 - (yPos - 1), pixel);
		//setPixel(xPos, 32 - yPos, pixel);
		//setPixel(xPos, 32 - (yPos + 1), pixel);
	//} else {
		//setPixel(xPos + 8, yPos - 1, pixel);
		//setPixel(xPos + 8, yPos, pixel);
		//setPixel(xPos + 8, yPos + 1, pixel);
	//}
//}
//
//void plotRight(unsigned char distance, double angle) {
	////Initializations
	//unsigned char xPos = 0x00;
	//unsigned char yPos = 0x00;
	//unsigned long pixel = 0x0000;
	//
	////Calculate X coordinate and pixel
	//double x = fabs(distance * cos(angle));
	//if (x >= 45.0) {
		//xPos = 4;
		//if (x >= 55.0) {
			//pixel = 0xE000;
		//} else if (x >= 50.0) {
			//pixel = 0x0380;
		//} else {
			//pixel = 0x000E;
		//}
	//} else if (x >= 30.0) {
		//xPos = 5;
		//if (x >= 40.0) {
			//pixel = 0xE000;
		//} else if (x >= 35.0) {
			//pixel = 0x0380;
		//} else {
			//pixel = 0x000E;
		//}
	//} else if (x >= 15.0) {
		//xPos = 6;
		//if (x >= 25.0) {
			//pixel = 0xE000;
		//} else if (x >= 20.0) {
			//pixel = 0x0380;
		//} else {
			//pixel = 0x000E;
		//}
	//} else {
		//xPos = 7;
		//if (x >= 10.0) {
			//pixel = 0xE000;
		//} else if (x >= 5.0) {
			//pixel = 0x0380;
		//} else {
			//pixel = 0x000E;
		//}
	//}
	//
	////Calculate Y coordinate
	//double y = distance * sin(angle);
	//if (y > 31.0) {
		//yPos = 31;
	//} else {
		//yPos = round(y);
	//}
	//
	////Plot point
	//double degrees = angle * (180 / M_PI);
	//if (degrees <= 90.0) {
		//setPixel(xPos, 32 - (yPos - 1), pixel);
		//setPixel(xPos, 32 - yPos, pixel);
		//setPixel(xPos, 32 - (yPos + 1), pixel);
	//} else {
		//setPixel(xPos + 8, yPos - 1, pixel);
		//setPixel(xPos + 8, yPos, pixel);
		//setPixel(xPos + 8, yPos + 1, pixel);
	//}
//}

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;	//Set data port to output, initialize to 0
	DDRB = 0xFF; PORTB = 0x00;	//Set command port to output, initialize to 0
	DDRD = 0xF2; PORTD = 0x00;	//Testing LED
	
	_delay_ms(100);	//Stabilization period as required by the ST7920 datasheet
	
	initGraphic();	//Initialize as graphic display
	clearScreen();	//Clears LCD screen
	drawAxes();		//Draw axes
	
	//faceTest();
	pointsTest();
	
	/*Testing code
	PORTD = 0x80;
	_delay_ms(1000);
	PORTD = 0x00;
	_delay_ms(1000);
	*/
}
