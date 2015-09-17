/**
 * \file
 *
 * \brief GMAC PHY (Ethernet MAC) driver for SAM.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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

#include "compiler.h"
#include "gmac.h"
#include <string.h>
#include "conf_eth.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \defgroup gmac_group Ethernet PHY Media Access Controller
 *
 * See \ref gmac_quickstart.
 *
 * Driver for the GMAC PHY (Ethernet Media Access Controller).
 * This file contains basic functions for the GMAC PHY.
 *
 * \section dependencies Dependencies
 * This driver does not depend on other modules.
 *
 * @{
 */

/**
 * \brief Wait PHY operation to be completed.
 *
 * \param p_gmac HW controller address.
 * \param ul_retry The retry times, 0 to wait forever until completeness.
 *
 * Return GMAC_OK if the operation is completed successfully.
 */
static uint8_t gmac_phy_wait(Gmac* p_gmac, const uint32_t ul_retry)
{
	volatile uint32_t ul_retry_count = 0;

	while (!gmac_is_phy_idle(p_gmac)) {
		if (ul_retry == 0) {
			continue;
		}

		ul_retry_count++;

		if (ul_retry_count >= ul_retry) {
			return GMAC_TIMEOUT;
		}
	}
	return GMAC_OK;
}

/**
 * \brief Read the PHY register.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_phy_address PHY address.
 * \param uc_address Register address.
 * \param p_value Pointer to a 32-bit location to store read data.
 *
 * \Return GMAC_OK if successfully, GMAC_TIMEOUT if timeout.
 */
uint8_t gmac_phy_read(Gmac* p_gmac, uint8_t uc_phy_address, uint8_t uc_address,
		uint32_t* p_value)
{
	gmac_maintain_phy(p_gmac, uc_phy_address, uc_address, 1, 0);

	if (gmac_phy_wait(p_gmac, MAC_PHY_RETRY_MAX) == GMAC_TIMEOUT) {
		return GMAC_TIMEOUT;
	}
	*p_value = gmac_get_phy_data(p_gmac);
	return GMAC_OK;
}

/**
 * \brief Write the PHY register.
 *
 * \param p_gmac   Pointer to the GMAC instance.
 * \param uc_phy_address PHY Address.
 * \param uc_address Register Address.
 * \param ul_value Data to write, actually 16-bit data.
 *
 * \Return GMAC_OK if successfully, GMAC_TIMEOUT if timeout.
 */
uint8_t gmac_phy_write(Gmac* p_gmac, uint8_t uc_phy_address,
		uint8_t uc_address, uint32_t ul_value)
{
	gmac_maintain_phy(p_gmac, uc_phy_address, uc_address, 0, ul_value);

	if (gmac_phy_wait(p_gmac, MAC_PHY_RETRY_MAX) == GMAC_TIMEOUT) {
		return GMAC_TIMEOUT;
	}
	return GMAC_OK;
}

//@}

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
