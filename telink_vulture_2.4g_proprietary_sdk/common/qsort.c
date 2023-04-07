/********************************************************************************************************
 * @file	qsort.c
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
// #include <limits.h>
// #include <stddef.h>
#include "types.h"

/* Insertion sort threshold shift
 *
 * This macro defines the threshold shift (power of 2) at which the insertion
 * sort algorithm replaces the Quicksort.  A zero threshold shift disables the
 * insertion sort completely.
 *
 * The value is optimized for Linux and MacOS on the Intel x86 platform.
 */
#ifndef INSERTION_SORT_THRESHOLD_SHIFT
#  define INSERTION_SORT_THRESHOLD_SHIFT 2
#endif

/* Macro SWAP
 *
 * Swaps the elements of two arrays.
 *
 * The length of the swap is determined by the value of "SIZE".  While both
 * arrays can't overlap, the case in which both pointers are the same works.
 */
#define SWAP(A,B,SIZE)                               \
    {                                                \
        register char       *a_byte = A;             \
        register char       *b_byte = B;             \
        register const char *a_end  = a_byte + SIZE; \
                                                     \
        while (a_byte < a_end)                       \
        {                                            \
            register const char swap_byte = *b_byte; \
            *b_byte++ = *a_byte;                     \
            *a_byte++ = swap_byte;                   \
        }                                            \
    }

/* Macro SWAP_NEXT
 *
 * Swaps the elements of an array with its next value.
 *
 * The length of the swap is determined by the value of "size".  This macro
 * must be used at the beginning of a scope and "A" shouldn't be an expression.
 */
#define SWAP_NEXT(A,SIZE)                                 \
    register char       *a_byte = A;                      \
    register const char *a_end  = A + SIZE;               \
                                                          \
    while (a_byte < a_end)                                \
    {                                                     \
        register const char swap_byte = *(a_byte + SIZE); \
        *(a_byte + SIZE) = *a_byte;                       \
        *a_byte++ = swap_byte;                            \
    }

/* Function Quicksort
 *
 * This function performs a basic Quicksort.  This implementation is the
 * in-place version of the algorithm and is done in he following way:
 *
 * 1. In the middle of the array, we determine a pivot that we temporarly swap
 *    to the end.
 * 2. From the beginning to the end of the array, we swap any elements smaller
 *    than this pivot to the start, adjacent to other elements that were
 *    already moved.
 * 3. We swap the pivot next to these smaller elements.
 * 4. For both sub-arrays on sides of the pivot, we repeat this process
 *    recursively.
 * 5. For a sub-array smaller than a certain threshold, the insertion sort
 *    algorithm takes over.
 *
 * As an optimization, rather than performing a real recursion, we keep a
 * global stack to track boundaries for each recursion level.
 *
 * To ensure that at most O(log2 N) space is used, we recurse into the smaller
 * partition first.  The log2 of the highest unsigned value of an integer type
 * is the number of bits needed to store that integer. 
 */
void qsort(void   *array,
               size_t  length,
               size_t  size,
               int(*compare)(const void *, const void *))
{
    /* Recursive stacks for array boundaries (both inclusive) */
    struct stackframe
    {
        void *left;
        void *right;
    } stack[8 * sizeof(void *)];

    /* Recursion level */
    struct stackframe *recursion = stack;

#if INSERTION_SORT_THRESHOLD_SHIFT != 0
    /* Insertion sort threshold */
    const int threshold = size << INSERTION_SORT_THRESHOLD_SHIFT;
#endif

    /* Assign the first recursion level of the sorting */
    recursion->left = array;
    recursion->right = (char *)array + size * (length - 1);

    do
    {
        /* Partition the array */
        register char *index = (char*)recursion->left;
        register char *right = (char*)recursion->right;
        char          *left  = index;

        /* Assigning store to the left */
        register char *store = index;

        /* Pop the stack */
        --recursion;

        /* Determine a pivot (in the middle) and move it to the end */
        SWAP(left + (((right - left) >> 1) & (~(size - 1))),right,size)

        /* From left to right */
        while (index < right)
        {
            /* If item is smaller than pivot */
            if (compare(right, index) > 0)
            {
                /* Swap item and store */
                SWAP(index,store,size)

                /* We increment store */
                store += size;
            }

            index += size;
        }

        /* Move the pivot to its final place */
        SWAP(right,store,size)

/* Performs a recursion to the left */
#define RECURSE_LEFT                     \
    if (left < store - size)             \
    {                                    \
        (++recursion)->left = left;      \
        recursion->right = store - size; \
    }

/* Performs a recursion to the right */
#define RECURSE_RIGHT                       \
    if (store + size < right)               \
    {                                       \
        (++recursion)->left = store + size; \
        recursion->right = right;           \
    }

/* Insertion sort inner-loop */
#define INSERTION_SORT_LOOP(LEFT)                                 \
    {                                                             \
        register char *trail = index - size;                      \
        while (trail >= LEFT && compare(trail, trail + size) > 0) \
        {                                                         \
            SWAP_NEXT(trail,size)                                 \
            trail -= size;                                        \
        }                                                         \
    }

/* Performs insertion sort left of the pivot */
#define INSERTION_SORT_LEFT                                \
    for (index = left + size; index < store; index +=size) \
        INSERTION_SORT_LOOP(left)

/* Performs insertion sort right of the pivot */
#define INSERTION_SORT_RIGHT                                        \
    for (index = store + (size << 1); index <= right; index +=size) \
        INSERTION_SORT_LOOP(store + size)

/* Sorts to the left */
#if INSERTION_SORT_THRESHOLD_SHIFT == 0
# define SORT_LEFT RECURSE_LEFT
#else
# define SORT_LEFT                 \
    if (store - left <= threshold) \
    {                              \
        INSERTION_SORT_LEFT        \
    }                              \
    else                           \
    {                              \
        RECURSE_LEFT               \
    }
#endif

/* Sorts to the right */
#if INSERTION_SORT_THRESHOLD_SHIFT == 0
# define SORT_RIGHT RECURSE_RIGHT
#else
# define SORT_RIGHT                 \
    if (right - store <= threshold) \
    {                               \
        INSERTION_SORT_RIGHT        \
    }                               \
    else                            \
    {                               \
        RECURSE_RIGHT               \
    }
#endif

        /* Recurse into the smaller partition first */
        if (store - left < right - store)
        {
            /* Left side is smaller */
            SORT_RIGHT
            SORT_LEFT

            continue;
        }

        /* Right side is smaller */
        SORT_LEFT
        SORT_RIGHT

#undef RECURSE_LEFT
#undef RECURSE_RIGHT
#undef INSERTION_SORT_LOOP
#undef INSERTION_SORT_LEFT
#undef INSERTION_SORT_RIGHT
#undef SORT_LEFT
#undef SORT_RIGHT
    }
    while (recursion >= stack);
}

#undef INSERTION_SORT_THRESHOLD_SHIFT
#undef SWAP
#undef SWAP_NEXT

