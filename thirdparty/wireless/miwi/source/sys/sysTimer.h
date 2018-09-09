/**
* \file  sysTimer.h
*
* \brief System timer interface
*
* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
*
* \asf_license_start
*
* \page License
*
* Subject to your compliance with these terms, you may use Microchip
* software and any derivatives exclusively with Microchip products. 
* It is your responsibility to comply with third party license terms applicable 
* to your use of third party software (including open source software) that 
* may accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
* WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, 
* INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, 
* AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE 
* LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL 
* LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE 
* SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE 
* POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY 
* RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, 
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*
* \asf_license_stop
*
*/

#ifndef _SYS_TIMER_H_
#define _SYS_TIMER_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/**
 * \ingroup group_miwi_sys
 * \defgroup group_miwi_sys_timer MiWi System Services - Timer
 * @{
 */

#define SYS_TIMER_INTERVAL      10ul /* ms */
#define MS 1000

#define ONE_SECOND              ((uint32_t)1000000)

#define ONE_MICRO_SECOND    (ONE_SECOND/1000000)
#define ONE_MILI_SECOND     (ONE_SECOND/1000)
#define HUNDRED_MILI_SECOND (ONE_SECOND/10)
#define FORTY_MILI_SECOND   (ONE_SECOND/25)
#define FIFTY_MILI_SECOND   (ONE_SECOND/20)
#define TWENTY_MILI_SECOND  (ONE_SECOND/50)
#define TEN_MILI_SECOND     (ONE_SECOND/100)
#define FIVE_MILI_SECOND    (ONE_SECOND/200)
#define TWO_MILI_SECOND     (ONE_SECOND/500)
#define ONE_MINUTE          (ONE_SECOND*60)
#define ONE_HOUR            (ONE_MINUTE*60)



#if defined(PROTOCOL_STAR)

#define SHARE_PEER_DEVICE_INFO_TIMEOUT      ONE_SECOND*15
#define LINK_STATUS_TIMEOUT                 ONE_SECOND*15
#define SW_ACK_TIMEOUT                      HUNDRED_MILI_SECOND*2
// every 10 minutes the stack will evaluate the inactive end nodes
#define FIND_INACTIVE_DEVICE_TIMEOUT        ONE_MINUTE*10
#define END_DEVICES_DISPLAY_TIMEOUT         ONE_SECOND*15

#endif

/*- Types ------------------------------------------------------------------*/
typedef enum SYS_TimerMode_t {
	SYS_TIMER_INTERVAL_MODE,
	SYS_TIMER_PERIODIC_MODE,
} SYS_TimerMode_t;

typedef struct SYS_Timer_t {
	/* Internal data */
	struct SYS_Timer_t *next;
	uint32_t timeout;

	/* Timer parameters */
	uint32_t interval;
	SYS_TimerMode_t mode;
	void (*handler)(struct SYS_Timer_t *timer);
} SYS_Timer_t;

typedef union _MIWI_TICK
{
    uint32_t Val;
    struct _MIWI_TICK_bytes
    {
        uint8_t b0;
        uint8_t b1;
        uint8_t b2;
        uint8_t b3;
    } byte;
    uint8_t v[4];
    struct _MIWI_TICK_words
    {
        uint16_t w0;
        uint16_t w1;
    } word;
} MIWI_TICK;

/*- Prototypes -------------------------------------------------------------*/
void SYS_TimerInit(void);
void SYS_TimerStart(SYS_Timer_t *timer);
void SYS_TimerStop(SYS_Timer_t *timer);
bool SYS_TimerStarted(SYS_Timer_t *timer);
void SYS_TimerTaskHandler(void);
void SYS_TimerAdjust_SleptTime(uint32_t sleeptime);

uint32_t MiWi_TickGet(void);
uint32_t MiWi_TickGetDiff(MIWI_TICK current_tick, MIWI_TICK previous_tick);

/** @} */
#endif /* _SYS_TIMER_H_ */
