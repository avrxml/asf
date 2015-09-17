/**************************************************************************
 *
 * \file
 *
 * \brief Application engine header file.
 *
 * Contains the includes, types and function declarations used by application engine.
 *
 ***************************************************************************/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include "btypes.h"

// Everything from this callback shall be printed on display.
// Strings are null terminated.
typedef void TAppEnginePrintCb( TString textToDisplay );

typedef enum
{
   AppEngine_None,
   AppEngine_Left 	= 0x3C,// <
   AppEngine_Right	= 0x3E,// >
   AppEngine_Up		= 0x2B,// +
   AppEngine_Down	= 0x2D,// -
   AppEngine_Push	= 0x3D // =

} TAppEngineJoystickEnum;
typedef uint8 TAppEngineJoystick;

// Init.
boolean AppEngineInit( TAppEnginePrintCb* pCb, TString deviceName );

// Start inquiry, stop inquiry.
void AppEngineInquiryButton( void );
// Devices are printed on display via TAppEnginePrintCb.
// Last printed device is used for connect.
void AppEngineSelectDeviceButton( void );
// Connect, disconnect.
void AppEngineConnectButton( void );
// Send events to remote device.
void AppEngineJoystickButton( TAppEngineJoystick joystick );
