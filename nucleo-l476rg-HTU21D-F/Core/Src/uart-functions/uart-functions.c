/*
 * Uart.c
 *
 *  Created on: Dec 8, 2021
 *      Author: Rafeeq
 */

//Includes
#include "uart-functions.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

//HAL_UART_RxCpltCallback Variables
char safe_buff[30];
uint8_t rx_buff[30];
uint8_t rx_char[1];
int rx_complete = 0;
int rx_count = 0;

//newlineCLI Variables
char icon[1] = ">"; //>, ~, $, #, @

//Information Variables
char VERSION[10] = "0.0.001";
char DATE[10] = "12/11/2021";

int listOfCommandsSize = 3;
char listOfCommands[3][10] = {
		"help",
		"info",
		"goat",
};


/*
 * Name: 
 *
 * Author:
 *
 * Description:
 */


/*
 * Name: Transmit
 *
 * Author: Rafeeq Muhammad
 *
 * Description: Accepts an array of characters and transmits that array through uart. The array
 * of characters can be passed in the form of a buffer or a raw string.
 */
void Transmit(char *message) {
	HAL_UART_Transmit(&huart2, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
}

/*
 * Name: cleanTransmit
 *
 * Author: Rafeeq Muhammad
 *
 * Description: Transmit() but with two newline characters before and after the message.
 */
void cleanTransmit(char *message) {
	char cleanTransmit_buf[500];
	snprintf(cleanTransmit_buf, sizeof(cleanTransmit_buf), "\r\n\r\n%s\r\n\r\n", message);
	HAL_UART_Transmit(&huart2, (uint8_t*)cleanTransmit_buf, strlen(cleanTransmit_buf), HAL_MAX_DELAY);
}

/*
 * Name: cleanTransmitWithIcon
 *
 * Author: Rafeeq Muhammad
 *
 * Description: cleanTransmit() but with a newline and a token.
 */
void cleanTransmitWithIcon(char *message) {
	char cleanTransmit_buf[500];
	snprintf(cleanTransmit_buf, sizeof(cleanTransmit_buf), "\r\n\r\n%s\r\n\r\n%s ", message, icon);
	HAL_UART_Transmit(&huart2, (uint8_t*)cleanTransmit_buf, strlen(cleanTransmit_buf), HAL_MAX_DELAY);
}

/*
 * Name: HAL_UART_RxCpltCallback
 *
 * Authors: Nolan, Rafeeq Muhammad
 *
 * Description: This is an interrupt function. It is called everytime the user inputs a key into PuTTy.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)  {

    if (huart->Instance == huart2.Instance) {

		switch (*rx_char) {
			//Enter
			case '\r':
			case '\n':
			case '\0':
			{
				if (rx_count){
					strcpy((char*)safe_buff, (char*)rx_buff);
					memset(rx_buff, 0, sizeof(rx_buff));
					rx_count = 0;
					rx_complete = 1;
					break;
				}
				else {
					cleanTransmitWithIcon("No command was entered.");
				}
			}

			//Backspace
			case 127:
			{
				if (rx_count)
				{
					rx_count --;
					rx_buff[rx_count] = 0;
					Transmit((char*)rx_char);
				}
				break;
			}

			//Any other character
			default:
			{
				rx_buff[rx_count] = *rx_char;
				rx_count ++;
				Transmit((char*)rx_char);
				break;
			}
		}

		if (rx_complete == 1) {
			commandComplete();
		}

		HAL_UART_Receive_IT(&huart2, rx_char, 1);
	}
}

/*
 * Name: initializeCLI
 *
 * Author: Rafeeq Muhammad
 *
 * Description: Initializes the CLI with the name of the project, version, and copyright.
 */
void initializeCLI() {
	char initializeCLI_buf[100];
	snprintf(initializeCLI_buf, sizeof(initializeCLI_buf), "STM32 CapeShell. [Version %s]\r\n", VERSION);
	Transmit(initializeCLI_buf);
	Transmit("(c) Cajun Advanced Pico-Satellite Experiment. All rights reserved. ~\r\n\r\n");
	Transmit("Type 'help' for a list of commands.\r\n");
	newlineCLI();
	HAL_UART_Receive_IT(&huart2, rx_char, 1);
}

/*
 * Name: newlineCLI()
 *
 * Author: Rafeeq Muhammad
 *
 * Description: Transmits a new line and a token-starter line.
 */
void newlineCLI() {
	char CLI_buf[100];
	snprintf(CLI_buf, sizeof(CLI_buf), "\r\n%s " , icon);
	Transmit(CLI_buf);
}

/*
 * Name: unrecognizedCommand
 *
 * Author: Rafeeq Muhammad
 *
 * Description: Called when the user inputs an unrecognized command. Notifies the user that the command is unrecognized.
 */
void unrecognizedCommand() {
	char unrecognizedCommand_buf[100];
	snprintf(unrecognizedCommand_buf, sizeof(unrecognizedCommand_buf), "CapeShell: %s: command not found.", safe_buff);
	cleanTransmitWithIcon(unrecognizedCommand_buf);
}

/*
 * Name: commandComplete
 *
 * Author: Rafeeq Muhammad
 *
 * Description: Called when the user presses enter and completes their command.
 */
void commandComplete() {
	processCommands();
	rx_complete = 0;
}

/*
 * Name: procssCommands
 *
 * Author: Rafeeq Muhammad
 *
 * Description: Called to process the command the user inputted.
 * Checks and displays if the command exists.
 * Processes correctly inputted commands.
 */
void processCommands() {
	int commandExists = 0;
	char processCommands_buf[500];

	//Check if the command exists
	for (int i = 0; i < listOfCommandsSize; i++) {
		if (strcmp(safe_buff, listOfCommands[i]) == 0) {
			commandExists = 1;
		}
	}

	if (commandExists == 1) {
		//Process the command
		if (strcmp(safe_buff, "help") == 0) {
			snprintf(processCommands_buf, sizeof(processCommands_buf), "\r\n\r\nCommands\r\n\r\n");
			Transmit(processCommands_buf);
			//Transmit(listOfCommands[1]);
			for (int i = 0; i < listOfCommandsSize; i++) {
				Transmit(listOfCommands[i]);
				Transmit("\r\n");
			}
			newlineCLI();
		}
		else if (strcmp(safe_buff, "info") == 0) {
			Transmit("\r\n\r\nAuthor: Rafeeq Muhammad\r\n");

			snprintf(
				processCommands_buf,
				sizeof(processCommands_buf),
				"Build: %s\r\nVersion Date: %s\r\n",
				VERSION, DATE);
			Transmit(processCommands_buf);

			Transmit("Latest build notes: Fixed the backspace error. Backspace is now fully functional.");

			cleanTransmitWithIcon("CapeShell is a prototype CLI for the Cajun Advanced Pico-Satellite Experiment.");
		}
		else if (strcmp(safe_buff, "goat") == 0) {
			snprintf(processCommands_buf, sizeof(processCommands_buf), "Rizwan Merchant, aka RizLord, the God of Cape");
			cleanTransmitWithIcon(processCommands_buf);
		}
	}
	else {
		unrecognizedCommand();
	}
}

