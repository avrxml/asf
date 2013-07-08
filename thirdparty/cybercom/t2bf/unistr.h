/*
*******************************************************************************
*                        (c) auSystems 2007                                   *
*******************************************************************************

   File name:     unistr.h
   Author:        Jonas Holgersson
   Revision:      D
   Description:   Unicode string handler


   REVISION HISTORY

   Rev   Date     Author   Description
   ------------------------------------
   PA1   070307   JOHO     Initial version.
   A     070402   MJOE     Release version.
   PB1   070413   THBR     Removed dependency tosal.
         070820   MJOE     Added GSM character set decoding.
   B     070829   MJOE     Release version.
   PC1   071011   JOHO     Added UNISTRToUtf8.
   C     071012   JOHO     Release version.
   PD1   071113   JOHO     Updated UNISTRQuotedPrintableGetNext to handle
                           end of string.
   D     071119   JOHO     Release version.

*******************************************************************************
*/

#ifndef _UNISTR_H
#define _UNISTR_H

#include "btypes.h"

/*
-------------------------
   Definitions
-------------------------
*/

// Settings for collation table
#define UNISTR_COLLATION_BASIC_LATIN
#define UNISTR_COLLATION_LATIN1_SUPPLEMENT
#define UNISTR_COLLATION_LATIN_EXTENDED_A
#define UNISTR_COLLATION_LATIN_EXTENDED_B
#define UNISTR_COLLATION_GREEK
#define UNISTR_COLLATION_CYRILLIC

#define UNISTR_PANIC_LENGTH     512

#define UNISTR_CHARSET_ASCII        "ASCII"
#define UNISTR_CHARSET_UTF8         "UTF-8"
#define UNISTR_CHARSET_UTF7         "UTF-7"
#define UNISTR_CHARSET_UCS2         "UCS2"
#define UNISTR_CHARSET_UTF16        "UTF-16"
#define UNISTR_CHARSET_ISO_8859_1   "ISO-8859-1"
#define UNISTR_CHARSET_GSM          "GSM"

typedef uint16* TUNISTRUtf16String;
typedef uint32  TUNISTRChar; // Unicode character

typedef enum
{
   UNISTR_ENCODING_NONE                 = 0x00,
   UNISTR_ENCODING_QUOTED_PRINTABLE     = 0x81,
   UNISTR_ENCODING_BASE64               = 0x82,
   UNISTR_ENCODING_8BIT                 = 0x83,

   UNISTR_ENCODING_HEX_IN_ASCII         = 0xC0,  // Hexadecimal values encoded with ASCII digits
   UNISTR_ENCODING_MSB_WORD             = 0xC1
} TUNISTREncodingEnum;
typedef uint8 TUNISTREncoding;

/*
   Old conversion options                    Corresponding option in new module
                                             Charset                    Encoding
   UCS2_CONV_ASCII                           UNISTR_CHARSET_ASCII       UNISTR_ENCODING_NONE
   UCS2_CONV_ASCII_NBO                       Handled by separate function since this is another output format
   UCS2_CONV_UTF8                            UNISTR_CHARSET_UTF8        UNISTR_ENCODING_NONE
   UCS2_CONV_UTF8_QUOTED_PRINTABLE           UNISTR_CHARSET_UTF8        UNISTR_ENCODING_QUOTED_PRINTABLE
   UCS2_CONV_UNICODE                         UNISTR_CHARSET_UTF16       UNISTR_ENCODING_HEX_IN_ASCII
   UCS2_CONV_QUOTED_PRINTABLE                UNISTR_CHARSET_ASCII       UNISTR_ENCODING_QUOTED_PRINTABLE
   UCS2_CONV_UTF7                            UNISTR_CHARSET_UTF7        UNISTR_ENCODING_NONE
   UCS2_CONV_UCS2_MSB,  // Input string is in UCS2 format, most significant byte first
                                             UNISTR_CHARSET_UTF16       UNISTR_ENCODING_MSB_WORD
*/

/******************************************************************************
   Callback TUNISTRConvUtf16Cb
******************************************************************************/
typedef TBStatus TUNISTRConvUtf16Cb( TString charset, TUNISTREncoding encoding,
                                     uint8* pFrom, uint16 toSize, TUNISTRUtf16String to );

/*
-------------------------
   Public functions
-------------------------
*/

#if defined( __cplusplus )
extern "C" {
#endif

/*******************************************************************
   Function UNISTRInit
   Input:         ---
   Output:        ---
   Return value:  ---
   Description:   Inits the unicode string handler
********************************************************************/
void UNISTRInit( void );

/*******************************************************************
   Function UNISTRRegisterStrConv
   Input:         pStrConvCb  Pointer to string converter function.
   Output:        ---
   Return value:  ---
   Description:   Registers a string conversion function. This
                  function can be used by the unistr module to convert
                  strings to UTF-16 format.
********************************************************************/
void UNISTRRegisterStrConv( TUNISTRConvUtf16Cb* pStrConvCb );

/*******************************************************************
   Function UNISTRStrClr
   Input:         ---
   Output:        ---
   Return value:  Length of the string, not including the terminal
                  character.
   Description:   Clears a string.
********************************************************************/
void UNISTRStrClr( TUNISTRUtf16String str );

/*******************************************************************
   Function UNISTRStrLen
   Input:         ---
   Output:        ---
   Return value:  Length of the string, not including the terminal
                  character.
   Description:   Measures length of a string.
********************************************************************/
uint16 UNISTRStrLen( const TUNISTRUtf16String str );

/*******************************************************************
   Function UNISTRStrCpy
   Input:         pTo         String to copy to.
                  pFrom       String to copy from. Must be NULL terminated.
                  size        Size of To buffer.
   Output:        ---
   Return value:  ---
   Description:   Copies a string to another string, with max size
                  'size'. New string is always NULL terminated.
********************************************************************/
void UNISTRStrCpy( TUNISTRUtf16String pTo,
                   const TUNISTRUtf16String pFrom,
                   uint16 size );

/*******************************************************************
   Function UNISTRStrCmp
   Input:         s1       String 1
                  s2       String 2
   Output:        ---
   Return value:  <0 if s1 is less than s2
                  0  if s1 and s2 are equal
                  >0 if s1 is greater than s2
   Description:   Compares the two strings lexiographically.
********************************************************************/
int8 UNISTRStrCmp( const TUNISTRUtf16String s1,
                   const TUNISTRUtf16String s2 );

/*******************************************************************
   Function UNISTRStrAppend
   Input:         pTo         String to append to.
                  pFrom       String to copy from. Must be NULL terminated.
                  size        Size of To buffer.
   Output:        ---
   Return value:  ---
   Description:   Appends a string to another string, with max size
                  'size'. New string is always NULL terminated.
********************************************************************/
void UNISTRStrAppend( TUNISTRUtf16String pTo,
                      const TUNISTRUtf16String pFrom,
                      uint16 size );

/*******************************************************************
   Function UNISTRToUtf16
   Input:         charset     ASCII string describing the charset
                              of the input string.
                  encoding    Encoding of the input string.
                  pFrom       String to convert.
                  pTo         String buffer to convert to.
                  size        Size of To buffer.
   Output:        ---
   Return value:  TRUE if conversion was successful.
                  FALSE otherwise. A conversion can fail if the
				  charset of the input string is not supported.
   Description:   Converts a string to UTF-16. The new string is
                  written to the pTo buffer. The new string is
				  always NULL terminated.
********************************************************************/
boolean UNISTRToUtf16( TString charset, TUNISTREncoding encoding,
					   uint8* pFrom, TUNISTRUtf16String pTo, uint16 size );

/*******************************************************************
   Function UNISTRToUtf16Nbo
   Input:         charset     ASCII string describing the charset
                              of the input string.
                  encoding    Encoding of the input string.
                  pFrom       String to convert.
                  pTo         String buffer to convert to.
                  size        Size of To buffer.
   Output:        ---
   Return value:  TRUE if conversion was successful.
                  FALSE otherwise. A conversion can fail if the
				  charset or encoding of the input string is not
                  supported.
   Description:   Converts a string to UTF-16 network byte order. The
                  new string is written to the pTo buffer. The new
                  string is always NULL terminated.
********************************************************************/
boolean UNISTRToUtf16Nbo( TString charset, TUNISTREncoding encoding,
					      uint8* pFrom, TUNISTRUtf16String pTo, uint16 size );

/*******************************************************************
   Function UNISTRToUtf8
   Input:         charset     ASCII string describing the charset
                              of the input string.
                  encoding    Encoding of the input string.
                  pFrom       String to convert.
                  pTo         String buffer to convert to.
                  size        Size of To buffer.
   Output:        ---
   Return value:  TRUE if conversion was successful.
                  FALSE otherwise. A conversion can fail if the
				      charset or encoding of the input string is not
                  supported.
   Description:   Converts a string to UTF-8 format. The
                  new string is written to the pTo buffer. The new
                  string is always NULL terminated.
********************************************************************/
boolean UNISTRToUtf8( TString charset, TUNISTREncoding encoding,
					      uint8* pFrom, uint8* pTo, uint16 size );

/*******************************************************************
   Function UNISTRGetNextChar
   Input:         str       String to get next character from.
   Output:        pUnicode  Unicode character which where
                            read from the string.
                  pOffset   Number of 16-bit words that where
                            used in the string to produce the
                            unicode character.
   Return value:  Boolean indicating if a valid character could be
                  read from the string.
   Description:   Reads out the first character from the string. Also
                  the NULL character will be read and returned in
                  pUnicode.
********************************************************************/
boolean UNISTRGetNextChar( TUNISTRUtf16String str,
                           TUNISTRChar*       pUnicode,
                           uint8*             pOffset );

/*******************************************************************
   Function UNISTRGetWeight
   Input:         unicode     Unicode character
   Return value:  Weight of this character.
   Description:   Gets collation weight for a specified character.
********************************************************************/
uint32 UNISTRGetWeight( TUNISTRChar unicode );

#if defined( __cplusplus )
}
#endif

#endif
