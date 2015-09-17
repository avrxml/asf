/**************************************************************************
 *
 * \file
 *
 * \brief Interface of the main file of the UART DFU ISP.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
