/**
 * \file
 *
 * \brief ICM example for SAM.
 *
 * Copyright (C) 2013-2015 Atmel Corporation. All rights reserved.
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
 * This example demonstrates how to use ICM driver to check integrity of internal SRAM.
 *
 * \section files Main Files
 *  - icm.c ICM driver implementation
 *  - icm.h ICM driver definitions
 *  - icm_example.c example application
 *
 * \section compilinfo Compilation Information
 * This software is written for GNU GCC and IAR Embedded Workbench
 * for Atmel. Other compilers may or may not work.
 *
 * \section deviceinfo Device Information
 * All SAM devices with a ICM can be used.
 *
 * \section exampledescription Description of the example
 * After an initialization phase, the ICM will compute memory region0 and region1 hash value
 * and write back to hash area. Next set the region monitoring mode to compare mode and
 * modify memory region0 and region1 value, then the mismatch interrupt will be generated.
 *
 * \section configinfo Configuration Information
 * - PC terminal settings:
 *   - 115200 bps,
 *   - 8 data bits,
 *   - no parity bit,
 *   - 1 stop bit,
 *   - no flow control.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com">Atmel</A>.\n
 * Support and FAQ: http://www.atmel.com/design-support/
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <asf.h>

/* Region descriptor in main list */
COMPILER_ALIGNED(64)
struct icm_region_descriptor_main_list reg_descriptor[2];

COMPILER_ALIGNED(4)
/* Region descriptor in secondary list */
struct icm_region_descriptor_sec_list reg_descriptor_sec;

/* Hash area */
COMPILER_ALIGNED(128)
uint32_t output_sha[0x20];

/* Memory region0 area */
volatile uint32_t message_sha_0[16] = {
	0x80636261,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x18000000
};

/* Memory region1 main list area */
volatile uint32_t message_sha_1_main[16] = {
	0x80636261,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x18000000,
};

/* Memory region1 secondary list area */
volatile uint32_t message_sha_1_sec[16] = {
	0x80636261,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x00000000,
	0x18000000,
};

/**
 *  Configure serial console.
 */
static void configure_console(void)
{
	const usart_serial_options_t uart_serial_options = {
		.baudrate = CONF_UART_BAUDRATE,
#ifdef CONF_UART_CHAR_LENGTH
		.charlength = CONF_UART_CHAR_LENGTH,
#endif
		.paritytype = CONF_UART_PARITY,
#ifdef CONF_UART_STOP_BITS
		.stopbits = CONF_UART_STOP_BITS,
#endif
	};

	/* Configure console. */
	stdio_serial_init(CONF_UART, &uart_serial_options);
}

static void reg_dig_mismatch_handler(uint8_t reg_num)
{
	if(reg_num == ICM_REGION_NUM_0) {
		printf( " Memory region0 is modified \n\r");
	} else if(reg_num == ICM_REGION_NUM_1) {
		printf( " Memory region1 is modified \n\r");
	}
}

/**
 * \brief Application entry point for ICM example.
 *
 * \return Unused (ANSI-C compatibility).
 */
int main(void)
{
	/* ICM configuration */
	struct icm_config icm_cfg;

	/* Initialize the SAM system */
	sysclk_init();
	board_init();
	sleepmgr_init();

	/* Initialize the console uart */
	configure_console();

	/* Output example information */
	printf("-- ICM Example --\r\n");
	printf("-- %s\n\r", BOARD_NAME);
	printf("-- Compiled: %s %s --\n\r", __DATE__, __TIME__);

	/* ICM initialization */
	icm_cfg.is_write_back= false;
	icm_cfg.is_dis_end_mon = false;
	icm_cfg.is_sec_list_branch = false;
	icm_cfg.bbc = 0;
	icm_cfg.is_auto_mode = false;
	icm_cfg.is_dual_buf = false;
	icm_cfg.is_user_hash = false;
	icm_cfg.ualgo = ICM_SHA_1;
	icm_cfg.hash_area_val = 0;
	icm_cfg.des_area_val = 0;
	icm_init(ICM, &icm_cfg);

	/* Set region0 descriptor */
	reg_descriptor[0].start_addr = (uint32_t)message_sha_0;
	reg_descriptor[0].cfg.is_compare_mode = false;
	reg_descriptor[0].cfg.is_wrap = false;
	reg_descriptor[0].cfg.is_end_mon = false;
	reg_descriptor[0].cfg.reg_hash_int_en = false;
	reg_descriptor[0].cfg.dig_mis_int_en = false;
	reg_descriptor[0].cfg.bus_err_int_en = false;
	reg_descriptor[0].cfg.wrap_con_int_en = false;
	reg_descriptor[0].cfg.ebit_con_int_en = false;
	reg_descriptor[0].cfg.status_upt_con_int_en = false;
	reg_descriptor[0].cfg.is_pro_dly = false;
	reg_descriptor[0].cfg.mem_reg_val = 0;
	reg_descriptor[0].cfg.algo = ICM_SHA_1;
	reg_descriptor[0].tran_size = 0;
	reg_descriptor[0].next_addr = 0;

	/* Set region1 descriptor in main list */
	reg_descriptor[1].start_addr = (uint32_t)message_sha_1_main;
	reg_descriptor[1].cfg.is_compare_mode = false;
	reg_descriptor[1].cfg.is_wrap = false;
	reg_descriptor[1].cfg.is_end_mon = true;
	reg_descriptor[1].cfg.reg_hash_int_en = false;
	reg_descriptor[1].cfg.dig_mis_int_en = false;
	reg_descriptor[1].cfg.bus_err_int_en = false;
	reg_descriptor[1].cfg.wrap_con_int_en = false;
	reg_descriptor[1].cfg.ebit_con_int_en = false;
	reg_descriptor[1].cfg.status_upt_con_int_en = false;
	reg_descriptor[1].cfg.is_pro_dly = false;
	reg_descriptor[1].cfg.mem_reg_val = 0;
	reg_descriptor[1].cfg.algo = ICM_SHA_1;
	reg_descriptor[1].tran_size = 0;
	reg_descriptor[1].next_addr = &reg_descriptor_sec;

	/* Set region1 descriptor in secondary list */
	reg_descriptor_sec.start_addr = (uint32_t)message_sha_1_sec;
	reg_descriptor_sec.tran_size = 0;
	reg_descriptor_sec.next_addr = 0;

	/* Set region descriptor start address */
	icm_set_reg_des_addr(ICM, (uint32_t)&reg_descriptor[0]);

	/* Set hash area start address */
	icm_set_hash_area_addr(ICM, (uint32_t)output_sha);

	/* Enable ICM */
	icm_enable(ICM);

	delay_ms(200);

	/* Set region monitoring mode to compare mode */
	reg_descriptor[0].cfg.is_compare_mode = true;
	reg_descriptor[1].cfg.is_compare_mode = true;

	/* Set callback function for digest mismatch interrupt handler */
	icm_set_callback(ICM, reg_dig_mismatch_handler, ICM_REGION_NUM_0,
			ICM_INTERRUPT_RDM, 1);
	icm_set_callback(ICM, reg_dig_mismatch_handler, ICM_REGION_NUM_1,
			ICM_INTERRUPT_RDM, 1);

	/* Modify memory region value */
	message_sha_0[0] = 0x12345678;
	message_sha_1_sec[0] = 0x12345678;

	/* Enable ICM */
	icm_enable(ICM);

	while (1) {
	}
}
