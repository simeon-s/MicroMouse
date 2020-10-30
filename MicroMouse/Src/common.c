#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "main.h"

//------------------------------------------------------------------
//           Basic read and Write to SPI functions
//-----------------------------------------------------------------

// Transfer Data to SPI Interface
void spi1_WriteByte(unsigned char ub_Data) {
	HAL_SPI_Transmit(&hspi1, &ub_Data, 1, 10);
}

// Send a Byte to SPI interface
unsigned char spi1_ReadByte(void) {
	uint8_t ingoing;
	HAL_SPI_Receive(&hspi1, &ingoing, 1, 10);
	return ingoing;
}

// Transfer Data to SPI Interface
void spi1_Write(unsigned char *p_Data, unsigned short uw_Size) {
	HAL_SPI_Transmit(&hspi1, p_Data, uw_Size, 10);
}

// Transfer Data to SPI Interface
void spi1_Read(unsigned char *p_Data, unsigned short uw_Size) {
	HAL_SPI_Receive(&hspi1, p_Data, uw_Size, 10);
}

//--------------------------   ADC -----------------------------
uint32_t adc_read(ADC_HandleTypeDef *hadc1, uint32_t channel, uint32_t rank) {
	ADC_ChannelConfTypeDef sConfig = { 0 };
	uint32_t adc_value;

	sConfig.Channel = channel;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_6CYCLES_5;
	HAL_ADC_ConfigChannel(hadc1, &sConfig);

	HAL_ADC_Start(hadc1);
	HAL_ADC_PollForConversion(hadc1, 100);
	adc_value = HAL_ADC_GetValue(hadc1);
	HAL_ADC_Stop(hadc1);
	return adc_value;
}

//============================== EEPROM ========================================
/* Private define ------------------------------------------------------------*/

/* Base address of the Flash sectors */

#define ADDR_FLASH_PAGE_0     ((uint32_t)0x08000000) /* Base @ of Page 0, 2 Kbytes */
#define ADDR_FLASH_PAGE_1     ((uint32_t)0x08000800) /* Base @ of Page 1, 2 Kbytes */
#define ADDR_FLASH_PAGE_2     ((uint32_t)0x08001000) /* Base @ of Page 2, 2 Kbytes */
#define ADDR_FLASH_PAGE_3     ((uint32_t)0x08001800) /* Base @ of Page 3, 2 Kbytes */
#define ADDR_FLASH_PAGE_4     ((uint32_t)0x08002000) /* Base @ of Page 4, 2 Kbytes */
#define ADDR_FLASH_PAGE_5     ((uint32_t)0x08002800) /* Base @ of Page 5, 2 Kbytes */
#define ADDR_FLASH_PAGE_6     ((uint32_t)0x08003000) /* Base @ of Page 6, 2 Kbytes */
#define ADDR_FLASH_PAGE_7     ((uint32_t)0x08003800) /* Base @ of Page 7, 2 Kbytes */
#define ADDR_FLASH_PAGE_8     ((uint32_t)0x08004000) /* Base @ of Page 8, 2 Kbytes */
#define ADDR_FLASH_PAGE_9     ((uint32_t)0x08004800) /* Base @ of Page 9, 2 Kbytes */
#define ADDR_FLASH_PAGE_10    ((uint32_t)0x08005000) /* Base @ of Page 10, 2 Kbytes */
#define ADDR_FLASH_PAGE_11    ((uint32_t)0x08005800) /* Base @ of Page 11, 2 Kbytes */
#define ADDR_FLASH_PAGE_12    ((uint32_t)0x08006000) /* Base @ of Page 12, 2 Kbytes */
#define ADDR_FLASH_PAGE_13    ((uint32_t)0x08006800) /* Base @ of Page 13, 2 Kbytes */
#define ADDR_FLASH_PAGE_14    ((uint32_t)0x08007000) /* Base @ of Page 14, 2 Kbytes */
#define ADDR_FLASH_PAGE_15    ((uint32_t)0x08007800) /* Base @ of Page 15, 2 Kbytes */
#define ADDR_FLASH_PAGE_16    ((uint32_t)0x08008000) /* Base @ of Page 16, 2 Kbytes */
#define ADDR_FLASH_PAGE_17    ((uint32_t)0x08008800) /* Base @ of Page 17, 2 Kbytes */
#define ADDR_FLASH_PAGE_18    ((uint32_t)0x08009000) /* Base @ of Page 18, 2 Kbytes */
#define ADDR_FLASH_PAGE_19    ((uint32_t)0x08009800) /* Base @ of Page 19, 2 Kbytes */
#define ADDR_FLASH_PAGE_20    ((uint32_t)0x0800A000) /* Base @ of Page 20, 2 Kbytes */
#define ADDR_FLASH_PAGE_21    ((uint32_t)0x0800A800) /* Base @ of Page 21, 2 Kbytes */
#define ADDR_FLASH_PAGE_22    ((uint32_t)0x0800B000) /* Base @ of Page 22, 2 Kbytes */
#define ADDR_FLASH_PAGE_23    ((uint32_t)0x0800B800) /* Base @ of Page 23, 2 Kbytes */
#define ADDR_FLASH_PAGE_24    ((uint32_t)0x0800C000) /* Base @ of Page 24, 2 Kbytes */
#define ADDR_FLASH_PAGE_25    ((uint32_t)0x0800C800) /* Base @ of Page 25, 2 Kbytes */
#define ADDR_FLASH_PAGE_26    ((uint32_t)0x0800D000) /* Base @ of Page 26, 2 Kbytes */
#define ADDR_FLASH_PAGE_27    ((uint32_t)0x0800D800) /* Base @ of Page 27, 2 Kbytes */
#define ADDR_FLASH_PAGE_28    ((uint32_t)0x0800E000) /* Base @ of Page 28, 2 Kbytes */
#define ADDR_FLASH_PAGE_29    ((uint32_t)0x0800E800) /* Base @ of Page 29, 2 Kbytes */
#define ADDR_FLASH_PAGE_30    ((uint32_t)0x0800F000) /* Base @ of Page 30, 2 Kbytes */
#define ADDR_FLASH_PAGE_31    ((uint32_t)0x0800F800) /* Base @ of Page 31, 2 Kbytes */
#define ADDR_FLASH_PAGE_32    ((uint32_t)0x08010000) /* Base @ of Page 32, 2 Kbytes */
#define ADDR_FLASH_PAGE_33    ((uint32_t)0x08010800) /* Base @ of Page 33, 2 Kbytes */
#define ADDR_FLASH_PAGE_34    ((uint32_t)0x08011000) /* Base @ of Page 34, 2 Kbytes */
#define ADDR_FLASH_PAGE_35    ((uint32_t)0x08011800) /* Base @ of Page 35, 2 Kbytes */
#define ADDR_FLASH_PAGE_36    ((uint32_t)0x08012000) /* Base @ of Page 36, 2 Kbytes */
#define ADDR_FLASH_PAGE_37    ((uint32_t)0x08012800) /* Base @ of Page 37, 2 Kbytes */
#define ADDR_FLASH_PAGE_38    ((uint32_t)0x08013000) /* Base @ of Page 38, 2 Kbytes */
#define ADDR_FLASH_PAGE_39    ((uint32_t)0x08013800) /* Base @ of Page 39, 2 Kbytes */
#define ADDR_FLASH_PAGE_40    ((uint32_t)0x08014000) /* Base @ of Page 40, 2 Kbytes */
#define ADDR_FLASH_PAGE_41    ((uint32_t)0x08014800) /* Base @ of Page 41, 2 Kbytes */
#define ADDR_FLASH_PAGE_42    ((uint32_t)0x08015000) /* Base @ of Page 42, 2 Kbytes */
#define ADDR_FLASH_PAGE_43    ((uint32_t)0x08015800) /* Base @ of Page 43, 2 Kbytes */
#define ADDR_FLASH_PAGE_44    ((uint32_t)0x08016000) /* Base @ of Page 44, 2 Kbytes */
#define ADDR_FLASH_PAGE_45    ((uint32_t)0x08016800) /* Base @ of Page 45, 2 Kbytes */
#define ADDR_FLASH_PAGE_46    ((uint32_t)0x08017000) /* Base @ of Page 46, 2 Kbytes */
#define ADDR_FLASH_PAGE_47    ((uint32_t)0x08017800) /* Base @ of Page 47, 2 Kbytes */
#define ADDR_FLASH_PAGE_48    ((uint32_t)0x08018000) /* Base @ of Page 48, 2 Kbytes */
#define ADDR_FLASH_PAGE_49    ((uint32_t)0x08018800) /* Base @ of Page 49, 2 Kbytes */
#define ADDR_FLASH_PAGE_50    ((uint32_t)0x08019000) /* Base @ of Page 50, 2 Kbytes */
#define ADDR_FLASH_PAGE_51    ((uint32_t)0x08019800) /* Base @ of Page 51, 2 Kbytes */
#define ADDR_FLASH_PAGE_52    ((uint32_t)0x0801A000) /* Base @ of Page 52, 2 Kbytes */
#define ADDR_FLASH_PAGE_53    ((uint32_t)0x0801A800) /* Base @ of Page 53, 2 Kbytes */
#define ADDR_FLASH_PAGE_54    ((uint32_t)0x0801B000) /* Base @ of Page 54, 2 Kbytes */
#define ADDR_FLASH_PAGE_55    ((uint32_t)0x0801B800) /* Base @ of Page 55, 2 Kbytes */
#define ADDR_FLASH_PAGE_56    ((uint32_t)0x0801C000) /* Base @ of Page 56, 2 Kbytes */
#define ADDR_FLASH_PAGE_57    ((uint32_t)0x0801C800) /* Base @ of Page 57, 2 Kbytes */
#define ADDR_FLASH_PAGE_58    ((uint32_t)0x0801D000) /* Base @ of Page 58, 2 Kbytes */
#define ADDR_FLASH_PAGE_59    ((uint32_t)0x0801D800) /* Base @ of Page 59, 2 Kbytes */
#define ADDR_FLASH_PAGE_60    ((uint32_t)0x0801E000) /* Base @ of Page 60, 2 Kbytes */
#define ADDR_FLASH_PAGE_61    ((uint32_t)0x0801E800) /* Base @ of Page 61, 2 Kbytes */
#define ADDR_FLASH_PAGE_62    ((uint32_t)0x0801F000) /* Base @ of Page 62, 2 Kbytes */
#define ADDR_FLASH_PAGE_63    ((uint32_t)0x0801F800) /* Base @ of Page 63, 2 Kbytes */

#define FLASH_USER_START_ADDR   ADDR_FLASH_PAGE_31   /* Start @ of user Flash area */
#define FLASH_USER_END_ADDR     ADDR_FLASH_PAGE_31 + FLASH_PAGE_SIZE - 1   /* End @ of user Flash area */
#define FLASH_USER_END_ADDR1	ADDR_FLASH_PAGE_31 + 0x7f0

/*Variable used for Erase procedure*/

/* Private function prototypes -----------------------------------------------*/
/**
 * @brief  Gets the page of a given address
 * @param  Addr: Address of the FLASH Memory
 * @retval The page of a given address
 */
static uint32_t GetPage(uint32_t Addr) {
	uint32_t page = 0;

	if (Addr < (FLASH_BASE + FLASH_BANK_SIZE)) {
		/* Bank 1 */
		page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
	} else {
		/* Bank 2 */
		page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;
	}

	return page;
}

//--------------------------- EEPROM -------------------------
//HAL_StatusTypeDef writeEEPROMByte(uint32_t address, uint8_t *data, uint32_t length) {
HAL_StatusTypeDef eepr_buffWrite(uint8_t *data, uint32_t address, uint8_t length) {
	HAL_StatusTypeDef status;
	uint32_t FirstPage = 0;
	uint32_t Address = 0, PAGEError = 0;
	__IO uint32_t data32 = 0, MemoryProgramStatus = 0;
	FLASH_EraseInitTypeDef EraseInitStruct;
	__IO uint8_t buff[FLASH_PAGE_SIZE];
	__IO uint64_t *flashData;
	__IO uint32_t *ramData;
	uint8_t *pBuff;

	/*  Preserve the content of the page */
	Address = FLASH_USER_START_ADDR;
	ramData = (uint32_t*)buff;
	while (Address < FLASH_USER_END_ADDR1) {
		*ramData = *(__IO uint32_t *) Address;
		Address = Address + 4;
		ramData ++;
	}

	/*  Update  page content   */
	pBuff = (uint8_t*)&buff[address];
	while (length) {
		*pBuff = *data;
		data++;
		length--;
		pBuff++;
	}

	/* Unlock the Flash to enable the flash control register access **/
	HAL_FLASH_Unlock();

	/* Get the 1st page to erase */
	FirstPage = GetPage(FLASH_USER_START_ADDR);

	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks = FLASH_BANK_1;
	EraseInitStruct.Page = FirstPage;
	EraseInitStruct.NbPages = 1;

	/* Note: If an erase operation in Flash memory also concerns data in the data or instruction cache,
	 you have to make sure that these data are rewritten before they are accessed during code
	 execution. If this cannot be done safely, it is recommended to flush the caches by setting the
	 DCRST and ICRST bits in the FLASH_CR register. */
	if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) {
		status = HAL_ERROR;
		return status;
	}

	/* Program the user Flash area word by word
	 (area defined by FLASH_USER_START_ADDR and FLASH_USER_END_ADDR) ***********/

	Address = FLASH_USER_START_ADDR;
	flashData = (uint64_t *)&buff[0];
	while (Address < FLASH_USER_END_ADDR) {
		if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Address, *flashData) == HAL_OK) {
			Address = Address + 8;
			flashData++;
		} else {
			/* Error occurred while writing data in Flash memory. */
			status = HAL_ERROR;
			return status;
		}
	}

	/* Lock the Flash to disable the flash control register access (recommended
	 to protect the FLASH memory against possible unwanted operation) *********/
	HAL_FLASH_Lock();

	/* Check if the programmed data is OK
	 MemoryProgramStatus = 0: data programmed correctly
	 MemoryProgramStatus != 0: number of words not programmed correctly ******/
	Address = FLASH_USER_START_ADDR;
	MemoryProgramStatus = 0x0;
	ramData = (uint32_t *) &buff[0];
	//while (Address < FLASH_USER_END_ADDR) {
	while (Address < FLASH_USER_END_ADDR1) {
		data32 = *(__IO uint32_t *) Address;

		if ( data32 != *ramData) {
			MemoryProgramStatus++;
		}
		Address = Address + 4;
		ramData++;
	}

	/*Check if there is an issue to program data*/
	if (MemoryProgramStatus != 0) {
		/* Error detected.  */
		status = HAL_ERROR;
		return status;
	}

	return HAL_OK;
}

void eepr_buffRead(uint8_t*buff, uint32_t address, uint8_t length) {
	uint32_t Address = 0;
	__IO uint32_t data32 = 0;

	Address = FLASH_USER_START_ADDR + address;
	while (length) {
		data32 = *(__IO uint32_t *) Address;
		*buff = (uint8_t) data32;
		Address = Address + 1;
		buff++;
		length--;
	}
}

//
