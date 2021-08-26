/*
* © 2021 Simtec AG. All rights reserved.
* Company Confidential
*/

#include "print_msg.h"
#include "adc_rs485_decoder.h"
#include <stdio.h>
#include <string.h>
#include <stdint.h>

/** @brief flag string representation */
static char *flag_str[] = {
    [FLAG_VALID] = "valid",
    [FLAG_RANGE_ABOVE] = "range+",
    [FLAG_RANGE_BELLOW] = "range-",
    [FLAG_INVALID_POS] = "invalid+",
    [FLAG_INVALID_NEG] = "invalid-",
    [FLAG_INVALID] = "invalid"};

static void print_air_data(air_data_t *air_data)
{
    const char deg = (char)0xF8;

    switch (air_data->type)
    {
    case RS485_QC:
        printf("Qc   = %9.1f [Pa]  (%s)\n", air_data->value, flag_str[air_data->flag]);
        break;
    case RS485_PS:
        printf("Ps   = %9.1f [Pa]  (%s)\n", air_data->value, flag_str[air_data->flag]);
        break;
    case RS485_AOA:
        printf("AoA  = %9.3f [%c]   (%s)\n", air_data->value, deg, flag_str[air_data->flag]);
        break;
    case RS485_AOS:
        printf("AoA  = %9.1f [%c]   (%s)\n", air_data->value, deg, flag_str[air_data->flag]);
        break;
    case RS485_CAS:
        printf("CAS  = %9.2f [m/s] (%s)\n", air_data->value, flag_str[air_data->flag]);
        break;
    case RS485_TAS:
        printf("TAS  = %9.2f [m/s] (%s)\n", air_data->value, flag_str[air_data->flag]);
        break;
    case RS485_HP:
        printf("HP   = %9.1f [m]   (%s)\n", air_data->value, flag_str[air_data->flag]);
        break;
    case RS485_MACH:
        printf("Mach = %9.3f [-]   (%s)\n", air_data->value, flag_str[air_data->flag]);
        break;
    case RS485_SAT:
        printf("SAT  = %9.1f [%cC]  (%s)\n", air_data->value, deg, flag_str[air_data->flag]);
        break;
    case RS485_TAT:
        printf("TAT  = %9.1f [%cC]  (%s)\n", air_data->value, deg, flag_str[air_data->flag]);
        break;
    case RS485_QNH:
        printf("QNH  = %9.1f [Pa]  (%s)\n", air_data->value, flag_str[air_data->flag]);
        break;
    case RS485_CR:
        printf("CR   = %9.1f [m/s] (%s)\n", air_data->value, flag_str[air_data->flag]);
        break;
    case RS485_PT:
        printf("Pt   = %9.1f [Pa]  (%s)\n", air_data->value, flag_str[air_data->flag]);
        break;
    case RS485_HBARO:
        printf("HBARO= %9.1f [m]   (%s)\n", air_data->value, flag_str[air_data->flag]);
        break;
    case RS485_DTR:
        printf("DTR  = %9.2f [-] (%s)\n", air_data->value, flag_str[air_data->flag]);
        break;
    case RS485_HTR:
        printf("HTR  = %9.1f [%cC]  (%s)\n", air_data->value, deg, flag_str[air_data->flag]);
        break;
    case RS485_CUR:
        printf("CUR  = %9.2f [A] (%s)\n", air_data->value, flag_str[air_data->flag]);
        break;
    case RS485_IAT:
        printf("IAT  = %9.1f [%cC]  (%s)\n", air_data->value, deg, flag_str[air_data->flag]);
        break;
    case RS485_BAT:
        printf("BAT  = %9.1f [%cC]  (%s)\n", air_data->value, deg, flag_str[air_data->flag]);
        break;
    default:
        break;
    }
}

static void print_gen_status(adc_gen_status_t *gen_status)
{
    printf("General status = 0x%04X \n\n", gen_status->number);
}

static void print_htr_status(htr_status_t *htr_status)
{
    printf("Heater status = 0x%04X \n\n", htr_status->number);
}

void print_message(adc_rs485_msg_t *msg)
{
    switch (msg->msg_type)
    {
    case RS485_ERROR:
        printf("\nError decoding the message! \n\n");
        break;
    case RS485_RETURNED_DATA:
        print_air_data(&(msg->air_data));
        break;
    case RS485_RETURNED_STATUS_GEN:
        print_gen_status(&(msg->gen_status));
        break;
    case RS485_RETURNED_STATUS_HTR:
        print_htr_status(&(msg->htr_status));
        break;
    default:
        break;
    }
}