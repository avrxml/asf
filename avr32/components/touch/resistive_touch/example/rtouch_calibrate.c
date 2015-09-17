/*****************************************************************************
 *
 * \file
 *
 * \brief Resistive Touch Calibration
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
/*============================ INCLUDES ======================================*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
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
