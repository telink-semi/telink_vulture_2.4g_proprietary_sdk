/********************************************************************************************************
 * @file	mmem.c
 *
 * @brief	This is the source file for 8355
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


#include "mmem.h"
#include "list.h"

#define		MMEM_SIZE	2048

LIST(mmemlist);
unsigned int avail_memory;
static char memory[MMEM_SIZE];

/*---------------------------------------------------------------------------*/
/**
 * \brief      Allocate a managed memory block
 * \param m    A pointer to a struct mmem.
 * \param size The size of the requested memory block
 * \return     Non-zero if the memory could be allocated, zero if memory
 *             was not available.
 * \author     Adam Dunkels
 *
 *             This function allocates a chunk of managed memory. The
 *             memory allocated with this function must be deallocated
 *             using the mmem_free() function.
 *
 *             \note This function does NOT return a pointer to the
 *             allocated memory, but a pointer to a structure that
 *             contains information about the managed memory. The
 *             macro MMEM_PTR() is used to get a pointer to the
 *             allocated memory.
 *
 */
int
mmem_alloc(struct mmem *m, unsigned int size)
{
  /* Check if we have enough memory left for this allocation. */
  if(avail_memory < size) {
    return 0;
  }

  /* We had enough memory so we add this memory block to the end of
     the list of allocated memory blocks. */
  list_add(mmemlist, m);

  /* Set up the pointer so that it points to the first available byte
     in the memory block. */
  m->ptr = &memory[MMEM_SIZE - avail_memory];

  /* Remember the size of this memory block. */
  m->size = size;

  /* Decrease the amount of available memory. */
  avail_memory -= size;

  /* Return non-zero to indicate that we were able to allocate
     memory. */
  return 1;
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Deallocate a managed memory block
 * \param m    A pointer to the managed memory block
 * \author     Adam Dunkels
 *
 *             This function deallocates a managed memory block that
 *             previously has been allocated with mmem_alloc().
 *
 */
void *  memmove(void * dest, const void * src, unsigned int n);
void
mmem_free(struct mmem *m)
{
  struct mmem *n;

  if(m->next != 0) {
    /* Compact the memory after the allocation that is to be removed
       by moving it downwards. */
    memmove(m->ptr, m->next->ptr,
	    &memory[MMEM_SIZE - avail_memory] - (char *)m->next->ptr);

    /* Update all the memory pointers that points to memory that is
       after the allocation that is to be removed. */
    for(n = m->next; n != 0; n = n->next) {
      n->ptr = (void *)((char *)n->ptr - m->size);
    }
  }

  avail_memory += m->size;

  /* Remove the memory block from the list. */
  list_remove(mmemlist, m);
}
/*---------------------------------------------------------------------------*/
/**
 * \brief      Initialize the managed memory module
 * \author     Adam Dunkels
 *
 *             This function initializes the managed memory module and
 *             should be called before any other function from the
 *             module.
 *
 */
void
mmem_init(void)
{
  list_init(mmemlist);
  avail_memory = MMEM_SIZE;
}
/*---------------------------------------------------------------------------*/

/** @} */
