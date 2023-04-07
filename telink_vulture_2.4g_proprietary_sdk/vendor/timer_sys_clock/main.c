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

/* Define system clock */
#define CLOCK_SYS_CLOCK_HZ  	24000000		// define system clock


/* List tick per second/millisecond/microsecond */
enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,				///< system tick per 1 second
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),		///< system tick per 1 millisecond
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),   ///< system tick per 1 microsecond
};


#define    GREEN_LED_PIN      GPIO_PD3
#define    WHITE_LED_PIN      GPIO_PD4
#define    RED_LED_PIN        GPIO_PD5

volatile unsigned char timer0_expire_flg = 0;
volatile unsigned char timer1_expire_flg = 0;
volatile unsigned char timer2_expire_flg = 0;

unsigned int timer0_irq_cnt = 0;
unsigned int timer1_irq_cnt = 0;
unsigned int timer2_irq_cnt = 0;


_attribute_ram_code_sec_noinline_ void irq_handler(void)
{
//	if(reg_tmr_sta & FLD_TMR_STA_TMR0){
//		reg_tmr_sta = FLD_TMR_STA_TMR0; //clear irq status
//		timer0_irq_cnt ++;
//		timer0_expire_flg = 1;
//
//	}
//	if(reg_tmr_sta & FLD_TMR_STA_TMR1){
//		reg_tmr_sta = FLD_TMR_STA_TMR1; //clear irq status
//		timer1_irq_cnt ++;
//		timer1_expire_flg = 1;
//
//	}

	if(reg_tmr_sta & FLD_TMR_STA_TMR2){
		reg_tmr_sta = FLD_TMR_STA_TMR2; //clear irq status
		timer2_irq_cnt ++;;
		timer2_expire_flg = 1;

	}


}


void user_init()
{
	WaitMs(1000);  //leave enough time for SWS_reset when power on




	timer2_set_mode(TIMER_MODE_SYSCLK,0,1000 * CLOCK_SYS_CLOCK_1MS);
	timer_start(TIMER2);


	irq_enable();

}



 int main (void)   //must on ramcode
{


	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

	clock_init(SYS_CLK_24M_Crystal);

	gpio_init(1);

	user_init();

	gpio_set_output_en(GREEN_LED_PIN|WHITE_LED_PIN|RED_LED_PIN, 1); //enable output
	gpio_set_input_en(GREEN_LED_PIN|WHITE_LED_PIN|RED_LED_PIN, 0); //disable input
	gpio_write(GREEN_LED_PIN|WHITE_LED_PIN|RED_LED_PIN, 0); //LED Off

	while (1) {

		if (timer0_expire_flg) {
			timer0_expire_flg = 0;
			gpio_toggle(GREEN_LED_PIN);
		}
		if (timer1_expire_flg) {
			timer1_expire_flg = 0;
			gpio_toggle(WHITE_LED_PIN);
		}
		if (timer2_expire_flg) {
			timer2_expire_flg = 0;
			gpio_toggle(RED_LED_PIN);
		}
	}
}





