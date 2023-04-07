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

#include "common.h"
#include "driver.h"

#define GREEN_LED_PIN           GPIO_PD3
#define RED_LED_PIN     		GPIO_PD5
#define CLOCK_SYS_CLOCK_HZ  	24000000

#define rec_buff_Len    16
#define trans_buff_Len    16

volatile unsigned int cnt=0;
volatile unsigned char uart_rx_flag=0;
volatile unsigned char uart_dmairq_tx_cnt=0;
volatile unsigned char uart_dmairq_rx_cnt=0;
volatile unsigned char uart_dmairq_err_cnt;
volatile unsigned char uart_dma_send_flag = 1;
volatile unsigned char uart_dma_rcv_flag = 0;

#define UART_DATA_LEN    12      //data max ?    (UART_DATA_LEN+4) must 16 byte aligned
typedef struct{
    unsigned int dma_len;        // dma len must be 4 byte
    unsigned char data[UART_DATA_LEN];
}uart_data_t;

uart_data_t rec_buff = {0,  {0, } };
uart_data_t trans_buff = {6,   {0x00,0xbb,0xcc,0xdd,0xee,0xff, } };


_attribute_ram_code_sec_noinline_ void irq_handler(void)
{
	if(reg_uart_status1 & FLD_UART_TX_DONE){

		uart_dmairq_tx_cnt++;
		uart_dma_send_flag = 1;
		gpio_toggle(GREEN_LED_PIN);
		uart_clr_tx_done();
	}
    // UART irq
    if(dma_chn_irq_status_get() & FLD_DMA_CHN_UART_RX){
        dma_chn_irq_status_clr(FLD_DMA_CHN_UART_RX);
        uart_dmairq_rx_cnt++;
        uart_dma_rcv_flag = 1;
        //Received uart data in rec_buff, user can copy data here

    }
    if(uart_is_parity_error()){//when stop bit error or parity error.
    		uart_clear_parity_error();
    		gpio_write(RED_LED_PIN,1);
    		uart_dmairq_err_cnt++;
    	}


}



void user_init()
{
    WaitMs(2000);  //leave enough time for SWS_reset when power on
    //note: dma addr must be set first before any other uart initialization! (confirmed by sihui)
    uart_recbuff_init( (unsigned char *)&rec_buff, sizeof(rec_buff));

    uart_gpio_set(UART_TX_PA2, UART_RX_PD6);// uart tx/rx pin set

    uart_reset();  //will reset uart digital registers from 0x90 ~ 0x9f, so uart setting must set after this reset

    uart_init_baudrate(115200,CLOCK_SYS_CLOCK_HZ,PARITY_NONE, STOP_BIT_ONE);

    uart_dma_enable(1, 1);     //uart data in hardware buffer moved by dma, so we need enable them first

    irq_set_mask(FLD_IRQ_DMA_EN);

    dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 1);       //uart Rx/Tx dma irq enable

    uart_mask_tx_done_irq_enable();

    uart_mask_error_irq_enable();// open uart_error_mask,when stop bit error or parity error,it will enter error_interrupt.

    irq_enable_type(FLD_IRQ_UART_EN);// uart_tx use uart_txdone irq

}



 int main (void)   //must on ramcode
{
	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    gpio_init(1);

    user_init();

    irq_enable();

    while (1) {
    	if(uart_dma_rcv_flag == 1)
    	{
    	uart_dma_send( (unsigned char*)&rec_buff);
    	uart_dma_rcv_flag = 0;
    	}
    	WaitMs(300);
    	if(uart_dma_send_flag == 1)
		{
		uart_send_dma((unsigned char *)&trans_buff);
		trans_buff.data[0]++;
		uart_dma_send_flag = 0;
		}
    	WaitMs(300);
    }
}
