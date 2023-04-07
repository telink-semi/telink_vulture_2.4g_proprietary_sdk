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


#define PTX_CHANNEL                  0
#define FAST_CALIBRATION_ENABLE      1
#define FAST_CALIB_VALUE_ADDRESS     0x44000//0x40000+16kbyte
#define GREEN_LED_PIN         GPIO_PD3
#define TX_PIN                GPIO_PD0
#define RX_PIN                GPIO_PD6

static volatile unsigned short hpmc[160] = {0};
static volatile unsigned char tx_data[32] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,
                                             0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,
                                             0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,
                                             0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20};
volatile unsigned char rx_flag = 0;
volatile unsigned char ds_flag = 0;
volatile unsigned char maxretry_flag = 0;
unsigned char preamble_len=0;

void TPLL_Fast_SettleCalib() {
	unsigned char tmp = 0;
	unsigned char tx_data[2] = {0x11,0x22};
	unsigned short hp[160] = {0};
	flash_read_page(FAST_CALIB_VALUE_ADDRESS,160*2,hp);
	if(hp[0] != 0xffff)
	{
		flash_read_page(FAST_CALIB_VALUE_ADDRESS,160*2,hpmc);
		return;
	}
	tmp = TPLL_WriteTxPayload(0, (const unsigned char *)tx_data, 2);
	for (int i = 0; i <= 160; i++) {
		TPLL_SetRFChannel(i);
		WaitUs(100);
		TPLL_PTXTrig();
		WaitUs(250);
		hpmc[i] = TPLL_GetHpmcCalVal();
		WaitUs(100);
	}
	flash_write_page(FAST_CALIB_VALUE_ADDRESS,160*2,hpmc);
	WaitUs(600);
}

static void User_Init(unsigned short chn)
{

    gpio_set_output_en(GREEN_LED_PIN|TX_PIN|RX_PIN, 1); //enable output
    gpio_set_input_en(GREEN_LED_PIN|TX_PIN|RX_PIN, 0); //disable input
    gpio_write(GREEN_LED_PIN|TX_PIN|RX_PIN, 1); //LED Off

//    rf configuration
    TPLL_Init(TPLL_BITRATE_2MBPS);
    TPLL_SetOutputPower(TPLL_RF_POWER_0DBM);
    TPLL_SetAddressWidth(ADDRESS_WIDTH_5BYTES);
    TPLL_ClosePipe(TPLL_PIPE_ALL);

#if PTX_CHANNEL == 0
    //unsigned char tx_address[3] = {0xe7,0xe7,0xe7};
    unsigned char tx_address[5] = {0xe7,0xe7,0xe7,0xe7,0xe7}; //{0xaa,0xbb,0xcc,0xdd,0xee};
    TPLL_SetAddress(TPLL_PIPE0, tx_address);
    TPLL_OpenPipe(TPLL_PIPE0);
    TPLL_SetTXPipe(TPLL_PIPE0);
#endif

#if PTX_CHANNEL == 1
    unsigned char tx_address1[5] = {0x55, 0x44, 0x33, 0x22, 0x11};
    TPLL_SetAddress(TPLL_PIPE1, tx_address1);
    TPLL_OpenPipe(TPLL_PIPE1);
    TPLL_SetTXPipe(TPLL_PIPE1);
#endif

#if PTX_CHANNEL == 2
    unsigned char tx_address2[5] = {0x55, 0x44, 0x33, 0x22, 0x11};
    TPLL_SetAddress(TPLL_PIPE1, tx_address2);
    tx_address2[0] = 0x22;
    TPLL_SetAddress(TPLL_PIPE2, &tx_address2[0]);
    TPLL_OpenPipe(TPLL_PIPE2);
    TPLL_SetTXPipe(TPLL_PIPE2);
#endif

#if PTX_CHANNEL == 3
    unsigned char tx_address3[5] = {0x55, 0x44, 0x33, 0x22, 0x11};
    TPLL_SetAddress(TPLL_PIPE1, tx_address3);
    tx_address3[0] = 0x33;
    TPLL_SetAddress(TPLL_PIPE3, &tx_address3[0]);
    TPLL_OpenPipe(TPLL_PIPE3);
    TPLL_SetTXPipe(TPLL_PIPE3);
#endif

	WaitUs(150);
	//configure irq
	irq_clr_src();
	rf_irq_clr_src(FLD_RF_IRQ_ALL);

	irq_enable_type(FLD_IRQ_ZB_RT_EN); //enable RF irq
	rf_irq_disable(FLD_RF_IRQ_ALL);
	rf_irq_enable(FLD_RF_IRQ_TX|FLD_RF_IRQ_TX_DS|FLD_RF_IRQ_RETRY_HIT|FLD_RF_IRQ_RX_DR|FLD_RF_IRQ_RX|FLD_RF_IRQ_INVALID_PID);
	irq_enable(); //enable general irq

    TPLL_ModeSet(TPLL_MODE_PTX);
    TPLL_Fast_RxSettleInit(TPLL_RX_SETTLE_TIME_44US);
    TPLL_Fast_TxSettleInit(TPLL_TX_SETTLE_TIME_53US);
    TPLL_SetAutoRetry(0,150);  //5,150
    TPLL_RxTimeoutSet(500);
    TPLL_EnableHpmc();//put this in user_init befor TPLL_SetRFChannel
    TPLL_SetRFChannel(4);
    TPLL_Fast_RxWaitSet(85);
    TPLL_Fast_TxWaitSet(118);
	TPLL_Fast_TxWaitSet(5);
	TPLL_Preamble_Set(2);

#if(FAST_CALIBRATION_ENABLE)
    TPLL_Fast_SettleCalib();
    TPLL_SetHpmcCalVal(hpmc[4]);
#endif

    TPLL_SetRFChannel(chn);
    TPLL_Fast_RxSettleSet(50);
   	TPLL_Fast_TxSettleSet(50);
	TPLL_Fast_TxWaitSet(0);
	TPLL_Fast_RxWaitSet(0);
	TPLL_Fast_RxSettleEnable();
	TPLL_Fast_TxSettleEnable();
	TPLL_ResetBuf();

}


void main(void)
{
    unsigned char tmp = 0;
    unsigned char tx_len = 32;

	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

    clock_init(SYS_CLK_48M_Crystal);

    User_Init(4);

    tmp = TPLL_WriteTxPayload(PTX_CHANNEL, (const unsigned char *)tx_data, tx_len);

    TPLL_PTXTrig();

    while(1)
    {
    	preamble_len=TPLL_Preamble_Read();
        if (ds_flag ||maxretry_flag)
        {
        	if(ds_flag)
            gpio_toggle(GREEN_LED_PIN);
            ds_flag = 0;
            maxretry_flag = 0;
            WaitMs(500);
            tmp = TPLL_WriteTxPayload(PTX_CHANNEL, (const unsigned char *)tx_data, tx_len);
            if(tmp)
            {
                TPLL_PTXTrig();
            }
        }
    }
}
