/**
 * \file
 *
 * \brief AT86RFx Wireless Module Unit Test.
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
 *
 */

/*! \mainpage
 * \section intro Introduction
 * This is the unit test application for the \ref at86rfx.h "AT86RFx component".
 * It consists of test cases for the following:
 * - Module initialization
 * - Read/write module register
 * - Frame transmission
 *
 * \section files Main Files
 * - \ref unit_tests.c: AT86RFx Wireless unit tests application.
 * - \ref at86rfx_driver.c: The AT86RFx module driver
 * - \ref at86rfx_driver.h: The AT86RFx module driver header
 * - \ref conf_test.h: test configuration file
 * - \ref conf_board.h: board initialization process configuration
 * - \ref conf_clock.h: clock specific initialization
 * - \ref conf_sleepmgr.h: sleep mode specific configuration
 * - \ref conf_spi_master.h: SPI master mode specific configuration
 * - \ref conf_at86rfx.h: AT86RFx module driver configuration (including SPI
 * service selection)
 * - \ref conf_usb.h: USB specific configuration
 *
 * \section deviceinfo Device Info
 * All AVR devices can be used. This application has been tested
 * with the following setup:
 *   - RZ600 Evaluation kits with a AT86RFx connected to an SPI interface.
 *
 * \section compinfo Compilation Info
 * This software was written for the GNU GCC and IAR for AVR.
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com/">Atmel</A>.\n
 */

#include <stdint.h>
#include <stdbool.h>
#include <asf.h>
#include <string.h>
#include "board.h"
#include "sysclk.h"
#include "conf_usb.h"
#include "stdio_usb.h"
#include "conf_test.h"
#include "unit_tests.h"
#include "avr2025_mac.h"
#include "common_sw_timer.h"

#if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B))
#ifdef CHINESE_BAND
#define DEFAULT_CHANNEL                 (0)
#define DEFAULT_CHANNEL_PAGE            (5)
#else
#define DEFAULT_CHANNEL                 (1)
#define DEFAULT_CHANNEL_PAGE            (0)
#endif  /* #ifdef CHINESE_BAND */
#else
#define DEFAULT_CHANNEL                 (20)
#define DEFAULT_CHANNEL_PAGE            (0)
#endif  /* #if ((TAL_TYPE == AT86RF212) || (TAL_TYPE == AT86RF212B)) */

#define SCAN_CHANNEL                    (1ul << DEFAULT_CHANNEL)

/** Defines the scan duration time. */
#define SCAN_DURATION                   (5)

static uint8_t wpan_reset_conf_rcvd = false;
static uint8_t wpan_reset_conf_status = FAILURE;
static uint8_t wpan_scan_conf_rcvd = false;
static uint8_t wpan_scan_conf_status = FAILURE;

/**
 * \brief Run Wireless Module unit tests
 *
 * Initializes the clock system, board and USB.
 * Then runs the wireless task continuously.
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
	wpan_init();
	/* Enable interrupts */
	cpu_irq_enable();

	stdio_usb_init();

	while (1) {
		wpan_task();
	}
}

/**
 * \brief Performs a Reset check on AT86RFx module
 *
 * This function will simply test the output of the function
 * \ref wpan_mlme_reset_req and returns an error in case of failure.
 *
 * \param test Current test case.
 */
static void run_wpan_reset_test(const struct test_case *test)
{
	wpan_mlme_reset_req(true);
	while (!wpan_reset_conf_rcvd) {
		wpan_task();
	}
	test_assert_true(test, wpan_reset_conf_status == MAC_SUCCESS,
			"AVR2025_MAC - MAC Reset request failed");
}

static void run_wpan_scan_test(const struct test_case *test)
{
	wpan_mlme_scan_req(MLME_SCAN_TYPE_ACTIVE,
			SCAN_CHANNEL,
			SCAN_DURATION,
			DEFAULT_CHANNEL_PAGE);
	while (!wpan_scan_conf_rcvd) {
		wpan_task();
	}
	test_assert_true(test,
			(wpan_scan_conf_status == MAC_SUCCESS) ||
			(wpan_scan_conf_status == MAC_NO_BEACON),
			"AVR2025_MAC - MAC Scan test failed");
}

void usr_mlme_reset_conf(uint8_t status)
{
	wpan_reset_conf_rcvd = true;
	wpan_reset_conf_status = status;
}

void usr_mlme_scan_conf(uint8_t status,
		uint8_t ScanType,
		uint8_t ChannelPage,
		uint32_t UnscannedChannels,
		uint8_t ResultListSize,
		void *ResultList)
{
	wpan_scan_conf_rcvd = true;
	wpan_scan_conf_status = status;
}

void main_cdc_set_dtr(bool b_enable)
{
	if (b_enable) {
		DEFINE_TEST_CASE(wpan_reset_test, NULL, run_wpan_reset_test,
				NULL, "AVR2025_MAC - MAC Reset request");
		DEFINE_TEST_CASE(wpan_scan_test, NULL,
				run_wpan_scan_test, NULL,
				"AVR2025_MAC - MAC Scan test (this covers all ASF drivers/services used)");

		/* Put test case addresses in an array. */
		DEFINE_TEST_ARRAY(wpan_tests) = {
			&wpan_reset_test,
			&wpan_scan_test
		};

		/* Define the test suite. */
		DEFINE_TEST_SUITE(wpan_suite, wpan_tests,
				"AVR2025_MAC - MAC unit test suite");

		/* Run all tests in the test suite. */
		test_suite_run(&wpan_suite);
	} else {
	}
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
	/* Keep compiler happy. */
	AssocShortAddress = AssocShortAddress;
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

void usr_mlme_set_conf(uint8_t status,
		uint8_t PIBAttribute)
{
}

#if (MAC_START_REQUEST_CONFIRM == 1)
void usr_mlme_start_conf(uint8_t status)
{
}

#endif

void usr_mlme_sync_loss_ind(uint8_t LossReason,
		uint16_t PANId,
		uint8_t LogicalChannel,
		uint8_t ChannelPage)
{
}
