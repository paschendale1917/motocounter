//SPI1
//1/64000000=~16 нс
//1600000~1.6 мкс
#include "spi.h"

void init_master_spi(void){ 
//настройка gpio	
	SET_BIT(RCC->IOPENR,RCC_IOPENR_GPIOAEN);
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE1,0x02<<GPIO_MODER_MODE1_Pos);							//alternate func
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE2,0x02<<GPIO_MODER_MODE2_Pos);
	MODIFY_REG(GPIOA->MODER,GPIO_MODER_MODE6,0x02<<GPIO_MODER_MODE6_Pos);

	MODIFY_REG(GPIOA->OSPEEDR,GPIO_OSPEEDR_OSPEED1,0x03<<GPIO_OSPEEDR_OSPEED1_Pos);
	MODIFY_REG(GPIOA->OSPEEDR,GPIO_OSPEEDR_OSPEED2,0x03<<GPIO_OSPEEDR_OSPEED2_Pos);
	MODIFY_REG(GPIOA->OSPEEDR,GPIO_OSPEEDR_OSPEED6,0x03<<GPIO_OSPEEDR_OSPEED6_Pos);
	
	CLEAR_BIT(GPIOA->AFR[0],GPIO_AFRL_AFSEL1); 														//sck AF0
	CLEAR_BIT(GPIOA->AFR[0],GPIO_AFRL_AFSEL2);														//mosi AF0
	CLEAR_BIT(GPIOA->AFR[0],GPIO_AFRL_AFSEL6);														//miso AF0
//настройка spi	
	SET_BIT(RCC->APBENR2,RCC_APBENR2_SPI1EN);
	DISABLE(SPI1);
	ENABLE_FULLDUPLEX(SPI1); 					
	SPI_MODE_0(SPI1);
	//LSBFIRST(SPI1);												//здесь какой-то кос€чило: шина виснет, если установить lsbfirst							
	BAUDRATE_2(SPI1);												//baud rate (APB/4)=64MHz/8=8 Mhz дл€ max 7219(clock clk period=100ns,1/100ns=10MHz)//смотри секцию RCC
	DISABLE_DMA_TX(SPI1);
	DISABLE_DMA_RX(SPI1);
	//DATASIZE_16(SPI1);
	ENABLE_SOFT_SSM(SPI1);
	ENABLE_MASTER(SPI1);
	ENABLE(SPI1);						  

}

	
uint8_t spi_sendbyte(uint8_t byte){
	uint32_t cpu_tact=10000;
	SET_BIT(SPI1->CR2,SPI_CR2_FRXTH);     															
	while(!(SPI1->SR & SPI_SR_TXE)){
		cpu_tact--;
		if(!cpu_tact)return ERROR;
	};
	*(__IO uint8_t *)(&SPI1->DR)=byte; 																//на 030 микроконтроллерах регистр всегда передает в 2хбайтном виде, поэтому нужно привести к 8 битному числу
	while(!(SPI1->SR & SPI_SR_RXNE)){
		cpu_tact--;
		if(!cpu_tact)return ERROR;
	}
	return  SPI1->DR;
}

uint16_t spi_sendword(uint16_t word){
	uint32_t cpu_tact=10000;
	CLEAR_BIT(SPI1->CR2,SPI_CR2_FRXTH);     //RXNE event, if 16 bit received
	while(!(SPI1->SR & SPI_SR_TXE)){
				cpu_tact--;
		if(!cpu_tact)return ERROR;
	};
	*(__IO uint8_t *)(&SPI1->DR)=word>>8; //на 030 микроконтроллерах регистр всегда передает в 2хбайтном виде, поэтому нужно привести к 8 битному числу
	*(__IO uint8_t *)(&SPI1->DR)=(uint8_t)word;  
	while(!(SPI1->SR & SPI_SR_RXNE)){
				cpu_tact--;
		if(!cpu_tact)return ERROR;
	};
	return  SPI1->DR;
}