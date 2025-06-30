#include "oled.h"
#include "delay.h"
#include "font.h"
void oled_gpio_init(){
    GPIO_InitTypeDef gpio_initstruct;
    
    OLED_I2C_SCL_CLK();
    OLED_I2C_SDA_CLK();
    
    //SCL
    gpio_initstruct.Pin = OLED_I2C_SCL_PIN;
    gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull = GPIO_PULLUP;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(OLED_I2C_SCL_PORT, &gpio_initstruct);
    
    //SDA
    gpio_initstruct.Pin = OLED_I2C_SDA_PIN;
    HAL_GPIO_Init(OLED_I2C_SDA_PORT, &gpio_initstruct);
}

void oled_init(){
    oled_gpio_init();
    
    oled_gpio_init();
    
    delay_ms(100);
    
    oled_write_cmd(0xAE);
    oled_write_cmd(0xD5);
    oled_write_cmd(0x80);

    oled_write_cmd(0xA8);
    oled_write_cmd(0x3F);

    oled_write_cmd(0xD3);
    oled_write_cmd(0x00);

    oled_write_cmd(0x40);

    oled_write_cmd(0xA1);

    oled_write_cmd(0xC8);

    oled_write_cmd(0xDA);
    oled_write_cmd(0x12);

    oled_write_cmd(0x81);
    oled_write_cmd(0xCF);

    oled_write_cmd(0xD9);
    oled_write_cmd(0xF1);

    oled_write_cmd(0xDB);
    oled_write_cmd(0x30);

    oled_write_cmd(0xA4);

    oled_write_cmd(0xA6);

    oled_write_cmd(0x8D);
    oled_write_cmd(0x14);

    oled_write_cmd(0xAF);
    
    oled_fill(0x00);
}

//i2c 
void i2c_start(){
    //SCL is high, SDA is in falling edge
    OLED_I2C_SCL_SET();
    OLED_I2C_SDA_SET();
    OLED_I2C_SDA_RESET();
    OLED_I2C_SCL_RESET();
}

void i2c_stop(){
    //SCL is high, SDA rising edge
    OLED_I2C_SCL_SET();
    OLED_I2C_SDA_RESET();
    OLED_I2C_SDA_SET();
}

void i2c_ack(){
    //when SCL is high, read SDA (0: ack, 1 nack)
    OLED_I2C_SCL_SET();
    OLED_I2C_SCL_RESET();
}

void i2c_write_byte(uint8_t data){
    //when scl is low, write data to sda, when scl is high, read data from sda
    uint8_t i, temp;
    temp = data;
    
    //begin with the first bit
    for(i = 0; i < 8; i++){
        if((temp & 0x80) == 0x80) OLED_I2C_SDA_SET();
        else OLED_I2C_SDA_RESET();
        temp = temp << 1;
        
        OLED_I2C_SCL_SET();
        OLED_I2C_SCL_RESET();
    }
}

void oled_write_cmd(uint8_t cmd){
    i2c_start();
    //send one byte 0b'0111000 (0x78)
    i2c_write_byte(0x78);
    i2c_ack();
    
    //then sent 0b'00000000 (0x00)
    i2c_write_byte(0x00);
    i2c_ack();
    //then send cmd
    i2c_write_byte(cmd);
    i2c_ack();
    i2c_stop();
}

void oled_write_data(uint8_t data){
    i2c_start();
    //send one byte 0b'0111000 (0x78)
    i2c_write_byte(0x78);
    i2c_ack();
    
    //then sent 0b'01000000 (0x40)
    i2c_write_byte(0x40);
    i2c_ack();
    //then send cmd
    i2c_write_byte(data);
    i2c_ack();
    i2c_stop();
}

//x axis is column, y axis is pages
void oled_set_position(uint8_t x, uint8_t y){
    oled_write_cmd(0xB0 + y);
    oled_write_cmd(x & 0x0F);
    oled_write_cmd(((x & 0xF0) >> 4) | 0x10);
}
void oled_fill(uint8_t data){
    uint8_t i, j;
    for(i = 0; i < 8; i++){
        for(j = 0; j < 128; j++){
            oled_set_position(j, i);
            oled_write_data(data);
        }
    }
}

void oled_display_char(uint8_t x, uint8_t y, uint8_t num, uint8_t size){
    uint8_t i, j, page;
    num = num - ' ';
    page = size / 8; //number of pages
    if(size % 8 != 0) page++;
    
    //j is used to iterate through pages
    for(j = 0; j < page; j++){
        //from 0 to 8, 8 to 16, .... 
        oled_set_position(x, y+j);
        for(i = size/2 * j; i < size/2 * (j+1); i++){
            if(size == 12) oled_write_data(ascii_6X12[num][i]);
            else if(size ==16) oled_write_data(ascii_8X16[num][i]);
            else if(size == 24) oled_write_data(ascii_12X24[num][i]);
        }
    }
}

void oled_display_celsius(uint8_t x, uint8_t y){
    uint8_t i, j;
    for(j = 0; j < 2; j++){
        oled_set_position(x, y + j);
        for(i = 16 * j; i < 16 * (j+1); i++){
            oled_write_data(celsius[0][i]);
        }
    }
}

void oled_display_string(uint8_t x, uint8_t y, char* p, uint8_t size){
    while(*p != '\0'){
        oled_display_char(x, y, *p, size);
        x += size/2; //the width of each character is equal to size/2
        p++;
    }
}

void oled_show_image(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t *bmp){
    uint8_t i, j;
    for(j = 0; j < height; j++){
        oled_set_position(x, y+j);
        for(i = 0; i < width; i++){
            oled_write_data(bmp[width*j + i]);
        }
    }
}

void oled_display_init(){
    oled_init();
    oled_display_string(10, 2, "K-W", 16);
    oled_display_string(10, 4, "MaxTem", 16);
    oled_display_celsius(100, 4);
    oled_display_string(10, 6, "MinTem", 16);
    oled_display_celsius(100, 6);
}