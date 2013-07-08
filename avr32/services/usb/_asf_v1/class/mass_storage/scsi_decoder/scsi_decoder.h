/*****************************************************************************
 *
 * \file
 *
 * \brief Management of the SCSI decoding.
 *
 * This file manages the SCSI decoding.
 *
 * Copyright (c) 2009 Atmel Corporation. All rights reserved.
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
 ******************************************************************************/


#ifndef _SCSI_DECODER_H_
#define _SCSI_DECODER_H_

//_____ I N C L U D E S ____________________________________________________

#include "conf_usb.h"
#include "compiler.h"
#include "preprocessor.h"


//_____ D E F I N I T I O N S ______________________________________________

/*! \name bmCBWFlags
 */
//! @{
#define SBC_CMD_DIR_IN                                  0x80
#define SBC_CMD_DIR_OUT                                 0x00
//! @}

/*! \name SCSI Commands
 */
//! @{
#define SBC_CMD_TEST_UNIT_READY                         0x00
#define SBC_CMD_REQUEST_SENSE                           0x03
#define SBC_CMD_FORMAT_UNIT                             0x04
#define SBC_CMD_REASSIGN_BLOCKS                         0x07
#define SBC_CMD_READ_6                                  0x08
#define SBC_CMD_WRITE_6                                 0x0A
#define SBC_CMD_INQUIRY                                 0x12
#define SBC_CMD_MODE_SELECT_6                           0x15
#define SBC_CMD_MODE_SENSE_6                            0x1A
#define SBC_CMD_START_STOP_UNIT                         0x1B
#define SBC_CMD_RECEIVE_DIAGNOSTIC_RESULTS              0x1C
#define SBC_CMD_SEND_DIAGNOSTIC                         0x1D
#define SBC_CMD_PREVENT_ALLOW_MEDIUM_REMOVAL            0x1E
#define SBC_CMD_READ_CAPACITY_10                        0x25
#define SBC_CMD_READ_10                                 0x28
#define SBC_CMD_WRITE_10                                0x2A
#define SBC_CMD_WRITE_AND_VERIFY_10                     0x2E
#define SBC_CMD_VERIFY_10                               0x2F
#define SBC_CMD_PREFETCH_10                             0x34
#define SBC_CMD_SYNCHRONIZE_CACHE_10                    0x35
#define SBC_CMD_READ_DEFECT_DATA_10                     0x37
#define SBC_CMD_WRITE_BUFFER                            0x3B
#define SBC_CMD_READ_BUFFER                             0x3C
#define SBC_CMD_READ_LONG_10                            0x3E
#define SBC_CMD_WRITE_LONG_10                           0x3F
#define SBC_CMD_WRITE_SAME_10                           0x41
#define SBC_CMD_LOG_SELECT                              0x4C
#define SBC_CMD_LOG_SENSE                               0x4D
#define SBC_CMD_XDWRITE_10                              0x50
#define SBC_CMD_XPWRITE_10                              0x51
#define SBC_CMD_XDREAD_10                               0x52
#define SBC_CMD_XDWRITEREAD_10                          0x53
#define SBC_CMD_MODE_SELECT_10                          0x55
#define SBC_CMD_MODE_SENSE_10                           0x5A
#define SBC_CMD_PERSISTENT_RESERVE_IN                   0x5E
#define SBC_CMD_PERSISTENT_RESERVE_OUT                  0x5F
#define SBC_CMD_EXTENDED_COPY                           0x83
#define SBC_CMD_RECEIVE_COPY_RESULTS                    0x84
#define SBC_CMD_ACCESS_CONTROL_IN                       0x86
#define SBC_CMD_ACCESS_CONTROL_OUT                      0x87
#define SBC_CMD_READ_16                                 0x88
#define SBC_CMD_WRITE_16                                0x8A
#define SBC_CMD_READ_ATTRIBUTE                          0x8C
#define SBC_CMD_WRITE_ATTRIBUTE                         0x8D
#define SBC_CMD_WRITE_AND_VERIFY_16                     0x8E
#define SBC_CMD_VERIFY_16                               0x8F
#define SBC_CMD_PREFETCH_16                             0x90
#define SBC_CMD_SYNCHRONIZE_CACHE_16                    0x91
#define SBC_CMD_WRITE_SAME_16                           0x93
#define SBC_CMD_REPORT_LUNS                             0xA0
#define SBC_CMD_READ_12                                 0xA8
#define SBC_CMD_WRITE_12                                0xAA
#define SBC_CMD_WRITE_AND_VERIFY_12                     0xAE
#define SBC_CMD_VERIFY_12                               0xAF
#define SBC_CMD_READ_DEFECT_DATA_12                     0xB7
//! @}

/*! \name Status Codes
 */
//! @{
#define COMMAND_PASSED                                  0x00
#define COMMAND_FAILED                                  0x01
#define CHECK_CONDITION                                 0x02
//! @}

/*! \name Sense Key Definitions
 */
//! @{
#define SBC_SENSE_KEY_NO_SENSE                          0x00
#define SBC_SENSE_KEY_RECOVERED_ERROR                   0x01
#define SBC_SENSE_KEY_NOT_READY                         0x02
#define SBC_SENSE_KEY_MEDIUM_ERROR                      0x03
#define SBC_SENSE_KEY_HARDWARE_ERROR                    0x04
#define SBC_SENSE_KEY_ILLEGAL_REQUEST                   0x05
#define SBC_SENSE_KEY_UNIT_ATTENTION                    0x06
#define SBC_SENSE_KEY_DATA_PROTECT                      0x07
#define SBC_SENSE_KEY_BLANK_CHECK                       0x08
#define SBC_SENSE_KEY_VENDOR_SPECIFIC                   0x09
#define SBC_SENSE_KEY_COPY_ABORTED                      0x0A
#define SBC_SENSE_KEY_ABORTED_COMMAND                   0x0B
#define SBC_SENSE_KEY_VOLUME_OVERFLOW                   0x0D
#define SBC_SENSE_KEY_MISCOMPARE                        0x0E
//! @}

/*! \name Additional Sense Code Definitions
 */
//! @{
#define SBC_ASC_NO_ADDITIONAL_SENSE_INFORMATION         0x00
#define SBC_ASC_LOGICAL_UNIT_NOT_READY                  0x04
#define SBC_ASC_INVALID_FIELD_IN_CDB                    0x24
#define SBC_ASC_WRITE_PROTECTED                         0x27
#define SBC_ASC_FORMAT_ERROR                            0x31
#define SBC_ASC_INVALID_COMMAND_OPERATION_CODE          0x20
#define SBC_ASC_NOT_READY_TO_READY_CHANGE               0x28
#define SBC_ASC_MEDIUM_NOT_PRESENT                      0x3A
//! @}

/*! \name Additional Sense Code Qualifier Definitions
 */
//! @{
#define SBC_ASCQ_FORMAT_COMMAND_FAILED                  0x01
#define SBC_ASCQ_INITIALIZING_COMMAND_REQUIRED          0x02
#define SBC_ASCQ_OPERATION_IN_PROGRESS                  0x07
//! @}

/*! \name Fixed Format Sense Data
 */
//! @{
#define SBC_RESPONSE_CODE_SENSE                         0x70
#define SBC_ADDITIONAL_SENSE_LENGTH                     0x0A
#define SBC_COMMAND_SPECIFIC_INFORMATION_3              0x00
#define SBC_COMMAND_SPECIFIC_INFORMATION_2              0x00
#define SBC_COMMAND_SPECIFIC_INFORMATION_1              0x00
#define SBC_COMMAND_SPECIFIC_INFORMATION_0              0x00
#define SBC_FIELD_REPLACEABLE_UNIT_CODE                 0x00
#define SBC_SENSE_KEY_SPECIFIC_2                        0x00
#define SBC_SENSE_KEY_SPECIFIC_1                        0x00
#define SBC_SENSE_KEY_SPECIFIC_0                        0x00
//! @}

/*! \name MODE SENSE Bit-Masks
 */
//! @{
#define SBC_MSK_DBD                                     0x08
#define SBC_MSK_PAGE_CONTROL                            0xC0
#define SBC_MSK_PAGE_CODE                               0x3F
//! @}

/*! \name Mode Page Codes for Direct-Access Block Devices
 */
//! @{
#define SBC_PAGE_CODE_READ_WRITE_ERROR_RECOVERY         0x01
#define SBC_PAGE_CODE_INFORMATIONAL_EXCEPTIONS          0x1C
#define SBC_PAGE_CODE_ALL                               0x3F
//! @}

/*! \name Mode Data Lengths for Direct-Access Block Devices
 */
//! @{
#define SBC_MODE_DATA_LENGTH_READ_WRITE_ERROR_RECOVERY  (SBC_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY + 2 + 3)
#define SBC_MODE_DATA_LENGTH_INFORMATIONAL_EXCEPTIONS   (SBC_PAGE_LENGTH_INFORMATIONAL_EXCEPTIONS + 2 + 3)
#define SBC_MODE_DATA_LENGTH_CODE_ALL                   (SBC_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY + 2 +\
                                                         SBC_PAGE_LENGTH_INFORMATIONAL_EXCEPTIONS + 2 +\
                                                         SBC_BLOCK_DESCRIPTOR_LENGTH + 3)
//! @}

//! <tt>MEDIUM TYPE</tt> for direct-access block devices.
#define SBC_MEDIUM_TYPE                                 0x00

/*! \name <tt>DEVICE-SPECIFIC PARAMETER</tt> Write Protect (\c WP) Bit-Masks.
 */
//! @{
#define SBC_DEV_SPEC_PARAM_WR_ENABLE                    0x00
#define SBC_DEV_SPEC_PARAM_WR_PROTECT                   0x80
//! @}

//! MODE SENSE <tt>BLOCK DESCRIPTOR LENGTH</tt>.
#define SBC_BLOCK_DESCRIPTOR_LENGTH                     0x00

/*! \name Mode Page Lengths for Direct-Access Block Devices
 */
//! @{
#define SBC_PAGE_LENGTH_READ_WRITE_ERROR_RECOVERY       0x0A
#define SBC_PAGE_LENGTH_INFORMATIONAL_EXCEPTIONS        0x0A
//! @}

/*! \name Read-Write Error Recovery Mode Page Bit-Fields
 */
//! @{
#define SBC_READ_RETRY_COUNT                            0x03
#define SBC_WRITE_RETRY_COUNT                           0x80
#define SBC_RECOVERY_TIME_LIMIT                       0x0000
//! @}

//! Method of reporting informational exceptions (\c MRIE).
#define SBC_MRIE_GENERATE_NO_SENSE                      0x05

//! READ CAPACITY (10) parameter data length.
#define SBC_READ_CAPACITY_LENGTH                        0x08


/*! \name MCU <-> SCSI Endianism Conversion Macros
 */
//! @{

#if LITTLE_ENDIAN_MCU

  /*! \brief Converts 16-, 32- or 64-bit data from MCU to SCSI endianism.
   *
   * Depending on MCU endianism, swaps or not data bytes.
   *
   * \param width Data width in bits: 16, 32 or 64.
   * \param data  16-, 32- or 64-bit data to format.
   *
   * \return Formatted 16-, 32- or 64-bit data.
   *
   * \note More optimized if only used with values known at compile time.
   */
  #define Sbc_format_mcu_to_scsi_data(width, data)  (TPASTE2(Swap, width)(data))

  /*! \brief Converts 16-, 32- or 64-bit data from SCSI to MCU endianism.
   *
   * Depending on MCU endianism, swaps or not data bytes.
   *
   * \param width Data width in bits: 16, 32 or 64.
   * \param data  16-, 32- or 64-bit data to format.
   *
   * \return Formatted 16-, 32- or 64-bit data.
   *
   * \note More optimized if only used with values known at compile time.
   */
  #define Sbc_format_scsi_to_mcu_data(width, data)  (TPASTE2(Swap, width)(data))

  /*! \brief Converts 16-, 32- or 64-bit data from MCU to SCSI endianism.
   *
   * Depending on MCU endianism, swaps or not data bytes.
   *
   * \param width Data width in bits: 16, 32 or 64.
   * \param data  16-, 32- or 64-bit data to format.
   *
   * \return Formatted 16-, 32- or 64-bit data.
   *
   * \note More optimized if only used with values unknown at compile time.
   */
  #define sbc_format_mcu_to_scsi_data(width, data)  (TPASTE2(swap, width)(data))

  /*! \brief Converts 16-, 32- or 64-bit data from SCSI to MCU endianism.
   *
   * Depending on MCU endianism, swaps or not data bytes.
   *
   * \param width Data width in bits: 16, 32 or 64.
   * \param data  16-, 32- or 64-bit data to format.
   *
   * \return Formatted 16-, 32- or 64-bit data.
   *
   * \note More optimized if only used with values unknown at compile time.
   */
  #define sbc_format_scsi_to_mcu_data(width, data)  (TPASTE2(swap, width)(data))

#else // BIG_ENDIAN_MCU

  /*! \brief Converts 16-, 32- or 64-bit data from MCU to SCSI endianism.
   *
   * Depending on MCU endianism, swaps or not data bytes.
   *
   * \param width Data width in bits: 16, 32 or 64.
   * \param data  16-, 32- or 64-bit data to format.
   *
   * \return Formatted 16-, 32- or 64-bit data.
   *
   * \note More optimized if only used with values known at compile time.
   */
  #define Sbc_format_mcu_to_scsi_data(width, data)  ((TPASTE2(U, width))(data))

  /*! \brief Converts 16-, 32- or 64-bit data from SCSI to MCU endianism.
   *
   * Depending on MCU endianism, swaps or not data bytes.
   *
   * \param width Data width in bits: 16, 32 or 64.
   * \param data  16-, 32- or 64-bit data to format.
   *
   * \return Formatted 16-, 32- or 64-bit data.
   *
   * \note More optimized if only used with values known at compile time.
   */
  #define Sbc_format_scsi_to_mcu_data(width, data)  ((TPASTE2(U, width))(data))

  /*! \brief Converts 16-, 32- or 64-bit data from MCU to SCSI endianism.
   *
   * Depending on MCU endianism, swaps or not data bytes.
   *
   * \param width Data width in bits: 16, 32 or 64.
   * \param data  16-, 32- or 64-bit data to format.
   *
   * \return Formatted 16-, 32- or 64-bit data.
   *
   * \note More optimized if only used with values unknown at compile time.
   */
  #define sbc_format_mcu_to_scsi_data(width, data)  ((TPASTE2(U, width))(data))

  /*! \brief Converts 16-, 32- or 64-bit data from SCSI to MCU endianism.
   *
   * Depending on MCU endianism, swaps or not data bytes.
   *
   * \param width Data width in bits: 16, 32 or 64.
   * \param data  16-, 32- or 64-bit data to format.
   *
   * \return Formatted 16-, 32- or 64-bit data.
   *
   * \note More optimized if only used with values unknown at compile time.
   */
  #define sbc_format_scsi_to_mcu_data(width, data)  ((TPASTE2(U, width))(data))

#endif

//! @}

/*! \brief Sends the USB mass-storage endpoint FIFO and updates
 *         \ref g_scsi_data_remaining accordingly.
 *
 * \param length  Number of bytes sent.
 */
#define Sbc_valid_write_usb(length)         (Usb_ack_in_ready_send(g_scsi_ep_ms_in),\
                                             g_scsi_data_remaining -= (length))

/*! \name Status Macros
 */
//! @{

/*! \brief Sets status to FAILED.
 */
#define Sbc_send_failed()                   (g_scsi_status = COMMAND_FAILED )

/*! \brief Sets status to CHECK CONDITION.
 */
#define Sbc_send_check_condition()          (g_scsi_status = CHECK_CONDITION)

/*! \brief Sets status to GOOD.
 */
#define Sbc_send_good()                     (g_scsi_status = COMMAND_PASSED )

//! @}

/*! \brief Builds sense data.
 *
 * \param skey  <tt>SENSE KEY</tt>.
 * \param sasc  <tt>ADDITIONAL SENSE CODE</tt>.
 * \param sascq <tt>ADDITIONAL SENSE CODE QUALIFIER</tt>.
 */
#define Sbc_build_sense(skey, sasc, sascq)  (g_scsi_sense.key  = (skey),\
                                             g_scsi_sense.asc  = (sasc),\
                                             g_scsi_sense.ascq = (sascq))


//! Sense data.
typedef struct
{
  uint8_t key;   //!< <tt>SENSE KEY</tt>.
  uint8_t asc;   //!< <tt>ADDITIONAL SENSE CODE</tt>.
  uint8_t ascq;  //!< <tt>ADDITIONAL SENSE CODE QUALIFIER</tt>.
} s_scsi_sense;

//! INQUIRY data.
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
  unsigned int PeripheralQualifier : 3;
  unsigned int DeviceType          : 5;

  unsigned int RemovableMedia      : 1;
  unsigned int Reserved1           : 7;

  uint8_t           Version                ;

  unsigned int AERC                : 1;
  unsigned int Obsolete0           : 1;
  unsigned int NormACA             : 1;
  unsigned int Reserved3           : 5;

  uint8_t           Reserved4           [3];

  unsigned int RelativeAddressing  : 1;
  unsigned int Wide32Bit           : 1;
  unsigned int Wide16Bit           : 1;
  unsigned int Synchronous         : 1;
  unsigned int LinkedCommands      : 1;
  unsigned int Reserved5           : 1;
  unsigned int CommandQueue        : 1;
  unsigned int SoftReset           : 1;

  char g_sbc_vendor_id             [8];

  char g_sbc_product_id           [16];

  char g_sbc_revision_id           [4];
}
#if __ICCAVR32__
#pragma pack()
#endif
sbc_st_std_inquiry_data;


//_____ D E C L A R A T I O N S ____________________________________________

#if USB_DEVICE_FEATURE == true

//! SCSI command buffer.
extern uint8_t g_scsi_command[16];

//! SCSI status.
extern uint8_t g_scsi_status;

//! Remaining data transfer length.
extern uint32_t g_scsi_data_remaining;

//! Sense data.
extern s_scsi_sense g_scsi_sense;

//! USB mass-storage IN endpoint.
extern uint8_t g_scsi_ep_ms_in;

//! USB mass-storage OUT endpoint.
extern uint8_t g_scsi_ep_ms_out;

/*! \brief Decodes and launches the execution of SCSI commands.
 *
 * \return Boolean indicating success.
 */
extern bool scsi_decode_command(void);

/*! \brief Manages the TEST UNIT READY command.
 *
 * The TEST UNIT READY command provides a means to check if the logical unit is
 * ready. This is not a request for a self-test. If the logical unit is able to
 * accept an appropriate medium-access command without returning CHECK CONDITION
 * status, this command shall return a GOOD status. If the logical unit is
 * unable to become operational or is in a state such that an application client
 * action (e.g., START UNIT command) is required to make the logical unit ready,
 * the command shall be terminated with CHECK CONDITION status, with the sense
 * key set to NOT READY.
 *
 * \return Boolean indicating success.
 */
extern bool sbc_test_unit_ready(void);

/*! \brief Manages the REQUEST SENSE command.
 *
 * The REQUEST SENSE command requests that the device server transfer sense data
 * to the application client.
 *
 * See \ref s_scsi_sense.
 *
 * \return Boolean indicating success.
 */
extern bool sbc_request_sense(void);

/*! \brief Manages the INQUIRY command.
 *
 * The INQUIRY command requests that information regarding the logical unit and
 * SCSI target device be sent to the application client.
 *
 * See \ref sbc_st_std_inquiry_data.
 *
 * \return Boolean indicating success.
 */
extern bool sbc_inquiry(void);

/*! \brief Manages the MODE SENSE (6) and (10) commands.
 *
 * The MODE SENSE (6/10) commands provide a means for a device server to report
 * parameters to an application client. They are complementary commands to the
 * MODE SELECT (6/10) commands. Device servers that implement the MODE SENSE
 * (6/10) commands shall also implement the MODE SELECT (6/10) commands.
 *
 * \param b_sense_10  Boolean indicating whether the (10) version of the command
 *                    is requested:
 *   \arg \c true to specify a MODE SENSE (10) command;
 *   \arg \c false to specify a MODE SENSE (6) command.
 *
 * \return Boolean indicating success.
 */
extern bool sbc_mode_sense(bool b_sense_10);

/*! \brief Manages the PREVENT ALLOW MEDIUM REMOVAL command.
 *
 * The PREVENT ALLOW MEDIUM REMOVAL command requests that the logical unit
 * enable or disable the removal of the medium. The logical unit shall not allow
 * medium removal if any initiator port currently has medium removal prevented.
 *
 * \return Boolean indicating success.
 */
extern bool sbc_prevent_allow_medium_removal(void);

/*! \brief Manages the READ CAPACITY (10) command.
 *
 * The READ CAPACITY (10) command requests that the device server transfer 8
 * bytes of parameter data describing the capacity and medium format of the
 * direct-access block device to the data-in buffer. This command may be
 * processed as if it has a HEAD OF QUEUE task attribute. If the logical unit
 * supports protection information, the application client should use the READ
 * CAPACITY (16) command instead of the READ CAPACITY (10) command.
 *
 * \return Boolean indicating success.
 */
extern bool sbc_read_capacity(void);

/*! \brief Manages the READ (10) command.
 *
 * The READ (10) command requests that the device server read the specified
 * logical block(s) and transfer them to the data-in buffer. Each logical block
 * read includes user data and, if the medium is formatted with protection
 * information enabled, protection information. Each logical block transferred
 * includes user data and may include protection information, based on the
 * \c RDPROTECT field and the medium format. The most recent data value written
 * in the addressed logical block shall be returned.
 *
 * \return Boolean indicating success.
 */
extern bool sbc_read_10(void);

/*! \brief Manages the WRITE (10) command.
 *
 * The WRITE (10) command requests that the device server transfer the specified
 * logical block(s) from the data-out buffer and write them. Each logical block
 * transferred includes user data and may include protection information, based
 * on the \c WRPROTECT field and the medium format. Each logical block written
 * includes user data and, if the medium is formatted with protection
 * information enabled, protection information.
 *
 * \return Boolean indicating success.
 */
extern bool sbc_write_10(void);

/*! \brief Sends the status "GOOD".
 */
extern void sbc_lun_status_is_good(void);

/*! \brief Sends the status "LUN not present".
 */
extern void sbc_lun_status_is_not_present(void);

/*! \brief Sends the status "busy or changed".
 */
extern void sbc_lun_status_is_busy_or_change(void);

/*! \brief Sends the status "FAILED".
 */
extern void sbc_lun_status_is_fail(void);

/*! \brief Sends the status "FAILED because of write protection".
 */
extern void sbc_lun_status_is_protected(void);

/*! \brief Sends the status "FAIL because CDB field is invalid".
 */
extern void sbc_lun_status_is_cdb_field(void);

#endif  // USB_DEVICE_FEATURE == true


#endif  // _SCSI_DECODER_H_
