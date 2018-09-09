/**
* \file  D_Nv.h
*
* \brief NV component header
*
*  The segment D_NV_MEMORY must be defined and reserved in the linker script
*  (.xcl file), for example like this:
*
*    -D_D_NV_MEMORY_START=400
*    -D_D_NV_MEMORY_SIZE=4000
*
*    -Z(CODE)D_NV_MEMORY+_D_NV_MEMORY_SIZE=[_D_NV_MEMORY_START:+_D_NV_MEMORY_SIZE]
*
*  Note that nothing should actually be placed in the segment.
*  These linker script commands will ensure that the entire range
*  is reserved and not programmed (left as 0xff).
*
*  The segment should start on a flash page boundary, and the
*  size should correspond to the define D_NV_MEMORY_SIZE.
*
*  Further, since the device can only erase a whole page and not
*  arbitrary locations in flash, the sector size _must_ be an
*  integer multiple of the flash page size.
*  Otherwise, erasing the first or last page of a sector will
*  also erase content in flash locations neighbouring the sector.

* Copyright (c) 2018 Microchip Technology Inc. and its subsidiaries. 
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
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, 
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

#include <compiler.h>

#ifndef D_NV_H
#define D_NV_H


typedef uint16_t D_Nv_Size_t;


#if !defined(D_NV_FIRST_SECTOR)
#  define D_NV_FIRST_SECTOR  0
#endif

#if !defined(D_NV_SECTOR_COUNT)
#  define D_NV_SECTOR_COUNT  2
#endif

// Sector size must be an integer multiple of flash pages
#if !defined(D_NV_SECTOR_SIZE)
#    define D_NV_SECTOR_SIZE  (8192U)
#endif

// Size of D_NV_MEMORY segment should equal this
#define D_NV_MEMORY_SIZE  (D_NV_SECTOR_COUNT * D_NV_SECTOR_SIZE)

#define  GET_FIELD_PTR(structPtr, typeName, fieldName) \
((uint8_t *)(structPtr) + offsetof(typeName, fieldName))

#define GET_PARENT_BY_FIELD(TYPE, FIELD, FIELD_POINTER)  \
((TYPE *)(((uint8_t *)FIELD_POINTER) - offsetof(TYPE, FIELD)))
#define GET_CONST_PARENT_BY_FIELD(TYPE, FIELD, FIELD_POINTER)  \
((const TYPE *)(((const uint8_t *)FIELD_POINTER) - offsetof(TYPE, FIELD)))

#define GET_STRUCT_BY_FIELD_POINTER(struct_type, field_name, field_pointer)\
((struct_type *) (((uint8_t *) field_pointer) - FIELD_OFFSET(struct_type, field_name)))
#define GET_INDEX_FROM_OFFSET(PTR1, PTR2)  (PTR1 - PTR2)

// Size of slice between firstField end lastField of struct (including lastField)
#define SLICE_SIZE(type, firstField, lastField)\
(offsetof(type, lastField) - offsetof(type, firstField) + sizeof(((type *)0)->lastField))

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

#define ADD_COMMA(value, n) value,
/* Macro for array initialization */
#define INIT_ARRAY(initValue, size) {REPEAT_MACRO(ADD_COMMA, initValue, size)}

#undef FIELD_OFFSET
#define FIELD_OFFSET(struct_type, field_name)\
(((uint8_t*) &((struct_type *)(NULL))->field_name) - (uint8_t*)NULL)

#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y) ((x)<(y)?(y):(x))

#define FIRST_BIT_SET(bitmask)     ((bitmask) & (~(bitmask) + 1))
#define FIRST_BIT_CLEARED(bitmask) (~(bitmask) & ((bitmask) + 1))

#define COMPARE_WITH_THRESHOLD(a, b, threshold) \
(abs((a) - (b)) < (threshold) ? ((a) > (b) ? 1 : 0) : ((a) > (b) ? 0 : 1))

#define CEIL(a, b) (((a) - 1U)/(b) + 1U)

#ifndef FLASH_PTR
  #define FLASH_PTR  
#endif

#ifdef __cplusplus
extern "C" {
#endif

void D_Nv_Read(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes);
void D_Nv_Write(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes);
void D_Nv_EraseSector(uint8_t sector);
bool D_Nv_IsEmpty(uint8_t sector, uint16_t offset, D_Nv_Size_t numberOfBytes);
bool D_Nv_IsEqual(uint8_t sector, uint16_t offset, uint8_t *pBuffer, D_Nv_Size_t numberOfBytes);
void D_Nv_SetSystemIntegrityCheckFunction(void (*pf)(void));

#ifdef __cplusplus
}
#endif

#endif // D_NV_H
