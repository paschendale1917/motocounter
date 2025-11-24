#include "handlers.h"

uint8_t menustate = MAINSCREEN;
uint8_t mainscreen_flag=1;
uint8_t clear_display_flag=1;

uint8_t firstinit[]={0,250,0,0,0,0,50,0,151,125,255,0,0}; 	//массив для первоначального инита //2bytes motohours,2bytes hours,1byte min, 1byte sec,1byte bright,2bytes divider,1byte vof,1byte red,1byte green,1byte blue
uint8_t firstinit_flag=0;
uint8_t emergency_saving_flag=0;
volatile uint8_t redcomponent=0xFF;
volatile uint8_t greencomponent=0;
volatile uint8_t bluecomponent=0;
volatile uint32_t color=0xFF0000; 					//по умолчанию красный

uint16_t motohours=0;
float vof=12.5;										//напряжение включения счетчика моточасов

void hardware_init(void){
	set_sysclk64();
	SysTick_init();
	eeprom_init(EE24C02_ADDR);
	startup_settings();
    st7735_init(brightness);
	init_buzzer();
	init_timer();
	init_adc_dma();
	buttons_init();
	background(BACKGROUND_COLOR); 
	init_after_hardreset();
	init_ws2812();
	start_effect();
	init_wwdg();
}

void startup_settings(void){
	motohours=eeprom_read_uint16(EE24C02_ADDR,MOTOHOURS_CELL);
	time.hour=eeprom_read_uint16(EE24C02_ADDR,HOUR_CELL);
	time.min=eeprom_read_byte(EE24C02_ADDR,MIN_CELL);
	time.sec=eeprom_read_byte(EE24C02_ADDR,SEC_CELL);
	brightness=eeprom_read_byte(EE24C02_ADDR,BACKLIGHT_CELL);
	divider=(float)(eeprom_read_uint16(EE24C02_ADDR, DIVIDER_CELL))/1000;
	vof=(float)(eeprom_read_byte(EE24C02_ADDR,VOF_CELL))/10;
	redcomponent=eeprom_read_byte(EE24C02_ADDR,REDCOMPONENT_CELL);
	greencomponent=eeprom_read_byte(EE24C02_ADDR,GREENCOMPONENT_CELL);
	bluecomponent=eeprom_read_byte(EE24C02_ADDR,BLUECOMPONENT_CELL);
	color&=~0xffffffff;
	color|=(redcomponent<<16)|(greencomponent<<8)|bluecomponent;
}

	
void menu_process(void){
	switch(readButtonState()){
		case BUTTON_LEFT:
			menu_navigate_prev(); 											//если поворот слайдера влево, то в указатель пункта меню пишем адрес структуры, описывающей предыдущий пункт меню
			pointer_clear_flag=1; 											//флаг используется для того, чтобы указатель перерисовывался не постоянно, тратя ресурсы мк, а только тогда, когда происходит поворот ручки энкодера
			clear_display_flag=1;
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		case BUTTON_RIGHT:
			menu_navigate_next();
			pointer_clear_flag=1;
			//clear_display_flag=1;
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		case BUTTON_RESET:
		case BUTTON_ENTERMENU:
			beep(BEEP_TIME);
			resetButton();
			break ;
					
		case BUTTON_MENUITEMBACK:
			enter_upmenu();	
			beep(BEEP_TIME);
			//clear_display_flag=1;
			resetButton();
			break ;
				
		case BUTTON_SELECT:
			clear_display_flag=1;
			execute_menu_action();											//запуск обработчика функции 
			enter_submenu();												//в current_menu адрес дочернего пункта меню
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		default:
			display_pointer(pointer);                                      //постоянно отслеживаем необходимость отрисовывать указатель, так как он у нас подвижный элемент
			break ;														   //меню обновляется только при переходе с одного уровня на другой
	}
}

void entermenu(void){
	mainscreen_flag=0;
	menu_init(&main_menu_item_1);                                                       //инициализируем меню и устанавливаем указатель на первый пункт
	display_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);                                  //отрисовываем начальное меню
	beep(BEEP_TIME);
}


void return_from_handler(void){
	menustate=MENU_PROCESS;												//пишем в переменную статуса меню любое значение, которое не отслеживает switch  в бесконечном цикле
	clear_display();													//очищаем дисплей от экрана обработчика
	display_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);					//выводим текущее меню
	draw_pointer(current_menu);											//отрисовываем указатель заново
	clear_display_flag=1;												//установка флага требующейся очистки дисплея от меню необходима для того, чтобы было возможно вторичное вхождение в обработчик без проблем с очисткой экрана
	beep(BEEP_TIME);
	resetButton();														//обязательно сбросить флаг нажатой кнопки, иначе он будет висеть и кнопка окажется недееспособной
}

void draw_counterhours(void){
	draw_char(140,60,'h',BACKGROUND_COLOR,GREEN,BigFont);
	if(motohours<1000&&motohours>100){
	draw_number(100,60,motohours,-3,BACKGROUND_COLOR,GREEN,BigFont);
	}; 
	if(motohours<100){
		draw_number(113,60,motohours,-3,BACKGROUND_COLOR,GREEN,BigFont);
	};
	if(motohours==1000){
		draw_number(86,60,motohours,-3,BACKGROUND_COLOR,GREEN,BigFont);
	}
}

void mainscreen_handler(void){
	menustate=MAINSCREEN;
	switch(readButtonState()){
		case BUTTON_LEFT:
		case BUTTON_RIGHT:
		case BUTTON_RESET:
		case BUTTON_MENUITEMBACK:
		case BUTTON_SELECT:
			resetButton();
			break;
		case BUTTON_ENTERMENU:											 //в обработчике ожидаем нжатие кнопки возврата, если оно происходит, то
			leds_off();
			return_from_handler();										 //вызываем функцию возврата к меню
			break;
		default:
			if(clear_display_flag){
				resetButton();											//отрисуем экран в память дисплея единожды
				start_counter();
				clear_display();
				//draw_border(GBLUE);
				draw_hline(4,56,ST7735_WIDTH,GBLUE);
				//draw_rect( 0, 56, ST7735_WIDTH,1,DARKBLUE);
				clear_display_flag=0;
			}
			
			draw_clocks(4,4,time.hour,4,BACKGROUND_COLOR,GBLUE,DotMatrix_XL_Num);
			draw_minutes(120,4,time.min,2,BACKGROUND_COLOR,GBLUE,DotMatrix_M_Slash);
			draw_minutes(120,30,time.sec,2,BACKGROUND_COLOR,GBLUE,DotMatrix_M_Slash);
			//get_voltage_dma();
			draw_voltage(0,60,-3,BACKGROUND_COLOR,ORANGE,BigFont);
			draw_counterhours();
			if(eeprom_error){
				draw_string(60,0,"ee_err",0,BACKGROUND_COLOR,RED,TinyFont);
			}
			time_tracker();
			break;
	}
}

void backlight_handler(void){
	menustate=BACKLIGHT;
	char buf[20]="";
	if(clear_display_flag){													//очистить нужно единожды, поэтому был введен флаг очистки дисплея
		clear_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);
		draw_border(ORANGE);
		draw_string(30,5,"BRIGHT",1,BACKGROUND_COLOR,GRAYBLUE,DotMatrix_M_Slash);
		clear_display_flag=0;
	}
	switch(readButtonState()){
		case BUTTON_SELECT:
		case BUTTON_ENTERMENU:
		case BUTTON_RESET:
			resetButton();
			break ;
		case BUTTON_MENUITEMBACK:
			eeprom_write_byte(EE24C02_ADDR,BACKLIGHT_CELL,brightness);
			//beep(BEEP_TIME);
			return_from_handler();
			break ;
		case BUTTON_LEFT:
			brightness-=5;
			brightness<1?brightness=1:0;
			//draw_number(60,50,motohours,1,BACKGROUND_COLOR,RED,DotMatrix_M_Slash);
			set_brigtness(brightness);
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		case BUTTON_RIGHT:
			if(brightness==1){
				brightness+=4;
			}else{
				brightness+=5;
			}
			brightness>100?brightness=100:0;
			//draw_number(60,50,motohours,1,BACKGROUND_COLOR,RED,DotMatrix_M_Slash);
			set_brigtness(brightness);
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		default:
			sprintf(buf,"%u%%",brightness);
			draw_string(60,50,buf,1,BACKGROUND_COLOR,RED,DotMatrix_M_Slash);
			brightness<10?draw_rect(60+DotMatrix_M_Slash[0]*2+2,50,DotMatrix_M_Slash[0],DotMatrix_M_Slash[1],BACKGROUND_COLOR):0;
			brightness<100&&brightness>=10?draw_rect(60+DotMatrix_M_Slash[0]*3+3,50,DotMatrix_M_Slash[0],DotMatrix_M_Slash[1],BACKGROUND_COLOR):0;
			break ;
		}
	
}

void hardreset(void){
	firstinit_flag=0;
	eeprom_write_byte(EE24C02_ADDR, FIRSTFLAG_CELL,firstinit_flag);
	_delay_ms(2);
	NVIC_SystemReset();//перезагрузка
}

void init_after_hardreset(void){
	firstinit_flag=eeprom_read_byte(EE24C02_ADDR,FIRSTFLAG_CELL);
	if(firstinit_flag==0){
		eeprom_write_some_bytes(EE24C02_ADDR,MOTOHOURS_CELL,sizeof(firstinit),firstinit);
		_delay_ms(2);
		firstinit_flag=1;
		eeprom_write_byte(EE24C02_ADDR, FIRSTFLAG_CELL,firstinit_flag);
		_delay_ms(2);
	}
}

void reset_handler(void){
	menustate=RESET;
	if(clear_display_flag){													//очистить нужно единожды, поэтому был введен флаг очистки дисплея
		clear_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);
		clear_display_flag=0;
	}
	draw_border(MAGENTA);
	draw_string(35,10,"SHURE?",1,BACKGROUND_COLOR,RED,DotMatrix_M_Slash);
	draw_string(15,35,"CLICK LONG RIGHT",TINYFONT_SPACE,BACKGROUND_COLOR,RED,TinyFont);
	draw_string(15,45,"TO RESET OR",TINYFONT_SPACE,BACKGROUND_COLOR,RED,TinyFont);
	draw_string(15,55,"LONG LEFT TO EXIT",TINYFONT_SPACE,BACKGROUND_COLOR,RED,TinyFont);
	//draw_string(0,70,"cell wear",TINYFONT_SPACE,BACKGROUND_COLOR,GRAYBLUE,TinyFont);
	//draw_number(85,70,cell_wear,TINYFONT_SPACE,BACKGROUND_COLOR,GRAYBLUE,TinyFont);
	switch(readButtonState()){
		case BUTTON_MENUITEMBACK:
			return_from_handler();
			break ;
		case BUTTON_SELECT:
		case BUTTON_LEFT:	
		case BUTTON_RIGHT:
		case BUTTON_ENTERMENU:
			resetButton();
			break ;
		case BUTTON_RESET:
			hardreset();
			break;
		default:
			break ;
		}
}

void flashlight_handler(void){
	menustate=FLASHLIGHT;
	if(clear_display_flag){													
		clear_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);
		clear_display_flag=0;
	}
	background(WHITE);
	set_brigtness(100);
	setcolor(WHITE_LED);
	send_data_leds();
	switch(readButtonState()){
		case BUTTON_MENUITEMBACK:
			leds_off();
			background(BACKGROUND_COLOR);
			set_brigtness(brightness=eeprom_read_byte(EE24C02_ADDR,BACKLIGHT_CELL));
			return_from_handler();
			break ;
		case BUTTON_SELECT:
		case BUTTON_LEFT:	
		case BUTTON_RIGHT:
		case BUTTON_ENTERMENU:
		case BUTTON_RESET:
			resetButton();
			break;
		default:
			break ;
		}
}

void about_handler(void){
	menustate=ABOUT;
	switch(readButtonState()){
		case BUTTON_LEFT:
		case BUTTON_RIGHT:
		case BUTTON_RESET:
		case BUTTON_ENTERMENU:
		case BUTTON_SELECT:
			resetButton();
			break;
		case BUTTON_MENUITEMBACK:												//в обработчике ожидаем нжатие кнопки возврата, если оно происходит, то
			return_from_handler();												//вызываем функцию возврата к меню
			break;
		default:
			if(clear_display_flag){
				resetButton();													//отрисуем экран в память дисплея единожды
				clear_display();
				draw_image(3, 53,80, 24, mipt);
				draw_string(4,6,"Alexander Biryukov",0,BACKGROUND_COLOR,DARKBLUE,TinyFont);
				draw_string(4,18,"bisiro@mail.ru",0,BACKGROUND_COLOR,DARKBLUE,TinyFont);
				draw_string(4,30,"TG: Paschendale",0,BACKGROUND_COLOR,DARKBLUE,TinyFont);
				draw_string(4,42,"Bryansk,2025",0,BACKGROUND_COLOR,DARKBLUE,TinyFont);
				draw_border(ORANGE);
				clear_display_flag=0;
			}
			break;
	}

	    
}

void motohour_handler(void){
	menustate=MOTOHOUR;
	static char buf[10]="";
	if(clear_display_flag){													
		clear_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);
		draw_border(ORANGE);
		draw_string(4,5,"SET HOURS",1,BACKGROUND_COLOR,GRAYBLUE,DotMatrix_M_Slash);
		clear_display_flag=0;
	}

	switch(readButtonState()){
		case BUTTON_SELECT:
		case BUTTON_RESET:
		case BUTTON_ENTERMENU:
			resetButton();
			break ;
		case BUTTON_MENUITEMBACK:
			eeprom_write_uint16(EE24C02_ADDR,MOTOHOURS_CELL,motohours);
			return_from_handler();
			break ;
		case BUTTON_LEFT:
			if(motohours==999){
				motohours-=9;
			}else{
			  motohours-=10;
			 }
			!motohours?motohours=999:0;
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		case BUTTON_RIGHT:
			if(motohours==990){
				motohours+=9;
			}else{
			  motohours+=10;
			 }
			motohours>999?motohours=0:0;
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		default:
			sprintf(buf,"%3u",motohours);
			draw_string(45,50,buf,1,BACKGROUND_COLOR,RED,DotMatrix_M_Slash);
			break ;
		}
	
}

void resetcounter_handler(void){
	menustate=RESETCOUNTER;
	if(clear_display_flag){
		resetButton();													
		clear_display();
		draw_border(MAGENTA);
		draw_string(35,10,"SHURE?",1,BACKGROUND_COLOR,RED,DotMatrix_M_Slash);
		draw_string(15,35,"CLICK LONG RIGHT",TINYFONT_SPACE,BACKGROUND_COLOR,RED,TinyFont);
		draw_string(15,45,"TO RESET OR",TINYFONT_SPACE,BACKGROUND_COLOR,RED,TinyFont);
		draw_string(15,55,"LONG LEFT TO EXIT",TINYFONT_SPACE,BACKGROUND_COLOR,RED,TinyFont);
		clear_display_flag=0;
	}
	switch(readButtonState()){
		case BUTTON_RESET:
			clear_display();
			draw_border(GREEN);
			draw_string(45,10,"DONE!",SYSTEMFONT_SPACE,BACKGROUND_COLOR,GREEN,DotMatrix_M_Slash);
			time=(Timer){0};
			uint8_t c[4]={time.hour>>8,(uint8_t)time.hour,time.min,time.sec};
			eeprom_write_some_bytes(EE24C02_ADDR,HOUR_CELL,4,c);
			_delay_ms(500);
			return_from_handler();
			break;
		case BUTTON_LEFT:
		case BUTTON_RIGHT:
		case BUTTON_ENTERMENU:
		case BUTTON_SELECT:
			resetButton();
			break;
		case BUTTON_MENUITEMBACK:																//в обработчике ожидаем нжатие кнопки возврата, если оно происходит, то
			return_from_handler();																//вызываем функцию возврата к меню
			break;
		default:
			break;
	}
}

void time_tracker(void){
//пищим, когда время счетчика сравнялось с уставкой временного пробега	
	if(voltage>vof){		
		if(time.hour>=motohours&&time.sec%2==0){
			beep_on();
			setcolor(color);
			send_data_leds();
   } else {
	   beep_off();
	   leds_off();
	}
}

 if(voltage<vof){
	 stop_counter();
	 beep_off();
	 leds_off();
	 if(!emergency_saving_flag){
		uint8_t c[4]={time.hour>>8,(uint8_t) time.hour,time.min,time.sec};
		eeprom_write_some_bytes(EE24C02_ADDR,HOUR_CELL,4,c);
		_delay_ms(2);
		//cell_wear++;
		//uint8_t cellwear[]={(uint8_t)cell_wear>>24,(uint8_t)cell_wear>>16,(uint8_t)cell_wear>>8,(uint8_t)cell_wear};
		//eeprom_write_some_bytes(EE24C02_ADDR,CELLWEAR_CELL,4,cellwear);
	  emergency_saving_flag=1; 
	}
 } else {
	 emergency_saving_flag=0;
	 start_counter();
	 }
 }

 void divider_handler(void){
		menustate=DIVIDER;
	if(clear_display_flag){																	//очистить нужно единожды, поэтому был введен флаг очистки дисплея
		clear_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);
		draw_border(ORANGE);
		draw_string(10,5,"SET DIV.",1,BACKGROUND_COLOR,GRAYBLUE,DotMatrix_M_Slash);
		clear_display_flag=0;
		
	}

	switch(readButtonState()){
		case BUTTON_SELECT:
		case BUTTON_RESET:
		case BUTTON_ENTERMENU:
			resetButton();
			break ;
		case BUTTON_MENUITEMBACK:
			eeprom_write_uint16(EE24C02_ADDR,DIVIDER_CELL,(uint16_t)(divider*1000));
			return_from_handler();
			break ;
		case BUTTON_LEFT:
			divider-=0.001;
			if (divider<=0.100)divider=0.100;
			voltage=get_voltage_dma();
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		case BUTTON_RIGHT:
			divider+=0.001;
			if (divider>=1)divider=1;
			voltage=get_voltage_dma();
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		default:
			draw_float_number(20,50,divider,"%0.3f",1,BACKGROUND_COLOR,RED,DotMatrix_M_Slash);
			draw_float_number(120,65,voltage,"%0.1f",0,BACKGROUND_COLOR,DARKBLUE,TinyFont);		//для ориентирования, чего там настроили, выведем значение напряжения
			break ;
		}
	 
 }
 
 void voltfactor_handler(void){
	menustate=VOLTFACTOR;
	if(clear_display_flag){																		//очистить нужно единожды, поэтому был введен флаг очистки дисплея
		clear_current_menu(X_MENU_OFFSET,Y_MENU_OFFSET);
		draw_border(ORANGE);
		draw_string(10,5,"SET VOF.",1,BACKGROUND_COLOR,GRAYBLUE,DotMatrix_M_Slash);
		clear_display_flag=0;
		
	}

	switch(readButtonState()){
		case BUTTON_SELECT:
		case BUTTON_RESET:
		case BUTTON_ENTERMENU:
			resetButton();
			break ;
		case BUTTON_MENUITEMBACK:
			eeprom_write_byte(EE24C02_ADDR,VOF_CELL,vof*10);
			return_from_handler();
			break ;
		case BUTTON_LEFT:
			vof-=0.1;
			//vof<=10.0?draw_number(98,50,66,1,BACKGROUND_COLOR, BACKGROUND_COLOR, DotMatrix_M_Slash):0;
			vof<=6.5?vof=6.5:0;
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		case BUTTON_RIGHT:
			vof+=0.1;
			vof>=15.5?vof=15.5:0;
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		default:
			draw_float_number(40,50,vof,"%0.1f",0,BACKGROUND_COLOR,DARKBLUE,DotMatrix_M_Slash);
			break ;
		}
	  
 }
 
 void redcomponent_handler(void){
	char buf[10]="";
	menustate=REDCOMPONENT;
	const uint8_t line_height = (*(MENU_FONT + 1) + POINTER_TAB);
	if(clear_display_flag){													
		draw_border(ORANGE);
		clear_display_flag=0;
	}
	
	switch(readButtonState()){
		case BUTTON_SELECT:
		case BUTTON_RESET:
		case BUTTON_ENTERMENU:
			resetButton();
			break ;
		case BUTTON_MENUITEMBACK:
			leds_off();
			eeprom_write_byte(EE24C02_ADDR,REDCOMPONENT_CELL,redcomponent);
			return_from_handler();
			break ;
		case BUTTON_LEFT:
			color&=~0x00ff0000;
			color|=((redcomponent-=1)<<16);
			redcomponent<0?redcomponent=255:0;
			bus_reset();
			setcolor(color);
			send_data_leds();
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		case BUTTON_RIGHT:
			color&=~0x00ff0000;
			color|=((redcomponent+=1)<<16);
			redcomponent>255?redcomponent=0:0;
			bus_reset();
			setcolor(color);
			send_data_leds();
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		default:
			sprintf(buf,"%3u",redcomponent);
			draw_string(X_MENU_OFFSET+MENU_FONT[0]*10,Y_MENU_OFFSET ,buf,1,BACKGROUND_COLOR,RED,MENU_FONT);
			draw_string(4,Y_MENU_OFFSET+ 5 * line_height ,"RESULT",1,BACKGROUND_COLOR,WHITE,MENU_FONT);
			draw_hexnumber(X_MENU_OFFSET+MENU_FONT[0]*7,Y_MENU_OFFSET+ 5 * line_height,color,1,BACKGROUND_COLOR,WHITE,MENU_FONT);
			break ;
		}
	
 }
 
 void greencomponent_handler(void){
	menustate=GREENCOMPONENT;
	char buf[10]="";
	const uint8_t line_height = (*(MENU_FONT + 1) + POINTER_TAB);
	if(clear_display_flag){													
		draw_border(ORANGE);
		clear_display_flag=0;
	}
	
	switch(readButtonState()){
		case BUTTON_SELECT:
		case BUTTON_RESET:
		case BUTTON_ENTERMENU:
			resetButton();
			break ;
		case BUTTON_MENUITEMBACK:
			leds_off();
			eeprom_write_byte(EE24C02_ADDR,GREENCOMPONENT_CELL,greencomponent);
			return_from_handler();
			break ;
		case BUTTON_LEFT:
			color&=~0x0000ff00;
			color|=((greencomponent-=1)<<8);
			greencomponent<0?greencomponent=255:0;
			bus_reset();
			setcolor(color);
			send_data_leds();
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		case BUTTON_RIGHT:
			color&=~0x0000ff00;
			color|=((greencomponent+=1)<<8);
			greencomponent>255?greencomponent=0:0;
			bus_reset();
			setcolor(color);
			send_data_leds();
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		default:
			sprintf(buf,"%3u",greencomponent);
			draw_string(X_MENU_OFFSET+MENU_FONT[0] * 10,Y_POINTER_OFFSET + 1 * line_height,buf,1,BACKGROUND_COLOR,GREEN,MENU_FONT);
			draw_string(4,Y_MENU_OFFSET+ 5 * line_height ,"RESULT",1,BACKGROUND_COLOR,WHITE,MENU_FONT);
			draw_hexnumber(X_MENU_OFFSET+MENU_FONT[0]*7,Y_MENU_OFFSET+ 5 * line_height,color,1,BACKGROUND_COLOR,WHITE,MENU_FONT);
			break ;
		} 
 }
 
 
 void bluecomponent_handler(void){
	menustate=BLUECOMPONENT;
	char buf[10]="";
	const uint8_t line_height = (*(MENU_FONT + 1) + POINTER_TAB);
	if(clear_display_flag){													
		draw_border(ORANGE);
		clear_display_flag=0;
	}
	
	switch(readButtonState()){
		case BUTTON_SELECT:
		case BUTTON_RESET:
		case BUTTON_ENTERMENU:
			resetButton();
			break ;
		case BUTTON_MENUITEMBACK:
			leds_off();
			eeprom_write_byte(EE24C02_ADDR,BLUECOMPONENT_CELL,bluecomponent);
			return_from_handler();
			break ;
		case BUTTON_LEFT:
			color&=~0x000000ff; //сбрасываем значения цвета, чтобы визуально было заметны изменения при ковырянии в менюшке
			color|=(bluecomponent-=1);
			bluecomponent<0?bluecomponent=255:0;
			bus_reset();
			setcolor(color);
			send_data_leds();
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		case BUTTON_RIGHT:
			color&=~0x000000ff;
			color|=(bluecomponent+=1);
			bluecomponent>255?bluecomponent=0:0;
			bus_reset();
			setcolor(color);
			send_data_leds();
			beep(BEEP_TIME);
			resetButton();
			break ;
				
		default:
			sprintf(buf,"%3u",bluecomponent);
			draw_string(X_MENU_OFFSET+MENU_FONT[0] * 10,Y_POINTER_OFFSET + 2 * line_height,buf,1,BACKGROUND_COLOR,BLUE,MENU_FONT);
			draw_string(4,Y_MENU_OFFSET+ 5 * line_height ,"RESULT",1,BACKGROUND_COLOR,WHITE,MENU_FONT);
			draw_hexnumber(X_MENU_OFFSET+MENU_FONT[0]*7,Y_MENU_OFFSET+ 5 * line_height,color,1,BACKGROUND_COLOR,WHITE,MENU_FONT);
			break ;
		} 	 
 }