/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GUI_H_
#define GUI_H_

#include "pm.h"
#include <stdio.h>
#include <util/console.h>
#include <stdint.h>

#ifdef GUI_COMPAT
typedef cmd_state_t (*button_cb_t)();
#else
typedef void (*button_cb_t)(void);
#endif

typedef void (*getstring_cb) (char *str, size_t len, void* ctx);

#ifdef WITH_GUI

void gui_init(const pm_freq_param_t *pm_freq_param);
void gui_draw(int force_draw);
void gui_exec(uint32_t ticks);
void gui_set_title(const char *str, unsigned char line);
int gui_set_button(short id, const char *label, size_t len, button_cb_t cb);
void gui_clear_scroll_box(void);
void gui_inc_scroll_cursor(void);
void gui_dec_scroll_cursor(void);
int gui_scroll_idx(void);
void gui_add_scroll_box_item(const char *str, size_t len);
int gui_get_scroll_box_focused_item(char **str, size_t *len);
void gui_del_scroll_box_item(const char *str, size_t len);
void gui_set_bg_color(short color);
void gui_getstring(getstring_cb cb, void *ctx);
void gui_getstring_onoff(int state);
void gui_save_buttons(void);
void gui_restore_buttons(void);

void gui_clear_infobox(short color);
void gui_add_infobox_item(const char *str, int line);
void gui_infobox_onoff(int state);
int gui_infobox_state(void);
void gui_display_pic(void *pic, size_t len);

#else
 #define gui_display_pic(a,b)
 #define gui_infobox_state()
 #define gui_add_infobox_item(a,b)
 #define gui_infobox_onoff(a)
 #define gui_clear_infobox(a)
#endif

#endif /* GUI_H_ */
