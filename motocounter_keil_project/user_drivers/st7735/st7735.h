#ifndef st77xx_H

#include "stm32g0xx.h"
#include "fonts.h"
#include "math.h"
#include <stdio.h>
#include <string.h>
#include "../spi/spi.h"
#include "../SysTick/systick_delay.h"
#include "../st7735_backlight/backlight.h"
#include "../ADC/ADC.h"

//#define STN_PORTRAIT
//#define STN_LANDSCAPE
//#define IPS_SQUARE
#define IPS_LANDSCAPE

#define COMMAND          												SET_BIT(GPIOA->BSRR,GPIO_BSRR_BR5)
#define DATA             												SET_BIT(GPIOA->BSRR,GPIO_BSRR_BS5)
#define RESET0           												SET_BIT(GPIOA->BSRR,GPIO_BSRR_BR3)
#define RESET1           												SET_BIT(GPIOA->BSRR,GPIO_BSRR_BS3)
#define START_Tx        												SET_BIT(GPIOA->BSRR,GPIO_BSRR_BR0) //chip select
#define STOP_Tx          												SET_BIT(GPIOA->BSRR,GPIO_BSRR_BS0)
#define ENABLE_SPI1														SET_BIT(SPI1->CR1,SPI_CR1_SPE)
#define DISABLE_SPI1													CLEAR_BIT(SPI1->CR1,SPI_CR1_SPE)

#define ST7735_HEIGHT                                                 	80
#define ST7735_WIDTH                                                  	160

#define ST7735_HEIGHT_OFFSET                                            1 //смещение китайского модуля по пикселам. на разных модулях по-разному
#define ST7735_WIDTH_OFFSET                                             26

#define WHITE     														0xFFFF
#define BLACK      														0x0000
#define BLUE        													0x001F
#define RED         													0xF800
#define MAGENTA     													0xF81F
#define GREEN       													0x07E0
#define CYAN        													0x7FFF
#define YELLOW      													0xFFE0
#define GRAY        													0X8430
#define BRED        													0XF81F
#define GRED       														0XFFE0
#define GBLUE      														0X07FF
#define BROWN      														0XBC40
#define BRRED       													0XFC07
#define DARKBLUE    													0X01CF
#define LIGHTBLUE   													0X7D7C
#define GRAYBLUE    													0X5458
#define ORANGE                                             				0xFA20

//макросы регистров контроллера дисплея st7735
#define ST7735_RDDMADCTL												0xB0
#define ST7735_NOP                                                      0x00
#define ST7735_SWRESET                                                  0x01
#define ST7735_RDDID                                                    0x04
#define ST7735_RDDST                                                    0x09
#define ST7735_SLPIN                                                    0x10
#define ST7735_SLPOUT                                                   0x11
#define ST7735_PTLON                                                    0x12
#define ST7735_NORON                                                    0x13
#define ST7735_INVOFF                                                   0x20
#define ST7735_INVON                                                    0x21
#define ST7735_DISPOFF                                                  0x28
#define ST7735_DISPON                                                   0x29
#define ST7735_CASET                                                    0x2A
#define ST7735_RASET                                                    0x2B
#define ST7735_RAMWR                                                    0x2C
#define ST7735_RAMRD                                                    0x2E
#define ST7735_PTLAR                                                    0x30
#define ST7735_COLMOD                                                   0x3A
#define ST7735_MADCTL                                                   0x36
#define ST7735_FRMCTR1                                                  0xB1
#define ST7735_FRMCTR2                                                  0xB2
#define ST7735_FRMCTR3                                                  0xB3
#define ST7735_INVCTR                                                   0xB4
#define ST7735_DISSET5                                                  0xB6
#define ST7735_RDDCOLMOD                                                0x0C
#define ST7735_PWCTR1                                                   0xC0
#define ST7735_PWCTR2                                                   0xC1
#define ST7735_PWCTR3                                                   0xC2
#define ST7735_PWCTR4                                                   0xC3
#define ST7735_PWCTR5                                                   0xC4
#define ST7735_VMCTR1                                                   0xC5
#define ST7735_RDID1                                                    0xDA
#define ST7735_RDID2                                                    0xDB
#define ST7735_RDID3                                                    0xDC
#define ST7735_RDID4                                                    0xDD
#define ST7735_PWCTR6                                                   0xFC
#define ST7735_GMCTRP1                                                  0xE0
#define ST7735_GMCTRN1                                                  0xE1
#define ST7735_SPI_TIMEOUT                                              100
#define MADCTL															0b01101000

#define BACKGROUND_ST7735(color)                                        draw_rect(0,0,ST7735_WIDTH,ST7735_HEIGHT,color)

#ifdef STN_PORTRAIT
	#define LCD_HEIGHT_SIZE                                                 160    //высота
	#define LCD_WIDTH_SIZE                                                  128		//ширина
	#define MADCTL															0b11000000
	#define LCD_WIDTH_OFFSET												0
	#define LCD_HEIGHT_OFFSET												0
#elif defined(STN_LANDSCAPE)
	#define LCD_HEIGHT_SIZE                                                 128    //высота
	#define LCD_WIDTH_SIZE                                                  160		//ширина
	#define MADCTL															0b01100000
	#define LCD_WIDTH_OFFSET												0
	#define LCD_HEIGHT_OFFSET												0
#elif defined(IPS_LANDSCAPE)
	#define LCD_HEIGHT_SIZE                                                 80    //высота
	#define LCD_WIDTH_SIZE                                                  160   //ширина
	#define MADCTL															0b01101000
	#define LCD_WIDTH_OFFSET												26
	#define LCD_HEIGHT_OFFSET												1
#elif defined(IPS_SQUARE)
	#define LCD_HEIGHT_SIZE                                                 128    //высота
	#define LCD_WIDTH_SIZE                                                  128   //ширина
	#define MADCTL															0b11001000
	#define LCD_WIDTH_OFFSET												1
	#define LCD_HEIGHT_OFFSET												2
#endif

#define BACKGROUND_COLOR                                                BLACK
#define FONT_COLOR        												BROWN

//#define BIGFONT_SPACE     												-4
//#define TINYFONT_SPACE    												1

#define TINYFONT

#ifdef BIGFONT
 #define SYSTEM_FONT       												BigFont
 #define SYSTEMFONT_SPACE 												BIGFONT_SPACE
#endif

#ifdef TINYFONT
	#define SYSTEM_FONT       											TinyFont
  #define SYSTEMFONT_SPACE 												TINYFONT_SPACE
#endif

void init_ports(void);
void st7735_init(uint8_t bright);
void init_st7735(void);
void draw_rect(uint16_t xstart, uint16_t ystart, uint16_t lenth, uint16_t height, uint16_t color);
void background(uint16_t color);
void draw_circle(uint8_t x, uint8_t y, uint8_t r, uint16_t color);
void draw_char(uint16_t xstart, uint16_t ystart, const char letter, uint16_t bcolor, uint16_t fcolor, uint8_t *font);
void draw_string(uint16_t xpos, uint16_t ypos, const char *string, int8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font);
void draw_number(uint16_t xpos,  uint16_t ypos, int32_t number, int8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font);
void draw_image(uint16_t x_start, uint16_t x_stop, uint16_t y_start, uint16_t y_stop, const unsigned short *image);
void draw_float_number(uint16_t xpos,  uint16_t ypos, float number,const char *dimens, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font);
uint8_t draw_2num_zero(uint16_t xpos,  uint16_t ypos, uint8_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font);
void draw_hexnumber(uint16_t xpos,  uint16_t ypos, int32_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font);
void clear_display(void);
void draw_border(uint16_t color);
void draw_clocks(uint16_t xpos,  uint16_t ypos, int32_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font);
void draw_minutes(uint16_t xpos,  uint16_t ypos, int32_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font);
void draw_hline(uint16_t xstart, uint16_t ystart, uint16_t lenth,  uint16_t color);
void draw_vline(uint16_t xstart, uint16_t ystart, uint16_t height,  uint16_t color);
void draw_voltage(uint16_t xpos,  uint16_t ypos,int8_t space,uint16_t bcolor,  uint16_t fcolor, uint8_t *font);
void draw_VDDA(uint16_t xpos,  uint16_t ypos,int8_t space,uint16_t bcolor,  uint16_t fcolor, uint8_t *font);
//void draw_counterhours(void);

#endif