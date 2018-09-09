/**
* \file
*
* \brief The definition of image data arrays
*
* Copyright (c) 2012-2014 Pervasive Displays Inc. All rights reserved.
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
*/

#define USE_EPD144 0 /**< 1.44 inch PDi EPD */
#define USE_EPD200 1 /**< 2 inch PDi EPD */
#define USE_EPD270 2 /**< 2.7 inch PDi EPD */

#if(USE_EPD_Type==USE_EPD144) // array size=96*16, (128/8=16)
	extern unsigned char const image_array_144_1[]; /**< use 1.44" image array #1 */
	extern unsigned char const image_array_144_2[]; /**< use 1.44" image array #2 */
#elif(USE_EPD_Type==USE_EPD200) // array size=96*25, (200/8=25)
	extern unsigned char const image_array_200_1[]; /**< use 2" image array #1 */
	extern unsigned char const image_array_200_2[]; /**< use 2" image array #2 */
#elif(USE_EPD_Type==USE_EPD270) // array size=176*33, (264/8=33)
	extern unsigned char const image_array_270_1[]; /**< use 2.7" image array #1 */
	extern unsigned char const image_array_270_2[]; /**< use 2.7" image array #2 */
#endif
