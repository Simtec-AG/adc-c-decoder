# Swiss Air-Data C Decoder
  
## Overview

This small C software decodes data messages sent over the RS-485 interface by air data computers and pressure modules developed by Simtec AG <https://www.swiss-airdata.com>. The decoded data labels are printed onto the terminal. The software has been designed to run on a Windows computer. The computer requires a half-duplex (2-wire) RS-485 serial interface. Alternatively an USB-to-RS485 converter can be used.

The software decodes data labels from the _ADC-10_, _ADS-12_, _AOA-16_, _ADP-5.5_, _PSS-8_ and _PMH_ air data computers and pressure modules. Details about the format can be found in the _Interface Control Document (ICD)_ of the respective device.

The following example shows the output of the decoder to the terminal:

```
Qc   =       0.1 [Pa]  (valid)
Ps   =   97430.0 [Pa]  (valid)
CAS  =      0.46 [m/s] (valid)
TAS  =      0.46 [m/s] (valid)
Hp   =     329.4 [m]   (valid)
Mach =     0.001 [-]   (valid)
SAT  =      24.2 [°C]  (valid)
TAT  =      24.2 [°C]  (valid)
General status = 0x0000

Qc   =       0.2 [Pa]  (valid)
Ps   =   97430.9 [Pa]  (valid)
CAS  =      0.53 [m/s] (valid)
TAS  =      0.53 [m/s] (valid)
Hp   =     329.3 [m]   (valid)
Mach =     0.002 [-]   (valid)
SAT  =      24.2 [°C]  (valid)
TAT  =      24.2 [°C]  (valid)
General status = 0x0000

Qc   =       0.1 [Pa]  (valid)
Ps   =   97431.3 [Pa]  (valid)
CAS  =      0.46 [m/s] (valid)
TAS  =      0.46 [m/s] (valid)
Hp   =     329.3 [m]   (valid)
Mach =     0.001 [-]   (valid)
SAT  =      24.2 [°C]  (valid)
TAT  =      24.2 [°C]  (valid)
General status = 0x0000

Qc   =       0.1 [Pa]  (valid)
Ps   =   97431.0 [Pa]  (valid)
CAS  =      0.46 [m/s] (valid)
TAS  =      0.53 [m/s] (valid)
Hp   =     329.3 [m]   (valid)
Mach =     0.002 [-]   (valid)
SAT  =      24.2 [°C]  (valid)
TAT  =      24.2 [°C]  (valid)
General status = 0x0000
```

## Usage

### Compilation
Execute the following command:

```
make compile
```

### Execution
Launch the following command:
```
decode serial-port [baudrate]
```
Arguments:
- _serial-port_: Serial port on which the air data computer is connected. 
- _baudrate_: Optional argument setting the baudrate which the air data computer uses. By default 230400 is used.

Example calls:
```
decode COM3
decode COM7 115200
```

## Integration

This software has been developed with the goal to ease its reusability as much as possible. 

The core decoder _adc_rs485_decoder.c_ and _adc_rs485_decoder.h_ has been implemented to run on almost any hardware. It only depends on the C standard libraries _stdint_, _stdbool_ and _stdlib_. You can very well take those two files and integrate them in your own code to run on a flight control computer for instance.

## How to write a simple decoder
As stated above, the main logic is in the file _adc_rs485_decoder.c_. However this decoder contains a lot of verification code to detect corrupted data and to accepts all message that could be sent from any Simtec air data computer. The following code contains a minimal implementation for a better understanding of the basic logic.

**1. Wait that a complete message is received**

This part is really easy due to the unique byte _SOH_ that marks the beginning of every messages. The _SOH_ is not used anywhere else in the message. You just have to wait for the correct number of bytes after receiving a _SOH_:

```c
/** Size in byte of the buffer used to decode one message */
#define BUFFER_LENGTH 12

void adc_rs485_decode(char byte)
{
    static uint8_t buffer[BUFFER_LENGTH];
    static uint8_t pos = 0;

    /* Verify if we have received a Start Of Header */
    static const char SOH_1 = 0x01u;
    if (byte == SOH_1)
    {
        buffer[0] = (uint8_t)byte;
        pos = 1;
    }
    else if(pos > 0)
    {
        /* Store the byte received */
        buffer[pos] = (uint8_t)byte;
        pos++;

        /* Check the message length */
        if (pos == 11)
        {
            /* The good number of bytes has been received -> decode */
            rs485_decode_data(buffer, &(returned_message.air_data));
            pos = 0;
        }
    }
}
```

_Note: Alternatively, the unique byte CR can be checked instead of counting the number of byte received_

**2. Decodes the label ID of the data**

The label ID allows to identify the value sent by the air data computer. It is contained in the 4 least significant bits of the 2nd byte of a message. A simple _enum_ with the values corresponding to the one sent by the air data computer can be used for the decoding:

```c
/* Air data label ID for SOH_1 message */
typedef enum 
{
    QC = 1,
    PS = 2,
    AOA = 3,
    AOA = 4,
    CAS = 5,
    TAS = 6,
    HP = 7,
    MACH = 8,
    SAT = 9,
    TAT = 10,
    QNH = 13
} data_type_t;

/* Decodes the label ID */
data->type = (data_type_t) (buffer[1] & 0x0Fu);
```

**3. Decodes the flag**

The flag is contained in bits 4 to 6 of the 2nd byte of a message. The same principle as the one used for the label ID can be use to decode it:

```c
/* Flag returned by the air data computer */
typedef enum
{
    FLAG_VALID = 0,
    FLAG_RANGE_ABOVE = 1,
    FLAG_RANGE_BELLOW = 2,
    FLAG_INVALID_POS = 3,
    FLAG_INVALID_NEG = 4,
    FLAG_INVALID = 5
} flag_t;

/* Decodes the flag */
data->flag = (flag_t)(buffer[1] >> 4) & 0x07u;
```

**4. Decode the value**

The actual value is always a floating point number encoded in a hexadecimal ASCII format. It can be easily decoded in C and other language:

```c
msg[10] = '\0';
int32_t bits = strtoll((char *)&buffer[2], NULL, 16);
float *value_ptr = (float *)&bits;
data->value = *value_ptr;
```

Unless otherwise stated in the _ICD_, values are encoded in metric units.

 ## Notes

Compiled with gcc version 10.2.0 (GCC) on windows 10 with mingw64 <http://mingw-w64.org/doku.php>.

Example code only. Use at own risk.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Simtec AG has no obligation to provide maintenance, support,  updates, enhancements, or modifications.
