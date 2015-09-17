/*****************************************************************************
 *
 * \file
 *
 * \brief Control Panel USART driver module.
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
 *****************************************************************************/

/*
  BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR USART.
*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "gpio.h" // Have to include gpio.h before FreeRTOS.h as long as FreeRTOS
                  // redefines the inline keyword to empty.
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Demo application includes. */
#include "serial.h"
#include <avr32/io.h>
#include "board.h"
#include "usart.h"

/*-----------------------------------------------------------*/

/* Constants to setup and access the USART. */
#define serINVALID_COMPORT_HANDLER        ( ( xComPortHandle ) 0 )
#define serINVALID_QUEUE                  ( ( xQueueHandle ) 0 )
#define serHANDLE                         ( ( xComPortHandle ) 1 )
#define serNO_BLOCK                       ( ( portTickType ) 0 )

/*
 * Usart private data.
 */
typedef struct usartPrivateData
{
	volatile avr32_usart_t  *usart;
        xQueueHandle xRxedChars;	/*< Queue used to hold received characters. */
	xQueueHandle xCharsForTx;	/*< Queue used to hold characters waiting to be transmitted. */
} xUsartPrivateData;

xUsartPrivateData xUsart0 = {&AVR32_USART0, NULL, NULL}; // Usart0 data.
xUsartPrivateData xUsart1 = {&AVR32_USART1, NULL, NULL}; // Usart1 data.

/*-----------------------------------------------------------*/

/* Queues used to hold received characters, and characters waiting to be
transmitted. */
/* static xQueueHandle xRxedChars;
static xQueueHandle xCharsForTx;
*/

/*-----------------------------------------------------------*/

/* Forward declaration. */
static int iprvSerialCreateQueues( unsigned portBASE_TYPE uxRxQueueLength,
        xQueueHandle *pxRxedChars, unsigned portBASE_TYPE uxTxQueueLength,
        xQueueHandle *pxCharsForTx );

/*-----------------------------------------------------------*/

#if defined(__GNUC__)
__attribute__((__noinline__))
#elif defined(__ICCAVR32__)
#pragma optimize = no_inline
#endif
static portBASE_TYPE prvUSART_ISR_NonNakedBehaviour( xUsartPrivateData *pxUsart )
{
  /* Now we can declare the local variables. */
  signed portCHAR     cChar;
  portBASE_TYPE     xHigherPriorityTaskWoken = pdFALSE;
  unsigned portLONG     ulStatus;
  portBASE_TYPE retstatus;

  /* What caused the interrupt? */
  ulStatus = pxUsart->usart->csr & pxUsart->usart->imr;

  if (ulStatus & AVR32_USART_CSR_TXRDY_MASK)
  {
    /* The interrupt was caused by the THR becoming empty.  Are there any
       more characters to transmit? */
    /* Because FreeRTOS is not supposed to run with nested interrupts, put all OS
      calls in a critical section . */
    portENTER_CRITICAL();
    retstatus = xQueueReceiveFromISR(pxUsart->xCharsForTx, &cChar, &xHigherPriorityTaskWoken);
    portEXIT_CRITICAL();
    if (retstatus == pdTRUE)
    {
      /* A character was retrieved from the queue so can be sent to the
         THR now. */
      pxUsart->usart->thr = cChar;
    }
    else
    {
      /* Queue empty, nothing to send so turn off the Tx interrupt. */
      pxUsart->usart->idr = AVR32_USART_IDR_TXRDY_MASK;
    }
  }

  if (ulStatus & AVR32_USART_CSR_RXRDY_MASK)
  {
    /* The interrupt was caused by the receiver getting data. */
    cChar = pxUsart->usart->rhr; //TODO

    /* Because FreeRTOS is not supposed to run with nested interrupts, put all OS
      calls in a critical section . */
    portENTER_CRITICAL();
    xQueueSendFromISR(pxUsart->xRxedChars, &cChar, &xHigherPriorityTaskWoken);
    portEXIT_CRITICAL();
  }

  /* The return value will be used by portEXIT_SWITCHING_ISR() to know if it
  should perform a vTaskSwitchContext(). */
  return ( xHigherPriorityTaskWoken );
}


/*
 * USART0 interrupt service routine.
 */
#if defined(__GNUC__)
__attribute__((__naked__))
#elif defined(__ICCAVR32__)
#pragma shadow_registers = full   // Naked.
#endif
void vUSART0_ISR( void )
{

 /* This ISR can cause a context switch, so the first statement must be a
  call to the portENTER_SWITCHING_ISR() macro.  This must be BEFORE any
  variable declarations. */
  portENTER_SWITCHING_ISR();
  prvUSART_ISR_NonNakedBehaviour(&xUsart0);
 /* Exit the ISR.  If a task was woken by either a character being received
  or transmitted then a context switch will occur. */
  portEXIT_SWITCHING_ISR();
}

/*
 * USART1 interrupt service routine.
 */
#if defined(__GNUC__)
__attribute__((__naked__))
#elif defined(__ICCAVR32__)
#pragma shadow_registers = full   // Naked.
#endif
static void vUSART1_ISR( void )
{

 /* This ISR can cause a context switch, so the first statement must be a
  call to the portENTER_SWITCHING_ISR() macro.  This must be BEFORE any
  variable declarations. */
  portENTER_SWITCHING_ISR();
  prvUSART_ISR_NonNakedBehaviour(&xUsart1);
 /* Exit the ISR.  If a task was woken by either a character being received
  or transmitted then a context switch will occur. */
  portEXIT_SWITCHING_ISR();
}
/*-----------------------------------------------------------*/

/*!
 * \brief Init the serial port.
 * \param UsartId The identifier of the Usart to init.
 * \param ulWantedBaud The required baudrate.
 * \param uxRxQueueLength The length of the Rx buffer (if 0, rx is not supported).
 * \param uxTxQueueLength The length of the Tx buffer (if 0, tx is not supported).
 * \return xComPortHandle Handler on the COM port.
 */
xComPortHandle xUsartInit( eCOMPort UsartId, unsigned portLONG ulWantedBaud,
                           unsigned portBASE_TYPE uxRxQueueLength,
                           unsigned portBASE_TYPE uxTxQueueLength)
{
  xComPortHandle    xReturn;
  xUsartPrivateData *pxUsart;
  int               UsartRxEnMask = ((uxRxQueueLength==0) ? 0 : AVR32_USART_CR_RXEN_MASK);
  int               UsartTxEnMask = ((uxTxQueueLength==0) ? 0 : AVR32_USART_CR_TXEN_MASK);
  int               iTempoStatus;
  // USART options.
  usart_options_t USART_OPTIONS =
  {
    .baudrate     = 57600,
    .charlength   = 8,
    .paritytype   = USART_NO_PARITY,
    .stopbits     = USART_1_STOPBIT,
    .channelmode  = USART_NORMAL_CHMODE
  };

  USART_OPTIONS.baudrate = ulWantedBaud;

  xReturn = pxUsart = (UsartId == serCOM1 ? &xUsart0 : &xUsart1);

  /* Create the rx and tx queues. */
  iTempoStatus =  iprvSerialCreateQueues( uxRxQueueLength, &(pxUsart->xRxedChars),
                          uxTxQueueLength, &(pxUsart->xCharsForTx) );

  /* Configure USART. */
  if( ( iTempoStatus != pdFAIL ) &&
      ( ulWantedBaud != ( unsigned portLONG ) 0 ) )
  {
    portENTER_CRITICAL();
    {
      /**
       ** Configure USART.
       **/
      /* Enable USART RXD & TXD pins. */
      if(UsartId == serCOM1)
      {
        if(uxRxQueueLength)
          gpio_enable_module_pin(AVR32_USART0_RXD_0_0_PIN, AVR32_USART0_RXD_0_0_FUNCTION);
        if(uxTxQueueLength)
          gpio_enable_module_pin(AVR32_USART0_TXD_0_0_PIN, AVR32_USART0_TXD_0_0_FUNCTION);
      }
      else
      {
        if(uxRxQueueLength)
          gpio_enable_module_pin(AVR32_USART1_RXD_0_0_PIN, AVR32_USART1_RXD_0_0_FUNCTION);
        if(uxTxQueueLength)
          gpio_enable_module_pin(AVR32_USART1_TXD_0_0_PIN, AVR32_USART1_TXD_0_0_FUNCTION);
      }

      // Initialize USART in RS232 mode.
      usart_init_rs232(pxUsart->usart, &USART_OPTIONS, CP_PBA_SPEED);

      /* We're not fully done yet: disable receiver and transmitter. */
      pxUsart->usart->cr |= AVR32_USART_CR_RXDIS_MASK | AVR32_USART_CR_TXDIS_MASK;

      // Register the USART interrupt handler to the interrupt controller and
      // enable the USART interrupt.
      if(UsartId == serCOM1)
      	INTC_register_interrupt((__int_handler)&vUSART0_ISR, AVR32_USART0_IRQ, AVR32_INTC_INT1);
      else
      	INTC_register_interrupt((__int_handler)&vUSART1_ISR, AVR32_USART1_IRQ, AVR32_INTC_INT1);

      /* Enable USART interrupt sources (but not Tx for now)... */
      if(uxRxQueueLength)
        pxUsart->usart->ier = AVR32_USART_IER_RXRDY_MASK;

      /* Enable receiver and transmitter... */
      pxUsart->usart->cr |= UsartTxEnMask | UsartRxEnMask;
    }
    portEXIT_CRITICAL();
  }
  else
  {
    xReturn = serINVALID_COMPORT_HANDLER;
  }

  return xReturn;
}


/*-----------------------------------------------------------*/

/*!
 * \brief Get char from Usart.
 * \param pxPort The Usart handle to get the char from
 * \param pcRxedChar The rxed char(output)
 * \param xBlockTime The max time to wait for a rxed char
 */
signed portBASE_TYPE xUsartGetChar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
  xUsartPrivateData *pxUsart = (xUsartPrivateData *)pxPort;

  /* Get the next character from the buffer.  Return false if no characters
  are available, or arrive before xBlockTime expires. */
  if( xQueueReceive( pxUsart->xRxedChars, pcRxedChar, xBlockTime ) )
  {
    return pdTRUE;
  }
  else
  {
    return pdFALSE;
  }
}

/*-----------------------------------------------------------*/

/*!
 * \brief Put char to Usart.
 * \param pxPort The Usart handle to put the char to
 * \param cOutChar The char to transmit
 * \param xBlockTime The max time to wait for getting the right to transmit the char.
 */
signed portBASE_TYPE xUsartPutChar( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime )
{
xUsartPrivateData *pxUsart = (xUsartPrivateData *)pxPort;

  /* Place the character in the queue of characters to be transmitted. */
  if( xQueueSend( pxUsart->xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
  {
    return pdFAIL;
  }

  /* Turn on the Tx interrupt so the ISR will remove the character from the
  queue and send it.   This does not need to be in a critical section as
  if the interrupt has already removed the character the next interrupt
  will simply turn off the Tx interrupt again. */
  pxUsart->usart->ier = (1 << AVR32_USART_IER_TXRDY_OFFSET);

  return pdPASS;
}

/*-----------------------------------------------------------*/

unsigned portSHORT usUsartPutString( xComPortHandle pxPort, const signed portCHAR * const pcString, unsigned portSHORT usStringLength )
{
signed portCHAR *pxNext;

  /* Send each character in the string, one at a time. */
  pxNext = ( signed portCHAR * ) pcString;
  do
  {
    if(xUsartPutChar( pxPort, *pxNext, serNO_BLOCK ) != pdPASS)
      break; // The queue is full.
    pxNext++;
  } while( --usStringLength );

  return( usStringLength ); // Return the number of remaining characters.
}

/*-----------------------------------------------------------*/

/*!
 * \brief Close the serial port
 * \param xPort The handle of the usart to close
 */
void vSerialClose( xComPortHandle xPort )
{
  xUsartPrivateData *pxUsart = (xUsartPrivateData *)xPort;
  /* Disable all USART interrupt sources */
  pxUsart->usart->idr = 0xFFFFFFFF;
}
/*-----------------------------------------------------------*/

/*###########################################################*/

/*
 * Create the rx and tx queues.
 */
static int iprvSerialCreateQueues( unsigned portBASE_TYPE uxRxQueueLength, xQueueHandle *pxRxedChars,
                                    unsigned portBASE_TYPE uxTxQueueLength, xQueueHandle *pxCharsForTx )
{
  int iRet = pdPASS;

  /* Create the queues used to hold Rx and Tx characters. */
  // NOTE: xQueueCreate() will return NULL if the required length is 0.
  *pxRxedChars = xQueueCreate( uxRxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
  *pxCharsForTx = xQueueCreate( uxTxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
  if( ( ( uxRxQueueLength ) && (*pxRxedChars == NULL) ) ||
      ( ( uxTxQueueLength ) && (*pxCharsForTx == NULL) ) )
    iRet = pdFAIL;

  return(iRet);
}
/*-----------------------------------------------------------*/
