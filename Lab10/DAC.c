// dac.c
// This software configures DAC output
// Runs on LM4F120 or TM4C123
// Program written by: Carly S., Vivian T. 
// Date Created: 8/25/2014 
// Last Modified: 4/23/2015 
// Section 1-2pm     TA: Wooseok Lee
// Lab number: 6
// Hardware connections
// DAC connections: PA5-PA2
// Piano Keys: PB2-PB0



#include <stdint.h>
#include "tm4c123gh6pm.h"
// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data


//Code from online textbook

// **************DAC_Init*********************
// Initialize 4-bit DAC, called once 
// Input: none
// Output: none
void DAC_Init(void){
	
	unsigned long volatile delay;
  SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB; // activate port B
  delay = SYSCTL_RCGC2_R;    						// allow time to finish activating
  GPIO_PORTB_AMSEL_R &= ~0x0F;      		// no analog
  GPIO_PORTB_PCTL_R &= ~0x0000000F; 		// regular GPIO function
  GPIO_PORTB_DIR_R |= 0x0F;      				// make PB3-0 outputs
  GPIO_PORTB_AFSEL_R &= ~0x0F;   				// disable alt funct on PB3-0
  GPIO_PORTB_DEN_R |= 0x0F;     				// enable digital I/O on PB3-0
	GPIO_PORTB_DR8R_R |= 0x0F;
	
}

// **************DAC_Out*********************
// output to DAC
// Input: 4-bit data, 0 to 15 				//gets input for wave? 
// Output: none
void DAC_Out(unsigned char data){
	
	//data = data;                   // shift left for PA 5-2
	GPIO_PORTB_DATA_R = data;
							
}
