
#include "ADC.h"
#include "../st7735/st7735.h"
#include "../SysTick/systick_delay.h"

uint8_t Tsel=30; 																		//темепратура, при которой калибровался датчик на заводе(из даташита)
float Uref=0;
volatile float divider=0.144;
float voltage;
volatile uint16_t adc_data[100];

void adc_calib(void){
	CLEAR_REG(ADC1->CR);
	CLEAR_BIT(ADC1->ISR, ADC_ISR_EOCAL);
	SET_BIT(ADC1->CR, ADC_CR_ADCAL);
	while(READ_BIT(ADC1->ISR,ADC_ISR_EOCAL));
}

void init_adc(void){
	SET_BIT(RCC->APBENR2,RCC_APBENR2_ADCEN);
	SET_BIT(RCC->IOPENR,RCC_IOPENR_GPIOBEN);
	MODIFY_REG(GPIOB->MODER,GPIO_MODER_MODE0,0x03<<GPIO_MODER_MODE0_Pos);
	GPIOB->PUPDR&=~GPIO_PUPDR_PUPD0_Msk;
  MODIFY_REG(ADC->CCR, ADC_CCR_PRESC, 0x0A<<ADC_CCR_PRESC_Pos); 						//prescaler 128/  16MHz/128=125 KHz
	MODIFY_REG(ADC1->CFGR2,ADC_CFGR2_OVSR,0x05<<ADC_CFGR2_OVSR_Pos); 					//oversampling 64x
	MODIFY_REG(ADC1->CFGR2,ADC_CFGR2_OVSS,0x06<<ADC_CFGR2_OVSS_Pos); 					// сдвиг на 6 бит вправо
	SET_BIT(ADC1->CFGR2,ADC_CFGR2_OVSE);												//enable oversampling
	adc_calib();

}

void init_adc_dma(void){
	
	//настройка порта
	SET_BIT(RCC->IOPENR,RCC_IOPENR_GPIOBEN);
	MODIFY_REG(GPIOB->MODER,GPIO_MODER_MODE0,0x03<<GPIO_MODER_MODE0_Pos);
	GPIOB->PUPDR&=~GPIO_PUPDR_PUPD0_Msk;
	
	//настройка ацп
	SET_BIT(RCC->APBENR2,RCC_APBENR2_ADCEN);
  MODIFY_REG(ADC->CCR, ADC_CCR_PRESC, 0x09<<ADC_CCR_PRESC_Pos);  						//prescaler 256/  16MHz/256=62.5 KHz
	MODIFY_REG(ADC1->CFGR2,ADC_CFGR2_OVSR,0x05<<ADC_CFGR2_OVSR_Pos); 					//oversampling 64x
	MODIFY_REG(ADC1->CFGR2,ADC_CFGR2_OVSS,0x06<<ADC_CFGR2_OVSS_Pos); 					// сдвиг на 6 бит вправо
	SET_BIT(ADC1->CFGR2,ADC_CFGR2_OVSE);												//enable oversampling
	adc_calib();
	SET_BIT(ADC1->CFGR1,ADC_CFGR1_OVRMOD);
	SET_BIT(ADC1->CFGR1,ADC_CFGR1_DMAEN);												//разрешаем генерацию запросов дма от ацп(обязательно только после калибровки)
	SET_BIT(ADC1->CFGR1,ADC_CFGR1_DMACFG);
	

	//настройка дма
	SET_BIT(RCC->AHBENR,RCC_AHBENR_DMA1EN);
	CLEAR_BIT(DMA1_Channel2->CCR,DMA_CCR_MEM2MEM|   									//режим MEM2MEM отключен														
								 DMA_CCR_PINC); 										//инкремент адреса периферии выключен
															  		
	CLEAR_BIT(DMA1_Channel2->CCR,DMA_CCR_DIR);  										// 0-из периферии в память  
	SET_BIT(DMA1_Channel2->CCR,DMA_CCR_MINC); 		 									//инкремент адреса памяти включен
	SET_BIT(DMA1_Channel2->CCR,DMA_CCR_CIRC); 		 									// кольцевой режим включен
	
	DMA1_Channel2->CPAR = (uint32_t)(&ADC1->DR); 										//заносим адрес регистра DR в CPAR
	DMA1_Channel2->CMAR = (uint32_t)&adc_data; 											//заносим адрес данных в регистр CMAR
	DMA1_Channel2->CNDTR = sizeof(adc_data)/sizeof(adc_data[0]); 										
	
	MODIFY_REG(DMA1_Channel2->CCR,DMA_CCR_PL,    0x00 << DMA_CCR_PL_Pos); 				//приоритет низкий
	MODIFY_REG(DMA1_Channel2->CCR,DMA_CCR_MSIZE, 0x01 << DMA_CCR_MSIZE_Pos); 			//разрядность данных в памяти 16 бит
	MODIFY_REG(DMA1_Channel2->CCR,DMA_CCR_PSIZE, 0x01 << DMA_CCR_PSIZE_Pos); 			//разрядность регистра данных 16 бит 
	MODIFY_REG(DMAMUX1_Channel1->CCR,DMAMUX_CxCR_DMAREQ_ID,5<<DMAMUX_CxCR_DMAREQ_ID_Pos);
	SET_BIT(DMA1_Channel2->CCR,DMA_CCR_EN);

}

uint16_t ema_filter(uint16_t input, uint16_t prev_output, float alpha) {
	return (uint16_t)(alpha * input + (1.0 - alpha) * prev_output);      
}


void ema_filter_array(volatile uint16_t *data,  uint8_t size, float alpha) { 
	for(uint8_t i = 1; i < size; i++) {
		data[i-1] = ema_filter(data[i], data[i-1], alpha);
	}
}

uint16_t get_adcdata_dma(void){
   uint32_t adcdata=0;
   uint16_t ar_size=sizeof(adc_data)/sizeof(adc_data[0]);
	
	CLEAR_BIT(ADC1->CR, ADC_CR_ADSTART);												//остановим преобразование
	SET_BIT(ADC1->CFGR1, ADC_CFGR1_CONT);												//continuous conversion mode
	SET_BIT(ADC1->CHSELR,ADC_CHSELR_CHSEL8);
	while(!(READ_BIT(ADC1->ISR, ADC_ISR_CCRDY)));
	if(!(ADC1->CR&ADC_CR_ADEN)){
		SET_BIT(ADC1->CR, ADC_CR_ADEN);
		while(READ_BIT(ADC1->ISR, ADC_ISR_ADRDY));
	}

	SET_BIT(ADC1->CR, ADC_CR_ADSTART);

 ema_filter_array(adc_data,ar_size,ALFHA);
	
// for( uint16_t i=0;i<=ar_size;i++){
//	 adcdata=adcdata+adc_data[i];
// }
//   adcdata=adcdata/ar_size;
	return *adc_data;
	
}

float get_voltage_dma(void){
	voltage=((REFERENCE/4096)*get_adcdata_dma())/divider; //del4095
	return voltage;
}



//здесь получаем сырые данные ацп от 13 канала, к которому подключен внутренний источник опорного напряжения
uint16_t get_intref_ch13(void){
	SET_BIT(ADC1->CR, ADC_CR_ADSTP);
	SET_BIT(ADC1->CHSELR,ADC_CHSELR_CHSEL13);
	while(!(READ_BIT(ADC1->ISR, ADC_ISR_CCRDY)));
	if(!(ADC1->CR&ADC_CR_ADEN)){
	SET_BIT(ADC1->CR, ADC_CR_ADEN);
	while(READ_BIT(ADC1->ISR, ADC_ISR_ADRDY));
	}
	SET_BIT(ADC->CCR, ADC_CCR_VREFEN);
	SET_BIT(ADC1->CR, ADC_CR_ADSTART);
	_delay_ms(2); //обождем, пока устаканится
	return (uint16_t)ADC1->DR;
}

//здесь рассчитываем напряжение питания
float get_refvoltage(void){
	float Uref_local=0;
	uint16_t adc_ch13=0;
	SET_BIT(ADC1->CR, ADC_CR_ADSTP);
	//CLEAR_BIT(ADC1->CHSELR,ADC_CHSELR_CHSEL8);
	SET_BIT(ADC1->CHSELR,ADC_CHSELR_CHSEL13);
	while(!(READ_BIT(ADC1->ISR, ADC_ISR_CCRDY)));
	if(!(ADC1->CR&ADC_CR_ADEN)){
		SET_BIT(ADC1->CR, ADC_CR_ADEN);
		while(READ_BIT(ADC1->ISR, ADC_ISR_ADRDY));
	}
	SET_BIT(ADC->CCR, ADC_CCR_VREFEN);
	SET_BIT(ADC1->CR, ADC_CR_ADSTART);
	_delay_ms(2);
	adc_ch13=ADC1->DR;
	Uref_local=(3*VREFINT_CAL)/(float)adc_ch13; 												//3 в-напряжение, при котором на заводе измерили VREFINT_CAL и записали в память МК//здесь идет расчет напряжения питания
	return (float)Uref_local;
}



//void draw_VDDA(uint16_t xpos,  uint16_t ypos,int8_t space,uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
//	char *n="v";		
//	float Uref_local=0;
//	Uref_local=get_refvoltage(); //3 в-напряжение, при котором на заводе измерили VREFINT_CAL и записали в память МК//здесь идет расчет напряжения питания;
//	draw_float_number(xpos,ypos,Uref_local,space,bcolor, fcolor, font);
//	draw_char(xpos+2*font[0]+8,ypos, *n,bcolor,fcolor,font);
//}

		
		
uint16_t get_adc_data_cont(void){
	uint16_t ADC_data;
	SET_BIT(ADC1->CFGR1, ADC_CFGR1_CONT);														//continuous conversion mode
	SET_BIT(ADC1->CHSELR,ADC_CHSELR_CHSEL8);
	while(!(READ_BIT(ADC1->ISR, ADC_ISR_CCRDY)));
	if(!(ADC1->CR&ADC_CR_ADEN)){
		SET_BIT(ADC1->CR, ADC_CR_ADEN);
		while(READ_BIT(ADC1->ISR, ADC_ISR_ADRDY));
	}
	SET_BIT(ADC1->CR, ADC_CR_ADSTART);
	ADC_data=ADC1->DR;
	return ADC_data;
}

float get_voltage(void){
	uint16_t adc=0;
	adc=get_adc_data_single();
	voltage=((REFERENCE/4096)*adc)/divider;
	return voltage;
}

uint16_t get_adc_data_single(void){
	uint16_t ADC_data;
	SET_BIT(ADC1->CR, ADC_CR_ADSTP);
	CLEAR_BIT(ADC1->CFGR1, ADC_CFGR1_CONT);												//single conversion mode
	SET_BIT(ADC1->CHSELR,ADC_CHSELR_CHSEL8);
	_delay_ms(1);
	while(!(READ_BIT(ADC1->ISR, ADC_ISR_CCRDY)));
	if(!(ADC1->CR&ADC_CR_ADEN)){
		SET_BIT(ADC1->CR, ADC_CR_ADEN);
		while(READ_BIT(ADC1->ISR, ADC_ISR_ADRDY));
	}
	SET_BIT(ADC1->CR, ADC_CR_ADSTART);
	_delay_ms(1);
	ADC_data=ADC1->DR;
	return ADC_data;
}


uint16_t get_sensor_ch12(void){
	uint16_t temp_sensor_ch12=0;
	SET_BIT(ADC1->CHSELR,ADC_CHSELR_CHSEL12);
	while(!(READ_BIT(ADC1->ISR, ADC_ISR_CCRDY)));
	if(!(ADC1->CR&ADC_CR_ADEN)){
		SET_BIT(ADC1->CR, ADC_CR_ADEN);
		while(READ_BIT(ADC1->ISR, ADC_ISR_ADRDY));
	}
	SET_BIT(ADC->CCR, ADC_CCR_TSEN);
	SET_BIT(ADC1->CR, ADC_CR_ADSTART);
	_delay_ms(1);
	temp_sensor_ch12=ADC1->DR;
	return (uint16_t)temp_sensor_ch12;
}
		
		
uint16_t get_temp(void){
	Uref=get_refvoltage();
	_delay_ms(2);
	uint16_t temp_sensor_ch12=get_sensor_ch12();
	_delay_ms(2);
	uint16_t sensor_calib=TS_CAL1*3/3.3;											//пересчет на напряжение питания 3.3 в
	float koef=2.5*4096/(Uref*1000);												//2.5 мВ на 1 градус из даташита
	uint16_t b=sensor_calib-koef*Tsel;   											//Tsel=30 из даташита, т-ра , при которой на заводе измеряли 
	uint16_t temp=(temp_sensor_ch12-b)/koef;
	return temp/1;
	
}
