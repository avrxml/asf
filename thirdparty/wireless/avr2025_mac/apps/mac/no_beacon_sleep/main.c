/**
 * \file main.c
 *
 * \brief Mac Application - No beacon Sleep
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
 * This is the reference manual for the IEEE 802.15.4 MAC No beacon Sleep
 * Application
 * \section main_files Application Files
 *      - main.c                 Application main file.
 * \section intro Application Introduction
 * The basic MAC Example Nobeacon Sleep Application deploys a nonbeacon-enabled
 * network.
 * The first node searching for existing networks, and if no existing network is
 * found on the default channel,
 * this nodes becomes the PAN Coodinator and creates a nonbeacon-enable network.
 * This node has its receiver always
 * enabled.
 * Other nodes that find an already existing network, associate with the PAN
 * Coordiantor and goes
 * to sleep(both MCU and transceiver). For every 2 seconds the device wakes up
 * and sends a data to the coordinator before going to sleep again.
 * For demonstration purposes the coordinator's configuration is limited to
 * associate two devices at maximum.
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
#include "conf_board.h"
#include "avr2025_mac.h"
#include "led.h"
#include "delay.h"
#include "common_sw_timer.h"
#include "sleepmgr.h"
#include <asf.h>

/* === TYPES =============================================================== */

typedef struct associated_device_tag {
	uint16_t short_addr;
	uint64_t ieee_addr;
}

/** This type definition of a structure can store the short address and the
 *  extended address of a device.
 */
associated_device_t;

typedef enum node_status_tag {
	COLD_START_RESET,
	PC_RUNNING,
	IDLE,
	TRANSMITTING
} SHORTENUM node_status_t;

/* === MACROS ============================================================== */

/** Defines the PAN ID of the network. */
#ifdef PAN_ID
#define DEFAULT_PAN_ID                  (PAN_ID)
#else
#define DEFAULT_PAN_ID                  CCPU_ENDIAN_TO_LE16(0xBABE)
#endif
/** Defines the short address of the coordinator. */
#define COORD_SHORT_ADDR                (0x0000)
/** Defines the maximum number of devices the coordinator will handle. */
#define MAX_NUMBER_OF_DEVICES           (2)
/** This is the time period in micro seconds for data transmissions. */
#define DATA_TX_PERIOD                  (2000000)

#define CHANNEL_OFFSET                  (0)

#define SCAN_CHANNEL                    (1ul << current_channel)

/** Defines the scan duration time. */
#define SCAN_DURATION                   (4)
/** Defines the maximum number of scans before starting own network. */
#define MAX_NUMBER_OF_SCANS             (3)

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

#define INTER_FRAME_DURATION_US         (200000)

/* === GLOBALS ============================================================= */

/** This structure stores the short and extended address of the coordinator. */
static associated_device_t coord_addr;
/** Number of done network scans */
static uint8_t number_of_scans;
/** This array stores all device related information. */
static associated_device_t device_list[MAX_NUMBER_OF_DEVICES];
/** Current status of the node. */
static node_status_t node_status;

static uint8_t current_channel;
static uint8_t current_channel_page;
static uint32_t channels_supported;

volatile bool timer_flag = false;
volatile bool in_sleep_mode = false;

static uint8_t TIMER_LED_OFF;
static uint8_t WAKE_TIMER;

#ifdef SAM
enum sleepmgr_mode mode = SLEEPMGR_ACTIVE;
#else
enum sleepmgr_mode mode = SLEEPMGR_IDLE;
#endif
/* === PROTOTYPES ========================================================== */

static void app_task(void);
static void network_scan_indication_cb(void *parameter);
static void data_exchange_led_off_cb(void *parameter);
static void awake_timer_cb(void *parameter);
static bool assign_new_short_addr(uint64_t addr64, uint16_t *addr16);
static void app_alert(void);

/* === IMPLEMENTATION ====================================================== */

/*! \brief Main function.
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
		/*
		 * Stay here; we need a valid IEEE address.
		 * Check kit documentation how to create an IEEE address
		 * and to store it into the EEPROM.
		 */
		app_alert();
	}

	/* Initialize LEDs. */
	LED_On(LED_START);     /* indicating application is started */
	LED_Off(LED_NWK_SETUP); /* indicating node is associated */
	LED_Off(LED_DATA);     /* indicating successfull data transmission */

	cpu_irq_enable();

	sw_timer_get_id(&TIMER_LED_OFF);
	sw_timer_get_id(&WAKE_TIMER);

	node_status = COLD_START_RESET;

	/*
	 * Reset the MAC layer to the default values
	 * This request will cause a mlme reset confirm message ->
	 * usr_mlme_reset_conf
	 */
	wpan_mlme_reset_req(true);

	/* Main loop */
	while (true) {
		wpan_task();
		app_task(); /* Application task */
	}
}

#if defined(ENABLE_TSTAMP)
void usr_mcps_data_conf(uint8_t msduHandle,
		uint8_t status,
		uint32_t Timestamp)
#else
void usr_mcps_data_conf(uint8_t msduHandle,
		uint8_t status)
#endif  /* ENABLE_TSTAMP */
{
	node_status = IDLE;

	if (status == MAC_SUCCESS) {
		delay_ms(10);

		/*
		 * Dummy data has been transmitted successfully.
		 * Application code could be added here ...
		 */
		LED_Off(LED_DATA);
	}

	/* Keep compiler happy. */
	msduHandle = msduHandle;
}

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
	/*
	 * Dummy data has been received successfully.
	 * Application code could be added here ...
	 */
	LED_On(LED_DATA);
	/* Start a timer switching off the LED */
	sw_timer_start(TIMER_LED_OFF,
			500000,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)data_exchange_led_off_cb,
			NULL);

	/* Keep compiler happy. */
	SrcAddrSpec = SrcAddrSpec;
	DstAddrSpec = DstAddrSpec;
	msduLength = msduLength;
	msdu = msdu;
	mpduLinkQuality = mpduLinkQuality;
	DSN = DSN;
}

#if ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
void usr_mcps_purge_conf(uint8_t msduHandle,
		uint8_t status)
{
}

#endif  /* ((MAC_PURGE_REQUEST_CONFIRM == 1) && (MAC_INDIRECT_DATA_BASIC == 1))
        **/

#if (MAC_ASSOCIATION_REQUEST_CONFIRM == 1)
void usr_mlme_associate_conf(uint16_t AssocShortAddress,
		uint8_t status)
{
	if (status == MAC_SUCCESS) {
		/* Stop timer used for search indication (same as used for data
		 * transmission) */
		sw_timer_stop(TIMER_LED_OFF);
		LED_On(LED_NWK_SETUP);

		node_status = IDLE;
		sw_timer_start(TIMER_LED_OFF,
				DATA_TX_PERIOD,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)awake_timer_cb,
				NULL);
	} else {
		/* Something went wrong; restart */
		wpan_mlme_reset_req(true);
	}

	/* Keep compiler happy. */
	AssocShortAddress = AssocShortAddress;
}

#endif  /* (MAC_ASSOCIATION_REQUEST_CONFIRM == 1) */

#if (MAC_ASSOCIATION_INDICATION_RESPONSE == 1)
void usr_mlme_associate_ind(uint64_t DeviceAddress,
		uint8_t CapabilityInformation)
{
	/*
	 * Any device is allowed to join the network
	 * Use: bool wpan_mlme_associate_resp(uint64_t DeviceAddress,
	 *                                    uint16_t AssocShortAddress,
	 *                                    uint8_t status);
	 *
	 * This response leads to comm status indication ->
	 * usr_mlme_comm_status_ind
	 * Get the next available short address for this device
	 */
	uint16_t associate_short_addr = macShortAddress_def;

	if (assign_new_short_addr(DeviceAddress,
			&associate_short_addr) == true) {
		wpan_mlme_associate_resp(DeviceAddress,
				associate_short_addr,
				ASSOCIATION_SUCCESSFUL);
	} else {
		wpan_mlme_associate_resp(DeviceAddress,
				associate_short_addr,
				PAN_AT_CAPACITY);
	}

	/* Keep compiler happy. */
	CapabilityInformation = CapabilityInformation;
}

#endif  /* (MAC_ASSOCIATION_INDICATION_RESPONSE == 1) */

#if (MAC_BEACON_NOTIFY_INDICATION == 1)
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
void usr_mlme_disassociate_conf(uint8_t status,
		wpan_addr_spec_t *DeviceAddrSpec)
{
}

#endif /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)*/

#if (MAC_DISASSOCIATION_BASIC_SUPPORT == 1)
void usr_mlme_disassociate_ind(uint64_t DeviceAddress,
		uint8_t DisassociateReason)
{
}

#endif  /* (MAC_DISASSOCIATION_BASIC_SUPPORT == 1) */

#if (MAC_GET_SUPPORT == 1)
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
				SCAN_CHANNEL,
				SCAN_DURATION,
				current_channel_page);

		/* Indicate network scanning by a LED flashing */
		sw_timer_start(TIMER_LED_OFF,
				500000,
				SW_TIMEOUT_RELATIVE,
				(FUNC_PTR)network_scan_indication_cb,
				NULL);
	}
}

#endif  /* (MAC_GET_SUPPORT == 1) */

#if (MAC_ORPHAN_INDICATION_RESPONSE == 1)
void usr_mlme_orphan_ind(uint64_t OrphanAddress)
{
}

#endif  /* (MAC_ORPHAN_INDICATION_RESPONSE == 1) */

#if (MAC_INDIRECT_DATA_BASIC == 1)
void usr_mlme_poll_conf(uint8_t status)
{
}

#endif  /* (MAC_INDIRECT_DATA_BASIC == 1) */

void usr_mlme_reset_conf(uint8_t status)
{
	if (status == MAC_SUCCESS) {
		wpan_mlme_get_req(phyCurrentPage);
	} else {
		/* something went wrong; restart */
		wpan_mlme_reset_req(true);
	}
}

#if (MAC_RX_ENABLE_SUPPORT == 1)
void usr_mlme_rx_enable_conf(uint8_t status)
{
}

#endif  /* (MAC_RX_ENABLE_SUPPORT == 1) */

#if ((MAC_SCAN_ED_REQUEST_CONFIRM == 1)      ||	\
	(MAC_SCAN_ACTIVE_REQUEST_CONFIRM == 1)  || \
	(MAC_SCAN_PASSIVE_REQUEST_CONFIRM == 1) || \
	(MAC_SCAN_ORPHAN_REQUEST_CONFIRM == 1))
void usr_mlme_scan_conf(uint8_t status,
		uint8_t ScanType,
		uint8_t ChannelPage,
		uint32_t UnscannedChannels,
		uint8_t ResultListSize,
		void *ResultList)
{
	number_of_scans++;

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
				/* Store the coordinator's address */
				if (coordinator->CoordAddrSpec.AddrMode ==
						WPAN_ADDRMODE_SHORT) {
					ADDR_COPY_DST_SRC_16(
							coord_addr.short_addr,
							coordinator->CoordAddrSpec.Addr.short_address);
				} else if (coordinator->CoordAddrSpec.AddrMode
						== WPAN_ADDRMODE_LONG) {
					ADDR_COPY_DST_SRC_64(
							coord_addr.ieee_addr,
							coordinator->CoordAddrSpec.Addr.long_address);
				} else {
					/* Something went wrong; restart */
					wpan_mlme_reset_req(true);
					return;
				}

				/*
				 * Associate to our coordinator
				 * Use: bool wpan_mlme_associate_req(uint8_t
				 * LogicalChannel,
				 *                                   uint8_t
				 * ChannelPage,
				 *
				 *
				 *
				 *
				 *
				 *                              wpan_addr_spec_t
				 ******CoordAddrSpec,
				 *                                   uint8_t
				 * CapabilityInformation);
				 * This request will cause a mlme associate
				 * confirm message ->
				 * usr_mlme_associate_conf
				 */
				wpan_mlme_associate_req(
						coordinator->LogicalChannel,
						coordinator->ChannelPage,
						&(coordinator->CoordAddrSpec),
						WPAN_CAP_ALLOCADDRESS);
				return;
			}

			/* Get the next PAN descriptor */
			coordinator++;
		}

		/*
		 * If here, the result list does not contain our expected
		 * coordinator.
		 * Let's scan again.
		 */
		if (number_of_scans < MAX_NUMBER_OF_SCANS) {
			wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
					SCAN_CHANNEL,
					SCAN_DURATION,
					current_channel_page);
		} else {
			/* No network could be found; therefore start new
			 * network. */

			/*
			 * Set the short address of this node.
			 * Use: bool wpan_mlme_set_req(uint8_t PIBAttribute,
			 *                             void *PIBAttributeValue);
			 *
			 * This request leads to a set confirm message ->
			 * usr_mlme_set_conf
			 */
			uint8_t short_addr[2];

			short_addr[0] = (uint8_t)COORD_SHORT_ADDR; /* low byte
			                                           **/
			short_addr[1] = (uint8_t)(COORD_SHORT_ADDR >> 8); /*
			                                                   *
			                                                   *
			                                                   *
			                                                   *
			                                                   *high
			                                                   *
			                                                   *
			                                                   *
			                                                   *
			                                                   *byte
			                                                   **/
			wpan_mlme_set_req(macShortAddress, short_addr);
		}
	} else if (status == MAC_NO_BEACON) {
		/*
		 * No beacon is received; no coordiantor is located.
		 * Scan again, but used longer scan duration.
		 */
		if (number_of_scans < MAX_NUMBER_OF_SCANS) {
			wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
					SCAN_CHANNEL,
					SCAN_DURATION,
					current_channel_page);
		} else {
			/* No network could be found; therefore start new
			 * network. */

			/*
			 * Set the short address of this node.
			 * Use: bool wpan_mlme_set_req(uint8_t PIBAttribute,
			 *                             void *PIBAttributeValue);
			 *
			 * This request leads to a set confirm message ->
			 * usr_mlme_set_conf
			 */
			uint8_t short_addr[2];

			short_addr[0] = (uint8_t)COORD_SHORT_ADDR; /* low byte
			                                           **/
			short_addr[1] = (uint8_t)(COORD_SHORT_ADDR >> 8); /*
			                                                   *
			                                                   *
			                                                   *
			                                                   *
			                                                   *high
			                                                   *
			                                                   *
			                                                   *
			                                                   *
			                                                   *byte
			                                                   **/
			wpan_mlme_set_req(macShortAddress, short_addr);
		}
	} else {
		/* Something went wrong; restart */
		wpan_mlme_reset_req(true);
	}

	/* Keep compiler happy. */
	ScanType = ScanType;
	UnscannedChannels = UnscannedChannels;
	ChannelPage = ChannelPage;
}

#endif

void usr_mlme_set_conf(uint8_t status,
		uint8_t PIBAttribute)
{
	if ((status == MAC_SUCCESS) && (PIBAttribute == macShortAddress)) {
		/*
		 * Allow other devices to associate to this coordinator.
		 * Use: bool wpan_mlme_set_req(uint8_t PIBAttribute,
		 *                             void *PIBAttributeValue);
		 *
		 * This request leads to a set confirm message ->
		 * usr_mlme_set_conf
		 */
		uint8_t association_permit = true;

		wpan_mlme_set_req(macAssociationPermit, &association_permit);
	} else if ((status == MAC_SUCCESS) &&
			(PIBAttribute == macAssociationPermit)) {
		/*
		 * Set RX on when idle to enable the receiver as default.
		 * Use: bool wpan_mlme_set_req(uint8_t PIBAttribute,
		 *                             void *PIBAttributeValue);
		 *
		 * This request leads to a set confirm message ->
		 * usr_mlme_set_conf
		 */
		bool rx_on_when_idle = true;

		wpan_mlme_set_req(macRxOnWhenIdle, &rx_on_when_idle);
	} else if ((status == MAC_SUCCESS) &&
			(PIBAttribute == macRxOnWhenIdle)) {
		/*
		 * Start a nonbeacon-enabled network
		 * Use: bool wpan_mlme_start_req(uint16_t PANId,
		 *                               uint8_t LogicalChannel,
		 *                               uint8_t ChannelPage,
		 *                               uint8_t BeaconOrder,
		 *                               uint8_t SuperframeOrder,
		 *                               bool PANCoordinator,
		 *                               bool BatteryLifeExtension,
		 *                               bool CoordRealignment)
		 *
		 * This request leads to a start confirm message ->
		 * usr_mlme_start_conf
		 */
		wpan_mlme_start_req(DEFAULT_PAN_ID,
				current_channel,
				current_channel_page,
				15, 15,
				true, false, false);
	} else {
		/* something went wrong; restart */
		wpan_mlme_reset_req(true);
	}
}

#if (MAC_START_REQUEST_CONFIRM == 1)
void usr_mlme_start_conf(uint8_t status)
{
	if (status == MAC_SUCCESS) {
		node_status = PC_RUNNING;

		/*
		 * Network is established.
		 * Waiting for association indication from a device.
		 * -> usr_mlme_associate_ind
		 */
		/* Stop timer used for search indication */
		sw_timer_stop(TIMER_LED_OFF);
		LED_On(LED_NWK_SETUP);
	} else {
		/* something went wrong; restart */
		wpan_mlme_reset_req(true);
	}
}

#endif  /* (MAC_START_REQUEST_CONFIRM == 1) */

void usr_mlme_sync_loss_ind(uint8_t LossReason,
		uint16_t PANId,
		uint8_t LogicalChannel,
		uint8_t ChannelPage)
{
}

static bool assign_new_short_addr(uint64_t addr64, uint16_t *addr16)
{
	uint8_t i;

	/* Check if device has been associated before */
	for (i = 0; i < MAX_NUMBER_OF_DEVICES; i++) {
		if (device_list[i].short_addr == 0x0000) {
			/* If the short address is 0x0000, it has not been used
			 * before */
			continue;
		}

		if (device_list[i].ieee_addr == addr64) {
			/* Assign the previously assigned short address again */
			*addr16 = device_list[i].short_addr;
			return true;
		}
	}

	for (i = 0; i < MAX_NUMBER_OF_DEVICES; i++) {
		if (device_list[i].short_addr == 0x0000) {
			*addr16 = CPU_ENDIAN_TO_LE16(i + 0x0001);
			device_list[i].short_addr = CPU_ENDIAN_TO_LE16(
					i + 0x0001);                    /* get
			                                                 * next
			                                                 * short
			                                                 *
			                                                 *
			                                                 *
			                                                 *
			                                                 *address
			                                                 **/
			device_list[i].ieee_addr = addr64; /* store extended
			                                    * address */
			return true;
		}
	}

	/* If we are here, no short address could be assigned. */
	return false;
}

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

static void network_scan_indication_cb(void *parameter)
{
	LED_Toggle(LED_NWK_SETUP);

	/* Re-start led timer again */
	sw_timer_start(TIMER_LED_OFF,
			500000,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)network_scan_indication_cb,
			NULL);

	parameter = parameter; /* Keep compiler happy. */
}

static void data_exchange_led_off_cb(void *parameter)
{
	LED_Off(LED_DATA);

	parameter = parameter; /* Keep compiler happy. */
}

static void app_task(void)
{
	switch (node_status) {
	case IDLE:
	{
		if (timer_flag) {
			timer_flag = false;
			LED_On(LED_DATA);

			node_status = TRANSMITTING;

			/*
			 * Send some data and put MCU to sleep again.
			 * Use: bool wpan_mcps_data_req(uint8_t SrcAddrMode,
			 *                              wpan_addr_spec_t
			 ******DstAddrSpec,
			 *                              uint8_t msduLength,
			 *                              uint8_t *msdu,
			 *                              uint8_t msduHandle,
			 *                              uint8_t TxOptions);
			 *
			 * This request will cause a mcps data confirm message
			 *->
			 * usr_mcps_data_conf
			 */

			uint8_t src_addr_mode;
			wpan_addr_spec_t dst_addr;
			uint8_t payload;
			static uint8_t msduHandle = 0;

			src_addr_mode = WPAN_ADDRMODE_SHORT;

			dst_addr.AddrMode = WPAN_ADDRMODE_SHORT;
			dst_addr.PANId = DEFAULT_PAN_ID;
			ADDR_COPY_DST_SRC_16(dst_addr.Addr.short_address,
					coord_addr.short_addr);

			payload = (uint8_t)rand();          /* any dummy data */
			msduHandle++;                       /* increment handle
				                            **/
			wpan_mcps_data_req(src_addr_mode,
					&dst_addr,
					1,
					&payload,
					msduHandle,
					WPAN_TXOPT_ACK);
		} else {
			/*
			 * The MCU will wake up again, once the buttons is
			 * pressed
			 * on the board.Since no button pressed now set system
			 * to sleep.
			 * Set MCU to sleep if stack is not busy
			 */
			if (mac_ready_to_sleep() && in_sleep_mode == false) {
				in_sleep_mode = true;
				sleepmgr_init();
				sleepmgr_lock_mode(mode);
				sleepmgr_enter_sleep();
			}

			/* MCU is awake again */
		}
	}
	break;

	default:
		/* Do nothing. */
		break;
	}
}

static void awake_timer_cb(void *parameter)
{
	timer_flag = true;
	in_sleep_mode = false;

	sw_timer_start(TIMER_LED_OFF,
			DATA_TX_PERIOD,
			SW_TIMEOUT_RELATIVE,
			(FUNC_PTR)awake_timer_cb,
			NULL);
}
