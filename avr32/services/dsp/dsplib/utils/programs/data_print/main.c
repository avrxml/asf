/**
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#include <windows.h>
#include <stdio.h>

#include "main.h"
#include "gui.h"
#include "error_management.h"

#include <richedit.h>

#define ITEM_GRAPH   gui_main_layout._gui_item[1]
#define GRAPH_WIDTH  (XSCREEN-30)
#define GRAPH_HEIGHT (YSCREEN-30)
#define GRAPH_MARGIN  30

#define MAX_DATA      8000 //256
#define DELAY         50

#define MAX(x, y)     (((x) > (y))?(x):(y))

LRESULT CALLBACK WindowProcedure(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK GraphProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void get_data(char *_path);
void get_config(char *_path);

HWND main_hwnd;
s_gui_tab_layout gui_main_layout;
float scale_x, scale_y;
float _data[MAX_DATA];
int cur_i = 0;
int n_data = 0;
int N_ARG;
char *_filename;

// This function is the entry point of the program, it will create the window.
int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
  MSG messages;
  WNDCLASSEX wincl;
  HBRUSH hbrush;
  char szClassName[] = "c_" TITLE;

  memset(&messages, 0, sizeof(MSG));

    // if no argument is passed to this function, return a usage message
    if (!*lpszArgument)
    {
       SET_ERROR("Usage: DataPrinter filename");
       return messages.wParam;
    }

    // Else assumed that the argument is the filename
    _filename = lpszArgument;

    // Create the class that will support this application
  wincl.hInstance = hThisInstance;
  wincl.lpszClassName = szClassName;
  wincl.lpfnWndProc = WindowProcedure;
  wincl.style = CS_DBLCLKS;
  wincl.cbSize = sizeof(WNDCLASSEX);

  wincl.hIcon = LoadIcon(NULL, IDI_APPLICATION);
  wincl.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
  wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
  wincl.lpszMenuName = "MAINMENU";
  wincl.cbClsExtra = 0;
  wincl.cbWndExtra = 0;

    hbrush = CreateSolidBrush(BACKGROUND_COLOR);
  wincl.hbrBackground = (HBRUSH) hbrush;

    // Register this class
  ASSERT(RegisterClassEx(&wincl));

    // Create the window
  main_hwnd = CreateWindowEx(
      0,
      szClassName,
      TITLE,
      WS_OVERLAPPED | WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX,
      CW_USEDEFAULT,
      CW_USEDEFAULT,
      XSCREEN + GetSystemMetrics(SM_CXFIXEDFRAME)*2,
      YSCREEN + GetSystemMetrics(SM_CYFIXEDFRAME)*2 + GetSystemMetrics(SM_CYCAPTION),
      HWND_DESKTOP,
      NULL,
      hThisInstance,
      NULL);

    // Show the window
    ShowWindow(main_hwnd, nFunsterStil);

    // Create the timer that will refresh the graph
    SetTimer(main_hwnd, 0, DELAY, NULL);

    // Message handler loop
    while(GetMessage(&messages, NULL, 0, 0))
    {
      TranslateMessage(&messages);
      DispatchMessage(&messages);
    }

  return messages.wParam;
}

// In order to retrieve data from the file
void get_data(char *_path)
{
     int n;
     FILE *_file;
     char temp[255];
     static int file_pointer = 0;
     float f;

     // Make sure all is set
     ASSERT(_data);
     ASSERT(_path);

     // Open the file
     _file = fopen(_path, "rb");
     if (!_file)
        return;

     fseek(_file, 0, SEEK_END);

     // Get the length of the file
     // If the length has not changed since the last time,
     // quit this function
     if (ftell(_file) == file_pointer)
     {
        fclose(_file);
        return;
     }

     // Else set the file pointer at the previous position
     fseek(_file, file_pointer, SEEK_SET);

     // Read the new information from the file
     while(!feof(_file))
     {
         // Get a line from the file
         fgets(temp, sizeof(temp) - 1, _file);

         // If this line is empty, continue
         if (!*temp)
            continue;

         // Get the first float on this line
         n = sscanf(temp, "%f", &f);
         if (n != 1)
            continue;

         // add this float to a circular buffer
         _data[cur_i++] = f;
         if (cur_i == MAX_DATA)
            cur_i = 0;

         // Set the size of this buffer and saturate it if the maximum size is reached
         if (n_data < MAX_DATA)
            n_data++;
     }

     // Update the file_pointer variable
     file_pointer = ftell(_file);

     // Close the file
     fclose(_file);
}

// The procedure that take care of the display
LRESULT CALLBACK GraphProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    hdc = (HDC) wParam;
    static float max = 0., min = 0.;
    float temp_max, temp_min;
    float cur_data;
    float offset, scale, y;
    float x, inc;
    int i, j;
    int start_i;
    char temp[32];
    HPEN hp2px, hpOld;
    SIZE sz;

    // Draw a white rectangle on the default Device Context
    Rectangle(hdc, 0, 0, GRAPH_WIDTH, GRAPH_HEIGHT);

    // Initialize data
    start_i = cur_i - n_data;
    if (start_i < 0)
       start_i += n_data;
    i = n_data;
    j = start_i;
    temp_max = _data[j];
    temp_min = temp_max;

    // Calculate the minimum and the maximum of the graph
    while(i--)
    {
         cur_data = _data[j++];
         if (temp_max < cur_data)
            temp_max = cur_data;
         if (temp_min > cur_data)
            temp_min = cur_data;
         if (j == n_data)
            j = 0;
    }

    // Update the maximum
    if (max < temp_max)
      max = temp_max;
    else
      max = temp_max + (max - temp_max)*0.9;

    // Update the minimum
    if (min > temp_min)
      min = temp_min;
    else
      min = temp_min - (temp_min - min)*0.9;

    // Set the scaling of the graph
    scale = (GRAPH_HEIGHT-GRAPH_MARGIN*2)/(max-min);
    // Set the offset of the graph
    offset = GRAPH_MARGIN + max*scale;

    // Draw the vertical axis
    MoveToEx(hdc, GRAPH_MARGIN, GRAPH_MARGIN, NULL);
    LineTo(hdc, GRAPH_MARGIN, GRAPH_HEIGHT - GRAPH_MARGIN);

    // Draw the horizontal axis
    MoveToEx(hdc, GRAPH_MARGIN, offset, NULL);
    LineTo(hdc, GRAPH_WIDTH - GRAPH_MARGIN, offset);

    // Gradations
    // Initialization
    inc = ((float) GRAPH_WIDTH - 2*GRAPH_MARGIN)/((float) n_data);
    MoveToEx(hdc, GRAPH_MARGIN, offset - _data[0]*scale, NULL);
    x = GRAPH_MARGIN;
    i = n_data;
    j = start_i;

    // Use a new pen
    hp2px = CreatePen(PS_SOLID, 1, 0xFF0000);
    hpOld = SelectObject(hdc, hp2px);

    // Draw the graph
    while(i--)
    {
        y = offset - _data[j++]*scale;
        LineTo(hdc, (int) x, y);
        x += inc;
        if (j == n_data)
           j = 0;
    }
    // Delete the pen
    DeleteObject(hp2px);

    // Use another pen
    hp2px = CreatePen(PS_DOT, 1, 0xAAAAAA);
    SelectObject(hdc, hp2px);

    // Change the color of the text
    SetTextColor(hdc, 0xAAAAAA);
    SetBkMode(hdc, TRANSPARENT);
    // Draw the gradations
    for(i=-(GRAPH_HEIGHT - GRAPH_MARGIN); i<(GRAPH_HEIGHT - GRAPH_MARGIN); i += (GRAPH_HEIGHT - 2*GRAPH_MARGIN)/5)
    {
       sprintf(temp, "%.1e", (offset-i)/scale);
       GetTextExtentPoint32(hdc, temp, strlen(temp) , &sz);

       TextOut(hdc, 0, i-sz.cy/2, temp, strlen(temp));

       MoveToEx(hdc, MAX(GRAPH_MARGIN, sz.cx) + 2, i, NULL);
       LineTo(hdc, GRAPH_WIDTH - GRAPH_MARGIN, i);
    }

    // Delete the pen
    SelectObject(hdc, hpOld);
    DeleteObject(hp2px);

    return DefWindowProc(hwnd, message, wParam, lParam);
}

// Main function that handles events
LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    LRESULT result;
    RECT rect;

    switch (message)
    {
    // On create event
  case WM_CREATE:
        // Create a 1-object layout
        gui_new_tab_layout(&gui_main_layout, 1);
        // Create one graph object
        GUI_TAB_ADD_ENTRY(gui_main_layout,
             (XSCREEN - GRAPH_WIDTH)/2,
             (XSCREEN - GRAPH_WIDTH)/2,
             GRAPH_WIDTH, GRAPH_HEIGHT,
             0, GUI_TYPE_PAINT, "", 0, GraphProcedure);
        // Create the layout
        gui_create(&gui_main_layout, hwnd);
        break;

    // On timer event
    case WM_TIMER:
        // Read new data
        get_data(_filename);
        // Update the view
        GetClientRect(hwnd, &rect);
        InvalidateRect(hwnd, &rect, FALSE);
        break;

    // On resize event
    case WM_SIZE:
        // Calculate the scale values of the display
        scale_x = ((float) LOWORD(lParam))/((float) XSCREEN);
        scale_y = ((float) HIWORD(lParam))/((float) YSCREEN);
        // Rescale the display
        gui_rescale(&gui_main_layout, scale_x, scale_y, FALSE);
        // Update the view
        GetClientRect(hwnd, &rect);
        InvalidateRect(hwnd, &rect, TRUE);
        break;
    }

    // Switch events to the sub event handler functions
    result = gui_proc(&gui_main_layout, hwnd, message, wParam, lParam);

    switch(message)
    {
    // On destroy event
  case WM_DESTROY:
        // Delete data
        ASSERT(_data);
        free(_data);
        // Quit the application
      PostQuitMessage(0);
      result = 1;
      break;
    }

    if (!result)
       result = DefWindowProc(hwnd, message, wParam, lParam);

    return result;
}
