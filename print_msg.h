/**
* This module print to the terminal a decoded message received by a swiss air-data computer.
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

#ifndef PRINT_MSG_H
#define PRINT_MSG_H

#include "adc_rs485_decoder.h"

/**
 * @brief Print one message received by an air data computer. This message shall already be decoded!
 *
 * @param[in]   msg     Decoded air-data massage sent by a swiss air-data computer.
*
 */
void print_message(adc_rs485_msg_t *msg);

#endif