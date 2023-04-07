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

#define 	DBG_DATA_LEN					16
#define 	DBG_DATA_NUM					16


/*************** i2c slave data buffer ****************/
//mapping mode, write buffer is the first half 64 byte, read buffer is last half 64 byte
volatile unsigned char slave_mapping_buff[128] = {0};
unsigned char slave_rx_buff_debug[DBG_DATA_LEN *DBG_DATA_NUM];   //store i2c master writing data here to debug
unsigned int slave_rx_index = 0;


#define IRQ_IIC_CLEAR       0
#define IRQ_IIC_READ  		BIT(0)  //read
#define IRQ_IIC_WRITE		BIT(1)  //write

unsigned int i2c_irq_flag = IRQ_IIC_CLEAR;


unsigned int i2c_read_cnt = 0;   //for debug
unsigned int i2c_write_cnt = 0;  //for debug


_attribute_ram_code_sec_noinline_ void irq_handler(void)
{

	unsigned char  irq_status = reg_i2c_map_host_status;//i2c slave can distinguish the operation host write or read.

	if(irq_status & FLD_HOST_CMD_IRQ){  //both host write & read trigger this status

		reg_i2c_map_host_status = irq_status; //clear all irq status

		if(irq_status & FLD_HOST_READ_IRQ){  //host read

			i2c_read_cnt ++;  //for debug


			i2c_irq_flag |= IRQ_IIC_READ;
		}
		else{  //host write

			i2c_write_cnt ++;  //for debug


			i2c_irq_flag |= IRQ_IIC_WRITE;


			/*********** copy the data written by i2c master to slave_rx_buff_debug for debug ****************/

			memcpy( (unsigned char*)(slave_rx_buff_debug + slave_rx_index*DBG_DATA_LEN), (unsigned char*)slave_mapping_buff, DBG_DATA_LEN);


			slave_rx_index ++;
			if(slave_rx_index>=DBG_DATA_NUM){
				slave_rx_index = 0;
			}
			/******************************** end ****************************************************/
		}



	}


}
void i2c_slave_test_init(void)
{





	i2c_gpio_set(I2C_GPIO_SDA_A3,I2C_GPIO_SCL_A4);// SDA/CK : A3/A4



	//slave device id 0x5C(write) 0x5D(read)
	//i2c slave mapping mode, master no need send any address when reading/writing,
	//for i2c master: writing data buffer is slave_mapping_buff,
	//and reading data buffer is (slave_mapping_buff+64) (this offset 64 is managed by MCU hardware, user can not change it)
	i2c_slave_init(0x5C, I2C_SLAVE_MAP, (unsigned char *)slave_mapping_buff);

	//slave_mapping_buff + 64 is master reading data buffer in mapping mode, put some data here for master read

	memset(&slave_mapping_buff[64],0x55,DBG_DATA_LEN);

	reg_i2c_map_host_status = (FLD_HOST_CMD_IRQ | FLD_HOST_READ_IRQ);  //clean irq status

	reg_irq_mask |= FLD_IRQ_HOST_CMD_EN;   //enable i2c irq

	irq_enable(); //enable system irq

}


void user_init()
{

	WaitMs(2000);  //leave enough time for SWS_reset when power on

	i2c_slave_test_init();



}



 int main (void)
{


	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

	clock_init(SYS_CLK_24M_Crystal);

	gpio_init(1);

	user_init();
    //LED pin config
    gpio_set_func(GREEN_LED_PIN, AS_GPIO);
    gpio_set_output_en(GREEN_LED_PIN, 1);
    gpio_write(GREEN_LED_PIN, 0);


	while(1)
	{
		WaitMs(50);
		gpio_toggle(GREEN_LED_PIN);
	}

}


















