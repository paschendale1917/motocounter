#ifndef backlight_H

#include "stm32g0xx.h"

#define F_TIM14 									32000000 						//зависит от настроек в секции RCC
#define F_PWM14 									1000 							//частота Ў»ћ 1000 √ц
#define TIM14_PSC 									1000   							//делитель частоты таймeра

extern int8_t brightness;

extern volatile uint8_t *bright_pt;
extern volatile uint8_t bright_level;
void set_backlight(void);                                                           //тупо включить  подсветку без регулировки €ркости
void init_backlight(void);
uint8_t set_brigtness(uint8_t bright); 												//€ркость в процентах
void backlightpwm_off(void);
void backlight_off(void);															//тупо выключить подсветку 
#endif