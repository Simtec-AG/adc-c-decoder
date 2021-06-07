/*
 * 2013-2021 (c) Simtec AG
 * All rights reserved
 * 
 * Decodes some important swiss air-data messages.
 * Use a RS485 to USB converter. Contact Simtec 
 * for more details or for a preconfigured and assembled cable.
 * 
 * Compiled and tested with MinGW (gcc)
 * http://sourceforge.net/projects/mingwbuilds/
 *
 * Example code only. Use at own risk.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * Simtec AG has no obligation to provide maintenance, support,
 * updates, enhancements, or modifications.
 */

#include "print_msg.h"
#include "adc_rs485_decoder.h"
#include "serial.h"
#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

// Baud-rate
#define DEFAULT_BAUDRATE 230400


static void print_help()
{
    printf("\n");
    printf("A simple terminal program to print simtec air-data messages\n");
    printf("(c) 2021, Simtec AG\n");
    printf("\n");
}



static void decode_and_print_message(char data)
{

    adc_rs485_msg_t air_data_msg = adc_rs485_decode(data);

    if(air_data_msg.msg_type != RS485_PENDING)
    {
        print_message(&air_data_msg);
    }

}

int main(int argc, char **argv)
{
    int32_t return_code = EXIT_FAILURE;
    print_help();
    serial_port_t adc_serial =
        {
            .baudrate = DEFAULT_BAUDRATE,
            .com_port = "\\\\.\\",
            .windows_handle = NULL};

    if (argc > 2)
    {
        adc_serial.baudrate = strtol(argv[2], NULL, 10);
    }

    if (argc > 1)
    {
        strcat(adc_serial.com_port, argv[1]);

        if (serial_open(&adc_serial) == EXIT_SUCCESS)
        {
            printf("Starting on %s @ B%d\n", adc_serial.com_port, adc_serial.baudrate);
            printf("Hit any key to exit\n\n");

            while (!kbhit())
            {
                char data = 0;
                if (serial_get_data(&adc_serial, &data) == EXIT_SUCCESS)
                {
                    decode_and_print_message(data);
                }
            }

            serial_close(&adc_serial);
        }
        else
        {
            printf("Couldn't open %s", adc_serial.com_port);
        }
    }
    else
    {
        printf("Error, The serial port needs to be passed as an argument! \n");
        printf("E.g.: COM1, COM2, ... \n\n");
    }

    return return_code;
}
