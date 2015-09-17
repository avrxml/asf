/**
 *
 * Copyright (c) 2009-2015 Atmel Corporation. All rights reserved.
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

#include <stdio.h>
#include <assert.h>
#include <windows.h>
#include <richedit.h>

#include "gui.h"
#include "main.h"
#include "error_management.h"

BOOL gui_paint(s_gui_tab_layout *_gui_layout, HDC hdc);
int gui_get_item_w_coord(s_gui_tab_layout *_gui_layout, int *x, int *y);
s_gui_tab_layout_item *gui_is_hwnd(s_gui_tab_layout *_gui_layout, HWND hwnd);
void get_path(char *_path, int size);

void get_path(char *_path, int size)
{
  char *_c;

  _c = _path + GetModuleFileName(NULL, _path, size);
  while(*_c != '\\' && _c > _path)
      _c--;
  if (_c == _path)
       SET_ERROR("Unable to find module path.");

  _c[1] = '\0';
}

void gui_set_font(s_gui_tab_layout_item *_gui_item, char *_name, int height, BOOL bolt, BOOL italic, BOOL underlined)
{
     HFONT hfont, hfont_old;
     LOGFONT lf;

     ASSERT(_gui_item);

     memset(&lf, 0, sizeof(LOGFONT));
     strcpy(lf.lfFaceName, _name);
     lf.lfHeight = height;
     if (bolt)
        lf.lfWeight = FW_BLACK;
     lf.lfItalic = italic;
     lf.lfUnderline = underlined;

     hfont_old = (HFONT) SendMessage(_gui_item->hwnd, WM_GETFONT, 0, 0);
     hfont = CreateFontIndirect(&lf);
     SendMessage(_gui_item->hwnd, WM_SETFONT, (WPARAM) hfont, MAKELPARAM(TRUE, 0));
     if (hfont_old)
         DeleteObject(hfont_old);
}


void gui_rescale(s_gui_tab_layout *_gui_layout, float scale_x, float scale_y, BOOL repaint)
{
     int i, offset_x, offset_y;
     s_gui_tab_layout_item *_gui_item;
     HFONT hfont, hfont_old;
     LOGFONT lf;
     int flags;

     ASSERT(_gui_layout);

     memset(&lf, 0, sizeof(LOGFONT));
     strcpy(lf.lfFaceName, "Arial");

     if (repaint)
         flags = 0;
     else
         flags = SWP_NOREDRAW;

     if (scale_x < 0 || scale_y < 0)
        flags |= SWP_NOMOVE | SWP_NOSIZE;

     _gui_layout->scale_x = scale_x;
     _gui_layout->scale_y = scale_y;

     for(i=0; i<_gui_layout->nb_elements; i++)
     {
           _gui_item = &_gui_layout->_gui_item[i];
           ASSERT(_gui_item);
           offset_x = 0;
           offset_y = 0;

           if (_gui_item->type == GUI_TYPE_COMBOBOX)
               offset_y = (20*(scale_y-1))/2;

           SetWindowPos(
                _gui_item->hwnd,
                NULL,
                _gui_item->x*scale_x+offset_x,
                _gui_item->y*scale_y+offset_y,
                _gui_item->width*scale_x,
                _gui_item->height*scale_y,
                flags | SWP_NOZORDER);

           if (_gui_item->type != GUI_TYPE_COMBOBOX && !(_gui_item->style & ES_MULTILINE))
           {
              lf.lfHeight = _gui_item->height*scale_y-2;
              lf.lfWeight = 800;
              hfont_old = (HFONT) SendDlgItemMessage(_gui_layout->hwnd, _gui_item->id, WM_GETFONT, 0, 0);
              hfont = CreateFontIndirect(&lf);
              SendDlgItemMessage(_gui_layout->hwnd, _gui_item->id, WM_SETFONT, (WPARAM) hfont, MAKELPARAM(TRUE, 0));
              if (hfont_old)
                 DeleteObject(hfont_old);
           }
     }
}

void gui_new_tab_layout(s_gui_tab_layout *_gui_layout, int max_elements)
{
     int i;

     ASSERT(_gui_layout);

     _gui_layout->_gui_item = (s_gui_tab_layout_item *) malloc(sizeof(s_gui_tab_layout_item)*max_elements);
     Assert(_gui_layout->_gui_item);
     memset(_gui_layout->_gui_item, 0, sizeof(s_gui_tab_layout_item)*max_elements);
     _gui_layout->hwnd = NULL;
     _gui_layout->nb_elements = 0;
     for(i=0; i<max_elements; i++)
         _gui_layout->_gui_item[i].actif = FALSE;
}

void gui_delete_tab_layout(s_gui_tab_layout *_gui_layout)
{
     int i;

     ASSERT(_gui_layout);

     for(i=0; i<_gui_layout->nb_elements; i++)
         DestroyWindow(_gui_layout->_gui_item[i].hwnd);
     free(_gui_layout->_gui_item);
}

void gui_create(s_gui_tab_layout *_gui_layout, HWND hwnd)
{
     int i;
     static int id = 1;
     s_gui_tab_layout_item *_gui_item;
     HANDLE handle;
     char _path[MAX_PATH];

     ASSERT(_gui_layout);

     _gui_layout->hwnd = hwnd;
     _gui_layout->scale_x = 1.0;
     _gui_layout->scale_y = 1.0;
     for(i=0; i<_gui_layout->nb_elements; i++)
     {
                 _gui_item = &_gui_layout->_gui_item[i];
                 switch(_gui_item->type)
                 {
                 case GUI_TYPE_IMAGE:
                      _gui_item->style |= WS_CHILD | WS_VISIBLE | SS_BITMAP;
                      _gui_item->hwnd = CreateWindowEx(
                              SS_BITMAP,
                              "STATIC",
                              _gui_item->_str,
                              _gui_item->style,
                              _gui_item->x,
                              _gui_item->y,
                              _gui_item->width,
                              _gui_item->height,
                              hwnd,
                              (HMENU) id++,
                              (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
                              NULL);
                      ASSERT(_gui_item->hwnd);
                      _gui_item->id = GetDlgCtrlID(_gui_item->hwnd);
                      get_path(_path, MAX_PATH);
                      strcat(_path, "images\\");
                      strcat(_path, _gui_item->_str);
                      handle = (HANDLE) LoadImage(NULL,
                             _path,
                             IMAGE_BITMAP,
                             0,
                             0,
                             LR_LOADFROMFILE | LR_DEFAULTSIZE);
                      ASSERT(handle);
                      SendMessage(_gui_item->hwnd, STM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) handle);
                      break;
                 case GUI_TYPE_BUTTON:
                      _gui_item->style |= WS_BORDER | WS_CHILD | WS_VISIBLE | WS_TABSTOP;
                      _gui_item->hwnd = CreateWindowEx(
                              0,
                              "BUTTON",
                              _gui_item->_str,
                              _gui_item->style,
                              _gui_item->x,
                              _gui_item->y,
                              _gui_item->width,
                              _gui_item->height,
                              hwnd,
                              (HMENU) id++,
                              (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
                              NULL);
                      ASSERT(_gui_item->hwnd);
                      _gui_item->id = GetDlgCtrlID(_gui_item->hwnd);
                      break;
                 case GUI_TYPE_PAINT:
                      break;
                 case GUI_TYPE_STATIC:
                      _gui_item->style |= WS_CHILD | WS_VISIBLE;
                      _gui_item->hwnd = CreateWindowEx(
                              0,
                              "STATIC",
                              _gui_item->_str,
                              _gui_item->style,
                              _gui_item->x,
                              _gui_item->y,
                              _gui_item->width,
                              _gui_item->height,
                              hwnd,
                              (HMENU) id++,
                              (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
                              NULL);
                      ASSERT(_gui_item->hwnd);
                      _gui_item->id = GetDlgCtrlID(_gui_item->hwnd);
                      break;
                 case GUI_TYPE_EDIT:
                     _gui_item->style |= WS_CHILD | WS_VISIBLE | WS_TABSTOP;
                     _gui_item->hwnd = CreateWindowEx(
                              0,
                              "EDIT",
                              _gui_item->_str,
                              _gui_item->style | ES_MULTILINE,
                              _gui_item->x,
                              _gui_item->y,
                              _gui_item->width,
                              _gui_item->height,
                              hwnd,
                              (HMENU) id++,
                              (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
                              NULL);
                     ASSERT(_gui_item->hwnd);
                     _gui_item->id = GetDlgCtrlID(_gui_item->hwnd);
                     break;
                 case GUI_TYPE_COMBOBOX:
                     _gui_item->style |= WS_VSCROLL | WS_CHILD | WS_TABSTOP | WS_VISIBLE | CBS_SORT | CBS_DROPDOWN;
                     _gui_item->hwnd = CreateWindowEx(
                              0,
                              "COMBOBOX",
                              _gui_item->_str,
                              _gui_item->style,
                              _gui_item->x,
                              _gui_item->y,
                              _gui_item->width,
                              _gui_item->height,
                              hwnd,
                              (HMENU) id++,
                              (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
                              NULL);
                     ASSERT(_gui_item->hwnd);
                     _gui_item->id = GetDlgCtrlID(_gui_item->hwnd);
                     break;
                 case GUI_TYPE_RICHEDIT:
                     _gui_item->style |= WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER | ES_MULTILINE | WS_HSCROLL | ES_AUTOVSCROLL | ES_NOHIDESEL;
                     ASSERT(LoadLibrary("RICHED32.DLL"));
                     _gui_item->hwnd = CreateWindowEx(
                              WS_EX_CLIENTEDGE,
                              RICHEDIT_CLASS,
                              _gui_item->_str,
                              _gui_item->style,
                              _gui_item->x,
                              _gui_item->y,
                              _gui_item->width,
                              _gui_item->height,
                              hwnd,
                              (HMENU) id++,
                              (HINSTANCE) GetWindowLong(hwnd, GWL_HINSTANCE),
                              NULL);
                     ASSERT(_gui_item->hwnd);
                     _gui_item->id = GetDlgCtrlID(_gui_item->hwnd);
                     break;
                 }
                 _gui_item->actif = TRUE;
     }
}

void gui_move(s_gui_tab_layout *_gui_layout, int x, int y)
{
     int i, inf_x, inf_y;
     s_gui_tab_layout_item *_gui_item;

     ASSERT(_gui_layout);

     inf_x = 0xFFFFFF;
     inf_y = 0xFFFFFF;
     for(i=0; i<_gui_layout->nb_elements; i++)
     {
         _gui_item = &_gui_layout->_gui_item[i];
         inf_x = (inf_x < _gui_item->x)?inf_x:_gui_item->x;
         inf_y = (inf_y < _gui_item->y)?inf_y:_gui_item->y;
     }

     for(i=0; i<_gui_layout->nb_elements; i++)
     {
          _gui_item = &_gui_layout->_gui_item[i];
          _gui_item->x = _gui_item->x-inf_x + x;
          _gui_item->y = _gui_item->y-inf_y + y;
          SetWindowPos(
                _gui_item->hwnd,
                NULL,
                _gui_item->x,
                _gui_item->y,
                0,
                0,
                SWP_NOZORDER | SWP_NOSIZE);
     }
}

void gui_visibility(s_gui_tab_layout *_gui_layout, BOOL visible)
{
     int i;
     UINT uFlags;

     ASSERT(_gui_layout);

     uFlags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOSIZE;
     if (visible)
        uFlags |= SWP_SHOWWINDOW;
     else
        uFlags |= SWP_HIDEWINDOW;

     for(i=0; i<_gui_layout->nb_elements; i++)
     {
         if (visible)
            _gui_layout->_gui_item[i].actif = TRUE;
         else
            _gui_layout->_gui_item[i].actif = FALSE;
          SetWindowPos(_gui_layout->_gui_item[i].hwnd, NULL, 0, 0, 0, 0, uFlags);
     }
}

void gui_visibility_item(s_gui_tab_layout_item *_gui_item, BOOL visible)
{
     UINT uFlags;

     ASSERT(_gui_item);

     uFlags = SWP_NOMOVE | SWP_NOZORDER | SWP_NOSIZE;
     if (visible)
        uFlags |= SWP_SHOWWINDOW;
     else
        uFlags |= SWP_HIDEWINDOW;

     if (visible)
        _gui_item->actif = TRUE;
     else
        _gui_item->actif = FALSE;
     SetWindowPos(_gui_item->hwnd, NULL, 0, 0, 0, 0, uFlags);
}

int gui_get_item_w_coord(s_gui_tab_layout *_gui_layout, int *x, int *y)
{
     int i;
     int new_x, new_y;
     s_gui_tab_layout_item *_gui_item;

     ASSERT(_gui_layout);

     for(i=0; i<_gui_layout->nb_elements; i++)
     {
           _gui_item = &_gui_layout->_gui_item[i];
           ASSERT(_gui_item);
           new_x = *x - _gui_item->x;
           new_y = *y - _gui_item->y;
           if (new_x < 0 || new_y < 0)
              continue;
           if (new_x < _gui_item->width && new_y < _gui_item->height)
           {
              *x = new_x;
              *y = new_y;
              return i;
           }
     }

     return -1;
}

s_gui_tab_layout_item *gui_is_hwnd(s_gui_tab_layout *_gui_layout, HWND hwnd)
{
    int i;

    ASSERT(_gui_layout);

    for(i=0; i<_gui_layout->nb_elements; i++)
        if (_gui_layout->_gui_item[i].hwnd == hwnd)
           return &_gui_layout->_gui_item[i];

    return NULL;
}

LRESULT CALLBACK gui_proc(s_gui_tab_layout *_gui_layout, HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HBRUSH hbrush = NULL;
    LRESULT result;
    HDC hdc;
    PAINTSTRUCT ps;
    int item_id, msg_code;
    int i, x, y;
    s_gui_tab_layout_item *_gui_item;
    COLORREF bg_color, fg_color;

    result = 0;
    switch(message)
    {
    case WM_DESTROY:
        ASSERT(_gui_layout);
        gui_delete_tab_layout(_gui_layout);
        result = 1;
        break;
  case WM_PAINT:
        ASSERT(_gui_layout);
        hdc = BeginPaint(hwnd, &ps);
        result = gui_paint(_gui_layout, hdc);
        EndPaint(hwnd, &ps);
      break;
  case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_RBUTTONDBLCLK:
        ASSERT(_gui_layout);
        x = LOWORD(lParam);
        y = HIWORD(lParam);
        i = gui_get_item_w_coord(_gui_layout, &x, &y);
        if (i == -1)
           return 0;
        ASSERT(i >= 0 && i < _gui_layout->nb_elements);
        _gui_item = &_gui_layout->_gui_item[i];
        ASSERT(_gui_item);
        lParam = x + (y << 16);
        if (_gui_item->_f_proc)
            result = _gui_item->_f_proc(_gui_item->hwnd, message, wParam, lParam);
        break;
    case WM_CTLCOLORSTATIC:
    case WM_CTLCOLOREDIT:
    case WM_CTLCOLORSCROLLBAR:
    case WM_CTLCOLORLISTBOX:
    case WM_CTLCOLORBTN:
        ASSERT(_gui_layout);
        if (!(_gui_item = gui_is_hwnd(_gui_layout, (HWND) lParam)))
           break;
        hdc = (HDC) wParam;
        ASSERT(hdc);
        switch(_gui_item->type)
        {
        case GUI_TYPE_STATIC:
             bg_color = (_gui_item->background_color)?_gui_item->background_color:GUI_BACKGROUND_COLOR_STATIC;
             fg_color = (_gui_item->foreground_color)?_gui_item->foreground_color:GUI_FOREGROUND_COLOR_STATIC;
             break;
        case GUI_TYPE_RICHEDIT:
        case GUI_TYPE_EDIT:
             bg_color = (_gui_item->background_color)?_gui_item->background_color:GUI_BACKGROUND_COLOR_EDIT;
             fg_color = (_gui_item->foreground_color)?_gui_item->foreground_color:GUI_FOREGROUND_COLOR_EDIT;
             break;
        case GUI_TYPE_COMBOBOX:
             bg_color = (_gui_item->background_color)?_gui_item->background_color:GUI_BACKGROUND_COLOR_COMBOBOX;
             fg_color = (_gui_item->foreground_color)?_gui_item->foreground_color:GUI_FOREGROUND_COLOR_COMBOBOX;
             break;
        }

        SetBkColor(hdc, bg_color);
        SetTextColor(hdc, fg_color);
        if (hbrush)
           DeleteObject(hbrush);
        hbrush = CreateSolidBrush(bg_color);
        ASSERT(hbrush);
        SelectObject(hdc, hbrush);
        result = (LRESULT) hbrush;
        break;
  case WM_COMMAND:
        ASSERT(_gui_layout);
        msg_code = (wParam >> 16) & 0xFFFF;
        item_id = wParam & 0xFFFF;

        i = _gui_layout->nb_elements;
        _gui_item = NULL;
        while(i--)
        {
            _gui_item = &_gui_layout->_gui_item[i];
            ASSERT(_gui_item);
            if (_gui_item->id == item_id)
                break;
        }
        if (!_gui_item)
        {
            result = (LRESULT) 0;
            break;
        }

        if (_gui_item->id == item_id)
        {
              if (_gui_item->_f_proc)
                  result = _gui_item->_f_proc(_gui_item->hwnd, msg_code, wParam, lParam);
              else
                  result = (LRESULT) 0;
        }
        else
            result = (LRESULT) 0;
        break;
  default:
      result = (LRESULT) 0;
    }

  return result;
}

BOOL gui_paint(s_gui_tab_layout *_gui_layout, HDC hdc)
{
     HDC hdc_buffer;
     HBITMAP hbmp;
     int w, h;
     int i;
     RECT rect;
     s_gui_tab_layout_item *_gui_item;

     GetUpdateRect(_gui_layout->hwnd, &rect, FALSE);
     for(i=0; i<_gui_layout->nb_elements; i++)
     {
          _gui_item = &_gui_layout->_gui_item[i];
          if (_gui_item->type == GUI_TYPE_PAINT)
          {
               w = _gui_item->width;
               h = _gui_item->height;

               hbmp = CreateCompatibleBitmap(hdc, w, h);
               hdc_buffer = CreateCompatibleDC(hdc);
               SelectObject(hdc_buffer, hbmp);

               _gui_item->_f_proc(_gui_item->hwnd, WM_PAINT, (WPARAM) hdc_buffer, (LPARAM) 0);

               StretchBlt(hdc,
                      _gui_item->x*_gui_layout->scale_x,
                      _gui_item->y*_gui_layout->scale_y,
                      w*_gui_layout->scale_x,
                      h*_gui_layout->scale_y,
                      hdc_buffer,
                      0,
                      0,
                      w,
                      h,
                      SRCCOPY);
               DeleteObject(hbmp);
               DeleteDC(hdc_buffer);
          }
     }
}
