#ifndef __OLED_H__
#define __OLED_H__

#include <sys.h>

#define OLED_I2C_SCL_CLK()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define OLED_I2C_SCL_PORT           GPIOB
#define OLED_I2C_SCL_PIN            GPIO_PIN_8

#define OLED_I2C_SDA_CLK()          __HAL_RCC_GPIOB_CLK_ENABLE()
#define OLED_I2C_SDA_PORT           GPIOB
#define OLED_I2C_SDA_PIN            GPIO_PIN_9

#define OLED_I2C_SCL_RESET()        HAL_GPIO_WritePin(OLED_I2C_SCL_PORT, OLED_I2C_SCL_PIN, GPIO_PIN_RESET)
#define OLED_I2C_SCL_SET()          HAL_GPIO_WritePin(OLED_I2C_SCL_PORT, OLED_I2C_SCL_PIN, GPIO_PIN_SET)

#define OLED_I2C_SDA_RESET()        HAL_GPIO_WritePin(OLED_I2C_SDA_PORT, OLED_I2C_SDA_PIN, GPIO_PIN_RESET)
#define OLED_I2C_SDA_SET()        HAL_GPIO_WritePin(OLED_I2C_SDA_PORT, OLED_I2C_SDA_PIN, GPIO_PIN_SET)


void oled_gpio_init();
void oled_init();
void i2c_start();
void i2c_stop();
void i2c_ack();
void i2c_write_byte(uint8_t data);
void oled_write_cmd(uint8_t cmd);
void oled_write_data(uint8_t data);
void oled_set_position(uint8_t x, uint8_t y);
void oled_fill(uint8_t data);
void oled_display_char(uint8_t x, uint8_t y, uint8_t num, uint8_t size);
void oled_display_string(uint8_t x, uint8_t y, char* p, uint8_t size);
void oled_show_image(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t *bmp);

void oled_display_celsius(uint8_t x, uint8_t y);

void oled_display_init();
#endif