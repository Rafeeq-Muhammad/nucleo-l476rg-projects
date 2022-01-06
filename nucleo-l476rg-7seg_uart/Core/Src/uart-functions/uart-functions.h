/*
 * Uart.h
 *
 *  Created on: Dec 8, 2021
 *      Author: Rafeeq Muhammad
 */

//Includes
#include "main.h"

//Definitions and variables
extern UART_HandleTypeDef huart2;

/*
	The following variables are used for the HAL_UART_RxCpltCallback function.
	safe_buff : Holds the completed command after enter was pressed.
	rx_buff : Holds the command as it's being typed.
	rx_char : Holds the key that was pressed.
	rx_complete : Represents whether the command has been completed or not.
	rx_count : Holds the position in the buffer that your next character will be placed in.
*/
extern char safe_buff[30];
extern uint8_t rx_buff[30];
extern uint8_t rx_char[1];
extern int rx_complete;
extern int rx_count;

//Prototypes
void Transmit(char*);
void cleanTransmit(char *message);
void cleanTransmitWithIcon(char *message);
void initializeCLI();
void newlineCLI();
void unrecognizedCommand();
void commandComplete();
void processCommands();
