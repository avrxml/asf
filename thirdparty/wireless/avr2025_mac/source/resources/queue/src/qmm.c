/**
 * @file qmm.c
 *
 * @brief This file implements the  functions for initializing the queues,
 *  appending a buffer into the queue, removing a buffer from the queue and
 *  reading a buffer from the queue as per the search criteria.
 *
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
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
 */

/*
 * Copyright (c) 2013-2015 Atmel Corporation. All rights reserved.
 *
 * Licensed under Atmel's Limited License Agreement --> EULA.txt
 */
/* === Includes ============================================================ */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "pal.h"
#include "return_val.h"
#include "bmm.h"
#include "qmm.h"
#include "app_config.h"

#if (TOTAL_NUMBER_OF_BUFS > 0)

/* === Types =============================================================== */

/*
 * Specifies whether the buffer needs to be read from the queue or to be
 * removed from the queue.
 */
typedef enum buffer_mode_tag {
	REMOVE_MODE,
	READ_MODE
} buffer_mode_t;

/* === Macros ============================================================== */

/* === Prototypes ========================================================== */

static buffer_t *queue_read_or_remove(queue_t *q,
		buffer_mode_t mode,
		search_t *search);

/* === Implementation ====================================================== */

/**
 * @brief Initializes the queue.
 *
 * This function initializes the queue. Note that this function
 * should be called before invoking any other functionality of QMM.
 *
 * @param q The queue which should be initialized.
 */
#ifdef ENABLE_QUEUE_CAPACITY
void qmm_queue_init(queue_t *q, uint8_t capacity)
#else
void qmm_queue_init(queue_t *q)
#endif  /* ENABLE_QUEUE_CAPACITY */
{
	q->head = NULL;
	q->tail = NULL;
	q->size = 0;
#ifdef ENABLE_QUEUE_CAPACITY
	q->capacity = capacity;
#endif  /* ENABLE_QUEUE_CAPACITY */
}

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
 */
#ifdef ENABLE_QUEUE_CAPACITY
retval_t qmm_queue_append(queue_t *q, buffer_t *buf)
#else
void qmm_queue_append(queue_t *q, buffer_t *buf)
#endif  /* ENABLE_QUEUE_CAPACITY */
{
#ifdef ENABLE_QUEUE_CAPACITY
	retval_t status;
#endif  /* ENABLE_QUEUE_CAPACITY */

	ENTER_CRITICAL_REGION();

#ifdef ENABLE_QUEUE_CAPACITY
	/* Check if queue is full */
	if (q->size == q->capacity) {
		/* Buffer cannot be appended as queue is full */
		status = QUEUE_FULL;
	} else
#endif  /* ENABLE_QUEUE_CAPACITY */
	{
		/* Check whether queue is empty */
		if (q->size == 0) {
			/* Add the buffer at the head */
			q->head = buf;
		} else {
			/* Add the buffer at the end */
			q->tail->next = buf;
		}

		/* Update the list */
		q->tail = buf;

		/* Terminate the list */
		buf->next = NULL;

		/* Update size */
		q->size++;

#if (_DEBUG_ > 1)
		if (q->head == NULL) {
			Assert("Corrupted queue: Null pointer has been queued");
		}
#endif

#ifdef ENABLE_QUEUE_CAPACITY
		status = MAC_SUCCESS;
#endif  /* ENABLE_QUEUE_CAPACITY */
	}

	LEAVE_CRITICAL_REGION();

#ifdef ENABLE_QUEUE_CAPACITY
	return (status);
#endif
} /* qmm_queue_append */

/*
 * @brief Reads or removes a buffer from queue
 *
 * This function reads or removes a buffer from a queue as per
 * the search criteria provided. If search criteria is NULL, then the first
 * buffer is returned, otherwise buffer matching the given criteria is returned
 *
 * @param q Queue from which buffer is to be read or removed.
 *
 * @param mode Mode of operations. If this parameter has value REMOVE_MODE,
 *             buffer will be removed from queue and returned. If this parameter
 * is
 *             READ_MODE, buffer pointer will be returned without
 *             removing from queue.
 *
 * @param search Search criteria structure pointer.
 *
 * @return Buffer header pointer, if the buffer is successfully
 *         removed or read, otherwise NULL is returned.
 * \ingroup group_qmm
 */
static buffer_t *queue_read_or_remove(queue_t *q,
		buffer_mode_t mode,
		search_t *search)
{
	buffer_t *buffer_current = NULL;
	buffer_t *buffer_previous;

	ENTER_CRITICAL_REGION();
	/* Check whether queue is empty */
	if (q->size != 0) {
		buffer_current = q->head;
		buffer_previous = q->head;

		/* First get buffer matching with criteria */
		if (NULL != search) {
			uint8_t match;
			/* Search for all buffers in the queue */
			while (NULL != buffer_current) {
				match = search->criteria_func(
						(void *)buffer_current->body,
						search->handle);

				if (match) {
					/* Break, if search criteria matches */
					break;
				}

				buffer_previous = buffer_current;
				buffer_current = buffer_current->next;
			}
		}

		/* Buffer matching with search criteria found */
		if (NULL != buffer_current) {
			/* Remove buffer from the queue */
			if (REMOVE_MODE == mode) {
				/* Update head if buffer removed is first node
				**/
				if (buffer_current == q->head) {
					q->head = buffer_current->next;
				} else {
					/* Update the link by removing the
					 * buffer */
					buffer_previous->next
						= buffer_current->next;
				}

				/* Update tail if buffer removed is last node */
				if (buffer_current == q->tail) {
					q->tail = buffer_previous;
				}

				/* Update size */
				q->size--;

				if (NULL == q->head) {
					q->tail = NULL;
				}
			}
			/* Read buffer from the queue */
			else {
				/* Nothing needs done if the mode is READ_MODE
				**/
			}
		}
	} /* q->size != 0 */

	LEAVE_CRITICAL_REGION();

	/* Return the buffer. note that pointer to header of buffer is returned
	**/
	return (buffer_current);
} /* queue_read_or_remove */

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
 */
buffer_t *qmm_queue_remove(queue_t *q, search_t *search)
{
	return (queue_read_or_remove(q, REMOVE_MODE, search));
}

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
 */
buffer_t *qmm_queue_read(queue_t *q, search_t *search)
{
	return (queue_read_or_remove(q, READ_MODE, search));
}

/**
 * @brief Internal function for flushing a specific queue
 *
 * @param q Queue to be flushed
 */
void qmm_queue_flush(queue_t *q)
{
	buffer_t *buf_to_free;

	while (q->size > 0) {
		/* Remove the buffer from the queue and free it */
		buf_to_free = qmm_queue_remove(q, NULL);

		if (NULL == buf_to_free) {
#if (_DEBUG_ > 0)
			ABORT("Corrupted queue");
#endif
			q->size = 0;
			return;
		}

		bmm_buffer_free(buf_to_free);
	}
}

#endif  /* (TOTAL_NUMBER_OF_BUFS > 0) */

/* EOF */
