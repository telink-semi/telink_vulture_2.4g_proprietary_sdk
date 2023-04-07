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


#define 	DBG_DATA_LEN					16
#define 	DBG_DATA_NUM					16
#define 	SLAVE_DMA_MODE_ADDR_WRITE		0x4A000  //i2c master write data to  0x4A000
#define 	SLAVE_DMA_MODE_ADDR_READ		0x4A000  //i2c master read data from 0x4A000

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_CLOCK_HZ  	24000000
//write buff
volatile unsigned char i2c_master_tx_buff[DBG_DATA_LEN] = {0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
//read buff
volatile unsigned char i2c_master_rx_buff[DBG_DATA_LEN] = {0};
unsigned char master_rx_buff_debug[DBG_DATA_LEN *DBG_DATA_NUM];   //store i2c master reading data here to debug
unsigned int master_rx_index = 0;



void i2c_master_test_init(void)
{
	//I2C pin set
	i2c_gpio_set(I2C_GPIO_SDA_A3,I2C_GPIO_SCL_A4);// SDA/CK : A3/A4

	//slave device id 0x5C(write) 0x5D(read)
	//i2c clock 200K, only master need set i2c clock
	i2c_master_init(0x5c, (unsigned char)(CLOCK_SYS_CLOCK_HZ/(4*200000)) );
}
void user_init()
{
	WaitMs(2000);  //leave enough time for SWS_reset when power on

	i2c_master_test_init();
}


 int main (void)
{


	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

	clock_init(SYS_CLK_24M_Crystal);

	gpio_init(1);

	user_init();


	while(1){
		
		i2c_master_tx_buff[0] += 1;
		//slave mapping mode, no need send address information, so address length should be 0
		i2c_write_series(0, 0, (unsigned char*)i2c_master_tx_buff, DBG_DATA_LEN);
		i2c_read_series(0, 0, (unsigned char*)i2c_master_rx_buff, DBG_DATA_LEN);

		/*********** copy the data read by i2c master from slave for debug  ****************/
		memcpy((unsigned char*)(master_rx_buff_debug + master_rx_index*DBG_DATA_LEN), (unsigned char*)i2c_master_rx_buff, DBG_DATA_LEN);

		master_rx_index ++;
		if(master_rx_index>=DBG_DATA_NUM){
			master_rx_index = 0;
		}
		}
}




