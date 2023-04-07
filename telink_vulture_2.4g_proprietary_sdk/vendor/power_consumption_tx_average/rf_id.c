/********************************************************************************************************
 * @file	rf_id.c
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

#include "common.h"
#include "driver.h"
#include "rf_id.h"
#include "genfsk_ll.h"

#define    TEST_CHN_NUM         7 //2403.5MHz

static unsigned char sync_word[4] = {0x53, 0x78, 0x56, 0x52};
static unsigned char __attribute__ ((aligned (4))) tx_buffer[64] = {0};

_attribute_ram_code_sec_noinline_ void rfid_radio_config(void)
{

    gen_fsk_datarate_set(GEN_FSK_DATARATE_2MBPS); //Note that this API must be invoked first before all other APIs
    gen_fsk_preamble_len_set(4);
    gen_fsk_sync_word_len_set(SYNC_WORD_LEN_4BYTE);
    gen_fsk_sync_word_set(GEN_FSK_PIPE0, sync_word); //set pipe0's sync word
    gen_fsk_pipe_open(GEN_FSK_PIPE0); //enable pipe0's reception
    gen_fsk_tx_pipe_set(GEN_FSK_PIPE0); //set pipe0 as the TX pipe
    gen_fsk_packet_format_set(GEN_FSK_PACKET_FORMAT_FIXED_PAYLOAD, 64);
    gen_fsk_radio_power_set(GEN_FSK_RADIO_POWER_0DBM);
    gen_fsk_channel_set(TEST_CHN_NUM); //set rf freq as 2403MHz
    gen_fsk_radio_state_set(GEN_FSK_STATE_AUTO); //set transceiver to basic TX state
    gen_fsk_tx_settle_set(149);
}

_attribute_ram_code_sec_noinline_ void rfid_packet_send(unsigned char *pdu, unsigned char pdu_len)
{
    unsigned char i = 0;

    //fill the DMA tx buffer
    tx_buffer[0] = pdu_len;
    tx_buffer[1] = 0x00;
    tx_buffer[2] = 0x00;
    tx_buffer[3] = 0x00;
    tx_buffer[4] = pdu[0]; //header0
    tx_buffer[5] = pdu[1]; //header1(payload length) 

    for (i = 0; i < (pdu_len-2); i++) {
        tx_buffer[6+i] = pdu[2+i];
    }
    //gpio_write(DEBUG_PIN, 1);
    //WaitMs(50);
   // rf_start_stx(tx_buffer, clock_time());
    gen_fsk_stx_start(tx_buffer, clock_time());
    cpu_stall(FLD_IRQ_ZB_RT_EN|FLD_IRQ_TMR1_EN, 1200,24);
}
