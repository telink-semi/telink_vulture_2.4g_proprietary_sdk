/********************************************************************************************************
 * @file	main.c
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

#include "driver.h"
#include "tpll.h"


#define PTX_CHANNEL    0

#define    DEBUG_PIN      GPIO_PD3

volatile unsigned char rx_flag = 0;
volatile unsigned char rx_data[128] = {0};


static void User_Init(unsigned char chn)
{

    WaitMs(3000);

    //rf configuration
    TPLL_Init(TPLL_BITRATE_2MBPS);
    TPLL_SetOutputPower(TPLL_RF_POWER_0DBM);
    TPLL_SetAddressWidth(ADDRESS_WIDTH_5BYTES);
    TPLL_ClosePipe(TPLL_PIPE_ALL);
#if PTX_CHANNEL == 0
    unsigned char rx_address[5] = {0xe7,0xe7,0xe7,0xe7,0xe7};
    //{0xe7,0xe7,0xe7,0xe7,0xe7};


    TPLL_SetAddress(TPLL_PIPE0,rx_address);
    TPLL_OpenPipe(TPLL_PIPE0);
#endif


#if PTX_CHANNEL == 1
    unsigned char rx_address1[5] = {0x55, 0x44, 0x33, 0x22, 0x11};
    TPLL_SetAddress(TPLL_PIPE1,rx_address1);
    TPLL_OpenPipe(TPLL_PIPE1);
#endif

#if PTX_CHANNEL == 2
    unsigned char rx_address2[5] = {0x55, 0x44, 0x33, 0x22, 0x11};
    TPLL_SetAddress(TPLL_PIPE1,rx_address2);
    rx_address2[0] = 0x22;
    TPLL_SetAddress(TPLL_PIPE2,&rx_address2[0]);
    TPLL_OpenPipe(TPLL_PIPE2);
#endif

#if PTX_CHANNEL == 3
    unsigned char rx_address3[5] = {0xaa,0xbb,0xcc,0xdd,0xee};;
    TPLL_SetAddress(TPLL_PIPE1,rx_address3);
    rx_address3[0] = 0x33;
    TPLL_SetAddress(TPLL_PIPE3,&rx_address3[0]);
    TPLL_OpenPipe(TPLL_PIPE3);
#endif

#if PTX_CHANNEL == 4
    unsigned char rx_address4[5] = {0xaa,0xbb,0xcc,0xdd,0xee};;
    TPLL_SetAddress(TPLL_PIPE1,rx_address4);
    rx_address4[0] = 0x44;
    TPLL_SetAddress(TPLL_PIPE4,&rx_address4[0]);
    TPLL_OpenPipe(TPLL_PIPE4);
#endif

#if PTX_CHANNEL == 5
    unsigned char rx_address5[5] = {0xaa,0xbb,0xcc,0xdd,0xee};;
    TPLL_SetAddress(TPLL_PIPE1,rx_address5);
    rx_address5[0] = 0x55;
    TPLL_SetAddress(TPLL_PIPE5,&rx_address5[0]);
    TPLL_OpenPipe(TPLL_PIPE5);
#endif


    TPLL_ModeSet(TPLL_MODE_PRX);
    TPLL_SetRFChannel(chn);
    TPLL_TxSettleSet(149);
    TPLL_RxSettleSet(80);
    irq_clr_src();
    rf_irq_clr_src(FLD_RF_IRQ_ALL);
    //enable irq
    irq_enable_type(FLD_IRQ_ZB_RT_EN); //enable RF irq
    rf_irq_disable(FLD_RF_IRQ_ALL);
    rf_irq_enable(FLD_RF_IRQ_TX|FLD_RF_IRQ_TX_DS|FLD_RF_IRQ_RX_DR);
    irq_enable(); //enable general irq

}

static unsigned char ack_payload[32] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
										0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,
										0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
										0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f};
volatile unsigned char ack_payload_length = 32;
volatile unsigned int Timestamp_value = 0;
volatile unsigned int Rx_interval_us = 0;
volatile signed char Rssi_value = 0;
unsigned short length_pip_ret;
void main(void)
{
	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    gpio_set_output_en(DEBUG_PIN, 1); //enable output
    gpio_set_input_en(DEBUG_PIN, 0); //disable input
    gpio_write(DEBUG_PIN,0);

    User_Init(4);

    //TPLL_WriteAckPayload(TPLL_PIPE0, ack_payload, ack_payload_length);
    TPLL_PRXTrig();
    Timestamp_value = clock_time();
     while (1)
    {

        if(1 == rx_flag)
        {
            rx_flag = 0;
            length_pip_ret = TPLL_ReadRxPayload(&rx_data);
            Rx_interval_us = (TPLL_GetTimestamp() - Timestamp_value) >> 4;
            Timestamp_value = TPLL_GetTimestamp();
            Rssi_value = TPLL_GetRxRssiValue();
            while(!TPLL_TxFifoEmpty(0));
            TPLL_WriteAckPayload(TPLL_PIPE0, ack_payload, ack_payload_length);
        }
    }
}




