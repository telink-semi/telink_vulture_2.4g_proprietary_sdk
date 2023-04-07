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


#define    GREEN_LED_PIN        GPIO_PD3
#define    ADC_INPUT_PIN        GPIO_PB0
#define    DEEP_DURATION        998 //unit: ms

_attribute_session_(".retention_data") volatile static unsigned int rfid_packet_cnt = 0;

_attribute_ram_code_sec_noinline_ static void gpio_high_z_config(void)
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

 int main (void)
{
    unsigned char rfid_payload[8];
    unsigned short battery_volt;

    //system and clock init
	blc_pm_select_external_32k_crystal();
//	blc_pm_select_internal_32k_crystal();


	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    //blink the led
    gpio_set_func(GREEN_LED_PIN, AS_GPIO);
    gpio_set_output_en(GREEN_LED_PIN, 1);
    gpio_write(GREEN_LED_PIN, 1);

    //do battery volt sample
    adc_init();
    adc_vbat_init(ADC_INPUT_PIN);
    adc_power_on_sar_adc(1);
    battery_volt = adc_sample_and_get_result();

    //prepare the packet payload
    rfid_payload[0] = battery_volt & 0xff;
    rfid_payload[1] = (battery_volt>>8) & 0xff;
    rfid_payload[2] = rfid_packet_cnt & 0xff;
    rfid_payload[3] = (rfid_packet_cnt>>8) & 0xff;
    rfid_payload[4] = (rfid_packet_cnt>>16) & 0xff;
    rfid_payload[5] = (rfid_packet_cnt>>24) & 0xff;
    rfid_payload[6] = 0xaa;
    rfid_payload[7] = 0xbb;

    rfid_packet_cnt++;

    //config radio and send data packet
    rfid_radio_config();
    rfid_packet_send(rfid_payload, sizeof(rfid_payload));

    //goto deep sleep
    gpio_high_z_config(); //set all unused GPIOs during deep sleep as high_Z state, avoiding current leakage
    cpu_sleep_wakeup(DEEPSLEEP_MODE_RET_SRAM_LOW32K, PM_WAKEUP_TIMER, ClockTime()+DEEP_DURATION*1000*16);

    while (1) {
        
    }
}
