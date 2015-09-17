/**
 * \file
 *
 * \brief AVR XMEGA XCL (XMEGA Custom Logic) Glue Logic (GL) sub-module driver.
 *
 *
 * Copyright (C) 2014-2015 Atmel Corporation. All rights reserved.
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

#ifndef _XCL_GL_H_
#define _XCL_GL_H_

#include "compiler.h"
#include "parts.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \ingroup xcl_group
 * \defgroup xcl_gl_group XMEGA Custom Logic (XCL) Glue Logic (GL) sub-module
 *
 * This group provides functions to configure LUT sub-module:
 * - Configure LUT type with xcl_lut_type()
 * - Select LUT0 output with xcl_lut0_output()
 * - Configure LUT delay type and position with xcl_lut_config_delay()
 * - Configure LUT input type and position with xcl_lut_in0(), xcl_lut_in1(),
 * xcl_lut_in2() and xcl_lut_in3()
 * - Configure the content of both LUT Look up table functions with
 * xcl_lut0_truth() and xcl_lut1_truth()
 *
 * \note Prior to use functions from this group, the XCL module should be
 * enabled with xcl_enable() and the XMEGA port connected to XCL module should
 * have been selected using xcl_port() functions.
 *
 * @{
 */

/* ! XCL Glue Logic (GL) configuration struct */
struct xcl_glue_logic_config_t {
	uint8_t ctrla;
	uint8_t ctrlb;
	uint8_t ctrlc;
	uint8_t ctrld;
};

/* ! XCL LUT0 output  configuration */
enum xcl_lut0_output_t {
	/* ! LUT0 Output disable */
	LUT0_OUT_DISABLE     = XCL_LUTOUTEN_DISABLE_gc,
	/* ! LUT0 Output to pin 0 */
	LUT0_OUT_PIN0        = XCL_LUTOUTEN_PIN0_gc,
	/* ! LUT0 Output to pin 4 */
	LUT0_OUT_PIN4        = XCL_LUTOUTEN_PIN4_gc,
};

/* ! XCL LUT type configuration */
enum xcl_lut_conf_t {
	/* ! Two independent 2-input */
	LUT_2LUT2IN = XCL_LUTCONF_2LUT2IN_gc,
	/* ! Two independent 2-input LUT with duplicated input */
	LUT_2LUT1IN      = XCL_LUTCONF_2LUT1IN_gc,
	/* ! Two 2-input LUT with one common input */
	LUT_2LUT3IN      = XCL_LUTCONF_2LUT3IN_gc,
	/* ! One 3-input LUT */
	LUT_1LUT3IN      = XCL_LUTCONF_1LUT3IN_gc,
	/* ! One 2-input multiplexer controlled by one 2-input LUT */
	LUT_CONF_MUX     = XCL_LUTCONF_MUX_gc,
	/* ! One D-Latch controlled by two 2-input LUT */
	LUT_CONF_DLATCH  = XCL_LUTCONF_DLATCH_gc,
	/* ! One RS-Latch LUT */
	LUT_CONF_RSLATCH = XCL_LUTCONF_RSLATCH_gc,
	/* ! One DFF with data driven by two independent 2-input LUT */
	LUT_CONF_DFF     = XCL_LUTCONF_DFF_gc,
};

/* ! XCL LUT input pin configuration */
enum xcl_lut_in_src_t {
	/* ! Event system selected as source */
	LUT_IN_EVSYS   = 0,
	/* ! XCL selected as source */
	LUT_IN_XCL     = 0x01,
	/* ! LSB port pin selected as source */
	LUT_IN_PINL    = 0x02,
	/* ! MSB port pin selected as source */
	LUT_IN_PINH    = 0x03,
};

/* ! XCL LUT delay type configuration */
enum xcl_delay_type_t {
	/* ! 1-cycle delay for both LUT1 and LUT0 */
	DLY11 = XCL_DLYSEL_DLY11_gc,
	/* ! 1-cycle delay for LUT1 and 2-cycle delay for LUT0 */
	DLY12 = XCL_DLYSEL_DLY12_gc,
	/* ! 2-cycle delay for LUT1 and 1-cycle delay for LUT0 */
	DLY21 = XCL_DLYSEL_DLY21_gc,
	/* ! 2-cycle delay for both LUT1 and LUT0 */
	DLY22 = XCL_DLYSEL_DLY22_gc,
};

/* ! XCL LUT delay position configuration */
enum xcl_delay_position_t {
	/* ! No delay */
	LUT_DLY_DISABLE = 0x00,
	/* ! Delay enable on LUT input */
	LUT_DLY_IN      = 0x01,
	/* ! Delay enable on LUT output */
	LUT_DLY_OUT     = 0x02,
};

/* ! XCL LUT lookup table logic function configuration */
enum xcl_lut_thruth_t {
	/* ! AND logic function */
	AND    = 0x8,
	/* ! NAND logic function */
	NAND   = 0x7,
	/* ! OR logic function */
	OR     = 0xE,
	/* ! XOR logic function */
	XOR    = 0x6,
	/* ! NOR logic function */
	NOR    = 0x1,
	/* ! XNOR logic function */
	XNOR   = 0x9,
	/* ! NOT logic function on IN0 input (ignore IN1) */
	NOT_IN0= 0x5,
	/* ! NOT logic function on IN1 input (ignore IN0) */
	NOT_IN1= 0x3,
	/* ! NOT logic function on IN2 input (ignore IN3) */
	NOT_IN2= 0x5,
	/* ! NOT logic function on IN3 input (ignore IN2) */
	NOT_IN3= 0x3,
	/* ! Copy IN0 input (ignore IN1) */
	IN0    = 0xA,                   
	/* ! Copy IN1 input (ignore IN0) */
	IN1    = 0xC,                   
	/* ! Copy IN2 input (ignore IN3) */
	IN2    = 0xA,                   
	/* ! Copy IN3 input (ignore IN2) */
	IN3    = 0xC,
};

/**
 * \brief Configure XCL LUT0 output
 *
 * This function configures the XCL LUT0 output (OUT0).
 *
 * \param lut0out The output pin selected
 */
static inline void xcl_lut0_output(enum xcl_lut0_output_t lut0out)
{
	XCL.CTRLA &= ~XCL_LUT0OUTEN_gm;
	XCL.CTRLA |= lut0out;
}

/**
 * \brief Configure XCL LUT configuration type
 *
 * This function configures the XCL type.
 *
 * \param lutconf The selected LUT type
 */
static inline void xcl_lut_type(enum xcl_lut_conf_t lutconf)
{
	XCL.CTRLA &= ~XCL_LUTCONF_gm;
	XCL.CTRLA |= lutconf;
}

/**
 * \brief Configure XCL LUT IN0 input.
 *
 * This function configures IN0 input.
 *
 * \param in The selected IN0 type
 */
static inline void xcl_lut_in0(enum xcl_lut_in_src_t in)
{
	XCL.CTRLB &= ~XCL_IN0SEL_gm;
	XCL.CTRLB |= in << XCL_IN0SEL_gp;
}

/**
 * \brief Configure XCL LUT IN1 input.
 *
 * This function configures IN1 input.
 *
 * \param in The selected IN1 type
 */
static inline void xcl_lut_in1(enum xcl_lut_in_src_t in)
{
	XCL.CTRLB &= ~XCL_IN1SEL_gm;
	XCL.CTRLB |= in << XCL_IN1SEL_gp;
}

/**
 * \brief Configure XCL LUT IN2 input.
 *
 * This function configures IN2 input.
 *
 * \param in The selected IN2 type
 */
static inline void xcl_lut_in2(enum xcl_lut_in_src_t in)
{
	XCL.CTRLB &= ~XCL_IN2SEL_gm;
	XCL.CTRLB |= in << XCL_IN2SEL_gp;
}

/**
 * \brief Configure XCL LUT IN3 input.
 *
 * This function configures IN3 input.
 *
 * \param in The selected IN3 type
 */
static inline void xcl_lut_in3(enum xcl_lut_in_src_t in)
{
	XCL.CTRLB &= ~XCL_IN3SEL_gm;
	XCL.CTRLB |= in << XCL_IN3SEL_gp;
}

/**
 * \brief Configure XCL LUT delays.
 *
 * This function configures the XCL LUT delays cells.
 *
 * \param dly_type The delays type (LUTO/LUT1 or both)
 * \param lut0_dly_pos Delay cell position for LUT0
 * \param lut1_dly_pos Delay cell position for LUT0
 */
static inline void xcl_lut_config_delay(enum xcl_delay_type_t dly_type,
		enum xcl_delay_position_t lut0_dly_pos,
		enum xcl_delay_position_t lut1_dly_pos)
{
	XCL.CTRLC = 0;
	XCL.CTRLC |= (uint8_t)dly_type | (uint8_t)lut0_dly_pos
			| (uint8_t)lut1_dly_pos << XCL_DLY1CONF_gp;
}

/**
 * \brief Configure XCL LUT0 Look up table.
 *
 * This function configures the XCL LUT0 with matching LUT configuration
 * according to the logic function requested.
 *
 * \param thruth The look up table value
 */
static inline void xcl_lut0_truth(enum xcl_lut_thruth_t thruth)
{
	XCL.CTRLD &= ~XCL_TRUTH0_gm;
	XCL.CTRLD |= thruth << XCL_TRUTH0_gp;
}

/**
 * \brief Configure XCL LUT1 Look up table.
 *
 * This function configures the XCL LUT1 with matching LUT configuration
 * according to the logic function requested.
 *
 * \param thruth The look up table value
 */
static inline void xcl_lut1_truth(enum xcl_lut_thruth_t thruth)
{
	XCL.CTRLD &= ~XCL_TRUTH1_gm;
	XCL.CTRLD |= thruth << XCL_TRUTH1_gp;
}

/** @} */

#ifdef __cplusplus
}
#endif

#endif  /* _XCL_GL_H_ */
