/*
 * RasPi SPI Flash storage driver
 *
 * Copyright (C) 2020, HENSOLDT Cyber GmbH
 */

#include "OS_Error.h"
#include "LibDebug/Debug.h"
#include "TimeServer.h"
#include "bcm2837_spi.h"
#include "max7219.h"

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <camkes.h>

static struct
{
    bool           init_ok;
    spiled_t       spi_led_ctx;
} ctx =
{
    .init_ok       = false,
};

const uint8_t TEXT[] = {
    0x00,0x66,0x66,0x7e,0x7e,0x66,0x66,0x00, //H
    0x00,0x7e,0x66,0x7e,0x60,0x7e,0x7e,0x00, //E
    0x00,0x62,0x72,0x5a,0x4a,0x4e,0x46,0x00, //N
    0x00,0x7e,0x60,0x7e,0x06,0x06,0x7e,0x00, //S
    0x00,0x7e,0x7e,0x66,0x66,0x7e,0x7e,0x00, //O
    0x00,0x60,0x60,0x60,0x60,0x7e,0x7e,0x00, //L
    0x00,0x70,0x7c,0x0e,0x0e,0x7c,0x70,0x00, //D
    0x00,0x7e,0x7e,0x18,0x18,0x18,0x18,0x00 //T
};

const uint8_t H[] = {
    0x00,0x66,0x66,0x7e,0x7e,0x66,0x66,0x00
};

const uint8_t E[] = {
    0x00,0x7e,0x66,0x7e,0x60,0x7e,0x7e,0x00
};

const uint8_t N[] = {
    0x00,0x62,0x72,0x5a,0x4a,0x4e,0x46,0x00
};

const uint8_t S[] = {
    0x00,0x7e,0x60,0x7e,0x06,0x06,0x7e,0x00
};

const uint8_t O[] = {
    0x00,0x7e,0x7e,0x66,0x66,0x7e,0x7e,0x00
};

const uint8_t L[] = {
    0x00,0x60,0x60,0x60,0x60,0x7e,0x7e,0x00
};

const uint8_t D[] = {
    0x00,0x70,0x7c,0x0e,0x0e,0x7c,0x70,0x00
};

const uint8_t T[] = {
    0x00,0x7e,0x7e,0x18,0x18,0x18,0x18,0x00
};

const uint8_t ZERO[] = {
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
//------------------------------------------------------------------------------
// callback from SPI library
static
__attribute__((__nonnull__))
int
impl_spiled_spi_txrx(
    spiled_t* spi,
    const uint8_t* tx_data,
    uint32_t tx_len)
{
    bcm2837_spi_writenb(
        (void*)tx_data,
        tx_len);

    return SPILED_OK;
}


//------------------------------------------------------------------------------
// callback from SPI library
static
__attribute__((__nonnull__))
void
impl_spiled_spi_cs(
    spiled_t* spi,
    uint8_t cs)
{
    bcm2837_spi_chipSelect(cs ? BCM2837_SPI_CS0 : BCM2837_SPI_CS2);
    return;
}


//------------------------------------------------------------------------------
// callback from SPI library
static
__attribute__((__nonnull__))
void
impl_spiled_wait(
    spiled_t* spi,
    uint32_t us)
{
    // TimeServer.h provides this helper function, it contains the hard-coded
    // assumption that the RPC interface is "timeServer_rpc"
    TimeServer_sleep(TimeServer_PRECISION_USEC, us);
}

//------------------------------------------------------------------------------
void post_init(void)
{
    Debug_LOG_INFO("BCM2837_SPI_LED init");

    // initialize BCM2837 SPI library
    if (!bcm2837_spi_begin(regBase))
    {
        Debug_LOG_ERROR("bcm2837_spi_begin() failed");
        return;
    }

    bcm2837_spi_setBitOrder(BCM2837_SPI_BIT_ORDER_MSBFIRST);
    bcm2837_spi_setDataMode(BCM2837_SPI_MODE0);
    // divider 8 gives 50 MHz assuming the RasPi3 is running with the default
    // 400 MHz, but for some reason we force it to run at just 250 MHz with
    // "core_freq=250" in config.txt and thus end up at 31.25 MHz SPI speed.
    bcm2837_spi_setClockDivider(BCM2837_SPI_CLOCK_DIVIDER_256);
    bcm2837_spi_chipSelect(BCM2837_SPI_CS0);
    bcm2837_spi_setChipSelectPolarity(BCM2837_SPI_CS0, 0);

    // initialize MAX7219 SPI library
    static const spiled_config_t spiled_config =
    {
        .device_number = 4,
        .decode_mode = 0,
        .intensity = MAX7219_INTENSITY_8,
        .scan_limit = MAX7219_SCAN_8 
    };

    static const spiled_hal_t hal =
    {
        ._spiled_spi_txrx  = impl_spiled_spi_txrx,
        ._spiled_spi_cs    = impl_spiled_spi_cs,
        ._spiled_wait      = impl_spiled_wait,
    };

    Max7219_Init(&ctx.spi_led_ctx, &spiled_config, &hal);

    if ( (NULL == ctx.spi_led_ctx.cfg) || (NULL == ctx.spi_led_ctx.hal) )
    {
        Debug_LOG_ERROR("MAX7219_Init() failed");
        return;
    }

    ctx.init_ok = true;

    Debug_LOG_INFO("BCM2837_SPI_LED done");
}


//------------------------------------------------------------------------------
// This is a CAmkES RPC interface handler. It's guaranteed that "written"
// never points to NULL.
// With these functions, it is possible for the main program to talk to the LED matrix
/**
 * @brief Displays a character on the LED matrix.
 * @return Implementation specific. 
 */
OS_Error_t 
__attribute__((__nonnull__))
led_rpc_displayChar(
    unsigned char character
)
{
    Debug_LOG_DEBUG("SPI displayChar");

    if (!ctx.init_ok)
    {
        Debug_LOG_ERROR("initialization failed, fail call %s()", __func__);
        return OS_ERROR_INVALID_STATE;
    }

    int32_t ret = 0;
    for (size_t i = 0; i < (sizeof(TEXT) / sizeof(TEXT[0])) / 8; i++)
    {
        for (uint8_t j = 0; j < MAX7219_DIGIT_7; j++)
        {
            ret |= write_digit(&(ctx.spi_led_ctx),1,(j+1),TEXT[i * 8 + j]);
        }
        ctx.spi_led_ctx.hal->_spiled_wait(&(ctx.spi_led_ctx),1000000);
    }
    for (size_t i = 0; i < (sizeof(TEXT) / sizeof(TEXT[0])) / 8; i++)
    {
        for (uint8_t j = 0; j < MAX7219_DIGIT_7; j++)
        {
            ret |= write_digit(&(ctx.spi_led_ctx),2,(j+1),TEXT[i * 8 + j]);
        }
        ctx.spi_led_ctx.hal->_spiled_wait(&(ctx.spi_led_ctx),1000000);
    }
    for (size_t i = 0; i < (sizeof(TEXT) / sizeof(TEXT[0])) / 8; i++)
    {
        for (uint8_t j = 0; j < MAX7219_DIGIT_7; j++)
        {
            ret |= write_digit(&(ctx.spi_led_ctx),3,(j+1),TEXT[i * 8 + j]);
        }
        ctx.spi_led_ctx.hal->_spiled_wait(&(ctx.spi_led_ctx),1000000);
    }
    for (size_t i = 0; i < (sizeof(TEXT) / sizeof(TEXT[0])) / 8; i++)
    {
        for (uint8_t j = 0; j < MAX7219_DIGIT_7; j++)
        {
            ret |= write_digit(&(ctx.spi_led_ctx),4,(j+1),TEXT[i * 8 + j]);
        }
        ctx.spi_led_ctx.hal->_spiled_wait(&(ctx.spi_led_ctx),1000000);
    }

    if (ret != 0)
    {
        Debug_LOG_ERROR("SPILED_displayChar() failed.");
        return OS_ERROR_GENERIC;
    }

    return OS_SUCCESS;
}

/**
 * @brief Clears the LED matrix.
 * @return Implementation specific. 
 */
OS_Error_t
__attribute__((__nonnull__))
led_rpc_clearLEDMatrix(void)
{
    Debug_LOG_DEBUG("SPI clearLEDMatrix");

    if (!ctx.init_ok)
    {
        Debug_LOG_ERROR("initialization failed, fail call %s()", __func__);
        return OS_ERROR_INVALID_STATE;
    }

    int32_t ret = 0;
    for (uint8_t i = 0; i < MAX7219_DIGIT_7; i++)
    {
        ret |= clear_digit(&(ctx.spi_led_ctx),1,(i+1));
    }
    
    if (ret != 0)
    {
        Debug_LOG_ERROR("SPILED_clearLEDMatrix() failed.");
        return OS_ERROR_GENERIC;
    }
    
    return OS_SUCCESS;
}

/**
 * @brief Scrolls text one column to the left.
 * @return Implementation specific. 
 */
OS_Error_t
__attribute__((__nonnull__))
led_rpc_scrollText(void)
{
    // TODO: still needs to be implemented
    return OS_SUCCESS;
}