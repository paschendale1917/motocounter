#ifndef timer_H_
#define timer_H_

#include "stm32g0xx.h"
#include "../standby_led/standby_led.h"

extern 	uint16_t tick_count;

#define TIM16_EnableCounter    			    		SET_BIT(TIM16->CR1, TIM_CR1_CEN )  //макрос включени€ таймера
#define TIM16_DisableCounter    				  	CLEAR_BIT(TIM16->CR1, TIM_CR1_CEN )  //макрос выключени€ таймера
#define TIM16_EnableInterruptUpdate	   	  			SET_BIT(TIM16->DIER,TIM_DIER_UIE) // макрос разрешени€ обновлени€ прерываний
#define TIM16_DisableInterruptUpdate	   			CLEAR_BIT(TIM16->DIER,TIM_DIER_UIE) // макрос разрешени€ обновлени€ прерываний


#define F_TIM16 									64000000 
#define F_PRESCALED_TIM16  							100000
#define TIM16_AUTORELOAD                   			100                //не отсчитываю секунду сразу дл€ того, чтобы можно было пошимить на ножке PA6 с приемлемой частотой

typedef struct  {
volatile	uint8_t sec;
volatile	uint8_t min;
volatile	uint16_t hour;
}Timer;

extern  Timer time;

void init_timer(void);
void init_pa6(void);
void stop_counter(void);
void start_counter(void);

#endif 