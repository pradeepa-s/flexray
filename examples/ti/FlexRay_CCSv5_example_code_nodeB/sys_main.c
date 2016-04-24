/** @file sys_main.c 
*   @brief Application main file
*   @date 04.October.2011
*   @version 1.02.000
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* (c) Texas Instruments 2009-2011, All rights reserved. */

/* USER CODE BEGIN (0) */
/* USER CODE END */

/* Include Files */

#include "sys_common.h"
#include "system.h"

/* USER CODE BEGIN (1) */
#include "Fr.h"
#include "gio.h"
/* USER CODE END */


#include "het.h"

/* External Demo Functions */
extern void run_LED_StartUp(void);


/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
/* USER CODE END */

void main(void)
{
/* USER CODE BEGIN (3) */
	gioInit();
	configure_initialize_node_b(FRAY1);
	Fr_StartCommunication(FRAY1);
	
	while(1)
	{
		transmit_check_node_b(FRAY1);

		/** - Call RUN LED routine that toggles NHET pin */
		run_LED_StartUp();

	}	
	
/* USER CODE END */
}


/* USER CODE BEGIN (4) */
/* USER CODE END */
