/** @file het.h
*   @brief HET Driver Definition File
*   @date 14.September.2009
*   @version 1.01.000
*   
*/

/* (c) Texas Instruments 2009-2010, All rights reserved. */


#ifndef __HET_H__
#define __HET_H__

#include "gio.h"
#include <string.h>

/** @def pwm0
*   @brief Pwm signal 0
*
*   Alias for pwm signal 0
*/
#define pwm0 0U

/** @def pwm1
*   @brief Pwm signal 1
*
*   Alias for pwm signal 1
*/
#define pwm1 1U

/** @def pwm2
*   @brief Pwm signal 2
*
*   Alias for pwm signal 2
*/
#define pwm2 2U

/** @def pwm3
*   @brief Pwm signal 3
*
*   Alias for pwm signal 3
*/
#define pwm3 3U

/** @def pwm4
*   @brief Pwm signal 4
*
*   Alias for pwm signal 4
*/
#define pwm4 4U

/** @def pwm5
*   @brief Pwm signal 5
*
*   Alias for pwm signal 5
*/
#define pwm5 5U

/** @def pwm6
*   @brief Pwm signal 6
*
*   Alias for pwm signal 6
*/
#define pwm6 6U

/** @def pwm7
*   @brief Pwm signal 7
*
*   Alias for pwm signal 7
*/
#define pwm7 7U


/** @def edge0
*   @brief Edge signal 0
*
*   Alias for edge signal 0
*/
#define edge0 0U

/** @def edge1
*   @brief Edge signal 1
*
*   Alias for edge signal 1
*/
#define edge1 1U

/** @def edge2
*   @brief Edge signal 2
*
*   Alias for edge signal 2
*/
#define edge2 2U

/** @def edge3
*   @brief Edge signal 3
*
*   Alias for edge signal 3
*/
#define edge3 3U

/** @def edge4
*   @brief Edge signal 4
*
*   Alias for edge signal 4
*/
#define edge4 4U

/** @def edge5
*   @brief Edge signal 5
*
*   Alias for edge signal 5
*/
#define edge5 5U

/** @def edge6
*   @brief Edge signal 6
*
*   Alias for edge signal 6
*/
#define edge6 6U

/** @def edge7
*   @brief Edge signal 7
*
*   Alias for edge signal 7
*/
#define edge7 7U


/** @def cap0
*   @brief Capture signal 0
*
*   Alias for capture signal 0
*/
#define cap0 0U

/** @def cap1
*   @brief Capture signal 1
*
*   Alias for capture signal 1
*/
#define cap1 1U

/** @def cap2
*   @brief Capture signal 2
*
*   Alias for capture signal 2
*/
#define cap2 2U

/** @def cap3
*   @brief Capture signal 3
*
*   Alias for capture signal 3
*/
#define cap3 3U

/** @def cap4
*   @brief Capture signal 4
*
*   Alias for capture signal 4
*/
#define cap4 4U

/** @def cap5
*   @brief Capture signal 5
*
*   Alias for capture signal 5
*/
#define cap5 5U

/** @def cap6
*   @brief Capture signal 6
*
*   Alias for capture signal 6
*/
#define cap6 6U

/** @def cap7
*   @brief Capture signal 7
*
*   Alias for capture signal 7
*/
#define cap7 7U

/** @def pwmEND_OF_DUTY
*   @brief Pwm end of duty
*
*   Alias for pwm end of duty notification
*/
#define pwmEND_OF_DUTY 2U

/** @def pwmEND_OF_PERIOD
*   @brief Pwm end of period
*
*   Alias for pwm end of period notification
*/
#define pwmEND_OF_PERIOD 4U

/** @def pwmEND_OF_BOTH
*   @brief Pwm end of duty and period
*
*   Alias for pwm end of duty and period notification
*/
#define pwmEND_OF_BOTH 6U

/** @struct hetBase
*   @brief HET Register Definition
*
*   This structure is used to access the HET module egisters.
*/
/** @typedef hetBASE_t
*   @brief HET Register Frame Type Definition
*
*   This type is used to access the HET Registers.
*/
typedef volatile struct hetBase
{
    unsigned GCR;     /**< 0x0000: Global control register              */
    unsigned PFR;     /**< 0x0004: Prescale factor register             */
    unsigned ADDR;    /**< 0x0008: Current address register             */
    unsigned OFF1;    /**< 0x000C: Interrupt offset register 1          */
    unsigned OFF2;    /**< 0x0010: Interrupt offset register 2          */
    unsigned INTENAS; /**< 0x0014: Interrupt enable set register        */
    unsigned INTENAC; /**< 0x0018: Interrupt enable clear register      */
    unsigned EXC1;    /**< 0x001C: Exeption control register 1          */
    unsigned EXC2;    /**< 0x0020: Exeption control register 2          */
    unsigned PRY;     /**< 0x0024: Interrupt priority register          */
    unsigned FLG;     /**< 0x0028: Interrupt flag register              */
    unsigned : 32U;   /**< 0x002C: Reserved                             */
    unsigned : 32U;   /**< 0x0030: Reserved                             */
    unsigned HRSH;    /**< 0x0034: High resoltion share register        */
    unsigned XOR;     /**< 0x0038: XOR share register                   */
    unsigned REQENS;  /**< 0x003C: Request enable set register          */
    unsigned REQENC;  /**< 0x0040: Request enable clear register        */
    unsigned REQDS;   /**< 0x0044: Request destination select register  */
    unsigned : 32U;   /**< 0x0048: Reserved                             */
    unsigned DIR;     /**< 0x004C: Direction register                   */
    unsigned DIN;     /**< 0x0050: Data input register                  */
    unsigned DOUT;    /**< 0x0054: Data output register                 */
    unsigned DSET;    /**< 0x0058: Data output set register             */
    unsigned DCLR;    /**< 0x005C: Data output clear register           */
    unsigned PDR;     /**< 0x0060: Open drain register                  */
    unsigned PULDIS;  /**< 0x0064: Pull disable register                */
    unsigned PSL;     /**< 0x0068: Pull select register                 */
    unsigned : 32U;   /**< 0x006C: Reserved                             */
    unsigned : 32U;   /**< 0x0070: Reserved                             */
    unsigned PCREG;   /**< 0x0074: Parity control register              */
    unsigned PAR;     /**< 0x0078: Parity address register              */
    unsigned PPR;     /**< 0x007C: Parity pin select register           */
    unsigned SFPRLD;  /**< 0x0080: Suppression filter preload register  */
    unsigned SFENA;   /**< 0x0084: Suppression filter enable register   */
    unsigned : 32U;   /**< 0x0088: Reserved                             */
    unsigned LBPSEL;  /**< 0x008C: Loop back pair select register       */
    unsigned LBPDIR;  /**< 0x0090: Loop back pair direction register    */
} hetBASE_t;


/** @def hetREG
*   @brief HET Register Frame Pointer
*
*   This pointer is used by the HET driver to access the het module registers.
*/
#define hetREG ((hetBASE_t *)0xFFF7B800U)


/** @def hetPORT
*   @brief HET GIO Port Register Pointer
*
*   Pointer used by the GIO driver to access I/O PORT of HET
*   (use the GIO drivers to access the port pins).
*/
#define hetPORT ((gioPORT_t *)0xFFF7B84CU)


/** @struct hetInstructionBase
*   @brief HET Instruction Definition
*
*   This structure is used to access the HET RAM.
*/
/** @typedef hetINSTRUCTION_t
*   @brief HET Instruction Type Definition
*
*   This type is used to access a HET Instruction.
*/
typedef volatile struct hetInstructionBase
{
    unsigned Program;
    unsigned Control;
    unsigned Data;
    unsigned : 32U;
} hetINSTRUCTION_t;


/** @struct hetRamBase
*   @brief HET RAM Definition
*
*   This structure is used to access the HET RAM.
*/
/** @typedef hetRAMBASE_t
*   @brief HET RAM Type Definition
*
*   This type is used to access the HET RAM.
*/
typedef volatile struct hetRamBase
{
    hetINSTRUCTION_t Instruction[128U];
} hetRAMBASE_t;

#define hetRAM ((hetRAMBASE_t *)0xFF460000U)


/** @struct hetSignal
*   @brief HET Signal Definition
*
*   This structure is used to define a pwm signal.
*/
/** @typedef hetSIGNAL_t
*   @brief HET Signal Type Definition
*
*   This type is used to access HET Signal Information.
*/
typedef struct hetSignal
{
	unsigned duty;   /**< Duty cycle in % of the period  */
	double   period; /**< Period in us                   */
} hetSIGNAL_t;


/* HET Interface Functions */
void hetInit(void);

/* PWM Interface Functions */
void        pwmStart(unsigned pwm);
void        pwmStop(unsigned pwm);
void        pwmSetDuty(unsigned pwm, unsigned duty);
void        pwmSetSignal(unsigned pwm, hetSIGNAL_t signal);
hetSIGNAL_t pwmGetSignal(unsigned pwm);
void        pwmEnableNotification(unsigned pwm, unsigned notification);
void        pwmDisableNotification(unsigned pwm, unsigned notification);
void        pwmNotification(unsigned pwm, unsigned notification);

/* Edge Interface Functions */
void     edgeResetCounter(unsigned edge);
unsigned edgeGetCounter(unsigned edge);
void     edgeEnableNotification(unsigned edge);
void     edgeDisableNotification(unsigned edge);
void     edgeNotification(unsigned edge);

/* Captured Signal Interface Functions */
hetSIGNAL_t capGetSignal(unsigned cap);

/* Timestamp Interface Functions */
void     hetResetTimestamp(void);
unsigned hetGetTimestamp(void);

#endif
