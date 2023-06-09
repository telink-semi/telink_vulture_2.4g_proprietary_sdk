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
#include "fw_update_phy.h"
#include "fw_update.h"



#define FW_UPDATE_FW_VERSION            0x0000
#define GPIO_IRQ_PIN                    GPIO_PB4
#define KEY2_GND_PIN                    GPIO_PB2

#define BLUE_LED_PIN                    GPIO_PD2
#define GREEN_LED_PIN                   GPIO_PD3
#define WHITE_LED_PIN                   GPIO_PD4
#define RED_LED_PIN                     GPIO_PD5
#define GPIO_VBAT_DETECT                GPIO_PB7

#define BATT_CHECK_ENABLE               1
#define VBAT_ALRAM_THRES_MV             2000
#define Flash_Addr                      0x08
#define Flash_Buff_Len                  1

volatile unsigned char Flash_Read_Buff[Flash_Buff_Len]={0};

volatile unsigned char FW_UPDATE_SlaveTrig = 0;
unsigned int fw_current_boot_addr = 0;
unsigned long firmwareVersion = 0;

#if(BATT_CHECK_ENABLE)
static unsigned char  battery_power_check()
{
    volatile unsigned char i;
    volatile unsigned short sample_result = 0;
    adc_init();
    adc_vbat_init(GPIO_VBAT_DETECT);
    adc_power_on_sar_adc(1);
    WaitMs(1);
    for(i = 0;i < 4;i ++)
    {
        sample_result = adc_sample_and_get_result();
        if(sample_result < VBAT_ALRAM_THRES_MV)
        {
            return 0;
        }
    }
    return 1;
}
#endif

void sys_init(void)
{
	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    gpio_set_func(GPIO_IRQ_PIN, AS_GPIO);
    gpio_set_input_en(GPIO_IRQ_PIN, 1);
    gpio_setup_up_down_resistor(GPIO_IRQ_PIN, PM_PIN_PULLUP_1M);
    gpio_set_interrupt(GPIO_IRQ_PIN, pol_falling);

    gpio_set_output_en(KEY2_GND_PIN, 0); //disable output
	gpio_set_input_en(KEY2_GND_PIN, 0); //disable input
	gpio_setup_up_down_resistor(KEY2_GND_PIN, PM_PIN_PULLDOWN_100K); //enable internal 100k pull-down

	//enable the irq
	gpio_set_interrupt(GPIO_IRQ_PIN, POL_FALLING);
	irq_enable_type(FLD_IRQ_GPIO_EN);
	irq_enable();
}

void main(void)
{
    sys_init();
    gpio_set_func(BLUE_LED_PIN|GREEN_LED_PIN|WHITE_LED_PIN|RED_LED_PIN, AS_GPIO);
    gpio_set_output_en(BLUE_LED_PIN|GREEN_LED_PIN|WHITE_LED_PIN|RED_LED_PIN, 1); //enable output
    gpio_set_input_en(BLUE_LED_PIN|GREEN_LED_PIN|WHITE_LED_PIN|RED_LED_PIN, 0); //disable input
    gpio_write(BLUE_LED_PIN|GREEN_LED_PIN|WHITE_LED_PIN|RED_LED_PIN, 0); //LED Off
    while (1) {
        if (FW_UPDATE_SlaveTrig) {
            FW_UPDATE_SlaveTrig = 0;
            gpio_toggle(BLUE_LED_PIN);
            WaitMs(100);
            gpio_toggle(BLUE_LED_PIN);
            WaitMs(100);
            gpio_toggle(BLUE_LED_PIN);
			WaitMs(100);
			gpio_toggle(BLUE_LED_PIN);
			WaitMs(100);

        #if(BATT_CHECK_ENABLE)
            if(!battery_power_check())
            {
                while(1)
                {
                     gpio_toggle(RED_LED_PIN);
                     WaitMs(50);
                }
            }
        #endif

            FW_UPDATE_PHY_Init(FW_UPDATE_RxIrq);

            flash_read_page(Flash_Addr,Flash_Buff_Len,(unsigned char *)Flash_Read_Buff);
            if(Flash_Read_Buff[0]==0x4b)
            {
                FW_UPDATE_SlaveInit(FW_UPDATE_SLAVE_BIN_ADDR, FW_UPDATE_FW_VERSION);
            }
            else
            {
                FW_UPDATE_SlaveInit(0, FW_UPDATE_FW_VERSION);
            }
            while (1) {
                FW_UPDATE_SlaveStart();
                ev_process_timer();
            }
        }
        gpio_toggle(WHITE_LED_PIN);
		WaitMs(1000);
    }
}




