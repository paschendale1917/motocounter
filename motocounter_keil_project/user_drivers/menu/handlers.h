#ifndef HANDLERS_H_
#define HANDLERS_H_

#include <stdbool.h>
#include "../st7735/st7735.h"
#include "../ADC/ADC.h"
#include "menu.h"
#include "../buttons/buttons.h"
#include "../buzzer/buzzer.h"
#include "../timer/timer.h"
#include "../eeprom/eeprom.h"
#include "../ws2812/ws2812_dma.h"


#define MENU_PROCESS			0xFF
#define MAINSCREEN				0x00
#define RESET                  	0x01
#define BACKLIGHT				0x02
#define ABOUT          			0x03
#define FLASHLIGHT              0x04
#define MOTOHOUR				0x05
#define RESETCOUNTER			0x06
#define DIVIDER					0x07
#define VOLTFACTOR				0x08
#define REDCOMPONENT			0x09
#define GREENCOMPONENT			0x0A
#define BLUECOMPONENT			0x0B

#define MOTOHOURS_CELL  		0x02
#define HOUR_CELL				0x04
#define MIN_CELL				0x06
#define SEC_CELL				0x07
#define BACKLIGHT_CELL     		0x08
#define DIVIDER_CELL    		0x09
#define VOF_CELL    			0x0B
#define REDCOMPONENT_CELL		0x0C
#define GREENCOMPONENT_CELL		0x0D
#define BLUECOMPONENT_CELL		0x0E
#define FIRSTFLAG_CELL  		0x32
#define CELLWEAR_CELL   		0xFA



extern uint8_t menustate;				//в этой переменной будем сохранять текущее состояние меню
extern uint8_t clear_display_flag;		//флаг очистки дисплея. при переходе в обработчик нужно очистить дисплей единожды. будем делать по флагу

extern uint16_t motohours;

void mainscreen_handler(void);		    //баня по-черному, где испытывается всяческое(оставить потом для примеров использования)
void enc_process(void);					//обработка средств ввода(конкретно здесь:вращение ручки энкодера, длинное и короткое нажатия кнопки энкодера и отдельной кнопки)
void hardware_init(void);				//инициализация "железа", используемого в проекте
void entermenu(void);
void menu_process(void);
void about_handler(void);
void backlight_handler(void);
void reset_handler(void);
void init_after_hardreset(void);
void flashlight_handler(void);
void motohour_handler(void);
void resetcounter_handler(void);
void time_tracker(void);
void startup_settings(void);
void divider_handler(void);
void voltfactor_handler(void);
void redcomponent_handler(void);
void greencomponent_handler(void);
void bluecomponent_handler(void);
#endif /* HANDLERS_H_ */ 