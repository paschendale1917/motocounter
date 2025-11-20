#include "eeprom.h"

uint8_t eeprom_error=0;

void eeprom_init(uint8_t dev_addr){
	i2c2_init();
	AUTOEND_EN;
	WRITE_REQUEST;
	MODIFY_REG(I2C2->CR2,I2C_CR2_SADD,dev_addr<<I2C_CR2_SADD_Pos) ; 
	MODIFY_REG(I2C2->CR2,I2C_CR2_NBYTES,17<<I2C_CR2_NBYTES_Pos);
    if(NACK) {                                        	
	  eeprom_error=1;
      i2c_reinit();                                   	
      return;
    }
}

uint8_t eeprom_write_byte(uint8_t dev_addr, uint16_t cell_addr,uint8_t byte){
	
	uint8_t overflow_flag = cell_addr/255; //доступно для записи 256 байт, затем нажно адрес микросхемы увеличить на A1 и снова станут доступны 256 байт
	for(uint8_t j=0; j<overflow_flag;j++){
		uint8_t ovf_mask=0b00000010<<j;
		dev_addr|=ovf_mask;
	}
	i2c_reinit();
	
	AUTOEND_EN;
	WRITE_REQUEST;
	
	MODIFY_REG(I2C2->CR2,I2C_CR2_SADD,dev_addr<<I2C_CR2_SADD_Pos) ; 
	MODIFY_REG(I2C2->CR2,I2C_CR2_NBYTES,2<<I2C_CR2_NBYTES_Pos);
	START; 
	i2c_write_byte(cell_addr); 
	i2c_write_byte(byte); 
 if(NACK){
	 return 0;
 }
 return 1;
	
}	

void eeprom_write_uint16(uint8_t dev_addr, uint16_t cell_addr,uint16_t word){
	
	uint8_t overflow_flag = cell_addr/255; //доступно для записи 256 байт, затем нажно адрес микросхемы увеличить на A1 и снова станут доступны 256 байт
	for(uint8_t j=0; j<overflow_flag;j++){
		uint8_t ovf_mask=0b00000010<<j;
		dev_addr|=ovf_mask;
	}
	
	i2c_reinit();
	AUTOEND_EN;
	WRITE_REQUEST;
//	if(NACK&&!(GPIOA->IDR,GPIO_IDR_ID11)){ // если еепромка ответила NACK, то, скорее всего, она пережевывает запихнутые в неё данные  и держит клок  внизу
//		while(!(GPIOA->IDR,GPIO_IDR_ID11)){}; //подождем, пока она отпустит клок
//		SET_BIT(I2C2->ICR,I2C_ICR_NACKCF);//сбросим ошибку nack
//		WRITE_REQUEST; //отправим запрос на передачу данных	
//	}
	MODIFY_REG(I2C2->CR2,I2C_CR2_SADD,dev_addr<<I2C_CR2_SADD_Pos) ; 
	MODIFY_REG(I2C2->CR2,I2C_CR2_NBYTES,3<<I2C_CR2_NBYTES_Pos);
	START; 
	i2c_write_byte(cell_addr); 
	i2c_write_byte(word>>8); 
	i2c_write_byte((uint8_t)word); 
}


void eeprom_write_one_page(uint8_t dev_addr, uint8_t page_addr,uint8_t *data){
	  uint16_t cell_addr=page_addr*16;
		uint8_t overflow_flag = cell_addr/255; //доступно для записи 256 байт, затем нажно адрес микросхемы увеличить на A1 и снова станут доступны 256 байт
		for(uint8_t j=0; j<overflow_flag;j++){
			uint8_t ovf_mask=0b00000010<<j;
			dev_addr|=ovf_mask;
	  }
		i2c_reinit();
		AUTOEND_EN;
		WRITE_REQUEST;
		MODIFY_REG(I2C2->CR2,I2C_CR2_SADD,dev_addr<<I2C_CR2_SADD_Pos) ; 
		MODIFY_REG(I2C2->CR2,I2C_CR2_NBYTES,17<<I2C_CR2_NBYTES_Pos);
		START; 
		i2c_write_byte(cell_addr); 
		for(uint8_t i=0;i<=16;i++){
			i2c_write_byte(*(data+i)); 
		}
}

void eeprom_write_some_bytes(uint8_t dev_addr,uint16_t cell_addr,uint8_t num_bytes,uint8_t *data){
	
	uint8_t overflow_flag = cell_addr/255; //доступно для записи 256 байт, затем нажно адрес микросхемы увеличить на A1 и снова станут доступны 256 байт
	for(uint8_t j=0; j<overflow_flag;j++){
		uint8_t ovf_mask=0b00000010<<j;
		dev_addr|=ovf_mask;
	}
	
	i2c_reinit();
	AUTOEND_EN;
	WRITE_REQUEST;
	MODIFY_REG(I2C2->CR2,I2C_CR2_SADD,dev_addr<<I2C_CR2_SADD_Pos) ; 
	MODIFY_REG(I2C2->CR2,I2C_CR2_NBYTES,(num_bytes+1)<<I2C_CR2_NBYTES_Pos);
	START; 
	i2c_write_byte(cell_addr); 
		for(uint8_t i=0;i<num_bytes;i++){
			i2c_write_byte(*(data+i)); 
		} 

}



//uint8_t i2c_read_byte(void){
//	uint8_t byte=0;
//	byte = I2C2->RXDR;
//	while (!(I2C2->ISR & I2C_ISR_TC)&&(!(I2C2->ISR&I2C_ISR_RXNE))){};
//		
//			
//	
//	
//	if (READ_BIT(I2C2->ISR, I2C_ISR_NACKF)) {   
//		i2c_stop();
//		CLEAR_BIT(I2C2->ISR, I2C_ISR_NACKF); 
//	}
//	return byte;
//}


uint8_t eeprom_read_byte(uint8_t dev_addr,uint16_t cell_addr){
	
	uint8_t byte=0;
	uint8_t overflow_flag = cell_addr/255; //доступно для записи 256 байт, затем нужно адрес микросхемы увеличить на A1 и снова станут доступны 256 байт
	for(uint8_t j=0; j<overflow_flag;j++){
		uint8_t ovf_mask=0b00000010<<j;
		dev_addr|=ovf_mask;
	}
	
	i2c_reinit();
	AUTOEND_DIS;
	WRITE_REQUEST;
	MODIFY_REG(I2C2->CR2,I2C_CR2_SADD,dev_addr<<I2C_CR2_SADD_Pos) ; 
	MODIFY_REG(I2C2->CR2,I2C_CR2_NBYTES,1<<I2C_CR2_NBYTES_Pos);
	START; 
	i2c_write_byte_wr(cell_addr);
	AUTOEND_DIS;
	READ_REQUEST;
	MODIFY_REG(I2C2->CR2,I2C_CR2_SADD,dev_addr<<I2C_CR2_SADD_Pos) ; 
	MODIFY_REG(I2C2->CR2,I2C_CR2_NBYTES,1<<I2C_CR2_NBYTES_Pos);
	START;
	byte=i2c_read_byte();
	i2c_stop();  //можно включить при  AUTOEND_DIS
	return byte;
}

uint16_t eeprom_read_uint16(uint8_t dev_addr,uint16_t cell_addr){
	
	volatile uint16_t word=0;
	volatile uint8_t temp;
	uint8_t overflow_flag = cell_addr/255; //доступно для записи 256 байт, затем нужно адрес микросхемы увеличить на A1 и снова станут доступны 256 байт
	for(uint8_t j=0; j<overflow_flag;j++){
		uint8_t ovf_mask=0b00000010<<j;
		dev_addr|=ovf_mask;
	}
	
	i2c_reinit();
	AUTOEND_EN;
	WRITE_REQUEST;
	MODIFY_REG(I2C2->CR2,I2C_CR2_SADD,dev_addr<<I2C_CR2_SADD_Pos) ; 
	MODIFY_REG(I2C2->CR2,I2C_CR2_NBYTES,1<<I2C_CR2_NBYTES_Pos);
	START; 
	i2c_write_byte_wr(cell_addr);
	AUTOEND_EN;
	READ_REQUEST;
	MODIFY_REG(I2C2->CR2,I2C_CR2_SADD,dev_addr<<I2C_CR2_SADD_Pos) ; 
	MODIFY_REG(I2C2->CR2,I2C_CR2_NBYTES,2<<I2C_CR2_NBYTES_Pos);
	START;
	word=i2c_read_byte();
	temp=i2c_read_byte();
	word<<=8;
	word|=temp;
	//i2c_stop();  //можно включить при  AUTOEND_DIS
	return word;
}

//очень медленная функция
void eeprom_read_data(uint8_t dev_addr,uint8_t cell_addr,uint8_t number_of_bytes, uint8_t *data){ //страницы по 16 байт //16 страниц в 24c01//*page -массив, в который производится чтение
	
	uint8_t overflow_flag = cell_addr/255; //доступно для записи 256 байт, затем нужно адрес микросхемы увеличить на A1 и снова станут доступны 256 байт
		for(uint8_t j=0; j<overflow_flag;j++){
			uint8_t ovf_mask=0b00000010<<j;
			dev_addr|=ovf_mask;
	}
	for(uint8_t i=0;i<number_of_bytes;++i){
	*(data+i-1)=eeprom_read_byte(dev_addr,cell_addr+i);
		
}
	
}



void eeprom_read_one_page(uint8_t dev_addr,uint8_t page_addr, uint8_t *page){ //страницы по 16 байт //16 страниц в 24c01//*page -массив, в который производится чтение
	uint8_t cell_addr=page_addr*16;
	uint8_t overflow_flag = cell_addr/255; //доступно для записи 256 байт, затем нужно адрес микросхемы увеличить на A1 и снова станут доступны 256 байт
		for(uint8_t j=0; j<overflow_flag;j++){
			uint8_t ovf_mask=0b00000010<<j;
			dev_addr|=ovf_mask;
	}
	i2c_reinit();
	AUTOEND_DIS;
	WRITE_REQUEST;
	MODIFY_REG(I2C2->CR2,I2C_CR2_SADD,dev_addr<<I2C_CR2_SADD_Pos) ; 
	MODIFY_REG(I2C2->CR2,I2C_CR2_NBYTES,1<<I2C_CR2_NBYTES_Pos);
	START; 
	i2c_write_byte_wr(cell_addr); 
	READ_REQUEST;
	MODIFY_REG(I2C2->CR2,I2C_CR2_NBYTES,16<<I2C_CR2_NBYTES_Pos);
	START; 
	for(uint8_t i=0;i<=15;i++){
		*(page+i-1)=i2c_read_byte();
	}
	i2c_stop();
	
}