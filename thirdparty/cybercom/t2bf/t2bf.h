/*
*******************************************************************************
*                     (c) Teleca Sweden West AB 2006                          *
*******************************************************************************

   File name:     t2bf.h
   Author:        Ulf Karlsson
   Revision:      T
   Description:   T2BF API. Teleca Telematics Bluetooth Framework
                  Application Programmers Interface.
                  See 104-1213 T2BF API Specification for more
                  information.

   REVISION HISTORY

   Rev   Date     Author   Description
   ------------------------------------
   PA1   050621   UKN      First version. No comments, see .doc file.
   PA2   050627   UKN      Updated to API document PB5.
   A     051102   UKN      API expanded. First release.
   PB1   051222   UKN      Introduced support levels.
   PB2   060222   UKAR     API expanded with more PIM functions.
                           Added call event for AG supported features.
   PB3   060309   UKAR     Small changes with PIM.
   B     060312   UKAR     Tested release version.
   PC1   060316   UKAR     Minor fixes.
   PC2   060330   HWEN     Added enum for PBAP
   PC3   060404   MJOE     Added _STATE_ to state events.
                           Changed to UCS2 format on name and operator.
                           Changed to UCS2 format on BT user friendly names.
                           Added count parameter to T2BFReadList.
                           Updated PIM interface.
   PC4   060420   UKAR     Added local BD address in init struct.
   PC5   060524   MJOE     Added index to T2BFTerminate function.
   C     060529   MJOE     Release version.
   PD1   060609   MJOE     Typedef for TT2BFFileAccess.
   E     060613   UKAR     Release version.
   PF1   060630   MJOE     Implemented local PIM interface.
                           Added _EVENT_STARTED and _EVENT_STOPPED.
                           Added T2BFWriteName and T2BFReadName.
                           Added T2BFPairedDeletePolicy.
                           Added T2BFPimContactLabel functions.
   F     060705   MJOE     Release version.
   PG1   060712   MJOE     Added extern "C" for C++ compatibility.
                           Implemented email, address and label access in local
                           PIM contacts.
                           Created a single type for PIM contact fields,
                           TT2BFPimFieldType, instead of seperate types for
                           numbers, address etc.
   G     060721   MJOE     Release version.
   PH1   060824   MJOE     Added PBAP_SIM define.
   H     060911   MJOE     Release version.
   PI1   061108   MJOE     Added T2BFReadHciChip.
                           Removed T2BFPin
                           Added T2BFWritePin and T2BFReadPin.
   I     061121   MJOE     Release version.
   PJ1   061206   MJOE     Added T2BFReadSyncSupported.
                           Added SPP functions.
   J     061220   MJOE     Release version.
   PK1   070328   JOHO     Changed during integration of UNISTR string module
                           (UTF-16 instead of UCS2).
   K     070402   MJOE     Release version.
   PL1   070531   MJOE     Changed to ANSII C comments.
                           Added T2BFGenericHci.
   L     070531   MJOE     Release version.
   M     070926   JOHO     Release version.
   PN1   071008   THBR     Added export macro.
   PN2   071011   JOHO     Added more detailed error codes from T2BFInit,
                           added T2BF_CALL_EVENT_SLC_COMPLETE and
                           T2BF_CALL_EVENT_CONNECT_ABORTED.
   N     071012   JOHO     Release version.
   O     071031   JOHO     Added conference call handling (not completely
                           supported by lower layers yet).
   P     071119   JOHO     Added test functions needed for PTS tests. (Conf call
                           now supported by lower layers).
   PQ1	 071128   EHAR	   Added T2BFConference function
   Q     071221   JOHO     Added handling of sync command in API. This is only
                           for PTS test purpose, should not be used in a real
                           application.
   R     080205   JOHO     Added T2BFSetCharset (for debug purpose).
   PS1   080213   THBR     Added media functions.
   PS2   080326   EHAR     Added T2BFEnd function and a init check in every other
                           public function. Made T2BF also stop SYNC
   PS3   080327   EHAR     Added T2BFIquiry and T2BFSniff functions
   PS4   080328   EHAR     Moved types to btypes.h and changed sniff funtion names
   PS5   080414   JOHO     New revision string.
   PS6   080417   EHAR     Added remoteServices function
   PS7   080508   JOHO     Added T2BFPimPhonebookDeleteSync.
   PS8   080512   THBR     Added init/end T2bfMgr.
   S     080513   JOHO     Release version.
   PT1   080520   EHAR     Added DUN functions
   PT2   080528   EHAR     Changed DUN API Cb and settings are now in parameters
                           in T2BFDunConnect insted of T2bFInit
   PT3   080529   EHAR     Added function T2BFPrivateConsultation
   T     080529   JOHO     Release version. Added read NVS revision tag function.
                           Changed API to break out CALL module handling from
                           generic T2BF handling.

******************************************************************************/

#ifndef _T2BF_H
#define _T2BF_H

/*
-------------------------------
   Include files
-------------------------------
*/
#include "btypes.h"

/*
-------------------------------
   Macros
-------------------------------
*/

#ifdef ENABLE_USE_DLL
#ifdef __cplusplus
     #define cppfudge "C"
#else
    #define cppfudge
#endif // __cplusplus

#ifdef BUILD_DLL
    // the dll exports
	#define T2BF_EXPORT __declspec(dllexport)
#else
    // the exe imports
   	#define T2BF_EXPORT extern cppfudge  __declspec(dllimport)
#endif // BUILD_DLL
#endif // ENABLE_USE_DLL

#ifndef ENABLE_USE_DLL
#ifndef BUILD_DLL
#undef T2BF_EXPORT
#define T2BF_EXPORT
#endif // BUILD_DLL
#endif // ENABLE_USE_DLL

/*
-------------------------------
   Definitions
-------------------------------
*/

#define T2BF_SW_VERSION        "v5.01B"
#define T2BF_SW_VERSION_LENGTH 30

#define T2BF_COD_HANDSFREE_W_OBEX    { 0x08, 0x04, 0x30 }
#define T2BF_COD_HANDSFREE           { 0x08, 0x04, 0x20 }
#define T2BF_COD_HEADSET             { 0x04, 0x04, 0x20 }

/* Set the pack directive */
#pragma pack( 1 )

/*
-------------------------------
   Definitions - Strings
-------------------------------
*/

typedef uint16* TUTF16String;
typedef uint32  TUnicodeChar;

/* Encodings */
typedef enum
{
   T2BF_ENCODING_NONE                 = 0x00,
   T2BF_ENCODING_QUOTED_PRINTABLE     = 0x81,
   T2BF_ENCODING_BASE64               = 0x82,
   T2BF_ENCODING_8BIT                 = 0x83
} TT2BFEncodingEnum;
typedef uint8 TT2BFEncoding;

/*
-------------------------------
   Definitions - General
-------------------------------
*/

typedef uint8 TT2BFCod[3];

/* T2BF status type */
typedef enum
{
   T2BF_STATUS_OK             = 0x00,
   T2BF_STATUS_BUSY,
   T2BF_STATUS_AG_ERROR,
   T2BF_STATUS_TIMEOUT,
   T2BF_STATUS_NOT_CONNECTED,
   T2BF_STATUS_NOT_SUPPORTED,
   T2BF_STATUS_NOT_AVAILABLE,
   T2BF_STATUS_HW_ERROR,
   T2BF_STATUS_BT_COMMUNICATION_ERROR,
   T2BF_STATUS_BT_COMMUNICATION_TIMEOUT,
   T2BF_STATUS_SOLID_MEM_ERROR,

   T2BF_STATUS_INFO_OVERFLOW  = 0x80,
   T2BF_STATUS_INFO_CONTINUE,

   T2BF_STATUS_ERROR_TSFP     = 0xE0,

   T2BF_STATUS_ERROR_OS       = 0xF0,
   T2BF_STATUS_ERROR_PARAM,
   T2BF_STATUS_ERROR          = 0xFF

} TT2BFStatusEnum;
typedef uint8 TT2BFStatus;


/*
-------------------------------
   Definitions - General
-------------------------------
*/

/* BT events */
typedef enum
{
   T2BF_BT_EVENT_CONNECTED = 0x01,           /*  id = Not used */
   T2BF_BT_EVENT_DISCONNECTED,               /*  id = HCI Error code. Why disconnection occurred */

   T2BF_BT_EVENT_DISCONNECTED_PORT,          /*  id = TServiceId. Service disconnected */
   T2BF_BT_EVENT_CONNECTION_LOST_PORT,       /*  id = TServiceId. Service disconnected */
   T2BF_BT_EVENT_CONNECTED_PORT,             /*  id = TServiceId. Service connected */

   T2BF_BT_EVENT_AUDIO_CONNECTED,            /*  id = Not used */
   T2BF_BT_EVENT_AUDIO_DISCONNECTED,         /*  id = Not used */

   T2BF_BT_EVENT_AUTHORISE,                  /*  id = Not used */
   T2BF_BT_EVENT_AUTHORISATION_CANCELLED,    /*  id = Not used */
   T2BF_BT_EVENT_AUTHENTICATE,               /*  id = Not used */
   T2BF_BT_EVENT_AUTHENTICATION_CANCELLED,   /*  id = Not used */
   T2BF_BT_EVENT_AUTHENTICATED,              /*  id = Not used */

   T2BF_BT_EVENT_STARTED,                    /*  id = Not used */
   T2BF_BT_EVENT_STOPPED,                    /*  id = Not used */

   T2BF_BT_EVENT_INQUIRY_RESULT,             /*  id = Result number in inquiry list */
   T2BF_BT_EVENT_INQUIRY_COMPLETE,           /*  id = Number of found devices */

   T2BF_BT_EVENT_REMOTE_SERVICES,            /*  id = Services searched for that remote device support */
   T2BF_BT_EVENT_REMOTE_SERVICES_FAILED      /*  id = Not used */


} TT2BFBtEventEnum;
typedef uint8 TT2BFBtEvent;

/* Security level type */
typedef enum
{
   T2BF_SEC_LEVEL_1 = 0x01,
   T2BF_SEC_LEVEL_2 = 0x02,
   T2BF_SEC_LEVEL_3 = 0x03,
   T2BF_SEC_LEVEL_3E = 0x83

} TT2BFSecLevelEnum;
typedef uint8 TT2BFSecLevel;

/* Port security */
typedef enum
{
   T2BF_SEC_NONE             = 0x00,
   T2BF_SEC_AUTHORISE_IN     = 0x01,
   T2BF_SEC_AUTHENTICATE_IN  = 0x02,
   T2BF_SEC_ENCRYPT_IN       = 0x04,
   T2BF_SEC_AUTHORISE_OUT    = 0x10,
   T2BF_SEC_AUTHENTICATE_OUT = 0x20,
   T2BF_SEC_ENCRYPT_OUT      = 0x40,
   T2BF_SEC_NORMAL           = 0x66,
   T2BF_SEC_ALL              = 0x77

} TT2BFSecurityEnum;
typedef uint8 TT2BFSecurity;

/* Authentication settings */
typedef enum
{
   T2BF_AUTO_AUTH_OFF = 1,
   T2BF_AUTO_AUTH_ALLOW,
   T2BF_AUTO_AUTH_DENY
} TT2BFAutoAuthEnum;
typedef uint8 TT2BFAutoAuth;

/* Delete policy for paired devices list
   Used when the list is full, and there is a new pairing */
typedef enum
{
   T2BF_DELETE_POLICY_DENY = 0,
   T2BF_DELETE_POLICY_CONNECTED,
   T2BF_DELETE_POLICY_PAIRED
} TT2BFDeletePolicyEnum;
typedef uint8 TT2BFDeletePolicy;

typedef enum
{
   T2BF_LOWPOWER_OFF         = 0,
   T2BF_LOWPOWER_SNIFF_MODE
} TT2BFLowPowerModeEnum;
typedef uint8 TT2BFLowPowerMode;

/* BT settings */
typedef struct
{
   boolean setBdAddr;      /* TRUE if below BD address shall be set. FALSE means ignore.     */
   TBdAddr bdAddr;         /* Local BD address to use by device.                             */
   TUTF16String localName; /* Local name of device e.g. "T2BF device"                        */
   TT2BFCod cod;           /* Class of device as specified in BT spec. headset, hands-free.
                              If using OPP, OBEX flag should be set to maximise interop.     */
   TT2BFSecLevel secLevel; /* General security level. Most common is level 2.                */
   uint16 supTO;           /* The number of BT slots (0.625 ms)T2BF shall wait before link
                              is considered lost                                             */
   uint16 maxSniff;        /* Maximum sniff BT slots. Set to 0 if sniff is not wanted.       */
   uint16 maxPark;         /* Maximum part BT slot. Set to 0 if park is not wanted.          */
   uint16 maxHold;         /* Maximum hold slots. Set to 0 if hold is not wanted.            */

} TT2BFBtSettings;

/* Device Info */
typedef struct
{
  TServiceClass  majorServiceClass;
  TDeviceClass   majorDeviceClass;

} TT2BFDeviceInfo;

typedef struct
{
   uint16 maxInterval;
   uint16 minInterval;
   uint16 attempt;
   uint16 timeout;
} TT2BFLowPowerParameters;
/*
-------------------------------
   Definitions - CALL
-------------------------------
*/

/* Call events */
typedef enum
{
   T2BF_CALL_EVENT_STATE_NOT_CONNECTED = 0x01,
   T2BF_CALL_EVENT_STATE_CONNECTED_HS,
   T2BF_CALL_EVENT_STATE_INCOMING_HS,
   T2BF_CALL_EVENT_STATE_CONNECTED_NO_CALL,
   T2BF_CALL_EVENT_STATE_INCOMING,
   T2BF_CALL_EVENT_STATE_OUTGOING,
   T2BF_CALL_EVENT_STATE_ACTIVE,
   T2BF_CALL_EVENT_STATE_ACTIVE_WAITING,
   T2BF_CALL_EVENT_STATE_ACTIVE_HELD,
   T2BF_CALL_EVENT_STATE_HELD,
   T2BF_CALL_EVENT_STATE_HELD_WAITING,
   T2BF_CALL_EVENT_STATE_HELD_OUTGOING,
   T2BF_CALL_EVENT_STATE_ACTIVE_HELD_WAITING,

   T2BF_CALL_EVENT_CONNECTION_LOST,
   T2BF_CALL_EVENT_CONNECTING,
   T2BF_CALL_EVENT_CONNECT_TIMEOUT,
   T2BF_CALL_EVENT_CONNECT_FAILURE,
   T2BF_CALL_EVENT_VOICE_DIAL_ON,
   T2BF_CALL_EVENT_VOICE_DIAL_OFF,
   T2BF_CALL_EVENT_RING,
   T2BF_CALL_EVENT_AUDIO_DISCONNECTED,
   T2BF_CALL_EVENT_AUDIO_CONNECTED,
   T2BF_CALL_EVENT_REMOTE_VOLUME,
   T2BF_CALL_EVENT_SERVICE_NOT_AVAILABLE,
   T2BF_CALL_EVENT_SERVICE,
   T2BF_CALL_EVENT_SIGNAL,
   T2BF_CALL_EVENT_BATTERY,
   T2BF_CALL_EVENT_ROAMING,
   T2BF_CALL_EVENT_NUMBER,
   T2BF_CALL_EVENT_LIST,
   T2BF_CALL_EVENT_LIST_FINISHED,
   T2BF_CALL_EVENT_LIST_FAILED,
   T2BF_CALL_EVENT_LIST_TIMEOUT,
   T2BF_CALL_EVENT_AG_SUPPORTED_FEATURES,
   T2BF_CALL_EVENT_SLC_COMPLETE,
   T2BF_CALL_EVENT_CONNECT_ABORTED

} TT2BFCallEventEnum;
typedef uint8 TT2BFCallEvent;

/* HF supported features, local device */
typedef enum
{
   T2BF_HF_FEATURES_EC_NR                 = 0x0001,
   T2BF_HF_FEATURES_THREE_WAY_CALLING     = 0x0002,
   T2BF_HF_FEATURES_NUMBER_PRESENT        = 0x0004,
   T2BF_HF_FEATURES_VOICE_DIAL            = 0x0008,
   T2BF_HF_FEATURES_REMOTE_VOLUME_CTRL    = 0x0010,
   T2BF_HF_FEATURES_ENHANCED_CALL_STATUS  = 0x0020,
   T2BF_HF_FEATURES_ENHANCED_CALL_CONTROL = 0x0040,
   T2BF_HF_FEATURES_ALL                   = 0x007F
} TT2BFHFFeaturesEnum;
typedef uint32 TT2BFHFFeatures;

/* Number types */
typedef enum
{
   T2BF_CALL_NUMBER_TYPE_ACTIVE    = 0x00,
   T2BF_CALL_NUMBER_TYPE_HELD      = 0x01,
   T2BF_CALL_NUMBER_TYPE_OUTGOING  = 0x02,
   T2BF_CALL_NUMBER_TYPE_INCOMING  = 0x04,
   T2BF_CALL_NUMBER_TYPE_WAITING   = 0x05,
   T2BF_CALL_NUMBER_TYPE_LIST      = 0x10

} TT2BFNumberTypeEnum;
typedef uint8 TT2BFNumberType;

/* List types */
typedef enum
{
   T2BF_LIST_TYPE_DIALLED       = 0x00,
   T2BF_LIST_TYPE_EMERGENCY     = 0x01,
   T2BF_LIST_TYPE_SIM_FIX_DIAL  = 0x02,
   T2BF_LIST_TYPE_SIM_DIALLED   = 0x03,
   T2BF_LIST_TYPE_MISSED        = 0x04,
   T2BF_LIST_TYPE_PHONE         = 0x05,
   T2BF_LIST_TYPE_COMBINED      = 0x06,
   T2BF_LIST_TYPE_OWN           = 0x07,
   T2BF_LIST_TYPE_RECIEVED      = 0x08,
   T2BF_LIST_TYPE_SIM           = 0x09,
   T2BF_LIST_TYPE_TA            = 0x0A

} TT2BFListTypeEnum;
typedef uint8 TT2BFListType;

/* Call settings */
typedef struct
{
   TT2BFHFFeatures hf;   /* HF features that the device supports. */
                         /* See HF profile spec. */
   TT2BFSecurity sec;    /* If security level is 2, specify exact service  */
                         /* security settings. */
   uint8 gain;           /* Initial speaker gain.  */
   uint8 gainMic;        /* Initial mic gain. */
   boolean hs : 1;       /* TRUE to turn on HS profile port. CALL will always */
                         /* attempt HF profile first. */
   boolean moveAudio : 1;/* If CALL manager should always retrieve the audio  */
                         /* after a phonecall is answered. */
   uint8 timeout;        /* Timeout in seconds for each AT command.  */
                         /* 30s is a recommendation */
                         /* since some older phones may take very long to e.g.  */
                         /* connect a dialled international number.  */
   uint8 maxLengthNumber;/* Max length of numbers that can be stored */
   uint8 maxLengthName;  /* Max length of names that can be stored */

} TT2BFCallSettings;



/*
-------------------------------
   Definitions - SYNC
-------------------------------
*/

/* Sync events */
typedef enum
{
   T2BF_SYNC_EVENT_SYNC_STARTED = 0x01,      /* value = TT2BFSync                   */
   T2BF_SYNC_EVENT_TOTAL,                    /* value = number of contacts to sync.
                                                0xFFFFFFFF means unknown.           */
   T2BF_SYNC_EVENT_PROGRESS,                 /* value = number of contacts synced.  */
   T2BF_SYNC_EVENT_FINISHED,                 /* value = TT2BFSync                   */
   T2BF_SYNC_EVENT_FAILED,                   /* value = TT2BFSync                   */
   T2BF_SYNC_EVENT_STOPPED,                  /* value = TT2BFSync                   */
   T2BF_SYNC_EVENT_OBJECT_INFO_FILENAME,     /* value = length of filename string   */
   T2BF_SYNC_EVENT_OBJECT_INFO_FILETYPE,     /* value = length of filetype string   */
   T2BF_SYNC_EVENT_OBJECT_INFO_FILELENGTH,   /* value = number of bytes             */
   T2BF_SYNC_EVENT_OBJECT,                   /* value = object id,
                                                MUST call T2BFAcceptObject( object id )
                                                or T2BFRejectObject( object id )    */
   T2BF_SYNC_EVENT_OBJECT_PROGRESS,          /* value = bytes received              */
   T2BF_SYNC_EVENT_OBJECT_RECEIVED,          /* value = object id                   */
   T2BF_SYNC_EVENT_OBJECT_FAILED,            /* value = object id                   */
   T2BF_SYNC_EVENT_OBJECT_REJECTED,          /* value = object id                   */
   T2BF_SYNC_EVENT_SYNC_CMD

} TT2BFSyncEventEnum;
typedef uint8 TT2BFSyncEvent;

/* Sync types */
typedef enum
{
   T2BF_SYNC_BT           = 0x01,
   T2BF_SYNC_ML           = 0x02,
   T2BF_SYNC_AT_PHONE     = 0x04,
   T2BF_SYNC_AT_SIM       = 0x08,
   T2BF_SYNC_OPP_SERVER   = 0x10,
   T2BF_SYNC_PBAP         = 0x20,
   T2BF_SYNC_PBAP_SIM     = 0x40,
   T2BF_SYNC_BT_SYNC_CMD  = 0x80
} TT2BFSyncEnum;
typedef uint8 TT2BFSync;

/* File info types */
typedef enum
{
   T2BF_OBJECT_INFO_FILENAME = 0x00,
   T2BF_OBJECT_INFO_FILETYPE = 0x01
} TT2BFObjectInfoEnum;
typedef uint8 TT2BFObjectInfo;

/* Sync settings */
typedef struct
{
   TT2BFSync   supportedTypes;   /* Types combined i.e
                                    T2BF_SYNC_BT | T2BF_SYNC_OPP_SERVER       */
   boolean     acceptAllFiles;   /* TRUE will accept all filetypes,
                                    FALSE will only accept vCards.            */
   boolean     vCard2Pim;        /* TRUE will move vCard objects from
                                    OPP to PIM rather than the default FMAN.  */
   uint16      buffSize;         /* Sync common receive buffer size.
                                    MUST be at least 300 bytes
                                    Larger buffer == faster sync
                                    1000 bytes recommended.                   */
} TT2BFSyncSettings;



/*
-------------------------------
   Definitions - PIM
-------------------------------
*/

/* Definitions */
#define T2BF_PIM_INVALID      0xFFFF
#define T2BF_PIM_DELETE_ALL   0xFFFF

/* PIM contact fields */
typedef enum
{
   T2BF_PIM_CONTACT_EMPTY  = 0x00,
   T2BF_PIM_CONTACT_FIRSTNAME,
   T2BF_PIM_CONTACT_LASTNAME,
   T2BF_PIM_CONTACT_NUMBER,
   T2BF_PIM_CONTACT_EMAIL,
   T2BF_PIM_CONTACT_LABEL,
   T2BF_PIM_CONTACT_ADDR_BOX,
   T2BF_PIM_CONTACT_ADDR_EXTENDED,
   T2BF_PIM_CONTACT_ADDR_STREET,
   T2BF_PIM_CONTACT_ADDR_TOWN,
   T2BF_PIM_CONTACT_ADDR_REGION,
   T2BF_PIM_CONTACT_ADDR_POSTALCODE,
   T2BF_PIM_CONTACT_ADDR_COUNTRY
} TT2BFPimContactFieldEnum;
typedef uint8 TT2BFPimContactField;

/* PIM field types */
typedef enum
{
   T2BF_PIM_TYPE_UNKNOWN     = 0x00000000,

   T2BF_PIM_TYPE_PREF        = 0x00000001,
   T2BF_PIM_TYPE_WORK        = 0x00000002,
   T2BF_PIM_TYPE_HOME        = 0x00000004,
   T2BF_PIM_TYPE_VOICE       = 0x00000008,
   T2BF_PIM_TYPE_FAX         = 0x00000010,
   T2BF_PIM_TYPE_MSG         = 0x00000020,
   T2BF_PIM_TYPE_CELL        = 0x00000040,
   T2BF_PIM_TYPE_PAGER       = 0x00000080,
   T2BF_PIM_TYPE_BBS         = 0x00000100,
   T2BF_PIM_TYPE_MODEM       = 0x00000200,
   T2BF_PIM_TYPE_CAR         = 0x00000400,
   T2BF_PIM_TYPE_ISDN        = 0x00000800,
   T2BF_PIM_TYPE_VIDEO       = 0x00001000,
   T2BF_PIM_TYPE_DOM         = 0x00002000,
   T2BF_PIM_TYPE_INTL        = 0x00004000,
   T2BF_PIM_TYPE_POSTAL      = 0x00008000,
   T2BF_PIM_TYPE_PARCEL      = 0x00010000,
   T2BF_PIM_TYPE_INTERNET    = 0x00020000
} TT2BFPimFieldTypeEnum;
typedef uint32 TT2BFPimFieldType;

/* Search types */
typedef enum
{
   T2BF_PIM_SEARCH_NORMAL,
   T2BF_PIM_SEARCH_SCAN
} TT2BFPimSearchTypeEnum;
typedef uint16 TT2BFPimSearchType;

/* Initial char list entry */
typedef struct
{
   TUnicodeChar unicodeChar;
   uint16       firstIndex;
}  TT2BFPimInitialCharEntry;

/* PIM settings */
typedef struct
{
   boolean  storeLocal;            /* TRUE will store locally, FALSE will store on host using the
                                      PIM callback. If FALSE, all the following struct
                                      parameters are not used.                                  */
   uint8    maxPhonebooks;         /* Max number of phonebooks. Each phonebook will keep an area for
                                      syncronisation settings.                                  */
   uint16   maxContacts;           /* Max number of contacts in each phonebook.                 */
   uint16   maxContactSize;        /* Max number of bytes in each contact. Includes item ID and
                                      and checksum byte. Name is stored in UTF-16 format.       */
   uint16   maxFirstNameSize;      /* Max length of first and last names, in UTF-16 format.     */
   uint16   maxLastNameSize;
   uint8    maxPhoneNumbers;       /* Max number of different phone number types in each
                                      contact. 0 will allow as many numbers as possible to fit. */
   uint16   maxNumberSize;         /* Max length of phone numbers.                              */
   uint8    maxEmails;             /* Max number of emails.                                     */
   uint16   maxEmailSize;          /* Max length of an email.                                   */
   uint8    maxAddresses;          /* Max number of addresses in a each contact.                */
   uint16   maxAddressSize;        /* Max number of bytes in each contact for address data.
                                      All address fields are stored in UTF-16 format. Set to 0
                                      when address data is not used.                            */
   uint16   prioPhoneNumberTypes;  /* Type of phone numbers that are prioritised.
                                      Include TT2BFPimNumberType values using OR
                                      e.g. ( PIM_NUM_TYPE_WORK | PIM_NUM_TYPE_HOME | PIM_NUM_TYPE_VOICE )
                                      There is no use to include more than maxPhoneNumbers.     */
   TT2BFPimContactField sortField; /* Field to sort on. T2BF_PIM_FIELD_FIRSTNAME or T2BF_PIM_FIELD_LASTNAME. */

} TT2BFPimSettings;

/*
-------------------------------
   Definitions - PIM Host
-------------------------------
*/

/* PIM host events */
typedef enum
{
   T2BF_PIM_EVENT_INIT           = 0x01,     /* pParam = NULL                    */
   T2BF_PIM_EVENT_GET_PB,                    /* pParam = TT2BFPimParamGetPb      */
   T2BF_PIM_EVENT_CLEAR_PB,                  /* pParam = THandle                 */
   T2BF_PIM_EVENT_DELETE_PB,                 /* pParam = THandle                 */
   T2BF_PIM_EVENT_READ_SETTINGS,             /* pParam = TT2BFPimParamIo         */
   T2BF_PIM_EVENT_WRITE_SETTINGS,            /* pParam = TT2BFPimParamIo         */
   T2BF_PIM_EVENT_ADD_VCARD,                 /* pParam = TT2BFPimParamAddVc      */
   T2BF_PIM_EVENT_ADD_VCARD_DATA,            /* pParam = TT2BFPimParamAddVcData  */
   T2BF_PIM_EVENT_DELETE_VCARD_HANDLE,       /* pParam = THandle                 */
   T2BF_PIM_EVENT_DELETE_VCARD_ID,           /* pParam = TT2BFPimParamIo         */
   T2BF_PIM_EVENT_ADD_AT_CONTACT,            /* pParam = TT2BFPimAtContact       */
   T2BF_PIM_EVENT_TIMEOUT,                   /* pParam = NULL                    */

} TT2BFPimEventEnum;
typedef uint8 TT2BFPimEvent;

/* PIM IO */
typedef enum
{
   T2BF_PIM_IO_TYPE_SETTINGS = 0,
   T2BF_PIM_IO_TYPE_ID,
   T2BF_PIM_IO_TYPE_VCARD

} TT2BFPimIoTypeEnum;
typedef uint8 TT2BFPimIoType;

/* UUID supplied options */
typedef enum
{
   T2BF_PIM_ID_TYPE_EMBEDDED = 0,
   T2BF_PIM_ID_TYPE_SUPPLIED,
   T2BF_PIM_ID_TYPE_OBJECT_FILENAME_UNICODE

} TT2BFPimIdTypeEnum;
typedef uint8 TT2BFPimIdType;

/* Get PB params */
typedef struct
{
   TBdAddr bdAddr;
   boolean create;
} TT2BFPimParamGetPb;

/* IO params */
typedef struct
{
   THandle handle;
   uint8 length;
} TT2BFPimParamIo;

/* Add vCard params */
typedef struct
{
   THandle        pb;
   uint8          syncType;
   TT2BFPimIdType idType;
   uint8          lengthId;
   uint16         lengthData;
   boolean        complete;
} TT2BFPimParamAddVc;

/* Add vCard data params */
typedef struct
{
   THandle pb;
   uint16 length;
   boolean complete;
} TT2BFPimParamAddVcData;

/* Add AT contact params */
typedef struct
{
   THandle   pb;
   uint8     syncType;
   uint16    id;
   uint8     numType;
   uint16    lengthNum;
   uint16    lengthName;    /* Length is given in UTF-16 chars */
} TT2BFPimParamAtContact;

/* PIM params union */
typedef union
{
   THandle handle;
   TT2BFPimParamGetPb getPb;
   TT2BFPimParamIo io;
   TT2BFPimParamAddVc addVc;
   TT2BFPimParamAddVcData addVcData;
   TT2BFPimParamAtContact addAt;

} TT2BFPimParam;


/*
-------------------------------
   Definitions - FILE
-------------------------------
*/

/* Definitions */
#define T2BF_FILE_INVALID 0xFFFF

/* File host events */
typedef enum
{
   T2BF_FILE_EVENT_INIT                = 0x01,  /* pParam = NULL           */
   T2BF_FILE_EVENT_OPEN,                        /* pParam = TT2BFFileOpen  */
   T2BF_FILE_EVENT_READ,                        /* pParam = TT2BFFileIo    */
   T2BF_FILE_EVENT_WRITE,                       /* pParam = TT2BFFileIo    */
   T2BF_FILE_EVENT_CLOSE,                       /* pParam = THandle        */
   T2BF_FILE_EVENT_CLOSE_DISCARD,               /* pParam = THandle        */
   T2BF_FILE_EVENT_TIMEOUT                      /* Previous event was not
                                                   responded to by host
                                                   pParam = NULL           */
} TT2BFFileEventEnum;
typedef uint8 TT2BFFileEvent;

/* File settings */
typedef struct
{
   boolean  storeLocal;            /* TRUE will store locally,
                                      FALSE will store on host using the File callback */
} TT2BFFileSettings;

/* File access */
typedef enum
{
   T2BF_FILE_ACCESS_READ   = 0x00,
   T2BF_FILE_ACCESS_WRITE  = 0x01
} TT2BFFileAccessEnum;
typedef uint8 TT2BFFileAccess;

/* File open params */
typedef struct
{
   uint16 filenameLengthBytes;
   TT2BFFileAccess access;
} TT2BFFileOpen;

/* File IO params */
typedef struct
{
   THandle fileHandle;
   uint16 lengthBytes;
} TT2BFFileIo;

/* File params union */
typedef union
{
   THandle fileHandle;
   TT2BFFileOpen open;
   TT2BFFileIo io;
} TT2BFFileParam;


/*
-------------------------------
   Definitions - SPP
-------------------------------
*/

/* SPP events */
typedef enum
{
   T2BF_SPP_EVENT_DISCONNECTED = 0,
   T2BF_SPP_EVENT_CONNECTION_LOST,
   T2BF_SPP_EVENT_CONNECTED,
   T2BF_SPP_EVENT_DATA
} TT2BFSppEvent;


/*
-------------------------------
   Definitions - MEDIA
-------------------------------
*/

/* Media events */
typedef enum
{
   T2BF_MEDIA_EVENT_DISCONNECTED = 0x01,
   T2BF_MEDIA_EVENT_CONNECTION_LOST,
   T2BF_MEDIA_EVENT_CONNECTED,
   T2BF_MEDIA_EVENT_CONNECTING,
   T2BF_MEDIA_EVENT_CONNECT_TIMEOUT,
   T2BF_MEDIA_EVENT_CONNECT_FAILURE,
   T2BF_MEDIA_EVENT_STREAM_STARTED,
   T2BF_MEDIA_EVENT_STREAM_STOPPED,
   T2BF_MEDIA_EVENT_CONTROL_NOT_AVAILABLE,
   T2BF_MEDIA_EVENT_CONTROL_AVAILABLE
} TT2BFMediaEventEnum;
typedef uint8 TT2BFMediaEvent;

// Shall be same as ibavrcp.h
typedef enum
{
   T2BF_MEDIA_CMD_VOLUP =                0x41,
   T2BF_MEDIA_CMD_VOLDWN  =              0x42,
   T2BF_MEDIA_CMD_MUTE =                 0x43,
   T2BF_MEDIA_CMD_PLAY =                 0x44,
   T2BF_MEDIA_CMD_STOP =                 0x45,
   T2BF_MEDIA_CMD_PAUSE =                0x46,
   T2BF_MEDIA_CMD_REWIND =               0x48,
   T2BF_MEDIA_CMD_FAST_FORWARD =         0x49,
   T2BF_MEDIA_CMD_FORWARD =              0x4B,
   T2BF_MEDIA_CMD_BACK =                 0x4C
} TT2BFMediaCtrlEnum;
typedef uint16 TT2BFMediaCtrlCmd;

/*
-------------------------------
   Definitions - DUN
-------------------------------
*/

/* DUN events */
typedef enum
{
  T2BF_DUN_EVENT_CONNECTING          = 0x01,
  T2BF_DUN_EVENT_CONNECT_TIMEOUT,
  T2BF_DUN_EVENT_CONNECT_FAILURE,
  T2BF_DUN_EVENT_CONNECT_ABORTED,
  T2BF_DUN_EVENT_CONNECTED,
  T2BF_DUN_EVENT_ONLINEDATA_STATE,
  T2BF_DUN_EVENT_DISCONNECTED,
  T2BF_DUN_EVENT_CONNECTION_LOST,
  T2BF_DUN_EVENT_READ_DATA
} TT2BFDunEventEnum;
typedef uint8 TT2BFDunEvent;

/* DUN settings */
typedef struct
{
    TT2BFSecurity sec;    /* If security level is 2, specify exact service  */
                         /* security settings. */

} TT2BFDunSettings;

/* Reset the pack directive */
#pragma pack( )


#if defined( __cplusplus )
extern "C" {
#endif

/******************************************************************************
   Callback TT2BFConvUtf16Cb
******************************************************************************/
typedef TT2BFStatus TT2BFConvUtf16Cb( TString charset, TT2BFEncoding encoding,
                                      uint8* pFrom, uint16 toSize, TUTF16String to );

/******************************************************************************
   Callback TT2BFBtCb
******************************************************************************/
typedef void TT2BFBtCb( TT2BFBtEvent event, TBdAddr bdAddr, TServiceId id );

/******************************************************************************
   Callback TT2BFCallCb
******************************************************************************/
typedef void TT2BFCallCb( TT2BFCallEvent event, uint16 value );

/******************************************************************************
   Callback TT2BFSyncCb
******************************************************************************/
typedef void TT2BFSyncCb( TT2BFSyncEvent event, uint32 value );

/******************************************************************************
   Callback TT2BFPimCb
******************************************************************************/
typedef void TT2BFPimCb( TT2BFPimEvent event, TT2BFPimParam* pParam );

/******************************************************************************
   Callback TT2BFFileCb
******************************************************************************/
typedef void TT2BFFileCb( TT2BFFileEvent event, TT2BFFileParam* pParam );

/******************************************************************************
   Callback TT2BFSppCb
******************************************************************************/
typedef void TT2BFSppCb( TT2BFSppEvent event, TPort port );

/******************************************************************************
   Callback TT2BFMediaCb
******************************************************************************/
typedef void TT2BFMediaCb( THandle handle, TT2BFMediaEvent event );

/******************************************************************************
   Callback TT2BFDunCb
******************************************************************************/
typedef void TT2BFDunCb( TT2BFDunEvent event, uint16 value );

/*
-------------------------------
   Public functions
-------------------------------
*/

/*
-------------------------------
   BT functions
-------------------------------
*/

/******************************************************************************
   Function T2BFInit
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFInit(  TT2BFBtSettings   *pBtSettings,
                                   TT2BFBtCb         *pBtCb );

/******************************************************************************
   Function T2BFInitStore
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFInitStore(  TT2BFSyncSettings *pSyncSettings,
                                        TT2BFPimSettings  *pPimSettings,
                                        TT2BFFileSettings *pFileSettings,
                                        TT2BFSyncCb       *pSyncCb,
                                        TT2BFPimCb        *pPimCb,
                                        TT2BFFileCb       *pFileCb );

/******************************************************************************
   Function T2BFRegisterStrConv
******************************************************************************/
T2BF_EXPORT void T2BFRegisterStrConv( TT2BFConvUtf16Cb *pStrConvCb );

/******************************************************************************
   Function T2BFStart
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFStart( void );

/******************************************************************************
   Function T2BFStop
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFStop( void );

/******************************************************************************
   Function T2BFEnd
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFEnd( void );

/******************************************************************************
   Function T2BFReadLocalBdAddr
******************************************************************************/
T2BF_EXPORT void T2BFReadLocalBdAddr( TBdAddr bdAddr );

/******************************************************************************
   Function T2BFReadSwVersion
******************************************************************************/
T2BF_EXPORT void T2BFReadSwVersion( uint8 length, TString swVersion );

/******************************************************************************
   Function T2BFReadNvsVersion
******************************************************************************/
T2BF_EXPORT void T2BFReadNvsVersion( uint8 length, TString nvsVersion );

/******************************************************************************
   Function T2BFWriteName
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFWriteName( TUTF16String pDeviceName );

/******************************************************************************
   Function T2BFReadName
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFReadName( uint8 maxLength,
                                      TUTF16String pDeviceName );

/******************************************************************************
   Function T2BFReadPaired
******************************************************************************/
T2BF_EXPORT uint8 T2BFReadPaired( uint8 *pMaxPaired );

/******************************************************************************
   Function T2BFReadPairedInfo
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFReadPairedInfo( uint8       prio,
                                            boolean     lastConnected,
                                            uint8       length,
                                            TBdAddr     bdAddr,
                                            TUTF16String pName );

/******************************************************************************
   Function T2BFRemovePaired
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFRemovePaired( TBdAddr bdAddr );

/******************************************************************************
   Function T2BFRemoveAllPaired
******************************************************************************/
T2BF_EXPORT void T2BFRemoveAllPaired( void );

/******************************************************************************
   Function T2BFPairedDeletePolicy
******************************************************************************/
T2BF_EXPORT void T2BFPairedDeletePolicy( TT2BFDeletePolicy policy );

/******************************************************************************
   Function T2BFWritePin
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFWritePin( TString pin );

/******************************************************************************
   Function T2BFPin
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFReadPin( uint8 maxLength, TString pin );

/******************************************************************************
   Function T2BFAuthenticate
******************************************************************************/
T2BF_EXPORT void T2BFAuthenticate( TBdAddr bdAddr, boolean auth );

/******************************************************************************
   Function T2BFAutoAuthentication
******************************************************************************/
T2BF_EXPORT void T2BFAutoAuthentication( TT2BFAutoAuth autoAuth );

/******************************************************************************
   Function T2BFRemoteName
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFRemoteName( TBdAddr     bdAddr,
                                        uint16      maxLength,
                                        TUTF16String pName );

/******************************************************************************
   Function T2BFDiscoverable
******************************************************************************/
T2BF_EXPORT void T2BFDiscoverable( boolean on );

/******************************************************************************
   Function T2BFReadHciChip
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFReadHciChip( uint8   maxLength,
                                         TString hciChip );

/******************************************************************************
   Function T2BFGenericHci
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFGenericHci( uint8    ogf,
                                        uint16   ocf,
                                        uint8    dataLength,
                                        uint8*   pData,
                                        uint8    maxReceiveLength,
                                        uint8*   pReceiveLength,
                                        uint8*   pReceiveBuf );

/******************************************************************************
   Function T2BFInquiryStart
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFInquiryStart( uint8             timeout,
                                          TServiceClass     majorServiceClassFilter,
                                          TDeviceClass      majorDeviceClassFilter,
                                          uint8             maxResults,
                                          uint8             maxNameLength,
                                          TUTF16String      pNames,
                                          TBdAddr*          pBdAddresses,
                                          TT2BFDeviceInfo*  pDeviceInfo );

/******************************************************************************
   Function T2BFInquiryStop
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFInquiryStop( void );

/******************************************************************************
   Function T2BFRemoteServices

   These services are not possible to search for. They will never be showed in
   answer even if they are in servcieFilter.
   SERVICE_ID_OBEX
   SERVICE_ID_OP_CLIENT
   SERVICE_ID_FT_CLIENT
   SERVICE_ID_SYNCML_SERVER
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFRemoteServices( TBdAddr      bdAddr,
                                            TServiceId   serviceFilter,
                                            uint8        timeout );

/******************************************************************************
   Function T2BFSetLowPowerParameters
   ATTENTION: Only to be used in disconnected mode, typically at startup
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSetLowPowerParameters( TT2BFLowPowerParameters lowPowerParam );

/******************************************************************************
   Function T2BFSetLowPowerMode
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSetLowPowerMode( TT2BFLowPowerMode lowPowerMode );


/*
-------------------------------
   CALL functions
-------------------------------
*/

/******************************************************************************
   Function T2BFHandsfreeCreatePort
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHandsfreeCreatePort( TT2BFCallSettings *pCallSettings,
                                                 TT2BFCallCb       *pCallCb );

/******************************************************************************
   Function T2BFHandsfreeDeletePort
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHandsfreeDeletePort( void );

/******************************************************************************
   Function T2BFConnect
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHandsfreeConnect( TBdAddr bdAddr,
                                              uint16 timeout );

/******************************************************************************
   Function T2BFDisconnect
******************************************************************************/
T2BF_EXPORT void T2BFHandsfreeDisconnect( void );

/******************************************************************************
   Function T2BFVoiceDial
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFVoiceDial( boolean on );

/******************************************************************************
   Function T2BFRedial
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFRedial( void );

/******************************************************************************
   Function T2BFAnswer
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFAnswer( void );

/******************************************************************************
   Function T2BFTerminate
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFTerminate( uint8 index );

/******************************************************************************
   Function T2BFTerminateAll
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFTerminateAll( void );

/******************************************************************************
   Function T2BFDialNumber
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFDialNumber( TString number );

/******************************************************************************
   Function T2BFDtmf
   Valid are: '0'-'9', 'a'-'d', '#' and '*'
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFDtmf( uint8 dtmf );

/******************************************************************************
   Function T2BFSwitch
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSwitch( uint8 index );

/******************************************************************************
   Function T2BFConference
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFConference( void );

/******************************************************************************
   Function T2BFPrivateConsultation
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPrivateConsultation( uint8 callIndex );

/******************************************************************************
   Function T2BFTransferAudio
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFTransferAudio( boolean on );

/******************************************************************************
   Function T2BFVolume
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFVolume( uint8 gain );

/******************************************************************************
   Function T2BFSetCharset. Added for debug purpose, should not be used by
                            application for other purpose.
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSetCharset( TCharSet charset );

/******************************************************************************
   Function T2BFReadNumberName
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFReadNumberName( TT2BFNumberType type,
                                            uint16      maxLengthNumber,
                                            uint16      maxLengthName,
                                            uint16*     pIndex,
                                            uint8*      pType,
                                            TString     pNumber,
                                            TUTF16String pName );

/******************************************************************************
   Function T2BFReadAvailableList
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFReadAvailableList( TT2BFListType list, uint16* pStart,
   uint16 *pStop );

/******************************************************************************
   Function T2BFReadList
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFReadList( TT2BFListType list, uint16 index, uint16 count );

/******************************************************************************
   Function T2BFReadOperator
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFReadOperator( boolean     numeric,
                                          uint8       maxLength,
                                          TUTF16String pOperator );

/******************************************************************************
   Function T2BFReadSubscriberNumber
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFReadSubscriberNumber( uint16  maxNumberLength,
                                                  uint8*  pType,
                                                  TString pNumber );

/*
-------------------------------
   SYNC functions
-------------------------------
*/

/******************************************************************************
   Function T2BFSync
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSync( TBdAddr bdAddr, TT2BFSync sync, TT2BFSecurity sec );

/******************************************************************************
   Function T2BFSyncStop
******************************************************************************/
T2BF_EXPORT void T2BFSyncStop( void );

/******************************************************************************
   Function T2BFReadObjectInfo
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFReadObjectInfo( TT2BFObjectInfo type, uint16 maxLength,
                                            uint16* pActualLength, uint8* pData );

/******************************************************************************
   Function T2BFAcceptObject
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFAcceptObject( uint32 counter );

/******************************************************************************
   Function T2BFRejectObject
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFRejectObject( uint32 counter );

/******************************************************************************
   Function T2BFReadSyncSupported
******************************************************************************/
T2BF_EXPORT TTristate T2BFReadSyncSupported( TBdAddr bdAddr, TT2BFSync sync );


/*
-------------------------------
   PIM functions
-------------------------------
*/

/******************************************************************************
   Function T2BFPimPhonebookHandle
******************************************************************************/
T2BF_EXPORT THandle T2BFPimPhonebookHandle( TBdAddr bdAddr );

/******************************************************************************
   Function T2BFPimPhonebookSize
******************************************************************************/
T2BF_EXPORT uint16 T2BFPimPhonebookSize( THandle pbHandle );

/******************************************************************************
   Function T2BFPimDeletePhonebook
   Use T2BF_PIM_DELETE_ALL to delete all phonebooks.
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPimPhonebookDelete( THandle pbHandle );

/******************************************************************************
   Function T2BFPimPhonebookDeleteSync
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPimPhonebookDeleteSync( THandle pbHandle, TT2BFSync sync );

/******************************************************************************
   Function T2BFPimReadInitialCharList
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPimReadInitialCharList( THandle                   pbHandle,
                                                    uint8                     maxListLength,
                                                    uint8*                    pListLength,
                                                    TT2BFPimInitialCharEntry* pEntryList );

/******************************************************************************
   Function T2BFPimSearch
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPimSearch( THandle              pbHandle,
                                       TT2BFPimSearchType   searchType,
                                       TT2BFPimContactField field1,
                                       void*                pPattern1,
                                       TT2BFPimContactField field2,
                                       void*                pPattern2,
                                       uint16               maxResults,
                                       uint16*              pResults,
                                       uint16*              pIndexList );

/******************************************************************************
   Function T2BFPimContactSyncType
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPimContactSyncType( THandle    pbHandle,
                                                uint16     contactId,
                                                TT2BFSync* pSyncType );

/******************************************************************************
   Function T2BFPimContactName
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPimContactName( THandle     pbHandle,
                                            uint16      contactId,
                                            uint16      maxFirstNameLength,
                                            uint16      maxLastNameLength,
                                            TUTF16String pFirstName,
                                            TUTF16String pLastName );

/******************************************************************************
   Function T2BFPimContactNumberCount
******************************************************************************/
T2BF_EXPORT uint8 T2BFPimContactNumberCount( THandle pbHandle,
                                 uint16  contactId );

/******************************************************************************
   Function T2BFPimContactNumber
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPimContactNumber( THandle             pbHandle,
                                              uint16              contactId,
                                              uint8               numberIndex,
                                              uint16              maxNumberLength,
                                              TT2BFPimFieldType*  pNumberType,
                                              uint8*              pType,
                                              TString             pNumber );

/******************************************************************************
   Function T2BFPimContactEmailCount
******************************************************************************/
T2BF_EXPORT uint8 T2BFPimContactEmailCount( THandle pbHandle,
                                            uint16  contactId );

/******************************************************************************
   Function T2BFPimContactEmail
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPimContactEmail( THandle             pbHandle,
                                             uint16              contactId,
                                             uint8               emailIndex,
                                             uint16              maxEmailLength,
                                             TT2BFPimFieldType*  pEmailType,
                                             TUTF16String         pEmail );

/******************************************************************************
   Function T2BFPimContactAddressCount
******************************************************************************/
T2BF_EXPORT uint8 T2BFPimContactAddressCount( THandle pbHandle,
                                              uint16  contactId );

/******************************************************************************
   Function T2BFPimContactAddressType
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPimContactAddressType( THandle            pbHandle,
                                                   uint16             contactId,
                                                   uint8              addressIndex,
                                                   TT2BFPimFieldType* pAddressType );

/******************************************************************************
   Function T2BFPimContactAddressField
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPimContactAddressField( THandle              pbHandle,
                                                    uint16               contactId,
                                                    uint8                addressIndex,
                                                    TT2BFPimContactField field,
                                                    uint16               maxFieldLength,
                                                    TUTF16String          pField );

/******************************************************************************
   Function TT2BFPimContactLabelCount
******************************************************************************/
T2BF_EXPORT uint8 T2BFPimContactLabelCount( THandle pbHandle,
                                uint16  contactId );

/******************************************************************************
   Function T2BFPimContactLabel
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPimContactLabel( THandle            pbHandle,
                                             uint16             contactId,
                                             uint8              labelIndex,
                                             uint16             maxLabelLength,
                                             TT2BFPimFieldType* pLabelType,
                                             TUTF16String        pLabel );

/*
-------------------------------
   PIM HOST functions
-------------------------------
*/

/******************************************************************************
   Function T2BFHostPimSetPhonebook
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHostPimSetPhonebook( THandle handle, boolean created );

/******************************************************************************
   Function T2BFHostPimPhonebookCleared
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHostPimPhonebookCleared( THandle handle );

/******************************************************************************
   Function T2BFHostPimPhonebookDeleted
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHostPimPhonebookDeleted( THandle handle );

/******************************************************************************
   Function T2BFHostPimPutRead
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHostPimPutRead( THandle pb, uint8 bufferSize, uint8 *pBuffer );

/******************************************************************************
   Function T2BFHostPimGetWrite
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHostPimGetWrite( THandle pb, TT2BFPimIoType type,
                                             uint16 bufferSize, uint16 *pBytes,
                                             uint8 *pBuffer );

/******************************************************************************
   Function T2BFHostPimVcardDeleted
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHostPimVcardDeleted( THandle handle );

/******************************************************************************
   Function T2BFHostPimGetAtContact
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHostPimGetAtContact( uint16 maxNumLength, uint16 maxNameLength,
                                                 TString pNumber, TUTF16String pName );

/*
-------------------------------
   FILE functions
-------------------------------
*/

/* Functions for accessing an internally located FMAN (file manager) are
  not defined nor implemented */

/*
-------------------------------
   FILE HOST functions
-------------------------------
*/

/******************************************************************************
   Function T2BFHostFileGetFilename
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHostFileGetFilename( uint16 maxFilenameLengthBytes,
                                                 TUTF16String filename );

/******************************************************************************
   Function T2BFHostFileOpen
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHostFileOpen( THandle fileHandle );

/******************************************************************************
   Function T2BFHostFilePutRead
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHostFilePutRead( THandle fileHandle,
                                             uint16 bufferSize,
                                             uint8 *pBuffer );

/******************************************************************************
   Function T2BFHostFileGetWrite
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHostFileGetWrite( THandle fileHandle, uint16 bufferSize,
                                              uint16 *pBytes, uint8 *pBuffer );

/******************************************************************************
   Function T2BFHostFileClose
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFHostFileClose( THandle fileHandle );


/*
-------------------------------
   SPP functions
-------------------------------
*/

/******************************************************************************
   Function T2BFSppCreatePort
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSppCreatePort( uint16        frameSize,
                               TT2BFSecurity security,
                               TT2BFSppCb    pSppCb,
                               TString       name,
                               TPort*        pPort );

/******************************************************************************
   Function T2BFSppDelete
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSppDelete( TPort port );

/******************************************************************************
   Function T2BFSppOpen
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSppOpen( TPort port );

/******************************************************************************
   Function T2BFSppClose
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSppClose( TPort port );

/******************************************************************************
   Function T2BFSppConnect
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSppConnect( TPort   port,
                            TBdAddr bdAddr,
                            TString remotePortName );

/******************************************************************************
   Function T2BFSppDisconnect
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSppDisconnect( TPort port );

/******************************************************************************
   Function T2BFSppTx
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSppTx( TPort  port,
                       uint16 dataLength,
                       uint8* pData );

/******************************************************************************
   Function T2BFSppRx
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFSppRx( TPort  port,
                       uint16 dataLength,
                       uint8* pData );

/******************************************************************************
   Function T2BFSppRxCount
******************************************************************************/
T2BF_EXPORT uint16 T2BFSppRxCount( TPort port );


/*
-------------------------------
   Media functions
-------------------------------
*/

/******************************************************************************
   Function T2BFMediaCreatePort
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFMediaCreatePort( TT2BFMediaCb *pCb,
                               THandle *pHandle );

/******************************************************************************
   Function T2BFMediaDeletePort
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFMediaDeletePort( THandle handle );

/******************************************************************************
   Function T2BFMediaConnect
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFMediaConnect( THandle handle,
                               TBdAddr bdAddr,
                               uint16 timeout );

/******************************************************************************
   Function T2BFMediaConnect
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFMediaDisconnect( THandle handle );

/******************************************************************************
   Function T2BFMediaControl
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFMediaControl( THandle handle,
                               TT2BFMediaCtrlCmd controlCmd );

/*
-------------------------------
   DUN functions
-------------------------------
*/

/******************************************************************************
   Function T2BFDunConnect
   Input:         bdAddr         BD address of remote unit
                  numberToCall   Numer that the GW should call to start a DUN
                  timeout        Connection timeout in ms
   Returns:       ---
   Description:   Starts a dun connection to a remote device
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFDunConnect(  TT2BFDunCb *pDunCb, TT2BFDunSettings *pDunSettings,
                                         TBdAddr bdAddr, TString numberToCall,
                                         uint32 timeout );

/******************************************************************************
   Function T2BFDunDisconnect
   Returns:       ---
   Description:   Disconnnects a connected remote unit
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFDunDisconnect( void );

/******************************************************************************
   Function T2BFDunRead
   Input:         dataLength  Length of data to receive. Length recived in
                              TDUNCb on read event.
   Output:        pData       Pointer to data to receive.
   Return values:
   Description:   Function is used to receive data from connected DUN.
                  Function will wait until data in received complete or failed.
                  Use datalength value in read cb.
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFDunRead( uint16 dataLength, uint8* pData );

/******************************************************************************
   Function DUNWrite
   Input:         dataLength  Length of data to transmit.
                  pData       Pointer to data to transmit.
   Description:   Function is used to transmit data to a connected DUN.
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFDunWrite( uint16 dataLength, uint8* pData );


/*
-------------------------------------------------------------------------
   PTS functions. Only for test purpose.
   Needed to be able to perform PTS testcases. Should not be used by the
   application.
-------------------------------------------------------------------------
*/

/******************************************************************************
   Function T2BFPtsChld
   Input:         command        CHLD-command that should be sent. Valid values
                                 are 0-4, for more information see handsfree
                                 specification.
                  useId          Indicates if call id should be added to the
                                 command.
                  callId         Call id.
   Return value:
   Description:   Should be used for testing only!! Sends a CHLD-command over
                  HFP (optionally with call id).
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPtsChld( uint8 command, boolean useId, uint8 callId );

/******************************************************************************
   Function T2BFPtsStartClcc
   Return value:
   Description:   Should be used for testing only!! Sends a CLCC-command over
                  HFP. If successful, continuing CLCC commands will be sent
                  during call activity.
******************************************************************************/
T2BF_EXPORT TT2BFStatus T2BFPtsStartClcc( void );

#if defined( __cplusplus )
}
#endif

#endif

