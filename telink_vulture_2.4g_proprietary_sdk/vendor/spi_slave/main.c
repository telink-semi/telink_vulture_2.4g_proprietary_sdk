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


#define SLAVE_REGADDR1  0x04
#define SLAVE_REGADDR2  0x80
#define SLAVE_REGADDR3  0x00
#define SPI_READ_CMD  	0x80// spi read command
#define SPI_WRITE_CMD 	0x00// spi write command
#define DBG_DATA_LEN    16

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_CLOCK_HZ  	24000000

#define     SPI_CS_PIN				        GPIO_PD6//SPI CS pin

unsigned char slaveRegAddr_WriteCMD[] = {SLAVE_REGADDR1,SLAVE_REGADDR2,SLAVE_REGADDR3,SPI_WRITE_CMD};
unsigned char slaveRegAddr_ReadCMD[]  = {SLAVE_REGADDR1,SLAVE_REGADDR2,SLAVE_REGADDR3,SPI_READ_CMD};


unsigned int spi_read_cnt = 0;

unsigned char  irq_status = 0;
unsigned char  irq_status1 = 0;

_attribute_ram_code_sec_noinline_ void irq_handler(void)
{

	irq_status = irq_get_src();
	irq_status1 = reg_i2c_map_host_status;//spi slave can distinguish the operation host write or read.
	if(irq_status & FLD_IRQ_HOST_CMD_EN)
	{
		if(irq_status1 & FLD_HOST_CMD_IRQ) //both host write & read trigger this status
		{

			reg_i2c_map_host_status = irq_status1; //clear all irq status
			spi_read_cnt ++;  //for debug


		}
	}
	irq_clr_src2(FLD_IRQ_HOST_CMD_EN);
}



void spi_slave_test_init(void)
{
	spi_slave_init((unsigned char)(CLOCK_SYS_CLOCK_HZ/(2*500000)-1),SPI_MODE0);           //slave mode init




	spi_slave_gpio_set(GPIO_PA4,GPIO_PD6,GPIO_PA2,GPIO_PA3);

}


void user_init()
{

	WaitMs(2000);  //leave enough time for SWS_reset when power on
	//SPI:CK/CN/DO/DI   A4/D6/A2/A3, D7/D2/B7/B6
	spi_slave_test_init();

	reg_spi_slave_irq_status = (FLD_HOST_CMD_IRQ | FLD_HOST_READ_IRQ);  //clean irq status
	reg_irq_mask |= FLD_IRQ_HOST_CMD_EN;   //enable i2c irq

	//irq_enable();
}



_attribute_ram_code_sec_noinline_ int main (void)
{


	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

	clock_init(SYS_CLK_24M_Crystal);

	gpio_init(1);

	user_init();

	irq_enable();

	while (1) {


	}
}





