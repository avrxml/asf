/**
* \file  miqueue.c
*
* \brief Queue handling for MiWi Protocol implementation
*
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

/* === Includes ============================================================ */

#include <stdlib.h>
#include "miqueue.h"
#include "mimem.h"

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

static miQueueBuffer_t *miQueueReadOrRemove(MiQueue_t *q,
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
void miQueueInit(MiQueue_t *q, uint8_t capacity)
#else
void miQueueInit(MiQueue_t *q)
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
 * 
 */
#ifdef ENABLE_QUEUE_CAPACITY
bool miQueueAppend(MiQueue_t *q, miQueueBuffer_t *buf)
#else
void miQueueAppend(MiQueue_t *q, miQueueBuffer_t *buf)
#endif  /* ENABLE_QUEUE_CAPACITY */
{
#ifdef ENABLE_QUEUE_CAPACITY
	bool status;
#endif  /* ENABLE_QUEUE_CAPACITY */

	cpu_irq_disable();

#ifdef ENABLE_QUEUE_CAPACITY
	/* Check if queue is full */
	if (q->size == q->capacity) {
		/* Buffer cannot be appended as queue is full */
		//status = QUEUE_FULL;
		status = false;
	} else
#endif  /* ENABLE_QUEUE_CAPACITY */
	{
		/* Check whether queue is empty */
		if (q->size == 0) {
			/* Add the buffer at the head */
			q->head = buf;
		} else {
			/* Add the buffer at the end */
			q->tail->nextItem = buf;
		}

		/* Update the list */
		q->tail = buf;

		/* Terminate the list */
		buf->nextItem = NULL;

		/* Update size */
		q->size++;

#ifdef ENABLE_QUEUE_CAPACITY
		status = true;
#endif  /* ENABLE_QUEUE_CAPACITY */
	}

	cpu_irq_enable();

#ifdef ENABLE_QUEUE_CAPACITY
	return (status);
#endif
} /* miQueueAppend */

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
static miQueueBuffer_t *miQueueReadOrRemove(MiQueue_t *q,
						buffer_mode_t mode,search_t *search)
{
	miQueueBuffer_t *buffer_current = NULL;
	miQueueBuffer_t *buffer_previous;

	cpu_irq_disable();
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
						(void *)buffer_current->buffer,
						search->handle);

				if (match) {
					/* Break, if search criteria matches */
					break;
				}

				buffer_previous = buffer_current;
				buffer_current = buffer_current->nextItem;
			}
		}

		/* Buffer matching with search criteria found */
		if (NULL != buffer_current) {
			/* Remove buffer from the queue */
			if (REMOVE_MODE == mode) {
				/* Update head if buffer removed is first node
				**/
				if (buffer_current == q->head) {
					q->head = buffer_current->nextItem;
				} else {
					/* Update the link by removing the
					 * buffer */
					buffer_previous->nextItem
						= buffer_current->nextItem;
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

	cpu_irq_enable();

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
miQueueBuffer_t *miQueueRemove(MiQueue_t *q, search_t *search)
{
	return (miQueueReadOrRemove(q, REMOVE_MODE, search));
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
miQueueBuffer_t *miQueueRead(MiQueue_t *q, search_t *search)
{
	return (miQueueReadOrRemove(q, READ_MODE, search));
}

/**
 * @brief Internal function for flushing a specific queue
 *
 * @param q Queue to be flushed
 */
void miQueueFlush(MiQueue_t *q)
{
	miQueueBuffer_t *buf_to_free;

	while (q->size > 0) {
		/* Remove the buffer from the queue and free it */
		buf_to_free = miQueueRemove(q, NULL);

		if (NULL == buf_to_free) {
			q->size = 0;
			return;
		}

		MiMem_Free((uint8_t *)buf_to_free);
	}
}

/* EOF */
