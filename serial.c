/*
* © 2021 Simtec AG. All rights reserved.
* Company Confidential
*/

#include "serial.h"
#include <stdbool.h>
#include <stdint.h>
#include <windef.h>
#include <windows.h>

int32_t serial_open(serial_port_t *serial)
{
    // Windows file handle
    DCB dcb;

    serial->windows_handle = CreateFile(serial->com_port, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
    if (serial->windows_handle == INVALID_HANDLE_VALUE)
    {
        return EXIT_FAILURE;
    }

    COMMTIMEOUTS timeouts;
    if (GetCommTimeouts(serial->windows_handle, &timeouts) == 0)
    {
        return EXIT_FAILURE;
    }

    // No (minimal) blocking!
    timeouts.ReadIntervalTimeout = 1;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    if (SetCommTimeouts(serial->windows_handle, &timeouts) == 0)
    {
        return EXIT_FAILURE;
    }

    if (GetCommState(serial->windows_handle, &dcb) == 0)
    {
        return EXIT_FAILURE;
    }
    dcb.BaudRate = serial->baudrate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;

    if (SetCommState(serial->windows_handle, &dcb) == 0)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

int32_t serial_close(serial_port_t *serial)
{
    if (serial->windows_handle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(serial->windows_handle);
        serial->windows_handle = INVALID_HANDLE_VALUE;
        return EXIT_SUCCESS;
    }
    else
    {
        return EXIT_FAILURE;
    }
}

int32_t serial_get_data(serial_port_t *serial, char *data)
{
    char buffer[8] = {0};
    long unsigned int cnt = 0;
    bool success = ReadFile(serial->windows_handle, buffer, 1, &cnt, NULL);
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
