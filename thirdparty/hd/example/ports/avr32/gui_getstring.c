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
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <wl_api.h>
#include "intc.h"
#include "board.h"
#include <util/printf-stdarg.h>
#include <util/console.h>
#include <ports/avr32/timer.h>
#include <ports/avr32/gui.h>
#if BOARD == EVK1104 || BOARD == EVK1105
#include "et024006dhu.h"
#endif
#if BOARD == EVK1100
#include "dip204.h"
#endif

#define SCROLL_X 0
#define SCROLL_Y 40
#define SCROLL_W 320
#define SCROLL_H 160
#define SCROLL_BG_COLOR WHITE
#define SCROLL_FG_COLOR BLACK
#define CURSOR_BG_COLOR BLUE
#define CURSOR_FG_COLOR WHITE

#define GS_MAX_ROWS 4

char char_array[][30] =
{
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ",
        "abcdefghijklmnopqrstuvwxyz",
        "0123456789",
        "!\"#$%&'()*+,-./:;<=>?@",
        "XXX" /* <DELETE> <ACCEPT> <ESCAPE>*/
};

char func_row[][16] =
{
        "<BAC>",
        "<DEL>",
        "<ACC>"
};

#if BOARD == EVK1100
#define SCROLL_DISP_LINES 2     /* 4 lines, alfanumeric display, reserve one for edited string*/
#define MAX_LINE_LEN 20         /* 20 chars */
#else
#define SCROLL_DISP_LINES 11
#define MAX_LINE_LEN 37
#endif

struct {
        int ready;                      /* true if <ready> is pressed */
        int escape;                     /* true if <escape> is pressed */
        int rowstart;                   /* start display on this row. For small displays */
        int row;                        /* which row we are on */
        int colstart;                   /* start display on this col. For small displays */
        int col;                        /* which col */
} get_string_data;


extern short mod;
short redisplay = false;

char getstring[MAX_LINE_LEN];
//char gs_key[MAX_KEY_LEN];
int gs_idx = 0;

static void gs_button_0()
{
        /* Left */
        get_string_data.col--;

        if (get_string_data.col < 0) {
                get_string_data.col = strlen(char_array[get_string_data.row]) - 1;
                if (get_string_data.colstart < get_string_data.col - MAX_LINE_LEN + 1) {
                        get_string_data.colstart = get_string_data.col - MAX_LINE_LEN + 1 ;
                        redisplay = true;
                }
        }

        if (get_string_data.col < get_string_data.colstart) {
                get_string_data.colstart--;
                redisplay = true;
        }
        mod = true;
}

static void gs_button_1()
{
        /* Right */
        get_string_data.col++;

        if (get_string_data.col >= strlen(char_array[get_string_data.row])) {
                get_string_data.col = 0;
                if (get_string_data.colstart > 0) {
                        get_string_data.colstart = 0;
                        redisplay = true;
                }
        }

        if (get_string_data.col >= get_string_data.colstart + MAX_LINE_LEN) {
                get_string_data.colstart++;
                redisplay = true;
        }
        mod = true;
}

static void gs_button_2()
{
        /* up */
        get_string_data.row--;

        if (get_string_data.row < 0)
                get_string_data.row = GS_MAX_ROWS;

        if (get_string_data.row < get_string_data.rowstart) {
                get_string_data.rowstart--;
                redisplay = true;
        }

        if (get_string_data.row > get_string_data.rowstart + SCROLL_DISP_LINES) {
                get_string_data.rowstart = get_string_data.row - SCROLL_DISP_LINES;
                redisplay = true;
        }

        if (get_string_data.col >= strlen(char_array[get_string_data.row])) {
                get_string_data.col = strlen(char_array[get_string_data.row]) - 1;
                if (get_string_data.col < MAX_LINE_LEN) {
                        get_string_data.colstart = 0;
                        redisplay = true;
                }
        }

        mod = true;
}

static void gs_button_3()
{
        /* dwn */

        get_string_data.row++;

        if (get_string_data.row > GS_MAX_ROWS) {
                get_string_data.row = 0;
                get_string_data.rowstart = 0;
                redisplay = true;
        }
        if (get_string_data.row > get_string_data.rowstart + SCROLL_DISP_LINES) {
                get_string_data.rowstart = get_string_data.row - SCROLL_DISP_LINES;
                redisplay = true;
        }

        if (get_string_data.col >= strlen(char_array[get_string_data.row])) {
                get_string_data.col = strlen(char_array[get_string_data.row]) - 1;
                if (get_string_data.col < MAX_LINE_LEN) {
                        get_string_data.colstart = 0;
                        redisplay = true;
                }
        }

        mod = true;
}

static void gs_button_4()
{
        /* enter */

        char chr;

        if (get_string_data.row == GS_MAX_ROWS)
        {
                /* Edit function row */
                if (get_string_data.col == 0) /* back */
                {
                        get_string_data.ready = true;
                        get_string_data.escape = true;
                }
                else if (get_string_data.col == 1) /* delete */
                {
                        gs_idx--;
                        if (gs_idx < 0) gs_idx = 0;
                        getstring[gs_idx] = 0;
                        gui_set_title(getstring, 1);
                }
                else if (get_string_data.col == 2) /* accept */
                {
                        get_string_data.ready = true;
                        get_string_data.escape = false;
                }
        }
        else
        {
                /* add current char */
                chr = char_array[get_string_data.row][get_string_data.col];
                getstring[gs_idx] = chr;
                gs_idx++;
                if (gs_idx > sizeof getstring) {
                        gs_idx = sizeof getstring;
                }
                getstring[gs_idx] = 0;
        }
#if BOARD == EVK1100
        dip204_set_cursor_position(1,1); /* col,line */
        dip204_write_string(getstring);
        dip204_set_cursor_position(get_string_data.col + 1 - get_string_data.colstart,
                                   get_string_data.row + 2 - get_string_data.rowstart); /* col,line */
#else
        gui_set_title(getstring, 1);
#endif
        mod = true;
}


void gui_getstring(getstring_cb cb, void *ctx) {
        static enum { IDLE, INIT, POLLING, FINISH } state = IDLE;
        static getstring_cb saved_cb;
        static void *cb_ctx;
        switch (state)
        {
        case IDLE:
                if (cb) {
                        saved_cb = cb;
                        cb_ctx = ctx;
                        state = INIT;
                }
                else {
                        return;
                }
                // Fallthrough
        case INIT: {
                gui_save_buttons();
#if BOARD == EVK1100
                dip204_clear_display();
                redisplay = true;
#endif
                gui_set_title("Enter preshared key", 0);
                gui_set_title(getstring, 1);
                gui_set_button(0, "Left", sizeof "Left",gs_button_0);
                gui_set_button(1, "Right", sizeof "Right", gs_button_1);
                gui_set_button(2, "Up", sizeof "Up", gs_button_2);
                gui_set_button(3, "Down", sizeof "Down", gs_button_3);
                gui_set_button(4, "Enter", sizeof "Enter", gs_button_4);

                get_string_data.col = 0;
                get_string_data.row = 0;
                get_string_data.colstart = 0;
                get_string_data.rowstart = 0;
                get_string_data.ready = false;
                get_string_data.escape = false;

                gui_getstring_onoff(true);
                state = POLLING;
                break;
        }
        case POLLING:
                if (get_string_data.ready) {
                        state = FINISH;
                }
                break;
        case FINISH:
                gui_getstring_onoff(false);
                gui_restore_buttons();
                if (get_string_data.escape)
                        saved_cb(NULL, 0, cb_ctx);
                else
                        saved_cb(getstring, strlen(getstring), cb_ctx);
                saved_cb = NULL;
                cb_ctx = NULL;
                state = IDLE;
                break;
        }

        return;
}



/* This function is called by gui_draw(), when the display is to be updated. */

void gui_display_getstring(void)
{
        int j;
        int i;
#if BOARD != EVK1100
        int fg_color, bg_color;
#endif
        char str[22];

#if BOARD == EVK1100
        if (redisplay) {
                dip204_clear_display();
                for (j = 0; j <= SCROLL_DISP_LINES; j++) {
                        strncpy(str, &(char_array[get_string_data.rowstart+j][get_string_data.colstart]), MAX_LINE_LEN);
                        str[MAX_LINE_LEN] = '\0';
                        dip204_set_cursor_position(1,j+2); /* col,line */
                        dip204_write_string(str);
                }
        }
#else
        for (j = 0; j < GS_MAX_ROWS; j++) {
                for (i = 0; i < strlen(char_array[j]); i++) {
                        if (get_string_data.row == j && get_string_data.col == i) {
                                fg_color = CURSOR_FG_COLOR;
                                bg_color = CURSOR_BG_COLOR;
                        } else {
                                fg_color = SCROLL_FG_COLOR;
                                bg_color = SCROLL_BG_COLOR;
                        }
                        str[0] = char_array[j][i];
                        str[1] = 0;
                        et024006_PrintString(str,
                                             (const unsigned char*)&FONT8x8,
                                             (i+1)*10+SCROLL_X + 10,
                                             (j+1)*12+SCROLL_Y,
                                             fg_color,
                                             bg_color);
                }
        }
#endif

#if BOARD == EVK1100
        if (redisplay){
                if (get_string_data.rowstart == GS_MAX_ROWS - SCROLL_DISP_LINES) {
                        dip204_set_cursor_position(1,1); /* col,line */
                        dip204_write_string("                   ");
                        j = 1;
                        for (i = 0; i <  (sizeof func_row/sizeof func_row[0]); i++) {
                                dip204_set_cursor_position(j,get_string_data.rowstart + SCROLL_DISP_LINES); /* col,line */
                                dip204_write_string(func_row[i]);
                                j += strlen(func_row[i]);
                        }
                }
                redisplay = false;
        }

        dip204_set_cursor_position(1,1); /* col,line */
        dip204_write_string("                   ");
        dip204_set_cursor_position(1,1); /* col,line */
        dip204_write_string(getstring);
        if (get_string_data.row == GS_MAX_ROWS) {
                dip204_set_cursor_position(get_string_data.col * strlen(func_row[0]) + 2, GS_MAX_ROWS); /* col,line */
        } else {
                dip204_set_cursor_position(get_string_data.col + 1 - get_string_data.colstart,
                                           get_string_data.row + 2 - get_string_data.rowstart); /* col,line */
        }
        dip204_show_cursor();
#else
        j = 1;
        for (i = 0; i <  (sizeof func_row/sizeof func_row[0]); i++) {
                if (get_string_data.row == GS_MAX_ROWS && get_string_data.col == i) {
                        fg_color = CURSOR_FG_COLOR;
                        bg_color = CURSOR_BG_COLOR;
                } else {
                        fg_color = SCROLL_FG_COLOR;
                        bg_color = SCROLL_BG_COLOR;
                }
                et024006_PrintString(func_row[i],
                                     (const unsigned char*)&FONT8x8,
                                     j*10 + SCROLL_X + 10,
                                     5*12+SCROLL_Y,
                                     fg_color,
                                     bg_color);
                j += strlen(func_row[i])-1;
        }
#endif
}
