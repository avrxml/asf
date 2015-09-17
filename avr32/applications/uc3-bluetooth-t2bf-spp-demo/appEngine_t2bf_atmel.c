/*
*******************************************************************************
*                     (c) Cybercom Sweden West AB 2009                        *
*******************************************************************************

   File name:     appEngine_t2bf_atmel.c
   Author:        Thomas Bredhammar, Daniel Persson
   Description:   The application engine manages the t2BF platform.
   Compiler:      IAR EWAVR32 for AVR32

*******************************************************************************
*/
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#include <string.h>
#include <stdio.h>

#include "compiler.h"
#include "appEngine_t2bf_atmel.h"

#include "t2bf.h"
#include "time.h"

#include "unistr.h"

boolean AppEngineInternalSendData( uint16 dataLength, uint8* pData );

void AppEngineBtCb( TT2BFBtEvent event, TBdAddr bdAddr, TServiceId id );
void AppEngineSppCb( TT2BFSppEvent event, TPort port );
void AppEngineTimeoutCb( uint32 value );

void AppEngineNotifyCallback( TString textToDisplay );

//Variables for search result
#define T2BFENG_MAX_SEARCH_TIMEOUT 	15
#define T2BFENG_MAX_SEARCH_RESULT 	10
#define T2BFENG_MAX_NAME_LENGTH 	20
typedef uint16 							TMyNameString[ T2BFENG_MAX_NAME_LENGTH ];
static TBdAddr          				pMyBdAddress[ T2BFENG_MAX_SEARCH_RESULT ];
static TT2BFDeviceInfo  				pMyDeviceInfo[ T2BFENG_MAX_SEARCH_RESULT ];
static TMyNameString      				pMyName[ T2BFENG_MAX_SEARCH_RESULT ];

typedef enum
{
   AppEngine_Idle_State,
   AppEngine_Inquiry_State,
   AppEngine_Connecting_State,
   AppEngine_Connected_State,
   AppEngine_Disconnecting_State,

} TAppEngineStateEnum;

TAppEngineStateEnum   	    myState = AppEngine_Idle_State;
TAppEnginePrintCb     	    *pMyCb;
TT2BFStatus 				myStatus;
TBdAddr 					myBdAddr;
TUTF16String 				myName = NULL;
TPort 						mySppPort;
uint16 						myBytes;
uint8 						myBytesBuffer[22];
uint8 						myIndex = 0;

// https://www.bluetooth.org/Technical/AssignedNumbers/baseband.htm
// 0x18 Wearable computer (Watch sized)
// 0x01 Computer (desktop,notebook, PDA, organizers, .... )
// 0x01 Positioning (Location identification)
#define T2BF_LITE_COD    { 0x18, 0x01, 0x01 }

/*! \brief Initializes the Bluetooth functionality.
 *
 * \param pCb        Pointer to callback function that shall receive events from t2BF
 * \param deviceName Sets the user friendly name that are used when this unit is discovered by others.
 *
 * \retval true      Initialization successful.
 * \retval false     Baud rate set point is out of range for the given input clock frequency.
 */
boolean AppEngineInit( TAppEnginePrintCb* pCb, TString deviceName )
{
    uint16 myNameBuf[20];

    myStatus = T2BF_STATUS_OK;
    pMyCb = pCb;
    myName = myNameBuf;

    // The Bluetooth address below is used as default during connect if no other has been selected
    myBdAddr[5] = 0x00;
    myBdAddr[4] = 0x00;
    myBdAddr[3] = 0x00;
    myBdAddr[2] = 0x00;
    myBdAddr[1] = 0x00;
    myBdAddr[0] = 0x00;

    // Convert friendly name to UTF-16
    if ( !UNISTRToUtf16( UNISTR_CHARSET_ASCII, UNISTR_ENCODING_NONE, deviceName, myName, 20 ) )
    {
     	// Invalid name string, set to NULL
     	myName = NULL;
    }

	// BT settings
    TT2BFBtSettings btSettings =
    	{
        false,               /* Means that the Bluetooth address stored locally in the chip shall be used. */
    	{ NULL },            /* No address to write since local address in chip shall be used. */
    	myName,              /* Set this to point at a string to use own device name instead of default name. */
        T2BF_LITE_COD,       /* Sets Class Of Device, used in remote en to classify this unit. Se at definition of T2BF_LITE_COD for details. */
        T2BF_SEC_LEVEL_2,    /* Sets security level. With t2BF SPP Lite only T2BF_SEC_LEVEL_2 is available. This level is also the most common. */
        0x1800,              /* The number of BT slots (0.625 ms)T2BF shall wait before link is considered lost. Advanced users only. */
        0x0000,              /* Maximum sniff BT slots. Set to 0 if sniff is not wanted. Advanced users only. */
        0x0000,              /* Maximum part BT slot. Set to 0 if park is not wanted. Advanced users only. */
        0x0000               /* Maximum hold slots. Set to 0 if hold is not wanted. Advanced users only. */
        };

    AppEngineNotifyCallback( "App init..." );
	if ( myStatus == T2BF_STATUS_OK )
	{
		// Init core and SPP
		myStatus = T2BFInit( &btSettings, AppEngineBtCb );
	}

	if ( myStatus == T2BF_STATUS_OK )
	{
		// Start core
		myStatus = T2BFStart( );
		T2BFDiscoverable( true );
      T2BFAutoAuthentication( T2BF_AUTO_AUTH_ALLOW );
	}

	if ( myStatus == T2BF_STATUS_OK )
	{
		// Change friendly name
		if( myName != NULL)
		{
			T2BFWriteName( myName );
		}
	}

	if ( myStatus == T2BF_STATUS_OK )
	{
		// Wanted pin
		T2BFWritePin("0000");
	}

	if ( myStatus == T2BF_STATUS_OK )
	{
		myStatus = T2BFSppCreatePort( 330,					// Demo is set to 330
                               	    T2BF_SEC_NORMAL,
                                    AppEngineSppCb,
                               		"COM1",
                               		&mySppPort );
	}

	if ( myStatus == T2BF_STATUS_OK )
	{
		myStatus = T2BFSppOpen( mySppPort );
	}

	if ( myStatus == T2BF_STATUS_OK )
	{
		myState = AppEngine_Idle_State;
		AppEngineNotifyCallback( "App OK." );
		return true;
	}
	else
	{
		AppEngineNotifyCallback( "App failed." );
		return false;
	}
}

/*! \brief Handles functionality coupled to press of the Inquiry button
 *
 * \param None
 *
 * \retval None
 */
void AppEngineInquiryButton( void )
{
	TT2BFStatus status;
	myIndex = 0;
	if ( myState == AppEngine_Idle_State )
	{
		status = T2BFInquiryStart( T2BFENG_MAX_SEARCH_TIMEOUT, SERVICE_CLASS_ALL, DEVICE_CLASS_ALL,
		                       T2BFENG_MAX_SEARCH_RESULT, T2BFENG_MAX_NAME_LENGTH,
							   (TUTF16String)pMyName, pMyBdAddress, pMyDeviceInfo );
		if ( status != T2BF_STATUS_OK )
		{
			myState = AppEngine_Idle_State;
			AppEngineNotifyCallback( "Inquiry failed" );
		}
		else
		{
			myState = AppEngine_Inquiry_State;
			myBytes = 0;
			AppEngineNotifyCallback( "Inquiry started" );
		}
	}
	else if ( myState == AppEngine_Inquiry_State )
	{
		T2BFInquiryStop( );
		AppEngineNotifyCallback( "Inquiry stopped" );
	}
	else
	{
		AppEngineNotifyCallback( "Not allowed" );
	}
}

/*! \brief Handles functionality coupled to press of the Select button
 *
 * \param None
 *
 * \retval None
 */
void AppEngineSelectDeviceButton( void )
{
	uint8 name[T2BFENG_MAX_NAME_LENGTH];
   	uint8 i;
	if ( myState == AppEngine_Idle_State )
	{
		//convert namestring to be able to send it
		for( i = 0; i < T2BFENG_MAX_NAME_LENGTH; i++)
		{
			name[ i ] = (uint8) pMyName[ myIndex ][ i ];
		}
	   	if ( name[ 0 ] != 0 )
	   	{
	   		memcpy( myBdAddr, pMyBdAddress[ myIndex ], sizeof( TBdAddr ) );
	   		AppEngineNotifyCallback( name );
	   		myIndex++;
	   	}
	   	else
	   	{
	   		if ( myIndex > 0)
	   		{
	   			myIndex = 0;
	   			//convert namestring to be able to send it
				for( i = 0; i < T2BFENG_MAX_NAME_LENGTH; i++)
				{
					name[ i ] = (uint8) pMyName[ myIndex ][ i ];
				}
			   	if ( name[ 0 ] != 0 )
			   	{
			   		memcpy( myBdAddr, pMyBdAddress[ myIndex ], sizeof( TBdAddr ) );
			   		AppEngineNotifyCallback( name );
			   		myIndex++;
			   	}
	   		}
	   		else
	   		{
	   			myIndex = 0;
	   			AppEngineNotifyCallback( "List is empty" );
	   		}
	   	}
	}
	else
	{
		AppEngineNotifyCallback( "Not allowed" );
	}
}

/*! \brief Handles functionality coupled to press of the Connect button
 *
 * \param None
 *
 * \retval None
 */
void AppEngineConnectButton( void )
{
	TT2BFStatus status;
	if ( myState == AppEngine_Connected_State )
	{
		myState = AppEngine_Disconnecting_State;
                AppEngineNotifyCallback( "Disconnecting" );
		status = T2BFSppDisconnect( mySppPort );

	}
	else if (  myState == AppEngine_Idle_State  )
	{
		myState = AppEngine_Connecting_State;
		AppEngineNotifyCallback( "Connecting" );
        status = T2BFSppConnect( mySppPort, myBdAddr, NULL );
		if ( status != T2BF_STATUS_OK )
		{
			myState = AppEngine_Idle_State;
			AppEngineNotifyCallback( "Connecting failed" );
		}
	}
}

/*! \brief Sends a letter for each direction on the joystick
 *
 * \param joystick   Identifies what direction the joystick has been pulled in.
 *
 * \retval None.
 */
void AppEngineJoystickButton( TAppEngineJoystick joystick )
{
	switch ( joystick )
	{
		case AppEngine_Left:
			AppEngineInternalSendData( 1, "L" );
		break;

		case AppEngine_Right:
			AppEngineInternalSendData( 1, "R" );
		break;

		case AppEngine_Up:
			AppEngineInternalSendData( 1, "U" );
		break;

		case AppEngine_Down:
			AppEngineInternalSendData( 1, "D" );
		break;

		case AppEngine_Push:
			AppEngineInternalSendData( 1, "P" );
		break;
	}
}

/*! \brief Sends data over SPP
 *
 * \param dataLength   Defines the length of the data that shall be sent using SPP.
 * \param pData        Points to data area containing data to be sent.
 *
 * \retval true        Transmission succeeded.
 * \retval false       Transmission failed.
 */
boolean AppEngineInternalSendData( uint16 dataLength, uint8* pData )
{
	if ( myState == AppEngine_Connected_State )
	{
		T2BFSppTx( mySppPort, dataLength, pData );
		return true;
	}
	else
	{
		AppEngineNotifyCallback( "Not connected" );
		return false;
	}
}

/*! \brief Callback function that handles events from t2BF.
 *
 * \param event       Specifies the occurred event.
 * \param bdAddr      Identifies the remote unit, if applicable.
 * \param TServiceId  Identifies to t2BF module the issued the event.
 *
 * \retval None.
 */
void AppEngineBtCb( TT2BFBtEvent event, TBdAddr bdAddr, TServiceId id )
{
	if ( event == T2BF_BT_EVENT_INQUIRY_COMPLETE )
	{
		myState = AppEngine_Idle_State;
		AppEngineNotifyCallback( "Inquiry complete" );
	}
	else if ( event == T2BF_BT_EVENT_INQUIRY_RESULT )
	{
		myBytes++;
		if (myBytes < 20)
		{
			memset( myBytesBuffer, '.', myBytes );
			myBytesBuffer[myBytes] = 0;
		}
   	AppEngineNotifyCallback( myBytesBuffer );
	}
}

/*! \brief Callback function that handles events related to the SPP connection.
 *
 * \param event     Specifies the occurred event.
 * \param port      Identifies the port on which the event occurred.
 *
 * \retval None.
 */
void AppEngineSppCb( TT2BFSppEvent event, TPort port )
{
	uint8 totalBytes;

	if ( event == T2BF_SPP_EVENT_DATA && port == mySppPort )
	{
		totalBytes = T2BFSppRxCount( mySppPort );
		while ( totalBytes > 0 )
		{
			T2BFSppRx( mySppPort, 1, (myBytesBuffer + myBytes) );
			totalBytes--;
			myBytes++;
			if (myBytes == 19)
			{
				myBytes = 0;
			}
		}
		myBytesBuffer[19] = 0;
		AppEngineNotifyCallback( myBytesBuffer );
	}
	else if ( event == T2BF_SPP_EVENT_CONNECTED && port == mySppPort )
	{
		myState = AppEngine_Connected_State;
		AppEngineNotifyCallback( "Connected" );
		myBytes = 0;
		memset( myBytesBuffer, ' ', 20);
		myBytesBuffer[20] = 0;
		T2BFDiscoverable( false );
	}
	else if ( ( event == T2BF_SPP_EVENT_DISCONNECTED || event == T2BF_SPP_EVENT_CONNECTION_LOST ) && port == mySppPort )
	{
		myState = AppEngine_Idle_State;
		AppEngineNotifyCallback( "Disconnected" );
		T2BFDiscoverable( true );
	}
}

/*! \brief Callback function used to access write functionality defined by the application.
 *
 * \param textToDisplay   String to print.
 *
 * \retval None.
 */
void AppEngineNotifyCallback( TString textToDisplay )
{
	if ( pMyCb )
	{
		pMyCb( textToDisplay );
	}
}
