/********************************************************************************************************
 * @file	ringbuffer.h
 *
 * @brief	This is the header file for 8355
 *
 * @author	2.4G Group
 * @date	2019
 *
 * @par     Copyright (c) 2019, Telink Semiconductor (Shanghai) Co., Ltd. ("TELINK")
 *          All rights reserved.
 *
 *          Redistribution and use in source and binary forms, with or without
 *          modification, are permitted provided that the following conditions are met:
 *
 *              1. Redistributions of source code must retain the above copyright
 *              notice, this list of conditions and the following disclaimer.
 *
 *              2. Unless for usage inside a TELINK integrated circuit, redistributions
 *              in binary form must reproduce the above copyright notice, this list of
 *              conditions and the following disclaimer in the documentation and/or other
 *              materials provided with the distribution.
 *
 *              3. Neither the name of TELINK, nor the names of its contributors may be
 *              used to endorse or promote products derived from this software without
 *              specific prior written permission.
 *
 *              4. This software, with or without modification, must only be used with a
 *              TELINK integrated circuit. All other usages are subject to written permission
 *              from TELINK and different commercial license may apply.
 *
 *              5. Licensee shall be solely responsible for any claim to the extent arising out of or
 *              relating to such deletion(s), modification(s) or alteration(s).
 *
 *          THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 *          ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 *          WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *          DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 *          DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 *          (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *          LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 *          ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *          (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *          SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************************************/

#pragma once

/* Includes: */
#include "types.h"
#include "../mcu/irq_i.h"

/* Enable C linkage for C++ Compilers: */
#if defined(__cplusplus)
	extern "C" {
#endif

/* Type Defines: */
/** \brief Ring buffer Management Structure.
 *
 *  Type define for a new ring buffer object. buffers should be initialized via a call to
 *  \ref ringbuffer_init() before use.
 */
typedef struct
{
	u8* in; /**< Current storage location in the circular buffer. */
	u8* out; /**< Current retrieval location in the circular buffer. */
	u8* start; /**< Pointer to the start of the buffer's underlying storage array. */
	u8* end; /**< Pointer to the end of the buffer's underlying storage array. */
	u16 size; /**< size of the buffer's underlying storage array. */
	u16 sount; /**< Number of bytes currently stored in the buffer. */
} ringbuffer_t;

/* Inline Functions: */
/** Initializes a ring buffer ready for use. buffers must be initialized via this function
 *  before any operations are called upon them. Already initialized buffers may be reset
 *  by re-initializing them using this function.
 *
 *  \param[out] buffer   Pointer to a ring buffer structure to initialize.
 *  \param[out] dataptr  Pointer to a global array that will hold the data stored into the ring buffer.
 *  \param[out] size     Maximum number of bytes that can be stored in the underlying data array.
 */
static inline void ringbuffer_init(ringbuffer_t* buffer, u8* const dataptr, const u16 size)
{
	u8 r = irq_disable();

	buffer->in     = dataptr;
	buffer->out    = dataptr;
	buffer->start  = &dataptr[0];
	buffer->end    = &dataptr[size];
	buffer->size   = size;
	buffer->sount  = 0;

	irq_restore(r);
}

/** Retrieves the current number of bytes stored in a particular buffer. This value is computed
 *  by entering an atomic lock on the buffer, so that the buffer cannot be modified while the
 *  computation takes place. This value should be cached when reading out the contents of the buffer,
 *  so that as small a time as possible is spent in an atomic lock.
 *
 *  \note The value returned by this function is guaranteed to only be the minimum number of bytes
 *        stored in the given buffer; this value may change as other threads write new data, thus
 *        the returned number should be used only to determine how many successive reads may safely
 *        be performed on the buffer.
 *
 *  \param[in] buffer  Pointer to a ring buffer structure whose count is to be computed.
 *
 *  \return Number of bytes currently stored in the buffer.
 */
static inline u16 ringbuffer_get_count(ringbuffer_t* const buffer)
{
	u8 r = irq_disable();
	u16 sount = buffer->sount;
	irq_restore(r);
	return sount;
}

/** Retrieves the free space in a particular buffer. This value is computed by entering an atomic lock
 *  on the buffer, so that the buffer cannot be modified while the computation takes place.
 *
 *  \note The value returned by this function is guaranteed to only be the maximum number of bytes
 *        free in the given buffer; this value may change as other threads write new data, thus
 *        the returned number should be used only to determine how many successive writes may safely
 *        be performed on the buffer when there is a single writer thread.
 *
 *  \param[in] buffer  Pointer to a ring buffer structure whose free count is to be computed.
 *
 *  \return Number of free bytes in the buffer.
 */
static inline u16 ringbuffer_get_free_count(ringbuffer_t* const buffer)
{
	return (buffer->size - ringbuffer_get_count(buffer));
}

/** Atomically determines if the specified ring buffer contains any data. This should
 *  be tested before removing data from the buffer, to ensure that the buffer does not
 *  underflow.
 *
 *  If the data is to be removed in a loop, store the total number of bytes stored in the
 *  buffer (via a call to the \ref ringbuffer_get_count() function) in a temporary variable
 *  to reduce the time spent in atomicity locks.
 *
 *  \param[in,out] buffer  Pointer to a ring buffer structure to insert into.
 *
 *  \return Boolean \c true if the buffer contains no free space, false otherwise.
 */
static inline bool ringbuffer_is_empty(ringbuffer_t* const buffer)
{
	return (ringbuffer_get_count(buffer) == 0);
}

/** Atomically determines if the specified ring buffer contains any free space. This should
 *  be tested before storing data to the buffer, to ensure that no data is lost due to a
 *  buffer overrun.
 *
 *  \param[in,out] buffer  Pointer to a ring buffer structure to insert into.
 *
 *  \return Boolean \c true if the buffer contains no free space, false otherwise.
 */
static inline bool ringbuffer_is_full(ringbuffer_t* const buffer)
{
	return (ringbuffer_get_count(buffer) == buffer->size);
}

/** Inserts an element into the ring buffer.
 *
 *  \note Only one execution thread (main program thread or an ISR) may insert into a single buffer
 *        otherwise data corruption may occur. Insertion and removal may occur from different execution
 *        threads.
 *
 *  \param[in,out] buffer  Pointer to a ring buffer structure to insert into.
 *  \param[in]     Data    Data element to insert into the buffer.
 */
static inline void ringbuffer_insert(ringbuffer_t* buffer, const u8 Data)
{
	*buffer->in = Data;

	if (++buffer->in == buffer->end)
	  buffer->in = buffer->start;

	u8 r = irq_disable();

	buffer->sount++;

	irq_restore(r);
}

/** Removes an element from the ring buffer.
 *
 *  \note Only one execution thread (main program thread or an ISR) may remove from a single buffer
 *        otherwise data corruption may occur. Insertion and removal may occur from different execution
 *        threads.
 *
 *  \param[in,out] buffer  Pointer to a ring buffer structure to retrieve from.
 *
 *  \return Next data element stored in the buffer.
 */
static inline u8 ringbuffer_remove(ringbuffer_t* buffer)
{
	u8 Data = *buffer->out;

	if (++buffer->out == buffer->end)
	  buffer->out = buffer->start;

	u8 r = irq_disable();

	buffer->sount--;

	irq_restore(r);

	return Data;
}

/** Returns the next element stored in the ring buffer, without removing it.
 *
 *  \param[in,out] buffer  Pointer to a ring buffer structure to retrieve from.
 *
 *  \return Next data element stored in the buffer.
 */
static inline u8 ringbuffer_peek(ringbuffer_t* const buffer)
{
	return *buffer->out;
}

/* Disable C linkage for C++ Compilers: */
#if defined(__cplusplus)
}
#endif

/** @} */

