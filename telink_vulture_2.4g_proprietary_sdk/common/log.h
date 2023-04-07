/********************************************************************************************************
 * @file	log.h
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

#include "log_id.h"
#include "printf.h"


#ifndef LOG_IN_RAM
#define LOG_IN_RAM   0
#endif

#define		LOG_MASK_BEGIN		0x40
#define		LOG_MASK_END		0x00
#define		LOG_MASK_TGL		0xC0
#define		LOG_MASK_DAT		0x80

#define U8_SET(addr, v)			(*(volatile unsigned char  *)(addr) = (unsigned char)(v))
static inline void swire2usb_init (void) {
	U8_SET(0x800128, 0x00);
	U8_SET(0x80012d, 0);
	U8_SET(0x80013b, 32);
}

static inline void usb_log_init (void) {
	U8_SET(0x80013c, 0x40);
	U8_SET(0x80013d, 0x09);
}

#if (__LOG_RT_ENABLE__)
void log_write_data(int id, int type, u32 dat);
void log_task_begin(int id);
void log_task_end(int id);
void log_event(int id);
void log_data(int id, u32 dat);

#define	LOG_TICK(id,e)	do{log_task_begin(id); e; log_task_end(id);}while(0)

#define LOG(x, s,...) printf("(%s:%d)"  x "\r\n" , __FUNCTION__, __LINE__, ## s)

#else
#define log_write_data(id, type, dat)
#define log_task_begin(id)
#define log_task_end(id)
#define log_event(id)
#define log_data(id, dat)
#define	LOG_TICK(id,e)			do{e;}while (0)

#define LOG(x, s,...)

#endif

