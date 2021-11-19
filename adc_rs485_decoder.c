/*
* © 2021 Simtec AG. All rights reserved.
* Company Confidential
*/

#include "adc_rs485_decoder.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/** Start of header of a data packet */
#define SOH_1 ((char)0x01u)
#define SOH_2 ((char)0x02u)
#define SOH_3 ((char)0x03u)
#define SOH_5 ((char)0x05u)

/** Carriage return */
#define CR ((char)0x0Du)

/** Maximum size in byte of the buffer needed to decode one message */
#define MAX_BUFFER_LENGTH 12

/** Maximum number of label ID per Start Of Header */
#define NUMBER_OF_LABELS_PER_SOH 15

/** Air data label ID meaning for SOH_1 message */
static const data_type_t SOH1_LABEL[NUMBER_OF_LABELS_PER_SOH] = {
    [0] = RS485_DATA_NOT_VALID,
    [1] = RS485_QC,
    [2] = RS485_PS,
    [3] = RS485_AOA,
    [4] = RS485_AOA,
    [5] = RS485_CAS,
    [6] = RS485_TAS,
    [7] = RS485_HP,
    [8] = RS485_MACH,
    [9] = RS485_SAT,
    [10] = RS485_TAT,
    [11] = RS485_DATA_NOT_VALID,
    [12] = RS485_DATA_NOT_VALID,
    [13] = RS485_DATA_NOT_VALID,
    [14] = RS485_QNH};

/** Air data label ID meaning for SOH_2 message */
static const data_type_t SOH2_LABEL[NUMBER_OF_LABELS_PER_SOH] = {
    [0] = RS485_DATA_NOT_VALID,
    [1] = RS485_CR,
    [2] = RS485_PT,
    [3] = RS485_DATA_NOT_VALID,
    [4] = RS485_DATA_NOT_VALID,
    [5] = RS485_CAS_RATE,
    [6] = RS485_TAS_RATE,
    [7] = RS485_HBARO,
    [8] = RS485_DATA_NOT_VALID,
    [9] = RS485_DATA_NOT_VALID,
    [10] = RS485_DATA_NOT_VALID,
    [11] = RS485_DATA_NOT_VALID,
    [12] = RS485_DTR,
    [13] = RS485_HTR,
    [14] = RS485_CUR};

/** Air data label ID meaning for SOH_3 message */
static const data_type_t SOH3_LABEL[NUMBER_OF_LABELS_PER_SOH] = {
    [0] = RS485_DATA_NOT_VALID,
    [1] = RS485_QCRAW,
    [2] = RS485_PSRAW,
    [3] = RS485_DPAOA,
    [4] = RS485_DPAOS,
    [5] = RS485_IAT,
    [6] = RS485_BAT,
    [7] = RS485_DATA_NOT_VALID,
    [8] = RS485_DATA_NOT_VALID,
    [9] = RS485_DATA_NOT_VALID,
    [10] = RS485_DATA_NOT_VALID,
    [11] = RS485_DATA_NOT_VALID,
    [12] = RS485_DATA_NOT_VALID,
    [13] = RS485_DATA_NOT_VALID,
    [14] = RS485_DATA_NOT_VALID};

/** Air data label ID meaning for SOH_4 message */
static const data_type_t SOH4_LABEL[NUMBER_OF_LABELS_PER_SOH] = {RS485_DATA_NOT_VALID};

/** Air data label ID meaning for SOH_5 message */
static const data_type_t SOH5_LABEL[NUMBER_OF_LABELS_PER_SOH] = {
    [0] = RS485_DATA_NOT_VALID,
    [1] = RS485_QC_U,
    [2] = RS485_PS_U,
    [3] = RS485_HP_U,
    [4] = RS485_HBARO_U,
    [5] = RS485_CAS_U,
    [6] = RS485_TAS_U,
    [7] = RS485_CR_U,
    [8] = RS485_DATA_NOT_VALID,
    [9] = RS485_DATA_NOT_VALID,
    [10] = RS485_DATA_NOT_VALID,
    [11] = RS485_DATA_NOT_VALID,
    [12] = RS485_DATA_NOT_VALID,
    [13] = RS485_DATA_NOT_VALID,
    [14] = RS485_DATA_NOT_VALID};

/** Map all labels ber SOH in a 2D table */
static const data_type_t *soh_label_map[5] = {SOH1_LABEL, SOH2_LABEL, SOH3_LABEL, SOH4_LABEL, SOH5_LABEL};

/** 
 * Verify if the string contains only hexadecimal characters encoded in ASCII.
 * @param[in]   string  String containing an encoded float number.
 * @param[in]   string  String containing an encoded float number.
 * @return TRUE is the string is valid, FALSE otherwise
 */
static bool rs485_is_string_hexa(char string[], uint8_t string_length)
{
    bool float_valid = true;

    for(uint8_t i = 0; i < string_length; i++)
    {
        // All characters should be between '0' and '9', or between 'A' and 'F'
        if(((string[i] < '0') || (string[i] > '9')) &&
           ((string[i] < 'A') || (string[i] > 'F')))
           {
               // The character is not valid, exit immediately
               float_valid = false;
               break;
           }
    }
    return float_valid;
}

/** 
 * Decodes an air data message.
 * @note The message length shall be 11 bytes long.
 * @param[in]   msg     Data received by the air data computer.
 * @param[out]  data    Pointer to an air data that will contain the decoded air data.
 * @return RS485_ERROR if there was an error decoding the message, RS485_RETURNED_DATA otherwise.
 */
static rs485_msg_type_t rs485_decode_data(uint8_t msg[], air_data_t *data)
{
    uint8_t label = msg[1] & 0x0Fu;
    uint8_t soh = msg[0];
    rs485_msg_type_t returned_value = RS485_ERROR;

    if ((soh >= SOH_1) && (soh <= SOH_5) && (label < NUMBER_OF_LABELS_PER_SOH))
    {
        data_type_t type = soh_label_map[soh - 1][label];

        if (type != RS485_DATA_NOT_VALID)
        {
            // Mark end of string and convert ASCII-hex to integer
            msg[10] = '\0';

            // Verify that the data bits are well composed only of hexadecimal characters
            if(rs485_is_string_hexa((char *)&msg[2], 8))
            {
                // Cast integer-bits to float
                int32_t bits = strtoll((char *)&msg[2], NULL, 16);
                float *value_ptr = (float *)&bits;
                data->value = *value_ptr;

                // No error, the flag and type can now be updated
                data->flag = (flag_t)(msg[1] >> 4) & 0x07u;
                data->type = type;
                returned_value = RS485_RETURNED_DATA;
            }
        }
    }
    return returned_value;
}

/** 
 * Decodes an air data status message.
 * @note The message length shall be 7 bytes long.
 * @param[in]   msg     Data received by the air data computer.
 * @param[out]  gen_st  Pointer to an air data general status that will contain the decoded air data.
 * @param[out]  htr_st  Pointer to an air data heater status that will contain the decoded air data.
 * @return RS485_ERROR if there was an error decoding the message, the type of status otherwise.
 */
static rs485_msg_type_t rs485_decode_status(uint8_t msg[], adc_gen_status_t *gen_st, htr_status_t *htr_st)
{
    uint8_t label = msg[1] & 0x0Fu;
    uint8_t soh = msg[0];
    rs485_msg_type_t returned_value = RS485_ERROR;

    if (label == 0xFu)
    {
        // Mark end of string and convert ASCII-hex to integer
        msg[6] = '\0';

        // Verify that the status bits are well composed only of hexadecimal characters
        if(rs485_is_string_hexa((char *) &msg[2], 4))
        {
            // Cast ASCII in integer
            int32_t bits = strtol((char *)&msg[2], NULL, 16);
            if (soh == SOH_1)
            {
                gen_st->number = bits;
                returned_value = RS485_RETURNED_STATUS_GEN;
            }
            else if (soh == SOH_2)
            {
                htr_st->number = bits;
                returned_value = RS485_RETURNED_STATUS_HTR;
            }
        }
    }
    return returned_value;
}

/** 
 * Decodes an entire message received from an air data computer. Call the good decoder 
 * depending on the message type.
 * @param[in]   raw_msg         Data received by the air data computer.
 * @param[in]   raw_msg_length  Length of the message received.
 * @param[out]  parsed_msg      Pointer that will contain the decoded air data message.
 * @return void
 */
static void rs485_decode_msg(uint8_t raw_msg[], uint8_t raw_msg_length, adc_rs485_msg_t *parsed_msg)
{
    parsed_msg->msg_type = RS485_ERROR;

    if (raw_msg_length == 11)
    {
        parsed_msg->msg_type = rs485_decode_data(raw_msg, &(parsed_msg->air_data));
    }
    else if (raw_msg_length == 7)
    {
        parsed_msg->msg_type = rs485_decode_status(raw_msg, &(parsed_msg->gen_status), &(parsed_msg->htr_status));
    }
    return;
}

adc_rs485_msg_t adc_rs485_decode(char raw_data)
{
    static uint8_t buffer[MAX_BUFFER_LENGTH] = {0};
    static uint8_t pos = 0;

    adc_rs485_msg_t returned_message;
    returned_message.msg_type = RS485_ERROR;

    if ((raw_data == SOH_1) || (raw_data == SOH_2) || (raw_data == SOH_3) || (raw_data == SOH_5))
    {
        // A SOH marks the beggining of a message
        buffer[0] = (uint8_t)raw_data;
        pos = 1;
        returned_message.msg_type = RS485_PENDING;
    }
    else if ((pos > 0) && (pos < MAX_BUFFER_LENGTH))
    {
        // Store the byte received
        buffer[pos] = (uint8_t)raw_data;

        if (raw_data == CR)
        {
            // A carriage return marks the end of a message, decode
            rs485_decode_msg(buffer, pos + 1, &returned_message);
            pos = 0;
        }
        else
        {
            // Current message is not totally received
            pos++;
            returned_message.msg_type = RS485_PENDING;
        }
    }

    return returned_message;
}
