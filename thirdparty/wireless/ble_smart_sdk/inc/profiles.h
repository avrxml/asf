/**************************************************************************//**
  \file profiles.h
 
  \brief Includes datatypes and signatures for profiles
 
  * Copyright (c) 2016-2018 Microchip Technology Inc. and its subsidiaries.
  Released under NDA
  Licensed under Atmel's Limited License Agreement.
 
 
  THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
  EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
 
  Microchip Technology Inc: http://www.microchip.com
 
******************************************************************************/

/*
 * profiles.h
 *
 *  Created on: Mar 12, 2015
 *      Author: aabdelfattah
 */


#ifndef PRF_DEFS_H_
#define PRF_DEFS_H_

/// Message structure used to inform APP that an error has occurred in the profile server role task
typedef struct prf_server_error_ind
{
    /// Connection Handle
    uint16_t conhdl;
    /// Message ID
    uint16_t msg_id;
    /// Status
    uint8_t status;
}prf_server_error_ind_t;
/// Time profile information
typedef struct prf_date_time
{
    /// year time element
    uint16_t year;
    /// month time element
    uint8_t month;
    /// day time element
    uint8_t day;
    /// hour time element
    uint8_t hour;
    /// minute time element
    uint8_t min;
    /// second time element
    uint8_t sec;
}prf_date_time_t;

#endif  /* PRF_DEFS_H_ */
