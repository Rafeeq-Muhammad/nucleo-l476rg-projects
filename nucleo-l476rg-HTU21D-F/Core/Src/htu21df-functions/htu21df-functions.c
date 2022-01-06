/*
 * sensor-functions.c
 *
 *  Created on: Dec 14, 2021
 *      Author: rafee
 */

/*
 Important notes from the data sheet:

START-UP
The HTU21D(F) sensor requires a voltage supply between 1.5V and 3.6V. After power up, the device needs at
most 15ms while SCK is high for reaching idle state (sleep mode), i.e to be ready accepting commands from the
MCU. No command should be sent before that time. Soft reset is recommended at start, refer p.11.


SENDING A COMMAND
After sending the start condition, the subsequent I²C header consist of a 7-bit I²C device address 0x40 and a
DATA direction bit (‘0’ for Write access : 0x80). The HTU21D(F) sensor indicates the proper reception of a byte
by pulling the DATA pin low (ACK bit) after the falling edge of the 8th SCK clock. After the issue of a
measurement command (0xE3 for temperature, 0xE5 for relative humidity), the MCU must wait for the
measurement to complete. The basic commands are given in the table below

TABLE ON PAGE 11

BITS
the two last least significant bits
(LSBs, bits 43 and 44) are used for transmitting status information. Bit 1 of the two LSBs indicates the
measurement type (‘0’: temperature, ‘1’: humidity). Bit 0 is currently not assigned.
+ diagram on page

For Hold Master sequence, bit 45 may be changed to NACK followed by a stop condition to omit checksum
transmission


In those examples, the HTU21D(F) sensor output is SRH = ‘0111’1100’1000’0000 (0x7C80). For the calculation
of physical values status bits must be set to ‘0’. Refer to “Conversion of signal outputs” section p.14.


SOFT RESET
This command is used for rebooting the HTU21D(F) sensor switching the power off and on again. Upon
reception of this command, the HTU21D(F) sensor system reinitializes and starts operation according to the
default settings with the exception of the heater bit in the user register. The soft reset takes less than 15ms.

master transimt: address/write // command
master receive: addresss+read // 2 bytes of data */


//Includes
#include "htu21df-functions.h"

#include "main.h"
#include <string.h>
#include <stdio.h>

//Definitions and Variables
I2C_HandleTypeDef hi2c1;
HAL_StatusTypeDef ret;

static const uint8_t device_address = 0x40 << 1;
static const uint8_t temperature_command = 0xE3;
static const uint8_t humidity_command = 0xE5;
static uint8_t buf[12]; //destination buffer of raw data
static int16_t raw_temp_data;
static int16_t raw_humid_data;
static float temp_c;
static float temp_f;
static float humidity;

float getTemp() {
	//Temperature

	  buf[0] = temperature_command;
	  ret = HAL_I2C_Master_Transmit(&hi2c1, device_address, buf, 1, 50);
	  if (ret != HAL_OK) {
		  strcpy((char*)buf, "Error Tx\r\n");
	  } else {
		  ret = HAL_I2C_Master_Receive(&hi2c1, device_address, buf, 2, 50);
		  if (ret != HAL_OK) {
			  strcpy((char*)buf, "Error Rx\r\n");
		  } else{
			  //combine the bytes to construct our 14-bit or 12 bit or 8 bit value
			  raw_temp_data = ((int16_t)buf[0] << 8) | buf[1] >> 0;

			  //conversions
			  temp_c = -46.85 + 175.72*((float)raw_temp_data / (65536));
			  temp_f = (temp_c * 9 / 5 ) + 32;
		  }
	  }
	  return temp_f;
}

float getHumid() {
	//humidity

    buf[0] = humidity_command;
    ret = HAL_I2C_Master_Transmit(&hi2c1, device_address, buf, 1, 50);
    if (ret != HAL_OK) {
      strcpy((char*)buf, "Error Tx\r\n");
    } else {
      ret = HAL_I2C_Master_Receive(&hi2c1, device_address, buf, 2, 50);
      if (ret != HAL_OK) {
        strcpy((char*)buf, "Error Rx\r\n");
      } else{
        //combine the bytes to construct our 14-bit or 12 bit or 8 bit value
        raw_humid_data = ((int16_t)buf[0] << 8) | buf[1] >> 0;

        //conversions
        humidity = -6 + 125*((float)raw_humid_data / (65536)); 
      }
    }
    return humidity;
}
