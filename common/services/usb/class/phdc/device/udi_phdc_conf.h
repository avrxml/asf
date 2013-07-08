/**
 * \file
 *
 * \brief Default PHDC configuration for a USB Device with a single interface
 *
 * Copyright (c) 2009-2012 Atmel Corporation. All rights reserved.
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

#ifndef _UDI_PHDC_CONF_H_
#define _UDI_PHDC_CONF_H_

/**
 * \addtogroup udi_phdc_group_single_desc
 * @{
 */

/* ! Control endpoint size */
#define  USB_DEVICE_EP_CTRL_SIZE       32 /* 8 is not supported by PHDC */

/* ! Endpoint numbers used by PHDC interface */
#define  UDI_PHDC_EP_BULK_OUT          (1 | USB_EP_DIR_OUT)
#define  UDI_PHDC_EP_BULK_IN           (2 | USB_EP_DIR_IN)
#define  UDI_PHDC_EP_INTERRUPT_IN      (3 | USB_EP_DIR_IN)

/* ! Endpoint sizes used by PHDC interface */
#define  UDI_PHDC_EP_SIZE_BULK_OUT     32
#define  UDI_PHDC_EP_SIZE_BULK_IN      32
#define  UDI_PHDC_EP_SIZE_INT_IN       8

/* ! Interface number */
#define  UDI_PHDC_IFACE_NUMBER          0

/**
 * \name UDD Configuration
 * @{
 */
/* ! 2 or 3 endpoints used by PHDC interface */
#if ((UDI_PHDC_QOS_IN & USB_PHDC_QOS_LOW_GOOD) == USB_PHDC_QOS_LOW_GOOD)
#define  USB_DEVICE_MAX_EP             3
#else
#define  USB_DEVICE_MAX_EP             2
#endif
/* @} */

/* @} */

#include "udi_phdc.h"

#endif /* _UDI_PHDC_CONF_H_ */
