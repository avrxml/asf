/**
 * @file main.c
 *
 * @brief MAC Example Nobeacon Application - Device
 *
 * Copyright (c) 2013 Atmel Corporation. All rights reserved.
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
 * This is the reference manual for the IEEE 802.15.4 MAC No-Beacon Application
 *- Device
 * \section main_files Application Files
 *      - main.c                 Application main file.
 * \section intro Application Introduction
 * The basic MAC Example Nobeacon Application deploys a nonbeacon-enabled
 *network.
 * The coordinator starts a PAN at channel DEFAULT_CHANNEL with the PAN ID
 *DEFAULT_PAN_ID.
 * The device scans for this network and associates to the coordinator.
 * Once the device is associated, it uses a timer that fires every 5 seconds to
 *poll for pending data at the coordinator by means of transmitting a data
 *request frame to the coordinator.
 * On the other hand every 5 seconds the coordinator queues a dummy data frame
 *for each associated device into its indirect frame queue.
 * If the coordinator receives a data request frame from a particular device, it
 *transmits the pending data frame to the device.
 * Device after receiving the data from the Coordinator sends back the same data
 *to the Coordinator itself by direct data transmission
 * The results of the proper data transmission/reception are printed to a
 *terminal program via Serial I/O (UART or USB).
 * \note For Two Processor Approach the Application needs to be flashed in the
 *Host board(eg.SAM4L-Xplained Pro) and the Serial-if application(NoBeacon FFD)
 *(MAC Stack)
 * needs to be flashed in the NCP(Network CoProcessor) board.
 *
 * For demonstration purposes the coordinator's configuration is limited to
 *associate two devices at maximum.
 * \section api_modules Application Dependent Modules
 * - \subpage api
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR .
 * Other compilers may or may not work.
 *
 * \section references References
 * 1)  IEEE Std 802.15.4-2006 Part 15.4: Wireless Medium Access Control (MAC)
 *     and Physical Layer (PHY) Specifications for Low-Rate Wireless Personal
 *Area
 *     Networks (WPANs).\n\n
 * 2)  AVR Wireless Support <A href="http://avr@atmel.com">avr@atmel.com</A>.\n
 *
 * \section contactinfo Contact Information
 * For further information,visit
 * <A href="http://www.atmel.com/avr">www.atmel.com</A>.\n
 */

/* === INCLUDES ============================================================ */
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include "conf_board.h"
#include "avr2025_mac.h"
#include "led.h"
#include "delay.h"
#include "common_sw_timer.h"
#include "sio2host.h"
#include <asf.h>

/* === TYPES =============================================================== */

/* === MACROS ============================================================== */

/** Defines the PAN ID of the network. */
#define DEFAULT_PAN_ID                  CCPU_ENDIAN_TO_LE16(0xBABE)

/** This is a device which will communicate with a single coordinator.
 *  Therefore, the maximum number of devices this code needs to
 *  handle is one.
 */
#define MAX_NUMBER_OF_DEVICES           (1)

/** This is the time period in micro seconds for polling transmissions. */
#define APP_POLL_PERIOD_MS              (5000)

/** Define the LED on duration time. */
#define LED_ON_DURATION                 (500000)

#define CHANNEL_OFFSET                  (0)

#define SCAN_CHANNEL                    (1ul << current_channel)

/** Defines the short scan duration time. */
#define SCAN_DURATION_SHORT             (5)
/** Defines the long scan duration time. */
#define SCAN_DURATION_LONG              (6)

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
#endif

/* === GLOBALS ============================================================= */

/** This structure stores the short address of the coordinator. */
static uint16_t coord_addr;

/*Stores total receive count*/
static uint32_t rx_cnt;

static uint8_t current_channel;
static uint8_t current_channel_page;
static uint32_t channels_supported;

static uint8_t APP_TIMER_LED_OFF;
static uint8_t APP_TIMER_POLL_DATA;

/* === PROTOTYPES ========================================================== */

/**
 * @brief Callback function for the application timer
 *        initiating polling of coordinator
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void app_initiate_polling(void *parameter);

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
 *
 */
static void rx_data_led_off_cb(void *parameter);

/**
 * @brief function for the sending received data back to coordinator
 *
 * @param data Pointer to received data
 *
 */
static void send_data(uint8_t *data);

/** Alert to indicate something has gone wrong in the application */
static void app_alert(void);

/* === IMPLEMENTATION ====================================================== */

/**
 * @brief Main function of the device application
 */
int main(void)
{
	irq_initialize_vectors();
	sysclk_init();

	/* Initialize the board.
	 * The board-specific conf_board.h file contains the configuration of
	 * the board initialization.
	 */
	board_init();

	sw_timer_init();

	if (MAC_SUCCESS != wpan_init()) {
		app_alert();
	}

	/* Initialize LEDs. */
	LED_On(LED_START);     /* indicating application is started */
	LED_Off(LED_NWK_SETUP); /* indicating node is associated */
	LED_Off(LED_DATA);     /* indicating successfull data transmission */

	cpu_irq_enable();

#ifdef SIO_HUB
	/* Initialize the serial interface used for communication with terminal
	 *program. */
	sio2host_init();

	/* To make sure the hyper terminal gets connected to the system */
	sio2host_getchar();

	printf("\nNobeacon_Application\r\n\n");
	printf("\nDevice\r\n\n");
#endif /* SIO_HUB */

	sw_timer_get_id(&APP_TIMER_LED_OFF);
	sw_timer_get_id(&APP_TIMER_POLL_DATA);

	wpan_mlme_reset_req(true);

	while (true) {
		wpan_task();
	}
}

#if defined(ENABLE_TSTAMP)

/*
 * Callback function usr_mcps_data_conf
 *
 * @param msduHandle  Handle of MSDU handed over to MAC earlier
 * @param status      Result for requested data transmission request
 * @param Timestamp   The time, in symbols, at which the data were transmitted
 *                    (only if timestamping is enabled).
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

/*
 * @brief Callback function usr_mcps_data_ind
 *
 * @param SrcAddrSpec      Pointer to source address specification
 * @param DstAddrSpec      Pointer to destination address specification
 * @param msduLength       Number of octets contained in MSDU
 * @param msdu             Pointer to MSDU
 * @param mpduLinkQuality  LQI measured during reception of the MPDU
 * @param DSN              DSN of the received data frame.
 * @param Timestamp        The time, in symbols, at which the data were received
 *                         (only if timestamping is enabled).
 */
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
{
#ifdef SIO_HUB
	char sio_array[255];
#endif

	/*increments the receive count*/
	rx_cnt++;

#ifdef SIO_HUB
	sprintf(sio_array, "Frame received: %" PRIu32 ",data: %" PRIu8 "\r\n",
			rx_cnt, *msdu);
	printf(sio_array);
#endif

	/*
	 * Dummy data has been received successfully.
	 */
	LED_On(LED_DATA);

	/*
	 * Device re-sending the data received from the coordinator
	 */
	send_data(msdu);

	printf("Frame Transmitted\r\n");
	/* Start a timer switching off the LED. */
	sw_timer_start(APP_TIMER_LED_OFF,
			LED_ON_DURATION,
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
 * Callback function that must be implemented by application (NHLE) for MAC
 *service
 * MCPS-PURGE.confirm.
 *
 * @param msduHandle           Handle (id) of MSDU to be purged.
 * @param status               Result of requested purge operation.
 *
 * @return void
 */
void usr_mcps_purge_conf(uint8_t msduHandle,
		uint8_t status)
{
}

#endif  /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
         **/

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)

/*
 * Callback function usr_mlme_associate_conf.
 *
 * @param AssocShortAddress    Short address allocated by the coordinator.
 * @param status               Result of requested association operation.
 *
 * @return void
 *
 */
void usr_mlme_associate_conf(uint16_t AssocShortAddress,
		uint8_t status)
{
	if (status == MAC_SUCCESS) {
#ifdef SIO_HUB
		printf("Connected to nonbeacon-enabled network\r\n");
#endif

		/* Stop timer used for search indication (same as used for data
		 *transmission). */
		sw_timer_stop(APP_TIMER_LED_OFF);
		LED_On(LED_NWK_SETUP);

		/* Start a timer that polls for pending data at the coordinator.
		 **/
		sw_timer_start(APP_TIMER_POLL_DATA,
				((uint32_t)APP_POLL_PERIOD_MS * 1000),
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)app_initiate_polling,
				NULL);
	} else {
		/* Something went wrong; restart. */
		wpan_mlme_reset_req(true);
	}

	/* Keep compiler happy. */
	AssocShortAddress = AssocShortAddress;
}

#endif  /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)

/*
 * @brief Callback function usr_mlme_associate_ind
 *
 * @param DeviceAddress         Extended address of device requesting
 *association
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
 *pending data.
 * @param sduLength      Length of beacon payload.
 * @param sdu            Pointer to beacon payload.
 *
 * @return void
 *
 */
void usr_mlme_beacon_notify_ind(uint8_t BSN,
		wpan_pandescriptor_t *PANDescriptor,
		uint8_t PendAddrSpec,
		uint8_t *AddrList,
		uint8_t sduLength,
		uint8_t *sdu)
{
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
		void *PIBAttributeValue)
{
	if ((status == MAC_SUCCESS) && (PIBAttribute == phyCurrentPage)) {
		current_channel_page = *(uint8_t *)PIBAttributeValue;
		wpan_mlme_get_req(phyChannelsSupported);
	} else if ((status == MAC_SUCCESS) &&
			(PIBAttribute == phyChannelsSupported)) {
		uint8_t index;

		channels_supported = *(uint32_t *)PIBAttributeValue;

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
		 *usr_mlme_scan_conf
		 * Scan for about 50 ms on each channel -> ScanDuration = 1
		 * Scan for about 1/2 second on each channel -> ScanDuration = 5
		 * Scan for about 1 second on each channel -> ScanDuration = 6
		 */
		wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
				SCAN_CHANNEL,
				SCAN_DURATION_SHORT,
				current_channel_page);

		/* Indicate network scanning by a LED flashing. */
		sw_timer_start(APP_TIMER_LED_OFF,
				LED_ON_DURATION,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)network_search_indication_cb,
				NULL);
	}
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
 *service
 * MLME-POLL.confirm.
 *
 * @param status           Result of requested poll operation.
 *
 * @return void
 *
 */
void usr_mlme_poll_conf(uint8_t status)
{
	/* Start a timer that polls for pending data at the coordinator. */
	sw_timer_start(APP_TIMER_POLL_DATA,
			((uint32_t)APP_POLL_PERIOD_MS * 1000),
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)app_initiate_polling,
			NULL);

	status = status; /* Keep compiler happy. */
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
		wpan_mlme_get_req(phyCurrentPage);
	} else {
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
		 *coodinator.
		 */
		coordinator = (wpan_pandescriptor_t *)ResultList;

		for (i = 0; i < ResultListSize; i++) {
			/*
			 * Check if the PAN descriptor belongs to our
			 *coordinator.
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
				if (coordinator->CoordAddrSpec.AddrMode ==
						WPAN_ADDRMODE_SHORT) {
					coord_addr
						= coordinator->CoordAddrSpec.
							Addr
							.short_address;
				} else {
					/* Something went wrong; restart. */
					wpan_mlme_reset_req(true);
					return;
				}

#ifdef SIO_HUB
				printf("Found network\r\n");
#endif

				/*
				 * Associate to our coordinator.
				 * Use: bool wpan_mlme_associate_req(uint8_t
				 *LogicalChannel,
				 *                                   uint8_t
				 *ChannelPage,
				 *
				 *
				 *
				 *
				 *
				 *                              wpan_addr_spec_t
				 **CoordAddrSpec,
				 *                                   uint8_t
				 *CapabilityInformation);
				 * This request will cause a mlme associate
				 *confirm message ->
				 * usr_mlme_associate_conf.
				 */
				wpan_mlme_associate_req(
						coordinator->LogicalChannel,
						coordinator->ChannelPage,
						&(coordinator->CoordAddrSpec),
						WPAN_CAP_ALLOCADDRESS);
				return;
			}

			/* Get the next PAN descriptor. */
			coordinator++;
		}

		/*
		 * If here, the result list does not contain our expected
		 *coordinator.
		 * Let's scan again.
		 */
		wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
				SCAN_CHANNEL,
				SCAN_DURATION_SHORT,
				current_channel_page);
	} else if (status == MAC_NO_BEACON) {
		/*
		 * No beacon is received; no coordiantor is located.
		 * Scan again, but used longer scan duration.
		 */
		wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
				SCAN_CHANNEL,
				SCAN_DURATION_LONG,
				current_channel_page);
	} else {
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
void usr_mlme_set_conf(uint8_t status,
		uint8_t PIBAttribute)
{
}

#if (MAC_START_REQUEST_CONFIRM == 1)

/*
 * @brief Callback function usr_mlme_start_conf
 *
 * @param status        Result of requested start operation
 */
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
 *
 * @return void
 *
 */
void usr_mlme_sync_loss_ind(uint8_t LossReason,
		uint16_t PANId,
		uint8_t LogicalChannel,
		uint8_t ChannelPage)
{
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
 * @brief Callback function for the application timer
 *        initiating polling of coordinator
 *
 * @param parameter Pointer to callback parameter
 *                  (not used in this application, but could be if desired).
 */
static void app_initiate_polling(void *parameter)
{
	/*
	 * Initiate MLME-POLL.request service and have it placed in the MLME-SAP
	 *queue.
	 *
	 * @param CoordAddrSpec   Pointer to wpan_addr_spec_t structure for the
	 *coordinator.
	 *
	 * @return true - success; false - buffer not availability or queue
	 *full.
	 */
	wpan_addr_spec_t coord_addr_spec;

	coord_addr_spec.AddrMode = WPAN_ADDRMODE_SHORT;
	coord_addr_spec.PANId = DEFAULT_PAN_ID;
	coord_addr_spec.Addr.short_address = coord_addr;
#ifdef SIO_HUB
	printf("Poll coordinator\r\n");
#endif
	wpan_mlme_poll_req(&coord_addr_spec);
	/*Keep compiler happy*/
	parameter = parameter;
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

	/* Re-start led timer again. */
	sw_timer_start(APP_TIMER_LED_OFF,
			LED_ON_DURATION,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)network_search_indication_cb,
			NULL);

	parameter = parameter; /* Keep compiler happy. */
}

/*
 * @brief Callback function for switching off the LED after data reception
 *
 * @param parameter Pointer to callback parameter
 *
 */
static void rx_data_led_off_cb(void *parameter)
{
	LED_Off(LED_DATA);

	parameter = parameter; /* Keep compiler happy. */
}

/*
 * @brief function for the sending received data back to coordinator
 *
 * @param data Pointer to received data
 *
 */
static void send_data(uint8_t *data)
{
	/*
	 * Send some data and restart timer.
	 * Use: bool wpan_mcps_data_req(uint8_t SrcAddrMode,
	 *                              wpan_addr_spec_t *DstAddrSpec,
	 *                              uint8_t msduLength,
	 *                              uint8_t *msdu,
	 *                              uint8_t msduHandle,
	 *                              uint8_t TxOptions);
	 *
	 * This request will cause a mcps data confirm message ->
	 * usr_mcps_data_conf
	 */

	uint8_t src_addr_mode;
	wpan_addr_spec_t dst_addr;
	static uint8_t msduHandle = 0;

	src_addr_mode = WPAN_ADDRMODE_SHORT;

	dst_addr.AddrMode = WPAN_ADDRMODE_SHORT;
	dst_addr.PANId = DEFAULT_PAN_ID;
	ADDR_COPY_DST_SRC_16(dst_addr.Addr.short_address, coord_addr);

	msduHandle++;
	wpan_mcps_data_req(src_addr_mode,
			&dst_addr,
			1,
			data,
			msduHandle,
			WPAN_TXOPT_ACK);
}
