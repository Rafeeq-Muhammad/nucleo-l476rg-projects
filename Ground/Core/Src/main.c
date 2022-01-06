/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/*
 * --//PROJECT NAME\\--
Ground Node.

 * --//PROJECT DESCRIPTION\\--
@brief
 This project will send translated commands to the remote node. And receive results from the remote node.

* --//PROJECT STATUS\\--
Complete.
*
* --//TASKS AT HAND\\--
Nothing.
*
* --//NOTES\\--
* Ground code status update: complete.

Commands entered in the CLI are parsed and translated.
Incorrect commands are notified to the user.
Received data from remote is transmitted to putty.

The commands are as follows:
1. led on
2. led off
3. transmit [message]
4. read temp
5. add [int] [int]
6. subtract [int] [int]
7. multiply [int] [int]
8. divide [int] [int]
9. exponent [int] [int]
10. factorial [int]

The commands are translated into the following api, stored in an uint8 array, and sent to the receiver.
Int's must be single digit.
1. 00
2. 01
3. 1[message]
4. 21
5. 3[int][int]
6. 4[int][int]
7. 5[int][int]
8. 6[int][int]
9. 7[int][int]
10. 8[int]
*
*
*/

/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include "LoRa.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
LoRa myLoRa;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char safe_buff[30] = {0}; //Global variable that holds the completed command that was typed into the PuTTy terminal.
uint8_t rx_buff[100] = {0}; //Global variable that holds the command as it's being entered.
uint8_t rx_char[1] = {0}; //Global variable. Passed in the calling of the hal-uart-interrupt-function.
int rx_complete = 0; //Global variable that represents whether the command has been completed or not.
int rx_count = 0; //Integer variable. Only used in Nolan's function, so should be local?

char global_message[100]; //This buffer is reused to send uart transmit general messages.
uint8_t tx_buff[1000]; //This buffer is reused to send uart transmit general messages.


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	char local_message[100]= {0}; //Reusable buffer to store messages to be printed to usart.
	char commandArray[3][10]; //Holds the parsed command.
	char translatedCommandArray[30]; // The array of encoded commands that I'm sending to remote.

	//Clear command arrays.
	memset(&translatedCommandArray, 0, strlen((char*)translatedCommandArray));
	for (int i = 0; i < 3; i++) {
		memset(&commandArray[i], 0, strlen((char*)commandArray[i]));
	}
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */



  /*
   * The next lines of code are used for an initial transmitting to PuTTy that the program has been run...
   * and I can start typing into the terminal.

   *As for why this message has to show up twice, I don't know why that is. I'm not transmitting it twice.
   *This is one of those "weird errors". My theory is that this has to do with the flashing.
   */
  strcpy( (char*)tx_buff, "\r\nGround node ready for use. You may begin typing commands.\r\n");
  HAL_UART_Transmit(&huart2, tx_buff, strlen((char*)tx_buff), 500);

  /*
   * This is the initialization of PuTTy receiving a character. It tells PuTTy, "hey,..
   * receive the next character I'm about to type."
   */
  HAL_UART_Receive_IT(&huart2, rx_char, 1); //Start listening for interrupts.



  //------------LORA STUFF-----------------------------------------------------

  /* MODULE SETTINGS */
  myLoRa = newLoRa();

  myLoRa.hSPIx                 = &hspi1;
  myLoRa.CS_port               = GPIOB;
  myLoRa.CS_pin                = CS_Pin;
  myLoRa.reset_port            = GPIOB;
  myLoRa.reset_pin             = reset_Pin;
  myLoRa.DIO0_port                         = dio0_GPIO_Port;
  myLoRa.DIO0_pin                             = dio0_Pin;

  myLoRa.frequency             = 433;                              // default = 433 MHz
  myLoRa.spredingFactor        = SF_7;                            // default = SF_7
  myLoRa.bandWidth                   = BW_62_5KHz;                  // default = BW_125KHz
  myLoRa.crcRate                       = CR_4_5;                        // default = CR_4_5
  myLoRa.power                           = POWER_20db;                // default = 20db
  myLoRa.overCurrentProtection = 120;                             // default = 100 mA
  myLoRa.preamble                       = 10;                              // default = 8;

  LoRa_reset(&myLoRa);
  LoRa_init(&myLoRa);



  /*The following buffer holds the number of transmissions that were attempted.*/
  //uint8_t tx_num = 0;

  /* START CONTINUOUS RECEIVING */
  LoRa_startReceiving(&myLoRa);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /*
     * If I get a enter key, rx_complete is true.
     */
    if (rx_complete == 1) {

    	//Initializations
    	int commandExists = 0;

    	//Clear the command arrays in each completed-command iteration.
    	for (int i = 0; i < 3; i++) {
			memset(&commandArray[i], 0, strlen((char*)commandArray[i]));
		}
        memset(&translatedCommandArray, 0, strlen((char*)translatedCommandArray));

    //Turning the raw data into tokens. Store these tokens in commandArray. Commands have a max of three tokens. Tokens have a max length of 10 characters -------------------------------------------------------
		const char s[2] = " ";
		char *token;
		/*Define where I'm getting tokens from*/
		token = strtok((char*)safe_buff, s);
		/* Walk through other tokens, store them to the subsequent positions of commandArray */
		int i = 0;
		while( token != NULL )
		{
			strcpy(commandArray[i], token);
			i++;
			/*Get the next token*/
			token = strtok(NULL, s);
		}

		//Show the user the three tokens they inputted.
		snprintf((char*)local_message,sizeof(local_message),"\r\nToken 0: %s\r\n",commandArray[0]);
		HAL_UART_Transmit(&huart2, (uint8_t*)local_message, strlen(local_message), 200);
		snprintf((char*)local_message,sizeof(local_message),"Token 1: %s\r\n",commandArray[1]);
		HAL_UART_Transmit(&huart2, (uint8_t*)local_message, strlen(local_message), 200);
		snprintf((char*)local_message,sizeof(local_message),"Token 2: %s\r\n",commandArray[2]);
		HAL_UART_Transmit(&huart2, (uint8_t*)local_message, strlen(local_message), 200);

		//Convert the commandArray (array of words) to translatedCommandArray (array of integers that will be sent to ground.)--------------------------------------------
		if (strcmp(commandArray[0], "led") == 0){
			strcat(translatedCommandArray, "0");
			if (strcmp(commandArray[1], "on") == 0){
				strcat(translatedCommandArray, "0");
				commandExists = 1;
			} else if (strcmp(commandArray[1], "off") == 0) {
				strcat(translatedCommandArray, "1");
				commandExists = 1;
			}
		} else if (strcmp(commandArray[0], "transmit") == 0){
			strcat(translatedCommandArray, "1");
      strcat(translatedCommandArray, commandArray[1]);
			commandExists = 1;
		} else if (strcmp(commandArray[0], "read") == 0){
      strcat(translatedCommandArray, "2");
			if (strcmp(commandArray[1], "temp") == 0){
				strcat(translatedCommandArray, "1");
				commandExists = 1;
			} else if (strcmp(commandArray[1], "photo") == 0){
				strcat(translatedCommandArray, "2");
				commandExists = 1;
			} else if (strcmp(commandArray[1], "buzz") == 0){
				strcat(translatedCommandArray, "3");
				commandExists = 1;
		}
		//For the math, copy the two last tokens in command array into the two last positions of translatedCommandArray
		else if (strcmp(commandArray[0], "add") == 0) {
		  strcat(translatedCommandArray, "3");
		  strcat(translatedCommandArray, commandArray[1]);//argument 1 of the add command
		  strcat(translatedCommandArray, commandArray[2]);//argument 2 of the add command
			commandExists = 1;
		} else if (strcmp(commandArray[0], "subtract") == 0) {
			strcat(translatedCommandArray, "4");
      strcat(translatedCommandArray, commandArray[1]);
      strcat(translatedCommandArray, commandArray[2]);
			commandExists = 1;
		} else if (strcmp(commandArray[0], "multiply") == 0) {
			strcat(translatedCommandArray, "5");
      strcat(translatedCommandArray, commandArray[1]);
      strcat(translatedCommandArray, commandArray[2]);
			commandExists = 1;
		} else if (strcmp(commandArray[0], "divide") == 0) {
			strcat(translatedCommandArray, "6");
      strcat(translatedCommandArray, commandArray[1]);
      strcat(translatedCommandArray, commandArray[2]);
			commandExists = 1;
		} else if (strcmp(commandArray[0], "exponent") == 0) {
			strcat(translatedCommandArray, "7");
      strcat(translatedCommandArray, commandArray[1]);
      strcat(translatedCommandArray, commandArray[2]);
			commandExists = 1;
		}
		//Take the factorial of the second token.
		else if (strcmp(commandArray[0], "factorial") == 0) {
			strcat(translatedCommandArray, "8");
      strcat(translatedCommandArray, commandArray[1]);
			commandExists = 1;
		}

		//Print the contents of the translatedCommandArray.
		snprintf((char*)local_message,sizeof(local_message),"translatedCommandArray: %s\r\n",translatedCommandArray);
		HAL_UART_Transmit(&huart2, (uint8_t*)local_message, strlen(local_message), 200);

		//Processing on what to do whether or not the command exists or not.
		if (commandExists == 0) { //The command does not exist.
			snprintf((char*)local_message,sizeof(local_message),"\r\nThat command does not exist. Please try again.\r\n\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t*)local_message, strlen(local_message), 200);
		} else { //The command exists.
			snprintf((char*)local_message,sizeof(local_message),"\r\nThat command exists.\r\n");
			HAL_UART_Transmit(&huart2, (uint8_t*)local_message, strlen(local_message), 200);

			//LoRa functionality
			uint8_t tx_res = LoRa_transmit(&myLoRa, (uint8_t*)translatedCommandArray, strlen((char*)translatedCommandArray), 1000);
			sprintf((char*)tx_buff, "\r\nTx Result: %u\r\n\r\n", tx_res);
			HAL_UART_Transmit(&huart2, tx_buff, strlen((char*)tx_buff), 1000);
			LoRa_startReceiving(&myLoRa);
		}

		/*
		* IMPORTANT. Reset rx_complete. Otherwise the rx complete code will run all the time.
		*/
		rx_complete = 0;
     }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
} //main end

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 7;
  hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, reset_Pin|CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : dio0_Pin */
  GPIO_InitStruct.Pin = dio0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(dio0_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : reset_Pin CS_Pin */
  GPIO_InitStruct.Pin = reset_Pin|CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

}

/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == huart2.Instance)
        {


            switch (*rx_char)
            {
                // if we receive a carriage return, newline, or null terminator, then we have received a complete command
                case '\r':
                case '\n':
                case '\0':
                {


                	strcpy((char*)safe_buff, (char*)rx_buff);

                    memset(rx_buff, 0, sizeof(rx_buff));
                    rx_count = 0;

                    rx_complete = 1;

                    break;
                }
                /*
                 * FIXME: When user back spaces, the data may reflect that the user backspaced, but when the user sends the command,
                 * the command comes back as unrecognized. Probably because there is some interfering data being left into the incoming_command buffer.
                 *
                 * To replicate error: type in a command, hit back space, then fill out the rest of the command, then hit enter. You will get an unrecognized command error
                 * even though you typed it in correctly.
                 */
                // if we have received a backspace character, then delete the last received character
                //case '\b':
                case 127:
                {
                    // set current buffer position to 0; if we are not already at index 0 then move back one index and set that value to 0 as well
                    //if (rx_count > 0)

                	if (rx_count > 0)
                    {
                    	rx_count --;
                        rx_buff[rx_count] = 0;
                        uint8_t show_input[50];
                        strcpy ( (char*)show_input, (char*)rx_char);
                        HAL_UART_Transmit(&huart2, show_input, strlen((char*)show_input),1000);
                    }
                    //rx_buff[rx_count] = 0;

                    break;
                }

                // we have received some other character, store it to the array
                default:
                {
                    rx_buff[rx_count] = *rx_char;
                    rx_count ++;
                    uint8_t show_input[50];
                    strcpy ( (char*)show_input, (char*)rx_char);
                    HAL_UART_Transmit(&huart2, show_input, strlen((char*)show_input),1000);
                    break;
                }
            }

            *rx_char = 0; //In the switch statement, one of the cases is a 0, so I assume this is necessary.

            /*
             * Leave the following line in the function. If I remove it, the user will only
             * be able to call a single interrupt, thus only be able to type a single
             * character into putty.
             */
            HAL_UART_Receive_IT(&huart2, rx_char, 1);
        }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    //INITIALIZATION OF VARIABLES
    char local_message[100]; //Local buffer used for displaying general messages.
    int rssi = LoRa_getRSSI(&myLoRa); //Local variable for the "Received Signal Strength Indicator"
    char* received_data[128]; //Initialize/create a pointer to a character buffer called received_data of size 30.
    memset(received_data,0,128); //Copies 0 to all 30 cells of my received_data buffer.
    uint8_t packet_size = 0;
    packet_size = LoRa_receive(&myLoRa, received_data, 30);

    //PRINTING INFORMATION ABOUT THE RAW DATA RECEIVED.
    snprintf((char*)local_message,sizeof(local_message),"Packet size: %d\n\r",packet_size);
    HAL_UART_Transmit(&huart2, (uint8_t*)local_message, strlen(local_message), 200);
    snprintf((char*)local_message,sizeof(local_message),"Received data: %s\n\r", received_data);
    HAL_UART_Transmit(&huart2, (uint8_t*)local_message, strlen(local_message), 200);
    snprintf((char*)local_message,sizeof(local_message),"RSSI value: %d\n\r",rssi);
    HAL_UART_Transmit(&huart2, (uint8_t*)local_message, strlen(local_message), 200);


}




/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
