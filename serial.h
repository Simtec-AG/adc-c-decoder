/**
* This module does read a serial port on a windows machine.
*
* © 2021 Simtec AG. All rights reserved.
* Company Confidential
*
* Compiled and tested with gcc version 8.1.0 (x86_64-posix-seh-rev0, Built by MinGW-W64 project)
*
* Example code only. Use at own risk.
*
* This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
* even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* Simtec AG has no obligation to provide maintenance, support,  updates, enhancements, or modifications.
*/

#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <windows.h>

/** @brief structure of an air-data computer */
typedef struct
{
    char com_port[32];
    uint32_t baudrate;
    HANDLE hCom;
} adc_t;

/**
 * @brief Open and initialize an air data serial interface.
 *
 * @param[in, out]      adc     Air data computer that we want to open.
 *
 * @return EXIT_FAILURE if the stream to the airdata computer couldnt be opened correctly,
 * EXIT_SUCCESS otherwise.
 */
int32_t serial_open(adc_t *adc);

/**
 * @brief Close the serial interface of an air data computer.
 *
 * @param[in,out]       adc   Air data computer to close.
 *
 * @return EXIT_FAILURE if the stream to the airdata computer couldnt be closed correctly,
 * EXIT_SUCCESS otherwise.
 */
int32_t serial_close(adc_t *adc);

/**
 * @brief Read a byte received by an air data computer.
 * @note A call to the function serial_open() shall have been done before calling this function
 *
 * @param[in,out]   adc         Pointer to the air-data-computer returned by the function adc_open()
 * @param[out]      data        Pointer to the variable that will contain the read data.
 * @return EXIT_FAILURE if no new character has been received by the air data computer, 
 * EXIT_SUCCESS otherwise.
 */
int32_t serial_get_data(adc_t *adc, char *data);

#endif