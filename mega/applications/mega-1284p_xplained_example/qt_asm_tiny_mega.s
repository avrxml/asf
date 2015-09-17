/*******************************************************************************
*   $FILE:  qt_asm_tiny_mega.S
*   Atmel Corporation:  http://www.atmel.com \n
*   Support: http://www.atmel.com/design-support/
******************************************************************************/

/**
 *
 * Copyright (c) 2010-2014 Atmel Corporation. All rights reserved.
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

#include "touch_qt_config.h"

FILE_HEADER

FILE_SEGMENT

#if (_POWER_OPTIMIZATION_ == 1)
#if defined(__IAR_SYSTEMS_ASM__)
EXPORT temp1
EXPORT temp2
temp1 GLOBAL_VAR 0
temp2 GLOBAL_VAR 0
#else
.global temp1
.global temp2
.EQU temp1,0
.EQU temp2,0
#endif
#endif

#if (_STATIC_PORT_PIN_CONF_ == 0)

GLOBAL_FUNCTION _0111010111_
_0111010111_:
    push r19
    push r25
    in   r19, REG( DDR, SNS1 )
    mov  r25, p_1
    com  r25
    and  r19, r25
    out  REG( DDR, SNS1 ), r19
    in   r19, REG( DDR, SNSK1 )
    and  r19, r25
    out  REG( DDR, SNSK1 ), r19
    in   r19, REG( PORT, SNS1 )
#if (_POWER_OPTIMIZATION_ == 1)
    sts  temp1, r19
#endif
    or   r19, p_1
    out  REG( PORT, SNS1 ), r19
    in   r19, REG( PORT, SNSK1 )
    and  r19, r25
    out  REG( PORT, SNSK1 ), r19
    in   r19, REG( DDR, SNSK1 )
    or   r19, p_1
    out  REG( DDR, SNSK1 ), r19
#if (_POWER_OPTIMIZATION_ == 1)
    sts  temp2, r19
#endif
    pop  r25
    pop  r19
    ret

#if (_POWER_OPTIMIZATION_ == 0)
GLOBAL_FUNCTION _01001010111_
_01001010111_:
    push r19
    push r23
    push r25
    in   r23, REG( DDR, SNSK1 )
    mov  r25, p_1
    com  r25
    and  r23, r25
    out  REG( DDR, SNSK1 ), r23
    in   r19, REG( DDR, SNS1 )
    or   r19, p_1
    and  r25, r19
    or   r23, p_1
    out  REG( DDR, SNS1 ), r19
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
    out  REG( DDR, SNS1 ), r25
    out  REG( DDR, SNSK1 ), r23
    pop  r25
    in   r_v, REG( PIN, SNS1 )
    and  r_v, p_2
    pop  r23
    pop  r19
    ret

#else

GLOBAL_FUNCTION _01001010111_
_01001010111_:
    out  REG( DDR, SNSK1 ), p_4
    out  REG( DDR, SNS1 ), p_1
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
    out  REG( DDR, SNS1 ), p_2
    out  REG( DDR, SNSK1 ), p_3
    nop
    in   r_v, REG( PIN, SNS1 )
    and  r_v, p_5
    ret

#endif

GLOBAL_FUNCTION _0101110111_
_0101110111_:
    push r19
    push r25
    mov  r25, p_1
    com  r25
    in   r19, REG( PORT, SNS1 )
    and  r19, r25
    out  REG( PORT, SNS1 ), r19
    in   r19, REG( DDR, SNS1 )
    or   r19, p_1
    out  REG( DDR, SNS1 ), r19
    pop  r25
    pop  r19
    ret

#endif
///////////////////////////////////////////////////////

#if ((_STATIC_PORT_PIN_CONF_ == 1) && (QTOUCH_SNS_PORT_COUNT >= 1))

GLOBAL_FUNCTION _1101010101_
_1101010101_:
    push r19
    push r23
    push r25
    mov  r23, p_2
    com  r23
    mov  r25, p_1
    com  r25
    in   r19, REG( DDR, SNS1 )
    and  r19, r25
    out  REG( DDR, SNS1 ), r19
#if (_POWER_OPTIMIZATION_ == 1)
    sts  temp1, r19
#endif
    in   r19, REG( DDR, SNSK1 )
    and  r19, r23
    out  REG( DDR, SNSK1 ), r19
    in   r19, REG( PORT, SNS1 )
    or   r19, p_1
    out  REG( PORT, SNS1 ), r19
    in   r19, REG( PORT, SNSK1 )
    and  r19, r23
    out  REG( PORT, SNSK1 ), r19
    in   r19, REG( DDR, SNSK1 )
#if (_POWER_OPTIMIZATION_ == 1)
    sts  temp1, r19
#endif
    or   r19, p_2
    out  REG( DDR, SNSK1 ), r19
    pop  r25
    pop  r23
    pop  r19
    ret

#if (_POWER_OPTIMIZATION_ == 0)
GLOBAL_FUNCTION _1101010111_
_1101010111_:
    push r19
    push r23
    push r25
    mov  r23, p_2
    com  r23
    mov  r25, p_1
    com  r25
    in   r19, REG( DDR, SNSK1 )
    and  r23, r19
    out  REG( DDR, SNSK1 ), r23
    in   r19, REG( DDR, SNS1 )
    or   r23, p_2
    or   r19, p_1
    and  r25, r19
    out  REG( DDR, SNS1 ), r19
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
    out  REG( DDR, SNS1 ), r25
    out  REG( DDR, SNSK1 ), r23
    pop  r25
    in   r_v, REG( PIN, SNS1 )
    and  r_v, p_3
    pop  r23
    pop  r19
    ret

#else
GLOBAL_FUNCTION _1101010111_
_1101010111_:
    out  REG( DDR, SNSK1 ), p_4
    out  REG( DDR, SNS1 ), p_1
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
    out  REG( DDR, SNS1 ), p_2
    out  REG( DDR, SNSK1 ), p_3
    nop
    in   r_v, REG( PIN, SNS1 )
    and  r_v, p_5
    ret
#endif

GLOBAL_FUNCTION _1101010110_
_1101010110_:
    push r19
    push r25
    mov  r25, p_1
    com  r25
    in   r19, REG( PORT, SNS1 )
    and  r19, r25
    out  REG( PORT, SNS1 ), r19
    in   r19, REG( DDR, SNS1 )
    or   r19, p_1
    out  REG( DDR, SNS1 ), r19
    pop  r25
    pop  r19
    ret

#endif

#if ((_STATIC_PORT_PIN_CONF_ == 1) && (QTOUCH_SNS_PORT_COUNT == 2))

GLOBAL_FUNCTION _1101011111_
_1101011111_:
    push r19
    push r23
    push r25
    mov  r23, p_2
    com  r23
    mov  r25, p_1
    com  r25
    in   r19, REG( DDR, SNS2 )
#if (_POWER_OPTIMIZATION_ == 1)
    sts  temp1, r19
#endif
    and  r19, r25
    out  REG( DDR, SNS2 ), r19
    in   r19, REG( DDR, SNSK2 )
    and  r19, r23
    out  REG( DDR, SNSK2 ), r19
    in   r19, REG( PORT, SNS2 )
    or   r19, p_1
    out  REG( PORT, SNS2 ), r19
    in   r19, REG( PORT, SNSK2 )
    and  r19, r23
    out  REG( PORT, SNSK2 ), r19
    in   r19, REG( DDR, SNSK2 )
#if (_POWER_OPTIMIZATION_ == 1)
    sts  temp1, r19
#endif
    or   r19, p_2
    out  REG( DDR, SNSK2 ), r19
    pop  r25
    pop  r23
    pop  r19
    ret

#if (_POWER_OPTIMIZATION_ == 0)
GLOBAL_FUNCTION _1101011110_
_1101011110_:
    push r19
    push r23
    push r25
    mov  r23, p_2
    com  r23
    mov  r25, p_1
    com  r25
    in   r19, REG( DDR, SNSK2 )
    and  r23, r19
    out  REG( DDR, SNSK2 ), r23
    in   r19, REG( DDR, SNS2 )
    or   r23, p_2
    or   r19, p_1
    and  r25, r19
    out  REG( DDR, SNS2 ), r19
#if (QT_DELAY_CYCLES == 1)
#elif (QT_DELAY_CYCLES == 2)
    _00011001_
#elif (QT_DELAY_CYCLES == 3)
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
    out  REG( DDR, SNS2 ), r25
    out  REG( DDR, SNSK2 ), r23
    pop  r25
    in   r_v, REG( PIN, SNS2 )
    and  r_v, p_3
    pop  r23
    pop  r19
    ret

#else

GLOBAL_FUNCTION _1101011110_
_1101011110_:
    out  REG( DDR, SNSK2 ), p_4
    out  REG( DDR, SNS2 ), p_1
#if (QT_DELAY_CYCLES == 1)
#elif (QT_DELAY_CYCLES == 2)
    _00011001_
#elif (QT_DELAY_CYCLES == 3)
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
    out  REG( DDR, SNS2 ), p_2
    out  REG( DDR, SNSK2 ), p_3
    nop
    in   r_v, REG( PIN, SNS2 )
    and  r_v, p_5
    ret

#endif

GLOBAL_FUNCTION _1101011010_
_1101011010_:
    push r19
    push r25
    mov  r25, p_1
    com  r25
    in   r19, REG( PORT, SNS2 )
    and  r19, r25
    out  REG( PORT, SNS2 ), r19
    in   r19, REG( DDR, SNS2 )
    or   r19, p_1
    out  REG( DDR, SNS2 ), r19
    pop  r25
    pop  r19
    ret

#endif


FILE_FOOTER
