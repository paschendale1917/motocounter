//0-24 вольт; 24 вольт == 3.32 после делителя.

#ifndef ADC_H
#define ADC_H

#include "stm32g0xx.h"

#define TS_CAL1     					*((uint16_t*)0x1FFF75A8) //измерено на заводе при питании 3в/ нужно пересчитать на питание 3.3в: TS_CAL1 =(3.3/3)=1.1
#define VREFINT_CAL    					*((uint16_t*)0x1FFF75AA)
#define TEMP30_CAL_ADDR  				((uint16_t*) ((uint32_t) 0x1FFF75A8))
#define VREFINT_CAL_ADDR 				((uint16_t*) ((uint32_t) 0x1FFF75AA))

#define REFERENCE            			3.37		//напряжение, измеренное на делителе
#define REAL_VOLTAGE         			24 		//напряжение, приходящее на делитель с учетом падения напряжения на светодиоде индикации и защитном диоде
#define MAX_MEASURED_VOLTAGE 			24         //максимальное напряжение, которое держит схема
#define DIFFERENCE           			(MAX_MEASURED_VOLTAGE-REAL_VOLTAGE)

#define ALFHA              				0.1


extern volatile float divider;
extern float Uref;
extern float voltage;
extern volatile uint16_t adc_data[100];


void adc_calib(void);
void init_adc(void);
uint16_t get_adc_data_cont(void);
uint16_t get_adc_data_single(void);
uint16_t get_temp(void);
void draw_voltage(uint16_t xpos,  uint16_t ypos,int8_t space,uint16_t bcolor,  uint16_t fcolor, uint8_t *font);
float  get_refvoltage(void);
uint16_t get_intref_ch13(void);
void draw_VDDA(uint16_t xpos,  uint16_t ypos,int8_t space,uint16_t bcolor,  uint16_t fcolor, uint8_t *font);
uint16_t get_sensor_ch12(void);
float get_voltage(void);

void init_adc_dma(void);
uint16_t get_adcdata_dma(void);
float get_voltage_dma(void);
void draw_voltage_dma(uint16_t xpos,  uint16_t ypos,int8_t space,uint16_t bcolor,  uint16_t fcolor, uint8_t *font);
#endif