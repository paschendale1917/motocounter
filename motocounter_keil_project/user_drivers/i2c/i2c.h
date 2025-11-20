#ifndef i2c_H
#define i2c_H

#include "stm32g0xx.h"
#include "../SysTick/systick_delay.h"

#define CLEAR_NACK_FLAG                 SET_BIT(I2C2->ICR, I2C_ICR_NACKCF)
#define ACK                             !READ_BIT(I2C2->ISR, I2C_ISR_NACKF)
#define NACK                 			READ_BIT(I2C2->ISR,I2C_ISR_NACKF)
#define ENABLE_PERIF      			  	SET_BIT(I2C2->CR1, I2C_CR1_PE) 
#define DISABLE_PERIF 				    CLEAR_BIT(I2C2->CR1, I2C_CR1_PE)
#define WRITE_REQUEST             		CLEAR_BIT(I2C2->CR2, I2C_CR2_RD_WRN)
#define READ_REQUEST              		SET_BIT(I2C2->CR2, I2C_CR2_RD_WRN)
#define AUTOEND_EN                		SET_BIT(I2C2->CR2,I2C_CR2_AUTOEND)
#define AUTOEND_DIS               		CLEAR_BIT(I2C2->CR2,I2C_CR2_AUTOEND)
#define RELOAD_EN						SET_BIT(I2C2->CR2,I2C_CR2_RELOAD)
#define RELOAD_DIS						CLEAR_BIT(I2C2->CR2,I2C_CR2_RELOAD)
#define START               			SET_BIT(I2C2->CR2,I2C_CR2_START)
#define STOP                			SET_BIT(I2C2->CR2,I2C_CR2_STOP)

extern uint8_t i2c_error;


void i2c2_init(void);
void i2c_softreset(void);
void i2c_reinit();
void i2c_start( uint8_t dev_addr, uint16_t datasize);
void i2c_stop(void);
void i2c_write_byte(uint8_t byte);
uint8_t i2c_read_byte(void);
void i2c_write_byte_wr(uint8_t byte);


#endif