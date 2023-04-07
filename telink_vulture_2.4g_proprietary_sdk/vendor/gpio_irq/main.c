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

#define GREEN_LED_PIN     		GPIO_PD3
#define IRQ_PIN           		GPIO_PD0
#define IRQ_KEY           		GPIO_PB4
#define GPIO_IRQ				1
#define GPIO_IRQ_KEY			2//SW2
#define GPIO_IRQ_RSIC0			3
#define GPIO_IRQ_RSIC1			4
#define GPIO_HIGH_RESISTOR		5

#define GPIO_MODE 				GPIO_IRQ

volatile unsigned int gpio_irq_cnt;
volatile unsigned int gpio_set_irq_cnt;
volatile unsigned char gpio_irq_flag = 0;

_attribute_ram_code_sec_noinline_ void irq_handler(void)
{
#if (GPIO_MODE == GPIO_IRQ )
    unsigned int irq_src = irq_get_src();
	if (irq_src & FLD_IRQ_GPIO_EN)
	{
		reg_irq_src |= FLD_IRQ_GPIO_EN; // clear the relevant irq
	    if (0 == gpio_read(IRQ_PIN))
	    {
	        WaitMs(10);
	        if (0 == gpio_read(IRQ_PIN))
	        {
	            while (0 == gpio_read(IRQ_PIN));
	            gpio_irq_cnt++;
	            gpio_toggle(GREEN_LED_PIN);
	        }
	    }
	}
#elif (GPIO_MODE == GPIO_IRQ_KEY )
    unsigned int irq_src = irq_get_src();
	if (irq_src & FLD_IRQ_GPIO_EN)
	{
		reg_irq_src |= FLD_IRQ_GPIO_EN; // clear the relevant irq
	    if (0 == gpio_read(IRQ_KEY))
	    {
	        WaitMs(10);
	        if (0 == gpio_read(IRQ_KEY))
	        {
	            while (0 == gpio_read(IRQ_KEY));
	            gpio_irq_cnt++;
	            gpio_toggle(GREEN_LED_PIN);
	        }
	    }
	}
#elif(GPIO_MODE == GPIO_IRQ_RSIC0)

	if((reg_irq_src & FLD_IRQ_GPIO_RISC0_EN)==FLD_IRQ_GPIO_RISC0_EN){
		reg_irq_src |= FLD_IRQ_GPIO_RISC0_EN; // clear the relevant irq
		gpio_irq_cnt++;
		gpio_toggle(GREEN_LED_PIN);
	}
#elif(GPIO_MODE == GPIO_IRQ_RSIC1)

	if((reg_irq_src & FLD_IRQ_GPIO_RISC1_EN)==FLD_IRQ_GPIO_RISC1_EN){
		reg_irq_src |= FLD_IRQ_GPIO_RISC1_EN; // clear the relevant irq
		gpio_irq_cnt++;
		gpio_toggle(GREEN_LED_PIN);

	}
#endif
}


void gpio_test_init(void)
{
	/*
		 * Button matrix table:
		 * 			PB4  	PB5
		 * 	PB2	    SW2		SW3
		 * 	PB3   	SW4		SW5
		 */
		gpio_set_func(GPIO_PB2 ,AS_GPIO);
		gpio_set_output_en(GPIO_PB2, 1); 		//enable output
		gpio_set_input_en(GPIO_PB2 ,0);			//disable input
		gpio_write(GPIO_PB2, 0);

		//2.init the KEY1 pin,for trigger interrupt
		gpio_set_func(IRQ_KEY ,AS_GPIO);
		gpio_set_output_en(IRQ_KEY, 0); 			// disable output
		gpio_set_input_en(IRQ_KEY ,1);				// enable input

		gpio_set_func(GREEN_LED_PIN, AS_GPIO);
		gpio_set_output_en(GREEN_LED_PIN, 1);         //enable output
		gpio_set_input_en(GREEN_LED_PIN, 0);          //disable input
		gpio_write(GREEN_LED_PIN, 1);                 //LED On

		/****  IRQ pin link PD1, PD1 produces a square wave.  **/
		gpio_set_func(GPIO_PD1 ,AS_GPIO);
		gpio_set_output_en(GPIO_PD1, 1); 		//enable output
		gpio_set_input_en(GPIO_PD1 ,0);			//disable input
		gpio_write(GPIO_PD1, 0);

		//2.init the IRQ pin, for trigger interrupt
		gpio_set_func(IRQ_PIN ,AS_GPIO);
		gpio_set_output_en(IRQ_PIN, 0); 			// disable output
		gpio_set_input_en(IRQ_PIN ,1);				// enable input
}

void user_init(void)
{
#if(GPIO_MODE == GPIO_HIGH_RESISTOR)
	gpio_shutdown(GPIO_ALL);				//set all gpio as high resistor except sws and mspi
#else

	gpio_test_init();

#if (GPIO_MODE == GPIO_IRQ )
	gpio_setup_up_down_resistor(IRQ_PIN, PM_PIN_PULLUP_10K);
	/****GPIO_IRQ POL_FALLING   Press SW2 to connect KEY1 and KEY3 to trigger an interrupt. **/
	gpio_set_interrupt(IRQ_PIN, POL_FALLING);	//When SW2 is pressed, the falling edge triggers the interrupt.
	irq_enable();
#elif (GPIO_MODE == GPIO_IRQ_KEY )
	gpio_setup_up_down_resistor(IRQ_KEY, PM_PIN_PULLUP_10K);
	/****GPIO_IRQ POL_FALLING   Press SW2 to connect KEY1 and KEY3 to trigger an interrupt. **/
	gpio_set_interrupt(IRQ_KEY, POL_FALLING);	//When SW2 is pressed, the falling edge triggers the interrupt.
	irq_enable();
#elif(GPIO_MODE == GPIO_IRQ_RSIC0)
	gpio_setup_up_down_resistor(IRQ_PIN, PM_PIN_PULLDOWN_100K);
	/****GPIO_IRQ_RSIC0  POL_RISING   toggle PA0 to trigger an interrupt. **/
	gpio_set_interrupt_risc0(IRQ_PIN, POL_RISING);	//When switching PA0, the rising edge triggers an interrupt.
	irq_enable();

#elif(GPIO_MODE == GPIO_IRQ_RSIC1)
	gpio_setup_up_down_resistor(IRQ_PIN, PM_PIN_PULLUP_10K);
	/****GPIO_IRQ_RSIC1  POL_FALLING   toggle PA0 to trigger an interrupt. **/
	gpio_set_interrupt_risc1(IRQ_PIN, POL_FALLING);	//When switching PA0, the falling edge triggers an interrupt.
	irq_enable();
#endif
#endif
}

int main(void)
{
	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);

    user_init();

    while (1)
    {
	   sleep_ms(10);
	   gpio_toggle(GPIO_PD1);
    }
	return 0;
}

