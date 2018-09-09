/**
 * \file
 *
 * \brief Ethernet Phy Management for a dummy Ethernet phy component
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
#ifndef DUMMY_PHY_H_INCLUDED
#define DUMMY_PHY_H_INCLUDED

#include "macb.h"


#ifndef EXTPHY_PHY_ADDR
/*! Phy Address (set through strap options) 
    This Define must be place in the board configuration
*/
#define EXTPHY_PHY_ADDR             0x01
#warning Set a default value : #define EXTPHY_PHY_ADDR 0x01
#endif 

/*! Phy Identifier for a dummy Ethernet phy */
#define EXTPHY_PHY_ID               0xFFFFFFFF



/*! \name Extended registers for a dummy Ethernet phy
 */
//! @{
#define PHY_RBR             0x17        //!< RMII Bypass reg
#define PHY_MICR            0x11        //!< Interrupt Control reg
#define PHY_MISR            0x12        //!< Interrupt Status reg
#define PHY_PHYCR           0x19        //!< Phy CTRL reg
//! @}

/*! RMII Bypass Register */
#define RBR_RMII                 0x0020  //!< RMII Mode
/*! \name Interrupt Ctrl Register.
 */
//! @{
#define MICR_INTEN               0x0002  //!< Enable interrupts
#define MICR_INTOE               0x0001  //!< Enable INT output
//! @}

/*! \name Interrupt Status Register.
 */
//! @{
#define MISR_ED_INT_EN           0x0040  //!< Energy Detect enabled
#define MISR_LINK_INT_EN         0x0020  //!< Link status change enabled
#define MISR_SPD_INT_EN          0x0010  //!< Speed change enabled
#define MISR_DP_INT_EN           0x0008  //!< Duplex mode change enabled
#define MISR_ANC_INT_EN          0x0004  //!< Auto-Neg complete enabled
#define MISR_FHF_INT_EN          0x0002  //!< False Carrier enabled
#define MISR_RHF_INT_EN          0x0001  //!< Receive Error enabled
#define MISR_ED_INT              0x4000  //!< Energy Detect
#define MISR_LINK_INT            0x2000  //!< Link status change
#define MISR_SPD_INT             0x1000  //!< Speed change
#define MISR_DP_INT              0x0800  //!< Duplex mode change
#define MISR_ANC_INT             0x0400  //!< Auto-Neg complete
#define MISR_FHF_INT             0x0200  //!< False Carrier
#define MISR_RHF_INT             0x0100  //!< Receive Error
//! @}

/*! \name Phy Ctrl Register.
 */
//! @{
#define PHYCR_MDIX_EN            0x8000  //!< Enable Auto MDIX
#define PHYCR_MDIX_FORCE         0x4000  //!< Force MDIX crossed
//! @}

/*! \brief Generate an Hardware Reset on the Phy.
 *
 */
static inline void ethernet_phy_hw_reset(void) {}

/*! \brief Generate a Software Reset on the Phy.
 *
 */
static inline void ethernet_phy_sw_reset(volatile avr32_macb_t *macb) {}

/*! \brief Setup the Phy in RMII mode.
 *
 */
static inline void ethernet_phy_setup_rmii(volatile avr32_macb_t *macb) {}

/*! \brief Setup auto-negotiation for the Phy.
 *
 */
static inline void ethernet_phy_setup_auto_negotiation(volatile avr32_macb_t *macb, volatile unsigned long *config) {}


#endif /* DUMMY_PHY_H_INCLUDED */
