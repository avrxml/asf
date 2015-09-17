/*
*******************************************************************************
*                     (c) Cybercom Sweden West AB 2009                        *
*******************************************************************************

   File name:     app_cfg.h
   Author:        Thomas Bredhammar, Daniel Persson
   Description:   In this file OS characteristics for the application are set.
   Compiler:      IAR EWAVR32 for AVR32

*******************************************************************************
*/

/*
*******************************************************************************
*                                               STACK SIZES
*******************************************************************************
*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#define  APP_TASK_STK_SIZE          256

/*
******************************************************************************
*                                             TASK PRIORITIES
******************************************************************************
*/

#define  APP_TASK_START_PRIO                28
#define  APP_TASK_PRIO                      29
#define  OS_TASK_TMR_PRIO                   5
