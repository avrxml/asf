/**
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
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef __GUI_H
#define __GUI_H

#include <windows.h>

#define GUI_TYPE_IMAGE     1
#define GUI_TYPE_BUTTON    2
#define GUI_TYPE_STATIC    3
#define GUI_TYPE_EDIT      4
#define GUI_TYPE_COMBOBOX  5
#define GUI_TYPE_RICHEDIT  6
#define GUI_TYPE_PAINT     7

// Add an entry in the GUI
#define GUI_TAB_ADD_ENTRY(gui_layout, new_x, new_y, new_width, new_height, new_value, new_type, _new_str, new_style, _function_proc)  {  gui_layout._gui_item[gui_layout.nb_elements].type = new_type; \
                                                                                                                           gui_layout._gui_item[gui_layout.nb_elements].x = new_x; \
                                                                                                                           gui_layout._gui_item[gui_layout.nb_elements].y = new_y; \
                                                                                                                           gui_layout._gui_item[gui_layout.nb_elements].width = new_width; \
                                                                                                                           gui_layout._gui_item[gui_layout.nb_elements].height = new_height; \
                                                                                                                           gui_layout._gui_item[gui_layout.nb_elements].value = new_value; \
                                                                                                                           gui_layout._gui_item[gui_layout.nb_elements].style = new_style; \
                                                                                                                           strncpy(gui_layout._gui_item[gui_layout.nb_elements]._str, _new_str, 20); \
                                                                                                                           gui_layout._gui_item[gui_layout.nb_elements]._f_proc = (obj_proc) _function_proc; \
                                                                                                                           gui_layout.nb_elements++; \
                                                                                                                           }
#define GUI_FOREGROUND_COLOR_STATIC     RGB(255, 255, 255)
#define GUI_BACKGROUND_COLOR_STATIC     BACKGROUND_COLOR
#define GUI_FOREGROUND_COLOR_EDIT       RGB(0, 0, 0)
#define GUI_BACKGROUND_COLOR_EDIT       RGB(255, 255, 255)
#define GUI_FOREGROUND_COLOR_COMBOBOX   RGB(255, 255, 255)
#define GUI_BACKGROUND_COLOR_COMBOBOX   BACKGROUND_COLOR
#define GUI_FOREGROUND_COLOR_SCROLLBAR  RGB(255, 255, 255)
#define GUI_BACKGROUND_COLOR_SCROLLBAR  BACKGROUND_COLOR

typedef LRESULT CALLBACK (*obj_proc)(HWND, UINT, WPARAM, LPARAM);

// Item structure
typedef struct
{
        HWND hwnd;
        int value, value2, value3;
        int id;
        int type;
        char _str[32];
        obj_proc _f_proc;
        int x, y, width, height;
        UINT style;
        BOOL actif;
        COLORREF foreground_color;
        COLORREF background_color;
}s_gui_tab_layout_item;

// GUI layout structure
typedef struct
{
        HWND hwnd;
        int nb_elements;
        s_gui_tab_layout_item *_gui_item;
        float scale_x, scale_y;
}s_gui_tab_layout;

// To allocate the resources for the creation of a new GUI
void gui_new_tab_layout(s_gui_tab_layout *_gui_layout, int max_elements);
// Delete a previously created GUI
void gui_delete_tab_layout(s_gui_tab_layout *_gui_layout);
// Create a GUI
void gui_create(s_gui_tab_layout *_gui_layout, HWND hwnd);
// The main procedure that dispatch messages to sub procedures
LRESULT CALLBACK gui_proc(s_gui_tab_layout *_gui_layout, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
// Change the visibility of a GUI
void gui_visibility(s_gui_tab_layout *_gui_layout, BOOL visible);
// Change the visibility of an item of a GUI
void gui_visibility_item(s_gui_tab_layout_item *_gui_item, BOOL visible);
// Rescale the dimensions of a GUI
void gui_rescale(s_gui_tab_layout *_gui_layout, float scale_x, float scale_y, BOOL repaint);
// Move a GUI
void gui_move(s_gui_tab_layout *_gui_layout, int x, int y);
// Change the font of a GUI
void gui_set_font(s_gui_tab_layout_item *_gui_item, char *_name, int height, BOOL bolt, BOOL italic, BOOL underlined);

#endif
