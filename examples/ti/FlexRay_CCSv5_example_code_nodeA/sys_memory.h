/** @file sys_memory.h
*   @brief System Memory Header File
*   @date 04.October.2011
*   @version 1.02.000
*   
*   This file contains:
*   - Memory Interface Functions
*   .
*   which are relevant for the System driver.
*/

/* (c) Texas Instruments 2009-2010, All rights reserved. */

#ifndef __SYS_MEMORY_H__
#define __SYS_MEMORY_H__

#include "sys_common.h"

/* USER CODE BEGIN (0) */
/* USER CODE END */


/* System Memory Interface Functions */

/** @fn void _memoryTest_(void)
*   @brief Pbist hardware memory selftest
*/
void _memoryTest_(void);

/** @fn void _memoryInit_(void)
*   @brief Automatic hardware memory initialization
*/
void _memoryInit_(void);


/* EFUSE Selftest Functions */

/** @fn void _efuseSelfTest_(void)
*   @brief Efuse controller Selftest 
*/
void _efuseSelfTest_(void);


/* USER CODE BEGIN (1) */
/* USER CODE END */


#endif
