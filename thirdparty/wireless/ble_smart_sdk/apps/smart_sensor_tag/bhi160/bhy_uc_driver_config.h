/*
****************************************************************************
* Copyright (C) 2015 - 2018 Bosch Sensortec GmbH
*
* Usage: APIs and Drivers for BHI160
*
****************************************************************************
* License:
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*
*   Redistributions of source code must retain the above copyright
*   notice, this list of conditions and the following disclaimer.
*
*   Redistributions in binary form must reproduce the above copyright
*   notice, this list of conditions and the following disclaimer in the
*   documentation and/or other materials provided with the distribution.
*
*   Neither the name of the copyright holder nor the names of the
*   contributors may be used to endorse or promote products derived from
*   this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER
* OR CONTRIBUTORS BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
* OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO,
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
* ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
*
* The information provided is believed to be accurate and reliable.
* The copyright holder assumes no responsibility
* for the consequences of use
* of such information nor for any infringement of patents or
* other rights of third parties which may result from its use.
* No license is granted by implication or otherwise under any patent or
* patent rights of the copyright holder.
**************************************************************************/




#ifndef BHY_UC_DRIVER_CONFIG_H_
#define BHY_UC_DRIVER_CONFIG_H_

/****************************************************************************/
/*						Driver configuration								*/
/****************************************************************************/

/* Enabling BHY_DEBUG will allow you to parse the strings coming out of a	*/
/* debug firmware in the BHy. If using a standard firmware then it is not	*/
/* necessary																*/
#define BHY_DEBUG 0

/* Enabling BHY_CALLBACK_MODE will allow you to install callback function	*/
/* that will be called from the bhy_parse_next_fifo_packet function. is uses*/
/* ~250 bytes of ROM and ~350 bytes of RAM									*/
#define BHY_CALLBACK_MODE 1

/* Enabling BST_APPLICATION_BOARD will modify the driver in order to work 	*/
/* over USB on the BST application board                                    */
#define BST_APPLICATION_BOARD 0

#endif /* BHY_UC_DRIVER_CONFIG_H_ */