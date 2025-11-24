#include "timer.h"
//#include "ADC.h"

uint8_t temperature=0;
uint16_t tick_count=0;
uint8_t endoftimer_flag=0;


Timer time={.sec=0,
			.min=0,
			.hour=0
			};

void init_timer(void){
	SET_BIT(RCC->APBENR2,RCC_APBENR2_TIM16EN);
	WRITE_REG(TIM16->PSC,F_TIM16/F_PRESCALED_TIM16-1);			          		// 16 000 000/ (15999+1) =1000√ц частота таймера
	WRITE_REG(TIM16->ARR,TIM16_AUTORELOAD);                                				//счет будет вестись до 1000√ц/1000(arr)= 1, затем event и прерывание(прерывание 1 раз в секунду)
	start_counter();
	init_pa6();
}

void TIM16_IRQHandler( void){
	if(TIM16->SR&TIM_SR_UIF){ //об€зательно нужно очищать бит дл€ регул€рных прерываний
		TIM16->SR&=~TIM_SR_UIF;
		time.sec++;
		if(time.sec>=60){
			time.sec=0;
			time.min++;
		}
		if(time.min>=60){
			time.min=0;
			time.hour++;
			tick_count=0;
		}
	}
	
}



void init_pa6(void){
	SET_BIT(RCC->IOPENR,RCC_IOPENR_GPIOAEN);
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE6,0x01<<GPIO_MODER_MODE6_Pos);
}



void stop_counter(void){
	TIM16_DisableCounter ;   				 
	TIM16_DisableInterruptUpdate; 
	NVIC_DisableIRQ(TIM16_IRQn);	
}

void start_counter(void){
	TIM16_EnableInterruptUpdate;
	TIM16_EnableCounter;
	NVIC_EnableIRQ(TIM16_IRQn);	
}