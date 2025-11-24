//счетчик секунд на таймере TIM16

#ifndef timer_H_
#define timer_H_


#include "stm32g0xx.h"
#include "../buzzer/buzzer.h"

extern 	uint16_t tick_count;
extern uint8_t endoftimer_flag;



#define TIM16_EnableCounter    			    		SET_BIT(TIM16->CR1, TIM_CR1_CEN )  //макрос включения таймера
#define TIM16_DisableCounter    				  	CLEAR_BIT(TIM16->CR1, TIM_CR1_CEN )  //макрос выключения таймера
#define TIM16_EnableInterruptUpdate	   	  			SET_BIT(TIM16->DIER,TIM_DIER_UIE) // макрос разрешения обновления прерываний
#define TIM16_DisableInterruptUpdate	   			CLEAR_BIT(TIM16->DIER,TIM_DIER_UIE) // макрос разрешения обновления прерываний


#define F_TIM16 									64000000 
#define F_PRESCALED_TIM16  							1000
#define TIM16_AUTORELOAD                   			1000

typedef struct  {
volatile	uint8_t sec;
volatile	uint8_t min;
volatile	uint16_t hour;
}Timer;

extern  Timer time;
extern uint8_t temperature;

void init_timer(void);
void init_pa6(void);
void stop_counter(void);
void start_counter(void);





#endif 