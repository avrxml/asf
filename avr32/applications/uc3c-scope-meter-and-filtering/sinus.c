/**
 * \file
 *
 * \brief Sinus File.
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
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include "sinus.h"

uint16_t sinus_table[SINUS_NB_SAMPLES] = {
    2222,
    2412,
    2601,
    2785,
    2963,
    3133,
    3293,
    3441,
    3576,
    3696,
    3799,
    3886,
    3954,
    4003,
    4032,
    4042,
    4032,
    4002,
    3952,
    3883,
    3796,
    3692,
    3571,
    3436,
    3287,
    3127,
    2957,
    2778,
    2594,
    2405,
    2215,
    2024,
    1836,
    1652,
    1474,
    1305,
    1145,
    997,
    863,
    744,
    641,
    555,
    488,
    439,
    410,
    401,
    413,
    444,
    494,
    563,
    651,
    756,
    877,
    1013,
    1162,
    1323,
    1493,
    1672,
    1856,
    2045
};

