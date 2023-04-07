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
#include "rf_id.h"

#define    IO_DEBUG_EN          0
#define    DEBUG_PIN            GPIO_PA2
#define    DEEP_DURATION        1000 //unit: ms

_attribute_ram_code_sec_noinline_ static void gpio_high_z_config(void)
{
    //output disable
    reg_gpio_pa_oen = 0xff;
    reg_gpio_pb_oen = 0xff;
    reg_gpio_pc_oen = 0xff;
#if IO_DEBUG_EN
    reg_gpio_pd_oen = 0xfd;
#else
    reg_gpio_pd_oen = 0xff;
#endif

    //digital output set as 0
    reg_gpio_pa_out = 0x00;
    reg_gpio_pb_out = 0x00;
    reg_gpio_pc_out = 0x00;
#if IO_DEBUG_EN    
    reg_gpio_pd_out = 0x02;
#else
    reg_gpio_pd_out = 0x00;
#endif

    //input disable
    reg_gpio_pa_ie = 0x00;


    reg_gpio_pb_ie = 0x00;


    analog_write(areg_gpio_pc_ie, 0);
    reg_gpio_pd_ie = 0x00;
}

__attribute__((section(".retention_data"))) volatile unsigned char deep_wkup_flg = 0;

 int main(void)
{
    //system and clock init
	blc_pm_select_external_32k_crystal();
//	blc_pm_select_internal_32k_crystal();


	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    gpio_high_z_config(); //set all unused GPIOs during deep sleep as high_Z state, avoiding current leakage

    //config radio and send data packet
    rfid_radio_config();
    rfid_packet_rx();
    
#if CLK_32K_XTAL_EN
    //goto deep sleep
    if (deep_wkup_flg != 1) {
        deep_wkup_flg = 1;
        clock_32k_init(CLK_32K_XTAL);
    }
#endif




    //goto deep sleep
    cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_TIMER, ClockTime()+DEEP_DURATION*1000*16);

    while (1) {
        
    }
}
