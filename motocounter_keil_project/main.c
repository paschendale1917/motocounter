#include "main.h"


int main(void){
	hardware_init();
	menu_init(&main_menu_item_1);                                                       //инициализируем меню и устанавливаем указатель на первый пункт
	//display_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);                                  //отрисовываем начальное меню	
	
	while(1){
		
	switch(menustate){
			case MAINSCREEN:
				mainscreen_handler();	
				break;
			case  RESET:
			    reset_handler();
				break;
			case BACKLIGHT:
				backlight_handler();
				break;
			case ABOUT:
				about_handler();
				break;
			case FLASHLIGHT:
				flashlight_handler();
				break;
			case MOTOHOUR:
				motohour_handler();
				break;
			case RESETCOUNTER:
				resetcounter_handler();
				break;
			case DIVIDER:
				divider_handler();
				break;
			case VOLTFACTOR:
				voltfactor_handler();
				break;
			case REDCOMPONENT:
				redcomponent_handler();
				break;
			case GREENCOMPONENT:
				greencomponent_handler();
				break;
			case BLUECOMPONENT:
				bluecomponent_handler();
				break;
			default:
				menu_process();
				
				break;
						
		}
	//policecolor_breathe();
		
	}
}

	



