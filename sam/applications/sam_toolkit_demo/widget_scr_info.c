/**
 * \file
 *
 * \brief Information widget for SAM toolkit demo applicaiton.
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */
 
#include "demo.h"
#include <asf.h>
#include "string.h"
#include "bitmaps.h"

/**
 * \brief Command event IDs for information widget.
 */
enum info_command_id {
	/** Dummy icon */
	ICON_DUMMY_ID = 0,
	/** Command event ID for information icon button */
	ICON_INFO_ID,
	/** Dummy ID */
	ICON_INFO_DUMMY_ID,
};

/**
 * \brief Information type.
 */
enum info_type {
	/** Information type of board name */
	INFO_BOARD_NAME = 0,
	/** Information type of version number */
	INFO_VER_NUM,
	/** Max information type */
	INFO_MAX,
};

/** Infomation widget context */
struct widget_context_info {
	/** Pointer to frame for application */
	struct wtk_basic_frame *frame;
	/** Frame background object */
	struct gfx_bitmap frame_bg;
	/* Group of icon button widgets */
	struct wtk_icon_group *icon_group;
	/* Array if icon button widgets */
	struct wtk_icon_button *icons;
	/** Label widget for user selection feedback */
	struct wtk_label *lbl_indication[INFO_MAX];
	/** Pointer to frame for application */
	struct wtk_basic_frame *logo_frame;
	/** Label widget for user selection feedback */
	struct wtk_label *lbl_board_name;
	/** Label widget for user selection feedback */
	struct wtk_label *lbl_func_name;
};

static char demo_version_number[] = DEMO_VERSION;
static char demo_func_name[] = "Demo Info";

/** Statically allocated context pointer */
static struct widget_context_info *widget_ctx_info;

/** Frame background bitmap */
static struct gfx_bitmap win_info_logo_icon = {
	.width = 87, .height=40,
	.type = GFX_BITMAP_EXT,
	.data.custom = DEMO_BMP_LOGO_ATMEL
};
/** Frame background bitmap */
static struct gfx_bitmap win_info_ret_icon = {
	.width = 48, .height=48,
	.type = GFX_BITMAP_EXT,
	.data.custom = DEMO_BMP_BTN_BACK
};

/**
 * \brief Frame handler for the information widget.
 *
 * Handles all command events from the information widgets in the frame.
 *
 * \param frame Pointer to the application frame
 * \param command_data Command event ID
 *
 * \return True if exiting, to destroy the window
 */
static bool widget_info_command_handler(struct wtk_basic_frame *frame,
		win_command_t command_data)
{
	char command;
	command = (uintptr_t)command_data;

	/* Remove warnings */
	UNUSED(frame);

	switch (command) {
	case ICON_INFO_ID:
		/* Turn off information widget and turn on the main widget */
		app_widget_info_on(false);
		app_widget_main_on(true);
		break;
	}
	return false;
}

/**
 * \brief Setup information widget.
 *
 * Allocates memory for the application context, and creates all widgets that
 * make up its interface. If memory allocation or widget creation fails, the
 * application exits immediately.
 */
static void app_widget_launch_info(void)
{
	struct win_window *parent;
	struct win_area area;

	/* Create a new context for the GUI, clear new context state */
	widget_ctx_info = membag_alloc(sizeof(struct widget_context_info));

	/* Set the background information for the GUI window */
	widget_ctx_info->frame_bg.type = GFX_BITMAP_SOLID;
	widget_ctx_info->frame_bg.data.color = GFX_COLOR(255, 255, 255);

	/* Set the area for the GUI window */
	area = win_get_attributes(win_get_root())->area;

	/* Create and show the main GUI frame */
	widget_ctx_info->frame = wtk_basic_frame_create(
			win_get_root(), &area, &widget_ctx_info->frame_bg, NULL,
			widget_info_command_handler, widget_ctx_info);
	if (!widget_ctx_info->frame) {
		goto error_frame;
	}

	parent = wtk_basic_frame_as_child(widget_ctx_info->frame);
	win_show(parent);

	/* Set positions for logo frame */
	area.pos.x = 0;
	area.pos.y = 0;
	area.size.x = 87;
	area.size.y = 40;

	/* Create logo frame */
	widget_ctx_info->logo_frame = wtk_basic_frame_create(
			parent, &area, &win_info_logo_icon, NULL,
			NULL, NULL);
	if (!widget_ctx_info->frame) {
		goto error_frame;
	}

	win_show(wtk_basic_frame_as_child(widget_ctx_info->logo_frame));

	/* Set area for the label widget of board name */
	area.pos.x = 100;
	area.pos.y = 0;
	wtk_label_size_hint(&area.size, demo_board_name);

	/* Create label inside frame */
	widget_ctx_info->lbl_board_name = wtk_label_create(
			parent, &area, demo_board_name,
			GFX_COLOR(0, 0, 0), NULL, false);
	if (!widget_ctx_info->lbl_board_name) {
		goto error_widget;
	}

	win_show(wtk_label_as_child(widget_ctx_info->lbl_board_name));

	/* Set area for the label widget of function name */
	area.pos.x = 100;
	area.pos.y = 20;
	wtk_label_size_hint(&area.size, demo_func_name);

	/* Create label inside frame */
	widget_ctx_info->lbl_func_name = wtk_label_create(
			parent, &area, demo_func_name,
			GFX_COLOR(0, 0, 0), NULL, false);
	if (!widget_ctx_info->lbl_func_name) {
		goto error_widget;
	}

	win_show(wtk_label_as_child(widget_ctx_info->lbl_func_name));

	/* Create a logical group for the icon buttons */
	widget_ctx_info->icon_group = wtk_icon_group_create();
	if (!widget_ctx_info->icon_group) {
		goto error_widget;
	}

	/* Set area for the board indication */
	area.pos.x = 100;
	area.pos.y = 100;
	wtk_label_size_hint(&area.size, demo_board_name);

	/* Create label inside frame */
	widget_ctx_info->lbl_indication[INFO_BOARD_NAME] = wtk_label_create(
			parent, &area, demo_board_name,
			GFX_COLOR(0, 0, 0), NULL, false);
	if (!widget_ctx_info->lbl_indication[INFO_BOARD_NAME]) {
		goto error_widget;
	}

	win_show(wtk_label_as_child(widget_ctx_info->lbl_indication[INFO_BOARD_NAME]));

	/* Set area for the version number indication */
	area.pos.x += 20;
	area.pos.y += 20;
	wtk_label_size_hint(&area.size, demo_version_number);

	/* Create label inside frame */
	widget_ctx_info->lbl_indication[INFO_VER_NUM] = wtk_label_create(
			parent, &area, demo_version_number,
			GFX_COLOR(0, 0, 0), NULL, false);
	if (!widget_ctx_info->lbl_indication[INFO_VER_NUM]) {
		goto error_widget;
	}

	win_show(wtk_label_as_child(widget_ctx_info->lbl_indication[INFO_VER_NUM]));

	/* Set the area for the return icon to create */
	area.pos.x = 12;
	area.pos.y = 220;
	wtk_icon_button_size_hint(&area.size, &win_info_ret_icon);

	/* Create the return icon button inside frame */
	widget_ctx_info->icons = wtk_icon_button_create(parent, &area,
			&win_info_ret_icon, false, widget_ctx_info->icon_group,
			(win_command_t)(ICON_INFO_ID));
	if (!widget_ctx_info->icons) {
		goto error_widget;
	}

	win_show(wtk_icon_button_as_child(widget_ctx_info->icons));

	return;

	/* Error handling to clean up allocations after an error */
error_widget:
	win_destroy(wtk_basic_frame_as_child(widget_ctx_info->frame));
error_frame:
	membag_free(widget_ctx_info);
}

/**
 * \brief Enable/disable information widget.
 *
 * \param on Turn on/off information widget.
 */
void app_widget_info_on(uint8_t on)
{
	if (on) {
		app_widget_launch_info();
	} else {
		if (widget_ctx_info->frame) {
			win_destroy(wtk_basic_frame_as_child(widget_ctx_info->frame));
		}
	}

}
