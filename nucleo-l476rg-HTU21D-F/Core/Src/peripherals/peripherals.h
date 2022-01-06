/*
 * Peripherals.h
 *
 *  Created on: Dec 9, 2021
 *      Author: Rafeeq Muhammad
 */

//Includes
#include "main.h"

//Definitions and variables
extern UART_HandleTypeDef huart2;
extern I2C_HandleTypeDef hi2c1;

//Prototypes
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void MX_I2C1_Init(void);

