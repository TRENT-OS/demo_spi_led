/******************************************************************//**
* @file max7219.h
* Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sublicense,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included
* in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
* OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
* IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
* OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
* OTHER DEALINGS IN THE SOFTWARE.
*
* Except as contained in this notice, the name of Maxim Integrated
* Products, Inc. shall not be used except as stated in the Maxim Integrated
* Products, Inc. Branding Policy.
*
* The mere transfer of this software does not imply any licenses
* of trade secrets, proprietary technology, copyrights, patents,
* trademarks, maskwork rights, or any other form of intellectual
* property whatsoever. Maxim Integrated Products, Inc. retains all
* ownership rights.
**********************************************************************/


#ifndef MAX7219_H
#define MAX7219_H

#include <stdint.h>
#include <string.h>


#ifndef _SPILED_ERR_BASE
#define _SPILED_ERR_BASE            (-24000)
#endif

#define SPILED_OK                   (0)
#define SPILED_ERR_INTERNAL         (_SPILED_ERR_BASE - 1)
#define SPILED_ERR_BAD_STATE        (_SPILED_ERR_BASE - 2)
#define SPILED_ERR_HW_BUSY          (_SPILED_ERR_BASE - 3)
#define SPILED_ERR_BUSY             (_SPILED_ERR_BASE - 4)
#define SPILED_ERR_ERASE_UNALIGNED  (_SPILED_ERR_BASE - 5)
#define SPILED_ERR_BAD_CONFIG       (_SPILED_ERR_BASE - 6)

/**
* @brief Structure for device configuration
* @code
* #include "max7219.h"
*
* max7219_configuration_t cfg = {
*     .device_number = 1,
*     .decode_mode = 0,
*     .intensity = Max7219::MAX7219_INTENSITY_8,
*     .scan_limit = Max7219::MAX7219_SCAN_8
* };
* @endcode
*/
typedef struct spiled_config_s
{
    uint8_t device_number;
    uint8_t decode_mode;
    uint8_t intensity;
    uint8_t scan_limit;
} spiled_config_t;

/**
* @brief   Serially Interfaced, 8-Digit, LED Display Driver
*
* @details The MAX7219/MAX7221 are compact, serial input/output common-cathode
* display drivers that interface microprocessors (µPs) to 7-segment numeric
* LED displays of up to 8 digits, bar-graph displays, or 64 individual LEDs.
* Included on-chip are a BCD code-B decoder, multiplex scan circuitry, segment
* and digit drivers, and an 8x8 static RAM that stores each digit. Only one
* external resistor is required to set the segment current for all LEDs.
* The MAX7221 is compatible with SPI™, QSPI™, and MICROWIRE™, and has
* slew-rate-limited segment drivers to reduce EMI.
*
* @code
* #include "mbed.h"
* #include "max7219.h"
*
* Max7219 max7219(SPI0_MOSI, SPI0_MISO, SPI0_SCK, SPI0_SS);
*
* int main()
* {
*     max7219_configuration_t cfg = {
*         .device_number = 1,
*         .decode_mode = 0,
*         .intensity = Max7219::MAX7219_INTENSITY_8,
*         .scan_limit = Max7219::MAX7219_SCAN_8
*     };
*
*     max7219.init_device(cfg);
*     max7219.enable_device(1);
*     max7219.set_display_test();
*     wait(1);
*     max7219.clear_display_test();
*
*     while (1) {
*         max7219.write_digit(1, Max7219::MAX7219_DIGIT_0, ...
*     }
* }
* @endcode
*/
/**
 * @brief   Digit and Control Registers
 * @details The 14 addressable digit and control registers.
 */
typedef enum
{
    MAX7219_NO_OP = 0,
    MAX7219_DIGIT_0,
    MAX7219_DIGIT_1,
    MAX7219_DIGIT_2,
    MAX7219_DIGIT_3,
    MAX7219_DIGIT_4,
    MAX7219_DIGIT_5,
    MAX7219_DIGIT_6,
    MAX7219_DIGIT_7,
    MAX7219_DECODE_MODE,
    MAX7219_INTENSITY,
    MAX7219_SCAN_LIMIT,
    MAX7219_SHUTDOWN,
    MAX7219_DISPLAY_TEST = 15
}max7219_register_e;
    
/**
 * @brief   Intensity values
 * @details Digital control of display brightness is provided by an
 * internal pulse-width modulator, which is controlled by
 * the lower nibble of the intensity register.
 */
typedef enum
{
    MAX7219_INTENSITY_0 = 0,
    MAX7219_INTENSITY_1,
    MAX7219_INTENSITY_2,
    MAX7219_INTENSITY_3,
    MAX7219_INTENSITY_4,
    MAX7219_INTENSITY_5,
    MAX7219_INTENSITY_6,
    MAX7219_INTENSITY_7,
    MAX7219_INTENSITY_8,
    MAX7219_INTENSITY_9,
    MAX7219_INTENSITY_A,
    MAX7219_INTENSITY_B,
    MAX7219_INTENSITY_C,
    MAX7219_INTENSITY_D,
    MAX7219_INTENSITY_E,
    MAX7219_INTENSITY_F
}max7219_intensity_e;


/**
 * @brief   Scan limit for mutiplexing digits
 * @details The scan-limit register sets how many digits are 
 * displayed, from 1 to 8. They are displayed in a multiplexed
 * manner with a typical display scan rate of 800Hz with 8
 * digits displayed.
 */
typedef enum
{
    MAX7219_SCAN_1 = 0,
    MAX7219_SCAN_2,
    MAX7219_SCAN_3,
    MAX7219_SCAN_4,
    MAX7219_SCAN_5,
    MAX7219_SCAN_6,
    MAX7219_SCAN_7,
    MAX7219_SCAN_8
}max7219_scan_limit_e;

struct spiled_s;
/**
 * Defines the hardware abstraction layer for the spi flash driver.
 */
typedef struct spiled_hal_s
{
    /**
     * Carry out a spi transaction.
     * First, if tx_len > 0, tx_data is to be transmitted from tx_data.
     * Then, if rx_len > 0, rx_data is to be received into rx_data.
     * This function is called when the spi flash driver needs to communicate on
     * the SPI bus. When the transaction is finished, spiflash_async_cb is to be
     * called in asynchronous mode.
     * In synchronous mode, this must block.
     *
     * @param spi      pointer to the spi flash driver struct.
     * @param tx_data  data to be transmitted, ignored if tx_len == 0.
     * @param tx_len   length of data to be transmitted.
     * @param rx_data  data to be received, ignored if rx_len == 0.
     * @param rx_len   length of data to be received.
     * @return 0 if ok, anything else is considered an error.
     */
    int (*_spiled_spi_txrx)(struct spiled_s* spi, const uint8_t* tx_data,
                              uint32_t tx_len);

    /**
     * Assert/deassert chip select. If parameter cs is nonzero, CS pin should be
     * asserted (normally means the pin should be at gnd level, low). If
     * parameter cs is zero, CS pin should be deasserted (normally means the pin
     * should be at Vddio level, high).
     *
     * @param spi  pointer to the spi flash driver struct.
     * @param cs   !0 to assert CS, 0 to deassert CS.
     */
    void (*_spiled_spi_cs)(struct spiled_s* spi, uint8_t cs);

    /**
     * Wait given number of milliseconds.
     * When the timeout is reached, spiflash_async_trigger is to be called in
     * asynchronous mode.
     * In synchronous mode, this must block given number of milliseconds.
     *
     * @param spi  pointer to the spi flash driver struct.
     */
    void (*_spiled_wait)(struct spiled_s* spi, uint32_t us);
} spiled_hal_t;

/**
 * The spi flash driver struct.
 */
typedef struct spiled_s
{
    // physical spi flash config
    const spiled_config_t* cfg;
    // HAL config
    const spiled_hal_t* hal;
} spiled_t;

void Max7219_Init(
    spiled_t* spi,
    const spiled_config_t* cfg,
    const spiled_hal_t* hal
);

int32_t set_num_devices(
    uint8_t num_devices
);

void set_display_test(
    spiled_t* spi
);

void clear_display_test(
    spiled_t* spi
);

int32_t init_device(
    spiled_t* spi,
    uint8_t device_number
);

void init_display(
    spiled_t* spi
);

int32_t enable_device(
    spiled_t* spi,
    uint8_t device_number
);

int32_t disable_device(
    spiled_t* spi,
    uint8_t device_number
);

void enable_display(
    spiled_t* spi
);

void disable_display(
    spiled_t* spi
);

int32_t write_digit(
    spiled_t* spi, 
    uint8_t device_number,
    uint8_t digit, 
    uint8_t data
);

int32_t clear_digit(
    spiled_t* spi, 
    uint8_t device_number,
    uint8_t digit
);

int32_t device_all_on(
    spiled_t* spi,
    uint8_t device_number
);

int32_t device_all_off(
    spiled_t* spi,
    uint8_t device_number
);

void display_all_on(
    spiled_t* spi
);
void display_all_off(
    spiled_t* spi
);

#endif /* MAX7219_H*/