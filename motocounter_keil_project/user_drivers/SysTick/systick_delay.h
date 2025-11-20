// для работы в теле int main (void)  прописать SysTick_init();
#ifndef systick_delay_H_
#define systick_delay_H_

#define F_CPU 64000000UL //частота микроконтроллера
#include "stm32g0xx.h"

extern volatile uint32_t system_tick;

void SysTick_init(void);
void _delay_ms(uint32_t);
void _delay_us(uint32_t us);
void _delay_tact(uint32_t);
void SysTick_Handler(void); //обработчик прерывания

#endif /* systick_delay_H_ */