/*
* © 2021 Simtec AG. All rights reserved.
* Company Confidential
*/

#include "serial.h"
#include <stdbool.h>
#include <stdint.h>
#include <windef.h>
#include <windows.h>

int32_t serial_open(adc_t *adc)
{
    // Windows file handle
    DCB dcb;

    adc->hCom = CreateFile(adc->com_port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (adc->hCom == INVALID_HANDLE_VALUE)
    {
        return EXIT_FAILURE;
    }

    COMMTIMEOUTS timeouts;
    if (GetCommTimeouts(adc->hCom, &timeouts) == 0)
    {
        return EXIT_FAILURE;
    }

    // No (minimal) blocking!
    timeouts.ReadIntervalTimeout = 1;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    if (SetCommTimeouts(adc->hCom, &timeouts) == 0)
    {
        return EXIT_FAILURE;
    }

    if (GetCommState(adc->hCom, &dcb) == 0)
    {
        return EXIT_FAILURE;
    }
    dcb.BaudRate = adc->baudrate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;

    if (SetCommState(adc->hCom, &dcb) == 0)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int32_t serial_close(adc_t *adc)
{
    if (adc->hCom != INVALID_HANDLE_VALUE)
    {
        CloseHandle(adc->hCom);
        adc->hCom = INVALID_HANDLE_VALUE;
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

int32_t serial_get_data(adc_t *adc, char *data)
{
    char buffer[8] = {0};
    long unsigned int cnt = 0;
    bool success = ReadFile(adc->hCom, buffer, 1, &cnt, NULL);
    if (success && (cnt == 1))
    {
        *data = buffer[0];
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}
