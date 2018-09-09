/*****************************************************************************
 *
 * \file
 *
 * \brief Resistive Touch Calibration
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
 *****************************************************************************/
/*============================ INCLUDES ======================================*/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <stdint.h>
#include <stdlib.h>
#include "compiler.h"
#include "assert.h"
#include "rtouch.h"
#include "rtouch_calibrate.h"

#include "et024006dhu.h"


/*============================ EXTERNAL DEPENDENCIES =========================*/

//! Compile-time binding to main work queue.
//extern WORKQUEUE_queue_t mainWorkQueue;



/*============================ PRIVATE DEFINES ===============================*/


#define tft_print_string(x, y, string, color) \
	et024006_PrintString((string), ((const unsigned char *)FONT8x8), (x), (y), (color), -1)
#define tft_draw_pixel(x, y, color) \
	et024006_DrawPixel((x), (y), (color))
#define tft_draw_filled_circle(x, y, radius, color, quadrant) \
	et024006_DrawFilledCircle((x), (y), (radius), (color), (quadrant))

#define RTOUCH_COLOR	0x0000
#define TFT_WHOLE	0xff
/*============================ PRIVATE TYPES =================================*/




/*============================ PRIVATE PROTOTYPES ============================*/

//void calibrate( void * data );
void calibratehandler( rtouch_event_t const * event );



/*============================ PRIVATE VARIABLES =============================*/

static rtouch_calibration_points_t points;
static rtouch_calibration_matrix_t matrix;

static int calibstate;





/*============================ IMPLEMENTATION (PUBLIC FUNCTIONS) =============*/

bool APP_Recalibrate( void )
{
	calibstate = 0;
	//WORKQUEUE_item_t item = { calibrate, &calibstate };
	//WORKQUEUE_AddWork( &mainWorkQueue, &item );

	return true;

	// --- Error handling ---
//	return false;
}



/*============================ IMPLEMENTATION (PRIVATE FUNCTIONS) ============*/

void calibratehandler( rtouch_event_t const * event )
{
	//WORKQUEUE_item_t item = { calibrate, &calibstate };
	//WORKQUEUE_AddWork( &mainWorkQueue, &item );
}


bool rtouch_calibrate(void)  //void * data )
{
	//int * state = (int *) data;
	rtouch_event_t event;
	bool in_progress = true;
	static int state = 0;
	static rtouch_event_handler_t oldHandler;

	switch (state) {

	case 0:
		//TFT_SetClipping( 0, 0, TFT_GetWidth(), TFT_GetHeight() );
		//TFT_color_t MEM_RAMTYPE * pixmap;
		//pixmap = MEM_ALLOC_ARRAY( TFT_color_t, (240UL * 320UL) );

		tft_print_string( 10, 10, "Touch screen calibration...",
			RTOUCH_COLOR);

		points.point1.panelX = 30;
		points.point1.panelY = 30;
		points.point2.panelX = 170;
		points.point2.panelY = 220;
		points.point3.panelX = 300;
		points.point3.panelY = 120;
		tft_draw_filled_circle( points.point1.panelX,
			points.point1.panelY, 2, RTOUCH_COLOR, TFT_WHOLE );
		state = 1;

		oldHandler = rtouch_get_event_handler();
		rtouch_set_event_handler( calibratehandler );

		break;

	case 1:
		if (rtouch_is_touched() == false) { break; }
		tft_draw_filled_circle( points.point1.panelX,
			points.point1.panelY, 2, RTOUCH_COLOR, TFT_WHOLE );
		rtouch_get_event(&event);
		points.point1.rawX = event.rawX;
		points.point1.rawY = event.rawY;
		state = 2;

		break;

	case 2:
		if (rtouch_is_touched() == true) { break; }
		tft_draw_filled_circle( points.point2.panelX,
			points.point2.panelY, 2, RTOUCH_COLOR, TFT_WHOLE );
		state = 3;
		break;

	case 3:
		if (rtouch_is_touched() == false) { break; }
		tft_draw_filled_circle( points.point2.panelX,
			points.point2.panelY, 2, RTOUCH_COLOR, TFT_WHOLE );
		rtouch_get_event(&event);
		points.point2.rawX = event.rawX;
		points.point2.rawY = event.rawY;
		state = 4;
		break;

	case 4:
		if (rtouch_is_touched() == true) { break; }
		tft_draw_filled_circle( points.point3.panelX,
			points.point3.panelY, 2, RTOUCH_COLOR, TFT_WHOLE );
		state = 5;
		break;

	case 5:
		if (rtouch_is_touched() == false) { break; }
		tft_draw_filled_circle( points.point3.panelX,
			points.point3.panelY, 2, RTOUCH_COLOR, TFT_WHOLE );
		rtouch_get_event(&event);
		points.point3.rawX = event.rawX;
		points.point3.rawY = event.rawY;
		state = 6;
		break;

	case 6:
		if (rtouch_is_touched() == true) { break; }
		rtouch_compute_calibration_matrix(&points, &matrix);
		rtouch_set_calibration_matrix(&matrix);
		state = 7;

		rtouch_set_event_handler(oldHandler);
		in_progress = false;
		state = 0;
		// TODO redraw the background if needed
		break;

	default:
		break;
	}
	return in_progress;
}




/* EOF */
