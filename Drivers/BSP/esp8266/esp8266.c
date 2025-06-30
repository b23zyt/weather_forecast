#include "esp8266.h"
#include "stdio.h"
#include "string.h"
#include "delay.h"
#include "stdarg.h"

uint8_t esp8266_rx_buf[ESP8266_RX_BUF_SIZE];
uint8_t esp8266_tx_buf[ESP8266_TX_BUF_SIZE];
uint16_t esp8266_cnt = 0, esp8266_cntPre = 0;

UART_HandleTypeDef esp8266_handle = {0};
void esp8266_uart_init(uint32_t baudrate)
{
    esp8266_handle.Instance = USART2;
    esp8266_handle.Init.BaudRate = baudrate;
    esp8266_handle.Init.WordLength = UART_WORDLENGTH_8B;
    esp8266_handle.Init.StopBits = UART_STOPBITS_1;
    esp8266_handle.Init.Parity = UART_PARITY_NONE;
    esp8266_handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    esp8266_handle.Init.Mode = UART_MODE_TX_RX;
    HAL_UART_Init(&esp8266_handle);
}

void USART2_IRQHandler(void)
{
    uint8_t receive_data = 0;
    if(__HAL_UART_GET_FLAG(&esp8266_handle, UART_FLAG_RXNE) != RESET)
    {
        if(esp8266_cnt >= sizeof(esp8266_rx_buf))
            esp8266_cnt = 0;
        HAL_UART_Receive(&esp8266_handle, &receive_data, 1, 1000);
        esp8266_rx_buf[esp8266_cnt++] = receive_data;
        //uart1_cnt++;
        //HAL_UART_Transmit(&uart1_handle, &receive_data, 1, 1000);
    }
}

uint8_t esp8266_wait_receive(void)
{
    if(esp8266_cnt == 0)
        return ESP8266_ERROR;
    
    if(esp8266_cnt == esp8266_cntPre)
    {
        esp8266_cnt = 0;
        return ESP8266_EOK;
    }
    
    esp8266_cntPre = esp8266_cnt;
    return ESP8266_ERROR;
}

void esp8266_rx_clear(void)
{
    memset(esp8266_rx_buf, 0, sizeof(esp8266_rx_buf));
    esp8266_cnt = 0;
}

void esp8266_receive_data(void)
{
    if(esp8266_wait_receive() == ESP8266_EOK)
    {
        printf("esp8266 recv: %s\r\n", esp8266_rx_buf);
        esp8266_rx_clear();
    }
}

void esp8266_send_data(char* fmt, ...){
    va_list ap;
    uint16_t len;
    
    va_start(ap,fmt);
    vsprintf((char*)esp8266_tx_buf, fmt, ap);
    va_end(ap);
    
    len = strlen((const char*)esp8266_tx_buf);
    HAL_UART_Transmit(&esp8266_handle, esp8266_tx_buf, len, 100);
}


//used to check if the command sent back from esp8266 = res
uint8_t esp8266_send_command(char *cmd, char *res)
{
    //之前的timeout 值为250， 太短了，导致wifi 无法连接
    uint32_t time_out = 2000;
    //clear the receive buffer
    esp8266_rx_clear();
    HAL_UART_Transmit(&esp8266_handle, (uint8_t *)cmd, strlen(cmd), 100);
    
    while(time_out--)
    {
        if(esp8266_wait_receive() == ESP8266_EOK)
        {
            //printf("%s\r\n", esp8266_rx_buf);
            if(strstr((const char*)esp8266_rx_buf, res) != NULL)
                return ESP8266_EOK;
        }
        delay_ms(10);
    }
    
    return ESP8266_ERROR;
}



//AT -> OK
uint8_t esp8266_test(){
    return esp8266_send_command("AT\r\n", "OK");
}

//AT+CWMODE = 1/2/3 STA/AP/STA AP
uint8_t esp8266_setMode(uint8_t mode){
    switch(mode)
    {
        case ESP8266_STA_MODE:
            return esp8266_send_command("AT+CWMODE=1\r\n", "OK");
        case ESP8266_AP_MODE:
            return esp8266_send_command("AT+CWMODE=2\r\n", "OK");
        case ESP8266_STA_AP_MODE:
            return esp8266_send_command("AT+CWMODE=3\r\n", "OK");
        default:
            return ESP8266_EINVAL;
    }
}

//connect to wifi (if mode = STA)
//AT+CWJAP="wifi name","password"
uint8_t esp8266_joinAp(char* name, char* password){
    char cmd[128];
    sprintf(cmd, "AT+CWJAP=\"%s\",\"%s\"\r\n", name, password);

    return esp8266_send_command(cmd, "OK");
}

//AT+CIPMUX connection mode. single route or dual route
uint8_t esp8266_connectionMode(uint8_t mode){
    char cmd[128];
    sprintf(cmd, "AT+CIPMUX=%d\r\n", mode);
    //printf("%s\r\n", cmd);
    return esp8266_send_command(cmd, "OK");
}

//AT+CIPSTART="TCP","192.168.....",8080
uint8_t esp8266_connect_tcp_server(char* server_ip, char* server_port){
    char cmd[128];
    sprintf(cmd, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",server_ip, server_port);
    //printf("%s\r\n", cmd);
    return esp8266_send_command(cmd, "CONNECT");
}

//AT+CIPMODE=1
//AT+CIPSEND
//uint8_t esp8266_enter_unvarnished(){
//    return (esp8266_send_command("AT+CIPMODE=1\r\n", "OK") && esp8266_send_command("AT+CIPSEND", ">"));
//这样写会导致提前return,错误
uint8_t esp8266_enter_unvarnished(void)
{
    uint8_t ret;
    ret = esp8266_send_command("AT+CIPMODE=1\r\n", "OK");
    ret += esp8266_send_command("AT+CIPSEND\r\n", ">");
    if (ret == ESP8266_EOK)
        return ESP8266_EOK;
    else
        return ESP8266_ERROR;
}



void esp8266_init(uint32_t baudrate)
{
    printf("Initialize starts \r\n");
    
    esp8266_uart_init(baudrate);
    
    esp8266_send_command("AT+RESTORE\r\n","ready");
    
    //return EOK=0, whenever receive a 0, skip the while loop 
    while(esp8266_test())
        delay_ms(500);
    printf("1. ESP8266 module is ready \r\n");
    
    
    while(esp8266_setMode(ESP8266_STA_MODE))
        delay_ms(500);
    printf("2. Mode is set to STATION \r\n");
    
    
    while(esp8266_connectionMode(ESP8266_SINGLE_CONECTION))
        delay_ms(500);
    printf("3. Connection is set to single IP \r\n");
    
    
    while(esp8266_joinAp(WIFI_NAME, WIFI_PASSWORD))
        delay_ms(1500);
    printf("4. WIFI connected \r\n");
    
    //需要先打开网路调试助手
    while(esp8266_connect_tcp_server(TCP_SERVER_IP, TCP_SERVER_PORT))
        delay_ms(500);
    printf("5. Connected to TCP server: server IP: %s, server_port: %s\r\n", TCP_SERVER_IP, TCP_SERVER_PORT);
    
    while(esp8266_enter_unvarnished())
        delay_ms(500);
    printf("6. Set to unvarnished transmission mode\r\n");
    
    
    printf("Able to send data now \r\n");
    printf("Initialize finished \r\n");
    
}

//https://api.weatherapi.com/v1/forecast.json?key=7a7581a747a64aa89a5215833252706&q=43.47,-80.52&days=3
uint8_t esp8266_get_weather(){
    uint8_t ret = ESP8266_ERROR;
    char http_request[200] = {0};
    sprintf(http_request, "GET https://api.open-meteo.com/v1/forecast?latitude=43.48&longitude=-80.54&daily=temperature_2m_max,temperature_2m_min,weather_code&forecast_days=4&wind_speed_unit=ms\r\n");
    ret = esp8266_send_command(http_request, "latitude");
    //printf("%s\r\n", esp8266_rx_buf);
    if(ret == ESP8266_EOK) return ESP8266_EOK;
    else return ESP8266_ERROR;
}

uint16_t esp8266_copy_rxdata(char *data){
    memcpy(data, esp8266_rx_buf, esp8266_cntPre);
    return esp8266_cntPre;
}

void esp8266_send_test(void)
{
    esp8266_send_data("FROM ESP8266\r\n");
    esp8266_receive_data();
}

