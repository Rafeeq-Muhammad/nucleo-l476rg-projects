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
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
char safe_buff[30] = {0}; //Global variable that holds the completed command that was typed into the PuTTy terminal.
uint8_t rx_buff[100] = {0}; //Global variable that holds the command as it's being entered.
uint8_t rx_char[1] = {0}; //Global variable. Passed in the calling of the hal-uart-interrupt-function.
int rx_complete = 0; //Global variable that represents whether the command has been completed or not.
int rx_count = 0; //Integer variable. Only used in Nolan's function, so should be local?
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  char main_message_buffer[50] = {0}; //Reusable buffer to store messages to be printed to usart.
  //char commandTokenOne[5] = {0};
  //char commandTokenTwo[5] = {0};
  //char commandTokenThree[5] = {0};
  char commandArray[3][5]; //Holds the parsed command.
  char translatedCommandArray[30] = {0};
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
  /* USER CODE BEGIN 2 */
  snprintf((char*)main_message_buffer,sizeof(main_message_buffer),"\r\nGround node ready for use. You may begin typing commands.\r\n");
  HAL_UART_Transmit(&huart2, (uint8_t*)main_message_buffer, strlen(main_message_buffer), 200);

  /*
   * This is the initialization of PuTTy receiving a character. It tells PuTTy, "hey,..
   * receive the next character I'm about to type."
   */
  HAL_UART_Receive_IT(&huart2, rx_char, 1); //Start listening for interrupts.
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    if (rx_complete == 1) {

      //Notify the user that a command has been completed.
      snprintf((char*)main_message_buffer,sizeof(main_message_buffer),"\r\nA command has been completed.\r\n");
      HAL_UART_Transmit(&huart2, (uint8_t*)main_message_buffer, strlen(main_message_buffer), 200);

      //Display the complete command.
      snprintf((char*)main_message_buffer,sizeof(main_message_buffer),"Command: %s\r\n", safe_buff);
      HAL_UART_Transmit(&huart2, (uint8_t*)main_message_buffer, strlen(main_message_buffer), 200);

      //Clear the commandToken arrays
      //memset(&commandTokenOne, 0, strlen((char*)commandTokenOne));
      //memset(&commandTokenTwo, 0, strlen((char*)commandTokenTwo));
      //memset(&commandTokenThree, 0, strlen((char*)commandTokenThree));
      for (int i = 0; i < 3; i++) {
        memset(&commandArray[i], 0, strlen((char*)commandArray));
      }
      memset(&translatedCommandArray, 0, strlen((char*)translatedCommandArray));

      //Turning the raw data into tokens. Store these tokens in commandArray. Commands have a max of three tokens. Tokens have a max length of 10 characters -------------------------------------------------------
      const char s[2] = " ";
      char *token;
      /*Define where I'm getting tokens from*/
      token = strtok((char*)safe_buff, s);
      /* Walk through other tokens, store them to the subsequent positions of commandArray */
      int i = 0;
      while( token != NULL ) {
        //if (i == 1) {
        //  strcpy(commandTokenOne, token);
        //} else if (i == 2) {
        //  strcpy(commandTokenTwo, token);
        //} else if (i == 3) {
        //  strcpy(commandTokenThree, token);
        //}
        strcpy(commandArray[i], token);
        
        i++;
        /*Get the next token*/
        token = strtok(NULL, s);
      }

      //Display the contents of the tokens
      snprintf((char*)main_message_buffer,sizeof(main_message_buffer),"Token 1: %s\r\n", commandArray[0]);
      HAL_UART_Transmit(&huart2, (uint8_t*)main_message_buffer, strlen(main_message_buffer), 200);
      snprintf((char*)main_message_buffer,sizeof(main_message_buffer),"Token 2: %s\r\n", commandArray[1]);
      HAL_UART_Transmit(&huart2, (uint8_t*)main_message_buffer, strlen(main_message_buffer), 200);
      snprintf((char*)main_message_buffer,sizeof(main_message_buffer),"Token 3: %s\r\n", commandArray[2]);
      HAL_UART_Transmit(&huart2, (uint8_t*)main_message_buffer, strlen(main_message_buffer), 200);

      if (strcmp(commandArray[0], "led") == 0) {

    	  strcat(translatedCommandArray, "0");

    	  if (strcmp(commandArray[1], "on") == 0) {
    		  strcat(translatedCommandArray, "0");
    	  } else if (strcmp(commandArray[1], "off") == 0) {
    		  strcat(translatedCommandArray, "1");
    	  }
      }

      snprintf((char*)main_message_buffer,sizeof(main_message_buffer),"translatedCommandArray: %s\r\n\r\n", translatedCommandArray);
      HAL_UART_Transmit(&huart2, (uint8_t*)main_message_buffer, strlen(main_message_buffer), 200);


      rx_complete = 0;
    }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

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
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

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
                case '\b':
                {
                    // set current buffer position to 0; if we are not already at index 0 then move back one index and set that value to 0 as well
                    if (rx_count)
                    {
                        rx_buff[rx_count] = 0;
                        rx_count --;
                    }
                    rx_buff[rx_count] = 0;

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
