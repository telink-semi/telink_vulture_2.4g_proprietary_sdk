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
#include "common.h"

#define CLOCK_SYS_CLOCK_HZ  	24000000

#define rec_buff_Len    16
#define trans_buff_Len    16
volatile unsigned int cnt=0;
volatile unsigned char uart_rx_flag=0;

volatile unsigned int  uart_ndmairq_cnt=0;
volatile unsigned char uart_ndmairq_index=0;

#define rec_buff_Len    16
#define trans_buff_Len  16
extern __attribute__((aligned(4))) unsigned char rec_buff[rec_buff_Len];
extern __attribute__((aligned(4))) unsigned char trans_buff[trans_buff_Len];

__attribute__((aligned(4))) unsigned char rec_buff[rec_buff_Len]={0};
__attribute__((aligned(4))) unsigned char trans_buff[trans_buff_Len] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, \
                                                                        0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x00};




_attribute_ram_code_sec_noinline_ void irq_handler(void)
{


    static unsigned char uart_ndma_irqsrc;
    uart_ndma_irqsrc = uart_ndmairq_get();  ///get the status of uart irq.
    if(uart_ndma_irqsrc){

    //cycle the four registers 0x90 0x91 0x92 0x93,in addition reading will clear the irq.
        if(uart_rx_flag==0){
            rec_buff[uart_ndmairq_cnt++] = uart_ndma_read_byte();
            if(uart_ndmairq_cnt%16==0&&uart_ndmairq_cnt!=0){
                uart_rx_flag=1;
            }
        }
        else{
        	 unsigned char uart_fifo_cnt =0;
			 uart_fifo_cnt= reg_uart_buf_cnt&FLD_UART_RX_BUF_CNT;
			 if(uart_fifo_cnt !=0)
			 {
				 for(unsigned char j=0;j<uart_fifo_cnt;j++){
					 uart_ndma_read_byte();
				 }
			 }
        }
    }
    if(uart_is_parity_error()){
       	 uart_reset();
       	 uart_ndma_clear_rx_index();
       	 uart_ndmairq_cnt=0;
       	 uart_rx_flag=0;
       	 for(int i=0;i<rec_buff_Len;i++)
   		 {
       		 rec_buff[i]=0;
   		 }

    }
}



void user_init()
{
    WaitMs(2000);  //leave enough time for SWS_reset when power on

    //note: dma addr must be set first before any other uart initialization! (confirmed by sihui)
   // uart_recbuff_init( (unsigned short *)&rec_buff, sizeof(rec_buff));

    uart_gpio_set(UART_TX_PA2, UART_RX_PD6);// uart tx/rx pin set

    uart_reset();  //will reset uart digital registers from 0x90 ~ 0x9f, so uart setting must set after this reset

    uart_init_baudrate(115200,CLOCK_SYS_CLOCK_HZ,PARITY_NONE, STOP_BIT_ONE);


    uart_dma_enable(0, 0);

    irq_disable_type(FLD_IRQ_DMA_EN);

    dma_chn_irq_enable(FLD_DMA_CHN_UART_RX | FLD_DMA_CHN_UART_TX, 0);

    uart_irq_enable(1,0);   //uart RX irq enable

    uart_ndma_irq_triglevel(1,0);   //set the trig level. 1 indicate one byte will occur interrupt

    uart_mask_error_irq_enable();// open uart_error_mask,when stop bit error or parity error,it will enter error_interrupt.

}


 int main (void)
{


	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    gpio_init(1);

    user_init();

    irq_enable();

    while (1) {

    	WaitMs(1000);

        for(unsigned char i=0;i<trans_buff_Len;i++){
            uart_ndma_send_byte(trans_buff[i]);
        }
        if(uart_rx_flag>0){
            uart_ndmairq_cnt=0; //Clear uart_ndmairq_cnt
            uart_rx_flag=0;
            for(unsigned char i=0;i<trans_buff_Len;i++){
                uart_ndma_send_byte(rec_buff[i]);
            }
        }
    }
}
