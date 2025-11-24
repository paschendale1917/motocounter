/* шаблон дл€ использовани€
	switch(readButtonState()){
			case BUTTON_LEFT:
				resetButton();
				break ;
				
			 case BUTTON_RIGHT:
				resetButton();
				break ;
			
      case BUTTON_UP:
				resetButton();
				break ;
			
      case BUTTON_DOWN:
				resetButton();
				break ;
			
			case BUTTON_SELECT:
				resetButton();
				break ;
			
      default: 
				break ;
    }
*/
//таймер TIM1

#include "buttons.h"


uint8_t  shortpress_left=0,
				 shortpress_right=0,
				 shortpress_ok=0,
				 longpress_left=0,
				 longpress_right=0,
				 longpress_ok=0;								 
																	

timer tim={.sec=0,
			.min=0,
			.hour=0
			};

void buttons_init(void){   
	SET_BIT(RCC->IOPENR,RCC_IOPENR_GPIOBEN); 								//тактирование порта B
	MODIFY_REG(GPIOB->MODER,GPIO_MODER_MODE6,0x00<<GPIO_MODER_MODE6_Pos); 	//mode[0:0] режим входа дл€ пина PB6
	MODIFY_REG(GPIOB->PUPDR,GPIO_PUPDR_PUPD6,0x01<<GPIO_PUPDR_PUPD6_Pos); 	//подт€га к питанию

	MODIFY_REG(GPIOB->MODER,GPIO_MODER_MODE7,0x00<<GPIO_MODER_MODE7_Pos); 	//mode[0:0] режим входа дл€ пина PB7
	MODIFY_REG(GPIOB->PUPDR,GPIO_PUPDR_PUPD7,0x01<<GPIO_PUPDR_PUPD7_Pos);
	
	SET_BIT(RCC->IOPENR,RCC_IOPENR_GPIOCEN); 								//тактирование порта C
	MODIFY_REG(GPIOC->MODER,GPIO_MODER_MODE15,0x00<<GPIO_MODER_MODE15_Pos); //mode[0:0] режим входа дл€ пина PC15
	MODIFY_REG(GPIOC->PUPDR,GPIO_PUPDR_PUPD15,0x01<<GPIO_PUPDR_PUPD15_Pos);	

	SET_BIT(RCC->APBENR2,RCC_APBENR2_TIM1EN);
	CLEAR_BIT(TIM1->CR1,TIM_CR1_CMS);										//The counter counts up or down depending on the direction bit
	CLEAR_BIT(TIM1->CR1,TIM_CR1_DIR);										//счет вверх
	SET_BIT(TIM1->DIER,TIM_DIER_UIE);										//включаем прерывание при переполнении счетчика
	WRITE_REG(TIM1->PSC,F_TIM1/F_TIM1_PRESCALED-1); 
	WRITE_REG(TIM1->ARR,TIM1_AUTORELOAD);
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);								//глобальное разрешение прерываний при переполнении и еще некоторых событи€х(описано в stm32g030xx.h)
	TIM_EnableCounter(TIM1);
}

void backlight_polling(void){
	static uint16_t lcd_cntr=0;
	if(readButtonState()==BUTTON_NOTHING){
		lcd_cntr++;
		if(lcd_cntr>=6000){
			lcd_cntr=0;
			set_brigtness(1);
			}
		}
		else{
			lcd_cntr=0;
			set_brigtness(brightness);
			}
}
void button_polling(void){
	static uint8_t count_left = 0;
	static uint8_t count_right = 0;
	static uint8_t count_ok = 0;
	
	//button left
	if(!BUTTON_LEFT_STATE){
	
		if(count_left < 255)  count_left++;
		}
	
	if(count_left>0&& count_left< SHORT_TIMEOUT) {
		if(BUTTON_LEFT_STATE){
			shortpress_left=1;
			count_left=0;
		}
	}
	if(count_left >LONG_TIMEOUT && count_left<255) {
		if(BUTTON_LEFT_STATE){
			longpress_left=1;
			count_left=0;
		}
	}
	//button ok
		if(!BUTTON_OK_STATE){	
			if(count_ok < 255)  count_ok++;
		}
		
	if(count_ok>0&& count_ok< SHORT_TIMEOUT) {
		if(BUTTON_OK_STATE){
			shortpress_ok=1;
			count_ok=0;
		}
	}
	if(count_ok >LONG_TIMEOUT && count_ok<255) {
		if(BUTTON_OK_STATE){
			longpress_ok=1;
			count_ok=0;
		}
	}
	//button right
		if(!BUTTON_RIGHT_STATE){		
			if(count_right < 255)  count_right++;
		}
		
	if(count_right>0&& count_right< SHORT_TIMEOUT) {
		if(BUTTON_RIGHT_STATE){
			shortpress_right=1;
			count_right=0;
		}
	}
	if(count_right >LONG_TIMEOUT && count_right<255) {
		if(BUTTON_RIGHT_STATE){
			longpress_right=1;
			count_right=0;
		}
	}
}

//обработчик прерывани€ при переполнении счетного регистра
 void TIM1_BRK_UP_TRG_COM_IRQHandler( void){
	if(READ_BIT(TIM1->SR, TIM_SR_UIF)){
		CLEAR_BIT(TIM1->SR,TIM_SR_UIF);										//сброс флага прерывани€ 
		WWDG_reload();
		button_polling();
		backlight_polling();
	}
}


uint8_t readButtonState(void){
	if(shortpress_ok){
		return BUTTON_SELECT;          
	}else 
		if(longpress_right){
			return BUTTON_RESET;
	}else 
		if(longpress_left){
			return BUTTON_MENUITEMBACK;
	}else 
		if(shortpress_left){
			return BUTTON_LEFT;
	}
	else 
		if(shortpress_right){
			return BUTTON_RIGHT;
	}else 
		if(longpress_ok){
		return BUTTON_ENTERMENU;          
	} else
		return BUTTON_NOTHING;
}

void resetButton(void){
	
	switch(readButtonState()){
		case BUTTON_SELECT:
			shortpress_ok=0;
			break;
		case BUTTON_RESET :
			longpress_right=0;
		  break;
		case BUTTON_MENUITEMBACK:
			longpress_left=0;
			break;
		case BUTTON_LEFT :
			shortpress_left=0;
			break;
		case BUTTON_RIGHT :
			shortpress_right=0;
			break;
		case BUTTON_ENTERMENU:
			longpress_ok=0;
			break;
	}
 } 

 
 //помойка
//		cnt_temp++;
//		if(cnt_temp==100){
//			tim.sec++;
//			cnt_temp=0;
//			if (tim.sec>59) {
//				tim.sec=0;
//				tim.min++;
//				if (tim.min>59){
//					tim.hour++;
//				}
//			}
//		}	