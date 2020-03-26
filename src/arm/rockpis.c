/*
 * Author: zzl <zzl@radxa.com>
 * Copyright (c) 2020 Vamrs Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <mraa/common.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include "arm/rockpis.h"
#include "common.h"

#define DT_BASE "/proc/device-tree"

#define PLATFORM_NAME_ROCK_PI_S "Radxa ROCK Pi S"

#define MAX_SIZE 64

const char* rockpis_serialdev[MRAA_ROCKPIS_UART_COUNT] = { "/dev/ttyS0","/dev/ttyS1","/dev/ttyS2"};

void
mraa_rockpis_pininfo(mraa_board_t* board, int index, int sysfs_pin, mraa_pincapabilities_t pincapabilities_t, char* fmt, ...)
{
    va_list arg_ptr;
    if (index > board->phy_pin_count)
        return;

    mraa_pininfo_t* pininfo = &board->pins[index];
    va_start(arg_ptr, fmt);
    vsnprintf(pininfo->name, MRAA_PIN_NAME_SIZE, fmt, arg_ptr);

    if( pincapabilities_t.gpio == 1 ) {
	   va_arg(arg_ptr, int);
   	pininfo->gpio.gpio_chip = va_arg(arg_ptr, int);
	   pininfo->gpio.gpio_line = va_arg(arg_ptr, int);
    }

    pininfo->capabilities = pincapabilities_t;

    va_end(arg_ptr);
    pininfo->gpio.pinmap = sysfs_pin;
    pininfo->gpio.mux_total = 0;
}

mraa_board_t*
mraa_rockpis()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b== NULL) {
        return NULL;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b);
        return NULL;
    }

    // pin mux for buses are setup by default by kernel so tell mraa to ignore them
    b->no_bus_mux = 1;
    b->phy_pin_count = MRAA_ROCKPIS_PIN_COUNT + 1;

    if (mraa_file_exist(DT_BASE "/model")) {
        // We are on a modern kernel, great!!!!
        if (mraa_file_contains(DT_BASE "/model", PLATFORM_NAME_ROCK_PI_S)  ) {
            b->platform_name = PLATFORM_NAME_ROCK_PI_S;
            b->uart_dev[0].device_path = (char*) rockpis_serialdev[0];
            b->uart_dev[1].device_path = (char*) rockpis_serialdev[1];
        }
    }

    // UART
    b->uart_dev_count = MRAA_ROCKPIS_UART_COUNT;
    b->def_uart_dev = 0;
    b->uart_dev[0].index = 0;
    b->uart_dev[1].index = 1;
    b->uart_dev[2].index = 2;

    // I2C
    if (strncmp(b->platform_name, PLATFORM_NAME_ROCK_PI_S, MAX_SIZE) == 0) {
        b->i2c_bus_count = MRAA_ROCKPIS_I2C_COUNT ;
        b->def_i2c_bus = 0;
        b->i2c_bus[0].bus_id = 0;
        b->i2c_bus[1].bus_id = 1;
        b->i2c_bus[3].bus_id = 3;
       
    }

    // SPI
    b->spi_bus_count = MRAA_ROCKPIS_SPI_COUNT;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 32766;
    b->spi_bus[2].bus_id = 32765;
    

    //PWM
    b->pwm_dev_count = MRAA_ROCKPIS_PWM_COUNT;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    b->pins[11].pwm.parent_id = 2;
    b->pins[11].pwm.mux_total = 0;
    b->pins[11].pwm.pinmap = 0;
    b->pins[13].pwm.parent_id = 3;
    b->pins[13].pwm.mux_total = 0;
    b->pins[13].pwm.pinmap = 0;

    b->aio_count = MRAA_ROCKPIS_AIO_COUNT;
    b->adc_raw = 10;
    b->adc_supported = 10;
    b->aio_dev[0].pin = 26;
    b->aio_non_seq = 1;

    mraa_rockpis_pininfo(b, 0,   -1, (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, "INVALID");
    mraa_rockpis_pininfo(b, 1,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_rockpis_pininfo(b, 2,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_rockpis_pininfo(b, 3,   11, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "I2C_SDA");
    mraa_rockpis_pininfo(b, 4,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_rockpis_pininfo(b, 5,   15, (mraa_pincapabilities_t){1,1,0,0,0,1,0,0}, "I2C_SCL");
    mraa_rockpis_pininfo(b, 6,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpis_pininfo(b, 7,   68, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "I2S0_8CH_MCLK,PDM_CLK_M_M2");
    mraa_rockpis_pininfo(b, 8,  65, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "UART0_TX");
    mraa_rockpis_pininfo(b, 9,   -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpis_pininfo(b, 10, 64, (mraa_pincapabilities_t){1,1,0,0,0,0,0,1}, "UART0_RX");
    mraa_rockpis_pininfo(b, 11, 15, (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "PWM2,I2C3_SDA_M0");
    mraa_rockpis_pininfo(b, 12, 69, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "I2S0_8CH_SCLK_TX");
    mraa_rockpis_pininfo(b, 13, 16, (mraa_pincapabilities_t){1,1,1,0,0,1,0,0}, "PWM3,I2C3_SCL_M0");
    mraa_rockpis_pininfo(b, 14,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpis_pininfo(b, 15, 17, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "SPDIF_TX");
    mraa_rockpis_pininfo(b, 16, 74, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "I2S0_8CH_SDO1");
    mraa_rockpis_pininfo(b, 17,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_rockpis_pininfo(b, 18, 73, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "I2S0_8CH_SDO0");
    mraa_rockpis_pininfo(b, 19,  55, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "UART1_RTSN,UART2_TX_M0,SPI2_MOSI");
    mraa_rockpis_pininfo(b, 20,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpis_pininfo(b, 21,  54, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "UART1_CTSN,UART2_RX_M0,SPI2_MISO");
    mraa_rockpis_pininfo(b, 22, 14, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "I2S0_8CH_LRCK_TX");
    mraa_rockpis_pininfo(b, 23,  56, (mraa_pincapabilities_t){1,1,0,0,1,1,0,1}, "UART1_RX,I2C0_SDA,SPI2_CLK");
    mraa_rockpis_pininfo(b, 24,  57, (mraa_pincapabilities_t){1,1,0,0,1,1,0,1}, "UART1_TX,I2C0_SCL,SPI2_CSN0");
    mraa_rockpis_pininfo(b, 25,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_rockpis_pininfo(b, 26,  -1, (mraa_pincapabilities_t){1,0,0,0,0,0,1,0}, "ADC_IN0");
   

    return b;
}