#include "driver.h"

#define    GREEN_LED_PIN       GPIO_PD3
#define    BLUE_LED_PIN        GPIO_PD2

unsigned int tick_suspend_interval = 0;
int main(void)
{
	blc_pm_select_internal_32k_crystal();

	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

    clock_init(SYS_CLK_24M_Crystal);
    //init led
	gpio_set_func(GREEN_LED_PIN ,AS_GPIO);
	gpio_set_output_en(GREEN_LED_PIN, 1); 		//enable output
	gpio_set_input_en(GREEN_LED_PIN ,0);			//disable input
	gpio_write(GREEN_LED_PIN, 0);              	//LED On
	gpio_set_func(BLUE_LED_PIN,AS_GPIO);
	gpio_set_output_en(BLUE_LED_PIN, 1); 		//enable output
	gpio_set_input_en(BLUE_LED_PIN,0);			//disable input
	gpio_write(BLUE_LED_PIN, 0);              	//LED On

	usb_set_pin_en();
	pm_set_suspend_power_cfg(PM_POWER_USB, 1);
    while (1) {
        if(clock_time_exceed (tick_suspend_interval, 300000))
        	{
        		gpio_toggle(BLUE_LED_PIN);
        		cpu_sleep_wakeup (SUSPEND_MODE, PM_WAKEUP_CORE, 0);
        		tick_suspend_interval = clock_time()|1;
        	}

    	sleep_ms(100);
    	gpio_toggle(GREEN_LED_PIN);
    }
}


