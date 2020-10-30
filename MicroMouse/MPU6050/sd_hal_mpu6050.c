/*
 * sd_hal_mpu6050.c
 *
 *  Created on: Feb 19, 2016
 *      Author: Sina Darvishi
 */

/**
 * |----------------------------------------------------------------------
 * | Copyright (C) Sina Darvishi,2016
 * |
 * | This program is free software: you can redistribute it and/or modify
 * | it under the terms of the GNU General Public License as published by
 * | the Free Software Foundation, either version 3 of the License, or
 * | any later version.
 * |
 * | This program is distributed in the hope that it will be useful,
 * | but WITHOUT ANY WARRANTY; without even the implied warranty of
 * | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * | GNU General Public License for more details.
 * |
 * | You should have received a copy of the GNU General Public License
 * | along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * |----------------------------------------------------------------------
 */

#include "main.h"
#include "sd_hal_mpu6050.h"
#include "hw_mpu6050.h"

/* Default I2C address */
#define MPU6050_I2C_ADDR			0xD0

/* Who I am register value */
#define MPU6050_I_AM				0x68

/* MPU6050 registers */
#define MPU6050_AUX_VDDIO			0x01
#define MPU6050_SMPLRT_DIV			0x19
#define MPU6050_CONFIG				0x1A
#define MPU6050_GYRO_CONFIG			0x1B
#define MPU6050_ACCEL_CONFIG		0x1C
#define MPU6050_MOTION_THRESH		0x1F
#define MPU6050_INT_PIN_CFG			0x37
#define MPU6050_INT_ENABLE			0x38
#define MPU6050_INT_STATUS			0x3A
#define MPU6050_ACCEL_XOUT_H		0x3B
#define MPU6050_ACCEL_XOUT_L		0x3C
#define MPU6050_ACCEL_YOUT_H		0x3D
#define MPU6050_ACCEL_YOUT_L		0x3E
#define MPU6050_ACCEL_ZOUT_H		0x3F
#define MPU6050_ACCEL_ZOUT_L		0x40
#define MPU6050_TEMP_OUT_H			0x41
#define MPU6050_TEMP_OUT_L			0x42
#define MPU6050_GYRO_XOUT_H			0x43
#define MPU6050_GYRO_XOUT_L			0x44
#define MPU6050_GYRO_YOUT_H			0x45
#define MPU6050_GYRO_YOUT_L			0x46
#define MPU6050_GYRO_ZOUT_H			0x47
#define MPU6050_GYRO_ZOUT_L			0x48
#define MPU6050_MOT_DETECT_STATUS	0x61
#define MPU6050_SIGNAL_PATH_RESET	0x68
#define MPU6050_MOT_DETECT_CTRL		0x69
#define MPU6050_USER_CTRL			0x6A
#define MPU6050_PWR_MGMT_1			0x6B
#define MPU6050_PWR_MGMT_2			0x6C
#define MPU6050_FIFO_COUNTH			0x72
#define MPU6050_FIFO_COUNTL			0x73
#define MPU6050_FIFO_R_W			0x74
#define MPU6050_WHO_AM_I			0x75

/* Gyro sensitivities in degrees/s */
#define MPU6050_GYRO_SENS_250		((float) 131)
#define MPU6050_GYRO_SENS_500		((float) 65.5)
#define MPU6050_GYRO_SENS_1000		((float) 32.8)

#ifdef ROBOT_N1
#define MPU6050_GYRO_SENS_2000		((float) 16.25)
#else
#define MPU6050_GYRO_SENS_2000		((float) 16.4)
#endif

/* Acce sensitivities in g/s */
#define MPU6050_ACCE_SENS_2			((float) 16384)
#define MPU6050_ACCE_SENS_4			((float) 8192)
#define MPU6050_ACCE_SENS_8			((float) 4096)
#define MPU6050_ACCE_SENS_16		((float) 2048)


//*****************************************************************************
//
// The factors used to convert the acceleration readings from the MPU6050 into
// floating point values in meters per second squared.
//
// Values are obtained by taking the g conversion factors from the data sheet
// and multiplying by 9.81 (1 g = 9.81 m/s^2).
//
//*****************************************************************************
static const float g_fMPU6050AccelFactors[] =
{
    0.00059875,                              // Range = +/- 2 g (16384 lsb/g)
    0.00119751,                              // Range = +/- 4 g (8192 lsb/g)
    0.00239502,                              // Range = +/- 8 g (4096 lsb/g)
    0.00479004                               // Range = +/- 16 g (2048 lsb/g)
};

//*****************************************************************************
//
// Global storage for most recent  Sensor Data and callibration values
//
//*****************************************************************************
float g_pfEulers[3];
float g_pfAccel[3];
volatile float g_pfGyro[3];
float g_pfGyroOffset[3];
float fAngle;

SD_MPU6050_Result SD_MPU6050_Init(I2C_HandleTypeDef* I2Cx, SD_MPU6050* DataStruct, SD_MPU6050_Device DeviceNumber,
		SD_MPU6050_Accelerometer AccelerometerSensitivity, SD_MPU6050_Gyroscope GyroscopeSensitivity) {
	uint8_t WHO_AM_I = (uint8_t) MPU6050_WHO_AM_I;
	uint8_t temp;
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t d[4];

	/* Format I2C address */
	DataStruct->Address = MPU6050_I2C_ADDR | (uint8_t) DeviceNumber;
	uint8_t address = DataStruct->Address;

	/* Check if device is connected */
	if (HAL_I2C_IsDeviceReady(Handle, address, 2, 5) != HAL_OK) {
		return SD_MPU6050_Result_Error;
	}
	/* Check who am I */
	//------------------
	/* Send address */
	if (HAL_I2C_Master_Transmit(Handle, address, &WHO_AM_I, 1, 1000) != HAL_OK) {
		return SD_MPU6050_Result_Error;
	}

	/* Receive multiple byte */
	if (HAL_I2C_Master_Receive(Handle, address, &temp, 1, 1000) != HAL_OK) {
		return SD_MPU6050_Result_Error;
	}

	/* Checking */
	while (temp != MPU6050_I_AM) {
		/* Return error */
		return SD_MPU6050_Result_DeviceInvalid;
	}
	//------------------

	/* Wakeup MPU6050 */
	// Write application specifice sensor configuration such as filter settings
	// and sensor range settings.
	//------------------
	/* Format array to send */
	d[0] = MPU6050_PWR_MGMT_1;
	d[1] = MPU6050_PWR_MGMT_1_CLKSEL_XG;

	// Try to transmit via I2C //
	if (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, (uint8_t *) d, 2, 1000) != HAL_OK) {
		return SD_MPU6050_Result_Error;
	}

	d[0] = MPU6050_O_CONFIG;
	d[1] =	MPU6050_CONFIG_DLPF_CFG_94_98;									// MPU6050_CONFIG_DLPF_CFG_21_20; //MPU6050_CONFIG_DLPF_CFG_44_42; //MPU6050_CONFIG_DLPF_CFG_94_98;
	if (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, (uint8_t *) d, 2, 1000) != HAL_OK) {
		return SD_MPU6050_Result_Error;
	}

	//------------------

	/* Set sample rate to 1kHz */
	SD_MPU6050_SetDataRate(I2Cx, DataStruct, SD_MPU6050_DataRate_8KHz);

	/* Config accelerometer */
	SD_MPU6050_SetAccelerometer(I2Cx, DataStruct, AccelerometerSensitivity);

	/* Config Gyroscope */
	SD_MPU6050_SetGyroscope(I2Cx, DataStruct, GyroscopeSensitivity);

	//
	// Configure the data ready interrupt pin output of the MPU6050.
	d[0] = MPU6050_O_INT_PIN_CFG;
	d[1] = ( MPU6050_INT_PIN_CFG_INT_LEVEL | MPU6050_INT_PIN_CFG_INT_RD_CLEAR | MPU6050_INT_PIN_CFG_LATCH_INT_EN);
	d[2] = MPU6050_INT_ENABLE_DATA_RDY_EN | MPU6050_INT_ENABLE_MOT_EN;
	if (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, (uint8_t *) d, 3, 1000) != HAL_OK) {
		return SD_MPU6050_Result_Error;
	}

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_SetDataRate(I2C_HandleTypeDef* I2Cx, SD_MPU6050* DataStruct, uint8_t rate) {
	uint8_t d[2];
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t address = DataStruct->Address;
	/* Format array to send */
	d[0] = MPU6050_SMPLRT_DIV;
	d[1] = rate;

	/* Set data sample rate */
	while (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, (uint8_t *) d, 2, 1000) != HAL_OK)
		;
	/*{
	 return SD_MPU6050_Result_Error;
	 }*/

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_SetAccelerometer(I2C_HandleTypeDef* I2Cx, SD_MPU6050* DataStruct,
		SD_MPU6050_Accelerometer AccelerometerSensitivity) {
	uint8_t temp;
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t address = DataStruct->Address;
	uint8_t regAdd = (uint8_t) MPU6050_ACCEL_CONFIG;

	/* Config accelerometer */
	while (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, &regAdd, 1, 1000) != HAL_OK)
		;
	/*{
	 return SD_MPU6050_Result_Error;
	 }*/
	if  (HAL_I2C_Master_Receive(Handle, (uint16_t) address, &temp, 1, 1000) != HAL_OK)
		;
	/*{
	 return SD_MPU6050_Result_Error;
	 }*/
	temp = (temp & 0xE7) | (uint8_t) AccelerometerSensitivity << 3;
	if  (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, &temp, 1, 1000) != HAL_OK)
		;
	/*{
	 return SD_MPU6050_Result_Error;
	 }*/

	/* Set sensitivities for multiplying gyro and accelerometer data */
	switch (AccelerometerSensitivity) {
	case SD_MPU6050_Accelerometer_2G:
		DataStruct->Acce_Mult = (float) 1 / MPU6050_ACCE_SENS_2;
		break;
	case SD_MPU6050_Accelerometer_4G:
		DataStruct->Acce_Mult = (float) 1 / MPU6050_ACCE_SENS_4;
		break;
	case SD_MPU6050_Accelerometer_8G:
		DataStruct->Acce_Mult = (float) 1 / MPU6050_ACCE_SENS_8;
		break;
	case SD_MPU6050_Accelerometer_16G:
		DataStruct->Acce_Mult = (float) 1 / MPU6050_ACCE_SENS_16;
		break;
	default:
		break;
	}

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_SetGyroscope(I2C_HandleTypeDef* I2Cx, SD_MPU6050* DataStruct,
		SD_MPU6050_Gyroscope GyroscopeSensitivity) {

	// Config gyroscope //
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t d[2];
	int8_t address = DataStruct->Address;
	d[0] = MPU6050_GYRO_CONFIG;
	d[1] = (uint8_t) GyroscopeSensitivity << 3;
	if   (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, (uint8_t *) d, 2, 1000) != HAL_OK);

	switch (GyroscopeSensitivity) {
	case SD_MPU6050_Gyroscope_250s:
		DataStruct->Gyro_Mult = (float) 1 / MPU6050_GYRO_SENS_250;
		break;
	case SD_MPU6050_Gyroscope_500s:
		DataStruct->Gyro_Mult = (float) 1 / MPU6050_GYRO_SENS_500;
		break;
	case SD_MPU6050_Gyroscope_1000s:
		DataStruct->Gyro_Mult = (float) 1 / MPU6050_GYRO_SENS_1000;
		break;
	case SD_MPU6050_Gyroscope_2000s:
		DataStruct->Gyro_Mult = (float) 1 / MPU6050_GYRO_SENS_2000;
		break;
	default:
		break;
	}
	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_ReadAccelerometer(I2C_HandleTypeDef* I2Cx, SD_MPU6050* DataStruct) {
	uint8_t data[6];
	uint8_t reg = MPU6050_ACCEL_XOUT_H;
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t address = DataStruct->Address;

	/* Read accelerometer data */
	while (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, &reg, 1, 1000) != HAL_OK)
		;

	while (HAL_I2C_Master_Receive(Handle, (uint16_t) address, data, 6, 1000) != HAL_OK)
		;
	 /// Format //
	 DataStruct->Accelerometer_X = (int16_t) (data[0] << 8 | data[1]);
	 DataStruct->Accelerometer_Y = (int16_t) (data[2] << 8 | data[3]);
	 DataStruct->Accelerometer_Z = (int16_t) (data[4] << 8 | data[5]);

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_ReadGyroscope(I2C_HandleTypeDef* I2Cx, SD_MPU6050* DataStruct) {
	uint8_t data[6];
	//uint8_t reg = MPU6050_GYRO_XOUT_H;
	uint8_t reg = MPU6050_GYRO_ZOUT_H;  // <<<<------- Only Z  - 2 bytes
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t address = DataStruct->Address;

	// Read gyroscope data //
	if (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, &reg, 1, 1000) != HAL_OK)
		;

	//HAL_I2C_Master_Receive_IT(&hi2c1, (uint16_t) address, data, 2);
	if (HAL_I2C_Master_Receive(Handle, (uint16_t) address, data, 2, 1000) != HAL_OK)
		;	// <<<<------- Only Z  - 2 bytes
		/*
		 // Format //
		 DataStruct->Gyroscope_X = (int16_t) (data[0] << 8 | data[1]);
		 DataStruct->Gyroscope_Y = (int16_t) (data[2] << 8 | data[3]);
		 DataStruct->Gyroscope_Z = (int16_t) (data[4] << 8 | data[5]);
		 */
	DataStruct->Gyroscope_Z = (int16_t) (data[0] << 8 | data[1]);	// <<<<------- Only Z  - 2 bytes

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_ReadTemperature(I2C_HandleTypeDef* I2Cx, SD_MPU6050* DataStruct) {
	uint8_t data[2];
	int16_t temp;
	uint8_t reg = MPU6050_TEMP_OUT_H;
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t address = DataStruct->Address;

	/* Read temperature */
	while (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, &reg, 1, 1000) != HAL_OK)
		;

	while (HAL_I2C_Master_Receive(Handle, (uint16_t) address, data, 2, 1000) != HAL_OK)
		;

	/* Format temperature */
	temp = (data[0] << 8 | data[1]);
	DataStruct->Temperature = (float) ((int16_t) temp / (float) 340.0 + (float) 36.53);

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_ReadAll(I2C_HandleTypeDef* I2Cx, SD_MPU6050* DataStruct) {
	uint8_t data[14];
	int16_t temp;
	uint8_t reg = MPU6050_ACCEL_XOUT_H;
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t address = DataStruct->Address;

	/* Read full raw data, 14bytes */
	while (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, &reg, 1, 1000) != HAL_OK)
		;

	while (HAL_I2C_Master_Receive(Handle, (uint16_t) address, data, 14, 1000) != HAL_OK)
		;

	/* Format accelerometer data */
	DataStruct->Accelerometer_X = (int16_t) (data[0] << 8 | data[1]);
	DataStruct->Accelerometer_Y = (int16_t) (data[2] << 8 | data[3]);
	DataStruct->Accelerometer_Z = (int16_t) (data[4] << 8 | data[5]);

	/* Format temperature */
	temp = (data[6] << 8 | data[7]);
	DataStruct->Temperature = (float) ((float) ((int16_t) temp) / (float) 340.0 + (float) 36.53);

	/* Format gyroscope data */
	DataStruct->Gyroscope_X = (int16_t) (data[8] << 8 | data[9]);
	DataStruct->Gyroscope_Y = (int16_t) (data[10] << 8 | data[11]);
	DataStruct->Gyroscope_Z = (int16_t) (data[12] << 8 | data[13]);

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

SD_MPU6050_Result SD_MPU6050_EnableInterrupts(I2C_HandleTypeDef* I2Cx, SD_MPU6050* DataStruct) {
	uint8_t temp;
	uint8_t reg[2] = { MPU6050_INT_ENABLE, 0x21 };
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t address = DataStruct->Address;

	/* Enable interrupts for data ready and motion detect */
	while (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, reg, 2, 1000) != HAL_OK)
		;

	uint8_t mpu_reg = MPU6050_INT_PIN_CFG;
	/* Clear IRQ flag on any read operation */
	while (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, &mpu_reg, 1, 1000) != HAL_OK)
		;

	while (HAL_I2C_Master_Receive(Handle, (uint16_t) address, &temp, 14, 1000) != HAL_OK)
		;
	temp |= 0x10;
	reg[0] = MPU6050_INT_PIN_CFG;
	reg[1] = temp;
	while (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, reg, 2, 1000) != HAL_OK)
		;

	/* Return OK */
	return SD_MPU6050_Result_Ok;
}
SD_MPU6050_Result SD_MPU6050_DisableInterrupts(I2C_HandleTypeDef* I2Cx, SD_MPU6050* DataStruct) {
	uint8_t reg[2] = { MPU6050_INT_ENABLE, 0x00 };
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t address = DataStruct->Address;

	/* Disable interrupts */
	while (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, reg, 2, 1000) != HAL_OK)
		;
	/* Return OK */
	return SD_MPU6050_Result_Ok;
}
SD_MPU6050_Result SD_MPU6050_ReadInterrupts(I2C_HandleTypeDef* I2Cx, SD_MPU6050* DataStruct,
		SD_MPU6050_Interrupt* InterruptsStruct) {
	uint8_t read;

	/* Reset structure */
	InterruptsStruct->Status = 0;
	uint8_t reg = MPU6050_INT_STATUS;
	I2C_HandleTypeDef* Handle = I2Cx;
	uint8_t address = DataStruct->Address;

	while (HAL_I2C_Master_Transmit(Handle, (uint16_t) address, &reg, 1, 1000) != HAL_OK)
		;

	while (HAL_I2C_Master_Receive(Handle, (uint16_t) address, &read, 14, 1000) != HAL_OK)
		;

	/* Fill value */
	InterruptsStruct->Status = read;
	/* Return OK */
	return SD_MPU6050_Result_Ok;
}

//*****************************************************************************
//
//! Gets the gyroscope data from the most recent data read.
//!
//! \param psInst is a pointer to the MPU6050 instance data.
//! \param pfGyroX is a pointer to the value into which the X-axis
//! gyroscope data is stored.
//! \param pfGyroY is a pointer to the value into which the Y-axis
//! gyroscope data is stored.
//! \param pfGyroZ is a pointer to the value into which the Z-axis
//! gyroscope data is stored.
//!
//! This function returns the gyroscope data from the most recent data read,
//! converted into radians per second.  If any of the output data pointers are
//! \b NULL, the corresponding data is not provided.
//!
//! \return None.
//
//*****************************************************************************
void MPU6050DataGyroGetFloat(SD_MPU6050 *psInst, float *pfGyroX, float *pfGyroY, float *pfGyroZ) {
	float fFactor;
	//mpu1.Gyroscope_X
	//
	// Get the conversion factor for the current data format.
	//
	fFactor = psInst->Gyro_Mult;

	//
	// Convert the gyroscope values into grad/sec
	//
	if (pfGyroX) {
		*pfGyroX = ((float) (int16_t) psInst->Gyroscope_X * fFactor);
	}
	if (pfGyroY) {
		*pfGyroY = ((float) (int16_t) psInst->Gyroscope_Y * fFactor);
	}
	if (pfGyroZ) {
		*pfGyroZ = ((float) (int16_t) psInst->Gyroscope_Z * fFactor);
	}
}


//*****************************************************************************
//
//! Gets the accelerometer data from the most recent data read.
//!
//! \param psInst is a pointer to the MPU6050 instance data.
//! \param pfAccelX is a pointer to the value into which the X-axis
//! accelerometer data is stored.
//! \param pfAccelY is a pointer to the value into which the Y-axis
//! accelerometer data is stored.
//! \param pfAccelZ is a pointer to the value into which the Z-axis
//! accelerometer data is stored.
//!
//! This function returns the accelerometer data from the most recent data
//! read, converted into meters per second squared (m/s^2).  If any of the
//! output data pointers are \b NULL, the corresponding data is not provided.
//!
//! \return None.
//
//*****************************************************************************
void
MPU6050DataAccelGetFloat(SD_MPU6050 *psInst, float *pfAccelX, float *pfAccelY,
                         float *pfAccelZ)
{
    float fFactor;

    //
    // Get the acceleration conversion factor for the current data format.
    //
    fFactor = g_fMPU6050AccelFactors[SD_MPU6050_Accelerometer_2G];

    //
    // Convert the Accelerometer values into floating-point gravity values.
    //
    if(pfAccelX)
    {
        *pfAccelX = ((float)(int16_t)psInst->Accelerometer_X * fFactor);
    }
    if(pfAccelY)
    {
        *pfAccelY = ((float)(int16_t)psInst->Accelerometer_Y  * fFactor);
    }
    if(pfAccelZ)
    {
        *pfAccelZ = ((float)(int16_t)psInst->Accelerometer_Z  * fFactor);
    }

}
