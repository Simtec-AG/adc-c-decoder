/*
 * 2013-2021 (c) Simtec AG
 * All rights reserved
 * 
 * Decodes some important swiss-airdata messages.
 * Status messages not implemented yet.
 * Use a RS485 to USB converter. Contact Simtec 
 * for more details or for a preconfigured and assembled cable.
 * 
 * Compiled and tested with MinGW (gcc)
 * http://sourceforge.net/projects/mingwbuilds/
 *
 * Example code only. No error handling.
 * Use at own risk.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * Simtec AG has no obligation to provide maintenance, support,
 * updates, enhancements, or modifications.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdint.h>
#include <stdbool.h>
#include <windows.h>
#include <winbase.h>

// Baud-rate
#define DEFAULT_BAUDRATE 230400

// Start of header
#define SOH 0x01

// Linefeed
#define LF  0x0a

// Carriage return
#define CR  0x0d

// Windows file handle
static HANDLE hCom = INVALID_HANDLE_VALUE;

/* Opens and initializes the serial interface. */
static bool serial_open(int baudrate, const char com_port[])
{
    bool fSuccess;
    DCB dcb;

    hCom = CreateFile(com_port, GENERIC_READ | GENERIC_WRITE,
            0, NULL, OPEN_EXISTING, 0, 0/*NULL*/);

    if (hCom == INVALID_HANDLE_VALUE) {
        printf("Cannot open %s\n", com_port);
        return FALSE;
    }

    COMMTIMEOUTS timeouts;
    fSuccess = GetCommTimeouts(hCom, &timeouts);
    if (!fSuccess) {
        printf("Cannot get timeouts on %s\n", com_port);
        return FALSE;
    }

    // No (minimal) blocking!
    timeouts.ReadIntervalTimeout = 1;
    timeouts.ReadTotalTimeoutMultiplier = 0;
    timeouts.ReadTotalTimeoutConstant = 1;
    timeouts.WriteTotalTimeoutMultiplier = 0;
    timeouts.WriteTotalTimeoutConstant = 0;
    fSuccess = SetCommTimeouts(hCom, &timeouts);
    if (!fSuccess) {
        printf("Cannot set timeouts on %s\n", com_port);
        return FALSE;
    }

    fSuccess = GetCommState(hCom, &dcb);
    if (!fSuccess) {
        printf("Cannot comm-state on %s\n", com_port);
        return FALSE;
    }

    dcb.BaudRate = baudrate;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    fSuccess = SetCommState(hCom, &dcb);
    if (!fSuccess) {
        printf("Cannot set comm-state on %s\n", com_port);
        return FALSE;
    }

    return TRUE;
};

/* Closes the serial interface. */
static bool serial_close()
{
    if (hCom != INVALID_HANDLE_VALUE) {
        CloseHandle(hCom);
        hCom = INVALID_HANDLE_VALUE;
        return TRUE;
    } else {
        return FALSE;
    }
};

/* Read a couple of bytes from the serial line. */
static int serial_read(byte buffer[], int size)
{
    DWORD cnt = 0;

    BOOL fSucess = ReadFile(hCom, buffer, size, &cnt, NULL);

    if (!fSucess) {
        printf("Cannot read from requested serial port\n");
        return 0;
    }

    return cnt;
}

static void print_help()
{
    printf("\n");
    printf("A simple terminal program to print simtec air-data messages\n");
    printf("(c) 2021, Simtec AG\n");
    printf("\n");
}

static char* flag_str[] = {
    "valid",
    "range+",
    "range-",
    "invalid+",
    "invalid-",
    "invalid"
};

/* Decode message. */
static void decode_and_print_message(byte msg[])
{
    const char deg = (char) 0xF8;

    // Extract label and flag from second byte
    int label = msg[1] & 0xF;
    int flag = (msg[1] >> 4) & 0x7;

    // Mark end of string and convert ASCII-hex to integer
    msg[10] = '\0';
    long long bits = strtoll((char*) &msg[2], NULL, 16);

    // Cast integer-bits to float
    float ans = *(float*) &bits;

    //
    // Print message
    //
    switch (label) {
        case 1:
            printf("Qc   = %9.1f [Pa]  (%s)\n", ans, flag_str[flag]);
            break;
        case 2:
            printf("Ps   = %9.1f [Pa]  (%s)\n", ans, flag_str[flag]);
            break;
        case 3:
            // ADP-5.5 only
            printf("AoA  = %9.3f [%c]   (%s)\n", ans, deg, flag_str[flag]);
            break;
        case 4:
            // ADP-5.5 only
            printf("AoS  = %9.3f [%c]   (%s)\n", ans, deg, flag_str[flag]);
            break;
        case 5:
            printf("CAS  = %9.2f [m/s] (%s)\n", ans, flag_str[flag]);
            break;
        case 6:
            printf("TAS  = %9.2f [m/s] (%s)\n", ans, flag_str[flag]);
            break;
        case 7:
            printf("Hp   = %9.1f [m]   (%s)\n", ans, flag_str[flag]);
            break;
        case 8:
            printf("Mach = %9.3f [-]   (%s)\n", ans, flag_str[flag]);
            break;
        case 9:
            printf("SAT  = %9.1f [%cC]  (%s)\n", ans, deg, flag_str[flag]);
            break;
        case 10:
            printf("TAT  = %9.1f [%cC]  (%s)\n", ans, deg, flag_str[flag]);
            break;
    }
}

static void read_and_decode_and_print_message()
{
    int pos = 0;
    byte buf = 0;
    byte msg[11];

    // Process as long a user does not hit any key
    while ((pos < 11)) {
        if (serial_read(&buf, 1) == 1) {
            if (buf == SOH) {
                // start a new message if a SOH is found
                pos = 0;
                msg[pos++] = buf;
            } else if (buf == CR && pos == 10) {
                // full data message collected, decode and print now
                msg[pos++] = buf;
                decode_and_print_message(msg);
                break;
            } else if(pos != 0) {
                // add data to message
                msg[pos++] = buf;
            }

            if (pos == 2 && msg[1] == 0x8F) {
                // This is a status message add a new 
                // line to separate message output
                printf("\n");
            }
        }
    }
}

int main(int argc, char** argv)
{
    print_help();

    int baudrate = DEFAULT_BAUDRATE;
    if(argc > 2)
    {
        baudrate = strtol(argv[2], NULL, 10);
    }

    if(argc > 1) {
        char com_port[32] = "\\\\.\\";
        strcat(com_port,argv[1]);

        if (serial_open(baudrate, com_port)) {

            printf("Starting on %s @ B%d\n", com_port, baudrate);
            printf("Hit any key to exit\n\n");

            while (!kbhit()) {
                read_and_decode_and_print_message();
            }

            serial_close();
        }
        return EXIT_SUCCESS;
    }
    else {
        printf("Error, The serial port needs to be passed as an argument! \n");
        printf("E.g.: COM1, COM2, ... \n\n");
        return EXIT_FAILURE;
    }
}
