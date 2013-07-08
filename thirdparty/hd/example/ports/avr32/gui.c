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

#include <ports/avr32/gui.h>

#include "intc.h"
#include "board.h"
#include "pm.h"
#include "gpio.h"
#include <util/printf-stdarg.h>
#include "spi.h"         // Used by touch sensor
#if BOARD == EVK1104 || BOARD == EVK1105
#include "conf_et024006dhu.h"
#include "et024006dhu.h" // Display driver
#endif
#include <util/console.h>
#if WITH_HTTP_GET
#include "bmp_reader.h"
#endif
#include <ports/avr32/board_init.h>
#include <avr32/io.h>
#include <string.h>


#if BOARD == EVK1100
#include "dip204.h"

/* 2=joy up, 3=joy dn, 0= joy lf, 1=joy rt, 4=joy pr */
int button[] = {GPIO_JOYSTICK_LEFT,
                GPIO_JOYSTICK_RIGHT,
                GPIO_JOYSTICK_UP,
                GPIO_JOYSTICK_DOWN,
                GPIO_JOYSTICK_PUSH };

#define BUTTON_PRESSED GPIO_JOYSTICK_DOWN_PRESSED

#elif BOARD == EVK1104
#include "qt60168.h"     // Touch sensor
#include "conf_qt60168.h"

static short idx_to_button_map[] = {
        QT60168_TOUCH_SENSOR_BUTTON_0,
        QT60168_TOUCH_SENSOR_BUTTON_1,
        QT60168_TOUCH_SENSOR_BUTTON_2,
        QT60168_TOUCH_SENSOR_BUTTON_3,
        QT60168_TOUCH_SENSOR_WHEEL_UP};

#elif BOARD == EVK1105
#include "qt1081.h"
int button[] = {QT1081_TOUCH_SENSOR_3,
                QT1081_TOUCH_SENSOR_2,
                QT1081_TOUCH_SENSOR_0,
                QT1081_TOUCH_SENSOR_1,
                QT1081_TOUCH_SENSOR_4 };

#define BUTTON_PRESSED QT1081_TOUCH_SENSOR_0_PRESSED

#endif

__always_inline static int is_pressed(short idx) {
#if BOARD == EVK1104
        return qt60168_is_key_pressed(idx_to_button_map[idx]);
#else
        return (gpio_get_pin_value(button[idx]) == BUTTON_PRESSED);
#endif
}

#define YELLOW 0xFFE0

/* Layout */
#define TITLE_X 0
#define TITLE_Y 0
#define TITLE_W 320
#define TITLE_H 40
#define TITLE_BG_COLOR BLACK
#define TITLE_FG_COLOR WHITE


#define SCROLL_X 0
#define SCROLL_Y 40
#define SCROLL_W 320
#define SCROLL_H 160
#define SCROLL_BG_COLOR WHITE
#define SCROLL_FG_COLOR BLACK
#define CURSOR_BG_COLOR BLUE
#define CURSOR_FG_COLOR WHITE
#define INFO_FG_COLOR WHITE
#define INFO_BG_COLOR BLACK

#define BUTTON_X 0
#define BUTTON_Y 200
#define BUTTON_W 320
#define BUTTON_H 40
#define BUTTON_BG_COLOR BLACK
#define BUTTON_FG_COLOR WHITE


// gui_draw only updates the display if this is set
short mod = 0;
#define SCROLL_MAX_LINES 50
#if BOARD == EVK1100
#define SCROLL_DISP_LINES 3     /* 4 lines, alfanumeric display */
#define MAX_LINE_LEN 20         /* 20 chars */
#else
#define SCROLL_DISP_LINES 11
#define MAX_LINE_LEN 37
#endif

struct {
        int bg_color;
        int cnt;
        int cursor;
        int dispstart;
        char lines[SCROLL_MAX_LINES][MAX_LINE_LEN];
} scroll_box_contents;

enum {
        GUI_LIST,
        GUI_INFOBOX,
        GUI_GETSTRING
} display_mode = GUI_LIST;

//int display_infobox = false;

#if BOARD == EVK1100
#define INFO_MAX_LINES 3
#else
#define INFO_MAX_LINES 12
#endif
struct {
        int bg_color;
        char lines[INFO_MAX_LINES][MAX_LINE_LEN];
} infobox_contents;

struct {
        int width;
        int height;
        char *data;
        size_t data_len;
} info_bitmap;

#define TITLE_MAX_LINES 3

struct {
        char title[TITLE_MAX_LINES][MAX_LINE_LEN];
} title_contents;

#define NUM_BUTTONS 5

struct {
        char labels[NUM_BUTTONS][MAX_LINE_LEN];
        button_cb_t cbs[NUM_BUTTONS];
} button_contents, saved_buttons;

int led_array[4] = {LED0,LED1,LED2,LED3};

/*! \brief Initializes QT60168 resources: GPIO and SPI
 */
#if BOARD == EVK1104
static void qt60168_resources_init(const pm_freq_param_t *pm_freq_param)
{
        static const gpio_map_t QT60168_SPI_GPIO_MAP =
                {
                        {QT60168_SPI_SCK_PIN,          QT60168_SPI_SCK_FUNCTION         },  // SPI Clock.
                        {QT60168_SPI_MISO_PIN,         QT60168_SPI_MISO_FUNCTION        },  // MISO.
                        {QT60168_SPI_MOSI_PIN,         QT60168_SPI_MOSI_FUNCTION        },  // MOSI.
                        {QT60168_SPI_NPCS0_PIN,        QT60168_SPI_NPCS0_FUNCTION}  // Chip Select NPCS.
                };

        // SPI options.
        spi_options_t spiOptions =
                {
                        .reg          = QT60168_SPI_NCPS,
                        .baudrate     = QT60168_SPI_MASTER_SPEED, // Defined in conf_qt60168.h.
                        .bits         = QT60168_SPI_BITS,         // Defined in conf_qt60168.h.
                        .spck_delay   = 0,
                        .trans_delay  = 0,
                        .stay_act     = 0,
                        .spi_mode     = 3,
                        .modfdis      = 1
                };

        // Assign I/Os to SPI.
        gpio_enable_module(QT60168_SPI_GPIO_MAP,
                           sizeof(QT60168_SPI_GPIO_MAP) / sizeof(QT60168_SPI_GPIO_MAP[0]));
#if EXT_BOARD != SPB105
        // Initialize as master.
        spi_initMaster(QT60168_SPI, &spiOptions);

        // Set selection mode: variable_ps, pcs_decode, delay.
        spi_selectionMode(QT60168_SPI, 0, 0, 0);

        // Enable SPI.
        spi_enable(QT60168_SPI);
#endif

        // Initialize QT60168 with SPI clock Osc0.
        spi_setupChipReg(QT60168_SPI, &spiOptions, pm_freq_param->cpu_f);
}
#endif



void gui_init(const pm_freq_param_t *pm_freq_param) {
#if BOARD == EVK1100
        static const gpio_map_t DIP204_SPI_GPIO_MAP =
                {
                        {DIP204_SPI_SCK_PIN,  DIP204_SPI_SCK_FUNCTION },  // SPI Clock.
                        {DIP204_SPI_MISO_PIN, DIP204_SPI_MISO_FUNCTION},  // MISO.
                        {DIP204_SPI_MOSI_PIN, DIP204_SPI_MOSI_FUNCTION},  // MOSI.
                        {DIP204_SPI_NPCS_PIN, DIP204_SPI_NPCS_FUNCTION}   // Chip Select NPCS.
                };
        spi_options_t spiOptions =
                {
                        .reg          = DIP204_SPI_NPCS,
                        .baudrate     = 1000000,
                        .bits         = 8,
                        .spck_delay   = 0,
                        .trans_delay  = 0,
                        .stay_act     = 1,
                        .spi_mode     = 3,
                        .modfdis      = 1
                };
#endif

        memset(&scroll_box_contents, 0, sizeof scroll_box_contents);
        memset(&title_contents, 0, sizeof title_contents);
        memset(&button_contents, 0, sizeof button_contents);
        memset(&infobox_contents, 0, sizeof infobox_contents);
        memset(&info_bitmap, 0, sizeof info_bitmap);
#if BOARD == EVK1104
        // Init touch sensor resources: GPIO, SPI and QT60168.
        qt60168_resources_init(pm_freq_param);
        // Initialize QT60168 component.
        qt60168_init(pm_freq_param->cpu_f);
#endif
#if BOARD == EVK1100
        // Assign I/Os to SPI
        gpio_enable_module(DIP204_SPI_GPIO_MAP,
                           sizeof(DIP204_SPI_GPIO_MAP) / sizeof(DIP204_SPI_GPIO_MAP[0]));
#if 0
        // Initialize as master
        spi_initMaster(DIP204_SPI, &spiOptions);

        // Set selection mode: variable_ps, pcs_decode, delay
        spi_selectionMode(DIP204_SPI, 0, 0, 0);

        // Enable SPI
        spi_enable(DIP204_SPI);
#endif
        // setup chip registers
        spi_setupChipReg(DIP204_SPI, &spiOptions, FOSC0);

        // initialize LCD
        dip204_init(backlight_PWM, true);
        dip204_set_cursor_position(1,1);
        dip204_write_string("http server demo!");
#else
        // Init display
        et024006_Init(  pm_freq_param->cpu_f, pm_freq_param->cpu_f /*HSB*/);
        // Turn on the display backlight
        gpio_set_gpio_pin(ET024006DHU_BL_PIN);
#endif
        mod = 1;
}


void gui_set_title(const char *str, unsigned char line)
{
        int len;

        Assert(line < 3);

        memset(&title_contents.title[line], 0, sizeof title_contents.title[0]);
        len = strlen(str);

        if (len >= sizeof title_contents.title[0]) {
                len = sizeof title_contents.title[0] - 1;
        }
        strncpy(title_contents.title[line], str, len);

        mod = 1;
}

int gui_set_button(short id, const char *label, size_t len, button_cb_t cb)
{

        if (id >= NUM_BUTTONS) {
                return 0;
        }
        if (len >= sizeof button_contents.labels[id]) {
                len = sizeof button_contents.labels[id] - 1;
        }
        button_contents.cbs[id] = cb;
        strncpy(button_contents.labels[id], label, len);
        mod = 1;

        return 1;
}

void gui_clear_scroll_box(void)
{

        memset(&scroll_box_contents, 0, sizeof scroll_box_contents);
        scroll_box_contents.dispstart = 0;
}

void gui_set_bg_color(short color)
{
        scroll_box_contents.bg_color = color;
}

void gui_inc_scroll_cursor(void)
{

#ifdef DEBUG
        printk("inc1: cur=%d, dstrt=%d\n", scroll_box_contents.cursor,
               scroll_box_contents.dispstart);
#endif
        scroll_box_contents.cursor++;
        if (scroll_box_contents.cursor > (scroll_box_contents.dispstart
                                          + SCROLL_DISP_LINES))
        {
                scroll_box_contents.dispstart++;
        }
        if (scroll_box_contents.cursor >= scroll_box_contents.cnt)
        {
                scroll_box_contents.dispstart = 0;
                scroll_box_contents.cursor = 1;
        }
#ifdef DEBUG
        printk("inc2: cur=%d, dstrt=%d\n", scroll_box_contents.cursor,
               scroll_box_contents.dispstart);
#endif
        mod = 1;
}


void gui_dec_scroll_cursor(void)
{
#ifdef DEBUG
        printk("dec1: cur:%d,dstrt=%d\n", scroll_box_contents.cursor,
               scroll_box_contents.dispstart);
#endif
        scroll_box_contents.cursor--;

        if (scroll_box_contents.cursor <= 0)
        {
                scroll_box_contents.dispstart = scroll_box_contents.cnt - SCROLL_DISP_LINES;
                if (scroll_box_contents.dispstart < 0) scroll_box_contents.dispstart = 0;
                scroll_box_contents.cursor = scroll_box_contents.cnt - 1;
        }
        if (scroll_box_contents.cursor < scroll_box_contents.dispstart)
        {
                scroll_box_contents.dispstart--;
        }
#ifdef DEBUG
        printk("dec2: cur:%d,dstrt=%d\n", scroll_box_contents.cursor,
               scroll_box_contents.dispstart);
#endif
        mod = 1;
}

int gui_scroll_idx(void)
{
        return scroll_box_contents.cursor;
}

int gui_get_scroll_box_focused_item(char **str, size_t *len) {
        if (scroll_box_contents.cursor <= 0) {
                return false;
        }
        *str = scroll_box_contents.lines[scroll_box_contents.cursor];
        *len = strlen(*str);

        return true;
}


void gui_add_scroll_box_item(const char *str, size_t len) {

        int cnt = scroll_box_contents.cnt;
        int i;

        if (cnt >= SCROLL_MAX_LINES) return;

        for (i = 0; i < cnt; i++) {
                if (strncmp(str, scroll_box_contents.lines[i], len) == 0) {
                        return;
                }
        }

        if (len >= sizeof scroll_box_contents.lines[cnt]) {
                len = sizeof scroll_box_contents.lines[cnt] - 1;
        }
        strncpy(scroll_box_contents.lines[cnt], str, len);
        scroll_box_contents.cnt++;
        mod = 1;
}



void gui_del_scroll_box_item(const char *str, size_t len) {

        int cnt = scroll_box_contents.cnt;
        int i;
        int moving = 0;

        printk("Delete %s\n", str);
        for (i = 0; i < cnt; i++) {
                if (! moving && strncmp(str, scroll_box_contents.lines[i], len) == 0) {
                        printk("Found it at %d\n", i);
                        /* Now move everything up */
                        moving = 1;
                }
                if (moving) {
                        if (scroll_box_contents.lines[i+1][0] == '\0') {
                                memset(scroll_box_contents.lines[i], 0, sizeof scroll_box_contents.lines[i]);
                                break;
                        }
                        printk("Moving \"%s\" to %i\n", scroll_box_contents.lines[i+1], i);
                        memcpy(scroll_box_contents.lines[i], scroll_box_contents.lines[i+1],
                               strlen(scroll_box_contents.lines[i+1]) + 1);
                        memset(scroll_box_contents.lines[i+1], 0, sizeof scroll_box_contents.lines[i+1]);
                }
        }
        if (i < cnt) {
                scroll_box_contents.cnt--;
        }

        mod = 1;
}


#define BUTTON_SENSITIVITY_TICKS 3

int poll_button(uint32_t tick) {
        static int pressed_button = -1;
        static int press_start_tick = 0;
        static int action_ok = 0;
        static int poll_limit_ts = 0;
        int ret, i;

        /* Cases :
         *  1. Button pressed, but was not previously
         *     Start waiting for the detect time
         *  2. Button pressed, and was previously
         *     Check detect time. If expired, wait for release
         *  3. Button not pressed, but was previously
         *     If waiting for release, reset and return 1
         *     otherwise do nothing.
         */
        if ( pressed_button >= 0 ) { // Button was held previously
                if ( is_pressed(pressed_button) ) { // Button is still held
                        // Has it been held enough time?
                        if ( tick - press_start_tick > BUTTON_SENSITIVITY_TICKS ) { //
                                action_ok = 1;
                        }
                        return -1; // Wait for release
                }
                // Button was pressed but now released. Time for action
                ret = pressed_button;
                pressed_button = -1;
                press_start_tick = 0;
                if (action_ok) {
                        action_ok = 0;
                        return ret;
                }
                // If we switched button it will be detected next polling time
                return -1;
        }
        // If we end up here no button was held previously
        // So find out if one is pressed now
        /* Rate limit polling on the EVK1104 because is_pressed()
         * takes a huge amount of time. */
#if BOARD != EVK1104
        poll_limit_ts = 0;
#endif
        if ( poll_limit_ts == 0 || tick - poll_limit_ts > 100 ) {
                for (i = 0; i < NUM_BUTTONS; i++) {
                        // Check buttons
                        if ( is_pressed(i) )
                        {
                                pressed_button = i;
                                press_start_tick = tick;
                                break;
                        }
                }
                poll_limit_ts = tick;
        }
        return -1;
}

void gui_infobox_onoff(int state)
{
        if (state)
                display_mode = GUI_INFOBOX;
        else
                display_mode = GUI_LIST;
        //  display_infobox = state;
        mod = 1;
}

int gui_infobox_state(void) {
        return (GUI_INFOBOX == display_mode);
}


void gui_clear_infobox(short color)
{
        memset(&infobox_contents, 0, sizeof infobox_contents);
        infobox_contents.bg_color = color;
}

void gui_add_infobox_item(const char *str, int line)
{
        if (line < INFO_MAX_LINES) {
                strncpy(infobox_contents.lines[line], str, sizeof infobox_contents.lines[line]);
                mod = 1;
        }
}

#if WITH_HTTP_GET
void gui_display_pic(void *pic, size_t len)
{
        if (info_bitmap.data) {
                free(info_bitmap.data);
                info_bitmap.data = NULL;
                info_bitmap.data_len = 0;
        }
        if (NULL == pic) {
                return;
        }
        printk("Allocating %d bytes for pic\n", len);
        info_bitmap.data = malloc(len);
        printk("Allocated pointer %p\n", info_bitmap.data);
        if (info_bitmap.data) {
                printk("Copying pic data\n");
                info_bitmap.data_len = bmp_parse(info_bitmap.data,
                                                 pic,
                                                 len,
                                                 &info_bitmap.height,
                                                 &info_bitmap.width);
/*                 info_bitmap.width = 80; */
/*                 info_bitmap.height = 80; */
/*                 info_bitmap.data_len = 80; */
                printk("Parsed width %d, height %d\n",
                       info_bitmap.width,
                       info_bitmap.height);
        }

}

#endif

void gui_display_infobox(void)
{
        int i;

#if BOARD != EVK1100
        // Draw info box
        et024006_DrawFilledRect(SCROLL_X,
                                SCROLL_Y,
                                SCROLL_W,
                                SCROLL_H,
                                infobox_contents.bg_color);

        for (i = 0; i < INFO_MAX_LINES; i++)
        {
                if (strlen(infobox_contents.lines[i]))
                {
                        int x;
                        x = 160 - ((strlen(infobox_contents.lines[i]) / 2)*10);
                        if (x < 0) x = 10;
                        et024006_PrintString(infobox_contents.lines[i],
                                             (const unsigned char*)&FONT8x8,
                                             x,
                                             (i+1)*12+SCROLL_Y,
                                             INFO_FG_COLOR,
                                             infobox_contents.bg_color);
                }
        }
        if (info_bitmap.data && info_bitmap.data_len) {
                printk("Displaying pixmap (size %d)\n", info_bitmap.data_len);
                et024006_PutPixmap( (et024006_color_t *)info_bitmap.data,
                                    info_bitmap.width, // Map width
                                    0,  // map_x
                                    0,  // map_y
                                    40, // x
                                    40, // y
                                    info_bitmap.width, // width
                                    info_bitmap.height); // height
        }
        else {
                printk("Not displaying picture\n");
        }
#else
        dip204_clear_display();
        for (i = 0; i < INFO_MAX_LINES; i++)
        {
                int len = strlen(infobox_contents.lines[i]);
                int col = (MAX_LINE_LEN/2) - (len/2);
                if ( col <= 0) col = 1;
                dip204_set_cursor_position(col,i+1); /* col,line */
                dip204_write_string(infobox_contents.lines[i]);
                dip204_hide_cursor();
        }
#endif
}

void gui_display_getstring(void);

void gui_getstring_onoff(int state)
{
        if (state)
                display_mode = GUI_GETSTRING;
        else
                display_mode = GUI_LIST;
        //  display_infobox = state;
        mod = 1;
}

void gui_save_buttons()
{
        memcpy(&saved_buttons, &button_contents, sizeof button_contents);
}

void gui_restore_buttons()
{
        memcpy(&button_contents, &saved_buttons, sizeof button_contents);
}

void gui_exec(uint32_t tick) {

    int i;
        static int pressed_button;
        static enum { INPUT, RUN } state = INPUT;

        switch (state)
        {
        case INPUT:
        {
                i = poll_button(tick);
                if ( i < 0 ) { // No action required
                        break;
                }
                else {
                        if (button_contents.cbs[i]) {
                                pressed_button = i;
                                state = RUN;
                        }
                }
        }
        break;

        case RUN:
        {
#ifdef GUI_COMPAT
                if (button_contents.cbs[pressed_button]() == CMD_INPROGRESS)
                        return;
#else
                button_contents.cbs[pressed_button]();
#endif
                state = INPUT;
                pressed_button = -1;
        }
        } /* case */
        // Poll gui_getstring
        gui_getstring(NULL, NULL);
        gui_draw(1);
}

void gui_draw(int force_draw) {
        int i;

        if (! mod) {
                return;
        }
        mod = 0;
#if BOARD != EVK1100
        // Draw title box
        et024006_DrawFilledRect(TITLE_X,
                                TITLE_Y,
                                TITLE_W,
                                TITLE_H,
                                TITLE_BG_COLOR);
        if (title_contents.title[0][0]) {
                et024006_PrintString(title_contents.title[0],
                                     (const unsigned char*)&FONT8x8,
                                     TITLE_X + 10,
                                     TITLE_Y + 5,
                                     TITLE_FG_COLOR,
                                     TITLE_BG_COLOR);
                et024006_PrintString(title_contents.title[1],
                                     (const unsigned char*)&FONT8x8,
                                     TITLE_X + 10,
                                     TITLE_Y + 5 + 12,
                                     TITLE_FG_COLOR,
                                     TITLE_BG_COLOR);
                et024006_PrintString(title_contents.title[2],
                                     (const unsigned char*)&FONT8x8,
                                     TITLE_X + 10,
                                     TITLE_Y + 5 + 12 + 12,
                                     TITLE_FG_COLOR,
                                     TITLE_BG_COLOR);

        }


        // Draw scroll box
        et024006_DrawFilledRect(SCROLL_X,
                                SCROLL_Y,
                                SCROLL_W,
                                SCROLL_H,
                                scroll_box_contents.bg_color);

#endif
        switch (display_mode)
        {
        case GUI_INFOBOX:
                gui_display_infobox();
                break;

        case GUI_GETSTRING:
                gui_display_getstring();
                break;

        case GUI_LIST:
        {
#if BOARD == EVK1100
                dip204_clear_display();
#endif
                for (i = 0; i <= SCROLL_DISP_LINES; i++)
                {

#if BOARD == EVK1100

                        dip204_set_cursor_position(1,i+1); /* col,line */

                        dip204_write_string(scroll_box_contents.lines[i+scroll_box_contents.dispstart]);

#else

                        if ((scroll_box_contents.cursor == i + scroll_box_contents.dispstart) &&
                            scroll_box_contents.cursor != 0)
                        {
                                /* Print cursor line. */
                                et024006_PrintString(scroll_box_contents.lines[i+scroll_box_contents.dispstart],
                                                     (const unsigned char*)&FONT8x8,
                                                     SCROLL_X + 10,
                                                     (i+1)*12+SCROLL_Y,
                                                     scroll_box_contents.bg_color,
                                                     CURSOR_BG_COLOR);
                        }
                        else
                        {
                                et024006_PrintString(scroll_box_contents.lines[i+scroll_box_contents.dispstart],
                                                     (const unsigned char*)&FONT8x8,
                                                     SCROLL_X + 10,
                                                     (i+1)*12+SCROLL_Y,
                                                     SCROLL_FG_COLOR,
                                                     scroll_box_contents.bg_color);
                        }
#endif
                }
#if BOARD == EVK1100
                dip204_set_cursor_position(1, scroll_box_contents.cursor - scroll_box_contents.dispstart+1);
                dip204_show_cursor();
#endif
        }
        } /* switch */
        // Draw buttons
#if BOARD != EVK1100
        et024006_DrawFilledRect(BUTTON_X,
                                BUTTON_Y,
                                BUTTON_W,
                                BUTTON_H,
                                BUTTON_BG_COLOR);
        for (i = 0; i < NUM_BUTTONS-1; i++) {
                et024006_DrawVertLine(i*BUTTON_W/NUM_BUTTONS,
                                      BUTTON_Y,
                                      BUTTON_H,
                                      BUTTON_FG_COLOR);
                // Display button labels
                if (button_contents.labels[i]) {
                        et024006_PrintString(button_contents.labels[i],
                                             (const unsigned char*)&FONT8x8,
                                             i*BUTTON_W/NUM_BUTTONS + 5,
                                             BUTTON_Y + 10,
                                             BUTTON_FG_COLOR,
                                             BUTTON_BG_COLOR);
                }
        }
#endif
}


