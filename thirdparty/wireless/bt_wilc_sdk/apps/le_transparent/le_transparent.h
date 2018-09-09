
// binary representation
// attribute size in bytes (16), flags(16), handle (16), uuid (16/128), value(...)

#include <stdint.h>

const uint8_t profile_data[] =
{
    // 0x0001 PRIMARY_SERVICE-GAP_SERVICE
    0x0a, 0x00, 0x02, 0x00, 0x01, 0x00, 0x00, 0x28, 0x00, 0x18, 
    // 0x0002 CHARACTERISTIC-GAP_DEVICE_NAME-READ
    0x0d, 0x00, 0x02, 0x00, 0x02, 0x00, 0x03, 0x28, 0x02, 0x03, 0x00, 0x00, 0x2a, 
    // 0x0003 VALUE-GAP_DEVICE_NAME-READ-'LE Counter'
    0x16, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x2a, 0x4c, 0x45, 0x5F, 0x54, 0x72, 0x61, 0x6e, 0x73, 0x70, 0x61, 0x72, 0x65, 0x6e, 0x74, 
    
	// 0x0004 PRIMARY_SERVICE-GATT_SERVICE
	0x0a, 0x00, 0x02, 0x00, 0x04, 0x00, 0x00, 0x28, 0x01, 0x18,
	// 0x0005 CHARACTERISTIC-GATT_SERVICE_CHANGED-READ
	0x0d, 0x00, 0x02, 0x00, 0x05, 0x00, 0x03, 0x28, 0x02, 0x06, 0x00, 0x05, 0x2a,
	// 0x0006 VALUE-GATT_SERVICE_CHANGED-READ-''
	0x08, 0x00, 0x02, 0x00, 0x06, 0x00, 0x05, 0x2a,
	
	// add Transparent Service
	// 0x0007 PRIMARY_SERVICE-Transparent Service
	0x18, 0x00, 0x02, 0x00, 0x07, 0x00, 0x00, 0x28, 0x55, 0xE4, 0x05, 0xD2, 0xAF, 0x9F, 0xA9, 0x8F, 0xE5, 0x4A, 0x7D, 0XFE, 0x43, 0x53, 0x53, 0x49,
	// 0x0008 Transparent TX CHARACTERISTIC-49535343-1E4D-4BD9-BA61-23C647249616 - NOTIFY | WRITE | WRITE WITHOUT RESPONSE
	0x1B, 0x00, 0x02, 0x00, 0x08, 0x00, 0x03, 0x28, 0x1C, 0x09, 0x00, 0x16, 0x96, 0x24, 0x47, 0xC6, 0x23, 0x61, 0xBA, 0xD9, 0x4B, 0x4D, 0x1E, 0x43, 0x53, 0x53, 0x49,
	// 0x0009 VALUE-49535343-1E4D-4BD9-BA61-23C647249616 |NOTIFY | WRITE | WRITE WITHOUT RESPONSE
	0x16, 0x00, 0x1C, 0x01, 0x09, 0x00, 0x16, 0x96, 0x24, 0x47, 0xC6, 0x23, 0x61, 0xBA, 0xD9, 0x4B, 0x4D, 0x1E, 0x43, 0x53, 0x53, 0x49,
	// 0x000A CLIENT_CHARACTERISTIC_CONFIGURATION
	0x0a, 0x00, 0x1A, 0x01, 0x0A, 0x00, 0x02, 0x29, 0x00, 0x00,
	// 0x000B Transparent RX CHARACTERISTIC-49535343-8841-43F4-A8D4-ECBE34729BB3 - WRITE | WRITE WITHOUT RESPONSE
	0x1B, 0x00, 0x02, 0x00, 0x0B, 0x00, 0x03, 0x28, 0x0C, 0x0C, 0x00, 0xB3, 0x9B, 0x72, 0x34, 0xBE, 0xEC, 0xD4, 0xA8, 0xF4, 0x43, 0x41, 0x88, 0x43, 0x53, 0x53, 0x49,
	// 0x000C VALUE-49535343-8841-43F4-A8D4-ECBE34729BB3 WRITE | WRITE WITHOUT RESPONSE
	0x16, 0x00, 0x0C, 0x01, 0x0C, 0x00, 0xB3, 0x9B, 0x72, 0x34, 0xBE, 0xEC, 0xD4, 0xA8, 0xF4, 0x43, 0x41, 0x88, 0x43, 0x53, 0x53, 0x49,
	// 0x000D Transparent Control Point CHARACTERISTIC-49535343-4C8A-39B3-2F49-511CFF073B7E - NOTIFY | WRITE
	0x1B, 0x00, 0x02, 0x00, 0x0D, 0x00, 0x03, 0x28, 0x18, 0x0E, 0x00, 0x7e, 0x3b, 0x07, 0xff, 0x1c, 0x51, 0x49, 0x2f, 0xb3, 0x39, 0x8a, 0x4c, 0x43, 0x53, 0x53, 0x49,
	// 0x000E VALUE-49535343-4C8A-39B3-2F49-511CFF073B7E - NOTIFY | WRITE
	0x16, 0x00, 0x18, 0x01, 0x0E, 0x00, 0x7e, 0x3b, 0x07, 0xff, 0x1c, 0x51, 0x49, 0x2f, 0xb3, 0x39, 0x8a, 0x4c, 0x43, 0x53, 0x53, 0x49,
	// 0x000F CLIENT_CHARACTERISTIC_CONFIGURATION
	0x0a, 0x00, 0x1A, 0x01, 0x0F, 0x00, 0x02, 0x29, 0x00, 0x00,

    // END
    0x00, 0x00, 
}; // total size 122 bytes 


#define TSS_RES_SUCCESS						0x0000									/**< Execution successfully. */
#define TSS_RES_FAIL						0x0001									/**< Execution fail. */
#define ATT_CHARACTERISTIC_TX_CHARATERISITC_VALUE_HANDLE 0x0009
#define ATT_CHARACTERISTIC_TX_CHARATERISITC_CCD_VALUE_HANDLE 0x000A
#define ATT_CHARACTERISTIC_RX_CHARATERISITC_VALUE_HANDLE 0x000C
#define ATT_CHARACTERISTIC_CONTROL_POINT_CHARATERISITC_VALUE_HANDLE 0x000E
#define ATT_CHARACTERISTIC_CONTROL_POINT_CHARATERISITC_CCD_VALUE_HANDLE 0x000F
#define TR_READ_MTU_SIZE_AND_ENABLE_CREDIT_CTRL			0x14	/**< Definition of Op Code for Credit Based Flow Control Protocol,sending by Server. */
#define CREDIT_CTRL_STATUS_SUCCESS					0	/**< Definition of Response for successful operation. */
#define CREDIT_BASED_FLOW_CONTROL_DISABLED				0		/**< Definition of CCCD Disable. */
#define CREDIT_BASED_FLOW_CONTROL_ENABLED				1		/**< Definition of CCCD Enable. */
#define MAX_PACKET_BUFFER_LIST		8	/**< Number of PacketOut/PacketIn. */
#define BLE_GAP_ROLE_SLAVE                   		0x01

typedef enum 
{
	TSS_IDLE,
	TSS_RETURN_CREDIT_S_TO_C,
	TSS_RETURN_CREDIT_C_TO_S,
	TSS_RETURN_DATA_S_TO_C
}tss_state_t;

/**@brief The structure of PacketOut/PacketIn. */

typedef struct Packet_Buffer
{
	uint8_t length;			/**< Data length. */
	uint8_t *packet;		/**< Pointer to the data buffer */
}Packet_Buffer;

/** 
 * @brief Device State Definitions
 */
typedef enum
{
	APP_STATE_OFF,				/**< Application State: OFF. */
	APP_STATE_STANDBY,			/**< Application State: Standby. */
	APP_STATE_LE_CONNECTED,		/**< Application State: LE Connected. */
	APP_STATE_LE_OPEN,			/**< Application State: Transparent Data Session Open. */
	SPP_STATE_TOTAL				/**< Application State: End of the State Definition. */
}app_state_type;

/**@brief The structure of . */
typedef struct App_List
{
	uint8_t  outReadIndex;			/**< The Index of Read Data from PacketOut Buffer. */
	uint8_t  outWriteIndex;			/**< The Index of Data, Written in PacketOut Buffer. */
	uint8_t  outUsedNum;			/**< The number of Data list of PacketOut Buffer. */
	uint8_t  inReadIndex;			/**< The Index of Read Data from PacketIn Buffer. */
	uint8_t  inWriteIndex;			/**< The Index of Data, Written in PacketIn Buffer. */
	uint8_t  inUsedNum;				/**< The number of Data list of PacketIn Buffer. */
	uint8_t  appState;				/**< Application State. */
	uint16_t attMtu;				/**< GATT MTU Length. */
	uint16_t connHandle;			/**< Connection Handle. */
	uint8_t  role;					/**< GAP role in connection state . */
	uint8_t  creditBasedEnable;		/**< Credit Based Flow Enable. */
	uint8_t  credit;				/**< Credit number. */
	uint8_t  creditBasedStoMEnable;	/**< Credit Based Flow Enable. */
	uint8_t  creditStoM;			/**< Credit number. */
	uint8_t  writeFlag;				/**< APP Transparent Service State. */
	uint8_t  updateConnPara;		/**< Request Connection Parameters Update. */
}App_List;

/**
 * @brief  The struct of handle list.
 */
typedef struct Handle_List
{
	uint16_t	transTx;			/**< Handle of transparent tx. */
	uint16_t	txCCCD;				/**< Handle of transparent tx cccd. */
	uint16_t	transRx;			/**< Handle of transparent rx. */
	uint16_t	transCtrl;			/**< Handle of transparent control. */
	uint16_t	ctrlCCCD;			/**< Handle of transparent control cccd. */
}Handle_List;


void app_master_initiate_credit_flow_control(void);
void process_data(void);
void app_master_check_credit(uint8_t *payload);
void app_master_enable_credit_flow_control(void);
void le_tss_start_scanning(void);
void tss_start_advertising(void);
void push_into_transmit_buffer(uint8_t len, uint8_t * buf);
void app_process_incomming_uart(void);
void app_process_incomming_packet(void);
void tss_app_initialize(void);
void tss_app_setup(void);