/**
 * \file
 *
 * \brief XMEGA E Quadrature decoder test signal generation header file. It
 *        generates a quadrature encoded signal.
 *        This file contains the function prototypes and Macros.
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
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef GENERATOR_QENC_H
#define GENERATOR_QENC_H

/**
 * \brief Ggenerates a quadrature encoder signals on port pins
 *
 * \param port       The port used to generate signals
 * \param pins_base  First pin number to be used for QDPH0 input from port
 *                   pins_base+1 for QDPH90 and pins_base+2 for QDINDX
 * \param revolution Number of position for a revolution
 * \param freq       The frequency of generated signal (mHz)
 * \param dir        The direction of generated signal
 */
void generator_qenc_enable(PORT_t *port, uint8_t pins_base,
		volatile void *timer, uint8_t revolution, uint32_t freq, bool dir);

/**
 * \brief Changes the speed of quadrature encoder generator
 *
 * \param freq       The frequency of generated signal (mHz)
 */
void generator_qenc_set_freq(uint32_t freq);

/**
 * \brief Changes the direction of quadrature encoder generator
 *
 * \param dir        The direction of generated signal
 */
void generator_qenc_set_direction(bool dir);

#endif /* GENERATOR_QENC_H */
