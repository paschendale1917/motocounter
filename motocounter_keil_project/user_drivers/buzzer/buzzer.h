#ifndef buzzer_H_
#define buzzer_H_

#include "stm32g0xx.h"
#include "../SysTick/systick_delay.h"
//#include "timer.h"
//#include "ws2812_dma.h"

#define F_BUZ_TIM 							64000000 					//зависит от настроек в секции RCC
#define F_BUZ_PWM 							2000						//частота ШИМ 4 КГц(резонансная частота буззера)
#define BUZ_TIM_PSC 						0   						//делитель частоты таймeра

#define BEEP_TIME							30

#define BUZ_1    		 					1000
#define BUZ_0 								0
extern uint32_t buz_long;

extern uint16_t beep_array[];

void init_buzzer(void);
void deinit_gpio_buzzer(void);
void beep(uint16_t delay);
//void beep_beep(void);
void beep_beep( uint16_t repetition);
void beep_dma(void);
void end_of_timer(void);
void beep_on(void);
void beep_off(void);
#endif