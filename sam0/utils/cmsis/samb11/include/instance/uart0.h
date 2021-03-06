/**
 * \file
 *
 * \brief Instance description for UART0
 *
 * Copyright (c) 2015-2018 Microchip Technology Inc. and its subsidiaries.
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

#ifndef _SAMB11_UART0_INSTANCE_
#define _SAMB11_UART0_INSTANCE_

/* ========== Register definition for UART0 peripheral ========== */
#if (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__))

#define REG_UART0_TRANSMIT_DATA (0x40004000U) /**< (UART0) Writes one byte to UART Transmit Data FIFO.  */
#define REG_UART0_TRANSMIT_STATUS (0x40004004U) /**< (UART0) Status of the UART transmitter. Each field can generate an interrupt if corresponding bit in  the Tx interrupt mask register is set.  */
#define REG_UART0_TX_INTERRUPT_MASK (0x40004008U) /**< (UART0) Enable or Disable the generation of interrupts by the tx_status register.  */
#define REG_UART0_RECEIVE_DATA  (0x40004010U) /**< (UART0) Read one byte from UART Receive Data FIFO.  */
#define REG_UART0_RECEIVE_STATUS (0x40004014U) /**< (UART0) Status of the UART receiver. Each field can generate an interrupt if corresponding bit in  the Rx interrupt mask register is set.  */
#define REG_UART0_RX_INTERRUPT_MASK (0x40004018U) /**< (UART0) Enable or Disable the generation of interrupts by the rx_status register.  */
#define REG_UART0_RECEIVE_TIMEOUT (0x4000401CU) /**< (UART0) Timeout counter configuration.  */
#define REG_UART0_CONFIGURATION (0x40004020U) /**< (UART0) UART Operation Configuration Register, for both Rx and Tx.  */
#define REG_UART0_BAUD_RATE     (0x40004024U) /**< (UART0) Baud Rate Control Register. Bits 15:3 specify the integral division of the clock (divide by n),  and bit 2:0 specify the fractional division.  */
#define REG_UART0_CLOCK_SOURCE  (0x40004028U) /**< (UART0) Selects Source of UART Clock  */

#else

#define REG_UART0_TRANSMIT_DATA (*(__O  uint8_t*)0x40004000U) /**< (UART0) Writes one byte to UART Transmit Data FIFO.  */
#define REG_UART0_TRANSMIT_STATUS (*(__I  uint8_t*)0x40004004U) /**< (UART0) Status of the UART transmitter. Each field can generate an interrupt if corresponding bit in  the Tx interrupt mask register is set.  */
#define REG_UART0_TX_INTERRUPT_MASK (*(__IO uint8_t*)0x40004008U) /**< (UART0) Enable or Disable the generation of interrupts by the tx_status register.  */
#define REG_UART0_RECEIVE_DATA  (*(__I  uint8_t*)0x40004010U) /**< (UART0) Read one byte from UART Receive Data FIFO.  */
#define REG_UART0_RECEIVE_STATUS (*(__I  uint8_t*)0x40004014U) /**< (UART0) Status of the UART receiver. Each field can generate an interrupt if corresponding bit in  the Rx interrupt mask register is set.  */
#define REG_UART0_RX_INTERRUPT_MASK (*(__IO uint8_t*)0x40004018U) /**< (UART0) Enable or Disable the generation of interrupts by the rx_status register.  */
#define REG_UART0_RECEIVE_TIMEOUT (*(__IO uint8_t*)0x4000401CU) /**< (UART0) Timeout counter configuration.  */
#define REG_UART0_CONFIGURATION (*(__IO uint8_t*)0x40004020U) /**< (UART0) UART Operation Configuration Register, for both Rx and Tx.  */
#define REG_UART0_BAUD_RATE     (*(__IO uint16_t*)0x40004024U) /**< (UART0) Baud Rate Control Register. Bits 15:3 specify the integral division of the clock (divide by n),  and bit 2:0 specify the fractional division.  */
#define REG_UART0_CLOCK_SOURCE  (*(__IO uint8_t*)0x40004028U) /**< (UART0) Selects Source of UART Clock  */

#endif /* (defined(__ASSEMBLY__) || defined(__IAR_SYSTEMS_ASM__)) */

#endif /* _SAMB11_UART0_INSTANCE_ */
