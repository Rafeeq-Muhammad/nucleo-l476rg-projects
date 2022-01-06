/*
 * 7seg-functions.c
 *
 *  Created on: Dec 15, 2021
 *      Author: rafee
 */



//Includes
#include "7seg-functions.h"


#include "main.h"
#include <string.h>
#include <stdio.h>

//Definitions and Variables
SPI_HandleTypeDef hspi1;
uint8_t data_buff_1 = 'T';
uint8_t data_buff_2 = 'E';
uint8_t data_buff_3 = 'S';
uint8_t data_buff_4 = 'T';
uint8_t colon_cmd[2] = {0x77, 0x10}; // colon enable
uint8_t brightness_val = 0; //0-254
uint8_t clear_display_cmd[1] = {0x76};
/*
{0x79, 0x00} brings cursor to first digit place
{0x79, 0x01} brings cursor to second digit place
{0x79, 0x02} brings cursor to third digit place
{0x79, 0x03} brings cursor back to fourth digit place
*/
uint8_t digit_cmd_array[4][2] = { {0x79, 0x00} , {0x79, 0x01} , {0x79, 0x02}, {0x79, 0x03} };
int timeout = 50;

static void gpioSetReset() {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
}

void moveCursor(int position) {
	HAL_SPI_Transmit(&hspi1, digit_cmd_array[position], sizeof(digit_cmd_array[position]), timeout);
	gpioSetReset();
}

/*
Write to the digit that the cursor is currently pointing to.
*/
void writeToDigit(uint8_t data) {
	HAL_SPI_Transmit(&hspi1, &data, sizeof(data), timeout);
	gpioSetReset();
}

/*
Write to all 4 digits at once.
*/
void writeToDisplay(char* data) {
	//data = (uint8_t)data;

	uint8_t data_buff_1 = (uint8_t)data[0];
	uint8_t data_buff_2 = (uint8_t)data[1];
	uint8_t data_buff_3 = (uint8_t)data[2];
	uint8_t data_buff_4 = (uint8_t)data[3];
	uint8_t data_array[4] = {data_buff_1, data_buff_2, data_buff_3, data_buff_4};


	for (int j = 0; j < 4; j++) {

		if (data_array[j]) {
			//move cursor
			moveCursor(j);

			//write to display
			writeToDigit(data_array[j]);
			//HAL_Delay(1000);
		}
    }
}

void setBrightness(int brightness) {
	uint8_t brightness_array[2] ={0x7A, (uint8_t)brightness}; // brightness ranges from 0 to 254
	HAL_SPI_Transmit(&hspi1, brightness_array, sizeof(brightness_array), timeout);
	gpioSetReset();
}

void writeTest() {

	uint8_t data_buff_1 = 'T';
	uint8_t data_buff_2 = 'E';
	uint8_t data_buff_3 = 'S';
	uint8_t data_buff_4 = 'T';
	uint8_t data_buff_array[4] = {data_buff_1, data_buff_2, data_buff_3, data_buff_4};

	//Writes TEST on display
	for (int i = 0; i < 4; i++) {
		//move cursor
		HAL_SPI_Transmit(&hspi1, digit_cmd_array[i], sizeof(digit_cmd_array[i]), timeout);
		gpioSetReset();

		//write to display
		HAL_SPI_Transmit(&hspi1, &data_buff_array[i], sizeof(data_buff_array[i]), timeout);
		gpioSetReset();
	}
}

void clearDisplay() {
	HAL_SPI_Transmit(&hspi1, clear_display_cmd, sizeof(clear_display_cmd), timeout);
	gpioSetReset();
}

void initializeDisplay() {
	//CS pin default high
	//HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
	
	//CS pin default/idle low
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);
}

void startCounter() {
	for (int i = 0; i < 10000; i++){
		data_buff_1 = (i/1000) % 10;
		data_buff_2 = (i/100) % 10;
		data_buff_3 = (i/10) % 10;
		data_buff_4 = (i/1) % 10;
		uint8_t data_buff_array[4] = {data_buff_1, data_buff_2, data_buff_3, data_buff_4};

	      for (int j = 0; j < 4; j++) {
			//move cursor
			moveCursor(j);

			//write to display
			writeToDigit(data_buff_array[j]);
			HAL_Delay(1000);
	      }
	  }
}
