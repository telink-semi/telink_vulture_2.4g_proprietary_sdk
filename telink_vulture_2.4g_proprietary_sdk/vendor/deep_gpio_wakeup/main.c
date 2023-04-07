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

#define    WAKEUP_PIN          GPIO_PB4
#define    KEY2_GND_PIN        GPIO_PB2
#define    GREEN_LED_PIN       GPIO_PD3
#define    WHITE_LED_PIN       GPIO_PD4
#define MAX_TIMES                       4

#define DEEP_PAD_WAKEUP		 	        1
#define DEEP_RET_PAD_WAKEUP     		2

#define PM_MODE			     			DEEP_RET_PAD_WAKEUP

_attribute_session_(".retention_data") volatile static unsigned char deep_retention_times = 0;

_attribute_session_(".ram_code") static void gpio_high_z_config(void)
{
    //output disable
    reg_gpio_pa_oen = 0xff;
    reg_gpio_pb_oen = 0xff;
    reg_gpio_pc_oen = 0xff;
    reg_gpio_pd_oen = 0xff;

    //digital output set as 0
    reg_gpio_pa_out = 0x00;
    reg_gpio_pb_out = 0x00;
    reg_gpio_pc_out = 0x00;
    reg_gpio_pd_out = 0x00;

    //input disable
    reg_gpio_pa_ie = 0x00;
    reg_gpio_pb_ie = 0x00;

    analog_write(areg_gpio_pc_ie, 0);
    reg_gpio_pd_ie = 0x00;
}

void debug_io_init(void)
{
	//key related pins config
	    gpio_set_output_en(WAKEUP_PIN, 0); //disable output
	    gpio_set_input_en(WAKEUP_PIN, 0); //disable input
	    gpio_setup_up_down_resistor(WAKEUP_PIN, PM_PIN_PULLUP_1M); //enable internal 1M pull-up
	    cpu_set_gpio_wakeup(WAKEUP_PIN, Level_Low, 1); //config low-level wakeup

	    gpio_set_output_en(KEY2_GND_PIN, 0); //disable output
	    gpio_set_input_en(KEY2_GND_PIN, 0); //disable input
	    gpio_setup_up_down_resistor(KEY2_GND_PIN, PM_PIN_PULLDOWN_100K); //enable internal 100k pull-down

	    //led pin config
	    gpio_set_func(GREEN_LED_PIN, AS_GPIO);
	    gpio_set_output_en(GREEN_LED_PIN, 1);
	    gpio_write(GREEN_LED_PIN, 1);
	    WaitMs(3000);
}
int main(void)
{
	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    debug_io_init();

    if(deep_retention_times == MAX_TIMES)
	{
		gpio_set_func(WHITE_LED_PIN, AS_GPIO);
		gpio_set_output_en(WHITE_LED_PIN, 1);
		gpio_write(WHITE_LED_PIN, 1);
		while(1);
	}

    gpio_high_z_config();//set all GPIO as high_Z state, avoiding current leakage

    deep_retention_times++;

#if(PM_MODE == DEEP_PAD_WAKEUP)
    cpu_sleep_wakeup(DEEPSLEEP_MODE, PM_WAKEUP_PAD, 0);
#elif(PM_MODE == DEEP_RET_PAD_WAKEUP)
    cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW16K, PM_WAKEUP_PAD, 0);
#endif

    while (1) {
    
    }
}
