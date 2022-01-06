/*
 * 7seg-functions.h
 *
 *  Created on: Dec 15, 2021
 *      Author: rafee
 */


//Includes
#include "main.h"

//Definitions and Variables

//Prototypes
void moveCursor(int);
void SPI_Transmit(uint8_t);
void clearDisplay();
void writeTest();
void setBrightness(int);
void initializeDisplay();
void startCounter();
void writeToDisplay(char*);
