/**
 * \file
 *
 * \brief Configuration Change Protection write functions
 *
 * Copyright (c) 2010-2018 Microchip Technology Inc. and its subsidiaries.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#ifndef CPU_CCP_H
#define CPU_CCP_H
#include <compiler.h>

/**
 * \defgroup ccp_group Configuration Change Protection
 *
 * See \ref xmega_ccp_quickstart.
 *
 * Function for writing to protected IO registers.
 * @{
 */

#if defined(__DOXYGEN__)
//! \name IAR Memory Model defines.
//@{

/**
 * \def CONFIG_MEMORY_MODEL_TINY
 * \brief Configuration symbol to enable 8 bit pointers.
 *
 */
# define CONFIG_MEMORY_MODEL_TINY

/**
 * \def CONFIG_MEMORY_MODEL_SMALL
 * \brief Configuration symbol to enable 16 bit pointers.
 * \note If no memory model is defined, SMALL is default.
 *
 */
# define CONFIG_MEMORY_MODEL_SMALL


/**
 * \def CONFIG_MEMORY_MODEL_LARGE
 * \brief Configuration symbol to enable 24 bit pointers.
 *
 */
# define CONFIG_MEMORY_MODEL_LARGE

//@}
#endif


/**
 * \brief Write to a CCP-protected 8-bit I/O register
 *
 * \param addr Address of the I/O register
 * \param value Value to be written
 *
 * \note Using IAR Embedded workbench, the choice of memory model has an impact
 *       on calling convention. The memory model is not visible to the
 *       preprocessor, so it must be defined in the Assembler preprocessor directives.
 */
extern void ccp_write_io(void *addr, uint8_t value);

/** @} */

/**
 * \page xmega_ccp_quickstart Quick start guide for CCP driver
 *
 * This is the quick start guide for the \ref ccp_group
 * "Configuration Change Protection (CCP) driver", with step-by-step
 * instructions on how to use the driver.
 *
 * The use case contains a code fragment, and this can be copied into, e.g.,
 * the main application function.
 *
 * \section ccp_basic_use_case Basic use case
 * In this use case, the CCP is used to write to the protected XMEGA Clock
 * Control register.
 *
 * \subsection ccp_basic_use_case_setup_flow Workflow
 * -# call CCP write io to change system clock selection:
 *   - \code ccp_write_io((uint8_t *)&CLK.CTRL, CLK_SCLKSEL_RC32M_gc); \endcode
 */

#endif /* CPU_CCP_H */
