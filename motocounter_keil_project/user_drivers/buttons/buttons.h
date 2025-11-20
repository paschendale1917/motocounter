#ifndef buttons_H_
#define buttons_H_

#include "stm32g0xx.h"

extern  uint8_t  shortpress_left,
								 shortpress_right,
								 shortpress_ok,
								 longpress_left,
								 longpress_right,
								 longpress_ok;	

extern uint8_t flagPressed;
extern uint16_t cnt_temp;
extern uint16_t cnt_delay;

typedef struct  {
	uint8_t sec;
	uint8_t min;
	uint16_t hour;
}timer;

extern  timer tim;



#define TIM1_PRESCALE                     6399													//делитель /частота 64000000:(6399+1)=10000 тиков в секунду - частота, на которой будет работать таймер
#define TIM1_AUTORELOAD                   100  											   //значение, до которoго будет считать счетчик;здесь 10000/100=100 прерываний в секунду

#define TIM_EnableCounter(TIMx)    			  TIMx->CR1 |= TIM_CR1_CEN   //макрос включения таймера
#define TIM_EnableInterruptUpdate(TIMx) 	TIMx->DIER |= TIM_DIER_UIE // макрос разрешения обновления прерываний
#define TIM3_Prescaler(TIMx)              WRITE_REG(TIMx->PSC,TIM1_PRESCALE)
#define TIM3_Arr(TIMx)                    WRITE_REG(TIMx->ARR,TIM1_AUTORELOAD)



#define SHORT_TIMEOUT 29
#define LONG_TIMEOUT  30

//настройки для микроменю

#define PRESSED 	1    
#define UNPRESSED 0

#define NUM_BUTTONS 6     //количество кнопок      

typedef  enum{
  BUTTON_LEFT    		=  0 ,
  BUTTON_RESET    			=  1 ,
  BUTTON_MENUITEMBACK   =  2 ,
  BUTTON_RIGHT   	 	=  3 ,
  BUTTON_SELECT   		=  4 ,
  BUTTON_ENTERMENU 		=  5 ,
  BUTTON_NOTHING  		=  255 
} Button_TypeDef;


void buttons_init(void);
uint8_t readButtonState(void);
void resetButton(void);
void _delay_ms_tim1(uint16_t ms);

#endif /* buttons_H__ */