#include "max7219.h"                                  

static const uint8_t _num_devices = 4;
//static uint8_t buffer[23768]; or put in config object

void Max7219_Init(spiled_t* spi,
                    const spiled_config_t* cfg,
                    const spiled_hal_t* hal)
{
    memset(spi, 0, sizeof(spiled_t));
    spi->cfg = cfg;
    spi->hal = hal;
    //_num_devices = spi->cfg->device_number;

    init_display(spi);
    enable_display(spi);
    
    set_display_test(spi);
    spi->hal->_spiled_wait(spi,100);
    clear_display_test(spi);
    spi->hal->_spiled_wait(spi,100);
    display_all_off(spi);
    spi->hal->_spiled_wait(spi,100);
}

int32_t set_num_devices(uint8_t num_devices)
{
    int32_t rtn_val = -1;
    
    if(num_devices > 0)
    {
        //_num_devices = num_devices;
        rtn_val = _num_devices;
    }
    
    return rtn_val;
}

void set_display_test(spiled_t* spi)
{
    uint8_t idx = 0;
    
    int32_t num_bytes = 2 * _num_devices;
    uint8_t * d = (uint8_t *)malloc(num_bytes * sizeof(uint8_t));
    for(idx = _num_devices; idx > 0; idx--)
    {
        d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_DISPLAY_TEST;
        d[(_num_devices - idx) * 2 + 1] = 1;
    }
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
    spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
    free(d);
}

void clear_display_test(spiled_t* spi)
{
    uint8_t idx = 0;
    
    int32_t num_bytes = 2 * _num_devices;
    uint8_t * d = (uint8_t *)malloc(num_bytes * sizeof(uint8_t));
    for(idx = _num_devices; idx > 0; idx--)
    {
        d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_DISPLAY_TEST;
        d[(_num_devices - idx) * 2 + 1] = 0;
    }
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
    spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
    free(d);
}

int32_t init_device(spiled_t* spi, uint8_t device_number)
{
    int32_t rtn_val = -1;
    uint8_t idx = 0;
    
    if(device_number > _num_devices)
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
        int32_t num_bytes = 2 * _num_devices;
        uint8_t * d = (uint8_t *)malloc(num_bytes * sizeof(uint8_t));
        for(idx = _num_devices; idx > 0; idx--)
        {
            if(idx == device_number)
            {
                d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_DECODE_MODE;
                d[(_num_devices - idx) * 2 + 1] = spi->cfg->decode_mode;
            }
            else
            {
                d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_NO_OP;
                d[(_num_devices - idx) * 2 + 1] = (uint8_t) 0;
            }
        }
        spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
        spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
        spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
        
        spi->hal->_spiled_wait(spi,(uint32_t) 1);
        
        //write INTENSITY register of device
        for(idx = _num_devices; idx > 0; idx--)
        {
            if(idx == device_number)
            {
                d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_INTENSITY;
                d[(_num_devices - idx) * 2 + 1] = spi->cfg->intensity;
            }
            else
            {
                d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_NO_OP;
                d[(_num_devices - idx) * 2 + 1] = (uint8_t) 0;
            }
        }
        spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
        spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
        spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
        
        spi->hal->_spiled_wait(spi,(uint32_t) 1);
        
        //write SCAN_LIMT register of device
        for(idx = _num_devices; idx > 0; idx--)
        {
            if(idx == device_number)
            {
                d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_SCAN_LIMIT;
                d[(_num_devices - idx) * 2 + 1] = spi->cfg->scan_limit;
            }
            else
            {
                d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_NO_OP;
                d[(_num_devices - idx) * 2 + 1] = (uint8_t) 0;
            }
        }
        spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
        spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
        spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
        
        spi->hal->_spiled_wait(spi,(uint32_t) 1);
        free(d);

        rtn_val = 0;
    }
    
    return(rtn_val);
}


void init_display(spiled_t* spi)
{
    uint8_t idx = 0;
    
    //write DECODE_MODE register of all devices
    int32_t num_bytes = 2 * _num_devices;
    uint8_t * d = (uint8_t *)malloc(num_bytes * sizeof(uint8_t));
    for(idx = _num_devices; idx > 0; idx--)
    {
        d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_DECODE_MODE;
        d[(_num_devices - idx) * 2 + 1] = spi->cfg->decode_mode;
    }
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
    spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
    
    spi->hal->_spiled_wait(spi,(uint32_t) 1);
    
    //write INTENSITY register of all devices
    for(idx = _num_devices; idx > 0; idx--)
    {
        d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_INTENSITY;
        d[(_num_devices - idx) * 2 + 1] = spi->cfg->intensity;
    }
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
    spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
    
    spi->hal->_spiled_wait(spi,(uint32_t) 1);
    
    //write SCAN_LIMT register of all devices
    for(idx = _num_devices; idx > 0; idx--)
    {
        d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_SCAN_LIMIT;
        d[(_num_devices - idx) * 2 + 1] = spi->cfg->scan_limit;
    }
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
    spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
    
    spi->hal->_spiled_wait(spi,(uint32_t) 1);
    free(d);
}

int32_t enable_device(spiled_t* spi, uint8_t device_number)
{
    int32_t rtn_val = -1;
    uint8_t idx = 0;
    
    if(device_number > _num_devices)
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
        int32_t num_bytes = 2 * _num_devices;
        uint8_t * d = (uint8_t *)malloc(num_bytes * sizeof(uint8_t));
        for(idx = _num_devices; idx > 0; idx--)
        {
            if(idx == device_number)
            {
                d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_SHUTDOWN;
                d[(_num_devices - idx) * 2 + 1] = (uint8_t) 1;
            }
            else
            {
                d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_NO_OP;
                d[(_num_devices - idx) * 2 + 1] = (uint8_t) 0;
            }
        }
        spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
        spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
        spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
        free(d);

        rtn_val = 0;
    }
    
    return(rtn_val);
}


int32_t disable_device(spiled_t* spi, uint8_t device_number)
{
    int32_t rtn_val = -1;
    uint8_t idx = 0;
    
    if(device_number > _num_devices)
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
        int32_t num_bytes = 2 * _num_devices;
        uint8_t * d = (uint8_t *)malloc(num_bytes * sizeof(uint8_t));
        for(idx = _num_devices; idx > 0; idx--)
        {
            if(idx == device_number)
            {
                d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_SHUTDOWN;
                d[(_num_devices - idx) * 2 + 1] = (uint8_t) 0;
            }
            else
            {
                d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_NO_OP;
                d[(_num_devices - idx) * 2 + 1] = (uint8_t) 0;
            }
        }
        spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
        spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
        spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
        free(d);
        
        rtn_val = 0;
    }
    
    return(rtn_val);
}


void enable_display(spiled_t* spi)
{
    uint8_t idx = 0;
    
    int32_t num_bytes = 2 * _num_devices;
    uint8_t * d = (uint8_t *)malloc(num_bytes * sizeof(uint8_t));
    for(idx = _num_devices; idx > 0; idx--)
    {
        d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_SHUTDOWN;
        d[(_num_devices - idx) * 2 + 1] = 1;
    }
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
    spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
    free(d);
}
    

void disable_display(spiled_t* spi)
{
    uint8_t idx = 0;
    
    int32_t num_bytes = 2 * _num_devices;
    uint8_t * d = (uint8_t *)malloc(num_bytes * sizeof(uint8_t));
    for(idx = _num_devices; idx > 0; idx--)
    {
        d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_SHUTDOWN;
        d[(_num_devices - idx) * 2 + 1] = 0;
    }
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
    spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
    spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
    free(d);
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
        if(device_number > _num_devices)
        {
            rtn_val = -1;
        }
        else if(device_number == 0)
        {
            rtn_val = -2;
        }
        else
        {
            int32_t num_bytes = 2 * _num_devices;
            //uint8_t * d = (uint8_t *)malloc(num_bytes * sizeof(uint8_t));
            uint8_t d[num_bytes];
            memset(d,0,num_bytes); 
            for(idx = _num_devices; idx > 0; idx--)
            {
                if(idx == device_number)
                {
                    d[(_num_devices - idx) * 2] = digit;
                    d[(_num_devices - idx) * 2 + 1] = data;
                }
                else
                {
                    d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_NO_OP;
                    d[(_num_devices - idx) * 2 + 1] = (uint8_t) 0;
                }
            }
            spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
            spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
            spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
            //free(d);
            
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
        if(device_number > _num_devices)
        {
            rtn_val = -1;
        }
        else if(device_number == 0)
        {
            rtn_val = -2;
        }
        else
        {
            int32_t num_bytes = 2 * _num_devices;
            uint8_t * d = (uint8_t *)malloc(num_bytes * sizeof(uint8_t));
            for(idx = _num_devices; idx > 0; idx--)
            {
                if(idx == device_number)
                {
                    d[(_num_devices - idx) * 2] = digit;
                    d[(_num_devices - idx) * 2 + 1] = (uint8_t) 0;
                }
                else
                {
                    d[(_num_devices - idx) * 2] = (uint8_t) MAX7219_NO_OP;
                    d[(_num_devices - idx) * 2 + 1] = (uint8_t) 0;
                }
            }
            spi->hal->_spiled_spi_cs(spi,(uint8_t) 1);
            spi->hal->_spiled_spi_txrx(spi,d,num_bytes);
            spi->hal->_spiled_spi_cs(spi,(uint8_t) 0);
            free(d);
            
            rtn_val = 0;
        }
    }
    
    return(rtn_val);
}


int32_t device_all_on(spiled_t* spi, uint8_t device_number)
{
    int32_t rtn_val = -1;
    uint8_t idx = 0;
    
    if(device_number > _num_devices)
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
    
    if(device_number > _num_devices)
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
    for(idx = 0; idx < _num_devices; idx++)
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
    for(idx = 0; idx < _num_devices; idx++)
    {
        for(idy = 0; idy < MAX7219_DIGIT_7; idy++)
        {
            write_digit(spi,(idx + 1), (idy + 1), 0);
        }
    }
}

void set_bit_pattern(uint8_t * data, const uint32_t length, char character){
    uint8_t * tmp;
    switch (character)
    {
    case 'A':
        tmp = (uint8_t[8]){
            0x00,0x3c,0x66,0x66,0x7e,0x66,0x66,0x66
        };
        break;
    
    case 'D':
        tmp = (uint8_t[8]){
            0x00,0x7c,0x66,0x66,0x66,0x66,0x66,0x7c
        };
        break;

    case 'E':
        tmp = (uint8_t[8]){
            0x00,0x7e,0x60,0x60,0x7c,0x60,0x60,0x7e
        };
        break;

    case 'H':
        tmp = (uint8_t[8]){
            0x00,0x66,0x66,0x66,0x7e,0x66,0x66,0x66
        };
        break;

    case 'L':
        tmp = (uint8_t[8]){
            0x00,0x60,0x60,0x60,0x60,0x60,0x60,0x7e
        };
        break;

    case 'N':
        tmp = (uint8_t[8]){
            0x00,0x63,0x73,0x7b,0x6f,0x67,0x63,0x63
        };
        break;

    case 'O':
        tmp = (uint8_t[8]){
            0x00,0x3c,0x66,0x66,0x66,0x66,0x66,0x3c
        };
        break;

    case 'S':
        tmp = (uint8_t[8]){
            0x00,0x3c,0x66,0x60,0x3c,0x06,0x66,0x3c
        };
        break;

    case 'T':
        tmp = (uint8_t[8]){
            0x00,0x7e,0x5a,0x18,0x18,0x18,0x18,0x18
        };
        break;

    default:
        tmp = (uint8_t[8]){
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
        };
        break;
    }
    for (size_t i = 0; i < length; i++)
    {
        data[i] = tmp[i];
    }
}