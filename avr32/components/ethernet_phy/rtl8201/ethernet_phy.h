/**
 * \file
 *
 * \brief Ethernet Phy Management for RTL8201 series
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
#ifndef RTL8201_H_INCLUDED
#define RTL8201_H_INCLUDED

#include "compiler.h"
#include "board.h"
#include "delay.h"
#include "conf_clock.h"
#include "conf_eth.h"
#include "conf_rtl8201.h"
#include "macb.h"

#ifndef EXTPHY_PHY_ADDR
/*! Phy Address (set through strap options) 
    This Define must be place in the board configuration
*/
#define EXTPHY_PHY_ADDR             0x01
#warning Set a default value : #define EXTPHY_PHY_ADDR 0x01
#endif 

#ifndef EXTPHY_MACB_PHYRSTB_PIN
#define EXTPHY_MACB_PHYRSTB_PIN             AVR32_PIN_PA01
#warning Set a default value : #define EXTPHY_MACB_PHYRSTB_PIN     AVR32_PIN_PA01
#endif 

/*! Phy Identifier for the RTL8201 */
#define EXTPHY_PHY_ID               0x001CC810

/*! \name Extended registers for RTL8201
 */
//! @{
#define PHY_TEST            0x19    //!< Test reg
/*! Test RMII Mask */
#define TEST_RMII           0x0400  //!< RMII Mode
/*! Test RMII TXC Out Mask */
#define TEST_TXC_OUT        0x0800  //!< TXC Out Mask
//! @}


/*! \brief Generate an Hardware Reset on the Phy.
 *
 */
static inline void  ethernet_phy_hw_reset(void)
{
     gpio_clr_gpio_pin(EXTPHY_MACB_PHYRSTB_PIN);
     delay_ms(100);
     gpio_set_gpio_pin(EXTPHY_MACB_PHYRSTB_PIN);
     delay_ms(1000);
}

/*! \brief Generate a Software Reset on the Phy.
 *
 */
static inline void ethernet_phy_sw_reset(volatile avr32_macb_t *macb) { }

/*! \brief Setup the Phy in RMII mode.
 *
 */
static inline void ethernet_phy_setup_rmii(volatile avr32_macb_t *macb)
{
    volatile unsigned long mode;
    // Force RMII Mode
    mode = ulReadMDIO(macb, PHY_TEST);
    mode |= TEST_RMII;
#ifdef RTL8201_CONF_TXC_OUT
    // Force PHY to generate its own clock
    mode &= (~TEST_TXC_OUT);
#endif
    vWriteMDIO(macb, PHY_TEST, mode);
    mode = ulReadMDIO(macb, PHY_TEST);
}
/*! \brief Setup auto-negotiation for the Phy.
 *
 */
static inline void ethernet_phy_setup_auto_negotiation(volatile avr32_macb_t *macb , volatile unsigned long *config)
{

#if ETHERNET_CONF_AN_ENABLE
    // reset auto-negotiation capability
    *config |= (BMCR_ANRESTART | BMCR_ANENABLE);
#else

    // clear AN bit
    *config &= ~BMCR_ANENABLE;

    #if ETHERNET_CONF_USE_100MB
      *config |= BMCR_SPEED100;
    #else
      *config &= ~BMCR_SPEED100;
    #endif
    #if ETHERNET_CONF_USE_FULL_DUPLEX
      *config |= BMCR_FULLDPLX;
    #else
      *config &= ~BMCR_FULLDPLX;
    #endif
#endif

}

#endif /* RTL8201_H_INCLUDED */
