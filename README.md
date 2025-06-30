
# Wi-Fi Weather Forecast Project

Use ESP8266 to obtain weather data (open-meteo) via HTTP and display it on the OLED screen.\
Key 1: Loop switch the weather data of today/tomorrow/the day after tomorrow/three days from now.\
Key 2: Update the weather.



## Tech Stack

**Hardware:** STM32F103, STlink, I2C Serial OLED Display, ESP8266 WIFI Module, CH340(for debug), jumpers, 2 keys

**WIRING** 
| STM32 | ESP8266   | OLED  | KEY1  | KEY2   | CH340(Optional)| STlink
| :---  |  :---:    | :---: |:---:  |:---:   | :---:     |  :---: |
| PA0   |           |       | I/O   |        |                | |
| PA1   |           |       |       |  I/O   |                ||
| PA2 (TX2)   |    RX     |       |       |        |             ||
| PA3 (RX2)  |    TX     |       |       |        |               ||
| PB8   |           |   SCL |       |        |              ||
| PB9   |           |   SDA |       |        |              ||
|CLK|||||||||||SWCLK|
|DIO|||||||||||SWDIO|
| PA9 (TX1)  |           |       |       |        | RX||
|PA10 (RX1) |             |     |          |        | TX    |||
| GND   |   GND| GND| GND |GND|GND|GND|
|VCC|VCC|VCC|VCC|VCC||||

**SOFTWARE**
Weather API: GET https://api.open-meteo.com/v1/forecast?latitude=43.46&longitude=-80.52&daily=temperature_2m_max,temperature_2m_min,weather_code&forecast_days=3&wind_speed_unit=ms
My location: Kitchener-Waterloo\
You can change the parameters weather in different days and different locations. 


## Deployment

Open the project in Keil and run.



## Demo

Insert gif or link to demo


## Related

Here are some related projects




## Support

For support, email benjaminzeng2110.com.

