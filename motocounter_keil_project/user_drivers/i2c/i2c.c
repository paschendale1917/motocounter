#include "i2c.h"
#include "stm32g0xx.h"

#define FREQ400KHZ

uint8_t page[16]={0};
uint8_t i2c_error=0;

void i2c2_init(void){
	RCC->IOPENR|=RCC_IOPENR_GPIOAEN;
	GPIOA->MODER&=~(GPIO_MODER_MODE11|
					GPIO_MODER_MODE12);
	GPIOA->MODER|=GPIO_MODER_MODE11_1|
				  GPIO_MODER_MODE12_1;
	GPIOA->AFR[1]|=GPIO_AFRH_AFSEL11_1|  //Alternate func output Open-drain
				   GPIO_AFRH_AFSEL11_2;
	GPIOA->AFR[1]|=GPIO_AFRH_AFSEL12_1|
					GPIO_AFRH_AFSEL12_2;
	GPIOA->OSPEEDR|=GPIO_OSPEEDR_OSPEED11_Msk|
					GPIO_OSPEEDR_OSPEED12_Msk;
	GPIOA->PUPDR&=~(GPIO_PUPDR_PUPD11_Msk|
					GPIO_PUPDR_PUPD12_Msk);
	GPIOA->OTYPER|=GPIO_OTYPER_OT11|
				   GPIO_OTYPER_OT12;
	
	RCC->APBENR1|=RCC_APBENR1_I2C2EN;
	DISABLE_PERIF;//i2c off
	I2C2->CR2&=~I2C_CR2_SADD_Msk;

#ifdef FREQ100KHZ
	MODIFY_REG(I2C2->TIMINGR,I2C_TIMINGR_PRESC,0x03<<I2C_TIMINGR_PRESC_Pos); 
	MODIFY_REG(I2C2->TIMINGR,I2C_TIMINGR_SCLL,0x13<<I2C_TIMINGR_SCLL_Pos); //из даташита дл€ 100к√ц
	MODIFY_REG(I2C2->TIMINGR,I2C_TIMINGR_SCLH,0x0f<<I2C_TIMINGR_SCLH_Pos);
	MODIFY_REG(I2C2->TIMINGR,I2C_TIMINGR_SDADEL,0x02<<I2C_TIMINGR_SDADEL_Pos);
	MODIFY_REG(I2C2->TIMINGR,I2C_TIMINGR_SCLDEL,0x04<<I2C_TIMINGR_SCLDEL_Pos);
#else 
	MODIFY_REG(I2C2->TIMINGR,I2C_TIMINGR_PRESC,0x01<<I2C_TIMINGR_PRESC_Pos);
	MODIFY_REG(I2C2->TIMINGR,I2C_TIMINGR_SCLL,0x09<<I2C_TIMINGR_SCLL_Pos); //из даташита дл€ 400к√ц
	MODIFY_REG(I2C2->TIMINGR,I2C_TIMINGR_SCLH,0x03<<I2C_TIMINGR_SCLH_Pos);
	MODIFY_REG(I2C2->TIMINGR,I2C_TIMINGR_SDADEL,0x02<<I2C_TIMINGR_SDADEL_Pos);
	MODIFY_REG(I2C2->TIMINGR,I2C_TIMINGR_SCLDEL,0x03<<I2C_TIMINGR_SCLDEL_Pos);
#endif

	CLEAR_BIT(I2C2->CR2,I2C_CR2_ADD10);//7 bit
	ENABLE_PERIF; //enable i2c
	
}

void i2c_softreset(void){
	DISABLE_PERIF;
	ENABLE_PERIF;	
}


void i2c_reinit(){ //грубо говор€, сброс к состо€нию после ресета. ¬ случае, если флаг бизи висит, но на шине высокий потенциал, то бишь она свободна
  if (READ_BIT(I2C2->ISR, I2C_ISR_BUSY)) {  
		if ((READ_BIT(GPIOA->IDR, GPIO_IDR_ID11)) && (READ_BIT(GPIOA->IDR, GPIO_IDR_ID12))) {   //≈сли лини€ свободна, а BUSY висит
			i2c_softreset(); 
			i2c2_init(); 
		} 
	}
//	CLEAR_REG(I2C2->TXDR);
}

void set_error_flag(uint8_t err_pos){
	 uint32_t millis=system_tick;
	if(system_tick-millis>1000){
			i2c_error=err_pos;
			return;
	}
}
void i2c_start( uint8_t dev_addr, uint16_t datasize){
	
	i2c_reinit();
	MODIFY_REG(I2C2->CR2,I2C_CR2_SADD,dev_addr<<I2C_CR2_SADD_Pos) ; 
	CLEAR_BIT(I2C2->CR2, I2C_CR2_RD_WRN);
	MODIFY_REG(I2C2->CR2,I2C_CR2_NBYTES,datasize<<I2C_CR2_NBYTES_Pos);
	START; 
	while(!(READ_BIT(I2C2->ISR, I2C_ISR_BUSY))){
		set_error_flag(1);
	};

}

void i2c_stop(void){
	SET_BIT(I2C2->CR2,I2C_CR2_STOP); 
	while (I2C2->ISR & I2C_ISR_BUSY) {};
	I2C2->CR2&=~I2C_CR2_SADD_Msk;
	I2C2->ICR |= I2C_ICR_STOPCF;      
  I2C2->ICR |= I2C_ICR_NACKCF;      
	 CLEAR_BIT(I2C2->CR2,I2C_CR2_AUTOEND);
		if (I2C2->ISR & (I2C_ISR_ARLO | I2C_ISR_BERR)) 
   {
      I2C2->ICR |= I2C_ICR_ARLOCF;
      I2C2->ICR |= I2C_ICR_BERRCF;
   }
}


void i2c_write_byte_wr(uint8_t byte){ 														//не разобралс€ почему, но чтение байта  после отправки адреса устройства и адреса регистра возможно только с  функцией в данном виде
	
	while (!(I2C2->ISR & I2C_ISR_TC)&&(!(I2C2->ISR & I2C_ISR_TXIS))) {
		set_error_flag(2);
	};
		I2C2->TXDR = byte;
					
	if (READ_BIT(I2C2->ISR, I2C_ISR_NACKF)) {   
		i2c_stop();
		SET_BIT(I2C2->ICR, I2C_ICR_NACKCF);  
	}
	
}

void i2c_write_byte(uint8_t byte){
	while (!(I2C2->ISR & I2C_ISR_TXE)) {
		set_error_flag(3);
	};
			I2C2->TXDR = byte;	
		
	if (READ_BIT(I2C2->ISR, I2C_ISR_NACKF)) {   
		i2c_stop();
		SET_BIT(I2C2->ICR, I2C_ICR_NACKCF); 
	}
	
}

uint8_t i2c_read_byte(void){
	uint8_t byte=0;
	while (!(I2C2->ISR & I2C_ISR_TC)&&(!(I2C2->ISR&I2C_ISR_RXNE))){
		set_error_flag(4);
	};//ждем, пока байт не придет в приемный	 регистр
	byte = I2C2->RXDR;
	
	if (READ_BIT(I2C2->ISR, I2C_ISR_NACKF)) {   
		i2c_stop();
		SET_BIT(I2C2->ICR, I2C_ICR_NACKCF);  
	}
	return byte;
}

