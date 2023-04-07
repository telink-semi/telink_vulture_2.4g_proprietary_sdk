/********************************************************************************************************
 * @file	tn_list.c
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
#include "tn_list.h"

struct list {
    struct list *next;
};

//Initialize a list
void tn_list_init(tn_list_t list)
{
    *list = 0;
}

//Get a pointer to the first element of a list
void *tn_list_head(tn_list_t list)
{
    return *list;
}

//Copy a list
void tn_list_copy(tn_list_t dest, tn_list_t src)
{
    *dest = *src;
}

//Get the tail of a list
void *tn_list_tail(tn_list_t list)
{
    struct list *ptr;

    if (*list == 0) {
        return 0;
    }

    for (ptr = *list; ptr->next != 0; ptr = ptr->next);

    return ptr;
}

//Add an item at the end of a list
void tn_list_add(tn_list_t list, void *item)
{
    struct list *ptr;

    tn_list_remove(list, item);/* Make sure not to add the same element twice */

    ((struct list *)item)->next = 0;

    ptr = tn_list_tail(list);

    if (ptr == 0) {
        *list = item;
    }
    else {
        ptr->next = item;
    }
}

//Add an item to the start of the list.
void tn_list_push(tn_list_t list, void *item)
{
    tn_list_remove(list, item);/* Make sure not to add the same element twice */

    ((struct list *)item)->next = *list;
    *list = item;
}

//Remove the last item from the list
void *tn_list_chop(tn_list_t list)
{
    struct list *ptr, *ptr2;

    if (*list == 0) {
        return 0;
    }

    if (((struct list *)*list)->next == 0) {
        ptr = *list;
        *list = 0;
        return ptr;
    }

    for (ptr = *list; ptr->next->next != 0; ptr = ptr->next);
    ptr2 = ptr->next;
    ptr->next = 0;

    return ptr2;
}

//Remove the first item from a list
void *tn_list_pop(tn_list_t list)
{
    struct list *ptr;
    ptr = *list;
    if (*list != 0) {
        *list = ((struct list *)*list)->next;
    }

    return ptr;
}

//Remove a specified item from a list
void tn_list_remove(tn_list_t list, void *item)
{
    struct list *ptr, *ptr2;

    if (*list == 0) {
        return;
    }

    ptr2 = 0;
    for (ptr = *list; ptr != 0; ptr = ptr->next) {
        if (ptr == item) {
            if (ptr2 == 0) {
                /* first on list */
                *list = ptr->next;
            }
            else {
                /* Not first on list */
                ptr2->next = ptr->next;
            }
            ptr->next = 0;
            return;
        }
        ptr2 = ptr;
    }
}

//Get the length of a list
int tn_list_length(tn_list_t list)
{
    struct list *ptr;
    int n = 0;

    for (ptr = *list; ptr != 0; ptr = ptr->next) {
        ++n;
    }

    return n;
}

//Insert an item after a specified item on the list
void tn_list_insert(tn_list_t list, void *previtem, void *newitem)
{
    if (previtem == 0) {
        tn_list_push(list, newitem);
    }
    else {
        ((struct list *)newitem)->next = ((struct list *)previtem)->next;
        ((struct list *)previtem)->next = newitem;
    }
}

//Get the item next to this item
void *tn_list_item_next(void *item)
{
    return item == 0 ? 0: ((struct list *)item)->next;
}
