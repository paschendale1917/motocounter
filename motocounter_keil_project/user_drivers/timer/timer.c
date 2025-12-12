#include "timer.h"

uint16_t tick_count=0;

Timer time={.sec=0,
			.min=0,
			.hour=0
			};
		
void init_timer(void){
	SET_BIT(RCC->APBENR2,RCC_APBENR2_TIM16EN);
	WRITE_REG(TIM16->PSC,F_TIM16/F_PRESCALED_TIM16-1);			          		// 16 000 000/ (15999+1) =1000√ц частота таймера
	WRITE_REG(TIM16->ARR,TIM16_AUTORELOAD);                                				//счет будет вестись до 1000√ц/1000(arr)= 1, затем event и прерывание(прерывание 1 раз в секунду)
	start_counter();

}



void TIM16_IRQHandler( void){
	static uint16_t local_cntr=0;
	if(TIM16->SR&TIM_SR_UIF){ //об€зательно нужно очищать бит дл€ регул€рных прерываний
		TIM16->SR&=~TIM_SR_UIF;
		local_cntr++;
		if(local_cntr==1000){
			time.sec++;
			local_cntr=0;
		}
		if(time.sec>=60){
			time.sec=0;
			time.min++;
		}
		if(time.min>=60){
			time.min=0;
			time.hour++;
			tick_count=0;
		}
		standby_led_polling(local_cntr);
	}
}



void stop_counter(void){
	STDBLED_OFF;
	TIM16_DisableCounter ;   				 
	TIM16_DisableInterruptUpdate; 
	NVIC_DisableIRQ(TIM16_IRQn);	
}

void start_counter(void){
	TIM16_EnableInterruptUpdate;
	TIM16_EnableCounter;
	NVIC_EnableIRQ(TIM16_IRQn);	
}