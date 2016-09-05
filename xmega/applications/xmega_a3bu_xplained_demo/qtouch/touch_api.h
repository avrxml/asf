/*******************************************************************************
*   Atmel Corporation:  http://www.atmel.com
*   Support: http://www.atmel.com/design-support/
******************************************************************************/
/**
 *
 * Copyright (c) 2010-2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef TOUCH_API_H
#define TOUCH_API_H

bool check_touch_key_pressed(void);
void touch_init(void);


/*----------------------------------------------------------------------------
                            nested include files
----------------------------------------------------------------------------*/
#if defined (__GNUC__) || defined (__ICCARM__) ||defined (__CC_ARM)
    #include <stdint.h>
#endif

#if !defined (__CC_ARM)
    #include <stdbool.h>
#endif
#include <stdlib.h>
#ifndef MAKE_BUILD
    #ifdef _QMATRIX_
        #include "touch_qm_config.h"
    #elif defined _QTOUCH_
        #if !(defined(__AVR32__) || defined(__ICCAVR32__))
            #include "touch_qt_config.h"
        #endif
    #else
        #error /* please provide any one of the acquisition methods*/
    #endif
#endif

#ifdef __ICCAVR__
    #include <intrinsics.h>
#endif /* (__ICCAVR__) */

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/*----------------------------------------------------------------------------
                        Note: choice of library version
------------------------------------------------------------------------------
QTouch libraries are supplied built with a range of charge times.
Charge time is the duration in which charge is transferred from the AVR onto
the measurement capacitor. Shorter charge times result in faster measurements,
but may be too fast to completely transfer charge onto the measurement
capacitor.

The range of charge times lets the user choose a suitable version for the clock
rate their chip is running at.

To make capacitive measurements, a charge time on the order of 0.25us to 2.5us
is typically required. The following table shows the charge times corresponding
to a range of clock speeds.

Clock speed (MHz) Cycle time (us) Suitable charge times
----------------- --------------- ---------------------
      1              1            1~2 cycles (1us to 2us)
      2              0.5          1~5 cycles (0.5us to 2.5us)
      4              0.25         1~10 cycles (0.25us to 2.5us)
      8              0.125        2~10 cycles (0.25us to 1.25us)
      10             0.1          3~25 cycles (0.3us to 2.5us)
      16             0.0625       4~25 cycles (0.25us to 1.5625s)
      20             0.05         5~50 cycles (0.25us to 2.5us)

----------------------------------------------------------------------------*/

/* *********Error Checking For the User Configuration Options Start********** */
#ifdef _QMATRIX_

/*
* Info stored for each x line.
*
* For each X line, enter the port and pin it is on.
* For instance, if X3 is on PB1, the 4th entry would be
*    FILL_OUT_X_LINE_INFO( 1,1 ), and PORT_X_1 is B
*/
#define FILL_OUT_X_LINE_INFO( port_num,x_bit ) {JOIN(burst_qm_, port_num) ,(uint8_t)( 1u << x_bit ), BURST_MASK_X_PORT_ ## port_num  }

/*
* Info stored for each y line.
*
* For each Y line, enter the pin it is on.
* For instance, if Y2 is on PA5 and PF5, the 3th entry would be
*    FILL_OUT_X_LINE_INFO( 5 )
* NOTE: 1. The PORTs for YA and YB on which Y lines are going to
*          be needs to be defined as PORT_YA and PORT_YB.
*
* Example: PORT_YA=A and PORT_YB=F in the case above.
*/

#define FILL_OUT_Y_LINE_INFO( bit ) { bit, (uint8_t)(1u<<bit) }


#ifndef MAKE_BUILD

/*  Check necessary definitions   */
#if (NUM_X_PORTS==1)
  #if !defined(PORT_X_1) || !defined(PORT_YA) || !defined(PORT_YB) || !defined(QT_DELAY_CYCLES) || !defined(PORT_SMP) || !defined(SMP_BIT)
			#error Need to define PORT_X_1, PORT_YA,PORT_YB,PORT_SMP,SMP_BIT and QT_DELAY_CYCLES before including the header file.
	#endif
#elif(NUM_X_PORTS==2)
  #if !defined(PORT_X_1)||!defined(PORT_X_2) || !defined(PORT_YA) || !defined(PORT_YB) || !defined(QT_DELAY_CYCLES) || !defined(PORT_SMP) || !defined(SMP_BIT)
			#error Need to define PORT_X_1,PORT_X_2, PORT_YA,PORT_YB,PORT_SMP,SMP_BIT and QT_DELAY_CYCLES before including the header file.
	#endif
#elif(NUM_X_PORTS==3)
  #if !defined(PORT_X_1)||!defined(PORT_X_2) ||!defined(PORT_X_3)|| !defined(PORT_YA) || !defined(PORT_YB) || !defined(QT_DELAY_CYCLES) || !defined(PORT_SMP) || !defined(SMP_BIT)
			#error Need to define PORT_X_1,PORT_X_2,PORT_X_3, PORT_YA,PORT_YB,PORT_SMP,SMP_BIT and QT_DELAY_CYCLES before including the header file.
	#endif
#endif
	#ifdef _ROTOR_SLIDER_
			#if (QT_NUM_CHANNELS == 4u)
				#if !(QT_MAX_NUM_ROTORS_SLIDERS == 1u)
					#error QT_MAX_NUM_ROTORS_SLIDERS should be define as 1
				#endif
			#elif (QT_NUM_CHANNELS == 8u)
				#if !(QT_MAX_NUM_ROTORS_SLIDERS == 2u)
					#error QT_MAX_NUM_ROTORS_SLIDERS should be define as 2
				#endif
			#elif (QT_NUM_CHANNELS == 16u)
				#if !((QT_MAX_NUM_ROTORS_SLIDERS == 2u) || (QT_MAX_NUM_ROTORS_SLIDERS == 4u))
					#error QT_MAX_NUM_ROTORS_SLIDERS should be define as 2 or 4 based on library selected
				#endif
			#elif (QT_NUM_CHANNELS == 32u)
				#if !(QT_MAX_NUM_ROTORS_SLIDERS == 4u)
					#error QT_MAX_NUM_ROTORS_SLIDERS should be define as 4 based on library selected
				#endif
			#elif (QT_NUM_CHANNELS == 64u)
				#if !((QT_MAX_NUM_ROTORS_SLIDERS == 4u) || (QT_MAX_NUM_ROTORS_SLIDERS == 8u))
					#error QT_MAX_NUM_ROTORS_SLIDERS should be define as 4 or 8 based on library selected
				#endif
			#endif
	#else
		#if !(QT_MAX_NUM_ROTORS_SLIDERS == 0u)
			#error QT_MAX_NUM_ROTORS_SLIDERS should be define as 0 Or _ROTOR_SLIDER_ Macro Needs to be define
		#endif
	#endif

#endif /*MAKE_BUILD*/

#endif/*_QMATRIX_*/
/* ******************************Error Checking For the User Configuration Options End ***************** */

/*  Total ticks per msec.  */
/*
*  TICKS_PER_MS = (CLK_FREQ/TIMER_PRESCALER)*(1/1000)
*
*/
#define TICKS_PER_MS                500u

/* Initialization values for the Qtouch library parameters. */
/*
* Sensor detect integration (DI) limit.
* Default value: 4.
*/
#define DEF_QT_DI                      4

/*
* Sensor negative drift rate.
*
* Units: 200ms
* Default value: 20 (4 seconds per LSB).
*/
#define DEF_QT_NEG_DRIFT_RATE          20      /* 4s per LSB */

/*
* Sensor positive drift rate.
*
* Units: 200ms
* Default value: 5 (1 second per LSB).
*/
#define DEF_QT_POS_DRIFT_RATE          5       /* 1s per LSB */

/*
* Sensor drift hold time.
*
* Units: 200ms
* Default value: 20 (hold off drifting for 4 seconds after leaving detect).
*/
#define DEF_QT_DRIFT_HOLD_TIME         20      /* 4s */

/*
* Sensor maximum on duration.
*
* Units: 200ms (e.g., 150 = recalibrate after 30s). 0 = no recalibration.
* Default value: 0 (recalibration disabled).
*/
#define DEF_QT_MAX_ON_DURATION         0       /* disabled */

/*
* Sensor recalibration threshold.
*
* Default: RECAL_50 (recalibration threshold = 50% of detection threshold).
*/
#define DEF_QT_RECAL_THRESHOLD         RECAL_50 /* recal threshold = 50% of detect */

/*
* Positive recalibration delay.
*
* Default: 3
*/
#define DEF_QT_POS_RECAL_DELAY         3

/*--------------------------------------------------------------------------
----------------------------------------------------------------------------
----------------------------------------------------------------------------
                    WARNING! Do not edit below this line.
----------------------------------------------------------------------------
----------------------------------------------------------------------------
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
                                    macros
----------------------------------------------------------------------------*/
/* utility macro for expanding port registers   */
#define JOIN( x, y ) x ## y
#define JOIN1( A, B, C ) A ## B ## C

/* Macro to build register writes for controlling ports. The intermediate
*  JOIN macro is required for correct expansion of the args.   */
#define REG( REGISTER, SIDE ) JOIN( REGISTER, SIDE )
#define CONCAT( A, B, C ) JOIN1( A, B, C )

/* The number of bytes required to report the maximum possible number of
*  sensors. */
#define QT_NUM_SENSOR_STATE_BYTES ( ( QT_NUM_CHANNELS + 7u ) / 8u )

/* Status flags used with debug  */
/* indicating no activity */
#define QTLIB_NO_ACTIVITY             (uint16_t) 0x0000
/* indicating that at least one sensor is in detect   */
#define QTLIB_IN_DETECT               (uint16_t) 0x0001
/* indicating that at least one sensor has changed ON/OFF state since last call to measure_sensors */
#define QTLIB_STATUS_CHANGE           (uint16_t) 0x0002
/* indicating that at least one rotor slider has changed position since last call to measure_sensors  */
#define QTLIB_ROTOR_SLIDER_POS_CHANGE (uint16_t) 0x0004
/* indicating that at least one reference value has changed since last call to measure_sensors  */
#define QTLIB_CHANNEL_REF_CHANGE      (uint16_t) 0x0008
/* indicating that reburst is needed to resolve in FILTERIN or FILTEROUT or RECALIBRATE state.  */
#define QTLIB_BURST_AGAIN             (uint16_t) 0x0100
/* indicating that reburst is needed to resolve in CALIBRATION  */
#define QTLIB_RESOLVE_CAL             (uint16_t) 0x0200
/* indicating that reburst is needed to resolve in FILTERING  */
#define QTLIB_RESOLVE_FILTERIN        (uint16_t) 0x0400
/* indicating that reburst is needed to resolve in RECAL */
#define QTLIB_RESOLVE_DI              (uint16_t) 0x0800
/* indicating that reburst is needed to resolve in RECAL */
#define QTLIB_RESOLVE_POS_RECAL       (uint16_t) 0x1000

/*----------------------------------------------------------------------------
                                type definitions
----------------------------------------------------------------------------*/
#if defined (__ICCAVR__) || defined (__ICCAVR32__)
# ifndef __INT8_T_TYPE__
    /*   An signed 8-bit value.  */
    typedef signed char int8_t;
    /*   An unsigned 8-bit value.   */
    typedef unsigned char uint8_t;
# endif
# ifndef __INT16_T_TYPE__
    /*   An unsigned 16-bit value.  */
    typedef unsigned short uint16_t;
    /*   A signed 16-bit value.  */
    typedef short int16_t;
# endif
# ifndef __INT32_T_TYPE__
    /*   An unsigned 32-bit value.  */
    typedef unsigned long uint32_t;
# endif
#endif

/*	This part of the code below is used for library selection. This code will not be documented.	*/
/****************************************************************************************************/
#ifdef _QTOUCH_


	#ifdef QTOUCH_STUDIO_MASKS
	  	#define CALCULATE_MASKS  NULL
	#else
		#define CALCULATE_MASKS  calc_masks
	#endif

    #if defined (__AVR32__) || defined (__ICCAVR32__) || defined (_TOUCH_ARM_)

        #if (QT_NUM_CHANNELS <= 8u && QT_NUM_CHANNELS != 0u)
            #define TOUCH_DATA_T uint8_t
        #elif (QT_NUM_CHANNELS > 8u && QT_NUM_CHANNELS <= 16u)
            #define TOUCH_DATA_T uint16_t
        #elif (QT_NUM_CHANNELS > 16u && QT_NUM_CHANNELS <= 32u)
            #define TOUCH_DATA_T uint32_t
        #else
            #error 'The number of channels specified is not supported.'
        #endif
    #else
            #define TOUCH_DATA_T uint8_t
    #endif

    #ifndef MAKE_BUILD
        #if !(defined(__AVR32__) || defined(__ICCAVR32__)) || defined(_TOUCH_ARM_)
            #if (QT_NUM_CHANNELS == 16u)
                #ifndef _STATIC_PORT_PIN_CONF_
                    #define _STATIC_PORT_PIN_CONF_
                    #define QTOUCH_SNS_PORT_COUNT 2u
                #endif
            #else
                #ifdef INTRABURST_1
                    #ifndef _STATIC_PORT_PIN_CONF_
                        #define _STATIC_PORT_PIN_CONF_
                        #define QTOUCH_SNS_PORT_COUNT 1u
                    #endif
                #endif
                #ifndef SNS1
                    #define SNS1 SNS
                #endif
                #ifndef SNSK1
                    #define SNSK1 SNSK
                #endif
            #endif
        #else
            #ifdef INTRABURST_1
                #ifndef _STATIC_PORT_PIN_CONF_
                    #define _STATIC_PORT_PIN_CONF_
                    #define QTOUCH_SNS_PORT_COUNT 1u
                #endif
                #define SNS1 SNS
                #define SNSK1 SNSK
            #endif
        #endif
        #ifdef _ROTOR_SLIDER_
            #if !defined(QT_MAX_NUM_ROTORS_SLIDERS)
                #if (QT_NUM_CHANNELS == 4)
                    #define QT_MAX_NUM_ROTORS_SLIDERS 1
                #elif (QT_NUM_CHANNELS == 8)
                    #define QT_MAX_NUM_ROTORS_SLIDERS 2
                #elif (QT_NUM_CHANNELS == 12)
                    #define QT_MAX_NUM_ROTORS_SLIDERS 3
                #elif (QT_NUM_CHANNELS == 16)
                    #define QT_MAX_NUM_ROTORS_SLIDERS 4
                #elif (QT_NUM_CHANNELS == 32)
                    #define QT_MAX_NUM_ROTORS_SLIDERS 8
                #endif
            #endif
        #else
            #define QT_MAX_NUM_ROTORS_SLIDERS 0
        #endif
    #endif

    #if !(defined(__AVR32__) || defined(__ICCAVR32__) || defined(_TOUCH_ARM_))
        /*   Macros for string concatenation. */
        #define JOIN3( a, b, c, d, e ) burst_ ## a ## _ ## b ## _ ## c ## d ## _ ## e
        #define BURST_JOIN( static_flag, num_ports, intra1, intra2, pow_opt ) JOIN3( static_flag, num_ports, intra1, intra2, pow_opt )

        /*   Build pointer to correct burst function   */
        #define BURST_FUNC_NAME		BURST_JOIN( _STATIC_PORT_PIN_CONF_, QTOUCH_SNS_PORT_COUNT, INTRABURST_1, INTRABURST_2, _POWER_OPTIMIZATION_ )


        /* Burst function prototype. The code itself is stored in the touch library   */
        extern void BURST_FUNC_NAME( TOUCH_DATA_T mask_index );

        /* Function pointer used for bursting  */
        extern void (*burst_proc)( TOUCH_DATA_T mask_index );

    #else
        #ifndef _STATIC_PORT_PIN_CONF_

            /*  Select SNS and SNSK port. Choosing the same port for both will
                configure intra-port bursting. Options are B,C, and D.  */
            #define QT_SNS_PORT  SNS
            #define QT_SNSK_PORT SNSK

            /*   Check necessary definitions   */
            #if !defined(QT_SNS_PORT) || !defined(QT_SNSK_PORT) || !defined(QT_DELAY_CYCLES)
                #error Need to define QT_SNS_PORT, QT_SNSK_PORT, and QT_DELAY_CYCLES before including this header file.
            #endif

            /*   Macros for string concatenation. */
            #define JOIN3( a, b, c ) burst_ ## a ## _ ## b ## c
            #define BURST_JOIN( cycles, snsk, sns ) JOIN3( cycles, snsk, sns )

            /*   Build pointer to correct burst function   */
            #define BURST_FUNC_NAME		BURST_JOIN( QT_DELAY_CYCLES, QT_SNSK_PORT , QT_SNS_PORT)

        #else

            /* static port pin configurability declarations */

            #if (QTOUCH_SNS_PORT_COUNT > 0u)
                /*  first SNS - SNSK port pin pairing.  */
                #define QT_SNS1_PORT SNS1
                #define QT_SNSK1_PORT SNSK1

                #if !defined(QT_SNS1_PORT) || !defined(QT_SNSK1_PORT) || !defined(QT_DELAY_CYCLES) || !defined(QT_NUM_CHANNELS) || \
                !defined(QT_MAX_NUM_ROTORS_SLIDERS)
                    #error Need to define QT_SNS1_PORT, QT_SNSK1_PORT, QT_DELAY_CYCLES, QT_NUM_CHANNELS and QT_MAX_NUM_ROTORS_SLIDERS before including this header file.
                #endif
            #endif

            #if (QTOUCH_SNS_PORT_COUNT > 1u) && !defined(_TOUCH_ARM_)
                #error 'Maximum of only 1 SNS-SNSK port pairings are supported by the UC3 libraries.'
            #elif (QTOUCH_SNS_PORT_COUNT == 0u)
                #error 'Invalid QTOUCH_SNS_PORT_COUNT value.'
            #endif /*  QTOUCH_SNS_PORT_COUNT   */

            #if (QTOUCH_SNS_PORT_COUNT > 1u)
                /* second SNS - SNSK port pin pairing. */
                #define QT_SNS2_PORT SNS2
                #define QT_SNSK2_PORT SNSK2

                #if !defined(QT_SNS2_PORT) || !defined(QT_SNSK2_PORT) || !defined(QT_DELAY_CYCLES) || !defined(QT_NUM_CHANNELS) || \
                !defined(QT_MAX_NUM_ROTORS_SLIDERS)
                    #error Need to define QT_SNS2_PORT, QT_SNSK2_PORT, QT_DELAY_CYCLES, QT_NUM_CHANNELS and QT_MAX_NUM_ROTORS_SLIDERS  before including this header file.
                #endif
            #endif

            #if (QTOUCH_SNS_PORT_COUNT == 1u)

                /*  Macros for string concatenation  */
                #define JOIN3( a, b, c, d, z ) burst_ ## a ## _ ## b ## c
                #define BURST_JOIN( cycles, snsk1, sns1, ch, rs ) JOIN3( cycles, snsk1, sns1, ch, rs )

                /*  Build pointer to correct burst function   */
                #define BURST_FUNC_NAME		BURST_JOIN( QT_DELAY_CYCLES, QT_SNSK1_PORT , QT_SNS1_PORT, QT_NUM_CHANNELS, \
                QT_MAX_NUM_ROTORS_SLIDERS)

           #elif (QTOUCH_SNS_PORT_COUNT == 2u)
#if defined(_TOUCH_ARM_)
                /*  Macros for string concatenation. */
                #define JOIN3( a, b, c, e, f, d, z ) burst_ ## a ## _ ## b ## c ## _ ## e ## f
                #define BURST_JOIN( cycles, snsk1, sns1, snsk2, sns2, ch, rs ) JOIN3( cycles, snsk1, sns1, snsk2, sns2, ch, rs )

                /*  Build pointer to correct burst function   */
                #define BURST_FUNC_NAME		BURST_JOIN( QT_DELAY_CYCLES, QT_SNSK1_PORT , QT_SNS1_PORT, QT_SNSK2_PORT , QT_SNS2_PORT, QT_NUM_CHANNELS, \
                QT_MAX_NUM_ROTORS_SLIDERS)
#endif
            #else
                #error 'Invalid QTOUCH_SNS_PORT_COUNT value.'
            #endif

        #endif

        /* Burst function prototype. The code itself is stored in the touch library   */
        extern void BURST_FUNC_NAME( TOUCH_DATA_T mask_index );

        /* Function pointer used for bursting  */
        extern void (*burst_proc)( TOUCH_DATA_T mask_index);
    #endif

#endif

/****************************************************************************************************/

/********************************************QMATRIX Start*************************************************/

#ifdef _QMATRIX_

    #ifdef __AVR32__
        #define TOUCH_DATA_T uint32_t
    #else
        #define TOUCH_DATA_T uint8_t
    #endif

    /*   Macros for string concatenation. */
    #define BURST_JOIN(A,B) JOIN(A,B)
    #if (NUM_X_PORTS ==1)
        #define BURST_FUNC_1 BURST_JOIN(burst_qm_,PORT_NUM_1)
        extern void BURST_FUNC_1 (uint8_t channel, TOUCH_DATA_T x_mask);
    #elif (NUM_X_PORTS ==2)
        #define BURST_FUNC_1 BURST_JOIN(burst_qm_,PORT_NUM_1)
        extern void BURST_FUNC_1 (uint8_t channel, TOUCH_DATA_T x_mask);
        #define BURST_FUNC_2 BURST_JOIN(burst_qm_,PORT_NUM_2)
        extern void BURST_FUNC_2 (uint8_t channel, TOUCH_DATA_T x_mask);
    #elif (NUM_X_PORTS ==3)
        #define BURST_FUNC_1 BURST_JOIN(burst_qm_,PORT_NUM_1)
        extern void BURST_FUNC_1(uint8_t channel, TOUCH_DATA_T x_mask);
        #define BURST_FUNC_2 BURST_JOIN(burst_qm_,PORT_NUM_2)
        extern void BURST_FUNC_2(uint8_t channel, TOUCH_DATA_T x_mask);
        #define BURST_FUNC_3 BURST_JOIN(burst_qm_,PORT_NUM_3)
        extern void BURST_FUNC_3(uint8_t channel, TOUCH_DATA_T x_mask);
    #endif

    /* Function pointer used for bursting  */
    extern void (*burst_proc_qm)( uint8_t channel, TOUCH_DATA_T drive_mask );

    /* pointer to function to be called for a channel */
    typedef void (*PF_CHANNEL)( uint8_t channel );

    /* pointer to function returning void */
    typedef void (*PFV)( uint8_t channel, uint8_t line_mask );

    /* info stored for each x line */
    typedef struct tag_x_line_info_t
    {
        /* ptr to function to burst on this line */
        PFV p_burst_function;

        /* bit mask for driving/clamping this x line */
        uint8_t mask;

        /* index into array of burst masks */
        uint8_t burst_mask_array_index;

    } x_line_info_t;

    /* info stored for each y line */
    typedef struct tag_y_line_info_t
    {
        /* which bit this y line is on */
        uint8_t bit;
        /* mask for y line*/
        uint8_t mask;

    } y_line_info_t;

    /* enumeration of recognised burst masks */
    typedef enum tag_burst_mask_t
    {
        BURST_MASK_Y, 	/*mask for y lines*/
        BURST_MASK_X_PORT_1, /*mask for x lines on A*/
        BURST_MASK_X_PORT_2, /*mask for x lines on A*/
        BURST_MASK_X_PORT_3, /*mask for x lines on A*/
        NUM_BURST_MASKS /*Total number of masks*/

    } burst_mask_t;

    /*
    * The burst length on each QMatrix channel.
    *
    * Units: pulses
    * Default value: 64 pulses
    */
    extern uint8_t qt_burst_lengths[QT_NUM_CHANNELS];

#endif /* _QMATRIX_ */

/********************************************QMATRIX End **************************************************/

/* An unsigned 8-bit number setting a sensor detection threshold. */
typedef uint8_t threshold_t;

/* Options for the sensor types available.   */
typedef enum tag_sensor_type_t
{
    SENSOR_TYPE_UNASSIGNED,
    SENSOR_TYPE_KEY,
    SENSOR_TYPE_ROTOR,
    SENSOR_TYPE_SLIDER
} sensor_type_t;

/* Which AKS group, if any, a sensor is in.
*
*  NO_AKS_GROUP = sensor is not in an AKS group, and cannot be suppressed.
*  AKS_GROUP_x  = sensor is in AKS group x.  */
typedef enum tag_aks_group_t
{
    NO_AKS_GROUP,
    AKS_GROUP_1,
    AKS_GROUP_2,
    AKS_GROUP_3,
    AKS_GROUP_4,
    AKS_GROUP_5,
    AKS_GROUP_6,
    AKS_GROUP_7
} aks_group_t;

/* The channel(s) in a sensor.   */
typedef enum tag_channel_t
{
#if (QT_NUM_CHANNELS > 0u)
    CHANNEL_0,
    CHANNEL_1,
    CHANNEL_2,
    CHANNEL_3
#endif
#if (QT_NUM_CHANNELS > 4u)
   ,CHANNEL_4,
    CHANNEL_5,
    CHANNEL_6,
    CHANNEL_7
#endif
#if (QT_NUM_CHANNELS > 8u)
   ,CHANNEL_8,
    CHANNEL_9,
    CHANNEL_10,
    CHANNEL_11
#endif
#if (QT_NUM_CHANNELS > 12u)
   ,CHANNEL_12,
    CHANNEL_13,
    CHANNEL_14,
    CHANNEL_15
#endif
#if (QT_NUM_CHANNELS > 16u)
   ,CHANNEL_16,
    CHANNEL_17,
    CHANNEL_18,
    CHANNEL_19,
    CHANNEL_20,
    CHANNEL_21,
    CHANNEL_22,
    CHANNEL_23
#endif
#if (QT_NUM_CHANNELS > 24u)
   ,CHANNEL_24,
    CHANNEL_25,
    CHANNEL_26,
    CHANNEL_27,
    CHANNEL_28,
    CHANNEL_29,
    CHANNEL_30,
    CHANNEL_31
#endif
#if (QT_NUM_CHANNELS > 32u)
   ,CHANNEL_32,
    CHANNEL_33,
    CHANNEL_34,
    CHANNEL_35,
    CHANNEL_36,
    CHANNEL_37,
    CHANNEL_38,
    CHANNEL_39,
    CHANNEL_40,
    CHANNEL_41,
    CHANNEL_42,
    CHANNEL_43,
    CHANNEL_44,
    CHANNEL_45,
    CHANNEL_46,
    CHANNEL_47,
    CHANNEL_48,
    CHANNEL_49,
    CHANNEL_50,
    CHANNEL_51,
    CHANNEL_52,
    CHANNEL_53,
    CHANNEL_54,
    CHANNEL_55
#endif
#if (QT_NUM_CHANNELS > 56u)
   ,CHANNEL_56,
    CHANNEL_57,
    CHANNEL_58,
    CHANNEL_59,
    CHANNEL_60,
    CHANNEL_61,
    CHANNEL_62,
    CHANNEL_63
#endif
} channel_t;

/* A sensor detection hysteresis value.  This is expressed as a percentage of
*  the sensor detection threshold.
*
*  HYST_x = hysteresis value is x% of detection threshold value (rounded down).
*  NB: a minimum value of 2 is used.
*
*  Example: if detection threshold = 20, then:
*     HYST_50   = 10 (50% of 20)
*     HYST_25   = 5  (25% of 20)
*     HYST_12_5 = 2  (12.5% of 20)
*     HYST_6_25 = 2  (6.25% of 20 = 1, but value is hardlimited to 2)   */
typedef enum tag_hysteresis_t
{
    HYST_50,
    HYST_25,
    HYST_12_5,
    HYST_6_25
} hysteresis_t;

/* For rotors and sliders, the resolution of the reported angle or position.
*
*  RES_x_BIT = rotor/slider reports x-bit values.
*
*  Example: if slider resolution is RES_7_BIT, then reported positions are in
*  the range 0..127. */
typedef enum tag_resolution_t
{
    RES_1_BIT,
    RES_2_BIT,
    RES_3_BIT,
    RES_4_BIT,
    RES_5_BIT,
    RES_6_BIT,
    RES_7_BIT,
    RES_8_BIT
} resolution_t;

/* A sensor recalibration threshold.  This is expressed as a percentage of the
*  sensor detection threshold.
*
*  RECAL_x = recalibration threshold is x% of detection threshold value
*           (rounded down).
*  NB:   a minimum value of 4 is used.
*
*  Example: if detection threshold = 40, then:
*     RECAL_100  = 40 (100% of 40)
*     RECAL_50   = 20 (50% of 40)
*     RECAL_25   = 10 (25% of 40)
*     RECAL_12_5 = 5  (12.5% of 40)
*  RECAL_6_25 = 4  (6.25% of 40 = 2, but value is hardlimited to 4)  */
typedef enum tag_recal_threshold_t
{
    RECAL_100,
    RECAL_50,
    RECAL_25,
    RECAL_12_5,
    RECAL_6_25
} recal_threshold_t;

/* The state of the library sensors. */
typedef struct tag_qt_touch_status_t
{
    /* The state (on/off) of the library sensors.
    *  Bit 'n' = state of sensor 'n'.
    *  Bit 0 = sensor_states[0] bit 0
    *  Bit 1 = sensor_states[0] bit 1
    *  ...
    *  Bit 7 = sensor_states[0] bit 7
    *  Bit 8 = sensor_states[1] bit 0
    *  Bit 9 = sensor_states[1] bit 1
    *  ... etc
    *  0 = not in detect, 1 = in detect.*/
    uint8_t sensor_states[QT_NUM_SENSOR_STATE_BYTES];
    /* Rotor angles or slider positions.
    *  These values are valid when sensor_states shows that the corresponding
    *  rotor or slider sensor is in detect.*/
    uint8_t rotor_slider_values[QT_MAX_NUM_ROTORS_SLIDERS];
} qt_touch_status_t;


/* post processing configuration data that is received from the debug interface. */
typedef struct tag_qt_touch_lib_config_data_t
{
    /*   Sensor recalibration threshold.  */
    recal_threshold_t qt_recal_threshold;
    /*   Sensor detect integration (DI) limit.  */
    uint8_t qt_di;
    /*   Sensor drift hold time. */
    uint8_t qt_drift_hold_time;
    /*   Sensor maximum on duration.   */
    uint8_t qt_max_on_duration;
    /*   Sensor negative drift rate.   */
    uint8_t qt_neg_drift_rate;
    /*   Sensor positive drift rate.   */
    uint8_t qt_pos_drift_rate;
    /*   Positive recalibration delay. */
    uint8_t qt_pos_recal_delay;
} qt_touch_lib_config_data_t;

/* measured touch data that is sent for the debugging and fine tunning. */
typedef struct tag_qt_touch_lib_measure_data_t
{
    /*   measured signal on each channel  */
    uint16_t channel_signals[QT_NUM_CHANNELS];
    /*   reference signal for each channel   */
    uint16_t channel_references[QT_NUM_CHANNELS];
    /*   state of sensors  */
    qt_touch_status_t qt_touch_status;
} qt_touch_lib_measure_data_t;

/* This will be used to get the library information from the library */
typedef struct qt_lib_sig_info_t
{
    uint16_t lib_sig_lword;
    uint16_t lib_sig_hword;
    uint16_t library_version;
}qt_lib_siginfo_t;


typedef struct tag_sensor_t
{
    /* sensor state (calibrating, on, ...) */
    uint8_t state;

    /* general purpose ctr: used for calibration, drifting, etc */
    uint8_t general_counter;

    /* DI ctr */
    uint8_t ndil_counter;

    /* sensor detection threshold */
    uint8_t threshold;

    /*
    * sensor type, AKS group, positive recal flag, and hysteresis value
    * bits 7..6: sensor type: 00: key
    *                         01: rotor
    *                         10: slider
    *                         11: reserved
    * bits 5..3: AKS group (0..7): 0 = no AKS group
    * bit 2    : positive recal flag
    * bits 1..0: hysteresis
    */
    uint8_t type_aks_pos_hyst;

    /*
    * sensor from and to channels
    * keys: from channel = to channel
    * rotors/sliders run between the from and to channels
    * NB: only need to_channel for keys in rotors/sliders build
    */
    uint8_t from_channel;

#ifdef _ROTOR_SLIDER_
    uint8_t to_channel;

    /* index into array of rotor/slider values */
    uint8_t index;
#endif /* _ROTOR_SLIDER_ */

} sensor_t;

/*----------------------------------------------------------------------------
                                extern variables
----------------------------------------------------------------------------*/
/* This configuration data structure parameters if needs to be changed will be
   changed in the qt_set_parameters function */
extern qt_touch_lib_config_data_t qt_config_data;
/* measurement data */
extern qt_touch_lib_measure_data_t qt_measure_data;
/* Get sensor delta values */
extern int16_t qt_get_sensor_delta( uint8_t sensor);
/* variable holding the status flags */
extern uint16_t lib_status_flags;
/* the library sensors */
extern sensor_t sensors[QT_NUM_CHANNELS];
/* timer period in msec */
extern uint16_t qt_measurement_period_msec;


/*This function is called after the library has made capacitive measurements,
* but before it has processed them. The user can use this hook to apply filter
* functions to the measured signal values. */
extern void (*qt_filter_callback)( void );

/*============================================================================
Name     :  qt_get_library_sig
------------------------------------------------------------------------------
Purpose  :  Get the library signature
Input    :  qt_lib_siginfo_t * lib_sig_ptr=Pointer to the library signature structure
Output   :  n/a
Notes    :  This API would provide the information of the
            library and fill into the structure
============================================================================*/

extern void  qt_get_library_sig(qt_lib_siginfo_t * lib_sig_ptr);

/*============================================================================
Name     :  qt_enable_key
------------------------------------------------------------------------------
Purpose  :  enable a key sensor
Input    :  channel = which touch channel the key sensor uses
            aks_group = which AKS group (if any) the sensor is in
            detect_threshold = the sensor detection threshold
            detect_hysteresis = the sensor detection hysteresis value
Output   :  n/a
Notes    :  The sensor number corresponding to the key depends on the order in
            which sensors are enabled. The first sensor enabled is sensor 0, the
            second is sensor 1, and so on.

            The current state of the key (on or off) can be checked in
            qt_touch_status.sensor_states.
============================================================================*/
extern void qt_enable_key(    channel_t channel,
                     aks_group_t aks_group,
                     threshold_t detect_threshold,
                    hysteresis_t detect_hysteresis );

/*============================================================================
Name     :  qt_init_sensing
------------------------------------------------------------------------------
Purpose  :  initialise touch sensing
Input    :  n/a
Output   :  n/a
Notes    :  Any sensors required must be enabled (using qt_enable_xxx()) before
            calling this function.

            This function calculates internal library variables and configures
            the touch channels, and must be called before qt_measure_sensors().
============================================================================*/
#if defined( _QTOUCH_ )

    #define qt_init_sensing( ) qt_init_sensing_with_burst( BURST_FUNC_NAME ,CALCULATE_MASKS)
    extern void qt_init_sensing_with_burst(void (*burst_function)(TOUCH_DATA_T mask_index), void (*calculate_masks)(void));
	extern void calc_masks(void);
#elif defined( _QMATRIX_ )

    #define qt_init_sensing( ) qt_init_sensing_with_burst(  )
    extern void qt_init_sensing_with_burst(  );

#endif


/*============================================================================
Name     :  qt_measure_sensors
------------------------------------------------------------------------------
Purpose  :  measure touch sensors
Input    :  current_time_ms = the current time, in ms
Output   :  outputs the library status bits
Notes    :  This function performs a capacitive measurement on all enabled
            sensors. The measured signals for each sensor are then processed to
            check for user touches, releases, changes in rotor angle, changes in
            slider position, etc.

            The current state of all enabled sensors is reported in the
            qt_touch_status struct.

            Before calling this function, one or more sensors must have been
            enabled (using qt_enable_xxx()), and qt_init_sensing() must
            have been called.
============================================================================*/
extern uint16_t qt_measure_sensors( uint16_t current_time_ms );


/*============================================================================
Name     :  qt_calibrate_sensing
------------------------------------------------------------------------------
Purpose  :  calibrate touch sensing
Input    :  n/a
Output   :  n/a
Notes    :  This function forces a calibration of all enabled sensors.

            This may be useful if, for instance, it is desired to globally
            recalibrate all sensors on a change in application operating mode.
============================================================================*/
extern void qt_calibrate_sensing( void );


/*============================================================================
Name     :  qt_reset_sensing
------------------------------------------------------------------------------
Purpose  :  reset touch sensing
Input    :  n/a
Output   :  n/a
Notes    :  This function disables all enabled sensors, and returns all library
            variables (e.g., qt_di) to their default values.

            This may be useful if it is desired to dynamically reconfigure
            sensing. After calling this function, any required sensors must be
            re-enabled, and qt_init_sensing() must be called before
            qt_measure_channels() is called again.
============================================================================*/
extern void qt_reset_sensing( void );

/*============================================================================
Name     :  qt_get_sensor_delta
------------------------------------------------------------------------------
Purpose  :  get the current delta on the specified sensor
Input    :  sensor = sensor number for which to get the delta
Output   :  returns the delta on the specified sensor
Notes    :  This function is supplied as a functional replacement for the
            sensor_deltas[] array provided in earlier library versions.
============================================================================*/
extern int16_t qt_get_sensor_delta( uint8_t sensor );

#ifdef _ROTOR_SLIDER_

/*============================================================================
Name     :  qt_enable_rotor
------------------------------------------------------------------------------
Purpose  :  enable a rotor sensor
Input    :  from_channel = the first channel in the rotor sensor
            to_channel = the last channel in the rotor sensor
            aks_group = which AKS group (if any) the sensor is in
            detect_threshold = the sensor detection threshold
            detect_hysteresis = the sensor detection hysteresis value
            angle_resolution = the resolution of the reported angle value
            angle_hysteresis = the hysteresis of the reported angle value
Output   :  n/a
Notes    :  The sensor number corresponding to the rotor depends on the order in
            which sensors are enabled. The first sensor enabled is sensor 0, the
            second is sensor 1, and so on.

            The current state of the rotor (on or off) can be checked in
            qt_touch_status.sensor_states.

            The rotor value is in qt_touch_status.rotor_slider_values[]. Which
            array element is used depends on the order in which sensors are
            enabled. The first rotor or slider enabled will use
            rotor_slider_values[0], the second will use rotor_slider_values[1],
            and so on.

            The reported rotor value is valid when the rotor is on.
============================================================================*/
extern void qt_enable_rotor(    channel_t from_channel,
                         channel_t to_channel,
                       aks_group_t aks_group,
                       threshold_t detect_threshold,
                      hysteresis_t detect_hysteresis,
                      resolution_t angle_resolution,
                           uint8_t angle_hysteresis );

/*============================================================================
Name     :  qt_enable_slider
------------------------------------------------------------------------------
Purpose  :  enable a slider sensor
Input    :  from_channel = the first channel in the slider sensor
            to_channel = the last channel in the slider sensor
            aks_group = which AKS group (if any) the sensor is in
            detect_threshold = the sensor detection threshold
            detect_hysteresis = the sensor detection hysteresis value
            position_resolution = the resolution of the reported position value
            position_hysteresis = the hysteresis of the reported position value
Output   :  n/a
Notes    :  The sensor number corresponding to the slider depends on the order in
            which sensors are enabled. The first sensor enabled is sensor 0, the
            second is sensor 1, and so on.

            The current state of the slider (on or off) can be checked in
            qt_touch_status.sensor_states.

            The slider value is in qt_touch_status.rotor_slider_values[]. Which
            array element is used depends on the order in which sensors are
            enabled. The first rotor or slider enabled will use
            rotor_slider_values[0], the second will use rotor_slider_values[1],
            and so on.

            The reported slider value is valid when the rotor is on.
============================================================================*/
extern void qt_enable_slider(    channel_t from_channel,
                          channel_t to_channel,
                        aks_group_t aks_group,
                        threshold_t detect_threshold,
                       hysteresis_t detect_hysteresis,
                       resolution_t position_resolution,
                            uint8_t position_hysteresis );

#endif

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

#endif /* TOUCH_API_H */

