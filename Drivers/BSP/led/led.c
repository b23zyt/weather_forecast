#include "led.h"
#include "sys.h"

//initialize
void led_init(void){
    GPIO_InitTypeDef gpio_initstruct;
    
    //open clock
    __HAL_RCC_GPIOB_CLK_ENABLE();
    gpio_initstruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
    gpio_initstruct.Mode = GPIO_MODE_OUTPUT_PP;
    gpio_initstruct.Pull = GPIO_NOPULL;
    gpio_initstruct.Speed = GPIO_SPEED_FREQ_HIGH;
    
    HAL_GPIO_Init(GPIOB, &gpio_initstruct);
    
    led1_off();
    led2_off();
}

void led1_on(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
}

void led1_off(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
}

void led1_toggle(void){
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
}

void led2_on(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
}

void led2_off(void){
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
}

void led2_toggle(void){
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_9);
}