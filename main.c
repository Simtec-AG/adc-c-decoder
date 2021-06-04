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
#include "rs485_decoder.h"
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

    rs485_message_t msg = rs485_decode(data);

    if(msg.message_type != RS485_PENDING)
    {
        print_message(&msg);
    }

}

int main(int argc, char **argv)
{
    int32_t return_code = EXIT_FAILURE;
    print_help();
    adc_t adc =
        {
            .baudrate = DEFAULT_BAUDRATE,
            .com_port = "\\\\.\\",
            .hCom = NULL};

    if (argc > 2)
    {
        adc.baudrate = strtol(argv[2], NULL, 10);
    }

    if (argc > 1)
    {
        strcat(adc.com_port, argv[1]);

        if (serial_open(&adc) == EXIT_SUCCESS)
        {
            printf("Starting on %s @ B%d\n", adc.com_port, adc.baudrate);
            printf("Hit any key to exit\n\n");

            while (!kbhit())
            {
                char data = 0;
                if (serial_get_data(&adc, &data) == EXIT_SUCCESS)
                {
                    decode_and_print_message(data);
                }
            }

            serial_close(&adc);
        }
        else
        {
            printf("Couldn't open %s", adc.com_port);
        }
    }
    else
    {
        printf("Error, The serial port needs to be passed as an argument! \n");
        printf("E.g.: COM1, COM2, ... \n\n");
    }

    return return_code;
}
