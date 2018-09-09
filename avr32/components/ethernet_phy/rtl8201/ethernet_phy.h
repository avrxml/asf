/**
 * \file
 *
 * \brief Ethernet Phy Management for RTL8201 series
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
