/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <stdio.h>

#include "common.h"
#include "../Libraries/motors.h"
#include "../Libraries/lcd_SPI_drv.h"
#include "../Libraries/ir_module.h"

#include "../Libraries/Glcd.h"
#include "../Fonts/fonts.h"
#include "../Fonts/splash_screen.h"

//-------------
#include "../Libraries/KBD_scan.h"
#include "../Menu/menu.h"
#include "../Language/menu_items.h"
#include "../Menu/dev_setup.h"

#include "../MPU6050/sd_hal_mpu6050.h"
#include "../Libraries/Encoders.h"

#include "../Maze/maze.h"
#include "../DogModeTEST/dog_mode.h"
#include "../WallFollowing/wall_following.h"


/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef enum {
	POWERON = 0, STARTED, STOPPED_SAFE, STOPPED, PROGRAMMING, SETTINGS
} E_ROBOT_STATE;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;
DMA_HandleTypeDef hdma_adc1;
DMA_HandleTypeDef hdma_adc2;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim6;
TIM_HandleTypeDef htim15;
TIM_HandleTypeDef htim16;

/* USER CODE BEGIN PV */

//------------------ Global variables -----------------
volatile uint32_t gulTick;
volatile uint32_t gulTickCountDown;
volatile uint32_t gulTimeTick;
volatile uint32_t gulSysTimer;

// ---------- Keyboard ----------
volatile uint16_t gucKeyboardTimer;
volatile uint8_t gucRepeatOff = 0;
volatile uint8_t gucKeyboardKey = 0xFF;
volatile uint16_t gusKeyRepeatTimer;
volatile uint8_t gucKeyHoldTimer;
uint8_t last_key;

volatile uint16_t sensor_values[SENSORS_NR];
volatile uint16_t sensor_calmin[SENSORS_NR];

unsigned int frontL_distance;
unsigned int frontR_distance;
unsigned int left_distance;
unsigned int right_distance;
unsigned int left_side;
 unsigned int right_side;
int16_t right_side_distance = 0;

// dog
extern int8_t dogTurnFlag;

//------------------  PID -----------------------
// Linear error - distance from Walls
signed int dt = 0;
signed int everyThird = 2;
signed int everySecond = 1;
signed int errorX;
signed int diff;
signed int errors_sumX;
signed int lastErrorX;
signed int proportionalX = 0;
signed long integralX = 0;
signed long derivateX = 0;
signed int control_valueX;
signed int speedX;

//  Rotational error - angular error from Gyro  - W = omega
signed int errorW;
signed int errors_sumW;
signed int lastErrorW;
signed int lastErrorW1;
signed int lastErrorW2;
signed int proportionalW = 0;
signed long integralW = 0;
signed long derivateW = 0;
signed int control_valueW;
signed int speedW;

//------------
signed int left_pwm;
signed int right_pwm;
float speed_ramp;
float current_speed;
signed int speed_setpoint;
//signed int distance;
signed int angle;

//----------- Maze ----------
int maze_rule;
int center_pos_value; //  (�������� �� ��������� /2 - 2��) * 1.414 ( ����� ��. �� 2)
int front_min; //  (MAZE_WIDTH  - 8) 			// �������� �� ��������� - �������� �� ������  - 2 cm  �� �� �� ������ ������� ��� ������
int front_way;				// 	(MAZE_WIDTH + MAZE_WIDTH/2 + SENSORS_GAP)
int goto_center_steps;// 	(MAZE_WIDTH + ROBOT_LENGTH - 2)		//  (10) - �������� �� ������ 10�� /2 * 2 (������)
int goto_wall;  //
int wall;
int sensor_error;
int front_turn_min;
///

int last_state;
int direction;
int last_wall_status;
E_ROBOT_STATE state;
E_3DMAZE_STATE maze_state;
//E_DOG_MODE_STATE dogState;
int flagGOTO_END = 1;


int timeout;
unsigned char move_result;

signed int endMapFlag = 0;
//--------------------
signed int lAngle;
signed int turn;

signed int last_turn;
signed int curve = 0;
int last_curve = 0;

signed int currentMapIndex = 0;


//------------
SD_MPU6050 mpu1;
volatile uint8_t mpu_irq = 0;
uint32_t gulDeltaT;
SD_MPU6050_Result result;
char mpu_ok[] = "MPU WORK FINE";
char mpu_not[] = "MPU NOT WORKING";

//-------------
volatile uint8_t adc1_complete = 0;
volatile uint8_t adc2_complete = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ADC1_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM6_Init(void);
static void MX_TIM15_Init(void);
static void MX_TIM16_Init(void);
static void MX_TIM1_Init(void);
static void MX_DMA_Init(void);
static void MX_ADC2_Init(void);
/* USER CODE BEGIN PFP */
void read_sensors(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//===================== Debug print =====================
int32_t i32IPart;
int32_t i32FPart;

void float_format(float inpData) {
	// Conver float value to a integer truncating the decimal part.
	//
	i32IPart = (int32_t) inpData;

	//
	// Multiply by 1000 to preserve first three decimal values.
	// Truncates at the 3rd decimal place.
	//
	i32FPart = (int32_t) (inpData * 1000.0f);

	//
	// Subtract off the integer part from this newly formed decimal
	// part.
	//
	i32FPart = i32FPart - (i32IPart * 1000);

	//
	// make the decimal part a positive number for display.
	//
	if (i32FPart < 0) {
		i32FPart *= -1;
	}
}

//------------------------ MPU6050 ORQ and IR pin interrupts -----------
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	//------------- IR remote ----------
	if (GPIO_Pin == GPIO_PIN_11) {
		if (pinIRQenable) {
			IRpinIntHandler();
		}
	}

	//------------ Encodes ------------
	if (GPIO_Pin == GPIO_PIN_6)
		enc_leftCounter++;
	if (GPIO_Pin == GPIO_PIN_7)
		enc_rightCounter++;

	//-------------  Gyroscope ----------
	if (GPIO_Pin == GPIO_PIN_15) {
		mpu_irq = 1;
	}
}

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
  int main(void) {
	/* USER CODE BEGIN 1 */
	uint32_t tmp_long;
	uint8_t tmp_char;
	uint8_t motors_status = 0;
	char sign;
	char tmp_str[64];
	int turbo_speed;

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */
	HAL_SYSTICK_Config(SystemCoreClock / 10000);

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_SPI1_Init();
	MX_TIM2_Init();
	MX_TIM6_Init();
	MX_TIM15_Init();
	MX_TIM16_Init();
	MX_TIM1_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_ADC2_Init();
	/* USER CODE BEGIN 2 */

	//eepr_buffRead( (uint8_t*)tmp_str, 10, 16);
	//--------------------------- aDC -----------------------
	if (HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED) != HAL_OK) {
		// Calibration Error //
		Error_Handler();
	}

	if (HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED) != HAL_OK) {
		// Calibration Error //
		Error_Handler();
	}

	//HAL_ADC_Start(&hadc1);

	//----------------------------------------
	TurboStop();
	turbo_speed = 0;

	//----------------------------------------
	MotorsStop();
	MotorLeftSpeed(0);
	MotorRightSpeed(0);

	//---------------------------------------
	EncodersInit();

	//---------------------------  Restore saved state --------------------
	if (dev_settings_chk()) {
		dev_factory_settings();
	}

	if (maze_settings_chk()) {
		dev_factory_settings();
	}

	//--------- IR remote init ----------
#ifndef START_MODULE
	ir_init();
#else
	ROM_GPIOPinTypeGPIOInput(IR_PORT, IR_PIN);		// IR_PIN regular digital input
	ROM_GPIOPadConfigSet( IR_PORT, IR_PIN, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPU);
#endif

	//------------  Encoders Init ----------------
	//EncodersInit();

	//-------------- LCD -------------
	LcdInit();
	LcdContrast(0xFF);

	LcdErase();
	LcdBitmap(0, 0, (IMG_DEF*) &splash_screen);
	LcdUpdate();
	speedW = 0;
	HAL_Delay(10000);
	tmp_long = 0;

	//---------- MPU6050 init -----------
	result = SD_MPU6050_Init(&hi2c1, &mpu1, SD_MPU6050_Device_0,
			SD_MPU6050_Accelerometer_2G, SD_MPU6050_Gyroscope_2000s);
	if (result == SD_MPU6050_Result_Ok) {
		LcdTextXY(0, 0, BIG_FNT, CENTERED, mpu_ok);
	} else {
		LcdTextXY(0, 0, BIG_FNT, CENTERED, mpu_not);
	}
	LcdUpdate();

	HAL_Delay(10000);
	for (tmp_char = 0; tmp_char < 200; tmp_char++) {
		SD_MPU6050_ReadGyroscope(&hi2c1, &mpu1);	// dummy read
		HAL_Delay(10);
	}
	fAngle = 0;

	// ---------- MPU6050 callibrate --------
	// ���������� ���� �� ������, �� �� ��� ���� ����� �� ���� �� ����������.
	g_pfGyroOffset[2] = dev_settings.gyro_offset;

	// ���������� �� ������� ������ = �������� �� ���������/2 -Sensors Gap - �������� �� ����������, ����� ������ �� �������.
	// ���� ��������� ����� �������� ����� - ������ ������ �� �� �� ������� �� ���������, � ���� � �� SENSOR_GAP �� �������
	//front_min = maze_settings.maze_width * 5 - SENSORS_GAP + dev_settings.ramp / STEPK + (dev_settings.fast_speed - 120) * 0.6; //(dev_settings.fast_speed - 100) * 0.6
	front_min = maze_settings.maze_width * 5 - SENSORS_GAP
			+ dev_settings.ramp / STEPK + (dev_settings.fast_speed * 0.35); // 25

	// �� �� ����� ������� � ���� ���� ����� �� � �� �������, ���� ������ �� ������� � SENSOR_GAP - (ROBOT_WIDTH/2) , ������� �� ��������� � �� �����
	//         SENSOR_GAP - (ROBOT_WIDTH/2) = 65 - 50 = 15
	//front_turn_min = maze_settings.maze_width * 5 + dev_settings.ramp / STEPK + (dev_settings.fast_speed * 0.3); // - 15
	front_turn_min = maze_settings.maze_width * 10
			+ (dev_settings.fast_speed * 0.7);

	goto_center_steps = ((maze_settings.maze_width * 5) + SENSORS_GAP
			- ROBOT_WIDTH / 2) * STEPK - dev_settings.fast_speed * 0.25; //SENSORS_GAP
	goto_wall = (maze_settings.maze_width * 5 - (SENSORS_GAP + ROBOT_WIDTH / 2))
			* STEPK - dev_settings.fast_speed * 0.25; //+ (40 * STEPK);

	//---------------------------------------------------------------------------------------------
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	HAL_TIM_Base_Start(&htim6);

	tmp_char = 0;
	//distance = 0;
	fAngle = 0;
	speedX = 0;
	speedW = 0;
	turn = 0;
	state = POWERON;

	//------------ For standart Phillips IR control ----------
#ifdef IR_RC5_PROTOCOL_STD
	//status.dohio = 8;				// bnt 9 = start   8 = stop
#endif

#ifdef IR_NEC_PROTOCOL
	//status.dohio = 0x46;			// CH+ = Start    CH = Stop
#endif

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		//--------------------------------------------------------------------------------
		//---------------------------------- Measurements --------------------------------
		//--------------------------------------------------------------------------------
		gulSysTimer = 10;			// time slot for all measurements

		//----------- Distance ---------------
		read_sensors();

		//------------ Giroscope -------------
		frontL_distance = dev_settings.frontL_callibration / sensor_values[4];
		frontR_distance = dev_settings.frontR_callibration / sensor_values[1];

		left_distance = dev_settings.sideL_callibration / sensor_values[3];
		right_distance = dev_settings.sideR_callibration / sensor_values[2];

		left_side = dev_settings.left_callibration / sensor_values[5];
		right_side = dev_settings.right_callibration / sensor_values[0];

		right_side_distance = right_side;

		while (gulSysTimer) {	// wait always time base - 10 = 1ms;
			if ((mpu_irq) && (gulSysTimer > 1)) {
				mpu_irq = 0;
				//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET);
				SD_MPU6050_ReadGyroscope(&hi2c1, &mpu1);
				//SD_MPU6050_ReadAccelerometer(&hi2c1, &mpu1);

				// Convert the gyroscope values into grad/sec
				g_pfGyro[2] = -(float) mpu1.Gyroscope_Z * mpu1.Gyro_Mult;// Get float
				gulDeltaT = gulTimeTick;
				gulTimeTick = 0;
				fAngle += (g_pfGyro[2] + g_pfGyroOffset[2]) * 0.0001
						* gulDeltaT;
				float_format(fAngle);
				lAngle = i32IPart;
				//HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET);
			}
		}

		//-------------------------------------------------------------------------------
#ifdef MAZE_ON
		//------------------------ Angular PID ----------------------
		errorW = lAngle - turn;
		everySecond++;
		everyThird++;
		if(everyThird % 3 == 0)
		{
			dt =  (errorW - lastErrorW);
			lastErrorW = errorW;

		}
		if(everySecond % 2 == 0)
		{
			errors_sumW += errorW;
			integralW = errors_sumW * dev_settings.wki;
			integralW = integralW / 1000;
		}

		if (errors_sumW > 1000)
			errors_sumW = 1000;
		if (errors_sumW < -1000)
			errors_sumW = -1000;

		control_valueW = dev_settings.wkp * errorW
				+ dev_settings.wkd * dt +  integralW;
		if (control_valueW > (signed int) dev_settings.turn_clamp)
			control_valueW = dev_settings.turn_clamp;
		if (control_valueW < (signed int) (-dev_settings.turn_clamp))
			control_valueW = (signed int) (-dev_settings.turn_clamp);
#endif

		//---------------------- Angular PID(end) -------------------------

		errorW = lAngle - turn;
		everySecond++;
		everyThird++;
		if(everyThird % 3 == 0)
		{
			dt =  (errorW - lastErrorW);
			lastErrorW = errorW;

		}
		if(everySecond % 2 == 0)
		{
			errors_sumW += errorW;
			integralW = errors_sumW * dev_settings.wki;
			integralW = integralW / 1000;
		}

		if (errors_sumW > 1000)
			errors_sumW = 1000;
		if (errors_sumW < -1000)
			errors_sumW = -1000;

		control_valueW = dev_settings.wkp * errorW
				+ dev_settings.wkd * dt +  integralW;


		if(dogTurnFlag)
		{
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, SET);
			HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET);

			//------------------------ Angular PID Dog mode ----------------------
			errorW = lAngle - turn;
			everySecond++;
			everyThird++;
			if(everyThird % 3 == 0)
			{
				dt =  (errorW - lastErrorW);
				lastErrorW = errorW;

			}
			if(everySecond % 2 == 0)
			{
				errors_sumW += errorW;
				integralW = errors_sumW * dev_settings.wki;
				integralW = integralW / 1000;
			}

			if (errors_sumW > 1000)
				errors_sumW = 1000;
			if (errors_sumW < -1000)
				errors_sumW = -1000;

			control_valueW = dev_settings.wkp * errorW
					+ dev_settings.wkd * dt +  integralW;
			if (control_valueW > (signed int) dev_settings.turn_clamp)
				control_valueW = dev_settings.turn_clamp;
			if (control_valueW < (signed int) (-dev_settings.turn_clamp))
				control_valueW = (signed int) (-dev_settings.turn_clamp);

		}

		//-----------------------------------------------------------

		switch (state) {

		case POWERON:

			//--------- Display  every 100ms measurements --------
			if (++tmp_char >= 100) {
				tmp_char = 0;

				sprintf(tmp_str, " %4d  %4d", frontL_distance, frontR_distance);
				LcdTextXY(0, 0, BIG_FNT, NORM_LINE, tmp_str);

				sprintf(tmp_str, " %4d  %4d", left_distance, right_distance);
				LcdTextXY(0, 2, BIG_FNT, NORM_LINE, tmp_str);

				sprintf(tmp_str, " %4d  %4d", left_side, right_side);
				LcdTextXY(0, 4, BIG_FNT, NORM_LINE, tmp_str);

			    sprintf(tmp_str, " %3ld   %3ld", enc_leftCounter, enc_rightCounter);
				LcdTextXY(0, 6, BIG_FNT, NORM_LINE, tmp_str);


				float_format(fAngle);
				if (fAngle < 0)
					sign = '-';
				else
					sign = ' ';
//				sprintf(tmp_str, "Angle %c%d.%03ld", sign, abs(i32IPart),
//						i32FPart);
//				LcdTextXY(0, 6, BIG_FNT, NORM_LINE, tmp_str);

				LcdUpdate();
			}

			//- ������ ���� ����� ���� �� �� ������ ��������� �� ��������
			// ��� �� ���� ���� � ����� �� �����.
			/*
			if (frontL_distance < 70) {
				maze_rule = LEFT_WALL;
				//status.rule = maze_rule;
				HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, SET);
				HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, RESET);
			}

			if (frontR_distance < 70) {
				maze_rule = RIGHT_WALL;
				//status.rule = maze_rule;
				HAL_GPIO_WritePin(LED1_GPIO_Port, LED1_Pin, SET);
				HAL_GPIO_WritePin(LED1_GPIO_Port, LED2_Pin, RESET);
			}
			*/

			//--------------- IR command check ---------------
			if (ir_data_ready) {
				ir_data_ready = 0;

				if (ir_data.command == KEY_NEXT) {
					MotorsStart();
					motors_status = 1;
					speedX = dev_settings.starting_speed;
					LcdErase();
					LcdTextXY(0, 2, BIG_FNT, CENTERED, "STARTED");
					LcdUpdate();
					state = STARTED;
					maze_state = FORWARD;
				//	distance = 600 * STEPK;
					enc_leftCounter = 0;
					enc_rightCounter = 0;
				}

			}

			if (gucKeyboardKey == KEY_TL) {
				state = SETTINGS;
				MENU_init(rootMenu);
				break;
			}

			break;
		case STARTED:
			endMapFlag = 0;
#ifdef MAZE_ON
			maze();
#endif

#ifdef DOG_MODE_ON
			DogModeFun();

#endif

#ifdef WALL_FOLLOWING_ON
		WallFollowing();

#endif
			if(endMapFlag)
			{
				MotorsStop();
				motors_status = 1;
				LcdErase();
				LcdTextXY(0, 2, BIG_FNT, CENTERED, "STOPPED");
				LcdUpdate();
				state = POWERON;
			}
			//--------------- IR command check ---------------
			if (ir_data_ready) {
				ir_data_ready = 0;

				if (ir_data.command == KEY_CH) {
					MotorsStop();
					motors_status = 1;
					LcdErase();
					LcdTextXY(0, 2, BIG_FNT, CENTERED, "STOPPED");
					LcdUpdate();
					state = POWERON;
				}

			}
			break;

		case SETTINGS:
			MotorLeftSpeed(0);
			MotorRightSpeed(0);

			//---------------- Keyboard key Pressed -----------------
			if (gucKeyboardKey != 0xFF) {
				if (last_key != gucKeyboardKey) {
					gucKeyHoldTimer = 3;
					if (gucKeyboardKey != 0xFF) {
						gucKeyboardTimer = 10; // 100 ms time inteval before next key reading
						MENU_handler(gucKeyboardKey); // Parameter only gucKeyboardKey, but in the menu function must be checked all events
					}
				}
				//last_key = gucKeyboardKey;
			}
			last_key = gucKeyboardKey;

			if (gucRepeatOff) {
				MENU_handler(0xFE);
				gucRepeatOff = 0;
			}

			break;

		default:
			state = POWERON;
			break;

			/*
			 //---------------- Keyboard key Pressed -----------------
			 if (gucKeyboardKey != 0xFF) {
			 if (last_key != gucKeyboardKey) {
			 gucKeyHoldTimer = 3;
			 if (gucKeyboardKey != 0xFF) {
			 gucKeyboardTimer = 10; // 100 ms time inteval before next key reading
			 MENU_handler(gucKeyboardKey); // Parameter only gucKeyboardKey, but in the menu function must be checked all events
			 }
			 }
			 last_key = gucKeyboardKey;
			 } else {
			 last_key = 0xFF;
			 }

			 if (gucRepeatOff) {
			 MENU_handler(0xFE);
			 gucRepeatOff = 0;
			 }
			 */

			/* USER CODE END WHILE */

			/* USER CODE BEGIN 3 */
		}
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1)
			!= HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 1;
	RCC_OscInitStruct.PLL.PLLN = 10;
	RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
	RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1 | RCC_PERIPHCLK_ADC;
	PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {

	/* USER CODE BEGIN ADC1_Init 0 */

	/* USER CODE END ADC1_Init 0 */

	ADC_MultiModeTypeDef multimode = { 0 };
	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC1_Init 1 */

	/* USER CODE END ADC1_Init 1 */
	/** Common config
	 */
	hadc1.Instance = ADC1;
	hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc1.Init.Resolution = ADC_RESOLUTION_10B;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc1.Init.LowPowerAutoWait = DISABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	hadc1.Init.NbrOfConversion = 3;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.NbrOfDiscConversion = 1;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc1.Init.DMAContinuousRequests = ENABLE;
	hadc1.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	hadc1.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc1) != HAL_OK) {
		Error_Handler();
	}
	/** Configure the ADC multi-mode
	 */
	multimode.Mode = ADC_MODE_INDEPENDENT;
	if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK) {
		Error_Handler();
	}
	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_4;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_5;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	sConfig.SamplingTime = ADC_SAMPLETIME_6CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC1_Init 2 */

	/* USER CODE END ADC1_Init 2 */

}

/**
 * @brief ADC2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC2_Init(void) {

	/* USER CODE BEGIN ADC2_Init 0 */

	/* USER CODE END ADC2_Init 0 */

	ADC_ChannelConfTypeDef sConfig = { 0 };

	/* USER CODE BEGIN ADC2_Init 1 */

	/* USER CODE END ADC2_Init 1 */
	/** Common config
	 */
	hadc2.Instance = ADC2;
	hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
	hadc2.Init.Resolution = ADC_RESOLUTION_10B;
	hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc2.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
	hadc2.Init.LowPowerAutoWait = DISABLE;
	hadc2.Init.ContinuousConvMode = DISABLE;
	hadc2.Init.NbrOfConversion = 3;
	hadc2.Init.DiscontinuousConvMode = DISABLE;
	hadc2.Init.NbrOfDiscConversion = 1;
	hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
	hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
	hadc2.Init.DMAContinuousRequests = ENABLE;
	hadc2.Init.Overrun = ADC_OVR_DATA_OVERWRITTEN;
	hadc2.Init.OversamplingMode = DISABLE;
	if (HAL_ADC_Init(&hadc2) != HAL_OK) {
		Error_Handler();
	}
	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_1;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_2;
	sConfig.Rank = ADC_REGULAR_RANK_2;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/** Configure Regular Channel
	 */
	sConfig.Channel = ADC_CHANNEL_3;
	sConfig.Rank = ADC_REGULAR_RANK_3;
	if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN ADC2_Init 2 */

	/* USER CODE END ADC2_Init 2 */

}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.Timing = 0x00702991;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	/** Configure Analogue filter
	 */
	if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE)
			!= HAL_OK) {
		Error_Handler();
	}
	/** Configure Digital filter
	 */
	if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief SPI1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_SPI1_Init(void) {

	/* USER CODE BEGIN SPI1_Init 0 */

	/* USER CODE END SPI1_Init 0 */

	/* USER CODE BEGIN SPI1_Init 1 */

	/* USER CODE END SPI1_Init 1 */
	/* SPI1 parameter configuration*/
	hspi1.Instance = SPI1;
	hspi1.Init.Mode = SPI_MODE_MASTER;
	hspi1.Init.Direction = SPI_DIRECTION_2LINES;
	hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
	hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
	hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
	hspi1.Init.NSS = SPI_NSS_SOFT;
	hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_64;
	hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
	hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
	hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	hspi1.Init.CRCPolynomial = 7;
	hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
	hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
	if (HAL_SPI_Init(&hspi1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN SPI1_Init 2 */

	/* USER CODE END SPI1_Init 2 */

}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void) {

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };
	TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = { 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 3;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 256;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterOutputTrigger2 = TIM_TRGO2_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 128;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
	sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
	if (HAL_TIM_PWM_ConfigChannel(&htim1, &sConfigOC, TIM_CHANNEL_1)
			!= HAL_OK) {
		Error_Handler();
	}
	sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
	sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
	sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
	sBreakDeadTimeConfig.DeadTime = 0;
	sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
	sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
	sBreakDeadTimeConfig.BreakFilter = 0;
	sBreakDeadTimeConfig.Break2State = TIM_BREAK2_DISABLE;
	sBreakDeadTimeConfig.Break2Polarity = TIM_BREAK2POLARITY_HIGH;
	sBreakDeadTimeConfig.Break2Filter = 0;
	sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
	if (HAL_TIMEx_ConfigBreakDeadTime(&htim1, &sBreakDeadTimeConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */
	HAL_TIM_MspPostInit(&htim1);

}

/**
 * @brief TIM2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM2_Init(void) {

	/* USER CODE BEGIN TIM2_Init 0 */

	/* USER CODE END TIM2_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };

	/* USER CODE BEGIN TIM2_Init 1 */

	/* USER CODE END TIM2_Init 1 */
	htim2.Instance = TIM2;
	htim2.Init.Prescaler = 3;
	htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim2.Init.Period = 256;
	htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_PWM_Init(&htim2) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 128;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.Pulse = 64;
	if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM2_Init 2 */

	/* USER CODE END TIM2_Init 2 */
	HAL_TIM_MspPostInit(&htim2);

}

/**
 * @brief TIM6 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM6_Init(void) {

	/* USER CODE BEGIN TIM6_Init 0 */

	/* USER CODE END TIM6_Init 0 */

	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM6_Init 1 */

	/* USER CODE END TIM6_Init 1 */
	htim6.Instance = TIM6;
	htim6.Init.Prescaler = 79;
	htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim6.Init.Period = 10000;
	htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
	if (HAL_TIM_Base_Init(&htim6) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM6_Init 2 */

	/* USER CODE END TIM6_Init 2 */

}

/**
 * @brief TIM15 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM15_Init(void) {

	/* USER CODE BEGIN TIM15_Init 0 */

	/* USER CODE END TIM15_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM15_Init 1 */

	/* USER CODE END TIM15_Init 1 */
	htim15.Instance = TIM15;
	htim15.Init.Prescaler = 79;
	htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim15.Init.Period = 10;
	htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim15.Init.RepetitionCounter = 0;
	htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim15) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
	if (HAL_TIM_ConfigClockSource(&htim15, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_OnePulse_Init(&htim15, TIM_OPMODE_SINGLE) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim15, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM15_Init 2 */

	/* USER CODE END TIM15_Init 2 */

}

/**
 * @brief TIM16 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM16_Init(void) {

	/* USER CODE BEGIN TIM16_Init 0 */

	/* USER CODE END TIM16_Init 0 */

	/* USER CODE BEGIN TIM16_Init 1 */

	/* USER CODE END TIM16_Init 1 */
	htim16.Instance = TIM16;
	htim16.Init.Prescaler = 127;
	htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim16.Init.Period = 100;
	htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim16.Init.RepetitionCounter = 0;
	htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim16) != HAL_OK) {
		Error_Handler();
	}
	if (HAL_TIM_OnePulse_Init(&htim16, TIM_OPMODE_SINGLE) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM16_Init 2 */

	/* USER CODE END TIM16_Init 2 */

}

/**
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Channel1_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
	/* DMA1_Channel2_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Channel2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel2_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, PING1_Pin | PING2_Pin | LCD_A0_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4 | LCD_CS_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, LED1_Pin | LED2_Pin | DIR1_Pin | DIR2_Pin,
			GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : PC13 PC14 PC15 PC6
	 PC7 PC8 PC9 PC10
	 PC11 PC12 */
	GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15 | GPIO_PIN_6
			| GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10 | GPIO_PIN_11
			| GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : PH0 PH1 PH3 */
	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

	/*Configure GPIO pins : PING1_Pin PING2_Pin LCD_A0_Pin */
	GPIO_InitStruct.Pin = PING1_Pin | PING2_Pin | LCD_A0_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : PC4 LCD_CS_Pin */
	GPIO_InitStruct.Pin = GPIO_PIN_4 | LCD_CS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

	/*Configure GPIO pins : BUTT1_Pin BUTT2_Pin BUTT3_Pin BUTT4_Pin */
	GPIO_InitStruct.Pin = BUTT1_Pin | BUTT2_Pin | BUTT3_Pin | BUTT4_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : PB11 PB14 PB8 PB9 */
	GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_14 | GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pins : LED1_Pin LED2_Pin DIR1_Pin DIR2_Pin */
	GPIO_InitStruct.Pin = LED1_Pin | LED2_Pin | DIR1_Pin | DIR2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : GYRO_Pin */
	GPIO_InitStruct.Pin = GYRO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GYRO_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : IR_Pin */
	GPIO_InitStruct.Pin = IR_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(IR_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pin : PA12 */
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pin : BEEP_Pin */
	GPIO_InitStruct.Pin = BEEP_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(BEEP_GPIO_Port, &GPIO_InitStruct);

	/*Configure GPIO pins : ENC1_Pin PB7 */
	GPIO_InitStruct.Pin = ENC1_Pin | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */

//---------------------
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle) {
	/* Report to main program that ADC sequencer has reached its end */
	if (AdcHandle == &hadc1) {
		adc1_complete = 1;
	}

	if (AdcHandle == &hadc2)
		adc2_complete = 1;

	//HAL_ADC_Stop_DMA(&hadc1);
}

//-------------------------------------------------------------------------------
//-------------------------------------------------------------------------------
//=============================== Read sensors  and scale =======================
const uint32_t sensors[6] = { ADC_CHANNEL_1, ADC_CHANNEL_2, ADC_CHANNEL_3,
ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_6 };
const uint32_t rank[6] = { ADC_REGULAR_RANK_1, ADC_REGULAR_RANK_2,
ADC_REGULAR_RANK_3, ADC_REGULAR_RANK_4, ADC_REGULAR_RANK_5,
ADC_REGULAR_RANK_6 };

void read_sensors(void) {
	unsigned char sens;

	adc1_complete = 0;
	adc2_complete = 0;
	HAL_ADC_Start_DMA(&hadc2, (uint32_t*) &sensor_calmin[0], 3);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &sensor_calmin[3], 3);

	while (!adc2_complete)
		;
	while (!adc1_complete)
		;

	//-------------- Read  Left sensors ------------
	HAL_GPIO_WritePin(PING1_GPIO_Port, PING1_Pin, SET);  //LEDS OFF
	HAL_GPIO_WritePin(PING2_GPIO_Port, PING2_Pin, RESET);  //LEDS On
	delay(3);

	HAL_ADC_Start_DMA(&hadc2, (uint32_t*) &sensor_values[0], 3);
	while (!adc2_complete)
		;
	adc2_complete = 0;
	//HAL_ADC_Stop_DMA(&hadc2);

	//-------------- Read  Right sensors ------------
	HAL_GPIO_WritePin(PING1_GPIO_Port, PING1_Pin, RESET);  //LEDS On
	HAL_GPIO_WritePin(PING2_GPIO_Port, PING2_Pin, SET);  //LEDS OFF
	delay(3);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) &sensor_values[3], 3);
	while (!adc1_complete)
		;
	adc1_complete = 0;
	HAL_GPIO_WritePin(PING1_GPIO_Port, PING1_Pin, RESET);  //LEDS OFF
	HAL_GPIO_WritePin(PING2_GPIO_Port, PING2_Pin, RESET);  //LEDS OFF

	//-------------------------------------------------
	for (sens = 0; sens < 6; sens++) {
		if (sensor_values[sens] > sensor_calmin[sens])
			sensor_values[sens] -= sensor_calmin[sens];
		else
			sensor_values[sens] = 1;
	}

}

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	 tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
