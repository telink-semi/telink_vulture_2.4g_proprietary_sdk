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
#include "genfsk_ll.h"


#define    GREEN_LED_PIN       GPIO_PD3
#define    TIME_DEBUG_PIN      GPIO_PC7

#define  RF_FAST_RX_SETTING_ENABLE  1

#define    RX_BUF_LEN                     64
#define    RX_BUF_NUM                     4
volatile static unsigned char rx_buf[RX_BUF_LEN*RX_BUF_NUM] __attribute__ ((aligned (4))) = {};
volatile static unsigned char rx_ptr = 0;
volatile static unsigned char rx_flag = 0;
volatile static unsigned char rx_first_timeout = 0;
volatile static unsigned char *rx_packet = 0;
volatile static unsigned char rx_payload_len = 0;
volatile static unsigned char *rx_payload = 0;
volatile static unsigned char rssi = 0;
volatile static unsigned int rx_timestamp = 0;

volatile static unsigned int rx_test_cnt = 0;

_attribute_ram_code_sec_noinline_ __attribute__((optimize("-Os")))void irq_handler (void)
{
    unsigned int irq_src = irq_get_src();
    unsigned short rf_irq_src = rf_irq_src_get();

    if (irq_src & FLD_IRQ_ZB_RT_EN) {//if rf irq occurs
        if (rf_irq_src & FLD_RF_IRQ_RX) {//if rf rx irq occurs
            rf_irq_clr_src(FLD_RF_IRQ_RX);
            rx_test_cnt++;
            rx_packet = rx_buf + rx_ptr*RX_BUF_LEN;

            //set to next rx_buf
            rx_ptr = (rx_ptr + 1) % RX_BUF_NUM;
            gen_fsk_rx_buffer_set((unsigned char *)(rx_buf + rx_ptr*RX_BUF_LEN), RX_BUF_LEN);
     
            if (gen_fsk_is_rx_crc_ok((unsigned char *)rx_packet)) {
            }
			rx_flag = 1;
        }

        if (rf_irq_src & FLD_RF_IRQ_FIRST_TIMEOUT) {//if rf rx irq occurs
            rf_irq_clr_src(FLD_RF_IRQ_FIRST_TIMEOUT);
            rx_first_timeout = 1;
        }
    }
    irq_clr_src2(FLD_IRQ_ALL);
}

int main(void)
{
    unsigned char sync_word[4] = {0x53, 0x78, 0x56, 0x52};
    
	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    //USB interface config
    gpio_set_func(GPIO_PA5, AS_USB); //config GPIO_PA5 as USB DM pin
    gpio_set_func(GPIO_PA6, AS_USB); //config GPIO_PA6 as USB DP pin
    usb_dp_pullup_en(1); //enable the pull-up of DP pin


    //LED pin config
    gpio_set_func(GREEN_LED_PIN, AS_GPIO);
    gpio_set_output_en(GREEN_LED_PIN, 1); //enable output
    gpio_write(GREEN_LED_PIN, 0); //LED Off

    gpio_set_func(TIME_DEBUG_PIN, AS_GPIO);
    gpio_set_output_en(TIME_DEBUG_PIN, 1); 
    gpio_write(TIME_DEBUG_PIN, 0);

    //generic FSK Link Layer configuratioin
    gen_fsk_datarate_set(GEN_FSK_DATARATE_2MBPS); //Note that this API must be invoked first before all other APIs
    gen_fsk_preamble_len_set(4);
    gen_fsk_sync_word_len_set(SYNC_WORD_LEN_4BYTE);
    gen_fsk_sync_word_set(GEN_FSK_PIPE0, sync_word); //set pipe0's sync word
    gen_fsk_pipe_open(GEN_FSK_PIPE0); //enable pipe0's reception
    gen_fsk_tx_pipe_set(GEN_FSK_PIPE0); //set pipe0 as the TX pipe
    gen_fsk_packet_format_set(GEN_FSK_PACKET_FORMAT_FIXED_PAYLOAD, 8);
    gen_fsk_radio_power_set(GEN_FSK_RADIO_POWER_0DBM);
    gen_fsk_rx_buffer_set((unsigned char *)rx_buf + rx_ptr*RX_BUF_LEN, RX_BUF_LEN);
    gen_fsk_channel_set(7); //set rf freq as 2403.5MHz
    gen_fsk_radio_state_set(GEN_FSK_STATE_AUTO); //set transceiver to basic RX state
#if(RF_FAST_RX_SETTING_ENABLE)
    gen_fsk_fast_rxsettleInit(GEN_RX_SETTLE_TIME_44US);
    gen_fsk_rx_settle_set(44);
    gen_fsk_fast_rxsettleEnable();
#else
    gen_fsk_rx_settle_set(89);
#endif
    //irq configuration
    rf_irq_disable(FLD_RF_IRQ_ALL);
    rf_irq_enable(FLD_RF_IRQ_RX | FLD_RF_IRQ_FIRST_TIMEOUT); //enable rf rx and rx first timeout irq
    irq_enable_type(FLD_IRQ_ZB_RT_EN); //enable RF irq
    irq_enable(); //enable general irq

    //start the SRX
    gen_fsk_srx_start(clock_time()+50*16, 0); //RX first timeout is disabled and the transceiver won't exit the RX state until a packet arrives

    while (1) {
        if (rx_flag) {
            rx_flag = 0;
            rx_payload = gen_fsk_rx_payload_get((unsigned char *)rx_packet, (unsigned char *)&rx_payload_len);
            rssi = (gen_fsk_rx_packet_rssi_get((unsigned char *)rx_packet) + 110);
            rx_timestamp = gen_fsk_rx_timestamp_get((unsigned char *)rx_packet);

            gpio_toggle(GREEN_LED_PIN);
            //start the SRX
            gen_fsk_srx_start(clock_time()+50*16, 350);
            gpio_write(TIME_DEBUG_PIN,1);
            gpio_write(TIME_DEBUG_PIN,0);
        }

        if (rx_first_timeout) {
            rx_first_timeout = 0;
            //start the SRX
            gen_fsk_srx_start(clock_time()+50*16, 350);
        }
    }
}
