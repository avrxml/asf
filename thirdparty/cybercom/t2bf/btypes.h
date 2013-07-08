/*
*******************************************************************************
*                        (c) Teleca Sweden West AB 2006                       *
*******************************************************************************

   File name:     btypes.h
   Author:        Ulf Karlsson
   Revision:      PI1
   Description:   BTYPES - Basic types.
                  This is a header file with basic types that are used within
                  Teleca SW.

   REVISION HISTORY

   Rev   Date     Author   Description
   ------------------------------------
   PA1   050427   UKN      First version.
   A     051101   UKN      Released.
   PB1   060315   UKAR     Added type fo BT link key.
   PB2   060329   UKAR     Changed pragma pack directive
   B     060529   MJOE     Release version.
   PC1   060824   MJOE     Added PBAP defines.
   C     060911   MJOE     Release version.
   PD1   061120   MJOE     Removed TPin.
                           Removed unused number types.
   D     061121   MJOE     Release version.
   PE1   061206   MJOE     Added TTristate type.
   E     061220   MJOE     Release version.
   PF1   070328   JOHO     Removed UCS2 string types.
   F     070330   MJOE     Release version.
   PG1   070531   MJOE     Changed to ANSII C comments.
   G     070531   MJOE     Release version.
   PH1   080328   EHAR     Added DeviceClassEnum, ServiceClassEnum and
                           changed serviceIdenum
   PH2   080417   EHAR     Added more service id's A2DP AVRCPT AVRCPC
   H     080513   JOHO     Release version.
   PI1   080520   EHAR     Added DUN_GW and DUN_DT in service id's

*******************************************************************************
*/

#ifndef _BTYPES_H
#define _BTYPES_H

/*
-------------------------------
   Include files
-------------------------------
*/

/*
-------------------------------
   Definitions
-------------------------------
*/

/* Max length of the Bluetooth PIN code, including NULL termination character */
#define PIN_CODE_MAX    (16+1)

/* General macros
   Get a uint16 from a uint8 byte stream = p where the bytes are ordered LSB first */
#define BTYPES_GET_LSB_UINT16( p ) ( ((uint16)*(p)) + (((uint16)*(p+1))<<8) )
/* Set a uint16 = v into a uint8 byte stream = p where the bytes are ordered LSB first */
#define BTYPES_SET_LSB_UINT16( p, v ) p[0] = (uint8)v; p[1] = (uint8)(v>>8)
/* Get a uint32 from a uint8 byte stream = p where the bytes are ordered LSB first */
#define BTYPES_GET_LSB_UINT32( p ) ( ((uint32)*(p)) + (((uint32)*(p+1))<<8) + (((uint32)*(p+2))<<16) + (((uint32)*(p+3))<<24) )

/* Get a uint16 from a uint8 byte stream = p where the bytes are ordered MSB first */
#define BTYPES_GET_MSB_UINT16( p ) ( (((uint16)*(p))<<8) + ((uint16)*(p+1)) )
/* Set a uint16 = v into a uint8 byte stream = p where the bytes are ordered MSB first */
#define BTYPES_SET_MSB_UINT16( p, v ) p[0] = (uint8)(v>>8); p[1] = (uint8)(v)
/* Get a uint32 from a uint8 byte stream = p where the bytes are ordered MSB first */
#define BTYPES_GET_MSB_UINT32( p ) ( (((uint32)*(p))<<24) + (((uint32)*(p+1))<<16) + (((uint32)*(p+2))<<8) + ((uint32)*(p+3)) )

/* Basic integers */
#ifndef uint8
typedef unsigned char   uint8;
#endif
#ifndef uint16
typedef unsigned short  uint16;
#endif
#ifndef int8
typedef signed char     int8;
#endif
#ifndef int16
typedef short           int16;
#endif
#ifndef uint32
typedef unsigned long   uint32;
#endif
#ifndef int32
typedef long            int32;
#endif
#ifndef boolean
typedef unsigned char   boolean;
#endif

#ifndef TRUE
#define TRUE (unsigned char) 1
#endif
#ifndef FALSE
#define FALSE (unsigned char) 0
#endif
#ifndef MAYBE
#define MAYBE (unsigned char) 2
#endif
#ifndef NULL
#define NULL 0
#endif
typedef unsigned char bitfield;

/* General types */
typedef uint16          TId;
typedef uint16          THandle;
typedef uint8*          TString;
typedef uint8           TChar;
typedef uint8           TPort;
typedef uint8           TFirst[ 1 ];
typedef uint8           TTristate;  /* TRUE, FALSE, or MAYBE */

typedef enum
{
   SERVICE_ID_ALL             = 0xFFFFFFFF,
   SERVICE_ID_UNKNOWN         = 0x00000000,
   SERVICE_ID_SPP             = 0x00000001,
   SERVICE_ID_HEADSET         = 0x00000002,
   SERVICE_ID_HEADSET_AG      = 0x00000004,
   SERVICE_ID_HANDSFREE       = 0x00000008,
   SERVICE_ID_HANDSFREE_AG    = 0x00000010,
   SERVICE_ID_OBEX            = 0x00000020,
   SERVICE_ID_OP_CLIENT       = 0x00000040,
   SERVICE_ID_OP_SERVER       = 0x00000080,
   SERVICE_ID_FT_CLIENT       = 0x00000100,
   SERVICE_ID_FT_SERVER       = 0x00000200,
   SERVICE_ID_SYNC_CLIENT     = 0x00000400,
   SERVICE_ID_SYNC_SERVER     = 0x00000800,
   SERVICE_ID_SYNCML_CLIENT   = 0x00001000,
   SERVICE_ID_SYNCML_SERVER   = 0x00002000,
   SERVICE_ID_PBAP_CLIENT     = 0x00004000,
   SERVICE_ID_PBAP_SERVER     = 0x00008000,
   SERVICE_ID_AVRCP_TARGET    = 0x00010000,
   SERVICE_ID_AVRCP_CONTROLLER = 0x00020000,
   SERVICE_ID_A2DP_SOURCE     = 0x00040000,
   SERVICE_ID_A2DP_SINK       = 0x00080000,
   SERVICE_ID_DUN_GW          = 0x00100000,
   SERVICE_ID_DUN_DT          = 0x00200000

} TServiceIdEnum;
typedef uint32 TServiceId;

/* Class Filter */

typedef enum
{
   SERVICE_CLASS_ALL             = 0xFFFF,
   SERVICE_CLASS_POSITIONING     = 0x0100,
   SERVICE_CLASS_NETWORKING      = 0x0200,
   SERVICE_CLASS_RENDERING       = 0x0400,
   SERVICE_CLASS_CAPTURING       = 0x0800,
   SERVICE_CLASS_OBJECT_TRANSFER = 0x1000,
   SERVICE_CLASS_AUDIO           = 0x2000,
   SERVICE_CLASS_TELEPHONY       = 0x4000,
   SERVICE_CLASS_INFOSERVER      = 0x8000

} ServiceClassEnum;
typedef uint16 TServiceClass;

typedef enum
{
   DEVICE_CLASS_ALL             = 0xFFFF,
   DEVICE_CLASS_COMPUTER        = 0x0001,
   DEVICE_CLASS_PHONE           = 0x0002,
   DEVICE_CLASS_LAN             = 0x0004,
   DEVICE_CLASS_AUDIO_VIDEO     = 0x0008,
   DEVICE_CLASS_PERIPHERAL      = 0x0010,
   DEVICE_CLASS_IMAGING         = 0x0020,
   DEVICE_CLASS_WEARABLE        = 0x0040,
   DEVICE_CLASS_TOY             = 0x0080,
   DEVICE_CLASS_MEDICAL         = 0x0100

} DeviceClassEnum;
typedef uint16 TDeviceClass;

typedef enum
{
   BSTATUS_OK              = 0x00,
   BSTATUS_BUSY,
   BSTATUS_REMOTE_ERROR,
   BSTATUS_TIMEOUT,
   BSTATUS_NOT_CONNECTED,
   BSTATUS_NOT_SUPPORTED,
   BSTATUS_NOT_AVAILABLE,
   BSTATUS_LOCKED,

   BSTATUS_INFO_OVERFLOW   = 0x80,
   BSTATUS_INFO_CONTINUE,
   BSTATUS_INFO_SETUP,
   BSTATUS_INFO_CREATED,

   BSTATUS_ERROR_OS        = 0xF0,
   BSTATUS_ERROR_PARAM,
   BSTATUS_ERROR_MEMORY,
   BSTATUS_ERROR_SOLID_MEMORY,

   BSTATUS_ERROR           = 0xFF

} TBStatusEnum;
typedef uint8 TBStatus;

typedef enum
{
   CHAR_SET_UNKNOWN            = 0x00,
   CHAR_SET_GSM                = 0x01,
   CHAR_SET_UCS2               = 0x02,
   CHAR_SET_UTF8               = 0x03
} TCharSetEnum;
typedef uint8 TCharSet;



#define BD_ADDR_LENGTH  6
typedef uint8 TBdAddr[ BD_ADDR_LENGTH ];

#define BD_LINKKEY_LENGTH 16
typedef uint8 TLinkKey[ BD_LINKKEY_LENGTH ];

#define BD_NAME_LENGTH 248
typedef uint8 TBdName[ BD_NAME_LENGTH ];

#define FILE_REV_LENGTH 8
typedef uint8 TFileRevision[ FILE_REV_LENGTH ];

#endif
