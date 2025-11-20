#include "buzzer.h"



uint16_t beep_array[]={BUZ_1,BUZ_1,BUZ_1,BUZ_1,BUZ_1,BUZ_1,
											 BUZ_1,BUZ_1,BUZ_1,BUZ_1,BUZ_1,BUZ_1,
											 BUZ_1,BUZ_1,BUZ_1,BUZ_1,BUZ_1,BUZ_1,
											 BUZ_1,BUZ_1,BUZ_1,BUZ_1,BUZ_1,BUZ_1
											 
											 
};

void init_gpio_buzzer(void){
	SET_BIT(RCC->IOPENR,RCC_IOPENR_GPIOBEN);
	MODIFY_REG(GPIOB->MODER,GPIO_MODER_MODE9,0x02<<GPIO_MODER_MODE9_Pos); //альтернативная функция вывода
	CLEAR_BIT(GPIOB->OTYPER,GPIO_OTYPER_OT9);//push-pull
	MODIFY_REG(GPIOB->PUPDR,GPIO_PUPDR_PUPD9,0x00<<GPIO_PUPDR_PUPD9_Pos); //;//No pull-up, pull-down
	MODIFY_REG(GPIOB->OSPEEDR,GPIO_OSPEEDR_OSPEED9,0x03<<GPIO_OSPEEDR_OSPEED9_Pos);
	SET_BIT(GPIOB->AFR[1],GPIO_AFRH_AFSEL9_1);	//AFR[0]==AFRL; AFR[1]==AFRH;AFSEL9_1 -нога PB9, альтенативная функция AF2, первый бит в AFSEL устанавливаем в единицу

}

void init_buzzer(void){
	init_gpio_buzzer();
	SET_BIT(RCC->AHBENR,RCC_AHBENR_DMA1EN);
	SET_BIT(RCC->APBENR2,RCC_APBENR2_TIM17EN);
	//настройка таймера
	SET_BIT(TIM17->CR1,TIM_CR1_ARPE);// режим предзагрузки для регистра ARR
	SET_BIT(TIM17->CR1,TIM_CR1_URS);//только переполнение счётчика таймера сопровождается генерацией события обновления
	CLEAR_BIT(TIM17->CR1,TIM_CR1_CMS);//счет либо от нуля, либо от значения ARR
	CLEAR_BIT(TIM17->CR1,TIM_CR1_DIR);//счет вверх
	MODIFY_REG(TIM17->CCMR1,TIM_CCMR1_OC1M,0x06<<TIM_CCMR1_OC1M_Pos);//шим на первом канале таймера
	SET_BIT(TIM17->CCMR1,TIM_CCMR1_OC1PE);//буферизация регистра CCR1:новое значение из буфера передаётся  при каждом событии обновления
	//SET_BIT(TIM17->DIER,TIM_DIER_CC1DE);//разрешаем запрос DMA в случае  срабатывания схемы сравнения в канале 1
	WRITE_REG(TIM17->PSC,BUZ_TIM_PSC);          								//prescaler делит частоту                      
	WRITE_REG(TIM17->ARR,F_BUZ_TIM/F_BUZ_PWM-1); //чтобы получить 4 кГц, сюда записывается 64000000/4000-1=15999c
	//SET_BIT(TIM17->DIER,TIM_DIER_UDE);//
	SET_BIT(TIM17->CCER,TIM_CCER_CC1E);//разрешаем таймеру использовать ножку микроконтроллера
	SET_BIT(TIM17->BDTR,TIM_BDTR_MOE);
	//SET_BIT(TIM17->CR1,TIM_CR1_OPM);//one pulse mode(после выдачи импульса счетчик таймера выключается и его необходимо вручную вырубать, если это требуется)
	//SET_BIT(TIM17->DIER,TIM_DIER_UIE);
	
	//настройка DMA
	
//	DMA1_Channel2->CPAR = (uint32_t)(&TIM17->CCR1); //куда пишем
//	DMA1_Channel2->CNDTR = sizeof(beep_array);//устанавливаем количество передаваемых данных
//	DMA1_Channel2->CMAR = (uint32_t)beep_array; //откуда берем данные для передачи
//  
//  SET_BIT(DMA1_Channel2->CCR,DMA_CCR_DIR);  			//1 - из памяти в периферию
//	SET_BIT(DMA1_Channel2->CCR,DMA_CCR_TCIE); // прерывание дма по окончанию передачи данных
//	CLEAR_BIT(DMA1_Channel2->CCR,DMA_CCR_MEM2MEM| DMA_CCR_PINC);//режим MEM2MEM отключен\инкремент адреса периферии отключен
//	SET_BIT(DMA1_Channel2->CCR,DMA_CCR_MINC|DMA_CCR_CIRC); 			//Включить инкремент адреса памяти+кольцевой режим
//	MODIFY_REG(DMA1_Channel2->CCR,DMA_CCR_PL, 0x00 << DMA_CCR_PL_Pos); //приоритет низкий
//  MODIFY_REG(DMA1_Channel2->CCR,DMA_CCR_MSIZE, 0x00 << DMA_CCR_MSIZE_Pos); //разрядность данных в памяти 8 бит
//  MODIFY_REG(DMA1_Channel2->CCR,DMA_CCR_PSIZE, 0x01 << DMA_CCR_PSIZE_Pos); //разрядность регистра данных 16 бит 
//  DMAMUX1_Channel1->CCR=(47 << DMAMUX_CxCR_DMAREQ_ID_Pos); //выбираем вход мультиплексора DMA для канала 1 таймера TIM17(по таблице в разделе DMAmux)
//	
//	NVIC_EnableIRQ(DMA1_Channel2_3_IRQn);
	//NVIC_EnableIRQ(TIM17_IRQn);         //глобальное разрешние прерываний для таймера

}


//void beep_dma(void){
//	
//	DMA1->IFCR=1<<DMA_IFCR_CTCIF2_Pos;//очищаем  флаг прерывания
//	WRITE_REG(TIM17->ARR,F_BUZ_TIM/F_BUZ_PWM-1); //установка низкого уровня на ножке микроконтроллера
//	WRITE_REG(TIM17->CNT,0);  
//	WRITE_REG(TIM17->CCR1,0);
//  SET_BIT(TIM17->CR1,TIM_CR1_CEN);
//  SET_BIT(DMA1_Channel2->CCR,DMA_CCR_EN); //включаем передачу по дма
//	
//	}
//void DMA1_Channel2_3_IRQHandler(void){
//	CLEAR_BIT(DMA1_Channel2->CCR,DMA_CCR_EN); //выключаем передачу по дма 
//	CLEAR_BIT(TIM17->CR1,TIM_CR1_CEN);  //останов счета  в таймере
//	DMA1->IFCR=1<<DMA_IFCR_CTCIF2_Pos;//очищаем  флаг прерывания
//  
//}
 


void beep_on(){
	CLEAR_BIT(TIM17->CR1,TIM_CR1_CEN);
	MODIFY_REG(GPIOB->MODER,GPIO_MODER_MODE9,0x02<<GPIO_MODER_MODE9_Pos); 
	MODIFY_REG(GPIOB->PUPDR,GPIO_PUPDR_PUPD9,0x00<<GPIO_PUPDR_PUPD9_Pos); 
	WRITE_REG(TIM17->CCR1,14000);
	SET_BIT(TIM17->CCER,TIM_CCER_CC1E);
	SET_BIT(TIM17->CR1,TIM_CR1_CEN);
}

void beep_t(){
	CLEAR_BIT(TIM17->CR1,TIM_CR1_CEN);
	MODIFY_REG(GPIOB->MODER,GPIO_MODER_MODE9,0x02<<GPIO_MODER_MODE9_Pos); 
	MODIFY_REG(GPIOB->PUPDR,GPIO_PUPDR_PUPD9,0x00<<GPIO_PUPDR_PUPD9_Pos); 
	WRITE_REG(TIM17->CCR1,0);
	SET_BIT(TIM17->CCER,TIM_CCER_CC1E);
	SET_BIT(TIM17->CR1,TIM_CR1_CEN);
}

void beep_off(){
	MODIFY_REG(GPIOB->MODER,GPIO_MODER_MODE9,0x01<<GPIO_MODER_MODE9_Pos); //прижал ногу к земле,чтобы в неактивном состоянии затвор транзистора не ловил наводки
	MODIFY_REG(GPIOB->PUPDR,GPIO_PUPDR_PUPD9,0x01<<GPIO_PUPDR_PUPD9_Pos); 
	CLEAR_BIT(TIM17->CR1,TIM_CR1_CEN);
	CLEAR_BIT(TIM17->CCER,TIM_CCER_CC1E);
}





void beep(uint16_t delay){  //пикалка для кнопок
	uint32_t millis=system_tick;
	do{
		beep_on();
	}while (system_tick-millis<delay);
	beep_off(); 
}

//void beep_beep(uint16_t repetition){
//	for(uint8_t x=0;x<=repetition;x++){
//	end_of_timer();
//	}

//}
//beep_beep(200,1000);	

//void beep_beep(void){
//	SET_BIT(TIM17->CCER,TIM_CCER_CC1E);//разрешаем таймеру использовать ножку микроконтроллера
// for(uint16_t x=0;x<sizeof(beep_array);x++){	
//		//WRITE_REG(TIM17->ARR,F_BUZ_TIM/F_BUZ_PWM-1);
//		WRITE_REG(TIM17->CNT,0); 
//		WRITE_REG(TIM17->CCR1,beep_array[x]); 
//	 SET_BIT(TIM17->CR1,TIM_CR1_CEN);
//		}	
//}