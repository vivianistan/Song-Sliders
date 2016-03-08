// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Last Modified: 3/6/2015 
// Student names: Carly S., Vivian T. 
// Last modification date: 04/07/15

#include <stdint.h>
#include "tm4c123gh6pm.h"

// ADC initialization function 
// Input: none
// Output: none
void ADC_Init(void){ 
	//PE3 is Ain0
	//case 0:                       //      Ain0 is on PE3
			volatile uint32_t delay; 
	
			SYSCTL_RCGCGPIO_R |= 0x10; 			// turn PE clock on
			delay = SYSCTL_RCGCGPIO_R;      // extra time for clock to stabilize
			delay = SYSCTL_RCGCGPIO_R;
      GPIO_PORTE_DIR_R &= ~0x08;  		// 3.0) make PE3 input
      GPIO_PORTE_AFSEL_R |= 0x08; 		// 4.0) enable alternate function on PE3
      GPIO_PORTE_DEN_R &= ~0x08;  		// 5.0) disable digital I/O on PE3
      GPIO_PORTE_AMSEL_R |= 0x08; // 6.0) enable analog functionality on PE3
			
			SYSCTL_RCGCADC_R |= 0x0001;   		// 7) activate ADC0 
			//  while((SYSCTL_PRADC_R&0x0001) != 0x0001){};   // good code, but not yet implemented in simulator
			delay = SYSCTL_RCGCADC_R;         // extra time for clock to stabilize
			delay = SYSCTL_RCGCADC_R;         // extra time for clock to stabilize
			delay = SYSCTL_RCGCADC_R;         // extra time for clock to stabilize
			delay = SYSCTL_RCGCADC_R;
	
			ADC0_PC_R &= ~0xF;              // 7) clear max sample rate field
			ADC0_PC_R |= 0x1;               //    configure for 125K samples/sec
			ADC0_SSPRI_R = 0x0123;          // 8) Sequencer 3 is highest priority
			ADC0_ACTSS_R &= ~0x0008;        // 9) disable sample sequencer 3
			ADC0_EMUX_R &= ~0xF000;         // 10) seq3 is software trigger
			ADC0_SSMUX3_R &= ~0x000F;       // 11) clear SS3 field
			ADC0_SSMUX3_R += 0;             //    set channel
			ADC0_SSCTL3_R = 0x0006;         // 12) no TS0 D0, yes IE0 END0
			ADC0_IM_R &= ~0x0008;           // 13) disable SS3 interrupts
			ADC0_ACTSS_R |= 0x0008;         // 14) enable sample sequencer 3		
}

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
uint32_t ADC_In(void){  
		uint32_t result;
		ADC0_PSSI_R = 0x0008;            // 1) initiate SS3
		while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
    // if you have an A0-A3 revision number, you need to add an 8 usec wait here
				//how do we do that? 
			result = SYSCTL_RCGCADC_R;         // extra time 
			result = SYSCTL_RCGCADC_R;         // extra time 
			result = SYSCTL_RCGCADC_R;         // extra time 
			result = SYSCTL_RCGCADC_R;
		result = ADC0_SSFIFO3_R&0xFFF;   // 3) read result
		ADC0_ISC_R = 0x0008;             // 4) acknowledge completion
		return result;
		
  //return 0; // remove this, replace with real code
	
	
	
	
}


