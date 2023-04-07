#include "driver.h"

/////////////////// Clock  /////////////////////////////////
#define CLOCK_SYS_CLOCK_HZ      24000000   //  24m
/* List tick per second/millisecond/microsecond */
enum{
	CLOCK_SYS_CLOCK_1S = CLOCK_SYS_CLOCK_HZ,				///< system tick per 1 second
	CLOCK_SYS_CLOCK_1MS = (CLOCK_SYS_CLOCK_1S / 1000),		///< system tick per 1 millisecond
	CLOCK_SYS_CLOCK_1US = (CLOCK_SYS_CLOCK_1S / 1000000),   ///< system tick per 1 microsecond
};

#define PWM_PIN		            GPIO_PA2
#define AS_PWMx			        AS_PWM0
#define PWM_ID					PWM0_ID
#define PWM_PULSE_NUM			4

#define TX_GROUP_NUM			6
volatile unsigned char cnt=0;

_attribute_ram_code_sec_noinline_ void irq_handler(void)
{
	if(pwm_get_interrupt_status(PWM_IRQ_PWM0_PNUM)){
		pwm_clear_interrupt_status(PWM_IRQ_PWM0_PNUM);
		cnt++;
        if(cnt==(TX_GROUP_NUM-1))
		{
        	pwm_set_mode(PWM_ID, PWM_COUNT_MODE);
		}
	}

}

void user_init()
{
	sleep_ms(2000);
	pwm_set_clk(CLOCK_SYS_CLOCK_HZ, CLOCK_SYS_CLOCK_HZ);

	gpio_set_func(PWM_PIN, AS_PWMx);
	pwm_set_mode(PWM_ID, PWM_IR_MODE);
	pwm_set_pulse_num(PWM_ID,PWM_PULSE_NUM);
	pwm_set_cycle_and_duty(PWM_ID, 1000 * CLOCK_SYS_CLOCK_1US, 500 * CLOCK_SYS_CLOCK_1US);

	pwm_set_interrupt_enable(PWM_IRQ_PWM0_PNUM);
	irq_set_mask(FLD_IRQ_SW_PWM_EN);
	irq_enable();

	pwm_start(PWM_ID);
#if (TX_GROUP_NUM == 1)
	pwm_set_mode(PWM_ID, PWM_COUNT_MODE);
#endif
}

int  main(void)
{
	cpu_wakeup_init(LDO_MODE, EXTERNAL_XTAL_24M);

	user_read_flash_value_calib();

	clock_init(SYS_CLK_24M_Crystal);

    gpio_init(0);

	user_init();

	while(1)
	{
		sleep_ms(50);
	}

}
