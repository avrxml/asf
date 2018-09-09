/**
* \file
*
* \brief Serial Input & Output configuration
*
* Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef CONF_SIO2HOST_H_INCLUDED
#define CONF_SIO2HOST_H_INCLUDED
#define SERIAL_RX_BUF_SIZE_HOST    156

#warning \
                "Using a default value. Edit this conf_sio2host.h file to modify that define value according to the current board."
/* ! \name Configuration for Xmega */
/* ! @{ */
#if (XMEGA)
#define USART_HOST                &USARTC0
#define USART_HOST_BAUDRATE       9600
#define USART_HOST_CHAR_LENGTH    USART_CHSIZE_8BIT_gc
#define USART_HOST_PARITY         USART_PMODE_DISABLED_gc
#define USART_HOST_STOP_BITS      false

#define USART_HOST_RX_ISR_ENABLE() usart_set_rx_interrupt_level(USART_HOST, \
                                USART_INT_LVL_HI)
#define USART_HOST_ISR_VECT()      ISR(USARTC0_RXC_vect)
#endif /* XMEGA */
/* ! @} */

/* ! \name Configuration for MegaRF */
/* ! @{ */
#if (MEGA_RF)
#define USART_HOST                &USARTA1
#define USART_HOST_BAUDRATE       9600
#define USART_HOST_CHAR_LENGTH    USART_CHSIZE_8BIT_gc
#define USART_HOST_PARITY         USART_PMODE_DISABLED_gc
#define USART_HOST_STOP_BITS      false

#define USART_HOST_RX_ISR_ENABLE() usart_rx_complete_interrupt_enable(USART_HOST)
#define USART_HOST_ISR_VECT()      ISR(USART1_RX_vect)
#endif /* MEGA_RF */
/* ! @} */

/* ! \name Configuration for UC3 */
/* ! @{ */
#if (UC3)
#define USART_HOST                &AVR32_USART0
#define USART_HOST_BAUDRATE       9600
#define USART_HOST_CHAR_LENGTH    8
#define USART_HOST_PARITY         USART_NO_PARITY
#define USART_HOST_STOP_BITS      USART_1_STOPBIT

#define USART_HOST_RX_ISR_ENABLE()
#define USART_HOST_ISR_VECT()     ISR(host_uart_isr, 2, 1)
#endif /* UC3 */
/* ! @} */

/* ! \name Configuration for SAM4L */
/* ! @{ */
#if (SAM && !(SAMD || SAMR21 || SAML21 || SAMR30))

#if SAMG55
                /* Configuration for console uart IRQ handler */
                #define USART_HOST_ISR_VECT()      ISR(FLEXCOM7_Handler)
                /* UART Host IRQ Number */
                #define USART_HOST_IRQn            FLEXCOM7_IRQn        
#else
                /* Configuration for console uart IRQ handler */
                #define USART_HOST_ISR_VECT()      ISR(USART0_Handler)
                /* UART Host IRQ Number */
                #define USART_HOST_IRQn            USART0_IRQn
#endif

                #define USART_HOST                 USART0
                /* / ** Baudrate setting * / */
                #define USART_HOST_BAUDRATE        9600
                /* / ** Character length setting * / */
                #define USART_HOST_CHAR_LENGTH     US_MR_CHRL_8_BIT
                /* / ** Parity setting * / */
                #define USART_HOST_PARITY          US_MR_PAR_NO
                /* / ** Stop bits setting * / */
                #define USART_HOST_STOP_BITS       US_MR_NBSTOP_1_BIT
                
#define USART_HOST_RX_ISR_ENABLE() usart_enable_interrupt(USART_HOST, \
                                US_IER_RXRDY); \
                NVIC_EnableIRQ(USART_HOST_IRQn);
#endif /* SAM */

/* ! \name Configuration for SAMD20 */
/* ! @{ */
#if (SAMD || SAMR21 || SAML21 || SAMR30)
#if (SAMR30E || SAMR21E)
#define USART_HOST                 SERCOM0
#define HOST_SERCOM_MUX_SETTING    USART_RX_1_TX_0_XCK_1
#define HOST_SERCOM_PINMUX_PAD0    PINMUX_UNUSED
#define HOST_SERCOM_PINMUX_PAD1    PINMUX_UNUSED
#define HOST_SERCOM_PINMUX_PAD2    PINMUX_PA06D_SERCOM0_PAD2
#define HOST_SERCOM_PINMUX_PAD3    PINMUX_PA07D_SERCOM0_PAD3
/** Baudrate setting */
#define USART_HOST_BAUDRATE        9600

#define USART_HOST_RX_ISR_ENABLE()  _sercom_set_handler(0, USART_HOST_ISR_VECT); \
USART_HOST->USART.INTENSET.reg = SERCOM_USART_INTFLAG_RXC; \
system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_SERCOM0);
#else
#define USART_HOST                 SERCOM0
#define HOST_SERCOM_MUX_SETTING    USART_RX_1_TX_0_XCK_1
#define HOST_SERCOM_PINMUX_PAD0    PINMUX_PA04D_SERCOM0_PAD0
#define HOST_SERCOM_PINMUX_PAD1    PINMUX_PA05D_SERCOM0_PAD1
#define HOST_SERCOM_PINMUX_PAD2    PINMUX_UNUSED
#define HOST_SERCOM_PINMUX_PAD3    PINMUX_UNUSED
/** Baudrate setting */
#define USART_HOST_BAUDRATE        9600

#define USART_HOST_RX_ISR_ENABLE()  _sercom_set_handler(0, USART_HOST_ISR_VECT); \
USART_HOST->USART.INTENSET.reg = SERCOM_USART_INTFLAG_RXC; \
system_interrupt_enable(SYSTEM_INTERRUPT_MODULE_SERCOM0);
#endif /* SAMD || SAMR21 || SAML21 */

#endif

/* ! @} */
#endif /* CONF_SIO2HOST_H_INCLUDED */

