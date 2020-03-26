/*
 * Author: zzl <zzl@radxa.com>
 * Copyright (c) Radxa Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_ROCKPIS_GPIO_COUNT   17
#define MRAA_ROCKPIS_I2C_COUNT        3
#define MRAA_ROCKPIS_SPI_COUNT        1
#define MRAA_ROCKPIS_UART_COUNT    3
#define MRAA_ROCKPIS_PWM_COUNT     2
#define MRAA_ROCKPIS_AIO_COUNT        1
#define MRAA_ROCKPIS_PIN_COUNT      26

mraa_board_t *
        mraa_rockpis();

#ifdef __cplusplus
}
#endif