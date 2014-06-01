/**
 * \file
 *
 * \brief TWIHS Master Mode management
 *
 * Copyright (c) 2014 Atmel Corporation. All rights reserved.
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
#ifndef TWIHS_MASTER_H_INCLUDED
#define TWIHS_MASTER_H_INCLUDED

#include <parts.h>
#include <compiler.h>

#if (SAMG)
# include "sam_twihs/twihs_master.h"
#else
# error Unsupported chip type
#endif

/**
 *
 * \defgroup twihs_group Two Wire-interface High Speed(TWIHS)
 *
 * This is the common API for TWIHS. Additional features are available
 * in the documentation of the specific modules.
 *
 * See \ref twihs_quickstart.
 *
 * \section twihs_group_platform Platform Dependencies
 *
 * The TWIHS API is partially chip- or platform-specific. While all
 * platforms provide mostly the same functionality, there are some
 * variations around how different bus types and clock tree structures
 * are handled.
 *
 * The following functions are available on all platforms, but there may
 * be variations in the function signature (i.e. parameters) and
 * behaviour. These functions are typically called by platform-specific
 * parts of drivers, and applications that aren't intended to be
 * portable:
 *   - Master TWIHS Module initialization
 *   \code status_code_t twihs_master_setup(*twihs_module_pointer, twihs_master_options_t *opt) \endcode
 *   - Enables TWIHS Module
 *   \code void twihs_master_enable(*twihs_module_pointer) \endcode
 *   - Disables TWIHS Module
 *   \code void twihs_master_disable(*twihs_module_pointer) \endcode
 *   - Read data from a slave device
 *   \code status_code_t twihs_master_read(*twihs_module_pointer, twihs_package_t *package) \endcode
 *   - Write data from to a slave device
 *   \code status_code_t twihs_master_write(*twihs_module_pointer, twihs_package_t *package) \endcode
 *
 * @{
 */

/**
 * \typedef twihs_master_t
 * This type can be used independently to refer to TWIHS master module for the
 * architecture used. It refers to the correct type definition for the
 * architecture.
 */

//! @}

#endif /* TWI_MASTER_H_INCLUDED */
