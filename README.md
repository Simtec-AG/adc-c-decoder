
  # Swiss Air-Data C Decoder
  
## Overview

This small C software decodes messages sent over the RS-485 interface by an air data computer from Simtec AG <https://www.swiss-airdata.com/> and print the result on the terminal. It has been designed to run on a Windows computer and requires the usage of a RS485 to USB converter. Contact Simtec for more details or for a preconfigured and assembled cable.

This software is compatible with the RS-485 data interface of the following products: ADC-10, ADS-12, AOA-16, ADP-5.5, PSS8 and PMH air data computer. Details about the format used to send the air data can be found in devices *Interface Control Document*.

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
Simply execute the following command:

```
make compile
```
 ### Execution
 Launch the following command:
```
decode serial-port [baudrate]
```
Arguments:
- _serial-port_: Serial port on which the swiss air-data computer is connected. 
- _baudrate_: Optional argument setting the baudrate which the air data computer uses. By default 230400 is used.

Example calls:
```
decode COM3
decode COM7 115200
```

 ## Integration

This software has been developed with the goal to ease its reusability as much as possible. 

The core decoder _adc_rs485_decoder.c_ and _adc_rs485_decoder.h_ has been implemented to run on almost any hardware. It only requires to have the C standard librairies _stdint_, _stdbool_ and _stdlib_. You can very well take those two files and integrate them in your own code to run on a flight control computer for instance.

 ## How to write a simple decoder
As stated above, the main logic is in the file _adc_rs485_decoder.c_. However this files contains lot of verification (against corrupted data) and accept all message that could be sent from any Simtec air data computer. Therefore it could be a little confusing at first look, and could be perceived as more complex as it really is. 

Therefore here is a small description of the logic that you should follow if you want to implement your own simple decoder without error verification.

**1. Wait that a complete message is received**

This part is really easy due to the unique byte SOH that marks the beginning of all messages, and can't be found anywhere else. So you just have to wait that the good number of bytes have been received since a SOH. 

Here is an example code in C:

```c
void adc_rs485_decode(char byte)
{
    static uint8_t buffer[MAX_BUFFER_LENGTH];
    static uint8_t pos = 0;

    /* Verify if we have received a Start Of Header */
    static const char SOH_1 = 0x01u;
    if (byte == SOH_1)
    {
        buffer[pos] = (uint8_t)byte;
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
      }
    }
}
```

_Note: Alternatively, the unique byte CR can be checked instead of counting the number of byte received_

**2. Decodes the label ID of the data**

The label ID allow to identify the value sent by the air data computer. It is contained in the 4 LSB of the 2nd byte of a message. A simple enum with the values corresponding to the one sent by the air data computer can be used for the decoding.

Here is an example code in C:

```c
/* Air data label ID for SOH_1 message */
typedef enum 
{
    DATA_NOT_VALID_0 = 0,
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
    DATA_NOT_VALID_11 = 11,
    DATA_NOT_VALID_12 = 12,
    DATA_NOT_VALID_14 = 12,
    QNH = 13,
    DATA_NOT_VALID_14 = 14
} data_type_t;

/* Decodes the label ID */
data->type = (data_type_t) (msg[1] & 0x0Fu);
```

**3. Decodes the flag**

The flag is contained in the bits 4 to 6 of the 2nd byte of a message. The same principle as for the label can be use to decode it.

Here is an example code in C:

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
data->flag = (flag_t)(msg[1] >> 4) & 0x07u;
```

**4. Decode the value**

The actual value is always a floating point number that is encoded in a hexadecimal ASCII format. It can be very easily decoded in C as well as many other language.

Here is an example code in C using the stdlib:


 ```c
msg[10] = '\0';
int32_t bits = strtoll((char *)&msg[2], NULL, 16);
float *value_ptr = (float *)&bits;
 ```

By default, the unit used is the one taken from the metric system.

 ## Notes

Compiled with gcc version 10.2.0 (GCC) on windows 10 with mingw64 <http://mingw-w64.org/doku.php>.

Example code only. Use at own risk.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Simtec AG has no obligation to provide maintenance, support,  updates, enhancements, or modifications.