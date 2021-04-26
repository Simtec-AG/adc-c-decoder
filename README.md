
  # Swiss Air Data C Decoder
  
## Overview

This small C software decodes air data frame sent by a module from Simtec AG <https://www.swiss-airdata.com/> and print the result to the terminal. It has be designed to run on a windows computer and requires the usage a RS485 to USB converter. Contact Simtec for more details or for a preconfigured and assembled cable.

It is compatible with the RS-485 data interface of the following products: ADES-12, ADP-5.5, PSS8 and PMH air data computer. Details about the format used to send the air data can be found in devices *Interface Control Document*.

Here is an example of what is the decoder output to the the serial terminal:

```
Qc   =       0.1 [Pa]  (valid)
Ps   =   97430.0 [Pa]  (valid)
CAS  =      0.46 [m/s] (valid)
TAS  =      0.46 [m/s] (valid)
Hp   =     329.4 [m]   (valid)
Mach =     0.001 [-]   (valid)
SAT  =      24.2 [°C]  (valid)
TAT  =      24.2 [°C]  (valid)

Qc   =       0.2 [Pa]  (valid)
Ps   =   97430.9 [Pa]  (valid)
CAS  =      0.53 [m/s] (valid)
TAS  =      0.53 [m/s] (valid)
Hp   =     329.3 [m]   (valid)
Mach =     0.002 [-]   (valid)
SAT  =      24.2 [°C]  (valid)
TAT  =      24.2 [°C]  (valid)

Qc   =       0.1 [Pa]  (valid)
Ps   =   97431.3 [Pa]  (valid)
CAS  =      0.46 [m/s] (valid)
TAS  =      0.46 [m/s] (valid)
Hp   =     329.3 [m]   (valid)
Mach =     0.001 [-]   (valid)
SAT  =      24.2 [°C]  (valid)
TAT  =      24.2 [°C]  (valid)

Qc   =       0.1 [Pa]  (valid)
Ps   =   97431.0 [Pa]  (valid)
CAS  =      0.46 [m/s] (valid)
TAS  =      0.53 [m/s] (valid)
Hp   =     329.3 [m]   (valid)
Mach =     0.002 [-]   (valid)
SAT  =      24.2 [°C]  (valid)
TAT  =      24.2 [°C]  (valid)
```

**Warning: Only data packet are parsed. Status and commands such as QNH setting and sensor zeroing are ignored by this extension** 

 ## Notes

Tested with the toolchain mingw64 <http://mingw-w64.org/doku.php>, gcc version 10.2.0 (GCC) on windows 10.

Example code only. No error handling. Use at own risk.

This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

Simtec AG has no obligation to provide maintenance, support,  updates, enhancements, or modifications.