/*
 * 2013-2021 (c) Simtec AG
 * All rights reserved
 * 
 * Decodes some important swiss-airdata messages.
 * Status messages not implemented yet.
 * Use a RS485 to USB converter. Contact Simtec 
 * for more details or for a preconfigured and assembled cable.
 * 
 * User must set correct serial device.
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

// Replace with COM1, COM2, etc
// \\.\CNCB0 is used by com0com
#define DEVICE "\\\\.\\CNCB0"

// Baud-rate
#define BAUDRATE 230400

// Start of header
#define SOH 0x01

// Linefeed
#define LF  0x0a

// Carriage return
#define CR  0x0d

// Windows file handle
static HANDLE hCom = INVALID_HANDLE_VALUE;

/* Opens and initializes the serial interface. */
static bool serial_open()
{
    bool fSuccess;
    DCB dcb;

    hCom = CreateFile(DEVICE, GENERIC_READ | GENERIC_WRITE,
            0, NULL, OPEN_EXISTING, 0, 0/*NULL*/);

    if (hCom == INVALID_HANDLE_VALUE) {
        printf("Cannot open %s\n", DEVICE);
        return FALSE;
    }

    COMMTIMEOUTS timeouts;
    fSuccess = GetCommTimeouts(hCom, &timeouts);
    if (!fSuccess) {
        printf("Cannot get timeouts on %s\n", DEVICE);
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
        printf("Cannot set timeouts on %s\n", DEVICE);
        return FALSE;
    }

    fSuccess = GetCommState(hCom, &dcb);
    if (!fSuccess) {
        printf("Cannot comm-state on %s\n", DEVICE);
        return FALSE;
    }

    dcb.BaudRate = BAUDRATE;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    dcb.fRtsControl = RTS_CONTROL_DISABLE;
    fSuccess = SetCommState(hCom, &dcb);
    if (!fSuccess) {
        printf("Cannot set comm-state on %s\n", DEVICE);
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
        printf("Cannot read from serial %s\n", DEVICE);
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
    while ((pos < 11) && !kbhit()) {
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
            } else {
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

    if (serial_open()) {

        printf("Starting on %s @ B%d\n", DEVICE, BAUDRATE);
        printf("Hit any key to exit\n\n");

        while (!kbhit()) {
            read_and_decode_and_print_message();
        }

        serial_close();
    }

    return EXIT_SUCCESS;
}
