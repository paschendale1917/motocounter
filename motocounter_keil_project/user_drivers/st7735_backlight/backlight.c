//PA4 таймер 14
#include "backlight.h"

int8_t brightness=50;

void set_backlight(void){
	SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE4,0x01<<GPIO_MODER_MODE4_Pos);
	CLEAR_BIT(GPIOA->OTYPER,GPIO_OTYPER_OT4);
	MODIFY_REG(GPIOA->OSPEEDR,GPIO_OSPEEDR_OSPEED4,0x03<<GPIO_OSPEEDR_OSPEED4_Pos);
	SET_BIT(GPIOA->BSRR,GPIO_BSRR_BS4);	
	
}

void init_backlight(void){
	SET_BIT(RCC->IOPENR, RCC_IOPENR_GPIOAEN);
	SET_BIT(RCC->APBENR2,RCC_APBENR2_TIM14EN);
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE4,0x02<<GPIO_MODER_MODE4_Pos);
	MODIFY_REG(GPIOA->PUPDR,GPIO_PUPDR_PUPD4,0x00<<GPIO_PUPDR_PUPD4_Pos);
	GPIOA->AFR[0]|=GPIO_AFRL_AFSEL4_2;																			//[0]==AFRL  [1]==AFRH
	
	MODIFY_REG(TIM14->CCMR1,TIM_CCMR1_OC1M,0x07<<TIM_CCMR1_OC1M_Pos); 											//pwm mode2
	SET_BIT(TIM14->CCMR1,TIM_CCMR1_OC1PE);																		//Output compare 1 preload enable
	SET_BIT(TIM14->CR1,TIM_CR1_ARPE); 																			//Auto-reload preload enable
	SET_BIT(TIM14->EGR,TIM_EGR_UG);
	WRITE_REG(TIM14->PSC,0);          																			//prescaler делит частоту                      
	WRITE_REG(TIM14->ARR,F_TIM14/F_PWM14-1); 																	//F_TIM 32 000 000/freq_pwm -1=32000000/1000Hz -1=31999  
	//WRITE_REG(TIM14->CCR1,0);																					//ccr=((arr+1)*duty_cycle)-1=((31999+1)*0.2)-1)=6399 регистр сравнения
	SET_BIT(TIM14->CCER,TIM_CCER_CC1E);																			//Capture/Compare 1 output enable
	SET_BIT(TIM14->CR1,TIM_CR1_CEN);																			//enable counter
	
}

uint8_t set_brigtness(uint8_t bright){
	if(bright<=100){
	WRITE_REG(TIM14->CCR1,bright*(F_TIM14/F_PWM14-1)/100);
	}
	return 0;
}


void backlightpwm_off(void){
	CLEAR_BIT(TIM14->CCER,TIM_CCER_CC1E);	
	CLEAR_BIT(TIM14->CR1,TIM_CR1_CEN);
	CLEAR_BIT(RCC->APBENR2,RCC_APBENR2_TIM14EN);
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE4,0x00<<GPIO_MODER_MODE4_Pos);
	MODIFY_REG(GPIOA->PUPDR,GPIO_PUPDR_PUPD4,0x00<<GPIO_PUPDR_PUPD4_Pos);
	
}

void backlight_off(void){
	
SET_BIT(GPIOA->BSRR,GPIO_BSRR_BR4);		
	
}