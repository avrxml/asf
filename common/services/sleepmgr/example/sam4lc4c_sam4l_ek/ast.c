/**
 * \file
 *
 * \brief AST driver for SAM.
 *
 * Copyright (C) 2011-2012 Atmel Corporation. All rights reserved.
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

#include "ast.h"

/**
 * \brief Enable the AST.
 *
 * \param ast Base address of the AST.
 */
void ast_enable(Ast *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}
	/* Enable the AST */
	ast->AST_CR |= AST_CR_EN;
	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function will initialize the AST module in counter Mode.
 *
 * \note  If you use the 32 KHz oscillator, it will enable this module.
 *
 * \param ast Base address of the AST.
 * \param osc_type The oscillator you want to use. If you need a better
 *        accuracy, use the 32 KHz oscillator (i.e. AST_OSC_32KHZ).
 * \param psel The preselector value for the corresponding oscillator (4-bits).
 *        To obtain this value, you can use this formula:
 *        psel = log(Fosc/Fast)/log(2)-1, where Fosc is the frequency of the
 *        oscillator you are using (32 KHz or 115 KHz) and Fast the frequency
 *        desired.
 * \param ast_counter Startup counter value
 *
 * \return 1 if the initialization succeeds otherwise it will return 0.
 */
uint32_t ast_init_counter(Ast *ast, uint8_t osc_type,
		uint8_t psel, uint32_t ast_counter)
{
	uint32_t time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return 0;
		}
	}
	ast->AST_CLOCK = osc_type << AST_CLOCK_CSSEL_Pos;
	time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return 0;
		}
	}
	ast->AST_CLOCK |= AST_CLOCK_CEN;
	time_out = AST_POLL_TIMEOUT;
	while (ast_is_clkbusy(ast)) {
		if (--time_out == 0) {
			return 0;
		}
	}

	/* Set the new AST configuration */
	ast->AST_CR = psel << AST_CR_PSEL_Pos;

	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}

	/* Set the calendar */
	ast_set_counter_value(ast, ast_counter);

	return 1;
}

/**
 * \brief This function sets the AST current counter value.
 *
 * \param ast         Base address of the AST.
 * \param ast_counter Startup counter value
 */
void ast_set_counter_value(Ast *ast,
		uint32_t ast_counter)
{
	/* Wait until we can write into the VAL register */
	while (ast_is_busy(ast)) {
	}
	/* Set the new val value */
	ast->AST_CV = ast_counter;
	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function set the AST periodic0 value.
 *
 * \param ast Base address of the AST.
 * \param pir AST periodic0.
 */
void ast_set_periodic0_value(Ast *ast, uint32_t pir)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}
	/* Set the periodic prescaler value */
	ast->AST_PIR0 = pir;
	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function enables the AST periodic0 event.
 *
 * \param ast Base address of the AST.
 */
void ast_enable_periodic0(Ast *ast)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}
	/* Enable the AST periodic0 peripheral event */
	ast->AST_EVE |= AST_EVE_PER0;
	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief Check the busy status of AST clock
 *
 * \param ast Base address of the AST.
 *
 * \return 1 If AST clock is busy, else it will return 0.
 */
uint32_t ast_is_clkbusy(Ast *ast)
{
	return (ast->AST_SR & AST_SR_CLKBUSY) != 0;
}

/**
 * \brief Check the busy status of AST.
 *
 * \param ast Base address of the AST.
 *
 * \return 1 If AST is busy, else it will return 0.
 */
uint32_t ast_is_busy(Ast *ast)
{
	return (ast->AST_SR & AST_SR_BUSY) != 0;
}

/**
 * \brief This function clears the AST status flags.
 *
 * \param ast          Base address of the AST.
 * \param status_mask  AST status flag to be cleared
 */
void ast_clear_status_flag(Ast *ast,
		uint32_t status_mask)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}
	/* Clear the AST status flags */
	ast->AST_SCR = status_mask;
	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function clears the AST Periodic event status flags.
 *
 * \param ast            Base address of the AST.
 * \param periodic_channel  AST wake-up flag to be cleared.
 */
void ast_clear_periodic_status_flag(Ast *ast,
		uint32_t periodic_channel)
{
	if (periodic_channel) {
		/* Clear the AST periodic event status flag */
		ast_clear_status_flag(ast, AST_SCR_PER1);
	} else {
		/* Clear the AST periodic event status flag */
		ast_clear_status_flag(ast, AST_SCR_PER0);
	}
}


/**
 * \brief This function enables the AST interrupts
 *
 * \param ast             Base address of the AST.
 * \param interrupt_mask  AST Interrupts to be enabled
 */
void ast_enable_interrupt(Ast *ast,
		uint32_t interrupt_mask)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}
	/* Enable the AST interrupt */
	ast->AST_IER |= interrupt_mask;
	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function enables the AST Periodic interrupt.
 *
 * \param ast              Base address of the AST.
 * \param periodic_channel AST Periodic Channel
 */
void ast_enable_periodic_interrupt(Ast *ast,
		uint8_t periodic_channel)
{
	/* Enable the AST Periodic Asynchronous Wake-up */
	if (periodic_channel) {
		ast_enable_interrupt(ast, AST_IER_PER1);
	} else {
		ast_enable_interrupt(ast, AST_IER_PER0);
	}
}

/**
 * \brief This function enables the AST Asynchronous wake-up.
 *
 * \param ast          Base address of the AST.
 * \param wakeup_mask  AST wake-up flag to be enabled.
 */
void ast_enable_async_wakeup(Ast *ast,
		uint32_t wakeup_mask)
{
	/* Wait until the ast CTRL register is up-to-date */
	while (ast_is_busy(ast)) {
	}
	/* Enable the AST Asynchronous Wake-up */
	ast->AST_WER |= wakeup_mask;
	/* Wait until write is done */
	while (ast_is_busy(ast)) {
	}
}

/**
 * \brief This function enables the AST Periodic Asynchronous wake-up.
 *
 * \param ast              Base address of the AST.
 * \param periodic_channel AST Periodic Channel
 */
void ast_enable_periodic_async_wakeup(Ast *ast,
		uint8_t periodic_channel)
{
	/* Enable the AST Periodic Asynchronous Wake-up */
	if (periodic_channel) {
		ast_enable_async_wakeup(ast, AST_WER_PER1);
	} else {
		ast_enable_async_wakeup(ast, AST_WER_PER0);
	}
}

