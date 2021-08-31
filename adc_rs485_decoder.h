/**
* This module decodes messages received by a swiss air-data computer though RS485.
*
* © 2021 Simtec AG. All rights reserved.
* Company Confidential
*
* Compiled and tested with gcc version 8.1.0 (x86_64-posix-seh-rev0, Built by MinGW-W64 project)
*
* Example code only. Use at own risk.
*
* This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
* even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*
* Simtec AG has no obligation to provide maintenance, support,  updates, enhancements, or modifications.
*/

#ifndef RS485_DECODER_H
#define RS485_DECODER_H

#include <stdint.h>
#include <stdbool.h>

/** @brief flag returned by the air data computer associated with the a value */
typedef enum
{
    FLAG_VALID = 0,
    FLAG_RANGE_ABOVE = 1,
    FLAG_RANGE_BELLOW = 2,
    FLAG_INVALID_POS = 3,
    FLAG_INVALID_NEG = 4,
    FLAG_INVALID = 5
} flag_t;

/** @brief Type of data that can be returned by the air data computer */
typedef enum
{
    RS485_QC,            /**< Impact pressure */
    RS485_PS,            /**< Static pressure */
    RS485_AOA,           /**< Angle of attack */
    RS485_AOS,           /**< Angle of slideslip */
    RS485_CAS,           /**< Calibrate airspeed */
    RS485_TAS,           /**< True airspeed */
    RS485_HP,            /**< Pressure altitude */
    RS485_MACH,          /**< Mach Number */
    RS485_SAT,           /**< Static air temperature */
    RS485_TAT,           /**< Total air temperature */
    RS485_QNH,           /**< QNH Setting */
    RS485_CR,            /**< Climb rate */
    RS485_PT,            /**< Total pressure */
    RS485_CAS_RATE,      /**< Calibrate airspeed rate */
    RS485_TAS_RATE,      /**< Total airspeed rate */
    RS485_HBARO,         /**< Barometric altitude */
    RS485_DTR,           /**< Heater duty rate */
    RS485_HTR,           /**< Heater temperature */
    RS485_CUR,           /**< Heater current */
    RS485_QCRAW,         /**< Uncorrected impact pressure */
    RS485_PSRAW,         /**< Uncorrected static pressure */
    RS485_DPAOA,         /**< Delta pressure AoA */
    RS485_DPAOS,         /**< Delta pressure AoS */
    RS485_IAT,           /**< Indicate air temperature */
    RS485_BAT,           /**< Base temperature */
    RS485_QC_U,          /**< User selected unit, impact pressure */
    RS485_PS_U,          /**< User selected unit, static pressure */
    RS485_HP_U,          /**< User selected unit, pressure altitude */
    RS485_HBARO_U,       /**< User selected unit, barometric pressure */
    RS485_CAS_U,         /**< User selected unit, calibrate airspeed */
    RS485_TAS_U,         /**< User selected unit, true airspeed */
    RS485_CR_U,          /**< User selected unit, climb rate */
    RS485_DATA_NOT_VALID /**< The type of data received is not valid */
} data_type_t;

/** @brief Decoded data sent by an air data computer */
typedef struct
{
    data_type_t type;
    float value;
    flag_t flag;
} air_data_t;

/** @brief Mode in which an air data computer can be */
typedef enum
{
    ADC_MODE_NORMAL = 0,
    ADC_MODE_MAINTENANCE = 1,
    ADC_MODE_CALIBRATION = 2
} adc_mode_t;

/** @brief General status decoded received by an air data computer */
typedef union
{
    struct
    {
        adc_mode_t mode : 2;
        bool serial_error : 1;
        bool sensor_error : 1;
        bool spi_error : 1;
        bool crc_error : 1;
        bool memory_error : 1;
        bool io_error : 1;
        bool other_reset : 1;
        bool cpu_reset : 1;
        bool bod_reset : 1;
        bool watchdog_reset : 1;
        bool qnh_set : 1;
        bool sensor_zeroed : 1;
        uint8_t not_used : 2;
    };

    uint16_t number;
} adc_gen_status_t;

/** @brief Mode of the air data boom heater controlled by an air data computer */
typedef enum
{
    HEATER_MODE_OFF = 0,
    HEATER_MODE_ON = 1,
    HEATER_MODE_AUTO = 2
} heater_mode_t;

/** @brief Air data boom heater status decoded, received by an air data computer */
typedef union
{
    struct
    {
        uint16_t not_used : 11;
        bool high_temperature : 1;
        bool low_temperature : 1;
        bool failure : 1;
        heater_mode_t mode : 2;
    };
    uint16_t number;
} htr_status_t;

/** @brief Type of message that could be returned by the air data rs485 decoder */
typedef enum
{
    RS485_PENDING = 0,             /**< No complete message has been decoded yet*/
    RS485_RETURNED_DATA = 1,       /**< One complete data message has been decoded */
    RS485_RETURNED_STATUS_GEN = 2, /**< One general status message has been decoded */
    RS485_RETURNED_STATUS_HTR = 3, /**< One heater status message has been decoded */
    RS485_ERROR = 4                /**< An error happened during the decoding of the message */
} rs485_msg_type_t;

/** @brief Decoded RS485 air data message sent by a swiss air-data computer*/
typedef struct
{
    rs485_msg_type_t msg_type;
    union
    {
        air_data_t air_data;
        adc_gen_status_t gen_status;
        htr_status_t htr_status;
    };
    
}adc_rs485_msg_t;

/**
 * @brief Decodes a message transmitted by a swiss air-data computer through RS485.
 * This function shall be called everytime a new byte has been received.
 * If the byte is the last of a message and this message was decoded successfully, an rs485 air data 
 * message will be returned.
 * If the message is not fully decoded yet, the rs485 message type will be RS485_PENDING.
 * If there was an error decoding the message, the rs485 message type will be RS485_ERROR.
 *
 * @param[in]   raw_data    Raw 8 bits data received by an air data computer.
*
 * @return Decoded air data message.
 */
adc_rs485_msg_t adc_rs485_decode(char raw_data);

#endif
