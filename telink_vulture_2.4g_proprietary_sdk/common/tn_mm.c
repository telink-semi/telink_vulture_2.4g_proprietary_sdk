/********************************************************************************************************
 * @file	tn_mm.c
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
#include "tn_mm.h"
#include "string.h"

void tn_mem_init(struct mem_desc *m)
{
    memset(m->count, 0, m->num);
    memset(m->mem, 0, m->size*m->num);
}

void *tn_mem_alloc(struct mem_desc *m)
{
    int i;

    for(i = 0; i < m->num; ++i) {
        if(m->count[i] == 0) {
            ++(m->count[i]);
            return (void *)((char *)m->mem + (i * m->size));
        }
    }

    return 0;
}

char tn_mem_free(struct mem_desc *m, void *ptr)
{
    int i;
    char *ptr2;

    ptr2 = (char *)m->mem;
    for (i = 0; i < m->num; ++i) {
        if (ptr2 == (char *)ptr) {
            if (m->count[i] > 0) {
                --(m->count[i]);
            }
            return m->count[i];
        }
        ptr2 += m->size;
    }
    return -1;
}

int tn_mem_inmemb(struct mem_desc *m, void *ptr)
{
    return (char *)ptr >= (char *)m->mem &&
           (char *)ptr < (char *)m->mem + (m->num * m->size);
}

int tn_mem_numfree(struct mem_desc *m)
{
    int i;
    int num_free = 0;

    for(i = 0; i < m->num; ++i) {
        if(m->count[i] == 0) {
            ++num_free;
        }
    }

    return num_free;
}
