
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

 ## Notes

Compiled with gcc version 10.2.0 (GCC) on windows 10 with mingw64 <http://mingw-w64.org/doku.php>.

Example code only. Use at own risk.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Simtec AG has no obligation to provide maintenance, support,  updates, enhancements, or modifications.