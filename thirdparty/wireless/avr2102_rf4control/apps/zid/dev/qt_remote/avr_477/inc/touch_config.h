/**
 * \file *********************************************************************
 *
 * \brief QMatrix Touch configuration
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 */

/*
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
#ifndef TOUCH_CONFIG_H
#define TOUCH_CONFIG_H

/*----------------------------------------------------------------------------
 *                      Acquisition method definition
 *
 *
 *----------------------------------------------------------------------------*/
#define _QMATRIX_

/*----------------------------------------------------------------------------
 *              Library configuration notes for QMatrix acquisition.
 *
 *
 *----------------------------------------------------------------------------*/

/**
 * The following rules need to be followed while configuring
 * the QTouch library for QTouch acquisition method.
 * 1. Always setup the following to have valid values
 *			#define QT_NUM_CHANNELS	<value>
 *			#define QT_DELAY_CYCLES	<value>
 *      The value for QT_NUM_CHANNELS should match the library chosen. For
 * example, when using
 *     libavr5g4-64qm-8x-8y-krs-4rs.a library, set QT_NUM_CHANNELS to 64 only.
 * 2. Define the following for X lines used
 *		  #define NUM_X_LINES	<value>
 *                #define NUM_X_PORTS <value>
 *                #define PORT_NUM_1	<value>
 *                #define PORT_NUM_2	<value, if using 2 X ports>
 *                #define PORT_NUM_3	<value, if using 3 X ports>
 *                #define PORT_X_1			<value>
 *		  #define PORT_X_2			<value, if using 2 X
 * ports>
 *		  #define PORT_X_3			<value, if using 3 X
 * ports>
 * 3. Define the following macros for Y lines used
 *               #define NUM_Y_LINES	<value>
 *		 #define PORT_YA				<value>
 *		 #define PORT_YB				<value>
 * 4. If ports for YA and YB are the same, define macro
 *		 #define SHARED_YAYB		1
 * 5 Define these macros for SMP pins used
 *		#define	PORT_SMP		<value>
 *              #define         SMP_PIN			<value>
 * 6. If using a library with rotors and sliders, define the macros below:
 *			#define _ROTOR_SLIDER_
 *			#define QT_MAX_NUM_ROTOR_SLIDER  <value>
 *    The value for QT_MAX_NUM_ROTOR_SLIDER should match the library chosen. For
 * example, when using
 *    libavr5g4-64qm-8x-8y-krs-4rs.a library, set QT_MAX_NUM_ROTOR_SLIDER to 4
 * only.
 * 7. If using a debug interface, define
 *              #define _DEBUG_INTERFACE_
 *
 **/

/*----------------------------------------------------------------------------
 *               QMatrix Configuration Options.
 *
 *
 *----------------------------------------------------------------------------*/

/**
 * Custom user configuration can be in below section.
 */
#if !(defined(_CONF_64QM_8X_8Y_8RS_) || defined(_CONF_64QM_8X_8Y_0RS_))

/**
 * Define the Number of ROTORS/SLIDERS used.
 * Possible values: 0             ( if _ROTOR_SLIDER_ is not defined)
 *                  1, 2, 4 and 8 ( if _ROTOR_SLIDER_ is defined)
 * Depending on the library used.Please refer to the QTouch library user
 * guide.pdf
 * and library selection guide.xls more information on selecting the number of
 * channels.
 */

#define _QMATRIX_
#define _ROTOR_SLIDER_

#define QT_MAX_NUM_ROTORS_SLIDERS 4
#define QT_DELAY_CYCLES 2

/**
 * Number of Channels(dependent on the library used and application
 * requirement).
 * The least possible number more that the application needs.
 * Please refer to the QTouch library user guide.pdf and library selection
 * guide.xls more information on selecting the number of channels.
 *
 * Possible values: 4, 8, 16, 32, 56, 64. in case of QMatrix.
 */
#define QT_NUM_CHANNELS   32

/**
 * Define the Number X lines to be used.
 * Possible values: 4 and 8
 * Depending on the library used.Please refer to the QTouch library user
 * guide.pdf
 * and library selection guide.xls more information on selecting the number of
 * channels.
 *
 */
#define NUM_X_LINES   8

/**
 * Specify the number of ports on which X-lines that are distributed.
 * Note: Support is provided only for a maximum of 3 ports for X.
 *		 ( Maximum possible value for NUM_X_PORTS is 3)
 * Possible values: 1,2,3
 * Also, note that code memory increases with the number of ports
 * used for X lines.
 */
#define NUM_X_PORTS     2
#define PORT_NUM_1      1
#define PORT_NUM_2  2

/**
 * Specify Which ports have X lines on them. These macros are used
 * to conditionally compile in support for ports driving X lines.
 */
#define PORT_X_1        D
#define PORT_X_2    B

/**
 * Define the Number Y lines and port for YA, YB to be used.
 *  Possible values: NUM_Y_LINES is 1, 2, 4 and 8
 * Depending on the library used. Please refer to the QTouch library user
 * guide.pdf
 * and library selection guide.xls for more information on selecting
 * the number of channels, YA and YB ports.
 */
#define NUM_Y_LINES     4
#define PORT_YA         G
#define PORT_YB         F

#define PORT_SMP    D
#define SMP_PIN     5
#endif /* _CUSTOM_CONF_ */

/**
 * Example configuration based on option selected in project options.
 * For user configuration make changes to the custom user configuration section
 * above.
 * Info on the below configuration macros refer above notes.
 */
#if defined(_CONF_64QM_8X_8Y_8RS_)
#define _ROTOR_SLIDER_
#define QT_MAX_NUM_ROTORS_SLIDERS 8

#define QT_NUM_CHANNELS   64

#define NUM_X_LINES     8

#define NUM_X_PORTS     1
#define PORT_NUM_1      1

#define PORT_X_1        D

#define NUM_Y_LINES     8
#define PORT_YA         B
#define PORT_YB         F
#elif defined(_CONF_64QM_8X_8Y_0RS_)
#define QT_MAX_NUM_ROTORS_SLIDERS 0

#define QT_NUM_CHANNELS   32

#define NUM_X_LINES     8

#define NUM_X_PORTS     1
#define PORT_NUM_1      1

#define PORT_X_1        D

#define NUM_Y_LINES     4
#define PORT_YA         B
#define PORT_YB         F
#endif

/*----------------------------------------------------------------------------
 *                    QTouch sensor configuration options.
 *
 *
 *----------------------------------------------------------------------------*/

/**
 * Delay cycles that determine the capacitance charge pulse width. Value of 0
 * will enable charge pulse width of 1 clock cycle, 1 will enable a width of 2
 * clock cycles and so on...
 *
 * Possible values: 1,2,3,4,5,10,25
 */
#define QT_DELAY_CYCLES 2

/**
 * Specify the port for SMP.
 * And Specify the pin for SMP on selected SMP port.
 * Any GPIO pin not conflicting with the other touch pins used for the
 * application
 */
#define PORT_SMP        D
#define SMP_PIN         5

/**
 * Specify if YA and YB share the same port.
 * Set the value of the below macro to '1' if the YA and YB pins are share in
 * same port.
 */
#define SHARED_YAYB             0

/*----------------------------------------------------------------------------
 *            Post Processing Configuration for Touch Library.
 *
 *
 *----------------------------------------------------------------------------*/

/**
 * Total ticks per msec.
 * TICKS_PER_MS = (CLK_FREQ/TIMER_PRESCALER)*(1/1000)
 *
 */
#define TICKS_PER_MS                16u /* (16MHz/1024)* (1/1000) */

/* Initialization values for the Qtouch library parameters. */

/*
 * Sensor detect integration (DI) limit.
 * Default value: 4.
 */
#define DEF_QT_DI                      4u

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
#define DEF_QT_RECAL_THRESHOLD         RECAL_50 /* recal threshold = 50% of
	                                         * detect */

/*
 * Positive recalibration delay.
 *
 * Default: 3
 */
#define DEF_QT_POS_RECAL_DELAY         3u

#endif /* TOUCH_CONFIG_H */
