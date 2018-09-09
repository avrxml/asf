/*****************************************************************************
 *
 * \file
 *
 * \brief FreeRTOS Serial Port management example for AVR32 UC3.
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
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
 *****************************************************************************/


/*
  BASIC INTERRUPT DRIVEN SERIAL PORT DRIVER FOR USART.
*/

#include <avr32/io.h>
#include "board.h"
#include "gpio.h"
#include "usart.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

/* Demo application includes. */
#include "serial.h"
#include "conf_example.h"

/*-----------------------------------------------------------*/

/* Constants to setup and access the USART. */
#define serINVALID_COMPORT_HANDLER        ( ( xComPortHandle ) 0 )
#define serINVALID_QUEUE                  ( ( xQueueHandle ) 0 )
#define serHANDLE                         ( ( xComPortHandle ) 1 )
#define serNO_BLOCK                       ( ( portTickType ) 0 )

/*-----------------------------------------------------------*/

/* Queues used to hold received characters, and characters waiting to be
transmitted. */
static xQueueHandle xRxedChars;
static xQueueHandle xCharsForTx;

/*-----------------------------------------------------------*/

/* Forward declaration. */
static void vprvSerialCreateQueues( unsigned portBASE_TYPE uxQueueLength,
									xQueueHandle *pxRxedChars,
									xQueueHandle *pxCharsForTx );

/*-----------------------------------------------------------*/

#if __GNUC__
	__attribute__((__noinline__))
#elif __ICCAVR32__
	#pragma optimize = no_inline
#endif

static portBASE_TYPE prvUSART_ISR_NonNakedBehaviour( void )
{
	/* Now we can declare the local variables. */
	signed portCHAR     cChar;
	portBASE_TYPE     xHigherPriorityTaskWoken = pdFALSE;
	unsigned portLONG     ulStatus;
	volatile avr32_usart_t  *usart = serialPORT_USART;
	portBASE_TYPE retstatus;

	/* What caused the interrupt? */
	ulStatus = usart->csr & usart->imr;

	if (ulStatus & AVR32_USART_CSR_TXRDY_MASK)
	{
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit?
		Because FreeRTOS is not supposed to run with nested interrupts, put all OS
		calls in a critical section . */
		portENTER_CRITICAL();
			retstatus = xQueueReceiveFromISR( xCharsForTx, &cChar, &xHigherPriorityTaskWoken );
		portEXIT_CRITICAL();

		if (retstatus == pdTRUE)
		{
			/* A character was retrieved from the queue so can be sent to the
			 THR now. */
			usart->thr = cChar;
		}
		else
		{
			/* Queue empty, nothing to send so turn off the Tx interrupt. */
			usart->idr = AVR32_USART_IDR_TXRDY_MASK;
		}
	}

	if (ulStatus & AVR32_USART_CSR_RXRDY_MASK)
	{
		/* The interrupt was caused by the receiver getting data. */
		cChar = usart->rhr; //TODO

		/* Because FreeRTOS is not supposed to run with nested interrupts, put all OS
		calls in a critical section . */
		portENTER_CRITICAL();
			xQueueSendFromISR(xRxedChars, &cChar, &xHigherPriorityTaskWoken);
		portEXIT_CRITICAL();
	}

	/* Using data memory barriers in the IRQ handler to make sure that:
	- the request has been disasserted, and that
	- the (potential) write in the idr has been done 
	... before returning from the handler */
	usart->csr;

	/* The return value will be used by portEXIT_SWITCHING_ISR() to know if it
	should perform a vTaskSwitchContext(). */
	return ( xHigherPriorityTaskWoken );
}
/*-----------------------------------------------------------*/

/*
 * USART interrupt service routine.
 */
#if __GNUC__
	__attribute__((__naked__))
#elif __ICCAVR32__
	#pragma shadow_registers = full   // Naked.
#endif

static void vUSART_ISR( void )
{
	/* This ISR can cause a context switch, so the first statement must be a
	call to the portENTER_SWITCHING_ISR() macro.  This must be BEFORE any
	variable declarations. */
	portENTER_SWITCHING_ISR();

	prvUSART_ISR_NonNakedBehaviour();

	/* Exit the ISR.  If a task was woken by either a character being received
	or transmitted then a context switch will occur. */
	portEXIT_SWITCHING_ISR();
}
/*-----------------------------------------------------------*/


/*
 * Init the serial port for the Minimal implementation.
 */
xComPortHandle xSerialPortInitMinimal( unsigned portLONG ulWantedBaud, unsigned portBASE_TYPE uxQueueLength )
{
static const gpio_map_t USART_GPIO_MAP =
{
	{ serialPORT_USART_RX_PIN, serialPORT_USART_RX_FUNCTION },
	{ serialPORT_USART_TX_PIN, serialPORT_USART_TX_FUNCTION }
};

xComPortHandle    xReturn = serHANDLE;
volatile avr32_usart_t  *usart = serialPORT_USART;

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

	/* Create the rx and tx queues. */
	vprvSerialCreateQueues( uxQueueLength, &xRxedChars, &xCharsForTx );

	/* Configure USART. */
	if( ( xRxedChars != serINVALID_QUEUE ) &&
	  ( xCharsForTx != serINVALID_QUEUE ) &&
	  ( ulWantedBaud != ( unsigned portLONG ) 0 ) )
	{
		portENTER_CRITICAL();
		{
			/**
			** Configure USART.
			**/
			/* Enable USART RXD & TXD pins. */
			gpio_enable_module( USART_GPIO_MAP, sizeof( USART_GPIO_MAP ) / sizeof( USART_GPIO_MAP[0] ) );

			// Initialize USART in RS232 mode.
			usart_init_rs232(usart, &USART_OPTIONS, configPBA_CLOCK_HZ);

			/* We're not fully done yet: disable receiver and transmitter. */
			usart->cr |= AVR32_USART_CR_RXDIS_MASK | AVR32_USART_CR_TXDIS_MASK;

			/* Register the USART interrupt handler to the interrupt controller and
			 enable the USART interrupt. */
			INTC_register_interrupt((__int_handler)&vUSART_ISR, serialPORT_USART_IRQ, AVR32_INTC_INT1);

			/* Enable USART interrupt sources (but not Tx for now)... */
			usart->ier = AVR32_USART_IER_RXRDY_MASK;

			/* Enable receiver and transmitter... */
			usart->cr |= AVR32_USART_CR_TXEN_MASK | AVR32_USART_CR_RXEN_MASK;
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

signed portBASE_TYPE xSerialGetChar( xComPortHandle pxPort, signed portCHAR *pcRxedChar, portTickType xBlockTime )
{
	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;

	/* Get the next character from the buffer.  Return false if no characters
	are available, or arrive before xBlockTime expires. */
	if( xQueueReceive( xRxedChars, pcRxedChar, xBlockTime ) )
	{
		return pdTRUE;
	}
	else
	{
		return pdFALSE;
	}
}
/*-----------------------------------------------------------*/

void vSerialPutString( xComPortHandle pxPort, const signed portCHAR * const pcString, unsigned portSHORT usStringLength )
{
signed portCHAR *pxNext;

	/* NOTE: This implementation does not handle the queue being full as no
	block time is used! */

	/* The port handle is not required as this driver only supports UART0. */
	( void ) pxPort;

	/* Send each character in the string, one at a time. */
	pxNext = ( signed portCHAR * ) pcString;
	while( *pxNext )
	{
		xSerialPutChar( pxPort, *pxNext, serNO_BLOCK );
		pxNext++;
	}
}
/*-----------------------------------------------------------*/

signed portBASE_TYPE xSerialPutChar( xComPortHandle pxPort, signed portCHAR cOutChar, portTickType xBlockTime )
{
volatile avr32_usart_t  *usart = serialPORT_USART;

	/* Place the character in the queue of characters to be transmitted. */
	if( xQueueSend( xCharsForTx, &cOutChar, xBlockTime ) != pdPASS )
	{
		return pdFAIL;
	}

	/* Turn on the Tx interrupt so the ISR will remove the character from the
	queue and send it.   This does not need to be in a critical section as
	if the interrupt has already removed the character the next interrupt
	will simply turn off the Tx interrupt again. */
	usart->ier = (1 << AVR32_USART_IER_TXRDY_OFFSET);

	return pdPASS;
}
/*-----------------------------------------------------------*/

void vSerialClose( xComPortHandle xPort )
{
  /* Not supported as not required by the demo application. */
}
/*-----------------------------------------------------------*/

/*###########################################################*/

/*
 * Create the rx and tx queues.
 */
static void vprvSerialCreateQueues(  unsigned portBASE_TYPE uxQueueLength, xQueueHandle *pxRxedChars, xQueueHandle *pxCharsForTx )
{
	/* Create the queues used to hold Rx and Tx characters. */
	xRxedChars = xQueueCreate( uxQueueLength, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xCharsForTx = xQueueCreate( uxQueueLength + 1, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );

	/* Pass back a reference to the queues so the serial API file can
	post/receive characters. */
	*pxRxedChars = xRxedChars;
	*pxCharsForTx = xCharsForTx;
}
/*-----------------------------------------------------------*/
