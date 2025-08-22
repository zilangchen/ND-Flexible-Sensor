#include "lt_matrix_key.h"
#include "lt_beep.h"
#include "delay.h"

/**
  ******************************************************************************
  * @file    lt_matrix_key.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-12-13
  * @brief   矩阵按键应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:零涂LT_STM32_Kit开发板 
  * 管脚    :行：C1、C3、A1、A5
             列: A7、C5、B1、F11
  * 使用示例:
             delay_init();	    	 //延时函数初始化	  
             NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
             uart_init(115200);	 //串口初始化为115200
             Matrix_KEY_GPIO_Config();
             
             while(1)
             {	
                KeyNum = Matrix_KEY_Scan();
                if(KeyNum != 0)
                    printf("%c\r\n", KEY_Value2Char(KeyNum));
                
                delay_ms(10);
             }	
  * 微信    :lingtu15679003191
  * 扣扣    :1372090604
  *
  ******************************************************************************
  */

/********************************************移植修订点开始*****************************************************/
/* 只需要在下面填写横竖行的IO口、管脚和时钟端口，IO口和管脚都可以随意定义 无需在一个IO口 管脚也无需按顺序 */
KGPT KPIN[2][4]= { 
    {{GPIOB,GPIO_Pin_11, RCC_APB2Periph_GPIOB},{GPIOB,GPIO_Pin_10,RCC_APB2Periph_GPIOB}, {GPIOA,GPIO_Pin_2,RCC_APB2Periph_GPIOB},{GPIOB,GPIO_Pin_1,RCC_APB2Periph_GPIOB}}, //row 横行
    {{GPIOB,GPIO_Pin_0,RCC_APB2Periph_GPIOB}, {GPIOA,GPIO_Pin_7,RCC_APB2Periph_GPIOA},  {GPIOA,GPIO_Pin_6,RCC_APB2Periph_GPIOA}, {GPIOA,GPIO_Pin_5,RCC_APB2Periph_GPIOA}}  //col 竖行
};
/********************************************移植修订点结束*****************************************************/
//按键初始化函数
void Matrix_KEY_GPIO_Config(void)
{
    u8 i;
    GPIO_InitTypeDef GPIO_InitStructure;
    // RCC_APB2PeriphClockCmd(RCC_APB2_GPIOX,ENABLE);//使能PORTC时钟
	
    for(i=0; i<4; i++)
    {
        RCC_APB2PeriphClockCmd(KPIN[row][i].RCC_APB2_GPIOX,ENABLE);//使能PORTC时钟
        GPIO_InitStructure.GPIO_Pin = KPIN[row][i].GPIO_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //设置成推挽输出
        GPIO_Init(KPIN[row][i].GPIOX, &GPIO_InitStructure);//初始化row
        GPIO_ResetBits(KPIN[row][i].GPIOX,KPIN[row][i].GPIO_Pin);
    }
	
    for(i=0; i<4; i++)
    {
        RCC_APB2PeriphClockCmd(KPIN[col][i].RCC_APB2_GPIOX,ENABLE);//使能PORTC时钟
        GPIO_InitStructure.GPIO_Pin  = KPIN[col][i].GPIO_Pin;//PC5
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //设置成上拉输入
        GPIO_Init(KPIN[col][i].GPIOX, &GPIO_InitStructure);//初始化col
    }
}

u8 ReScan(GPIO_TypeDef* GPIOX,uint16_t colPin,u8 colIndex)
{
    u8 i;
    delay_ms(10);//消抖延时
    for(i = 0; i<4; i++) {
        GPIO_SetBits(KPIN[row][i].GPIOX,KPIN[row][i].GPIO_Pin);//每个行置1
        if((GPIO_ReadInputDataBit(GPIOX, colPin) == 1)) //如果列也变了 行的值就知道了 为 i
        {
            GPIO_ResetBits(KPIN[row][i].GPIOX,KPIN[row][i].GPIO_Pin); //行恢复 置0
            return colIndex+i*4+1;//返回的数据 为1-16 对应4x4键盘的16个键
        }
        GPIO_ResetBits(KPIN[row][i].GPIOX,KPIN[row][i].GPIO_Pin);     //行恢复 置0
    }
    return 0;
}

u8 KEY_Value2Char(u8 key_value)
{
	switch(key_value)
	{
		case 1:
		case 2:
		case 3:
		{
			key_value = '0' + key_value;
			break;
		}
		case 5:
		case 6:
		case 7:
		{
			key_value--;
			key_value = '0' + key_value;
			break;
		}
		case 9:
		case 10:
		case 11:
		{
			key_value -= 2;
			key_value = '0' + key_value;
			break;
		}
		case 4:
		case 8:
		case 12:
		case 16:
		{
			key_value = 'A'+(key_value/4 - 1);
			break;
		}
		case 13:
		{
			key_value = '*';
			break;
		}
		case 14:
		{
			key_value = '0';
			break;
		}
		case 15:
		{
			key_value = '#';
			break;
		}
		default:
			break;
	}
	
	return key_value;
}

u8 Matrix_KEY_Scan(void)
{
    u8 i,keyvalue;

    for(i = 0; i<4; i++)
    {
        if(GPIO_ReadInputDataBit(KPIN[col][i].GPIOX,KPIN[col][i].GPIO_Pin) == 0)//检测列 列值为 i
        {
            delay_ms(10);
            if(GPIO_ReadInputDataBit(KPIN[col][i].GPIOX,KPIN[col][i].GPIO_Pin) == 0)//检测列 列值为 i
            {
                BEEP_ON;
                keyvalue = ReScan(KPIN[col][i].GPIOX,KPIN[col][i].GPIO_Pin,i);//检测行 取键值
                while(GPIO_ReadInputDataBit(KPIN[col][i].GPIOX,KPIN[col][i].GPIO_Pin) == 0);
                //	while((GPIO_ReadInputDataBit(KPIN[col][i].GPIOX,KPIN[col][i].GPIO_Pin) == 0)&&(j<MAX_Time))j++;//松手检测
                keyvalue = KEY_Value2Char(keyvalue);
                BEEP_OFF;
                return keyvalue;//返回键值
            }
            
        }
    }
    return 0;
}
