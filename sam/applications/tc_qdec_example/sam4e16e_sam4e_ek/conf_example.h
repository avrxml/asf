/**
 * \file
 *
 * \brief SAM4E-EK TC Quadrature Decoder Example configuration.
 *
 * Copyright (c) 2014-2016 Atmel Corporation. All rights reserved.
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

#ifndef CONF_EXAMPLE_H_INCLUDED
#define CONF_EXAMPLE_H_INCLUDED

/**
 * \brief  QDEC Instance
 *
 * This enum is used to specify the QDEC Instances
 *
 */
enum{
	QDEC_INSTANCE0,
	QDEC_INSTANCE1,
	QDEC_INSTANCE2,
	QDEC_INSTANCE_MAX
};

/**
 * \brief TC QDEC channel id for each instance enum
 *
 * This enum is used to specify the QDEC channel id for each instance enum
 *
 */
enum id_tc_qdec_channel {
	ID_TC_QDEC_CHANNEL0,
	ID_TC_QDEC_CHANNEL1,
	ID_TC_QDEC_CHANNEL2,
	ID_TC_QDEC_CHANNEL_MAX
};

/**
 * \brief TC QDEC mode 
 *
 * This enum is used to specify the QDEC mode
 *
 */
enum tc_qdec_mode {
	/*QDEC position measurement mode */
	TC_QDEC_MODE_POSITION ,
	/*QDEC speed measurement mode */
	TC_QDEC_MODE_SPEED,
	/*QDEC speed measurement mode */
	TC_QDEC_MODE_TEST_BASETIMER,
};

#define TC_QDEC0_ACTIVATION			DISABLE
#define TC_QDEC0_MODE				TC_QDEC_MODE_POSITION

#define TC_QDEC1_ACTIVATION			ENABLE
#define TC_QDEC1_MODE				TC_QDEC_MODE_POSITION

#define TC_QDEC2_ACTIVATION			DISABLE
#define TC_QDEC2_MODE				TC_QDEC_MODE_POSITION


#if ( TC_QDEC0_ACTIVATION == ENABLE || TC_QDEC2_ACTIVATION == ENABLE)
#error This Example developed and tested for QDEC1 in SAM4E-EK only. Enable TC_QDEC1.\
(other options provided for flexible porting for different evaluation kits).
#elif (TC_QDEC1_ACTIVATION == DISABLE)
#warning No QDEC is enabled. Enable TC_QDEC1 for this example.\
(other options provided for flexible porting for different evaluation kits).
#endif

//! [TC_QDEC0 define]
/* Configure TC0 QDEC input signals pins and corresponding channel ids */
#define TC_QDEC0_INSTANCE 			((TC_QDEC0_ACTIVATION == ENABLE) ? TC0 : 0)
#define ID_TC_QDEC0_CHANNEL0		ID_TC0
#define ID_TC_QDEC0_CHANNEL1		ID_TC1
#define ID_TC_QDEC0_CHANNEL2		ID_TC2

#define PIN_TC_QDEC0_PHA			(PIO_PA0_IDX)			//TIOA0
#define PIN_TC_QDEC0_PHA_MUX		(IOPORT_MODE_MUX_B)
#define PIN_TC_QDEC0_PHB			(PIO_PA1_IDX)			//TIOB0
#define PIN_TC_QDEC0_PHB_MUX		(IOPORT_MODE_MUX_B)
#define PIN_TC_QDEC0_INDEX			(PIO_PA16_IDX)			//TIOB1
#define PIN_TC_QDEC0_INDEX_MUX		(IOPORT_MODE_MUX_B)

#define ID_TC_QDEC0_WAVEFORM		ID_TC2
#define PIN_TC_QDEC0_WAVEFORM		(PIO_PC26_IDX)			//TIOA2
#define PIN_TC_QDEC0_WAVEFORM_MUX	(IOPORT_MODE_MUX_B)

//! [TC_QDEC1 define]
/* Configure TC1 QDEC input signals pins and corresponding channel ids */
#define TC_QDEC1_INSTANCE			((TC_QDEC1_ACTIVATION == ENABLE) ? TC1 : 0)
#define ID_TC_QDEC1_CHANNEL0		ID_TC3
#define ID_TC_QDEC1_CHANNEL1		ID_TC4
#define ID_TC_QDEC1_CHANNEL2		ID_TC5

#define PIN_TC_QDEC1_PHA			(PIO_PC23_IDX)			//TIOA3
#define PIN_TC_QDEC1_PHA_MUX		(IOPORT_MODE_MUX_B)
#define PIN_TC_QDEC1_PHB			(PIO_PC24_IDX)			//TIOB3
#define PIN_TC_QDEC1_PHB_MUX		(IOPORT_MODE_MUX_B)
#define PIN_TC_QDEC1_INDEX			(PIO_PA27_IDX)			//TIOB4
#define PIN_TC_QDEC1_INDEX_MUX		(IOPORT_MODE_MUX_B)

#define ID_TC_QDEC1_WAVEFORM		ID_TC5
#define PIN_TC_QDEC1_WAVEFORM		(PIO_PC29_IDX)			//TIOA5
#define PIN_TC_QDEC1_WAVEFORM_MUX	(IOPORT_MODE_MUX_B)

//! [TC_QDEC2 define]
/* Configure TC2 QDEC input signals pins and corresponding channel ids */
#define TC_QDEC2_INSTANCE			((TC_QDEC2_ACTIVATION == ENABLE) ? TC2 : 0)
#define ID_TC_QDEC2_CHANNEL0		ID_TC6
#define ID_TC_QDEC2_CHANNEL1		ID_TC7
#define ID_TC_QDEC2_CHANNEL2		ID_TC8

#define PIN_TC_QDEC2_PHA			(PIO_PC5_IDX)			//TIOA6
#define PIN_TC_QDEC2_PHA_MUX		(IOPORT_MODE_MUX_B)
#define PIN_TC_QDEC2_PHB			(PIO_PC6_IDX)			//TIOB6
#define PIN_TC_QDEC2_PHB_MUX		(IOPORT_MODE_MUX_B)
#define PIN_TC_QDEC2_INDEX			(PIO_PC9_IDX)			//TIOB7
#define PIN_TC_QDEC2_INDEX_MUX		(IOPORT_MODE_MUX_B)

#define ID_TC_QDEC2_WAVEFORM		ID_TC3
#define PIN_TC_QDEC2_WAVEFORM		(PIO_PC11_IDX)			//TIOA8
#define PIN_TC_QDEC2_WAVEFORM_MUX	(IOPORT_MODE_MUX_B)

#endif /* CONF_EXAMPLE_H_INCLUDED */
