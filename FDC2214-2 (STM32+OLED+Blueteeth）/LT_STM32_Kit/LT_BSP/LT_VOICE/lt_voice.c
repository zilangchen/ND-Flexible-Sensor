#include "stdint.h"
#include "usart.h"
#include "delay.h"
#include "lt_voice.h"
#include "lt_oled.h"

uint8_t voice[VOICE_SEG_MAX][DATA_LEN] = {
	{0xAA, 0x07, 0x02, 0x00, 0x01, 0xB4},	//0
	{0xAA, 0x07, 0x02, 0x00, 0x02, 0xB5}, 	//1
	{0xAA, 0x07, 0x02, 0x00, 0x03, 0xB6}, 	//2
	{0xAA, 0x07, 0x02, 0x00, 0x04, 0xB7},   //3
	{0xAA, 0x07, 0x02, 0x00, 0x05, 0xB8}, 	//4
	{0xAA, 0x07, 0x02, 0x00, 0x06, 0xB9}, 	//5
	{0xAA, 0x07, 0x02, 0x00, 0x07, 0xBA}, 	//6
	{0xAA, 0x07, 0x02, 0x00, 0x08, 0xBB}, 	//7
	{0xAA, 0x07, 0x02, 0x00, 0x09, 0xBC}, 	//8
	{0xAA, 0x07, 0x02, 0x00, 0x0A, 0xBD},	//9
	{0xAA, 0x07, 0x02, 0x00, 0x0B, 0xBE}, 	//¾àÀëÎª
	{0xAA, 0x07, 0x02, 0x00, 0x0C, 0xBF},	//ºÁÃ×
};

void play_temper(float temper)
{
    uint8_t  num[10] = {0};
    uint16_t temperInt = 0;
    int8_t i = 0;
    
    temperInt = temper*10;
    do
    {
        num[i] = temperInt%10;
        i++;
        temperInt/=10;
    }
    while(temperInt != 0);
    i = i - 1;
    
    uart2_send_buff(&voice[10][0], DATA_LEN);
    delay_ms(1000);
    
    for(; i >= 0; i--)
	{
		delay_ms(1000);
		uart2_send_buff(&voice[num[i]][0], DATA_LEN);
        if(i == 1)
        {
            delay_ms(1000);
            uart2_send_buff(&voice[11][0], DATA_LEN);
        }
	}
    delay_ms(1000);
    uart2_send_buff(&voice[12][0], DATA_LEN);
    
    if((temper > 37.5) || (temper < 36))
    {
        OLED_ShowCHinese(72, 4, 14);
        delay_ms(3000);
        uart2_send_buff(&voice[13][0], DATA_LEN);
    }
    else
    {
        OLED_ShowCHinese(72, 4, 15);
    }
}

void play_distance(uint32_t distance)
{
    uint8_t  num[10] = {0};
    int8_t i = 0;
    
    do
    {
        num[i] = distance%10;
        i++;
        distance/=10;
    }
    while(distance != 0);
    i = i - 1;
    
    uart2_send_buff(&voice[10][0], DATA_LEN);
    delay_ms(1000);
    
   for(; i >= 0; i--)
	{
		delay_ms(1000);
		uart2_send_buff(&voice[num[i]][0], DATA_LEN);

	}
    delay_ms(1000);
    uart2_send_buff(&voice[11][0], DATA_LEN);
}