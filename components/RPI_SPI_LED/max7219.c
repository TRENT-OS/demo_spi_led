#include "max7219.h"                                  

void Max7219_Init(spiled_t* spi,
                    const spiled_config_t* cfg,
                    const spiled_hal_t* hal)
{
    memset(spi, 0, sizeof(spiled_t));
    spi->cfg = cfg;
    spi->hal = hal;

    init_display(spi);
    enable_display(spi);

    //the display needs this initial test, otherwise we can run in issues if internal memory is set to 1 when booting up
    //not quite sure why this is 
    set_display_test(spi);
    spi->hal->_spiled_wait(spi,100);
    clear_display_test(spi);
    spi->hal->_spiled_wait(spi,100);
    display_all_off(spi);
    spi->hal->_spiled_wait(spi,100);
}

void set_display_test(spiled_t* spi)
{
    uint8_t idx = 0;
    
    int32_t num_bytes = 2 * spi->cfg->num_devices;
    uint8_t d[num_bytes];
    memset(d,0,num_bytes); 
    for(idx = spi->cfg->num_devices; idx > 0; idx--)
    {
        d[(spi->cfg->num_devices - idx) * 2] = MAX7219_DISPLAY_TEST;
        d[(spi->cfg->num_devices - idx) * 2 + 1] = 1;
    }
    spi->hal->_spiled_spi_cs(spi,1);
    spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,0);
}

void clear_display_test(spiled_t* spi)
{
    uint8_t idx = 0;
    
    int32_t num_bytes = 2 * spi->cfg->num_devices;
    uint8_t d[num_bytes];
    memset(d,0,num_bytes); 
    for(idx = spi->cfg->num_devices; idx > 0; idx--)
    {
        d[(spi->cfg->num_devices - idx) * 2] = MAX7219_DISPLAY_TEST;
        d[(spi->cfg->num_devices - idx) * 2 + 1] = 0;
    }
    spi->hal->_spiled_spi_cs(spi,1);
    spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,0);
}

int32_t init_device(spiled_t* spi, uint8_t device_number)
{
    int32_t rtn_val = -1;
    uint8_t idx = 0;
    
    if(device_number > spi->cfg->num_devices)
    {
        rtn_val = -1;
    }
    else if(device_number == 0)
    {
        //device numbering starts with index 1
        rtn_val = -2;
    }
    else
    {
        //write DECODE_MODE register of device
        int32_t num_bytes = 2 * spi->cfg->num_devices;
        uint8_t d[num_bytes];
        memset(d,0,num_bytes); 
        for(idx = spi->cfg->num_devices; idx > 0; idx--)
        {
            if(idx == device_number)
            {
                d[(spi->cfg->num_devices - idx) * 2] = MAX7219_DECODE_MODE;
                d[(spi->cfg->num_devices - idx) * 2 + 1] = spi->cfg->decode_mode;
            }
            else
            {
                d[(spi->cfg->num_devices - idx) * 2] = MAX7219_NO_OP;
                d[(spi->cfg->num_devices - idx) * 2 + 1] = 0;
            }
        }
        spi->hal->_spiled_spi_cs(spi,1);
        spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
        spi->hal->_spiled_spi_cs(spi,0);
        
        spi->hal->_spiled_wait(spi,1);
        
        //write INTENSITY register of device
        for(idx = spi->cfg->num_devices; idx > 0; idx--)
        {
            if(idx == device_number)
            {
                d[(spi->cfg->num_devices - idx) * 2] = MAX7219_INTENSITY;
                d[(spi->cfg->num_devices - idx) * 2 + 1] = spi->cfg->intensity;
            }
            else
            {
                d[(spi->cfg->num_devices - idx) * 2] = MAX7219_NO_OP;
                d[(spi->cfg->num_devices - idx) * 2 + 1] = 0;
            }
        }
        spi->hal->_spiled_spi_cs(spi,1);
        spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
        spi->hal->_spiled_spi_cs(spi,0);
        
        spi->hal->_spiled_wait(spi,1);
        
        //write SCAN_LIMT register of device
        for(idx = spi->cfg->num_devices; idx > 0; idx--)
        {
            if(idx == device_number)
            {
                d[(spi->cfg->num_devices - idx) * 2] = MAX7219_SCAN_LIMIT;
                d[(spi->cfg->num_devices - idx) * 2 + 1] = spi->cfg->scan_limit;
            }
            else
            {
                d[(spi->cfg->num_devices - idx) * 2] = MAX7219_NO_OP;
                d[(spi->cfg->num_devices - idx) * 2 + 1] = 0;
            }
        }
        spi->hal->_spiled_spi_cs(spi,1);
        spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
        spi->hal->_spiled_spi_cs(spi,0);
        
        spi->hal->_spiled_wait(spi,1);

        rtn_val = 0;
    }
    
    return(rtn_val);
}


void init_display(spiled_t* spi)
{
    uint8_t idx = 0;
    
    //write DECODE_MODE register of all devices
    int32_t num_bytes = 2 * spi->cfg->num_devices;
    uint8_t d[num_bytes];
    memset(d,0,num_bytes); 
    for(idx = spi->cfg->num_devices; idx > 0; idx--)
    {
        d[(spi->cfg->num_devices - idx) * 2] = MAX7219_DECODE_MODE;
        d[(spi->cfg->num_devices - idx) * 2 + 1] = spi->cfg->decode_mode;
    }
    spi->hal->_spiled_spi_cs(spi,1);
    spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,0);
    
    spi->hal->_spiled_wait(spi,1);
    
    //write INTENSITY register of all devices
    for(idx = spi->cfg->num_devices; idx > 0; idx--)
    {
        d[(spi->cfg->num_devices - idx) * 2] = MAX7219_INTENSITY;
        d[(spi->cfg->num_devices - idx) * 2 + 1] = spi->cfg->intensity;
    }
    spi->hal->_spiled_spi_cs(spi,1);
    spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,0);
    
    spi->hal->_spiled_wait(spi,(uint32_t) 1);
    
    //write SCAN_LIMT register of all devices
    for(idx = spi->cfg->num_devices; idx > 0; idx--)
    {
        d[(spi->cfg->num_devices - idx) * 2] = MAX7219_SCAN_LIMIT;
        d[(spi->cfg->num_devices - idx) * 2 + 1] = spi->cfg->scan_limit;
    }
    spi->hal->_spiled_spi_cs(spi,1);
    spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,0);
    
    spi->hal->_spiled_wait(spi,1);
}

int32_t enable_device(spiled_t* spi, uint8_t device_number)
{
    int32_t rtn_val = -1;
    uint8_t idx = 0;
    
    if(device_number > spi->cfg->num_devices)
    {
        rtn_val = -1;
    }
    else if(device_number == 0)
    {
        //device numbering starts with index 1
        rtn_val = -2;
    }
    else
    {
        int32_t num_bytes = 2 * spi->cfg->num_devices;
        uint8_t d[num_bytes];
        memset(d,0,num_bytes); 
        for(idx = spi->cfg->num_devices; idx > 0; idx--)
        {
            if(idx == device_number)
            {
                d[(spi->cfg->num_devices - idx) * 2] = MAX7219_SHUTDOWN;
                d[(spi->cfg->num_devices - idx) * 2 + 1] = 1;
            }
            else
            {
                d[(spi->cfg->num_devices - idx) * 2] = MAX7219_NO_OP;
                d[(spi->cfg->num_devices - idx) * 2 + 1] = 0;
            }
        }
        spi->hal->_spiled_spi_cs(spi,1);
        spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
        spi->hal->_spiled_spi_cs(spi,0);

        rtn_val = 0;
    }
    
    return(rtn_val);
}


int32_t disable_device(spiled_t* spi, uint8_t device_number)
{
    int32_t rtn_val = -1;
    uint8_t idx = 0;
    
    if(device_number > spi->cfg->num_devices)
    {
        rtn_val = -1;
    }
    else if(device_number == 0)
    {
        //device numbering starts with index 1
        rtn_val = -2;
    }
    else
    {
        int32_t num_bytes = 2 * spi->cfg->num_devices;
        uint8_t d[num_bytes];
        memset(d,0,num_bytes); 
        for(idx = spi->cfg->num_devices; idx > 0; idx--)
        {
            if(idx == device_number)
            {
                d[(spi->cfg->num_devices - idx) * 2] = MAX7219_SHUTDOWN;
                d[(spi->cfg->num_devices - idx) * 2 + 1] = 0;
            }
            else
            {
                d[(spi->cfg->num_devices - idx) * 2] = MAX7219_NO_OP;
                d[(spi->cfg->num_devices - idx) * 2 + 1] = 0;
            }
        }
        spi->hal->_spiled_spi_cs(spi,1);
        spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
        spi->hal->_spiled_spi_cs(spi,0);
        
        rtn_val = 0;
    }
    
    return(rtn_val);
}


void enable_display(spiled_t* spi)
{
    uint8_t idx = 0;
    
    int32_t num_bytes = 2 * spi->cfg->num_devices;
    uint8_t d[num_bytes];
    memset(d,0,num_bytes); 
    for(idx = spi->cfg->num_devices; idx > 0; idx--)
    {
        d[(spi->cfg->num_devices - idx) * 2] = MAX7219_SHUTDOWN;
        d[(spi->cfg->num_devices - idx) * 2 + 1] = 1;
    }
    spi->hal->_spiled_spi_cs(spi,1);
    spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,0);
}
    

void disable_display(spiled_t* spi)
{
    uint8_t idx = 0;
    
    int32_t num_bytes = 2 * spi->cfg->num_devices;
    uint8_t d[num_bytes];
    memset(d,0,num_bytes); 
    for(idx = spi->cfg->num_devices; idx > 0; idx--)
    {
        d[(spi->cfg->num_devices - idx) * 2] = MAX7219_SHUTDOWN;
        d[(spi->cfg->num_devices - idx) * 2 + 1] = 0;
    }
    spi->hal->_spiled_spi_cs(spi,1);
    spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,0);
}

int32_t write_digit(spiled_t* spi, uint8_t device_number, uint8_t digit, uint8_t data)
{
    int32_t rtn_val = -1;
    uint8_t idx = 0;
    
    if(digit > MAX7219_DIGIT_7)
    {
        rtn_val = -3;
    }
    else if(digit < MAX7219_DIGIT_0)
    {
        rtn_val = -4;
    }
    else
    {
        if(device_number > spi->cfg->num_devices)
        {
            rtn_val = -1;
        }
        else if(device_number == 0)
        {
            rtn_val = -2;
        }
        else
        {
            int32_t num_bytes = 2 * spi->cfg->num_devices;
            uint8_t d[num_bytes];
            memset(d,0,num_bytes); 
            for(idx = spi->cfg->num_devices; idx > 0; idx--)
            {
                if(idx == device_number)
                {
                    d[(spi->cfg->num_devices - idx) * 2] = digit;
                    d[(spi->cfg->num_devices - idx) * 2 + 1] = data;
                }
                else
                {
                    d[(spi->cfg->num_devices - idx) * 2] = MAX7219_NO_OP;
                    d[(spi->cfg->num_devices - idx) * 2 + 1] = 0;
                }
            }
            spi->hal->_spiled_spi_cs(spi,1);
            spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
            spi->hal->_spiled_spi_cs(spi,0);
            
            rtn_val = 0;
        }
    }
    
    return(rtn_val);
}
    

int32_t clear_digit(spiled_t* spi, uint8_t device_number, uint8_t digit)
{
    int32_t rtn_val = -1;
    uint8_t idx = 0;
    
    if(digit > MAX7219_DIGIT_7)
    {
        rtn_val = -3;
    }
    else if(digit < MAX7219_DIGIT_0)
    {
        rtn_val = -4;
    }
    else
    {
        if(device_number > spi->cfg->num_devices)
        {
            rtn_val = -1;
        }
        else if(device_number == 0)
        {
            rtn_val = -2;
        }
        else
        {
            int32_t num_bytes = 2 * spi->cfg->num_devices;
            uint8_t d[num_bytes];
            memset(d,0,num_bytes); 
            for(idx = spi->cfg->num_devices; idx > 0; idx--)
            {
                if(idx == device_number)
                {
                    d[(spi->cfg->num_devices - idx) * 2] = digit;
                    d[(spi->cfg->num_devices - idx) * 2 + 1] = 0;
                }
                else
                {
                    d[(spi->cfg->num_devices - idx) * 2] = MAX7219_NO_OP;
                    d[(spi->cfg->num_devices - idx) * 2 + 1] = 0;
                }
            }
            spi->hal->_spiled_spi_cs(spi,1);
            spi->hal->_spiled_spi_writenb(spi,d,num_bytes);
            spi->hal->_spiled_spi_cs(spi,0);
            
            rtn_val = 0;
        }
    }
    
    return(rtn_val);
}


int32_t device_all_on(spiled_t* spi, uint8_t device_number)
{
    int32_t rtn_val = -1;
    uint8_t idx = 0;
    
    if(device_number > spi->cfg->num_devices)
    {
        rtn_val = -1;
    }
    else if(device_number == 0)
    {
        rtn_val = -2;
    }
    else
    {
        rtn_val = 0;
        
        //writes every digit of given device to 0xFF
        for(idx = 0; idx < 8; idx++)
        {
            if(rtn_val == 0)
            {
                rtn_val = write_digit(spi, device_number, (idx + 1), 0xFF);
            }
        }
    }
    
    return(rtn_val);
}


int32_t device_all_off(spiled_t* spi, uint8_t device_number)
{
    int32_t rtn_val = -1;
    uint8_t idx = 0;
    
    if(device_number > spi->cfg->num_devices)
    {
        rtn_val = -1;
    }
    else if(device_number == 0)
    {
        rtn_val = -2;
    }
    else
    {
        rtn_val = 0;
        
        //writes every digit of given device to 0
        for(idx = 0; idx < 8; idx++)
        {
            if(rtn_val == 0)
            {
                rtn_val = write_digit(spi, device_number, (idx + 1), 0);
            }
        }
    }
    
    return(rtn_val);
}


void display_all_on(spiled_t* spi)
{
    uint8_t idx, idy;
    
    //writes every digit of every device to 0xFF
    for(idx = 0; idx < spi->cfg->num_devices; idx++)
    {
        for(idy = 0; idy < MAX7219_DIGIT_7; idy++)
        {
            write_digit(spi,(idx + 1), (idy + 1), 0xFF);
        }
    }
}
    
    
void display_all_off(spiled_t* spi)
{
    uint8_t idx, idy;
    
    //writes every digit of every device to 0
    for(idx = 0; idx < spi->cfg->num_devices; idx++)
    {
        for(idy = 0; idy < MAX7219_DIGIT_7; idy++)
        {
            write_digit(spi,(idx + 1), (idy + 1), 0);
        }
    }
}