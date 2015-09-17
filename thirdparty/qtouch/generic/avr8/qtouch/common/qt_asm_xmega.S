/* This assembly file is part of the ATMEL QTouch Library Release 4.4 */
/*! \file *********************************************************************
 *
 * \brief  This file contains the QTouch Libary AVR 8-bit Xmega assembly
 *         code for QTouch method Capacitive Touch acquisition.        
 *
 * - Compiler:           IAR EWAVR and GNU GCC for AVR
 * - Supported devices:  Atmel AVR 8-bit Xmega.
 * - Userguide:          QTouch Library User Guide - doc8207.pdf.
 * - Support:            http://www.atmel.com/design-support/
 *
 * \author               Atmel Corporation: http://www.atmel.com
 *
 ******************************************************************************/

/* Copyright (c) 2010-2014 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

#include "touch_qt_config.h"

FILE_HEADER

FILE_SEGMENT

#if (_STATIC_PORT_PIN_CONF_ == 0)

GLOBAL_FUNCTION _0111010111_
_0111010111_:
    sts	CONCAT( PORT, SNS1, _DIRCLR ), p_1
    sts	CONCAT( PORT, SNSK1, _DIRCLR ), p_1
    sts	CONCAT( PORT, SNS1, _OUTSET), p_1
    sts	CONCAT( PORT, SNSK1, _OUTCLR), p_1    
    sts	CONCAT( PORT, SNSK1, _DIRSET), p_1    
    ret

GLOBAL_FUNCTION _01001010111_
_01001010111_:
    push r19
    sts	CONCAT( PORT, SNSK1, _DIRCLR), p_1
    sts	CONCAT( PORT, SNS1, _DIRSET), p_1
#if   (QT_DELAY_CYCLES == 1)	
#elif (QT_DELAY_CYCLES == 2)
    _00011001_
#elif (QT_DELAY_CYCLES == 3)
    _00011001_
    _00011001_
#elif ((QT_DELAY_CYCLES - 1) - (3 * ((QT_DELAY_CYCLES - 1)/3)) == 0)
    _11100011_
    _10100011_
    _01101001_
#elif ((QT_DELAY_CYCLES - 1) - (3 * ((QT_DELAY_CYCLES - 1)/3)) == 1)
    _11100011_
    _10100011_
    _01101001_
    _00011001_
#else
    _11100011_
    _10100011_
    _01101001_
    _00011001_
    _00011001_
#endif  
    sts	CONCAT( PORT, SNS1, _DIRCLR ), p_1
    sts	CONCAT( PORT, SNSK1, _DIRSET ), p_1
    pop  r19
    lds	r_v, CONCAT(PORT, SNS1, _IN )
    and	r_v, p_2
    ret

GLOBAL_FUNCTION _0101110111_
_0101110111_:
    sts	CONCAT( PORT, SNS1, _OUTCLR), p_1
    sts	CONCAT( PORT, SNS1, _DIRSET), p_1
    ret
   
#endif

#if ((_STATIC_PORT_PIN_CONF_ == 1) && (QTOUCH_SNS_PORT_COUNT >= 1))

GLOBAL_FUNCTION _1101010101_
_1101010101_:
    sts	CONCAT( PORT, SNS1, _DIRCLR ), p_1
    sts	CONCAT( PORT, SNSK1, _DIRCLR ), p_2
    sts	CONCAT( PORT, SNS1, _OUTSET), p_1
    sts	CONCAT( PORT, SNSK1, _OUTCLR), p_2    
    sts	CONCAT( PORT, SNSK1, _DIRSET), p_2   
    ret


GLOBAL_FUNCTION _1101010111_
_1101010111_:
    push r19
    sts	CONCAT( PORT, SNSK1, _DIRCLR), p_2
    sts	CONCAT( PORT, SNS1, _DIRSET), p_1
#if (QT_DELAY_CYCLES == 1)	
#elif (QT_DELAY_CYCLES == 2)
    _00011001_
#elif (QT_DELAY_CYCLES == 3)
    _00011001_
    _00011001_
#elif ((QT_DELAY_CYCLES - 1) - (3 * ((QT_DELAY_CYCLES - 1)/3)) == 0)
    _11100011_
    _10100011_
    _01101001_
#elif ((QT_DELAY_CYCLES - 1) - (3 * ((QT_DELAY_CYCLES - 1)/3)) == 1)
    _11100011_
    _10100011_
    _01101001_
    _00011001_
#else
    _11100011_
    _10100011_
    _01101001_
    _00011001_
    _00011001_
#endif
    sts	CONCAT( PORT, SNS1, _DIRCLR ), p_1
    sts	CONCAT( PORT, SNSK1, _DIRSET ), p_2 
    pop  r19
    lds	r_v, CONCAT(PORT, SNS1, _IN )
    and	r_v, p_3
    ret

GLOBAL_FUNCTION _1101010110_
_1101010110_:
    sts	CONCAT( PORT, SNS1, _OUTCLR), p_1
    sts	CONCAT( PORT, SNS1, _DIRSET), p_1
    ret
    
#endif

#if ((_STATIC_PORT_PIN_CONF_ == 1) && (QTOUCH_SNS_PORT_COUNT == 2))

GLOBAL_FUNCTION _1101011111_
_1101011111_:
    sts	CONCAT( PORT, SNS2, _DIRCLR ), p_1
    sts	CONCAT( PORT, SNSK2, _DIRCLR ), p_2
    sts	CONCAT( PORT, SNS2, _OUTSET), p_1
    sts	CONCAT( PORT, SNSK2, _OUTCLR), p_2    
    sts	CONCAT( PORT, SNSK2, _DIRSET), p_2   
    ret

GLOBAL_FUNCTION _1101011110_
_1101011110_:
    push r19
    sts	CONCAT( PORT, SNSK2, _DIRCLR), p_2
    sts	CONCAT( PORT, SNS2, _DIRSET), p_1
#if (QT_DELAY_CYCLES == 0)	
#elif (QT_DELAY_CYCLES == 1)
    _00011001_
#elif (QT_DELAY_CYCLES == 2)
    _00011001_
    _00011001_
#elif ((QT_DELAY_CYCLES - 1) - (3 * ((QT_DELAY_CYCLES - 1)/3)) == 0)
    _11100011_
    _10100111_
    _01101011_
#elif ((QT_DELAY_CYCLES - 1) - (3 * ((QT_DELAY_CYCLES - 1)/3)) == 1)
    _11100011_
    _10100111_
    _01101011_
    _00011001_
#else
    _11100011_
    _10100111_
    _01101011_
    _00011001_
    _00011001_
#endif
    sts	CONCAT( PORT, SNS2, _DIRCLR ), p_1
    sts	CONCAT( PORT, SNSK2, _DIRSET ), p_2 
    pop  r19
    lds	r_v, CONCAT(PORT, SNS2, _IN )
    and	r_v, p_3
    ret

GLOBAL_FUNCTION _1101011010_
_1101011010_:
    sts	CONCAT( PORT, SNS2, _OUTCLR), p_1
    sts	CONCAT( PORT, SNS2, _DIRSET), p_1
    ret

#endif

FILE_FOOTER
