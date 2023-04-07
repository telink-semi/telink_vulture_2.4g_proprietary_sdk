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

//write buff
volatile unsigned char spi_master_tx_buff[DBG_DATA_LEN]={0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,0x99,0xaa,0xbb,0xcc,0xdd,0xee,0xff};
//read buff
volatile unsigned char spi_master_rx_buff[DBG_DATA_LEN]={0x00};

void spi_master_test_init(void)
{
	//spi clock 500K, only master need set i2c clock
	spi_master_init((unsigned char)(CLOCK_SYS_CLOCK_HZ/(2*500000)-1),SPI_MODE0);          //div_clock. spi_clk = sys_clk/((div_clk+1)*2),mode select

	spi_master_gpio_set(GPIO_PA4,GPIO_PD6,GPIO_PA2,GPIO_PA3);

}

void user_init()
{

	WaitMs(2000);  //leave enough time for SWS_reset when power on

	spi_master_test_init();


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

		WaitMs(1000);   //1S
		spi_master_tx_buff[0] += 1;
		spi_master_tx_buff[0] &= 0xff;

	    spi_write(slaveRegAddr_WriteCMD, 4,(unsigned char*)spi_master_tx_buff, DBG_DATA_LEN,SPI_CS_PIN);// pls refer to the datasheet for the write and read format of spi.
		spi_read( slaveRegAddr_ReadCMD , 4,(unsigned char*)spi_master_rx_buff,DBG_DATA_LEN,SPI_CS_PIN);
	}
}





