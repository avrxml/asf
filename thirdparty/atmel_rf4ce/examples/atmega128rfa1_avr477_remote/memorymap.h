/*============================================================================*/

#ifndef MEMORY_H
#define MEMORY_H

/*----------------------------------------------------------------------------
  nested include files
----------------------------------------------------------------------------*/
#if defined(__ICCAVR__)
#include <ioavr.h>
#include <intrinsics.h>              
#include "inavr.h"
#elif defined(__GNUC__)
#include<avr/io.h>
#include<avr/interrupt.h>
#endif

#include <stdint.h>

/*----------------------------------------------------------------------------
  manifest constants
----------------------------------------------------------------------------*/

/* Memory Map */


/* Identification Info */
//! @ingroup RApp-API
//! @{
#define CHIPID    0x55u /* HP */
#define SWVER     0xAAu /* Revision x.x */

#define NUM_STATUS_BYTES   5u
#define FIRST_STATUS_BYTE  2u
#define NO_MATCH           0xFF
#define MATCH              0x00

/*----------------------------------------------------------------------------
  type definitions
----------------------------------------------------------------------------*/

/* definition of memory map body */
typedef struct tag_memory_map_body_t
{
   uint8_t id;                            /* Address  0 - hard-coded chip id */
   uint8_t major_minor;                   /* Address  1 - hard-coded chip version */
   uint8_t sensor_status[2];                 /* Address  3-4 - Key status */
   uint8_t wheel_position;                /* Address  5 - Wheel Position */
   uint8_t prox_status;                   /* Address  6 - Proximity Sensor Status */
   uint8_t accelero_status;                /* Address  7 - Acclerometer Status */
} memory_map_body_t;

/* union allowing memory map to be addressed by field name, or as an array */
typedef union tag_memory_map_t
{
   memory_map_body_t body;
   uint8_t           array[sizeof( memory_map_body_t )];

} memory_map_t;
//! @}
/*----------------------------------------------------------------------------
  macros
----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  extern variables
----------------------------------------------------------------------------*/

extern memory_map_t memory_map;  /* application memory map */
extern uint8_t sent_status_out[NUM_STATUS_BYTES];

extern uint8_t comms_match(void);
#endif /* MEMORY_H */

/*----------------------------------------------------------------------------
  End of file $RCSfile: memory_map.h $
----------------------------------------------------------------------------*/
