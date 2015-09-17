/*******************************************************************************
*   $FILE:  touch_config.h
*   Atmel Corporation:  http://www.atmel.com \n
*   Support: http://www.atmel.com/design-support/
******************************************************************************/

/**
 *
 * Copyright (c) 2010-2015 Atmel Corporation. All rights reserved.
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

#ifndef _TOUCH_CONFIG_H_
#define _TOUCH_CONFIG_H_


/*
* Number of Channels(dependent on the library used).Please refer to the user guide
* more information on selecting the number of channels.
*
* Possible values: 4, 8, 12, 16.
*/
#ifndef QT_NUM_CHANNELS
    #define QT_NUM_CHANNELS 4
#endif

/*
* Delay cycles that determine the capacitance charge pulse width.
*
* Possible values: 1 to 255
*/
#ifndef QT_DELAY_CYCLES
    #define QT_DELAY_CYCLES 1
#endif

/*
* Enabling the _ROTOR_SLIDER_ constant will link the library need for using rotors
* and sliders.
*
* Possible values: comment/uncomment the define
*/
#ifndef _ROTOR_SLIDER_
    //#define _ROTOR_SLIDER_
#endif

/*
* Define the ports to be used for SNS1 and SNSK1 pins. SNS1 and SNSK1 port pins
* can be available on the same port or on different ports
*
* Possible values: refer to the device data sheet and QTouch libraries user guide.
*/
#ifndef SNS1
    #define SNS1            F
#endif
#ifndef SNSK1
    #define SNSK1           F
#endif

/*
* If the same port is being used for SNS1 and SNSK1 then the _SNS1_SNSK1_SAME_PORT_
* constant has to be enabled
*
* Possible values: comment/uncomment the define
*/
#ifndef _SNS1_SNSK1_SAME_PORT_
    //#define _SNS1_SNSK1_SAME_PORT_
#endif

/*
* Define the ports to be used for SNS2 and SNSK2 pins. SNS2 and SNSK2 port pins
* can be available on the same port or on different ports
*
* Possible values: refer to the device data sheet and QTouch libraries user guide.
*/
#ifndef SNS2
    #define SNS2            C
#endif
#ifndef SNSK2
    #define SNSK2           B
#endif

/*
* If the same port is being used for SNS2 and SNSK2 then the _SNS2_SNSK2_SAME_PORT_
* constant has to be enabled
*
* Possible values: comment/uncomment the define
*/
#ifndef _SNS2_SNSK2_SAME_PORT_
    //#define _SNS2_SNSK2_SAME_PORT_
#endif

/*
* Enabling _POWER_OPTIMIZATION_ will lead to a 40% reduction in power consumed
* by the library, but by disabling spread spectrum feature. When power optimization
* is enabled the unused pins, within a port used for QTouch, may not be usable for
* interrupt driven applications. This option is available only for ATtiny and ATmega
* devices.
*
* Possible values: 0 or 1 (For ATtiny and ATmega devices)
*                  0 (For ATxmega devices)
*/

#ifndef _POWER_OPTIMIZATION_
    #define _POWER_OPTIMIZATION_ 0
#endif

/*
* Enabling _DEBUG_INTERFACE_ will add QDEBUG code to the existing projects. This
* will enable viewing the output of the QTouch functionality using the QTouch studio.
*
* Possible values: comment/uncomment the define
*/
#ifndef _DEBUG_INTERFACE_
    //#define _DEBUG_INTERFACE_
#endif

/**************************************************************/
/* Please do not change any part of the code below this line. */
/**************************************************************/
#if (QT_DELAY_CYCLES == 0)
#error 'QT_DELAY_CYCLES can only have values in the range 1-255.'
#endif

#if !(defined(__AVR32__) || defined(__ICCAVR32__))
	#if ((QT_NUM_CHANNELS == 8) || (QT_NUM_CHANNELS == 4))
		#ifdef _SNS1_SNSK1_SAME_PORT_
			#define _STATIC_PORT_PIN_CONF_ 1
			#define INTRABURST_1 1
			#if (QT_NUM_CHANNELS == 4)
				#define QTOUCH_SNS_PORT_COUNT 1
				#define INTRABURST_2 0
			#elif (QT_NUM_CHANNELS == 8)
				#define QTOUCH_SNS_PORT_COUNT 2
				#define INTRABURST_2 1
			#endif
		#else
 			#ifndef _STATIC_PORT_PIN_CONF_
                  #define _STATIC_PORT_PIN_CONF_ 0
            #else
                  #define _STATIC_PORT_PIN_CONF_ 1
            #endif
			#define QTOUCH_SNS_PORT_COUNT 1
			#define INTRABURST_1 0
			#define INTRABURST_2 0
		#endif
	#elif ((QT_NUM_CHANNELS == 16) || (QT_NUM_CHANNELS == 12))
		#define _STATIC_PORT_PIN_CONF_ 1
		#define QTOUCH_SNS_PORT_COUNT 2

		#ifdef _SNS1_SNSK1_SAME_PORT_
			#define INTRABURST_1 1
		#else
			#define INTRABURST_1 0
		#endif

		#ifdef _SNS2_SNSK2_SAME_PORT_
			#define INTRABURST_2 1
		#else
			#define INTRABURST_2 0
		#endif

		#if ((INTRABURST_1== 1) && (INTRABURST_2== 1))
			#error 'QT_NUM_CHANNELS value should be 8 in this case.'
		#elif ((INTRABURST_1== 1) || (INTRABURST_2== 1))
			#if (QT_NUM_CHANNELS == 16)
				#error 'QT_NUM_CHANNELS value should be 12 in this case.'
			#endif
		#elif ((INTRABURST_1== 0) && (INTRABURST_2== 0))
			#if (QT_NUM_CHANNELS == 12)
				#error 'QT_NUM_CHANNELS value should be 16 in this case.'
			#endif
		#endif
	#elif (QT_NUM_CHANNELS == 32)
		#define _STATIC_PORT_PIN_CONF_ 1
		#define QTOUCH_SNS_PORT_COUNT 2

		#ifdef _SNS1_SNSK1_SAME_PORT_
			#define INTRABURST_1 1
		#else
			#define INTRABURST_1 0
		#endif

		#ifdef _SNS2_SNSK2_SAME_PORT_
			#define INTRABURST_2 1
		#else
			#define INTRABURST_2 0
		#endif
	#else
	#error 'QT_NUM_CHANNELS specified is not supported.'
	#endif
#else
	#ifdef _SNS1_SNSK1_SAME_PORT_
		#define _STATIC_PORT_PIN_CONF_ 1
		#define QTOUCH_SNS_PORT_COUNT 1
		#define INTRABURST_1 1
		#define INTRABURST_2 0
	#else
		#define _STATIC_PORT_PIN_CONF_ 0
		#define QTOUCH_SNS_PORT_COUNT 1
		#define INTRABURST_1 0
		#define INTRABURST_2 0
	#endif
#endif

#if (defined(__IAR_SYSTEMS_ASM__) || defined(__ASSEMBLER__) || defined(__ICCAVR32__) || defined(__AVR32__))
    #if (defined(__IAR_SYSTEMS_ASM__) || defined(__ICCAVR32__))
        #if defined(__IAR_SYSTEMS_ASM__)
            #include <ioavr.h>
            #define p_1 r16
            #define p_2 r17
            #define p_3 r18
            #define p_4 r19
            #define p_5 r20
            #define r_v r16
            #define FILE_HEADER NAME qt_asm_iar
            #define FILE_FOOTER END
            #define FILE_SEGMENT RSEG CODE
            #define GLOBAL_FUNCTION PUBLIC

            #define GLOBAL_VAR EQU
        #elif defined(__ICCAVR32__)
            #include <avr32/io.h>
            #define p_1 r12
            #define p_2 r11
            #define p_3 r10
            #define r_v r12
            #define _UC3A_
            #define _QTOUCH_
            //#define FILE_HEADER NAME qt_asm_iar
            //#define FILE_FOOTER END
            //#define FILE_SEGMENT RSEG CODE
            #define FILE_HEADER
            #define FILE_FOOTER
            #define FILE_SEGMENT
            #define GLOBAL_FUNCTION PUBLIC
        #endif
    #elif (defined(__ASSEMBLER__) || defined(__AVR32__))
        #if defined(__AVR32__)
            #include <avr32/io.h>

            #define p_1 r12
            #define p_2 r11
            #define p_3 r10
            #define r_v r12

            #define _QTOUCH_
            #define _UC3A_
            #define _STATIC_PORT_PIN_CONF_ 0
            #define QTOUCH_SNS_PORT_COUNT 1
            #define INTRABURST_1 0
            #define INTRABURST_2 0
        #else
            #define __SFR_OFFSET 0
            #include <avr/io.h>
            #define p_1 r24
            #define p_2 r22
            #define p_3 r20
            #define p_4 r18
            #define p_5 r16
            #define r_v r24

            #define GLOBAL_VAR .EQU
        #endif

        #define FILE_HEADER
        #define FILE_FOOTER
        #define FILE_SEGMENT
        #define GLOBAL_FUNCTION .global
    #else
        #error 'Assembler not supported.'
    #endif

    #define JOIN( x, y ) x ## y
    #define JOIN1( A, B, C ) A ## B ## C
    #define REG( REGISTER, SIDE ) JOIN( REGISTER, SIDE )
    #define CONCAT( A, B, C ) JOIN1( A, B, C )

    #define sreg_save r2

    #define _00011001_	nop
    #define _01101001_	brne _111_
    #define _01101011_	brne _222_
    #define _10001110_	rjmp .
    #if (defined (__AVR32__) || defined (__ICCAVR32__))
        #define _10100011_	_111_: sub r9,1
        #define _10100111_	_222_: sub r9,1
        #define _11100011_	mov r9,((QT_DELAY_CYCLES - 1)/3)
    #else
        #define _10100011_	_111_: dec r19
        #define _10100111_	_222_: dec r19
        #define _11100011_	ldi r19,((QT_DELAY_CYCLES - 1)/3)
    #endif

#else
    #ifndef _QTOUCH_
        #define _QTOUCH_
    #endif
#endif

#endif /*_TOUCH_CONFIG_H_*/
