/*
 * RasPi SPI LED driver
 *
 * Copyright (C) 2020, HENSOLDT Cyber GmbH
 */

#include "OS_Error.h"
#include "LibDebug/Debug.h"
#include "TimeServer.h"
#include "bcm2837_spi.h"
#include "max7219.h"
#include "font.h"

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

/**
 * @brief Transfers data to the bcm2837 specific driver layers.
 * 
 * @details Is called by the max7219 driver files and builds the interface to the lower level driver files.
 * 
 * @param spi      pointer to the spi led driver struct.
 * @param tx_data  data to be transmitted, ignored if tx_len == 0.
 * @param tx_len   length of data to be transmitted.
 */
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


/**
 * @brief Selects the CS (chip select) output line.
 * 
 * @details Is called by the max7219 driver files and builds the interface to the lower level driver files.
 * 
 * @param spi      pointer to the spi led driver struct.
 * @param cs       !0 to assert CS, 0 to deassert CS.
 */
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


/**
 * @brief Wait given number of microseconds.
 * 
 * @details Is called by the max7219 driver files and builds the interface to the lower level driver files.
 * 
 * @param spi      pointer to the spi led driver struct.
 * @param us       number of microseconds to wait.
 */
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

/**
 * @brief Post init function.
 * 
 * @details Gets called before the actual program logic starts. 
 * Is used for any initialization that should be done before the program starts.
 */
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


/**
 * @brief Displays specific character on specific device.
 * 
 * @details CAmkES RPC interface handler. The main program (with the run function) can call
 *          this function.
 * 
 * @param character     character that should be displayed
 * @param device        device to write to
 * 
 * @return  Implementation specific.
 */ 
OS_Error_t 
__attribute__((__nonnull__))
led_rpc_display_char_on_device(
    unsigned char character,
    uint8_t device
) 
{
    Debug_LOG_DEBUG("SPI displayChar");

    if (!ctx.init_ok)
    {
        Debug_LOG_ERROR("initialization failed, fail call %s()", __func__);
        return OS_ERROR_INVALID_STATE;
    }
    
    int32_t ret = 0;
    const uint8_t * bytes = font8x8[(uint8_t)character];
    for (uint8_t j = 0; j < MAX7219_DIGIT_7; j++)
    {
        ret |= write_digit(&(ctx.spi_led_ctx),device,(j+1),bytes[j]);
    }
    //ctx.spi_led_ctx.hal->_spiled_wait(&(ctx.spi_led_ctx),1000000);

    if (ret != 0)
    {
        Debug_LOG_ERROR("SPILED_display_char_on_device() failed.");
        return OS_ERROR_GENERIC;
    }

    return OS_SUCCESS;
}

/**
 * @brief Displays specific character on all devices.
 * 
 * @details CAmkES RPC interface handler. The main program (with the run function) can call
 *          this function.
 * 
 * @param character     character that should be displayed
 * 
 * @return Implementation specific.
 */ 
OS_Error_t 
__attribute__((__nonnull__))
led_rpc_display_char(
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
    const uint8_t * bytes = font8x8[(uint8_t)character];
    for (size_t device = 0; device < ctx.spi_led_ctx.cfg->device_number; device++)
    {
        for (uint8_t j = 0; j < MAX7219_DIGIT_7; j++)
        {
            ret |= write_digit(&(ctx.spi_led_ctx),(device + 1),(j+1),bytes[j]);
        }
    }
    //ctx.spi_led_ctx.hal->_spiled_wait(&(ctx.spi_led_ctx),1000000);
    
    if (ret != 0)
    {
        Debug_LOG_ERROR("SPILED_displayChar() failed.");
        return OS_ERROR_GENERIC;
    }

    return OS_SUCCESS;
}

/**
 * @brief Clears entire display.
 * 
 * @details CAmkES RPC interface handler. The main program (with the run function) can call
 *          this function.
 * 
 * @return Implementation specific.
 */ 
OS_Error_t
__attribute__((__nonnull__))
led_rpc_clear_display(void)
{
    Debug_LOG_DEBUG("SPI clearLEDMatrix");

    if (!ctx.init_ok)
    {
        Debug_LOG_ERROR("initialization failed, fail call %s()", __func__);
        return OS_ERROR_INVALID_STATE;
    }

    display_all_off(&(ctx.spi_led_ctx));
    //ctx.spi_led_ctx.hal->_spiled_wait(&(ctx.spi_led_ctx),1000000);

    return OS_SUCCESS;
}

/**
 * @brief Scrolls text to the left.
 * 
 * @details CAmkES RPC interface handler. The main program (with the run function) can call
 *          this function.
 * 
 * @param text      text that should be scrolled over the display
 * 
 * @return Implementation specific.
 */ 
OS_Error_t
__attribute__((__nonnull__))
led_rpc_scroll_text(
    const char * text
)
{
    Debug_LOG_DEBUG("SPI displayChar");

    if (!ctx.init_ok)
    {
        Debug_LOG_ERROR("initialization failed, fail call %s()", __func__);
        return OS_ERROR_INVALID_STATE;
    }

    int32_t ret = 0;

    /*
     * buf: Data structure that stores the current bit pattern of the different LEDs on the 8x8 LED matrix.
     *      Data is shifted for each digit to the left.
     * 
     * Structure:
     *      digit0 - device4 | digit0 - device3 | digit0 - device2 | digit0 - device1 
     *      digit1 - device4 | ...
     */
    uint8_t num_devices = ctx.spi_led_ctx.cfg->device_number; 
    uint8_t buf[num_devices * (MAX7219_DIGIT_7 + 1)];
    memset(buf,0,num_devices * (MAX7219_DIGIT_7 + 1) * sizeof(uint8_t));

    for (size_t letter = 0; letter < strlen(text); letter++) //go through all the letters
    {
        const uint8_t * bytes = font8x8[(uint8_t)text[letter]];
        for (int8_t shift = 7; shift >= 0; shift--) //shift each letter into the buf
        {
            for (size_t digit = 0; digit < MAX7219_DIGIT_7; digit++) //make this for every digit
            {
                for (size_t dev_loc = 0; dev_loc < (num_devices - 1); dev_loc++) //and for every device
                {
                    buf[(digit * num_devices) + dev_loc] = (buf[(digit * num_devices) + dev_loc] << 1) //shift data one-bit to the left...
                                                          | (((buf[(digit * num_devices) + dev_loc + 1]) & (1 << 7)) > 0); //... and fill with msb bit of previous device
                }
                buf[(digit * num_devices) + num_devices - 1] = (buf[(digit * num_devices) + num_devices - 1] << 1) //shift data one-bit to the left ...
                                                               | (((bytes[digit]) & (1 << shift)) > 0); //... and fill with according bit of current character
                //print current content of buf
                for (size_t device = 1; device <= num_devices; device++)
                {
                    ret |= write_digit(&(ctx.spi_led_ctx),device,(digit + 1),buf[(digit * num_devices) + (num_devices - device)]);
                }
            }
            ctx.spi_led_ctx.hal->_spiled_wait(&(ctx.spi_led_ctx),100000);
        }
    }
    //push all data out of display
    for (size_t i = 0; i < num_devices; i++)
    {
        for (int8_t shift = 7; shift >= 0; shift--) //shift each letter into the buf
        {
            for (size_t digit = 0; digit < MAX7219_DIGIT_7; digit++) //make this for every digit
            {
                for (size_t dev_loc = 0; dev_loc < (num_devices - 1); dev_loc++) //and for every device
                {
                    buf[(digit * num_devices) + dev_loc] = (buf[(digit * num_devices) + dev_loc] << 1)
                                                          | (((buf[(digit * num_devices) + dev_loc + 1]) & (1 << 7)) > 0);
                }
                buf[(digit * num_devices) + num_devices - 1] = (buf[(digit * num_devices) + num_devices - 1] << 1); //let all the data just be pushed out
                //print current content of buf
                for (size_t device = 1; device <= num_devices; device++)
                {
                    ret |= write_digit(&(ctx.spi_led_ctx),device,(digit + 1),buf[(digit * num_devices) + (num_devices - device)]);
                }
            }
            ctx.spi_led_ctx.hal->_spiled_wait(&(ctx.spi_led_ctx),100000);
        }
    }

    return OS_SUCCESS;
}