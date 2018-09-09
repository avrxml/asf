/**
 * \file
 *
 * \brief AT45DBX configuration file.
 *
 * This file contains the possible external configuration of the AT45DBX.
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
 */

#ifndef CONF_AT45DBX_H_INCLUDED
#define CONF_AT45DBX_H_INCLUDED

#include "board.h"

//! Connect AT45DBx driver to spi master service
#define AT45DBX_USES_SPI_MASTER_SERVICE

//! Select the SPI module AT45DBX is connected to
#define AT45DBX_SPI_MODULE          SPI0

//! Size of AT45DBX data flash memories to manage.
#define AT45DBX_MEM_SIZE            AT45DBX_500KB

//! Number of AT45DBX components to manage.
#define AT45DBX_MEM_CNT             1

//! SPI master speed in Hz.
#define AT45DBX_SPI_MASTER_SPEED    1000000

//! First chip select used by AT45DBX components on the SPI module instance.
#define AT45DBX_CS                  0

//! Number of bits in each SPI transfer.
#define AT45DBX_SPI_BITS            8

#endif  /* CONF_AT45DBX_H_INCLUDED */
