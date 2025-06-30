#include "sys.h"
#include "delay.h"
#include "led.h"
#include "uart1.h"
#include "key.h"
#include "cJSON.h"
#include "esp8266.h"
#include "oled.h"
#include "weather.h"

int main(void)
{
    HAL_Init();                         /* 初始化HAL库 */
    stm32_clock_init(RCC_PLL_MUL9); /* 设置时钟, 72Mhz */
    led_init();
    led1_on();
    led2_off();
    
    uart1_init(115200);
    key_init();
    esp8266_init(115200);
    int check = get_weather();
    if(!check) printf("Something went wrong\r\n");
    
    oled_fill(0x00);
    oled_display_init();
    show_weather(0);
    
    printf("Hello World!\r\n");    
    //oled_display_celsius(0,0);
    uint8_t keyNum = 0;
    uint8_t day = 0;
    
    while(1)
    {      
        keyNum = key_scan();
        if(keyNum == 1){
            day++;
            if(day > 3) day = 0;
            oled_display_init();
            show_weather(day);
        }else if(keyNum ==2){
            printf("update weather \r\n");
            oled_display_init();
            show_weather(0);
            day = 0;
        }
    }
}