/*****************************************************************************
 *
 * \file
 *
 * \brief QDEC driver for AVR32 UC3.
 *
 * AVR32 QDEC driver module.
 *
 * Copyright (c) 2014-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 ******************************************************************************/
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */


#ifndef _QDEC_H_
#define _QDEC_H_

/**
 * \defgroup group_avr32_drivers_qdec QDEC - Quadrature Decoder
 *
 * Driver for the QDEC (Quadrature Decoder). The QDEC handles three input channels:
 * two phase signals (QEPA, QEPB) and one index pulse (QEPI).
 * It has a 16-bit position counter and a 16-bit revolution counter
 * and supports 32-bit timer/counter mode.
 *
 * \{
 */

#include <avr32/io.h>

#include "compiler.h"

//  These defines are missing from or wrong in the toolchain header file ip_xxx.h or part.h
#ifdef AVR32_QDEC_100_H_INCLUDED
#define AVR32_QDEC0_IRQ_GROUP           31
#define AVR32_QDEC1_IRQ_GROUP           32
#endif

//! QDEC driver functions return value in case of invalid argument(s).
#define QDEC_INVALID_ARGUMENT                     (-1)

/*! \name QDEC Index Phase Detection
 */
//! @{
#define QDEC_IDXPHS_QEPB_0_QEPA_0
#define QDEC_IDXPHS_QEPB_0_QEPA_1
#define QDEC_IDXPHS_QEPB_1_QEPA_0
#define QDEC_IDXPHS_QEPB_1_QEPA_1
//! @}

//! QDEC Mode Selection
#define QDEC_QDEC_MODE                    1
//! TIMER Mode Selection
#define QDEC_TIMER_MODE                   0

//! Direction Up In TIMER Mode
#define QDEC_TSIR_UP                       0
//! Direction Down In TIMER Mode.
#define QDEC_TSIR_DOWN                     1
//! TSDIR Changes Input In TIMER Mode
#define QDEC_UPD_TSDIR                     0
//! EVENT Changes Input In TIMER Mode
#define QDEC_UPD_EVNT                      1

//! QDEC interrupts.
typedef struct
{
  unsigned int                      :24;

  //! Count Direction Inversion Detection Mask Bit Enable
  unsigned int ovr                  :1;

  //! Count Direction Inversion Detection Mask Bit Enable
  unsigned int dirinv               :1;

  //! Index Error Detection Mask Bit Enable
  unsigned int idexerr              :1;

  //! Revolution Counter Roll Over Detection Mask Bit Enable
  unsigned int rcro                 :1;

  //! Position Counter Roll Over Detection Mask Bit Enable.
  unsigned int pcro                 :1;

  //! Counter Capture Detection Mask Bit Enable
  unsigned int cap                  :1;

  //! Counter Compare Detection Mask Bit Enable
  unsigned int cmp                  :1;

  //! Index Signal Detection Mask Bit Enable.
  unsigned int qepi                 :1;

} qdec_interrupt_t;

//! Parameters when initializing a QDEC in Quadrature Decoder mode.
typedef struct
{
  unsigned int                        :5;

  unsigned int                        :3;

  //!
  unsigned int                        :6;

  //!
  unsigned int                        :2;

  //!
  unsigned int                        :2;

  //! Digital Filter Enable
  unsigned int filten                 :1;

  //! quadrature phase of index detection
  unsigned int idxphs                 :2;

  //! Index signal Inversion
  unsigned int idxinv                 :1;

  //! Phase Signals Inversion
  unsigned int phsinvb                :1;

  //! Phase Signals Inversion
  unsigned int phsinva                :1;

  //!
  unsigned int                        :3;

  //! Event Trigger Enable
  unsigned int evtrge                 :1;

  //! Revolution Counter Compare Enable
  unsigned int rcce                   :1;

  //! Position Counter Compare Enable
  unsigned int pcce                   :1;

  //! Index detection enable
  unsigned int idxe                   :1;

  //! Quadrature decoder module enable
  unsigned int qdec                   :1;

} qdec_quadrature_decoder_opt_t;

//! Parameters when initializing a QDEC in Counter mode.
typedef struct
{
  unsigned int                        :5;

  //! QDEC Clock Selection
  unsigned int clks                   :3;

  //!
  unsigned int                        :6;

  //! Up/Down Mode Timer
  unsigned int upd                    :1;

  //! Timer Set Direction
  unsigned int tsdir                  :1;

  unsigned int                        :2;

  //! Digital Filter Enable
  unsigned int filten                 :1;

  //!
  unsigned int                        :5;

  //!
  unsigned int                        :3;

  //! Event Trigger Enable
  unsigned int evtrge                 :1;

  //! Revolution Counter Compare Enable
  unsigned int rcce                   :1;

  //! Position Counter Compare Enable
  unsigned int pcce                   :1;

  //!
  unsigned int                        :1;

  //! Quadrature decoder module enable
  unsigned int qdec                   :1;

} qdec_timer_opt_t;

/*! \brief Get the QDEC current interrupt settings.
 *
 * \param qdec Pointer to the QDEC instance to access.
 *
 * \retval >=0 The interrupt enable configuration organized according to \ref qdec_interrupt_t.
 * \retval Interrupt Settings Value.
 */
extern int qdec_get_interrupt_settings(volatile avr32_qdec_t *qdec);

/*! \brief Enables various QDEC interrupts.
 *
 * \param qdec      Pointer to the QDEC instance to access.
 * \param bitfield  The interrupt enable configuration.
 * \retval 0        Success.
 */
extern int qdec_configure_interrupts(volatile avr32_qdec_t *qdec, const qdec_interrupt_t *bitfield);


/*! \brief Sets options for QDEC quadrature decoder initialization.
 *
 * \param qdec    Pointer to the QDEC instance to access.
 * \param opt     Options for quadrature decoder mode.
 * \retval 0      Success.
 */
extern int qdec_init_quadrature_decoder_mode(volatile avr32_qdec_t *qdec, const qdec_quadrature_decoder_opt_t *opt);

/*! \brief Sets options for QDEC Timer mode initialization.
 *
 * \param qdec    Pointer to the QDEC instance to access.
 * \param opt     Options for quadrature decoder mode.
 * \retval 0      Success.
 */
extern int qdec_init_timer_mode(volatile avr32_qdec_t *qdec, const qdec_timer_opt_t *opt);

/*! \brief Software Trigger Condition to start QDEC service.
 *
 * \param qdec     Pointer to the QDEC instance to access.
 * \retval 0       Success.
 */
extern int qdec_software_trigger(volatile avr32_qdec_t *qdec);

/*! \brief Stops a QDEC.
 *
 * \param qdec     Pointer to the QDEC instance to access.
 * \retval 0       Success.
 */
extern int qdec_stop(volatile avr32_qdec_t *qdec);

/*! \brief Read value of the channel's QDEC Revolution Current Counter  register.
 *
 * \param qdec      Pointer to the QDEC instance to access.
 * \retval >=0      Status register value.
 */
extern unsigned short int qdec_read_rc(volatile avr32_qdec_t *qdec);

/*! \brief Read value of the channel's QDEC Position Current Counter  register.
 *
 * \param qdec      Pointer to the QDEC instance to access.
 * \retval >=0      Status register value.
 */
extern unsigned short int qdec_read_pc(volatile avr32_qdec_t *qdec);

/*! \brief Write value of the channel's QDEC Revolution Current Counter  register.
 *
 * \param qdec      Pointer to the QDEC instance to access.
 * \param value     Value to write to the RC register.
 *
 * \retval >=0      Status register value.
 */
extern unsigned short int qdec_write_rc_cnt(volatile avr32_qdec_t *qdec,unsigned short int value);

/*! \brief Write value of the channel's QDEC Position Current Counter  register.
 *
 * \param qdec      Pointer to the QDEC instance to access.
 * \param value     Value to write to the PC register.
 *
 * \retval >=0      Status register value.
 */
extern unsigned short int qdec_write_pc_cnt(volatile avr32_qdec_t *qdec,unsigned short int value);


/*! \brief Writes a value to the channel's QDEC Revolution Counter Top register.
 *
 * \param qdec      Pointer to the QDEC instance to access.
 * \param value     Value to write to the RC register.
 *
 * \retval >=0      Written value.
 */
extern unsigned short int qdec_write_rc_top(volatile avr32_qdec_t *qdec, unsigned short int value);

/*! \brief Writes a value to the channel's QDEC Position Counter Top register.
 *
 * \param qdec              Pointer to the QDEC instance to access.
 * \param value           Value to write to the RC register.
 *
 * \retval >=0 Written value.
 * \retval QDEC_INVALID_ARGUMENT Invalid argument(s).
 */
extern unsigned short int qdec_write_pc_top(volatile avr32_qdec_t *qdec, unsigned short int value);

/*! \brief Writes a value to the channel's QDEC Revolution Counter Compare register.
 *
 * \param qdec              Pointer to the QDEC instance to access.
 * \param value           Value to write to the RC register.
 *
 * \retval >=0 Written value.
 * \retval QDEC_INVALID_ARGUMENT Invalid argument(s).
 */
extern unsigned short int qdec_write_rc_cmp(volatile avr32_qdec_t *qdec, unsigned short int value);

/*! \brief Writes a value to the channel's QDEC Position Counter Compare register.
 *
 * \param qdec              Pointer to the QDEC instance to access.
 * \param value           Value to write to the RC register.
 *
 * \retval >=0 Written value.
 * \retval QDEC_INVALID_ARGUMENT Invalid argument(s).
 */
extern unsigned short int qdec_write_pc_cmp(volatile avr32_qdec_t *qdec, unsigned short int value);


/*! \brief Read value of the channel's QDEC Position Capture Counter  register.
 *
 * \param qdec              Pointer to the QDEC instance to access.
 *
 * \retval >=0 Status register value.
 * \retval QDEC_INVALID_ARGUMENT Invalid argument(s).
 */
extern unsigned short int qdec_read_pc_cap(volatile avr32_qdec_t *qdec);


/*! \brief Read value of the channel's QDEC Revolution Capture register.
 *
 * \param qdec              Pointer to the QDEC instance to access.
 *
 * \retval >=0 Status register value.
 * \retval QDEC_INVALID_ARGUMENT Invalid argument(s).
 */
extern unsigned short int qdec_read_rc_cap(volatile avr32_qdec_t *qdec);

/**
 * \}
 */

#endif // _QDEC_H_



