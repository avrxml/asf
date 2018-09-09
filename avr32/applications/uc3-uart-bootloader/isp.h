/**************************************************************************
 *
 * \file
 *
 * \brief Interface of the main file of the UART DFU ISP.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _ISP_H_
#define _ISP_H_

#include "preprocessor.h"
#include "conf_isp.h"


//_____ D E F I N I T I O N S ______________________________________________

#define pm_enable_osc_ext_clock   ATPASTE3(pm_enable_osc, ISP_OSC, _ext_clock)
#define pm_enable_osc_crystal     ATPASTE3(pm_enable_osc, ISP_OSC, _crystal)
#define pm_enable_clk_no_wait     ATPASTE3(pm_enable_clk, ISP_OSC, _no_wait)
#define pm_disable_clk            ATPASTE2(pm_disable_clk, ISP_OSC)


//_____ D E C L A R A T I O N S ____________________________________________

/*! \brief Wait 10ms active
 */
extern void wait_10_ms(void);

/*! \brief Setup the system clocks
 */
extern void sys_clk_gen_start(void);

/*! \brief Undo the setup of the system clocks (restore the same clock settings
 *  as after a POR.
 */
extern void sys_clk_gen_stop(void);


#endif  // _ISP_H_
