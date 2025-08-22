
#ifndef __LT_MATRIX_KEY_H
#define __LT_MATRIX_KEY_H

#include "stm32f10x.h"

#define row 0
#define col 1				
#define MAX_Time   36000000 // 松手检测最大延时

typedef struct
{
    GPIO_TypeDef* GPIOX;
    uint16_t      GPIO_Pin;
    uint32_t      RCC_APB2_GPIOX;
}KGPT; //4*4

void Matrix_KEY_GPIO_Config(void);//IO初始化
u8 Matrix_KEY_Scan(void);   //按键扫描函数
u8 ReScan(GPIO_TypeDef* GPIOX,uint16_t colPin,u8 colIndex);//二次扫描
u8 KEY_Value2Char(u8 key_value);

#endif
