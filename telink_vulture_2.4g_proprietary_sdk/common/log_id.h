/********************************************************************************************************
 * @file	log_id.h
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

/*
	ID == -1 is invalid
	if you want to shut down logging a specified id,  assigne -1 to it
*/


///////////////////////////////////////////////////////////////

// Please donot change the following system defines
// 

#define TR_T_invalid			-1
#define TR_T_EVENT_0			0
#define TR_T_EVENT_1			1
#define TR_T_EVENT_2			2
#define TR_T_EVENT_3			3
#define TR_T_EVENT_4			4
#define TR_T_EVENT_5			5
#define TR_T_EVENT_6			6
#define TR_T_EVENT_7			7
#define TR_T_EVENT_8			8
#define TR_T_EVENT_E			9

#define TR_T_POLL_0				10
#define TR_T_POLL_1				11
#define TR_T_POLL_2				12
#define TR_T_POLL_3				13
#define TR_T_POLL_4				14
#define TR_T_POLL_5				15
#define TR_T_POLL_6				16
#define TR_T_POLL_7				17
#define TR_T_POLL_8				18
#define TR_T_POLL_E				19

#define TR_T_TIMER_0			20
#define TR_T_TIMER_1			21
#define TR_T_TIMER_2			22
#define TR_T_TIMER_3			23
#define TR_T_TIMER_4			24
#define TR_T_TIMER_5			25
#define TR_T_TIMER_6			26
#define TR_T_TIMER_7			27
#define TR_T_TIMER_8			28
#define TR_T_TIMER_E			29	// user define tick log id, from 30 - 40,  10 valid for using

#define TR_T_user0				30

#define TR_24_TIMERS_ADDR		0	// this ID cannot be used because timer id == 0 is reserved for indicating id not added
#define TR_24_TIMER0_ADDR		1
#define TR_24_TIMER1_ADDR		2
#define TR_24_TIMER2_ADDR		3
#define TR_24_TIMER3_ADDR		4
#define TR_24_TIMER4_ADDR		5
#define TR_24_TIMER5_ADDR		6
#define TR_24_TIMER6_ADDR		7
#define TR_24_TIMER7_ADDR		8
#define TR_24_TIMER8_ADDR		9
#define TR_24_TIMER9_ADDR		10
#define TR_24_TIMER10_ADDR		11
#define TR_24_TIMER11_ADDR		12
#define TR_24_TIMER12_ADDR		13
#define TR_24_TIMER13_ADDR		14
#define TR_24_TIMER14_ADDR		15

#define TR_24_user1				16	// user define data log id, from 16 -- 64,
#define TR_24_user2				17

