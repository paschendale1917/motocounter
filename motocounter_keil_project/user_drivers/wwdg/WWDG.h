//счет идет вниз от задаваемого значения(T[6:0]) до 0x3f(63) 
//перезагружать таймер можно только тогда, когда таймер дотикал до второй устанавливаемой границы W[6:0]>0x3f(63)
//один тик таймера 64000000/4096/divider=64000000/4096/4=3906.25 Hz; 1/3906.25=0.000256 c=0.256 мс
//время, когда нельзя перезагружать (UPPER_VALUE-WINDOW_VALUE)*0.256=(127-117)*0.256 =2.56 мс после предыдущей перезагрузки
//время, когда мк будет перезагружен таймером (UPPER_VALUE-63)*0.256=(127-63)*0.256=16.384 мс после предыдущей перезагрузки
//промежуток времени, когда мне нужно обновить значение в T[6:0], чтобы таймер не ресетнул мк 16.384-2.56=13.824 мс


#ifndef WWDG_H
#define WWDG_H

#include "stm32g0xx.h"
#include <math.h>

#define F_APB1						 64000000UL 	//смотрим в секции RCC
#define WWDG_TIMEOUT                 12 			//устанавливаем задержку в мс
#define CLOCK_PERIOD                 1000/F_APB1 	//ms

#define UPPER_VALUE					 127        	//значение в T[6:0], от него начнем считать
#define WINDOW_VALUE				 117			//значение в W[6:0], если перезагрузить счетчик ранее, чем таймер досчитает до него, то произойдет ресет
                     
#define NON_DIV                      4096          	//неконтролируемый делитель частоты(жестко в железе)

#define WWDG_DIV1                    0x00			//задаваемые делители
#define WWDG_DIV2                    0x01
#define WWDG_DIV4                    0x02
#define WWDG_DIV8                    0x03			//pow(2,3)
#define WWDG_DIV16                   0x04			//pow(2,4)
#define WWDG_DIV32                   0x05			
#define WWDG_DIV64                   0x06
#define WWDG_DIV128                  0x07

#define SET_WWDG_DIV(divider)        MODIFY_REG(WWDG->CFR,WWDG_CFR_WDGTB,divider<<WWDG_CFR_WDGTB_Pos)   
#define F_WWDG(divider)         	 F_APB1/(NON_DIV*pow(2,divider)) //частота wwdg с учетом выбранного делителя и некотрорлируемого делителя
#define SET_UPPER_VALUE(divider)	 MODIFY_REG(WWDG->CR,WWDG_CR_T,WWDG_TIMEOUT/(CLOCK_PERIOD*F_WWDG(divider)<<WWDG_CR_T_Pos)//значение в T[6:0], от него начнем считать
#define ENABLE_WWDG        			 SET_BIT(WWDG->CR,WWDG_CR_WDGA)

void init_wwdg(void);
void WWDG_reload(void);
#endif
