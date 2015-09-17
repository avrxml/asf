/*****************************************************************************
 *
 * \file
 *
 * \brief Management of the generic HID features.
 *
 * This file manages the generic HID features.
 *
 * Copyright (c) 2014-2015 Atmel Corporation. All rights reserved.
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
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */


#ifndef _HID_H_
#define _HID_H_


//_____ I N C L U D E S ________________________________________________________


//_____ M A C R O S ____________________________________________________________

/*! \name HID Descriptor Types
 */
//! @{
#define HID_DESCRIPTOR                          0x21
#define HID_REPORT_DESCRIPTOR                   0x22
#define HID_PHYSICAL_DESCRIPTOR                 0x23
//! @}

/*! \name Constants of field DESCRIPTOR_HID
 */
//! @{
#define  HID_BDC                              0x0111  //!< Numeric expression identifying the HID Class Specification release (here V1.11)
#define  HID_CLASS_DESC_NB_DEFAULT            0x01    //!< Numeric expression specifying the number of class descriptors (always at least one i.e. Report descriptor.)
//! @}

/*! \name HID Country Codes
 */
//! @{
#define HID_COUNTRY_CODE_NOT_SUPPORTED          0
#define HID_COUNTRY_CODE_ARABIC                 1
#define HID_COUNTRY_CODE_BELGIAN                2
#define HID_COUNTRY_CODE_CANADIAN_BILINGUAL     3
#define HID_COUNTRY_CODE_CANADIAN_FRENCH        4
#define HID_COUNTRY_CODE_CZECH_REPUBLIC         5
#define HID_COUNTRY_CODE_DANISH                 6
#define HID_COUNTRY_CODE_FINNISH                7
#define HID_COUNTRY_CODE_FRENCH                 8
#define HID_COUNTRY_CODE_GERMAN                 9
#define HID_COUNTRY_CODE_GREEK                  10
#define HID_COUNTRY_CODE_HEBREW                 11
#define HID_COUNTRY_CODE_HUNGARY                12
#define HID_COUNTRY_CODE_INTERNATIONAL_ISO      13
#define HID_COUNTRY_CODE_ITALIAN                14
#define HID_COUNTRY_CODE_JAPAN_KATAKANA         15
#define HID_COUNTRY_CODE_KOREAN                 16
#define HID_COUNTRY_CODE_LATIN_AMERICAN         17
#define HID_COUNTRY_CODE_NETHERLANDS_DUTCH      18
#define HID_COUNTRY_CODE_NORWEGIAN              19
#define HID_COUNTRY_CODE_PERSIAN_FARSI          20
#define HID_COUNTRY_CODE_POLAND                 21
#define HID_COUNTRY_CODE_PORTUGUESE             22
#define HID_COUNTRY_CODE_RUSSIA                 23
#define HID_COUNTRY_CODE_SLOVAKIA               24
#define HID_COUNTRY_CODE_SPANISH                25
#define HID_COUNTRY_CODE_SWEDISH                26
#define HID_COUNTRY_CODE_SWISS_FRENCH           27
#define HID_COUNTRY_CODE_SWISS_GERMAN           28
#define HID_COUNTRY_CODE_SWITZERLAND            29
#define HID_COUNTRY_CODE_TAIWAN                 30
#define HID_COUNTRY_CODE_TURKISH_Q              31
#define HID_COUNTRY_CODE_UK                     32
#define HID_COUNTRY_CODE_US                     33
#define HID_COUNTRY_CODE_YUGOSLAVIA             34
#define HID_COUNTRY_CODE_TURKISH_F              35
//! @}

//! \name HID KEYS values
//! @{
#define  HID_A                4
#define  HID_B                5
#define  HID_C                6
#define  HID_D                7
#define  HID_E                8
#define  HID_F                9
#define  HID_G                10
#define  HID_H                11
#define  HID_I                12
#define  HID_J                13
#define  HID_K                14
#define  HID_L                15
#define  HID_M                16
#define  HID_N                17
#define  HID_O                18
#define  HID_P                19
#define  HID_Q                20
#define  HID_R                21
#define  HID_S                22
#define  HID_T                23
#define  HID_U                24
#define  HID_V                25
#define  HID_W                26
#define  HID_X                27
#define  HID_Y                28
#define  HID_Z                29
#define  HID_1                30
#define  HID_2                31
#define  HID_3                32
#define  HID_4                33
#define  HID_5                34
#define  HID_6                35
#define  HID_7                36
#define  HID_8                37
#define  HID_9                38
#define  HID_0                39
#define  HID_ENTER            40
#define  HID_ESCAPE           41
#define  HID_BACKSPACE        42
#define  HID_TAB              43
#define  HID_SPACEBAR         44
#define  HID_UNDERSCORE       45
#define  HID_PLUS             46
/*
#define  HID_[ {              47
#define  HID_] }              48
*/
#define  HID_BACKSLASH        49
/*
#define  HID_# ~              50
#define  HID_; :              51
#define  HID_` "              52
*/
#define  HID_TILDE            53
#define  HID_COMMA            54
#define  HID_DOT              55
#define  HID_SLASH            56
#define  HID_CAPS LOCK        57
#define  HID_F1               58
#define  HID_F2               59
#define  HID_F3               60
#define  HID_F4               61
#define  HID_F5               62
#define  HID_F6               63
#define  HID_F7               64
#define  HID_F8               65
#define  HID_F9               66
#define  HID_F10              67
#define  HID_F11              68
#define  HID_F12              69
#define  HID_PRINTSCREEN      70
#define  HID_SCROLL LOCK      71
#define  HID_PAUSE            72
#define  HID_INSERT           73
#define  HID_HOME             74
#define  HID_PAGEUP           75
#define  HID_DELETE           76
#define  HID_END              77
#define  HID_PAGEDOWN         78
#define  HID_RIGHT            79
#define  HID_LEFT             80
#define  HID_DOWN             81
#define  HID_UP               82
#define  HID_KEYPAD_NUM_LOCK  83
#define  HID_KEYPAD_DIVIDE    84
#define  HID_KEYPAD_AT        85
#define  HID_KEYPAD_MULTIPLY  85
#define  HID_KEYPAD_MINUS     86
#define  HID_KEYPAD_PLUS      87
#define  HID_KEYPAD_ENTER     88
#define  HID_KEYPAD_1         89
#define  HID_KEYPAD_2         90
#define  HID_KEYPAD_3         91
#define  HID_KEYPAD_4         92
#define  HID_KEYPAD_5         93
#define  HID_KEYPAD_6         94
#define  HID_KEYPAD_7         95
#define  HID_KEYPAD_8         96
#define  HID_KEYPAD_9         97
#define  HID_KEYPAD_0         98

   //! \name HID modifier values
   //! @{
#define  HID_MODIFIER_NONE          0x00
#define  HID_MODIFIER_LEFT_CTRL     0x01
#define  HID_MODIFIER_LEFT_SHIFT    0x02
#define  HID_MODIFIER_LEFT_ALT      0x04
#define  HID_MODIFIER_LEFT_GUI      0x08
#define  HID_MODIFIER_RIGHT_CTRL    0x10
#define  HID_MODIFIER_RIGHT_SHIFT   0x20
#define  HID_MODIFIER_RIGHT_ALT     0x40
#define  HID_MODIFIER_RIGHT_GUI     0x80
   //! @}
//! @}

/*! \name HID Requests
 */
//! @{
#define HID_GET_REPORT                          0x01
#define HID_GET_IDLE                            0x02
#define HID_GET_PROTOCOL                        0x03
#define HID_SET_REPORT                          0x09
#define HID_SET_IDLE                            0x0A
#define HID_SET_PROTOCOL                        0x0B
//! @}

/*! \name HID Report Types
 */
//! @{
#define HID_REPORT_INPUT                        0x01
#define HID_REPORT_OUTPUT                       0x02
#define HID_REPORT_FEATURE                      0x03
//! @}

/*! \name Special HID Report IDs
 */
//! @{
#define HID_REPORT_ID_ALL                       0x00
//! @}

/*! \name Special HID Idle Durations
 */
//! @{
#define HID_IDLE_DURATION_INDEFINITE            0x00
//! @}

/*! \name HID Protocols
 */
//! @{
#define HID_BOOT_PROTOCOL                       0x00
#define HID_REPORT_PROTOCOL                     0x01
//! @}

/*! \name HID Report Descriptor Item Types
 */
//! @{
#define HID_ITEM_TYPE_MAIN                      0x00
#define HID_ITEM_TYPE_GLOBAL                    0x01
#define HID_ITEM_TYPE_LOCAL                     0x02
#define HID_ITEM_TYPE_RESERVED                  0x03
//! @}

/*! \name Special HID Report Descriptor Item Tags
 */
//! @{
#define HID_ITEM_TAG_LONG_ITEM                  0x0F
//! @}

/*! \name HID Report Descriptor Main Item Tags
 */
//! @{
#define HID_MAIN_ITEM_TAG_INPUT                 0x08
#define HID_MAIN_ITEM_TAG_OUTPUT                0x09
#define HID_MAIN_ITEM_TAG_FEATURE               0x0B
#define HID_MAIN_ITEM_TAG_COLLECTION            0x0A
#define HID_MAIN_ITEM_TAG_END_COLLECTION        0x0C
//! @}

/*! \name HID Report Descriptor Global Item Tags
 */
//! @{
#define HID_GLOBAL_ITEM_TAG_USAGE_PAGE          0x00
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM     0x01
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM     0x02
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MINIMUM    0x03
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MAXIMUM    0x04
#define HID_GLOBAL_ITEM_TAG_UNIT_EXPONENT       0x05
#define HID_GLOBAL_ITEM_TAG_UNIT                0x06
#define HID_GLOBAL_ITEM_TAG_REPORT_SIZE         0x07
#define HID_GLOBAL_ITEM_TAG_REPORT_ID           0x08
#define HID_GLOBAL_ITEM_TAG_REPORT_COUNT        0x09
#define HID_GLOBAL_ITEM_TAG_PUSH                0x0A
#define HID_GLOBAL_ITEM_TAG_POP                 0x0B
//! @}

/*! \name HID Report Descriptor Local Item Tags
 */
//! @{
#define HID_LOCAL_ITEM_TAG_USAGE                0x00
#define HID_LOCAL_ITEM_TAG_USAGE_MINIMUM        0x01
#define HID_LOCAL_ITEM_TAG_USAGE_MAXIMUM        0x02
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_INDEX     0x03
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MINIMUM   0x04
#define HID_LOCAL_ITEM_TAG_DESIGNATOR_MAXIMUM   0x05
#define HID_LOCAL_ITEM_TAG_STRING_INDEX         0x07
#define HID_LOCAL_ITEM_TAG_STRING_MINIMUM       0x08
#define HID_LOCAL_ITEM_TAG_STRING_MAXIMUM       0x09
#define HID_LOCAL_ITEM_TAG_DELIMITER            0x0A
//! @}


//_____ D E F I N I T I O N S __________________________________________________

//! HID descriptor.
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
struct
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
  unsigned char bLength;
  unsigned char bDescriptorType;
  unsigned short bcdHID;
  unsigned char bCountryCode;
  unsigned char bNumDescriptors;
  struct
#if (defined __GNUC__)
  __attribute__((__packed__))
#endif
  {
    unsigned char bType;
    unsigned short wLength;
  } Descriptor[];
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
hid_descriptor_t;

//! HID report ID.
typedef unsigned char hid_report_id_t;

//! HID report descriptor item.
typedef
#if (defined __ICCAVR32__)
#pragma pack(1)
#endif
union
#if (defined __GNUC__)
__attribute__((__packed__))
#endif
{
  struct
#if (defined __GNUC__)
  __attribute__((__packed__))
#endif
  {
    unsigned char bTag                          : 4;
    unsigned char bType                         : 2;
    unsigned char bSize                         : 2;
  } header;
  struct
#if (defined __GNUC__)
  __attribute__((__packed__))
#endif
  {
    unsigned char bTag                          : 4;
    unsigned char bType                         : 2;
    unsigned char bSize                         : 2;
    unsigned char data[];
  } short_format;
  struct
#if (defined __GNUC__)
  __attribute__((__packed__))
#endif
  {
    unsigned char bTag                          : 4;
    unsigned char bType                         : 2;
    unsigned char bSize                         : 2;
    unsigned char bDataSize;
    unsigned char bLongItemTag;
    unsigned char data[];
  } long_format;
}
#if (defined __ICCAVR32__)
#pragma pack()
#endif
hid_item_t;

//! HID report descriptor short item data.
typedef union
{
  unsigned long value;
  union
  {
    struct
    {
      unsigned int                              : 23;
      unsigned int buffered_bytes               : 1;
      unsigned int                              : 1;
      unsigned int null_state                   : 1;
      unsigned int no_preferred                 : 1;
      unsigned int non_linear                   : 1;
      unsigned int wrap                         : 1;
      unsigned int relative                     : 1;
      unsigned int variable                     : 1;
      unsigned int constant                     : 1;
    } input;
    struct
    {
      unsigned int                              : 23;
      unsigned int buffered_bytes               : 1;
      unsigned int volatile_                    : 1;
      unsigned int null_state                   : 1;
      unsigned int no_preferred                 : 1;
      unsigned int non_linear                   : 1;
      unsigned int wrap                         : 1;
      unsigned int relative                     : 1;
      unsigned int variable                     : 1;
      unsigned int constant                     : 1;
    } output;
    struct
    {
      unsigned int                              : 23;
      unsigned int buffered_bytes               : 1;
      unsigned int volatile_                    : 1;
      unsigned int null_state                   : 1;
      unsigned int no_preferred                 : 1;
      unsigned int non_linear                   : 1;
      unsigned int wrap                         : 1;
      unsigned int relative                     : 1;
      unsigned int variable                     : 1;
      unsigned int constant                     : 1;
    } feature;
    enum
    {
      HID_COLLECTION_PHYSICAL                   = 0x00,
      HID_COLLECTION_APPLICATION                = 0x01,
      HID_COLLECTION_LOGICAL                    = 0x02,
      HID_COLLECTION_REPORT                     = 0x03,
      HID_COLLECTION_NAMED_ARRAY                = 0x04,
      HID_COLLECTION_USAGE_SWITCH               = 0x05,
      HID_COLLECTION_USAGE_MODIFIER             = 0x06
    } collection;
  } main;
  union
  {
    unsigned int usage_page;
    int logical_minimum;
    int logical_maximum;
    int physical_minimum;
    int physical_maximum;
    struct
    {
      unsigned int                              : 28;
      int unit_exponent                         : 4;
    };
    struct
    {
      unsigned int                              : 4;
      enum
      {
        HID_UNIT_LUMINOUS_INTENSITY_NONE        = 0x0,
        HID_UNIT_LUMINOUS_INTENSITY_CANDELA     = 0x1
      } luminous_intensity                      : 4;
      enum
      {
        HID_UNIT_CURRENT_NONE                   = 0x0,
        HID_UNIT_CURRENT_AMPERE                 = 0x1
      } current                                 : 4;
      enum
      {
        HID_UNIT_TEMPERATURE_NONE               = 0x0,
        HID_UNIT_TEMPERATURE_KELVIN             = 0x1,
        HID_UNIT_TEMPERATURE_FAHRENHEIT         = 0x3
      } temperature                             : 4;
      enum
      {
        HID_UNIT_TIME_NONE                      = 0x0,
        HID_UNIT_TIME_SECONDS                   = 0x1
      } time                                    : 4;
      enum
      {
        HID_UNIT_MASS_NONE                      = 0x0,
        HID_UNIT_MASS_GRAM                      = 0x1,
        HID_UNIT_MASS_SLUG                      = 0x3
      } mass                                    : 4;
      enum
      {
        HID_UNIT_LENGTH_NONE                    = 0x0,
        HID_UNIT_LENGTH_CENTIMETER              = 0x1,
        HID_UNIT_LENGTH_RADIANS                 = 0x2,
        HID_UNIT_LENGTH_INCH                    = 0x3,
        HID_UNIT_LENGTH_DEGREES                 = 0x4
      } length                                  : 4;
      enum
      {
        HID_UNIT_SYSTEM_NONE                    = 0x0,
        HID_UNIT_SYSTEM_SI_LINEAR               = 0x1,
        HID_UNIT_SYSTEM_SI_ROTATION             = 0x2,
        HID_UNIT_SYSTEM_ENGLISH_LINEAR          = 0x3,
        HID_UNIT_SYSTEM_ENGLISH_ROTATION        = 0x4
      } system                                  : 4;
    } unit;
    unsigned int report_size;
    unsigned int report_id;
    unsigned int report_count;
  } global;
  union
  {
    unsigned int usage;
    unsigned int usage_minimum;
    unsigned int usage_maximum;
    unsigned int designator_index;
    unsigned int designator_minimum;
    unsigned int designator_maximum;
    unsigned int string_index;
    unsigned int string_minimum;
    unsigned int string_maximum;
    enum
    {
      HID_DELIMITER_OPEN_SET                    = 0x01,
      HID_DELIMITER_CLOSE_SET                   = 0x00
    } delimiter;
  } local;
} hid_short_item_data_t;


//_____ D E C L A R A T I O N S ________________________________________________


#endif  // _HID_H_
