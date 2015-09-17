/**
 * \file
 *
 * \brief 32-bit vectors program test
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
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

/**
 * \mainpage
 * \section intro Introduction
 * This example demonstrates use of 32-bit fixed point vector functions.\n
 * To visualize the result, you may set a breakpoint after the sprintf and
 * check the string variable output through Watch Window in AVR Studio 5.
 * Through this page, you can access the \ref group_dsplib documentation.
 *
 * \section files Main Files
 * - vectors_example.c : DSPLIB Vector Usage example
 * - conf_board.h: Simulator configuration for example
 * - conf_clock.h: Clock configuration for example
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC for AVR32.
 *
 * \section configinfo Configuration Information
 * This example has been tested with the following configuration:
 * - AVR Simulator
 * - CPU clock: <i> Internal RC oscillator (about 115200 Hz) </i>.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/products/AVR32/">Atmel AVR32</A>.\n
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#include <avr32/io.h>

#include "compiler.h"
#include "board.h"

#include "dsp.h"
#include "dsp_debug.h"

#include "sysclk.h"
#include "cycle_counter.h"

// Length of the output signal
#define VECT1_SIZE  16
// Length of the first input signal
#define VECT2_SIZE  16
// Length of the second input signal
#define VECT3_SIZE  16

// The output signal
A_ALIGNED dsp32_t vect1[VECT1_SIZE];
// The first input signal
A_ALIGNED dsp32_t vect2[VECT2_SIZE] = {
	DSP32_Q(0.012010),
	DSP32_Q(0.059717),
	DSP32_Q(0.101397),
	DSP32_Q(0.0583150),
	DSP32_Q(0.0000000),
	DSP32_Q(0.0921177),
	DSP32_Q(0.0951057),
	DSP32_Q(0.0884270),
	DSP32_Q(0.0732020),
	DSP32_Q(0.515080),
	DSP32_Q(0.0583150)
};
// The second input signal
A_ALIGNED dsp32_t vect3[VECT3_SIZE] = {
	DSP32_Q(0.5),
	DSP32_Q(0.101397),
	DSP32_Q(0.110140),
	DSP32_Q(0.076849),
	DSP32_Q(-0.794687),
	DSP32_Q(-0.113757),
	DSP32_Q(-0.250267),
	DSP32_Q(-0.389747),
	DSP32_Q(-0.509602),
	DSP32_Q(-0.587785),
	DSP32_Q(-0.113757)
};

#define CHECK_CONDITIONS(x_cond) \
			if (!(x_cond)) { \
				return -1; \
			}

// Type to point on the function for the menu
typedef int (*fct_item_menu)(int *);

// Type to define the items of the menu
typedef struct {
	char *title;
	fct_item_menu action;
}s_item_menu;

// Number of Functions implemented in this example
#define NUMBER_OF_MENU_ITEM 14

// Functions implemented in this example
int addition(int *size);
int subtraction(int *size);
int real_multiplication(int *size);
int real_division(int *size);
int dot_multiplication(int *size);
int dot_division(int *size);
int minimum(int *size);
int maximum(int *size);
int negate(int *size);
int convolution(int *size);
int partial_convolution(int *size);
int copy(int *size);
int zero_padding(int *size);
int power(int *size);

// To define the menu
s_item_menu item_menu[] = {
	{ "Addition", addition },
	{ "Subtraction", subtraction },
	{ "Multiplication with real", real_multiplication },
	{ "Division with real", real_division },
	{ "Dot multiplication", dot_multiplication },
	{ "Dot division", dot_division },
	{ "Power", power },
	{ "Minimum", minimum },
	{ "Maximum", maximum },
	{ "Negate", negate },
	{ "Copy", copy },
	{ "Zero padding", zero_padding },
	{ "Convolution", convolution },
	{ "Partial convolution", partial_convolution }
};


// Function Definitions
/**
 * \brief Function to perform Vector Addition
 * \param size Pointer to store output Vector Size
 */
int addition(int *size)
{
	int cycle_count;

	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
	CHECK_CONDITIONS(VECT2_SIZE == VECT3_SIZE)

	*size = VECT2_SIZE;

	cycle_count = Get_sys_count();
	dsp32_vect_add(vect1, vect2, vect3, VECT2_SIZE);

	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to perform Vector Subtraction
 * \param size Pointer to store output Vector Size
 */
int subtraction(int *size)
{
	int cycle_count;

	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
	CHECK_CONDITIONS(VECT2_SIZE == VECT3_SIZE)

	*size = VECT2_SIZE;

	cycle_count = Get_sys_count();
	dsp32_vect_sub(vect1, vect2, vect3, VECT2_SIZE);

	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to perform Vector Real Multiplication
 * \param size Pointer to store output Vector Size
 */
int real_multiplication(int *size)
{
	int cycle_count;

	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
	CHECK_CONDITIONS(VECT3_SIZE > 0)

	*size = VECT2_SIZE;

	cycle_count = Get_sys_count();
	dsp32_vect_realmul(vect1, vect2, VECT2_SIZE, vect3[0]);

	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to perform Vector Real Division
 * \param size Pointer to store output Vector Size
 */
int real_division(int *size)
{
	int cycle_count;

	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
	CHECK_CONDITIONS(VECT3_SIZE > 0)

	*size = VECT2_SIZE;

	cycle_count = Get_sys_count();
	dsp32_vect_realdiv(vect1, vect2, VECT2_SIZE, vect3[0]);

	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to perform Vector Dot Multiplication
 * \param size Pointer to store output Vector Size
 */
int dot_multiplication(int *size)
{
	int cycle_count;

	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
	CHECK_CONDITIONS(VECT2_SIZE == VECT3_SIZE)

	*size = VECT2_SIZE;

	cycle_count = Get_sys_count();
	dsp32_vect_dotmul(vect1, vect2, vect3, VECT2_SIZE);

	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to perform Vector Dot Division
 * \param size Pointer to store output Vector Size
 */
int dot_division(int *size)
{
	int cycle_count;

	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
	CHECK_CONDITIONS(VECT2_SIZE == VECT3_SIZE)

	*size = VECT2_SIZE;

	cycle_count = Get_sys_count();
	dsp32_vect_dotdiv(vect1, vect2, vect3, VECT2_SIZE);

	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to perform Vector Power operation
 * \param size Pointer to store output Vector Size
 */
int power(int *size)
{
	int cycle_count;

	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)
	CHECK_CONDITIONS(VECT3_SIZE > 0)

	*size = VECT2_SIZE;

	cycle_count = Get_sys_count();
	dsp32_vect_pow(vect1, vect2, VECT2_SIZE, vect3[0]);

	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to find Vector Minimum
 * \param size Pointer to store output Vector Size
 */
int minimum(int *size)
{
	int cycle_count;

	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE > 0)
	CHECK_CONDITIONS(VECT2_SIZE > 0)

	*size = 1;

	cycle_count = Get_sys_count();
	vect1[0] = dsp32_vect_min(vect2, VECT2_SIZE);

	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to find Vector Maximum
 * \param size Pointer to store output Vector Size
 */
int maximum(int *size)
{
	int cycle_count;

	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE > 0)
	CHECK_CONDITIONS(VECT2_SIZE > 0)

	*size = 1;

	cycle_count = Get_sys_count();
	vect1[0] = dsp32_vect_max(vect2, VECT2_SIZE);

	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to perform Vector Negation
 * \param size Pointer to store output Vector Size
 */
int negate(int *size)
{
	int cycle_count;

	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)

	*size = VECT2_SIZE;

	cycle_count = Get_sys_count();
	dsp32_vect_neg(vect1, vect2, VECT2_SIZE);

	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to perform Vector Copy
 * \param size Pointer to store output Vector Size
 */
int copy(int *size)
{
	int cycle_count;

	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE >= VECT2_SIZE)

	*size = VECT2_SIZE;

	cycle_count = Get_sys_count();
	dsp32_vect_copy(vect1, vect2, VECT2_SIZE);

	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to perform Zero Padding to Vector
 * \param size Pointer to store output Vector Size
 */
int zero_padding(int *size)
{
	int cycle_count;

	*size = VECT1_SIZE;

	cycle_count = Get_sys_count();
	dsp32_vect_zeropad(vect1, VECT1_SIZE, VECT1_SIZE);

	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to perform Vector Convolution
 * \param size Pointer to store output Vector Size
 */
int convolution(int *size)
{
	int cycle_count;
	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE >= (VECT2_SIZE + VECT3_SIZE - 1))
	CHECK_CONDITIONS(VECT2_SIZE >= 8)
	CHECK_CONDITIONS(VECT3_SIZE >= 8)
	if (VECT2_SIZE > VECT3_SIZE)
		CHECK_CONDITIONS(VECT1_SIZE >= (VECT2_SIZE + 2*VECT3_SIZE - 2))
	else
		CHECK_CONDITIONS(VECT1_SIZE >= (VECT3_SIZE + 2*VECT2_SIZE - 2))

	*size = VECT2_SIZE + VECT3_SIZE - 1;

	cycle_count = Get_sys_count();
	// Perform a convolution
	dsp32_vect_conv(vect1, vect2, VECT2_SIZE, vect3, VECT3_SIZE);
	return Get_sys_count() - cycle_count;
}

/**
 * \brief Function to perform Partial Convolution
 * \param size Pointer to store output Vector Size
 */
int partial_convolution(int *size)
{
	int cycle_count;

	// Conditions
	CHECK_CONDITIONS(VECT1_SIZE >= (VECT2_SIZE - VECT3_SIZE + 4))
	CHECK_CONDITIONS(!(VECT2_SIZE&3) && VECT2_SIZE > 0)
	CHECK_CONDITIONS(VECT3_SIZE >= 8)

	*size = VECT2_SIZE - VECT3_SIZE + 1;

	cycle_count = Get_sys_count();
	// Perform a partial convolution
	dsp32_vect_convpart(vect1, vect2, VECT2_SIZE, vect3, VECT3_SIZE);
	return Get_sys_count() - cycle_count;
}

//! \brief Main Function
int main(int argc, char *argv[])
{
	int cycle_count, size;
	int i;
	struct {
		char menu[25];
		char result[VECT1_SIZE][15];
	} vectors_result;
	/**
	 * \note the call to sysclk_init() will disable all non-vital
	 * peripheral clocks, except for the peripheral clocks explicitly
	 * enabled in conf_clock.h.
	 */
	sysclk_init();

	// Initialize the DSP debug USART module when running in external board
#if BOARD != AVR_SIMULATOR_UC3
	dsp_debug_initialization(FOSC0);
#endif

	for (i = 0; i >= 0 && i < (sizeof(item_menu) / sizeof(s_item_menu)); i++) {
		// Call the function to execute
		cycle_count = item_menu[i].action(&size);
		dsp32_debug_sprintf(vectors_result.menu, "%s", item_menu[i].title);
		if (cycle_count != -1) {
			// Print the result
			for (i = 0; i < size; i++) {
				dsp32_debug_sprintf(vectors_result.result[i],"%f", vect1[i]);
			}
			/*
			 * Place a breakpoint here and check the ASCII output in
			 * vectors_result in Memory Window.
			 * Note: Find the variable address in Watch Window and enter it in
			 * Memory Window
			 */
			 asm("nop");
		}
	}

	while (1) {
		// Intentionally left blank.
	}
}
