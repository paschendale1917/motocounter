#ifndef eeprom_H
#define eeprom_H

#include "stm32g0xx.h"
#include "../i2c/i2c.h"

#define EE24C02_ADDR              		0b10100000  //пятиногая еепромка

#define REQUEST_WRITE                   0x00
#define REQUEST_READ                    0x01

extern uint8_t eeprom_error;

void eeprom_read_data(uint8_t dev_addr,uint8_t cell_addr,uint8_t number_of_bytes, uint8_t *data); //медленное чтение
uint8_t eeprom_write_byte(uint8_t dev_addr, uint16_t cell_addr,uint8_t byte);
void eeprom_write_one_page(uint8_t dev_addr, uint8_t page_addr,uint8_t *data);
uint8_t eeprom_read_byte(uint8_t dev_addr,uint16_t cell_addr);
void eeprom_read_one_page(uint8_t dev_addr,uint8_t page_addr, uint8_t *page);
void eeprom_write_uint16(uint8_t dev_addr, uint16_t cell_addr,uint16_t word);
uint16_t eeprom_read_uint16(uint8_t dev_addr,uint16_t cell_addr);
void eeprom_write_some_bytes(uint8_t dev_addr,uint16_t cell_addr,uint8_t num_bytes,uint8_t *data);
void eeprom_init(uint8_t dev_addr);

#endif