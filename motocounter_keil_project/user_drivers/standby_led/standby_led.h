#ifndef stanby_led_H_
#define stanby_led_H_


#include "../timer/timer.h"

#define STDBLED_OFF               WRITE_REG(TIM16->CCR1,0)

extern uint8_t standby_led_flag;
void init_led(void);
void set_led(uint8_t bright);
void standby_led_polling(uint16_t counter);


#endif
