#ifndef spi_H

#include "stm32g0xx.h"

#define WAIT_LAST_BYTE                    while((SPI1->SR & SPI_SR_BSY)) 								//если не ожидать передачи последнего байта в посылке, то чип селект поднимется раньше, чем последний байт приплывет в память дисплея

#define ENABLE(SPIx)                      SET_BIT(SPIx->CR1,SPI_CR1_SPE)
#define DISABLE(SPIx)                     CLEAR_BIT(SPIx->CR1,SPI_CR1_SPE)

#define DATASIZE_8(SPIx)                  MODIFY_REG(SPI1->CR2,SPI_CR2_DS,0x07<<SPI_CR2_DS_Pos);SET_BIT(SPI1->CR2,SPI_CR2_FRXTH)
#define DATASIZE_16(SPIx)                 MODIFY_REG(SPI1->CR2,SPI_CR2_DS,0x0f<<SPI_CR2_DS_Pos);CLEAR_BIT(SPI1->CR2,SPI_CR2_FRXTH)
#define SET_DATASIZE_8(SPIx)              DISABLE(SPIx);DATASIZE_8(SPIx);ENABLE(SPIx)
#define SET_DATASIZE_16(SPIx)             DISABLE(SPIx);DATASIZE_16(SPIx);ENABLE(SPIx)

#define ENABLE_DMA_TX(SPIx)               SET_BIT(SPIx->CR2,SPI_CR2_TXDMAEN)
#define DISABLE_DMA_TX(SPIx)              CLEAR_BIT(SPIx->CR2,SPI_CR2_TXDMAEN)
#define ENABLE_DMA_RX(SPIx)               SET_BIT(SPIx->CR2,SPI_CR2_RXDMAEN)
#define DISABLE_DMA_RX(SPIx)              CLEAR_BIT(SPIx->CR2,SPI_CR2_RXDMAEN)

#define ENABLE_SOFT_SSM(SPIx)             SET_BIT(SPIx->CR1,SPI_CR1_SSM|SPI_CR1_SSI)
#define ENABLE_MASTER(SPIx)               SET_BIT(SPIx->CR1,SPI_CR1_MSTR)

#define ENABLE_FULLDUPLEX(SPIx)           CLEAR_BIT(SPIx->CR1,SPI_CR1_RXONLY)
#define ENABLE_RXONLY(SPIx)               SET_BIT(SPIx->CR1,SPI_CR1_RXONLY)

#define SPI_MODE_0(SPIx)                  CLEAR_BIT(SPIx->CR1,SPI_CR1_CPOL|SPI_CR1_CPHA)
#define SPI_MODE_1(SPIx)                  SET_BIT(SPIx->CR1,SPI_CR1_CPOL);CLEAR_BIT(SPIx->CR1,SPI_CR1_CPHA)
#define SPI_MODE_2(SPIx)                  CLEAR_BIT(SPIx->CR1,SPI_CR1_CPOL);SET_BIT(SPIx->CR1,SPI_CR1_CPHA)
#define SPI_MODE_3(SPIx)                  SET_BIT(SPIx->CR1,SPI_CR1_CPOL|SPI_CR1_CPHA)

#define MSBFIRST(SPIx)                    CLEAR_BIT(SPIx->CR1,SPI_CR1_LSBFIRST)
#define LSBFIRST(SPIx)                    SET_BIT(SPIx->CR1,SPI_CR1_LSBFIRST)
#define SET_LSBFIRST(SPIx)				  DISABLE(SPIx);LSB_FIRST(SPIx);ENABLE(SPIx)
#define SET_MSBFIRST(SPIx)				  DISABLE(SPIx);LSB_FIRST(SPIx);ENABLE(SPIx)

#define BAUDRATE_2(SPIx)                  MODIFY_REG(SPIx->CR1,SPI_CR1_BR,0x00<<SPI_CR1_BR_Pos);
#define BAUDRATE_4(SPIx)                  MODIFY_REG(SPIx->CR1,SPI_CR1_BR,0x01<<SPI_CR1_BR_Pos);
#define BAUDRATE_8(SPIx)                  MODIFY_REG(SPIx->CR1,SPI_CR1_BR,0x02<<SPI_CR1_BR_Pos);
#define BAUDRATE_16(SPIx)                 MODIFY_REG(SPIx->CR1,SPI_CR1_BR,0x03<<SPI_CR1_BR_Pos);
#define BAUDRATE_32(SPIx)                 MODIFY_REG(SPIx->CR1,SPI_CR1_BR,0x04<<SPI_CR1_BR_Pos);
#define BAUDRATE_64(SPIx)                 MODIFY_REG(SPIx->CR1,SPI_CR1_BR,0x05<<SPI_CR1_BR_Pos);
#define BAUDRATE_128(SPIx)                MODIFY_REG(SPIx->CR1,SPI_CR1_BR,0x06<<SPI_CR1_BR_Pos);
#define BAUDRATE_256(SPIx)                MODIFY_REG(SPIx->CR1,SPI_CR1_BR,0x07<<SPI_CR1_BR_Pos);

#define DATAREG(SPIx)                     SPIx->DR

#define HALF_TRANSFER_COMPLETE            DMA2->HISR&DMA_HISR_HTIF5
#define TOTAL_TRANSFER_COMPLETE           DMA2->HISR&DMA_HISR_TCIF5
#define RESET_HT_FLAG                     SET_BIT(DMA2->HIFCR,DMA_HIFCR_CHTIF5)
#define RESET_TC_FLAG                     SET_BIT(DMA2->HIFCR,DMA_HIFCR_CTCIF5)

void init_master_spi(void);
uint8_t spi_sendbyte(uint8_t byte);
uint16_t spi_sendword(uint16_t word);

#endif