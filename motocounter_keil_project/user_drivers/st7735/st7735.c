#include "st7735.h"

const uint8_t frame_ctrl_data[] = {0x01, 0x2C, 0x2D};
const uint8_t pwctr1_data[] = {0xA2, 0x02, 0x84};
const uint8_t gmctrp1_data[] = {
	0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
	0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10
};

const uint8_t gmctrn1_data[] = {
	0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
	0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10
};

volatile uint16_t endoftime=250; //переменная, хранящая значение счетчика моточасов,при котором будет подаваться сигнал

void init_ports(void){
	SET_BIT(RCC->IOPENR,RCC_IOPENR_GPIOAEN);
	
	//reset PA3
	SET_BIT(GPIOA->MODER, GPIO_MODER_MODE3_0);
	CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE3_1);//output
	CLEAR_BIT(GPIOA->OTYPER,GPIO_OTYPER_OT3);//push/pull
	GPIOA->OSPEEDR|=GPIO_OSPEEDR_OSPEED3_Msk;
	
	//data/command PA5
	SET_BIT(GPIOA->MODER, GPIO_MODER_MODE5_0);
	CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE5_1);//output
	CLEAR_BIT(GPIOA->OTYPER,GPIO_OTYPER_OT5);//push/pull
	GPIOA->OSPEEDR|=GPIO_OSPEEDR_OSPEED5_Msk;
	
	//chip select PA0
	SET_BIT(GPIOA->MODER, GPIO_MODER_MODE0_0);
	CLEAR_BIT(GPIOA->MODER, GPIO_MODER_MODE0_1);//output
	CLEAR_BIT(GPIOA->OTYPER,GPIO_OTYPER_OT0);//push/pull
	GPIOA->OSPEEDR|=GPIO_OSPEEDR_OSPEED0_Msk;

}

void send_command(uint8_t command){
	COMMAND;
	spi_sendbyte(command);
}

void send_data(uint8_t data){
	
	DATA;
	spi_sendbyte(data);
}

void send_16bit_data(uint16_t data){
	
	DATA;
	spi_sendword(data);
}

void st7735_multiply_send(const uint8_t *data, uint32_t num){
	DATA;
	for (uint32_t i = 0; i < num; i++)
	{
		send_data(*(data+i));	
	}
}

void st7735_init(uint8_t bright){
	init_backlight();
	set_brigtness(bright);
	init_ports();
	init_master_spi();
	START_Tx;
	RESET1;
	_delay_ms(5);
	RESET0;
	_delay_ms(5);
	RESET1;
	_delay_ms(5);
	send_command(ST7735_SWRESET);
	_delay_ms(10);
	send_command(ST7735_SLPOUT);
	_delay_ms(5);
	send_command(ST7735_FRMCTR1);
	st7735_multiply_send(frame_ctrl_data, sizeof(frame_ctrl_data));
	send_command(ST7735_INVCTR);
	send_data(0x07);
	send_command(ST7735_PWCTR1);
	st7735_multiply_send(pwctr1_data, sizeof(pwctr1_data));
	send_command(ST7735_VMCTR1);
	send_data(0x0E);
#ifdef STN_PORTRAIT
	send_command(ST7735_INVOFF);
#elif defined(STN_LANDSCAPE)
	send_command(ST7735_INVOFF);
#elif defined(IPS_LANDSCAPE)
	send_command(ST7735_INVON);
#elif defined(IPS_SQUARE)
	send_command(ST7735_INVON);
#endif
	send_command(ST7735_INVON);
	send_command(ST7735_MADCTL);
	send_data(MADCTL); 
	send_command(ST7735_COLMOD);
	send_data(0x05);
	send_command(ST7735_GMCTRP1);
	st7735_multiply_send(gmctrp1_data, sizeof(gmctrp1_data));
	send_command(ST7735_GMCTRN1);
	st7735_multiply_send(gmctrn1_data, sizeof(gmctrn1_data));
	send_command(ST7735_NORON);
	_delay_ms(ST7735_SPI_TIMEOUT);
	send_command(ST7735_DISPON);
	_delay_ms(ST7735_SPI_TIMEOUT);
	STOP_Tx;
}


 void setXY(uint16_t xstart,  uint16_t xstop, uint16_t ystart,  uint16_t ystop){
	send_command(ST7735_RASET);
	send_data((ystart + ST7735_WIDTH_OFFSET) >> 8);
	send_data((uint8_t)ystart + ST7735_WIDTH_OFFSET);
	send_data((ystart + ystop + ST7735_WIDTH_OFFSET - 1) >> 8);
	send_data((uint8_t)ystart + ystop + ST7735_WIDTH_OFFSET - 1);

	send_command(ST7735_CASET);
	send_data((xstart + ST7735_HEIGHT_OFFSET) >> 8);
	send_data((uint8_t)xstart + ST7735_HEIGHT_OFFSET);
	send_data((xstart + xstop + ST7735_HEIGHT_OFFSET - 1) >> 8);
	send_data((uint8_t)xstart + xstop + ST7735_HEIGHT_OFFSET - 1);
}


void draw_rect(uint16_t xstart, uint16_t ystart, uint16_t lenth, uint16_t height, uint16_t color){
	START_Tx;
	setXY(xstart, lenth, ystart, height);
	send_command(ST7735_RAMWR);
	DATA;
	for(uint32_t i = 0; i < lenth * height; i++) {
		spi_sendword(color);
	}

	STOP_Tx;
}

void draw_hline(uint16_t xstart, uint16_t ystart, uint16_t lenth,  uint16_t color){
	draw_rect( xstart, ystart, lenth,2,color);
}

void draw_vline(uint16_t xstart, uint16_t ystart, uint16_t height,  uint16_t color){
	draw_rect( xstart, ystart, 2,height,color);
}

void draw_border(uint16_t color){
	draw_hline(0,0,ST7735_WIDTH,color);
	draw_hline(0,ST7735_HEIGHT-2,ST7735_WIDTH,color);
	draw_vline(0,0,ST7735_HEIGHT ,color);
	draw_vline(ST7735_WIDTH-2,0,ST7735_HEIGHT,color);
}

void background(uint16_t color){
	draw_rect(0,0,ST7735_WIDTH ,ST7735_HEIGHT,color);
}

void clear_display(void){
	background(BACKGROUND_COLOR);
}

//void draw_image(uint16_t x_start, uint16_t x_stop, uint16_t y_start, uint16_t y_stop, const uint16_t *image){
//	uint16_t s=0;
//	START_Tx;
//	setXY(x_start,x_stop,y_start,y_stop);
//	send_command(ST7735_RAMWR);
//	DATA;
//	while(s++ < x_stop*y_stop){
//		spi_sendbyte((uint8_t)image[s]);
//		spi_sendbyte((uint8_t)(image[s]>>8));
//	}
//	STOP_Tx;
//}


void draw_image(uint16_t x_start, uint16_t y_start, uint16_t height, uint16_t lenth,  const unsigned short *image){
	START_Tx;
	setXY(x_start, height, y_start, lenth);
	send_command(ST7735_RAMWR);
	for(uint32_t i = 0; i < lenth * height; i++) {
		send_16bit_data(*(image+i));     
	}
	STOP_Tx;
}

void draw_char(uint16_t xstart, uint16_t ystart, const char letter, uint16_t bcolor,uint16_t fcolor,uint8_t *font) {
	
	// Получаем параметры шрифта
	const uint8_t data_w = font[0] / 8;     // ширина в байтах
	const uint8_t letter_height = *(font + 1);  // высота символа
	const uint8_t letter_width = data_w * 8;            // ширина символа в пикселях
	const uint8_t ascii_offset = *(font + 2);  // смещение ASCII
	
	// Вычисляем смещение символа в массиве
	const uint16_t offset = (letter - ascii_offset) * letter_height * data_w + 4;
	const uint16_t lettersize = letter_height * data_w;
	
	// Буфер для хранения данных символа
	uint8_t letter_buf[400] = {0};
	
	// Считываем данные символа из памяти
	for(uint16_t i = 0; i < lettersize; i++) {
		letter_buf[i] = *(font + offset + i);
	}
	
	// Устанавливаем позицию и режим записи
	START_Tx;
	setXY(xstart, letter_width, ystart, letter_height);  // Указываем реальные размеры символа
	send_command(ST7735_RAMWR);
	DATA;
	
	for(uint16_t y = 0; y < letter_height; y++) {
		for(uint16_t x = 0; x < letter_width; x += 8) {
			uint8_t data = letter_buf[y * data_w + x / 8];
			for(uint8_t bit = 0; bit < 8; bit++) {
				if (x + bit >= letter_width) break;			// Защита от выхода за границы
				spi_sendbyte((data & 0x80) ? (fcolor >> 8) : (bcolor >> 8));
				spi_sendbyte((data & 0x80) ? fcolor : bcolor);
				data <<= 1;
			}
		}
	}
	
	STOP_Tx;
}

void draw_string(uint16_t xpos, uint16_t ypos, const char *string, int8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	while(*string != 0) {
		draw_char(xpos, ypos, *string++, bcolor, fcolor, font);
		xpos += (uint16_t)(font[0] + space);
	}
}

void draw_dot(uint16_t x, uint16_t y, uint16_t color)
{
	START_Tx;
	setXY(x, 0, y, 1);
	send_command(ST7735_RAMWR);
	DATA;
	send_data((uint8_t)(color>>8));
	send_data((uint8_t)color);
	STOP_Tx;
}

void draw_circle(uint8_t x, uint8_t y, uint8_t r, uint16_t color){
	int16_t f = 1 - r;
	int16_t x0 = 0;
	int16_t y0 = r;
	while(x0 <= y0) {
		draw_dot(x + x0, y + y0, color);
		draw_dot(x - x0, y + y0, color);
		draw_dot(x + x0, y - y0, color);
		draw_dot(x - x0, y - y0, color);

		draw_dot(x + y0, y + x0, color);
		draw_dot(x - y0, y + x0, color);
		draw_dot(x + y0, y - x0, color);
		draw_dot(x - y0, y - x0, color);
		x0++;
		if(f > 0) {
			y0--;
			f += 2 * (x0 - y0) + 1;
		} else{
			 f += 2 * x0 + 1; }
	}
}

void draw_number(uint16_t xpos,  uint16_t ypos, int32_t number, int8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	char buf[10] = {0};
	uint8_t len = sprintf(buf, "%u", number);					//определяем длину числа
	//memset(buf + len, ' ', sizeof(buf) - len - 1);				// очищаем лишние позиции                                                                                   
	draw_string(xpos,ypos,buf,space,bcolor,fcolor,font);
}

void draw_hexnumber(uint16_t xpos,  uint16_t ypos, int32_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
    char buf[10] = {0};
	sprintf(buf, "%s%x","0x", number);
	draw_string(xpos, ypos, buf, space, bcolor, fcolor, font);
}

//draw_float_number(60,70,84.67,"%0.2f",0,BACKGROUND_COLOR,GREEN,TinyFont);
void draw_float_number(uint16_t xpos,  uint16_t ypos, float number, const char *dimens, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	char buf[10] = {0};
	sprintf(buf, dimens, number);
	draw_string(xpos, ypos, buf, space, bcolor, fcolor, font);
}

/*потом поковырять*/
uint8_t draw_2num_zero(uint16_t xpos,  uint16_t ypos, uint8_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	if(number > 9 || number < 1)
	{ return 0; }

	uint8_t tens, units;
	int32_t temp_num = number;
	int32_t *pt_num;
	pt_num = &temp_num;
	int32_t num = 0;
	num = *pt_num;

	char buf_in[2] = {0}; //разделение числа на цифры и складирование в буфер
	char buf_out[2] = {0};
	num = *pt_num;
	tens = num / 10;
	units = num % 10;
	buf_in[0] = tens;
	buf_in[1] = units;

	for(uint8_t j = 0; j < 2; j++) {   //преобразование буфера в строку
		sprintf(&buf_out[j], "%d", buf_in[j]);
	}


	for(uint8_t y = 0; y < 2; y++) {   //вывод строки с положительным числом
		draw_char(xpos, ypos, *(buf_out + y), bcolor, fcolor, font);
		ypos += font[0] + space;
	}

	return 1;
}



void draw_clocks(uint16_t xpos,  uint16_t ypos, int32_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	draw_number(xpos + (font[0] + space) * 2, ypos, number % 10, space, bcolor, fcolor, font);
	draw_number(xpos + font[0] + space, ypos, number / 10 % 10, space, bcolor, fcolor, font);
	draw_number(xpos, ypos, number / 100 % 10, space, bcolor, fcolor, font);
}

void draw_minutes(uint16_t xpos,  uint16_t ypos, int32_t number, uint8_t space, uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	draw_number(xpos + font[0] + space, ypos, number % 10, space, bcolor, fcolor, font);
	draw_number(xpos, ypos, number / 10 % 10, space, bcolor, fcolor, font);
}

void draw_VDDA(uint16_t xpos,  uint16_t ypos,int8_t space,uint16_t bcolor,  uint16_t fcolor, uint8_t *font){
	char *n="v";		
	float Uref_local=0;
	Uref_local=get_refvoltage(); //3 в-напряжение, при котором на заводе измерили VREFINT_CAL и записали в память МК//здесь идет расчет напряжения питания;
	draw_float_number(xpos,ypos,Uref_local,"2",space,bcolor, fcolor, font);
	draw_char(xpos+2*font[0]+8,ypos, *n,bcolor,fcolor,font);
}

void draw_voltage(uint16_t xpos,  uint16_t ypos,int8_t space,uint16_t bcolor,  uint16_t fcolor, uint8_t *font){	
	uint8_t nt=0;
	voltage=((REFERENCE/4096)*get_adcdata_dma())/divider;
	draw_float_number(xpos,ypos,voltage,"%0.2f",space,bcolor, fcolor, font);
	if(voltage>=10.0){
		nt=4;
	} else if(voltage<10.0){
		nt=3;
		draw_rect(xpos+4*font[1]+4,ypos,font[0],font[1],bcolor);
	}
	draw_char(xpos+nt*font[0]+4,ypos, 'v',bcolor,fcolor,font);
	  
}

