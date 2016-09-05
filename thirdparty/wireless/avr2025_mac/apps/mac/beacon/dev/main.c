/**
 * @file main.c
 *
 * @brief MAC Example Beacon Application - Device
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 */

/**
 * \mainpage
 * \section preface Preface
 * This is the reference manual for the IEEE 802.15.4 MAC Beacon Application -
 * Device
 * \section main_files Application Files
 *      - main.c                 Application main file.
 * \section intro Application Introduction
 * The basic MAC Example Beacon Application deploys a beacon-enabled network
 * with star topology.
 * The coordinator starts a PAN at channel DEFAULT_CHANNEL with the PAN ID
 * DEFAULT_PAN_ID.
 *
 * The coordinator starts a beaconing network and transmits user data within
 * beacon payload of transmitted beacon frames.
 * The device scans for this network,sync up with the coordinator and associates
 * to the coordinator.The devices receive
 * these beacon frames, extract the receveived user data from the coordinator
 *,print the received data on the hyper
 * terminal and also sends the extracted beacon payload back to the coordinator.
 * The coordinator also transmits broadcast data frames periodically.The devices
 * receive these broadcast data frames
 * and increase a counter.
 * The coordinator also puts the data in the indirect queue periodically and
 * transmits data frames based on the
 * periodic poll request from the device.
 * The results of the proper data transmission/reception are printed to a
 * terminal program via Serial I/O (UART or USB).
 * For demonstration purposes the coordinator's configuration is limited to
 * associate maximum of 100 devices.
 * \note For Two Processor Approach the Application needs to be flashed in the
 * Host board(eg.SAM4L-Xplained Pro) and the Serial-if application (Beacon
 * FFD)(MAC Stack)
 * needs to be flashed in the NCP(Network CoProcessor) board.
 * \section api_modules Application Dependent Modules
 * - \subpage api
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR .
 * Other compilers may or may not work.
 *
 * \section references References
 * 1)  IEEE Std 802.15.4-2006 Part 15.4: Wireless Medium Access Control (MAC)
 *     and Physical Layer (PHY) Specifications for Low-Rate Wireless Personal
 * Area
 *     Networks (WPANs).\n\n
 * 2)  <a href="http://www.atmel.com/design-support/">Atmel Design
 * Support</a>.\n
 *
 * \section contactinfo Contact Information
 * For further information,visit
 * <A href="http://www.atmel.com/avr">www.atmel.com</A>.\n
 */

#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include "conf_board.h"
#include "avr2025_mac.h"
#include "delay.h"
#include "common_sw_timer.h"
#include "sio2host.h"
#include "common_hw_timer.h"
#include "beacon_app.h"
#include <asf.h>

/* === MACROS ============================================================== */
/** Channel Offset will give us the channel number as (CHANNEL_OFFSET + 11) */
#define CHANNEL_OFFSET                  (0)

/** Defines the short scan duration time. */
#define SCAN_DURATION_SHORT             (5)
/** Defines the long scan duration time. */
#define SCAN_DURATION_LONG              (6)

/*
 * This is the timeout in ms once the node tries to synchronize with the
 * coordinator. This timeout is set so that the node should be able to receive
 * at least a beacon frame from its parent.
 * In case the Beacon Order is increased to a high value, this timeout might
 * need to be increased as well.
 */
#define TIMER_SYNC_BEFORE_ASSOC_MS      (3000)
#define APP_GUARD_TIME_US               (10000)
#define MCU_WAKEUP_TIME                 (150)
#define PAYLOAD_LEN                     (104)

#ifdef GTS_SUPPORT
#define GTS_PAYLOAD_LEN                 (18)
#endif
#if (LED_COUNT >= 3)
#define LED_START                       (LED0)
#define LED_NWK_SETUP                   (LED1)
#define LED_DATA                        (LED2)
#elif (LED_COUNT == 2)
#define LED_START                       (LED0)
#define LED_NWK_SETUP                   (LED0)
#define LED_DATA                        (LED1)
#else
#define LED_START                       (LED0)
#define LED_NWK_SETUP                   (LED0)
#define LED_DATA                        (LED0)
#define LED_DATA                        (LED0)
#endif

/* === GLOBALS ============================================================= */
extern uint8_t default_key_source[8];
/* This structure stores the short and extended address of the coordinator. */
wpan_addr_spec_t coord_addr_spec;

/* This variable counts the number of received broadcast data frames. */
static uint32_t bc_rx_cnt;

/* This variable counts the number of received indirect data frames. */
static uint32_t indirect_rx_cnt;
/** This variable stores the current state of the node. */
app_state_t app_state = APP_IDLE;
static uint8_t APP_TIMER;
#ifdef RTC_SLEEP
struct rtc_module rtc_instance;
#endif
/* RTC Configuration and Callback functions */

/** This function shows the stack and application
 *  capabilities on terminal if SIO_HUB switch
 *  is enabled.
 */
static void print_stack_app_build_features(void);

#ifdef RTC_SLEEP

/**
 * @brief Configuring RTC Callback Funtion on Overflow
 *
 * @param void
 */
void configure_rtc_callbacks(void);

/**
 * @brief Configures and enables RTC count value
 *
 * @param void
 */
void configure_rtc_count(void);

/**
 * @brief Callback Function indicating RTC Overflow
 *
 * @param void
 */
void rtc_overflow_callback(void);

#endif

/** This array stores the current msdu payload. */
static uint8_t msdu_payload[PAYLOAD_LEN];

uint8_t current_channel;
uint8_t current_channel_page;
static uint32_t channels_supported;

#if (defined ENABLE_SLEEP || defined RTC_SLEEP)

/**
 * @brief API called to put the controller Power save mode with 4MHz timer
 *
 * @param timeout Time duration in microseconds for which controller can sleep
 */
static void  enter_sleep(uint32_t timeout);

/**
 * @brief Callback function to wakeup and switching to the default clock source
 * for starting MAC timers
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
void wakeup_cb(void *parameter);

/* Residual time returned to MAC for Synchronization after wakeup,When Switched
 * to Low precision clock during sleep*/
static uint32_t res;
/* This variable denotes the sleep duration returned from MAC*/
uint32_t sleep_time = 0;
/* Sleep State of the Application and MAC*/
bool sys_sleep = false;
#endif

#ifdef ENABLE_SLEEP
/* Timer for Handling Application Sleep*/
static uint8_t APP_TIMER_SLEEP;
extern void hw_overflow_cb();
extern void hw_expiry_cb();

#endif

#if (defined ENABLE_SLEEP || defined RTC_SLEEP)
#undef SIO_HUB
#endif

#ifdef MAC_SECURITY_ZIP
uint8_t deviceShortAddress = 0xFF;
#endif

/* === PROTOTYPES ========================================================== */

/**
 * @brief Callback function indicating network search
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void network_search_indication_cb(void *parameter);

/**
 * @brief Callback function for switching off the LED after data reception
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void rx_data_led_off_cb(void *parameter);

/** Alert to indicate something has gone wrong in the application */
static void app_alert(void);

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Main function of the device application
 */
int main(void)
{
	irq_initialize_vectors();
	#if SAMD || SAMR21
	system_init();
	delay_init();
	#else
	sysclk_init();

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();
	#endif
#ifdef SIO_HUB
	sio2host_init();
#endif
	sw_timer_init();

	if (MAC_SUCCESS != wpan_init()) {
		app_alert();
	}

	/* Initialize LEDs. */
	LED_On(LED_START);     /* indicating application is started */
	LED_Off(LED_NWK_SETUP); /* indicating network is started */
	LED_Off(LED_DATA);     /* indicating data transmission */
	cpu_irq_enable();
#ifdef SIO_HUB

	/* Initialize the serial interface used for communication with terminal
	 * program. */

	/* To Make sure the Hyper Terminal to the System */
	sio2host_getchar();

	printf("\nBeacon_Application\r\n\n");
	printf("\nDevice\r\n\n");
	print_stack_app_build_features();
#endif

	sw_timer_get_id(&APP_TIMER);

	wpan_mlme_reset_req(true);
    #ifdef ENABLE_SLEEP
	sw_timer_get_id(&APP_TIMER_SLEEP);
    #endif
	LED_Off(LED_NWK_SETUP);
	while (true) {
		wpan_task();
		#if (defined ENABLE_SLEEP || defined RTC_SLEEP)
		/* Requesting MAC for Sleep Duration*/
		sleep_time = mac_ready_to_sleep();
		if ((sleep_time > (uint32_t)APP_GUARD_TIME_US)) {
			/*Entering Power save Mode when the sleep duration is
			 * above the guard time*/
			enter_sleep(sleep_time);
		}
		#endif
	}
}

#if defined(ENABLE_TSTAMP)

/*
 * Callback function usr_mcps_data_conf
 *
 * @param msduHandle  Handle of MSDU handed over to MAC earlier
 * @param status      Result for requested data transmission request
 * @param Timestamp   The time, in symbols, at which the data were transmitted
 *                    (only if time stamping is enabled).
 *
 */
void usr_mcps_data_conf(uint8_t msduHandle,
		uint8_t status,
		uint32_t Timestamp)
#else
void usr_mcps_data_conf(uint8_t msduHandle,
		uint8_t status)
#endif  /* ENABLE_TSTAMP */
{
}

#ifdef GTS_SUPPORT
void usr_mlme_gts_ind(uint16_t DeviceAddr, gts_char_t GtsChar)
{
	DeviceAddr = DeviceAddr;
	GtsChar = GtsChar;
}

#endif

#ifdef GTS_SUPPORT
void usr_mlme_gts_conf(gts_char_t GtsChar, uint8_t status)
{
	GtsChar = GtsChar;
	status = status;
}

#endif

/*
 * @brief Callback function usr_mcps_data_ind
 *
 * @param SrcAddrSpec      Pointer to source address specification
 * @param DstAddrSpec      Pointer to destination address specification
 * @param msduLength       Number of octets contained in MSDU
 * @param msdu             Pointer to MSDU
 * @param mpduLinkQuality  LQI measured during reception of the MPDU
 * @param DSN              DSN of the received data frame.
 * @param Timestamp        The time, in symbols, at which the data were
 * received.
 *                         (only if timestamping is enabled).
 */
#ifdef MAC_SECURITY_ZIP
void usr_mcps_data_ind(wpan_addr_spec_t *SrcAddrSpec,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t mpduLinkQuality,
		uint8_t DSN,
#ifdef ENABLE_TSTAMP
		uint32_t Timestamp,
				#endif  /* ENABLE_TSTAMP */
		uint8_t SecurityLevel,
		uint8_t KeyIdMode,
		uint8_t KeyIndex)
#else /* No MAC_SECURITY */

void usr_mcps_data_ind(wpan_addr_spec_t *SrcAddrSpec,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t msduLength,
		uint8_t *msdu,
		uint8_t mpduLinkQuality,
#ifdef ENABLE_TSTAMP
		uint8_t DSN,
		uint32_t Timestamp)
#else
		uint8_t DSN)
#endif  /* ENABLE_TSTAMP */
#endif
{
#ifdef SIO_HUB
	/* char sio_array[255]; */
#ifdef SIO_HUB
	const char Display_Received_Frame[] = "Frame received: %lu\r\n";
#endif
#endif

	if (DstAddrSpec->Addr.short_address == BROADCAST) {
		bc_rx_cnt++;
#ifdef SIO_HUB
		printf("Broadcast ");
		printf(Display_Received_Frame, bc_rx_cnt);
#endif
	} else {
		indirect_rx_cnt++;
#ifdef SIO_HUB
		printf("Frame received: ");
		for (uint8_t i = 0; i < msduLength; i++) {
			printf("%c", msdu[i]);
		}
		printf("\n\r");
#endif
	}

	/*
	 * Dummy data has been received successfully.
	 * Application code could be added here ...
	 */
	LED_On(LED_DATA);

	/* Start a timer switching off the LED. */
	sw_timer_start(APP_TIMER,
			500000,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)rx_data_led_off_cb,
			NULL);

	/* Keep compiler happy. */
	SrcAddrSpec = SrcAddrSpec;
	DstAddrSpec = DstAddrSpec;
	msduLength = msduLength;
	msdu = msdu;
	mpduLinkQuality = mpduLinkQuality;
	DSN = DSN;
#ifdef ENABLE_TSTAMP
	Timestamp = Timestamp;
#endif  /* ENABLE_TSTAMP */
}

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))

/*
 * Callback function usr_mcps_purge_conf
 *
 * @param msduHandle           Handle (id) of MSDU to be purged.
 * @param status               Result of requested purge operation.
 *
 * @return void
 *
 */
void usr_mcps_purge_conf(uint8_t msduHandle,
		uint8_t status)
{
}

#endif  /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
        **/

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)

/*
 * @brief Callback function usr_mlme_associate_conf
 *
 * @param AssocShortAddress    Short address allocated by the coordinator
 * @param status               Result of requested association operation
 */
void usr_mlme_associate_conf(uint16_t AssocShortAddress,
		uint8_t status)
{
	if (status == MAC_SUCCESS) {
#ifdef SIO_HUB
		printf("Connected to beacon-enabled network\r\n");
#endif
		/* Set proper state of application. */
		app_state = APP_DEVICE_RUNNING;

		/* Stop timer used for search indication */
		sw_timer_stop(APP_TIMER);

		LED_On(LED_NWK_SETUP);
#ifdef MAC_SECURITY_ZIP
		uint16_t mac_dev_table_entries = 3;

		wpan_mlme_set_req(macDeviceTableEntries,
				NO_PIB_INDEX,
				&mac_dev_table_entries);
#endif
	#ifdef GTS_SUPPORT
		gts_char_t gts_spec;
		gts_spec.GtsLength = 2;
		gts_spec.GtsDirection = GTS_RX_SLOT;
		gts_spec.GtsCharType = GTS_ALLOCATE;
		wpan_mlme_gts_req(AssocShortAddress, gts_spec);
	#endif
	} else {
		LED_Off(LED_NWK_SETUP);

		/* Set proper state of application. */
		app_state = APP_IDLE;

		/* Something went wrong; restart. */
		wpan_mlme_reset_req(true);
	}

	/* Keep compiler happy. */
	AssocShortAddress = AssocShortAddress;
#ifdef MAC_SECURITY_ZIP
	deviceShortAddress = AssocShortAddress;
#endif
}

#endif  /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)

/*
 * @brief Callback function usr_mlme_associate_ind
 *
 * @param DeviceAddress         Extended address of device requesting
 * association
 * @param CapabilityInformation Capabilities of device requesting association
 */
void usr_mlme_associate_ind(uint64_t DeviceAddress,
		uint8_t CapabilityInformation)
{
	/* Keep compiler happy. */
	DeviceAddress = DeviceAddress;
	CapabilityInformation = CapabilityInformation;
}

#endif  /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_BEACON_NOTIFY_INDICATION == 1)

/*
 * Callback function usr_mlme_beacon_notify_ind
 *
 * @param BSN            Beacon sequence number.
 * @param PANDescriptor  Pointer to PAN descriptor for received beacon.
 * @param PendAddrSpec   Pending address specification in received beacon.
 * @param AddrList       List of addresses of devices the coordinator has
 * pending data.
 * @param sduLength      Length of beacon payload.
 * @param sdu            Pointer to beacon payload.
 */
void usr_mlme_beacon_notify_ind(uint8_t BSN,
		wpan_pandescriptor_t *PANDescriptor,
		uint8_t PendAddrSpec,
		uint8_t *AddrList,
		uint8_t sduLength,
		uint8_t *sdu)
{
	if (APP_SCAN_DONE == app_state) {
		/* Set proper state of application. */
		app_state = APP_ASSOC_IN_PROGRESS;

		/*
		 * Associate to our coordinator.
		 * Use: bool wpan_mlme_associate_req(uint8_t LogicalChannel,
		 *                                   uint8_t ChannelPage,
		 *                                   wpan_addr_spec_t
		 ******CoordAddrSpec,
		 *                                   uint8_t
		 * CapabilityInformation);
		 * This request will cause a mlme associate confirm message ->
		 * usr_mlme_associate_conf.
		 */
		wpan_mlme_associate_req(current_channel,
				current_channel_page,
				&coord_addr_spec,
				WPAN_CAP_ALLOCADDRESS);
	} else if (APP_DEVICE_RUNNING == app_state) {
		/* This is the standard portion once the node is associated
		 * with the application.
		 */

		/*
		 * Extract the beacon payload from our coordinator and feed it
		 * back
		 * to the coordinator via a data frame.
		 */

		/* Use: bool wpan_mcps_data_req(uint8_t SrcAddrMode,
		 *                              wpan_addr_spec_t *DstAddrSpec,
		 *                              uint8_t msduLength,
		 *                              uint8_t *msdu,
		 *                              uint8_t msduHandle,
		 *                              uint8_t TxOptions);
		 *
		 * This request will cause a mcps data confirm message ->
		 * usr_mcps_data_conf
		 */
		if (sduLength > PAYLOAD_LEN) {
			sduLength = PAYLOAD_LEN;
		}

		/* Copy payload from beacon frame. */
		memcpy(&msdu_payload, sdu, sduLength);

		uint8_t src_addr_mode = WPAN_ADDRMODE_SHORT;
		static uint8_t msdu_handle = 0;

		msdu_handle++;      /* Increment handle */

#ifdef MAC_SECURITY_ZIP
		wpan_mcps_data_req(src_addr_mode,
				&coord_addr_spec,
				sduLength,  /* msduLength */
				&msdu_payload[0],
				msdu_handle,
				WPAN_TXOPT_ACK,
				ZIP_SEC_MIN,     /* SecurityLevel */
				NULL,
				ZIP_KEY_ID_MODE, /* KeyIdMode */
				deviceShortAddress);  /* KeyIndex */
#else
		wpan_mcps_data_req(src_addr_mode,
				&coord_addr_spec,
				sduLength,
				&msdu_payload[0],
				msdu_handle,
				WPAN_TXOPT_ACK);
#endif

#ifdef SIO_HUB
		{
			static uint32_t rx_cnt;
			const char Rx_Beacon_Payload[]
				= "Rx beacon payload (%lu): ";

			/* Print received payload. */
			rx_cnt++;
			printf(Rx_Beacon_Payload, rx_cnt);

			/* Set last element to 0. */
			if (sduLength == PAYLOAD_LEN) {
				msdu_payload[PAYLOAD_LEN - 1] = '\0';
			} else {
				msdu_payload[sduLength] = '\0';
			}

			for (uint8_t i = 0; i < sduLength; i++) {
				printf("%c", msdu_payload[i]);
			}
			printf("\r\n");
		}
#endif  /* SIO_HUB */
	}

	/* Keep compiler happy. */
	BSN = BSN;
	PANDescriptor = PANDescriptor;
	PendAddrSpec = PendAddrSpec;
	AddrList = AddrList;
}

#endif  /* (MAC_BEACON_NOTIFY_INDICATION == 1) */

#if ((MAC_ORPHAN_INDICATION_RESPONSE == 1) || \
	(MAC_ASSOCIATION_INDICATION_RESPONSE == 1))

/*
 * @brief Callback function usr_mlme_comm_status_ind
 *
 * @param SrcAddrSpec      Pointer to source address specification
 * @param DstAddrSpec      Pointer to destination address specification
 * @param status           Result for related response operation
 */
void usr_mlme_comm_status_ind(wpan_addr_spec_t *SrcAddrSpec,
		wpan_addr_spec_t *DstAddrSpec,
		uint8_t status)
{
	if (status == MAC_SUCCESS) {
		/*
		 * Now the association of the device has been successful and its
		 * information, like address, could  be stored.
		 * But for the sake of simple handling it has been done
		 * during assignment of the short address within the function
		 * assign_new_short_addr()
		 */
	}

	/* Keep compiler happy. */
	SrcAddrSpec = SrcAddrSpec;
	DstAddrSpec = DstAddrSpec;
}

#endif  /* ((MAC_ORPHAN_INDICATION_RESPONSE == 1) ||
         *(MAC_ASSOCIATION_INDICATION_RESPONSE == 1)) */

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/*
 * Callback function usr_mlme_disassociate_conf
 *
 * @param status             Result of requested disassociate operation.
 * @param DeviceAddrSpec     Pointer to wpan_addr_spec_t structure for device
 *                           that has either requested disassociation or been
 *                           instructed to disassociate by its coordinator.
 *
 * @return void
 */
void usr_mlme_disassociate_conf(uint8_t status,
		wpan_addr_spec_t *DeviceAddrSpec)
{
}

#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)*/

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)

/*
 * Callback function usr_mlme_disassociate_ind
 *
 * @param DeviceAddress        Extended address of device which initiated the
 *                             disassociation request.
 * @param DisassociateReason   Reason for the disassociation. Valid values:
 *                           - @ref WPAN_DISASSOC_BYPARENT,
 *                           - @ref WPAN_DISASSOC_BYCHILD.
 *
 * @return void
 */
void usr_mlme_disassociate_ind(uint64_t DeviceAddress,
		uint8_t DisassociateReason)
{
}

#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_GET_SUPPORT == 1)

/*
 * Callback function usr_mlme_get_conf
 *
 * @param status            Result of requested PIB attribute get operation.
 * @param PIBAttribute      Retrieved PIB attribute.
 * @param PIBAttributeIndex Index of the PIB attribute to be read.
 * @param PIBAttributeValue Pointer to data containing retrieved PIB attribute,
 *
 * @return void
 */
void usr_mlme_get_conf(uint8_t status,
		uint8_t PIBAttribute,
#ifdef MAC_SECURITY_ZIP
		uint8_t PIBAttributeIndex,
#endif  /* MAC_SECURITY_ZIP */
		void *PIBAttributeValue)
{
#ifdef MAC_SECURITY_ZIP
	mac_key_table_t *key_table = (mac_key_table_t *)PIBAttributeValue;
#endif
	if ((status == MAC_SUCCESS) && (PIBAttribute == phyCurrentPage)) {
	#ifdef HIGH_DATA_RATE_SUPPORT
		current_channel_page = 17;
	#else
		current_channel_page = *(uint8_t *)PIBAttributeValue;
	#endif
#ifdef MAC_SECURITY_ZIP
		wpan_mlme_get_req(phyChannelsSupported, NO_PIB_INDEX);
#else
		wpan_mlme_get_req(phyChannelsSupported);
#endif
	} else if ((status == MAC_SUCCESS) &&
			(PIBAttribute == phyChannelsSupported)) {
		uint8_t index;

		channels_supported = convert_byte_array_to_32_bit(
				PIBAttributeValue);

		for (index = 0; index < 32; index++) {
			if (channels_supported & (1 << index)) {
				current_channel = index + CHANNEL_OFFSET;
				break;
			}
		}
#ifdef SIO_HUB
		printf("Searching network in Channel - %d\r\n",
				current_channel);
#endif

		/*
		 * Initiate an active scan over all channels to determine
		 * which channel is used by the coordinator.
		 * Use: bool wpan_mlme_scan_req(uint8_t ScanType,
		 *                              uint32_t ScanChannels,
		 *                              uint8_t ScanDuration,
		 *                              uint8_t ChannelPage);
		 *
		 * This request leads to a scan confirm message ->
		 * usr_mlme_scan_conf
		 * Scan for about 50 ms on each channel -> ScanDuration = 1
		 * Scan for about 1/2 second on each channel -> ScanDuration = 5
		 * Scan for about 1 second on each channel -> ScanDuration = 6
		 */
		wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
				SCAN_CHANNEL(current_channel),
				SCAN_DURATION_LONG,
				current_channel_page);

		/* Indicate network scanning by a LED flashing. */
		sw_timer_start(APP_TIMER,
				500000,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)network_search_indication_cb,
				NULL);
	}

#ifdef MAC_SECURITY_ZIP
	else if ((status == MAC_SUCCESS) &&
			(PIBAttribute == macKeyTable)) {
		for (uint8_t i = 0; i < key_table->KeyDeviceListEntries; i++) {
			if (EMPTY_DEV_HANDLE ==
					(key_table->KeyDeviceList[i].
					DeviceDescriptorHandle)) {
				key_table->KeyDeviceList[i].
				DeviceDescriptorHandle = 0x00;
				key_table->KeyDeviceList[i].UniqueDevice = true;
				break;
			}
		}
		wpan_mlme_set_req(macKeyTable,
				deviceShortAddress - 1,
				(uint8_t *)PIBAttributeValue);
	}
#endif
}

#endif  /* (MAC_GET_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)

/*
 * Callback function usr_mlme_orphan_ind
 *
 * @param OrphanAddress     Address of orphaned device.
 *
 * @return void
 *
 */
void usr_mlme_orphan_ind(uint64_t OrphanAddress)
{
}

#endif  /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)

/*
 * Callback function that must be implemented by application (NHLE) for MAC
 * service
 * MLME-POLL.confirm.
 *
 * @param status           Result of requested poll operation.
 *
 * @return void
 *
 */
void usr_mlme_poll_conf(uint8_t status)
{
}

#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

/*
 * @brief Callback function usr_mlme_reset_conf
 *
 * @param status Result of the reset procedure
 */
void usr_mlme_reset_conf(uint8_t status)
{
	if (status == MAC_SUCCESS) {
#ifdef MAC_SECURITY_ZIP
		/*	wpan_mlme_get_req(phyCurrentPage,NO_PIB_INDEX);*/
		wpan_mlme_set_req(macDefaultKeySource,
				NO_PIB_INDEX,
				&default_key_source);
#else
		wpan_mlme_get_req(phyCurrentPage);
#endif
	} else {
		/* Set proper state of application. */
		app_state = APP_IDLE;

		/* Something went wrong; restart. */
		wpan_mlme_reset_req(true);
	}
}

#if (MAC_RX_ENABLE_SUPPORT == 1)

/*
 * Callback function usr_mlme_rx_enable_conf
 *
 * @param status           Result of requested receiver enable operation.
 *
 * @return void
 */
void usr_mlme_rx_enable_conf(uint8_t status)
{
}

#endif  /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1)      ||	\
	(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)  || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))

/*
 * @brief Callback function usr_mlme_scan_conf
 *
 * @param status            Result of requested scan operation
 * @param ScanType          Type of scan performed
 * @param ChannelPage       Channel page on which the scan was performed
 * @param UnscannedChannels Bitmap of unscanned channels
 * @param ResultListSize    Number of elements in ResultList
 * @param ResultList        Pointer to array of scan results
 */
void usr_mlme_scan_conf(uint8_t status,
		uint8_t ScanType,
		uint8_t ChannelPage,
		uint32_t UnscannedChannels,
		uint8_t ResultListSize,
		void *ResultList)
{
	if (status == MAC_SUCCESS) {
		wpan_pandescriptor_t *coordinator;
		uint8_t i;

		/*
		 * Analyze the ResultList.
		 * Assume that the first entry of the result list is our
		 * coodinator.
		 */
		coordinator = (wpan_pandescriptor_t *)ResultList;

		for (i = 0; i < ResultListSize; i++) {
			/*
			 * Check if the PAN descriptor belongs to our
			 * coordinator.
			 * Check if coordinator allows association.
			 */
			if ((coordinator->LogicalChannel == current_channel) &&
					(coordinator->ChannelPage ==
					current_channel_page) &&
					(coordinator->CoordAddrSpec.PANId ==
					DEFAULT_PAN_ID) &&
					((coordinator->SuperframeSpec &
					((uint16_t)1 <<
					ASSOC_PERMIT_BIT_POS)) ==
					((uint16_t)1 << ASSOC_PERMIT_BIT_POS))
					) {
				/* Store the coordinator's address information.
				**/
				coord_addr_spec.AddrMode = WPAN_ADDRMODE_SHORT;
				coord_addr_spec.PANId = DEFAULT_PAN_ID;
				ADDR_COPY_DST_SRC_16(
						coord_addr_spec.Addr.short_address,
						coordinator->CoordAddrSpec.Addr.short_address);
#ifdef SIO_HUB
				printf("Found network\r\n");
#endif

				/* Set proper state of application. */
				app_state = APP_SCAN_DONE;

				/*
				 * Set the PAN-Id of the scanned network.
				 * This is required in order to perform a proper
				 * sync
				 * before assocation.
				 * Use: bool wpan_mlme_set_req(uint8_t
				 * PIBAttribute,
				 *                             void
				 ******PIBAttributeValue);
				 *
				 * This request leads to a set confirm message
				 *-> usr_mlme_set_conf
				 */
				uint16_t pan_id;
				pan_id = DEFAULT_PAN_ID;
				wpan_mlme_set_req(macPANId,
#ifdef MAC_SECURITY_ZIP
						NO_PIB_INDEX,
#endif
						&pan_id);

				return;
			}

			/* Get the next PAN descriptor. */
			coordinator++;
		}

		/*
		 * If here, the result list does not contain our expected
		 * coordinator.
		 * Let's scan again.
		 */
		wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
				SCAN_CHANNEL(current_channel),
				SCAN_DURATION_SHORT,
				current_channel_page);
	} else if (status == MAC_NO_BEACON) {
		/*
		 * No beacon is received; no coordiantor is located.
		 * Scan again, but used longer scan duration.
		 */
		wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
				SCAN_CHANNEL(current_channel),
				SCAN_DURATION_LONG,
				current_channel_page);
	} else {
		/* Set proper state of application. */
		app_state = APP_IDLE;

		/* Something went wrong; restart. */
		wpan_mlme_reset_req(true);
	}

	/* Keep compiler happy. */
	ScanType = ScanType;
	ChannelPage = ChannelPage;
	UnscannedChannels = UnscannedChannels;
}

#endif

/*
 * @brief Callback function usr_mlme_set_conf
 *
 * @param status        Result of requested PIB attribute set operation
 * @param PIBAttribute  Updated PIB attribute
 */
#ifndef MAC_SECURITY_ZIP

/*
 * @brief Callback function usr_mlme_set_conf
 *
 * @param status        Result of requested PIB attribute set operation
 * @param PIBAttribute  Updated PIB attribute
 */
void usr_mlme_set_conf(uint8_t status,
		uint8_t PIBAttribute)
{
	if ((status == MAC_SUCCESS) && (PIBAttribute == macPANId)) {
		/*
		 * Set the Coordinator Short Address of the scanned network.
		 * This is required in order to perform a proper sync
		 * before assocation.
		 */
		wpan_mlme_set_req(macCoordShortAddress, &coord_addr_spec.Addr);
	} else if ((status == MAC_SUCCESS) &&
			(PIBAttribute == macCoordShortAddress)) {
		/*
		 * Sync with beacon frames from our coordinator.
		 * Use: bool wpan_mlme_sync_req(uint8_t LogicalChannel,
		 *                              uint8_t ChannelPage,
		 *                              bool TrackBeacon);
		 *
		 * This does not lead to an immediate reaction.
		 *
		 * In case we receive beacon frames from our coordinator
		 * including
		 * a beacon payload, this is indicated in the callback function
		 * usr_mlme_beacon_notify_ind().
		 *
		 * In case the device cannot find its coordinator or later
		 * looses
		 * synchronization with its parent, this is indicated in the
		 * callback function usr_mlme_sync_loss_ind().
		 */
		wpan_mlme_sync_req(current_channel,
				current_channel_page,
				1);
	} else {
		/* Set proper state of application. */
		app_state = APP_IDLE;

		/* Something went wrong; restart. */
		wpan_mlme_reset_req(true);
	}
}

#endif

#if (MAC_START_REQUEST_CONFIRM == 1)
void usr_mlme_start_conf(uint8_t status)
{
}

#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

/*
 * Callback function usr_mlme_sync_loss_ind
 *
 * @param LossReason     Reason for synchronization loss.
 * @param PANId          The PAN identifier with which the device lost
 *                       synchronization or to which it was realigned.
 * @param LogicalChannel The logical channel on which the device lost
 *                       synchronization or to which it was realigned.
 * @param ChannelPage    The channel page on which the device lost
 *                       synchronization or to which it was realigned.
 */
void usr_mlme_sync_loss_ind(uint8_t LossReason,
		uint16_t PANId,
		uint8_t LogicalChannel,
		uint8_t ChannelPage)
{
	/*
	 * Once we lost sync this the coordinator we need to re-sync.
	 * Since we the network parameter are not supposed ot change,
	 * use the already known parameters form our coordinator.
	 */
	wpan_mlme_sync_req(LogicalChannel,
			ChannelPage,
			1);

	/* Keep compiler happy. */
	LossReason = LossReason;
	PANId = PANId;
}

/* Alert to indicate something has gone wrong in the application */
static void app_alert(void)
{
	while (1) {
		#if LED_COUNT > 0
		LED_Toggle(LED0);
		#endif

		#if LED_COUNT > 1
		LED_Toggle(LED1);
		#endif

		#if LED_COUNT > 2
		LED_Toggle(LED2);
		#endif

		#if LED_COUNT > 3
		LED_Toggle(LED3);
		#endif

		#if LED_COUNT > 4
		LED_Toggle(LED4);
		#endif

		#if LED_COUNT > 5
		LED_Toggle(LED5);
		#endif

		#if LED_COUNT > 6
		LED_Toggle(LED6);
		#endif

		#if LED_COUNT > 7
		LED_Toggle(LED7);
		#endif
		delay_us(0xFFFF);
	}
}

/*
 * @brief Callback function indicating network search
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void network_search_indication_cb(void *parameter)
{
	LED_Toggle(LED_NWK_SETUP);

	/* Re-start timer again. */

	/*sw_timer_start(APP_TIMER,
	 *              500000,
	 *              SW_TIMEOUT_RELATIVE,
	 *              (FUNC_PTR)network_search_indication_cb,
	 *              NULL);*/

	parameter = parameter; /* Keep compiler happy. */
}

/*
 * @brief Callback function for switching off the LED after data reception
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void rx_data_led_off_cb(void *parameter)
{
	LED_Off(LED_DATA);

	parameter = parameter; /* Keep compiler happy. */
}

#if (defined ENABLE_SLEEP || defined RTC_SLEEP)
static void enter_sleep(uint32_t timeout)
{
	/*Entering Power save after configuring Timer running at 4MHz as Wakeup
	 * source*/
	sys_sleep = true;
	#ifdef ENABLE_SLEEP
	ENTER_CRITICAL_REGION();

	/* Stop the Software timer running at CPU clock and initialize the
	 * software timer to run at 4MHz clock*/
	common_tc_stop();
	common_tc_init();
	set_common_tc_overflow_callback(hw_overflow_cb);
	set_common_tc_expiry_callback(hw_expiry_cb);
	timeout = timeout - MCU_WAKEUP_TIME;
	/*Start Wakeup timer*/
	sw_timer_start(APP_TIMER_SLEEP, timeout,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)wakeup_cb,
			NULL);
	LEAVE_CRITICAL_REGION();
	/* put the MCU in idle mode with timer as wakeup source*/
	system_set_sleepmode(SYSTEM_SLEEPMODE_IDLE_2);
	system_sleep();
    #endif
    #ifdef RTC_SLEEP
	/*Configure the rtc module and callback*/
	configure_rtc_count();
	/* Configure and enable callback */
	configure_rtc_callbacks();
	/* Set timeout period for rtc*/
	res = timeout % 1000;
	timeout = timeout / 1000;
	rtc_count_set_period(&rtc_instance, timeout);
	rtc_count_enable(&rtc_instance);
	system_set_sleepmode(SYSTEM_SLEEPMODE_STANDBY);
	system_sleep();
	#endif
}

#ifdef RTC_SLEEP
void configure_rtc_count(void) /* Configuring rtc clock prescaler and mode*/
{
	struct rtc_count_config config_rtc_count;
	rtc_count_get_config_defaults(&config_rtc_count);
	config_rtc_count.prescaler           = RTC_COUNT_PRESCALER_DIV_1;
	config_rtc_count.mode                = RTC_COUNT_MODE_16BIT;

	/* Continuously update the counter value so no synchronization is
	 * needed for reading. */
	config_rtc_count.continuously_update = true;
	rtc_count_init(&rtc_instance, RTC, &config_rtc_count);
}

void configure_rtc_callbacks(void)
{
	/*Register rtc callback*/
	rtc_count_register_callback(&rtc_instance, rtc_overflow_callback,
			RTC_COUNT_CALLBACK_OVERFLOW);
	rtc_count_enable_callback(&rtc_instance, RTC_COUNT_CALLBACK_OVERFLOW);
}

void rtc_overflow_callback(void)
{
	/* Do something on RTC overflow here */
	rtc_count_disable(&rtc_instance);
	/* Wakeup callback to switch the timer to default cpu clock*/
	wakeup_cb(NULL);
}

#endif
void wakeup_cb(void *parameter)
{
	sys_sleep = false;
    #ifdef ENABLE_SLEEP
	ENTER_CRITICAL_REGION();

	/* Stop the Software timer running at 4MHz clock and initialize the
	 * software timer to default cpu clock*/
	common_tc_stop();
	common_tc_init();
	set_common_tc_overflow_callback(hw_overflow_cb);
	set_common_tc_expiry_callback(hw_expiry_cb);
	sw_timer_stop(APP_TIMER_SLEEP);
	LEAVE_CRITICAL_REGION();
	#endif

	/* Callback Function in MAC for beacon Synchronization and Handling the
	 * residual time after Wakeup*/
	mac_wakeup(res);
}

#endif

/*
 * @brief This function will show up the application
 *        and stack build features and available in the firmware
 * @param void
 *
 */
static void print_stack_app_build_features(void)
{
#ifdef STB_ON_SAL
	printf("\r\n Security Tool Box On SAL : Enabled");
#else
	printf("\r\n Security Tool Box On SAL : Disabled");
#endif

#ifdef MAC_SECURITY_ZIP
	printf("\r\n MAC Data & Security Module : Enabled");
#else
	printf("\r\n MAC Data & Security Module : Disabled");
#endif

#ifdef MAC_SECURITY_BEACON
	printf("\r\n MAC Beacon Security : Enabled");
#else
	printf("\r\n MAC Beacon Security : Disabled");
#endif

#ifdef HIGH_DATA_RATE_SUPPORT
	printf("\r\n High Data Rate Support : Enabled");
#else
	printf("\r\n High Data Rate Support : Disabled");
#endif

#ifdef GTS_SUPPORT
	printf("\r\n MAC GTS Support : Enabled\r\n\r\n");
#else
	printf("\r\n MAC GTS Support : Disabled\r\n\r\n");
#endif
}
