#include "weather.h"
#include "esp8266.h"
#include "stdio.h"
#include "cJSON.h"
#include "stdlib.h"
#include "string.h"
#include "oled.h"

char weather_data[1024];

typedef struct{
    char date[20];
    char maxTemp[10];
    char minTemp[10];
    int weatherCode;
}weather_struct;

weather_struct weatherForecast[4] = {0};

//used to duplicate a string (char*)
//copies the original string and allocates new memory on the heap, returns a pointer to the new memory
char* strdup(const char* str) {
    if (str == NULL) return NULL;
    
    size_t len = strlen(str) + 1; // +1 for '\0'
    char* new_str = (char*)malloc(len);
    
    if (new_str != NULL) {
        strcpy(new_str, str);
    }
    
    return new_str;
}

//helper function to remove the two wield symbol (didn't use it)
void remove_degree_symbol(char *data) {
    char *src = data;
    char *dst = data;
    
    while (*src) {
        if ((uint8_t)src[0] == 0xC2 && (uint8_t)src[1] == 0xB0) {
            src += 2;  // 跳过这两个字节
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';  // 确保字符串终止
}

uint8_t get_weather(){
    esp8266_get_weather();
    esp8266_copy_rxdata(weather_data);
    //remove_degree_symbol(weather_data);
    //printf("%s\r\n", weather_data);
    
    
    //the issue was root was always NULL
    //solution: increase the heap size in startup_stm32f10xb.s
    cJSON *root = cJSON_Parse((const char*)weather_data);
    if (!root) {
        printf("Error parsing JSON.\n");
        return 0;
    }
    
    cJSON *daily = cJSON_GetObjectItem(root, "daily");
    
    if (daily == NULL) {
        printf("Error getting 'daily' object.\n");
        cJSON_Delete(root);
        return 0;
    }
    
    cJSON *time_array = cJSON_GetObjectItem(daily, "time");
    if (time_array == NULL || !cJSON_IsArray(time_array)) {
        printf("Error getting 'time' array.\n");
        cJSON_Delete(root);
        return 0;
    }
    
    cJSON *temp_max_array = cJSON_GetObjectItem(daily, "temperature_2m_max");
    if (temp_max_array == NULL || !cJSON_IsArray(temp_max_array)) {
        printf("Error getting 'temperature_2m_max' array.\n");
        cJSON_Delete(root);
        return 0;
    }
    
    cJSON *temp_min_array = cJSON_GetObjectItem(daily, "temperature_2m_min");
    if (temp_min_array == NULL || !cJSON_IsArray(temp_min_array)) {
        printf("Error getting 'temperature_2m_min' array.\n");
        cJSON_Delete(root);
        return 0;
    }
    
    cJSON *weather_code_array = cJSON_GetObjectItem(daily, "weather_code");
    if (weather_code_array == NULL || !cJSON_IsArray(weather_code_array)) {
        printf("Error getting 'weather_code' array.\n");
        cJSON_Delete(root);
        return 0;
    }
    
    int array_size = 4;
//    char **time = malloc(array_size * sizeof(char *));
//    double *temperature_2m_max = malloc(array_size * sizeof(double));
//    double *temperature_2m_min = malloc(array_size * sizeof(double));
//    int *weather_code = malloc(array_size * sizeof(int));
    
    //copy contentt from CJSON arrays to local arrays
    for (int i = 0; i < array_size; i++) {
        cJSON *time_item = cJSON_GetArrayItem(time_array, i);
        cJSON *temp_max_item = cJSON_GetArrayItem(temp_max_array, i);
        cJSON *temp_min_item = cJSON_GetArrayItem(temp_min_array, i);
        cJSON *weather_code_item = cJSON_GetArrayItem(weather_code_array, i);

        if (time_item && temp_max_item && temp_min_item && weather_code_item) {
            strcpy(weatherForecast[i].date, time_item->valuestring);
            
            //converted variables in to string
            snprintf(weatherForecast[i].maxTemp, sizeof(weatherForecast[i].maxTemp), "%.1f", temp_max_item->valuedouble);
            snprintf(weatherForecast[i].minTemp, sizeof(weatherForecast[i].minTemp), "%.1f", temp_min_item->valuedouble);

//            snprintf(weatherForecast[i].weatherCode, sizeof(weatherForecast[i].weatherCode), "%d", weather_code_item->valueint);

//            time[i] = strdup(time_item->valuestring);
//            temperature_2m_max[i] = temp_max_item->valuedouble;
//            temperature_2m_min[i] = temp_min_item->valuedouble;
            weatherForecast[i].weatherCode = weather_code_item->valueint;
        }
    }
    
    printf("time: ");
    for(int i = 0; i < array_size; i++)
        printf("%s ", weatherForecast[i].date);
    printf("\r\n");
    
    printf("MaxTemp: ");
    for(int i = 0; i < array_size; i++)
        printf("%s ", weatherForecast[i].maxTemp);
    printf("\r\n");
    
    printf("MinTemp: ");
    for(int i = 0; i < array_size; i++)
        printf("%s ", weatherForecast[i].minTemp);
    printf("\r\n");
    
    printf("weather_code: ");
    for (int i = 0; i < array_size; i++) {
        printf("%d ", weatherForecast[i].weatherCode);
    }
    printf("\n");
    
//    for(int i = 0; i < array_size; i++)
//        free(time[i]);
//    
//    free(time);
//    free(temperature_2m_max);
//    free(temperature_2m_min);
//    free(weather_code);
    cJSON_Delete(root);
    
    return 1;
}

char* weatherCodeToText(int code) {
  switch (code) {
    case 0: return "Sunny";
    case 1: return "Sunny";
    case 2: return "Cloudy";
    case 3: return "Cloudy";
    case 45: return "Overcast";
    case 48: return "Foggy";
    case 51: return "Drizzly";
    case 53: return "Drizzly";
    case 55: return "Drizzly";
    case 56: return "Drizzly";
    case 57: return "Drizzly";
    case 61: return "Rainy";
    case 63: return "Rainy";
    case 65: return "Rainy";
    case 66: return "Rainy";
    case 67: return "Rainy";
    case 71: return "Snowy";  
    case 73: return "Snowy";  
    case 75: return "Snowy"; 
    case 77: return "Snowy"; 
    case 80: return "Rainy";
    case 81: return "Rainy";
    case 82: return "Rainy";
    case 85: return "Snowy";
    case 86: return "Snowy";
    case 95: return "Stormy";
    case 96: return "Haily";
    case 99: return "Haily";
    default: return "Unknown";
  }
}

void show_weather(uint8_t day){
    oled_display_string(10, 0, weatherForecast[day].date, 16);
    char* todayWeather = weatherCodeToText(weatherForecast[day].weatherCode);
    oled_display_string(64, 2, todayWeather, 16);
    
    oled_display_string(64, 4, weatherForecast[day].maxTemp, 16);
    oled_display_string(64, 6, weatherForecast[day].minTemp, 16);
}


