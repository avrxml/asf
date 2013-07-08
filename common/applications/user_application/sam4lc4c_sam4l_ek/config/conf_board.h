/**
 * \file
 *
 * \brief SAM4L-EK board configuration template
 *
 */

#ifndef CONF_BOARD_H_INCLUDED
#define CONF_BOARD_H_INCLUDED

/* Initialize the USART pins for the RS485 interface */
#define CONF_BOARD_RS485

/* Initialize the push button PB0 as an External Interrupt pin */
#define CONF_BOARD_EIC

/* Initialize the LCD Backlight */
#define CONF_BOARD_BL

/* Initialize the SAM4L pins for the USB SAM4L plug */
#define CONF_BOARD_USB_PORT
#define CONF_BOARD_USB_ID_DETECT           /* ID detect NOT enabled,  uncomment it if jumper PB05/USB set */
#define CONF_BOARD_USB_VBUS_CONTROL        /* Host VBUS control enabled,  uncomment it if jumper PC08/USB set */
#define CONF_BOARD_USB_VBUS_ERR_DETECT     /* Host VBUS error detect NOT enabled,  uncomment it if jumper PC07/USB set */

/* Initialize the USART pins for the COM PORT interface with the JLink OB
module that acts as a USB CDC gateway over the USB JLink plug. */
#define CONF_BOARD_COM_PORT

/* Initialize the USART pins that interface with the Board Monitor(BM) */
#define CONF_BOARD_BM_USART

/* Initialize the SPI pins for use with the on-board serial flash or with the
 * WIRELESS connector or with the Sensors Xplained extension board. */
#define CONF_BOARD_SPI

/* Initialize the SPI CS for the WIRELESS connector. */
#define CONF_BOARD_SPI_NPCS0

/* Initialize the SPI CS for the on-board serial flash. */
#define CONF_BOARD_SPI_NPCS2

/* Initialize the SPI CS for the Sensors Xplained extension board. */
#define CONF_BOARD_SPI_NPCS3

/* Initialize the DACC VOUT pin */
#define CONF_BOARD_DACC_VOUT

#endif // CONF_BOARD_H_INCLUDED
