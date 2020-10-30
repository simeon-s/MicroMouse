#ifndef COMMON_H_
#define COMMON_H_

#include "main.h"



void spi1_Write(unsigned char *p_Data, unsigned short uw_Size);
void spi1_WriteByte(unsigned char ub_Data);
void spi1_Read(unsigned char *p_Data, unsigned short uw_Size);
unsigned char spi1_ReadByte( void );

uint32_t adc_read( ADC_HandleTypeDef *hadc, uint32_t channel , uint32_t rank);

HAL_StatusTypeDef writeEEPROMByte(uint32_t address, uint8_t *data, uint32_t length);
uint8_t readEEPROMByte(uint32_t address , uint32_t lenth);

void eepr_buffRead(uint8_t*buff, uint32_t address, uint8_t len);
HAL_StatusTypeDef eepr_buffWrite(uint8_t *data, uint32_t address, uint8_t length);

void jumpToBootloader();


#endif /*COMMON_H_*/
