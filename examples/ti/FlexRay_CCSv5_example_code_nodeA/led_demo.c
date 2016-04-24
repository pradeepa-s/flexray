/** @file led_demo.c 
*   @brief led demo implementation File
*   @date 11.August.2009
*   @version 1.00.000
*
*   (c) Texas Instruments 2009, All rights reserved.
*/

#include "het.h"







/** @fn run_LED_StartUp(void)
*   
*   This function is called during the start up, LED's are turned on 
*   in a sequence to indicate the board is powerd up and ready to run
*   the demo software.  
*   
*/
void run_LED_StartUp(void)
{
   	int temp,delay;
   	
	/** - Delay Parameter */
	delay				= 0x200000;

    /** - NHET is configured as Master 
     *  - NHET is turned On */    
   	hetREG->GCR     	= 0x01000001; 
	
	/** - PULL functinality is enabled */
	hetREG->PULDIS	= 0x00000000;
	 
   		/** - Configure NHET pins as output */
   		hetREG->DIR   	= 0xA8178035; 
		/** - Turn all LEDs off */
		hetREG->DOUT   	= 0x08110034;
		
		/** - Set only NHET[0] */ 
		hetREG->DOUT   	= 0x08110035;
		/** - Simple Dealy */
		for(temp=0;temp<delay;temp++);
		
		/** - Turn all LEDs off */
		hetREG->DOUT   	= 0x08110034;
		for(temp=0;temp<delay;temp++);
}
