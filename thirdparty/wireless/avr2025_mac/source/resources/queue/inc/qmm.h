/**
 * @file qmm.h
 *
 * @brief This file contains the Queue Management Module definitions.
 *
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
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

/*
 * Copyright (c) 2013-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */

/* Prevent double inclusion */
#ifndef QMM_INTERFACE_H
#define QMM_INTERFACE_H

/* === Includes ============================================================ */

#include "return_val.h"
#include "bmm.h"

/**
 * \ingroup group_resources
 * \defgroup group_qmm  Queue Management
 * Queue Management: provides services for creating and maintaining the queues.
 *  @{
 */

/* === Macros ============================================================== */

/* === Types =============================================================== */

/**
 * @brief Structure to search for a buffer to be removed from a queue
 */
__PACK__DATA__
typedef struct
#if !defined(__DOXYGEN__)
		search_tag
#endif
{
	/** Pointer to search criteria function */
	uint8_t (*criteria_func)(void *buf, void *handle);
	/** Handle to callbck parameter */
	void *handle;
} search_t;

/**
 * @brief Queue structure
 *
 * This structur defines the queue structure.
 * The application should declare the queue of type queue_t
 * and call qmm_queue_init before invoking any other functionality of qmm.
 *
 * @ingroup apiMacTypes
 */
typedef struct
#if !defined(DOXYGEN)
		queue_tag
#endif
{
	/** Pointer to head of queue */
	buffer_t *head;
	/** Pointer to tail of queue */
	buffer_t *tail;
#ifdef ENABLE_QUEUE_CAPACITY

	/**
	 * Maximum number of buffers that can be accomodated in the current
	 * queue
	 * Note: This is only required if the queue capacity shall be different
	 * from 255.
	 */
	uint8_t capacity;
#endif  /* ENABLE_QUEUE_CAPACITY */

	/**
	 * Number of buffers present in the current queue
	 */
	uint8_t size;
} queue_t;
__PACK__RST_DATA__
/* === Externals =========================================================== */

/* === Prototypes ========================================================== */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief Initializes the queue.
 *
 * This function initializes the queue. Note that this function
 * should be called before invoking any other functionality of QMM.
 *
 * @param q The queue which should be initialized.
 *
 */
#ifdef ENABLE_QUEUE_CAPACITY
void qmm_queue_init(queue_t *q, uint8_t capacity);

#else
void qmm_queue_init(queue_t *q);

#endif  /* ENABLE_QUEUE_CAPACITY */

/**
 * @brief Appends a buffer into the queue.
 *
 * This function appends a buffer into the queue.
 *
 * @param q Queue into which buffer should be appended
 *
 * @param buf Pointer to the buffer that should be appended into the queue.
 * Note that this pointer should be same as the
 * pointer returned by bmm_buffer_alloc.
 *
 */
#ifdef ENABLE_QUEUE_CAPACITY
retval_t qmm_queue_append(queue_t *q, buffer_t *buf);

#else
void qmm_queue_append(queue_t *q, buffer_t *buf);

#endif  /* ENABLE_QUEUE_CAPACITY */

/**
 * @brief Removes a buffer from queue.
 *
 * This function removes a buffer from queue
 *
 * @param q Queue from which buffer should be removed
 *
 * @param search Search criteria. If this parameter is NULL, first buffer in the
 * queue will be removed. Otherwise buffer matching the criteria will be
 * removed.
 *
 * @return Pointer to the buffer header, if the buffer is
 * successfully removed, NULL otherwise.
 *
 */
buffer_t *qmm_queue_remove(queue_t *q, search_t *search);

/**
 * @brief Reads a buffer from queue.
 *
 * This function reads either the first buffer if search is NULL or buffer
 * matching the given criteria from queue.
 *
 * @param q The queue from which buffer should be read.
 *
 * @param search If this parameter is NULL first buffer in the queue will be
 * read. Otherwise buffer matching the criteria will be read
 *
 * @return Pointer to the buffer header which is to be read, NULL if the buffer
 * is not available
 *
 */
buffer_t *qmm_queue_read(queue_t *q, search_t *search);

/**
 * @brief Internal function for flushing a specific queue
 *
 * @param q Queue to be flushed
 *
 */
void qmm_queue_flush(queue_t *q);

#ifdef __cplusplus
} /* extern "C" */
#endif

/* ! @} */
#endif /* QMM_INTERFACE_H */

/* EOF */
