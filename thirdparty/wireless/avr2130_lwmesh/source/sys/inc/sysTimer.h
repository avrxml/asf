/**
 * \file sysTimer.h
 *
 * \brief System timer interface
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
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
 *
 */

/*
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

#ifndef _SYS_TIMER_H_
#define _SYS_TIMER_H_

/*- Includes ---------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/**
 * \ingroup group_lwmesh_sys
 * \defgroup group_lwmesh_sys_timer LWMesh System Services - Timer
 * @{
 */

 #define SYS_TIMER_INTERVAL      10ul /* ms */
 #define MS 1000

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

/*- Prototypes -------------------------------------------------------------*/
void SYS_TimerInit(void);
void SYS_TimerStart(SYS_Timer_t *timer);
void SYS_TimerStop(SYS_Timer_t *timer);
bool SYS_TimerStarted(SYS_Timer_t *timer);
void SYS_TimerTaskHandler(void);
void SYS_HwExpiry_Cb(void);

/** @} */
#endif /* _SYS_TIMER_H_ */
