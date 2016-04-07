// SpaceInvaders.c

//should probably change name to SongSliders.c

// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10
// Modified by Vivian Tan and Carly Stalder

// Last Modified: 3/9/2016 
// http://www.spaceinvaders.de/
// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
/* This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2014

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2014

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PE3/AIN0
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "DAC.h"
#include "Sound.h" 
#include "Timer1.h"
#include "Timer0.h"

#include <time.h>
#include <stdlib.h>


//note timing
//last note note playing? 
//notes randomly freezing

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
	
#define white			ST7735_Color565(0, 0, 0);

const uint8_t notePositions [10] = {1, 3, 5, 7, 9, 11, 13, 15, 17, 19}; 
const unsigned char SineWave[16] = {1,3,5,8,11,13,15,15,15,13,11,8,5,3,1};  
unsigned char IndexW=0; 
char stringScore[5] = {'0', '0', '0', '0', '0'}; 




void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds
void Timer0_Init(void(*task)(void), uint32_t period);
void Timer1_Init(void(*task)(void), uint32_t period);
void ADC_Init(void); 
void LCD_OutFix(uint32_t);
void DAC_Init(void); 
void DAC_Out(unsigned char data);
void DreamPlay(void);
void WaitForInterrupt(void); 
int rand(void);
//void SysTick_Wait10ms(unsigned long delay);
void changeTempo(uint32_t tempo); 
uint32_t ADC_In(void);

uint32_t keep, ADCMail, ADCIn, screen, portD, c, tempo; 

uint32_t noteCount = 0; 
uint32_t score = 0; 
int lives = 3; 
uint32_t count = 1;
uint32_t loopNumber = 0; 
uint32_t timeCount = 0; 
uint32_t i = 0; 

uint32_t song2tempo = 30000000; 
uint32_t Dreamtempo = 20000000; 
uint32_t songLength = 29; 


//should probably put these in another file 

//*************************Structs/Enums*****************************//
	
	typedef enum{Menu, Screen1, Screen2, Screen3, Screen4, Screen5, Screen6, Screen7, Screen8, Screen9, Screen10, 
	Game1, Game2, Game3, Game4, Game5, Game6, Game7, Game8, Game9, Game10, GameOver}screenStatus; 
	
	typedef enum{New, Falling}noteStatus; 
	
	typedef enum{Grow, Invincible, Extra, None}powerStatus; 
	
	typedef enum{NewNote, Rest}newNoteStatus; 
	
	typedef enum{Dream, Dance, NoSong}songStatus; 
	
	
	screenStatus gameScreen = Menu; 
	songStatus songChosen = NoSong; 
	newNoteStatus next = NewNote; 
	
	struct note{
	char name;
	uint32_t time;
	uint32_t period; 						
};

typedef struct note NType;
	
const NType Dreaming [29] = {
	//{'C', 3, 9556},
	{'C', 3, 9556},			//START
	{'B', 3, 10124},
	{'C', 2, 9556},
	{'A', 1, 11364},
	{'B', 1, 10124},
	{'C', 1, 9556},
	{'A', 1, 11363}, 
	{'B', 2, 10124},
	{'D', 1, 8518},
	{'E', 2, 7587},
	{'c', 1, 9025},
	{'D', 6, 8518},			//	
	{'G', 3, 6378},
	{'f', 3, 6757},
	{'F', 2, 7153},
	{'D', 1, 8518},
	{'F', 1, 7135},
	{'E', 1, 7587},
	{'D', 1, 8518},
	{'A', 2, 5681},
	{'G', 1, 6378},
	{'F', 2, 7135},
	{'E', 2, 7587},
	{'D', 1, 8518},
	{'c', 1, 9025},
	{'D', 1, 8518},
	{'A', 1, 11364},
	{'B', 1, 10124}
};

const NType Wish [29] = {
	{'G', 1, 12755}, 
	{'G', 2, 12755},
	{'f', 1, 13513},
	{'G', 1, 12755},
	{'E', 3, 7584},
	{'B', 1, 10124},
	{'D', 2, 8513},
	{'C', 6, 9556},
	{'G', 3, 12755},
	{'C', 1, 9556},
	{'g', 3, 12040},
	{'B', 1, 10124},
	{'A', 6, 11364},
	//{'N', 1, 1},     //is this valid for a rest?
	{'A', 1, 11364},
	{'A', 2, 11364},
	{'g', 1, 12040},
	{'A', 1, 11364},
	{'F', 3, 7159},
	{'c', 1, 9019},
	{'E', 2, 7584},
	{'D', 5, 8513},
	{'c', 1, 9019},
	{'E', 2, 7584},
	{'D', 1, 8513},
	{'A', 1, 11364},
	{'C', 2, 9556},
	{'B', 1, 10124},
	{'G', 1, 12755},
	{'E', 7, 15169}
};

const NType Hold [38] = {
{'E', 2, 7584},
{'D', 1, 8513},
{'C', 1, 9556},
{'B', 5, 10124},
{'D', 1, 8513},
{'C', 1, 9556},
{'B', 1, 10124},
{'A', 7, 11364},
{'B', 5, 10124},
{'f', 6, 13514},
{'E', 2, 7584},
{'D', 1, 8513},
{'C', 1, 9556},
{'B', 5, 10124},
{'D', 1, 8513},
{'C', 1, 9556},
{'B', 1, 10124},
{'A', 7, 11364},
{'B', 7, 10124},
{'b', 9, 5062},
{'g', 1, 6378},
{'f', 1, 6757},
{'E', 1, 7584},
{'D', 2, 8513},
{'C', 2, 9556},
{'D', 1, 8513},
{'C', 1, 9556},
{'B', 1, 10124},
{'A', 1, 11364},
{'B', 1, 10124},
{'A', 1, 11364},
{'G', 2, 12755},
{'g', 1, 6378},
{'f', 1, 6757},
{'E', 1, 7584},
{'D', 2, 8513},
{'A', 2, 11364},
{'G', 6, 12755}
};

const NType HotBuns [17] = {
	//{'C', 3, 9556},
	{'B', 2, 10124},
	{'A', 2, 11364},
	{'G', 4, 12755},
	{'B', 2, 10124},
	{'A', 2, 11364},
	{'G', 4, 12755},
	{'G', 1, 12755},
	{'G', 1, 12755},
	{'G', 1, 12755},
	{'G', 1, 12755},
	{'A', 1, 11364},
	{'A', 1, 11364},
	{'A', 1, 11364},
	{'A', 1, 11364},
	{'B', 2, 10124},
	{'A', 2, 11364},
	{'G', 4, 12755}	
	
};
	
const NType Twinkle [43] = {
	{'C', 2, 9556},
	{'C', 2, 9556},
	{'G', 2, 6378},
	{'G', 2, 6378},
	{'A', 2, 5682},
	{'A', 2, 5682},
	{'G', 4, 6378},			//	
	{'F', 2, 7163},
	{'F', 2, 7163},
	{'E', 2, 7587},
	{'E', 2, 7587},
	{'D', 2, 8518},
	{'D', 2, 8518},
	{'C', 4, 9556},		//	
	{'G', 2, 6378},
	{'G', 2, 6378},
	{'F', 2, 7163},
	{'F', 2, 7163},
	{'E', 2, 7587},
	{'E', 2, 7587},
	{'D', 4, 8518},		//
	{'G', 2, 6378},
	{'G', 2, 6378},
	{'F', 2, 7163},
	{'F', 2, 7163},
	{'E', 2, 7587},
	{'E', 2, 7587},
	{'D', 4, 8518},		//
	{'C', 2, 9556},
	{'C', 2, 9556},
	{'G', 2, 6378},
	{'G', 2, 6378},
	{'A', 2, 5682},
	{'A', 2, 5682},
	{'G', 4, 6378},
	{'F', 2, 7163},
	{'F', 2, 7163},
	{'E', 2, 7587},
	{'E', 2, 7587},
	{'D', 2, 8518},		//
	{'D', 1, 8518},
	{'E', 1, 7587},
	{'C', 4, 9556}
	
};
const NType Wind [46] = {
{'E', 1, 15169},
{'G', 1, 12755},
{'C', 1, 9556},
{'B', 1, 10124},
{'B', 1, 10124 },
{'A', 1, 11364},
{'A', 1, 11364},
{'G', 1, 12755},
{'E', 1, 15169},
{'D', 1, 17027},
{'E', 2, 15169},
{'G', 2, 12755},
{'A', 3, 11364},
{'G', 1, 12755},
{'C', 1, 9556},
{'B', 1, 10124},
{'B', 1, 10124},
{'A', 1, 11364},
{'A', 1, 11364},
{'G', 1, 12755},
{'G', 1, 12755},
{'E', 1, 15169},
{'G', 6, 12755},
{'E', 1, 15169},
{'G', 1, 12755},
{'C', 1, 9556},
{'B', 1, 10124},
{'B', 1, 10124},
{'A', 1, 11364},
{'A', 1, 11364},
{'G', 1, 12755},
{'G', 1, 12755},
{'A', 1, 11364},
{'G', 2, 12755},
{'C', 4, 19111},
{'A', 1, 22727},
{'C', 1, 19111},
{'D', 1, 17027},
{'E', 1, 15169},
{'E', 1, 15169},
{'D', 1, 17027},
{'D', 1, 17027},
{'C', 1, 19111},
{'C', 1, 19111},
{'A', 1, 22727},
{'D', 6, 17027}
};

const NType Hakuna [43] = {
{'D', 1, 8513},
{'E', 2, 8035},
{'D', 1, 8513},
{'E', 1, 8035},
{'D', 1, 8513},
{'C', 6, 9556},
{'A', 1, 11364},
{'C', 1, 9556},
{'D', 1, 8513},
{'E', 1, 8035},
{'D', 1, 8513},
{'C', 6, 9556},
{'D', 1, 8513},
{'E', 2, 8035},
{'D', 1, 8513},
{'E', 1, 8035},
{'D', 1, 8513},
{'C', 6, 9556},
{'A', 2, 11364},
{'C', 2, 9556},
{'D', 2, 8513},
{'C', 2, 9556},
{'E', 6, 8035},
{'E', 1, 8035},
{'E', 2, 7584},
{'E', 2, 7584},
{'D', 2, 8513},
{'C', 6, 9556},
{'A', 2, 11364},
{'C', 1, 9556},
{'C', 2, 9556},
{'A', 1, 11364},
{'C', 2, 9556},
{'D', 6, 8513},
{'E', 2, 7584},
{'E', 2, 7584},
{'D', 1, 8513},
{'C', 2, 9556},
{'C', 6, 9556},
{'E', 2, 7584},
{'E', 2, 7584},
{'D', 1, 8513},
{'D', 6, 8513},
};

const NType Man[48] = {
{'E', 2, 7584},
{'B', 1, 10124},
{'D', 2, 8513},
{'A', 1, 11364},
{'B', 2, 10124},
{'D', 5, 8513},
{'A', 1, 11364},
{'B', 1, 10124},
{'C', 6, 9556},
{'D', 1, 8513},
{'A', 10, 11364},
{'E', 2, 7584},
{'B', 1, 10124},
{'D', 2, 8513},
{'A', 1, 11364},
{'B', 2, 10124},
{'D', 5, 8513},
{'A', 1, 11364},
{'B', 1, 10124},
{'C', 6, 9556},
{'D', 1, 8513},
{'D', 7, 8513},
{'D', 1, 17027},
{'D', 1, 17027},
{'E', 2, 15169},
{'E', 2, 15169},
{'E', 2, 15169},
{'A', 2, 11364},
{'G', 1, 12755},
{'S', 1, 13514},
{'S', 3, 13514},
{'S', 1, 13514},
{'G', 1, 12755},
{'A', 1, 11364},
{'B', 3, 10124},
{'C', 1, 9556},
{'B', 2, 10124},
{'D', 1, 17027},
{'E', 6, 15169},
{'C', 1, 9556},
{'B', 1, 10124},
{'G', 6, 12755},
{'E', 1,15169},
{'B', 1, 10124},
{'A', 6, 11364},
{'G', 1, 12755},
{'F', 1, 13514},
{'E', 9, 15169}
};


const NType World [30] = {
	{'f', 2, 13514},
	{'E', 1, 15169},
	{'G', 1, 12755},//
	{'G', 1, 12755},//
	{'f', 1, 13514},
	{'D', 2, 17026},
	{'A', 8, 22727},			//	LOWER
	{'f', 2, 13514},
	{'E', 1, 15169},
	{'G', 1, 12755},
	{'G', 1, 12755},
	{'f', 1, 13514},
	{'D', 2, 17026},
	{'f', 4, 13514},		//	
	{'E', 4, 15169},
	{'E', 2, 15169},
	{'d', 1, 16077},
	{'f', 1, 13514},
	{'f', 1, 13514},
	{'E', 1, 15169},
	{'c', 2, 18051},		//
	{'E', 2, 15169},
	{'D', 2, 17026},
	{'c', 2, 18051},
	{'D', 2, 17026},
	{'B', 1, 20243},
	{'c', 2, 18051},
	{'E', 2, 15169},		//
	{'D', 2, 17026},
	{'A', 9, 22727}
	};
	
const NType Favorite [44] = {
	{'E', 1, 15169},
	{'B', 1, 10124},
	{'B', 1, 10124},
	{'f', 1, 13514},
	{'E', 1, 15169},
	{'E', 1, 15169},
	{'b', 1, 20243},			//	
	{'E', 1, 15169},
	{'E', 1, 15169},
	{'f', 1, 13514},
	{'E', 1, 15169},
	{'N', 1, 1},
	{'E', 1, 15169},
	{'B', 1, 10124},		//	
	{'B', 1, 10124},
	{'f', 1, 13514},
	{'E', 1, 15169},
	{'E', 1, 15169},
	{'b', 1, 20243},
	{'E', 1, 15169},
	{'E', 1, 15169},
	{'f', 1, 13514},		//
	{'E', 1, 15169},
	{'N', 1, 1},
	{'E', 1, 15169},
	{'B', 1, 10124},
	{'A', 1, 11364},
	{'E', 1, 15169},
	{'f', 1, 13514},		//
	{'D', 1, 17026},
	{'D', 1, 17026},
	{'A', 1, 11364},
	{'G', 1, 12755},
	{'C', 3, 19111},
	{'B', 1, 20243},
	{'C', 1, 19111},
	{'D', 1, 17026},
	{'E', 1, 15169},
	{'f', 1, 13514},
	{'G', 1, 12755},
	{'A', 1, 11364},		//
	{'B', 1, 10124},
	{'A', 1, 11364},
	{'d', 6, 16077}
	
};


typedef struct fallingNotes_struct{
		uint32_t xLocation; 
		uint32_t yLocation;
		noteStatus status; 
		
	}fallingNotes; 
 typedef fallingNotes Notes; 
	
//powerups
	//location,
	 typedef struct powerup_struct{
		 uint32_t xLocation; 
		 uint32_t yLocation; 
		 uint32_t rarity; 
		 uint32_t time; 
		 char picture; 
		 powerStatus type; 
	 }powerup; 
	 typedef powerup Powerup;

	 
	 
	typedef struct character_struct{
		uint32_t xLocation;  
		powerStatus powerup; 
	}character; 
		typedef character Character; 

	Character player = {52, None};
	 
	//set array of notes
 Notes maxNotes [80] = {
					 {1, 3, Falling}, {3, 3,Falling}, {5, 3,Falling}, {7, 3,Falling},
				   {9, 3, Falling},{11, 3, Falling}, {13, 3,Falling}, {15, 3,Falling},
					 {17, 3,Falling}, {19, 3,Falling}, {1, 3, Falling}, {3, 3,Falling}, 
           {5, 3,Falling}, {7, 3, Falling}, {9, 3, Falling},{11, 3, Falling}, 
					 {13, 3,Falling}, {15, 3,Falling},{17, 3,Falling}, {19, 3,Falling},
					 {1, 3, Falling}, {3, 3,Falling}, {5, 3,Falling}, {7, 3,Falling},
				   {9, 3, Falling},{11, 3, Falling}, {13, 3,Falling}, {15, 3,Falling},
					 {17, 3,Falling}, {19, 3,Falling}, {1, 3, Falling}, {3, 3,Falling}, 
           {5, 3,Falling}, {7, 3, Falling}, {9, 3, Falling},{11, 3, Falling}, 
					 {13, 3,Falling}, {15, 3,Falling},{17, 3,Falling}, {19, 3,Falling},
					 {1, 3, Falling}, {3, 3,Falling}, {5, 3,Falling}, {7, 3,Falling},
				   {9, 3, Falling},{11, 3, Falling}, {13, 3,Falling}, {15, 3,Falling},
					 {17, 3,Falling}, {19, 3,Falling}, {1, 3, Falling}, {3, 3,Falling}, 
           {5, 3,Falling}, {7, 3, Falling}, {9, 3, Falling},{11, 3, Falling}, 
					 {13, 3,Falling}, {15, 3,Falling},{17, 3,Falling}, {19, 3,Falling},
					 {1, 3, Falling}, {3, 3,Falling}, {5, 3,Falling}, {7, 3,Falling},
				   {9, 3, Falling},{11, 3, Falling}, {13, 3,Falling}, {15, 3,Falling},
					 {17, 3,Falling}, {19, 3,Falling}, {1, 3, Falling}, {3, 3,Falling}, 
           {5, 3,Falling}, {7, 3, Falling}, {9, 3, Falling},{11, 3, Falling}, 
					 {13, 3,Falling}, {15, 3,Falling},{17, 3,Falling}, {19, 3,Falling}
				 };
 
 //set array of note xlocations
uint16_t noteLocations [9] = {3, 5, 7, 9, 11, 13, 15, 17, 19}; 



//**************************************Songs***************************************//
	
//************************************* Images ************************************//
// enemy ship that starts at the top of the screen (arms/mouth closed)
// width=16 x height=10

	const unsigned short Alien[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0xFFFF, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
};
	
	const unsigned short AlienCover[] = {
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,


};





//*******************************************Other************************************************//

//converts ADC to position on screen
uint32_t ADC_Convert(uint32_t input){		//from ADC value to position in FP	
	uint32_t pos; 
	pos = (1638*input + 692224);		//pos = (num*input); (pixel location?) 
	pos = pos >> 16; 
	keep = 0;
	//PF3 = 0;         
 // end of Convert Profile	
  return pos;
}

//converts ASCII xlocation to pixel location
uint32_t Ascii_ConvertX(uint32_t location){
	location = (location*5)+3;
	//location += 3; //maybe? 
	return location; 
}

Notes clearNotes(Notes noteArray[]){
	int n = 0; 
	for(n = 0; n<80; n++){
		noteArray[n].status = New; 
	}
	return noteArray[80];
}


//changes score string
void changeScoreString(void){
	//DisableInterrupts(); 
	uint32_t div = score; 
	div = score/10000; 
	stringScore[0] = div+0x30; 
	div = score%10000; 
	div = div/1000; 
	stringScore[1] = div+0x30; 
	div = (score%10000)%1000; 
	div = div/100; 
	stringScore[2] = div+0x30; 
	div = ((score%10000)%1000)%100; 
	div = div/10;
	stringScore[3] = div+0x30; 
	div = (((score%10000)%1000)%100)%10;
	stringScore[4] = div+0x30; 
	//previous = 1; 
	//EnableInterrupts();
}

//prints current score
void printScore(void){
	DisableInterrupts(); 
		ST7735_SetCursor(15, 1);
		changeScoreString(); 
		//char num = toString(score); 
		ST7735_OutString(stringScore); 
	EnableInterrupts(); 
}

//prints number of lives 
void printLives(void){
	DisableInterrupts(); 
	if (lives == 0){
		gameScreen = GameOver; 
	}
	if(lives >= 0){
	ST7735_SetCursor(17, 2);
	if(lives != 3){
	ST7735_OutChar(0x00); 
	ST7735_OutChar(0x00);
	ST7735_OutChar(0x00);
	ST7735_SetCursor(17, 2);
	}
	int n = 0; 
	for(n = 0; n< lives; n++)
		ST7735_OutChar(0x03); 
}
	EnableInterrupts(); 
}


//plays the next note in the song
//goes to systick? 

void PlaySong(void){
	
	//last note of song won't play? 
	i = (i+1)%songLength;
	
	if(gameScreen == Game1){
		Sound_Play(HotBuns[i].period);	 	//change note	
	}	 	
	else if(gameScreen == Game2){
		Sound_Play(Twinkle[i].period);	 	//change note	
	}	 
	else if(gameScreen == Game3){
		Sound_Play(Hold[i].period);	 	//change note	
	}	 
	else if(gameScreen == Game4){
		Sound_Play(Wish[i].period);	 	//change note	
	}	
	else if(gameScreen == Game5){
		Sound_Play(Dreaming[i].period);	 	//change note	
	}	 	
	else if(gameScreen == Game6){
		Sound_Play(Wind[i].period);	 	//change note	
	}	
	else if(gameScreen == Game7){
		Sound_Play(World[i].period);	 	//change note	
	}		
	else if(gameScreen == Game8){
		Sound_Play(Hakuna[i].period);	 	//change note	
	}		
	else if(gameScreen == Game9){
		Sound_Play(Man[i].period);	 	//change note	
	}	
	else if(gameScreen == Game10){
		Sound_Play(Favorite[i].period);	 	//change note	
	}	
		if(i == (songLength-1))
	{
		//i = 29;  
		loopNumber++; 
		if(loopNumber <=3){
			tempo = (tempo-loopNumber*1000000)%30000000; 
			changeTempo(tempo);
		}
		else{
			tempo = (tempo-loopNumber*80000)%30000000;
			changeTempo(tempo);
		}
		
	}
	//i = (i+1)%songLength;

}

//checks to see if note is caught
//returns a one if true
int checkCatch(Notes note){
	int location = note.xLocation; 
	location = Ascii_ConvertX(location); 
	int charloc = player.xLocation; 
  if((location>=(charloc-16)) && (location<= (charloc+20))){
		return 1; 
	}
	else
		return 0; 	
		
	}

//moves a note down
Notes moveNote (Notes note){
	//Random_Init(NVIC_ST_CURENT_R*ADC_In()); 		
	if(note.yLocation == (14)){
		int catch = checkCatch(note);
		if(catch == 1){
			score+=15*(loopNumber+1); 
			PlaySong(); 
			printScore(); 
		}
		else{
			lives--; 
      printLives();
			//PlaySong();
			if(i == (songLength-1))
				{
					//i = 29;  
					loopNumber++; 
					tempo = (tempo-loopNumber*1000000)%30000000; 
					changeTempo(tempo);
				}
					i = (i+1)%songLength;
		}
		note.status = New; 		
	}			
	ST7735_SetCursor(note.xLocation, note.yLocation); 
	ST7735_OutChar(0x00);																			//erase location
	note.yLocation = (note.yLocation+1)%15; 
	if(note.yLocation == 0){
		note.yLocation = 3; 
	}
	if(note.status == Falling){
	ST7735_SetCursor(note.xLocation, note.yLocation); 
	ST7735_OutChar(0x0E);
	}																				//draw new note	
	return note; 
	
}

//needs to sample and draw frequently in order to "cover" alien
///moves alien to a given position 
void moveCharacter(uint32_t position){
	ST7735_DrawBitmap(position, 159, Alien, 16, 10);
}


	




//***************************************Handlers**************************************************//

//plays song if note is caught?
//change to play song while other things are happening? how? 
void SysTick_Handler(void){	

	GPIO_PORTF_DATA_R ^= 0x08; 				//heartbeat--green LED on Launchpad	
  IndexW = (IndexW+1)&0x0F;      	 //increase index of sin wave value
	DAC_Out(SineWave[IndexW]);     	//output voltages go through given value
	
}

//samples ADC and draw's character
//much faster execution than NoteFall
void ADC(){
	 PF2 ^= 0x04;     								 // Heartbeat
	 ADCMail = ADC_In();
	 //PF2 ^= 0x04;      							// Heartbeat	
	 ADC0_SSCTL3_R &= 0x06;
	 ADCMail = ADC_Convert(ADCMail); 
	
	 if (player.xLocation != ADCMail){
	 ST7735_DrawBitmap(player.xLocation, 159, AlienCover, 16, 10);
	 player.xLocation = ADCMail; 
	 moveCharacter(ADCMail);
	 }		 	
}

//reads from song array and outputs notes onto the screen
//much slower execution than the ADC
//check for catcher here? Play sound from here? Calls SysTick? 
void NoteFall(){
	GPIO_PORTF_DATA_R ^= 0x02;
	//only put a note every so often (depends on time length) 	
	//put a new note
	
	
		int n = 0; 
	//moving
	for(n = 0; n<(songLength); n++){
		if(maxNotes[n].status == Falling) {
		maxNotes[n] = moveNote(maxNotes[n]);
		}																															//move notes
	}	
	//waiting
		uint32_t noteint = noteCount; 
	if(noteCount == 0){
		noteint++; 
	}	
	
	if(gameScreen == Game1){
		if(timeCount != HotBuns[((noteint)%(songLength))].time){
			next = Rest; 
			timeCount++; 
	}
		else{
			next = NewNote; 
			timeCount = 0 ;
	}	
}
	else if(gameScreen == Game2){
		if(timeCount != Twinkle[((noteint)%(songLength))].time){
			next = Rest; 
			timeCount++; 
	}	
			else{
				next = NewNote; 
				timeCount = 0 ;
		//i = (i+1)%29;
	}	
}	
	else if(gameScreen == Game3){
		if(timeCount != Hold[((noteint)%(songLength))].time){
			next = Rest; 
			timeCount++; 
	}	
			else{
				next = NewNote; 
				timeCount = 0 ;
	}	
}
	else if(gameScreen == Game4){
		if(timeCount != Wish[((noteint)%(songLength))].time){
			next = Rest; 
			timeCount++; 
	}	
			else{
				next = NewNote; 
				timeCount = 0 ;
	}	
}	
	else if(gameScreen == Game5){
		if(timeCount != Dreaming[((noteint)%(songLength))].time){
			next = Rest; 
			timeCount++; 
	}	
			else{
				next = NewNote; 
				timeCount = 0 ;
	}	
}		
	else if(gameScreen == Game6){
		if(timeCount != Wind[((noteint)%(songLength))].time){
			next = Rest; 
			timeCount++; 
	}	
			else{
				next = NewNote; 
				timeCount = 0 ;
	}	
}
	else if(gameScreen == Game7){
		if(timeCount != World[((noteint)%(songLength))].time){
			next = Rest; 
			timeCount++; 
	}	
			else{
				next = NewNote; 
				timeCount = 0 ;
	}	
}		
	else if(gameScreen == Game8){
		if(timeCount != Hakuna[((noteint)%(songLength))].time){
			next = Rest; 
			timeCount++; 
	}	
			else{
				next = NewNote; 
				timeCount = 0 ;
	}	
}		
	else if(gameScreen == Game9){
		if(timeCount != Man[((noteint)%(songLength))].time){
			next = Rest; 
			timeCount++; 
	}	
			else{
				next = NewNote; 
				timeCount = 0 ;
	}	
}		
else if(gameScreen == Game10){
		if(timeCount != Favorite[((noteint)%(songLength))].time){
			next = Rest; 
			timeCount++; 
	}	
			else{
				next = NewNote; 
				timeCount = 0 ;
	}	
}		
		
	
	if(next != Rest){
		srand(ADC_In()*(noteCount+1));
		int location = rand() % 9;	
		maxNotes[noteCount].xLocation = noteLocations[location]; 				//sets location of note
		maxNotes[noteCount].status = Falling; 
		noteCount = (noteCount+1)%(songLength);																		//increment number of notes
 }
	//move notes on screen

	//next = Rest; 
	
	//check to see if notes are caught?
	
}


	
//*****************************More Subroutines*********************************//
	void changeTempo(uint32_t tempo){
	//how to change tempo to beats per second? 
	Timer1_Init(&NoteFall, tempo);	
}
void Wait10ms(uint32_t count){
		uint32_t volatile time2;
		while(count>0){
    time2 = 727240/10;  // 0.1sec at 80 MHz
    while(time2){
	  	time2--;
    }
    count--;
  }
}

//*****************************Initializations***********************************//
//heartbeats
void PortF_Init(void){	
	volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x00000021;     // 1) activate clock for Port F
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTF_LOCK_R = 0x4C4F434B;   // 2) unlock GPIO Port F
  GPIO_PORTF_CR_R = 0x0F;           // allow changes to PF3-0
  // only PF0 needs to be unlocked, other bits can't be locked
  GPIO_PORTF_AMSEL_R = 0x00;        // 3) disable analog on PF
  GPIO_PORTF_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PF4-0
  GPIO_PORTF_DIR_R = 0x0E;          // 5) PF3-1 OUT
  GPIO_PORTF_AFSEL_R = 0x00;        // 6) disable alt funct on PF7-0
  GPIO_PORTF_PUR_R = 0x0E;          // enable pull-up on PF3-1
  GPIO_PORTF_DEN_R = 0x0E;          // 7) enable digital I/O on PF4-0
}



//buttons
void PortD_Init(void){	
	volatile unsigned long delay; 
  SYSCTL_RCGC2_R |= 0x0008;		// 1) activate clock for Port D 	
  delay = SYSCTL_RCGC2_R;           // allow time for clock to start
  GPIO_PORTD_CR_R = 0x03;           // allow changes to PD1-0
  GPIO_PORTD_AMSEL_R = 0x00;        // 3) disable analog on PD
  GPIO_PORTD_PCTL_R = 0x00000000;   // 4) PCTL GPIO on PD1-0
  GPIO_PORTD_DIR_R = 0x00;          // 5) PD0-1 IN
  GPIO_PORTD_AFSEL_R = 0x00;        // 6) disable alt funct on PD7-0
  GPIO_PORTD_DEN_R = 0x03;          // 7) enable digital I/O on PD1-0	

}


void SysTick_Init(void){	
	NVIC_ST_CTRL_R = 0;         								// disable SysTick during setup
  NVIC_ST_RELOAD_R = 125000-1;								// reload value
  NVIC_ST_CURRENT_R = 0;      								// any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007; 										// enable SysTick with core clock and interrupts
		}







//**********************************************Mains***********************************************//


	int main(void){
	tempo = Dreamtempo; 
	TExaS_Init(SSI0_Real_Nokia5110_Scope); 					 // set system clock to 80 MHz
  //Random_Init(NVIC_ST_CURRENT_R);
  Output_Init();
	ADC_Init(); 
	DAC_Init(); 
	//Sound_Init(); 
	PortF_Init();
	PortD_Init(); 
	Timer1_Init(&NoteFall, tempo);										//change to appropriate times
	Timer0_Init(&ADC, 250000);	
	
	DisableInterrupts(); 
	

	//handler heavy 
	 
//	EnableInterrupts(); 
		ST7735_FillScreen(0x0000);           							 // set screen to black
	  ST7735_SetCursor(3, 1);
		ST7735_OutChar(0x0E); 
		ST7735_DrawString(5, 1, "Song Slider", ST7735_YELLOW);
		ST7735_SetCursor(17, 1);
		ST7735_OutChar(0x0E); 
		ST7735_DrawString(4, 3, "Blue to scroll", ST7735_CYAN);	
		ST7735_DrawString(5, 5, "Red to start", ST7735_RED);	
		ST7735_DrawString(2, 11, "TEN songs to play!", ST7735_MAGENTA);
		ST7735_DrawString(4, 7, "Slide to catch!", ST7735_WHITE);
		gameScreen = Menu; 
	
	
	
	while(1){

		int v = 0;
		for(v = 0; v<80; v++){
			maxNotes[v].status = New; 
		}
		
		gameScreen = Menu; 
		
		while((gameScreen == Menu) || (gameScreen == Screen1) || (gameScreen == Screen2) || (gameScreen == Screen3)|| (gameScreen == Screen4) || (gameScreen == Screen5) 
			|| (gameScreen == Screen6) || (gameScreen == Screen7) || (gameScreen == Screen8) || (gameScreen == Screen9) || (gameScreen == Screen10)){

		uint32_t button1 = GPIO_PORTD_DATA_R; 					//check button 
		Wait10ms(15); 
		
		if(((button1&0x02) == 0x02) && (gameScreen == Screen1)){
			gameScreen = Game1; 
			songLength = 17; 
		}		
		
			
		if(((button1&0x02) == 0x02) && (gameScreen == Screen2)){
			gameScreen = Game2; 
			songLength = 43;  
		}		
		
		if(((button1&0x02) == 0x02) && (gameScreen == Screen3)){
			gameScreen = Game3; 
			songLength = 38; 
		}	
		if(((button1&0x02) == 0x02) && (gameScreen == Screen4)){
			gameScreen = Game4; 
			songLength = 29;  
		}		
		
		if(((button1&0x02) == 0x02) && (gameScreen == Screen5)){
			gameScreen = Game5; 
			songLength = 29; 
		}	
		if(((button1&0x02) == 0x02) && (gameScreen == Screen6)){
			gameScreen = Game6; 
			songLength = 46; 
		}				
		if(((button1&0x02) == 0x02) && (gameScreen == Screen7)){
			gameScreen = Game7; 
			songLength = 30; 
		}			
		if(((button1&0x02) == 0x02) && (gameScreen == Screen8)){
			gameScreen = Game8; 
			songLength = 43; 
		}		
		if(((button1&0x02) == 0x02) && (gameScreen == Screen9)){
			gameScreen = Game9; 
			songLength = 48; 
		}		
		if(((button1&0x02) == 0x02) && (gameScreen == Screen10)){
			gameScreen = Game10; 
			songLength = 44; 
		}		
					
		else if(((button1&0x01)==0x01 && (gameScreen == Menu))){		//go to next screen if at menu
			ST7735_FillScreen(0x0000);
			ST7735_SetCursor(1, 1); 
			ST7735_OutString("Hot X Buns"); 
			gameScreen = Screen1;  															
		}
		else if(((button1&0x01)==0x01) && (gameScreen == (Screen1))){
			ST7735_FillScreen(0x0000);
			ST7735_SetCursor(1, 1); 
			ST7735_OutChar(0x04);									//star 
			ST7735_OutString("Twinkle");
			ST7735_OutChar(0x04);									//star
			//ST7735_SetCursor(1, 1); 
			gameScreen = Screen2; 
		}
		
			else if(((button1&0x01)==0x01) && (gameScreen == (Screen2))){
			ST7735_FillScreen(0x0000);
			ST7735_SetCursor(1, 1); 			
			ST7735_OutString("Hold Your Hand"); 
			gameScreen = Screen3; 
		}
			else if(((button1&0x01)==0x01) && (gameScreen == (Screen3))){
			ST7735_FillScreen(0x0000);
			ST7735_SetCursor(1, 1); 			
			ST7735_OutString("A Dream is Wish"); 
			gameScreen = Screen4; 
		}	
			else if(((button1&0x01)==0x01) && (gameScreen == (Screen4))){
			ST7735_FillScreen(0x0000);
			ST7735_SetCursor(1, 1); 			
			ST7735_OutString("Dream Waltz"); 
			gameScreen = Screen5; 
		}	
			else if(((button1&0x01)==0x01) && (gameScreen == (Screen5))){
			ST7735_FillScreen(0x0000);
			ST7735_SetCursor(1, 1); 			
			ST7735_OutString("Colors Wind"); 
			gameScreen = Screen6; 
		}	
			else if(((button1&0x01)==0x01) && (gameScreen == (Screen6))){
			ST7735_FillScreen(0x0000);
			ST7735_SetCursor(1, 1); 			
			ST7735_OutString("I Can Show You"); 
			gameScreen = Screen7; 
		}	
			else if(((button1&0x01)==0x01) && (gameScreen == (Screen7))){
			ST7735_FillScreen(0x0000);
			ST7735_SetCursor(1, 1); 			
			ST7735_OutString("Hakuna Matata"); 
			gameScreen = Screen8; 
		}			
			else if(((button1&0x01)==0x01) && (gameScreen == (Screen8))){
			ST7735_FillScreen(0x0000);
			ST7735_SetCursor(1, 1); 			
			ST7735_OutString("I'll Make a Man"); 
			gameScreen = Screen9; 
		}				
			else if(((button1&0x01)==0x01) && (gameScreen == (Screen9))){
			ST7735_FillScreen(0x0000);
			ST7735_SetCursor(1, 1); 			
			ST7735_OutString("Favorite Things"); 
			gameScreen = Screen10; 
		}							
		
		else if (((button1&0x01)==0x01) && (gameScreen == Screen10)){
			ST7735_FillScreen(0x0000);           							 // set screen to black
			ST7735_SetCursor(3, 1);
			ST7735_OutChar(0x0E); 
			ST7735_DrawString(5, 1, "Song Slider", ST7735_GREEN);
			ST7735_SetCursor(17, 1);
			ST7735_OutChar(0x0E); 
			ST7735_DrawString(4, 3, "Blue to scroll", ST7735_CYAN);	
			ST7735_DrawString(5, 5, "Red to start", ST7735_RED);	
			ST7735_DrawString(2, 11, "TEN songs to play!", ST7735_MAGENTA);
			ST7735_DrawString(4, 7, "Slide to catch!", ST7735_WHITE);
			gameScreen = Menu;   																//delay
  
		}		
	}
		
	printScore();
	printLives();
  EnableInterrupts();
		while((gameScreen == Game1)||(gameScreen == Game1) || (gameScreen == Game2) || (gameScreen == Game3) || (gameScreen == Game4) || (gameScreen == Game5) 
			|| (gameScreen == Game6) || (gameScreen == Game7) || (gameScreen == Game8) || (gameScreen == Game9) || (gameScreen == Game10)){
		WaitForInterrupt(); 
		}
		
		DisableInterrupts(); 
		ST7735_FillScreen(0x0000); 
	 while(gameScreen == GameOver){		 
		 uint32_t button1 = GPIO_PORTD_DATA_R;
		 Wait10ms(10); 
		 if((button1&0x03)>= 0x01){
			  gameScreen = Menu; 
		 }
		 ST7735_SetCursor(5, 7);	
		 ST7735_OutString("GAME OVER");
		 ST7735_SetCursor(5, 8);
		 ST7735_OutString("SCORE:");
		 ST7735_OutString(stringScore);
		 //clear values
		 
		i = 0; 
		noteCount = 0; 
		tempo = Dreamtempo;
    lives = 3; 
    score = 0; 	
		//TExaS_Init(SSI0_Real_Nokia5110_Scope);
		//Output_Init();
		//ADC_Init(); 
		DAC_Init();
		Timer1_Init(&NoteFall, tempo);										//change to appropriate times
		Timer0_Init(&ADC, 250000);
		//DisableInterrupts(); 
		// SysTick_Wait10ms(2);  
	 }
	 
		 
	}


}


void Delay100ms(uint32_t count){
	uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}




