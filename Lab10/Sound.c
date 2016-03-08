// Sound.c
// Runs on any computer
// Sound assets based off the original Space Invaders 
// Import these constants into your SpaceInvaders.c for sounds!
// Jonathan Valvano
// November 17, 2014
#include <stdint.h>
#include "Sound.h"
#include "DAC.h"
#include "tm4c123gh6pm.h"


void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
uint32_t n = 0; 



//"Once Upon a Dream" or "Waltz from Sleeping Beauty"


//what is this? 


void Sound_Init(void){
// write this		
  //Index = 0;
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 125000-1;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007; 		// enable SysTick with core clock and interrupts
	
};
void Sound_Play(uint32_t period){
// write this
	//From Lab 6
	DisableInterrupts(); 
	NVIC_ST_CTRL_R = 0;         								// disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;								// reload value
  NVIC_ST_CURRENT_R = 0;      								// any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007; 										// enable SysTick with core clock and interrupts	
	EnableInterrupts(); 
	//fuction pointer?
		
		
};
//sets frequency for next note
/*	//uint32_t period = song[n].period;
	NVIC_ST_CTRL_R = 0;         								// disable SysTick during setup
  NVIC_ST_RELOAD_R = period-1;								// reload value
  NVIC_ST_CURRENT_R = 0;      								// any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007; 										// enable SysTick with core clock and interrupts
	n = (n+1)%30;
	 
	 
 }*/
