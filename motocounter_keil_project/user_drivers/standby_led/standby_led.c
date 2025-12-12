
#include "standby_led.h"

uint8_t standby_led_flag=0;

void init_led(void){
	SET_BIT(RCC->IOPENR,RCC_IOPENR_GPIOAEN);
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE6,0x02<<GPIO_MODER_MODE6_Pos);
	MODIFY_REG(GPIOA->AFR[0],GPIO_AFRL_AFSEL6,0x05<<GPIO_AFRL_AFSEL6_Pos);	

	
	//SET_BIT(TIM16->DIER,TIM_DIER_CC1IE);
	MODIFY_REG(TIM16->CCMR1,TIM_CCMR1_OC1M,0x06<<TIM_CCMR1_OC1M_Pos);
	SET_BIT(TIM16->CCER,TIM_CCER_CC1E);
	SET_BIT(TIM16->BDTR,TIM_BDTR_MOE);
	set_led(1);
}

void set_led(uint8_t bright){
	//if(bright>100)return;
	WRITE_REG(TIM16->CCR1,bright);
}

void standby_led_polling(uint16_t counter){
	static uint16_t led_cntr=0;
	static uint16_t led_flag=0;
	switch(standby_led_flag){
		case 1:
			if(!(counter%10)){ 
				switch(led_flag){
					case 0:
						WRITE_REG(TIM16->CCR1,led_cntr++);
						led_cntr>=100?led_flag=1:0;
						break;	
					case 1:
						WRITE_REG(TIM16->CCR1,led_cntr--);
						!led_cntr?led_flag=0:0;
						break;
				}
			}
		case 0:
			break;
	}
}