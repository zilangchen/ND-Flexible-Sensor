/**
  ******************************************************************************
  * @file    lt_key.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-9-2
  * @brief   按键应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:零涂LT_STM32_Kit开发板 
  * 管脚    :KEY1:PA0   KEY2:PB8    KEY3:PB9   
  * 触发方式:低电平触发
  * 使用示例:
			 KEY_GPIO_Config();		//初始化按键管脚
			 // 检测KEY1
			 if(Single_Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
			 {
				 printf("KEY1 press!\r\n");
			 }
			 // 检测KEY2
			 if(Single_Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
			 { 
				 printf("KEY2 press!\r\n");
			 }
			 // 检测KEY3
			 if(Single_Key_Scan(KEY3_GPIO_PORT, KEY3_GPIO_PIN) == KEY_ON)
			 {
				 printf("KEY3 press!\r\n");
			 }
  * 微信    :lingtu15679003191
  * 扣扣    :1372090604
  *
  ******************************************************************************
  */
#include "delay.h"  
#include "lt_key.h"  

/**
  * @brief  配置按键用到的I/O口
  * @param  无
  * @retval 无
  */
void KEY_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启按键端口的时钟*/
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK|KEY3_GPIO_CLK|KEY4_GPIO_CLK|ESP_GPIO_CLK,ENABLE);
	
	#if(KEY_TRI_LEVEL == 0)
		//设置按键的引脚为上拉输入
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	#else
		//设置按键的引脚为下拉输入
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	#endif
 

#if (KEY_NUM >= 1)
	/************************KEY1**************************/
	//选择按键的引脚
	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN; 
	//使用结构体初始化按键
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
#endif

#if (KEY_NUM >= 2)
	/************************KEY2**************************/
	//选择按键的引脚
	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN; 
	//使用结构体初始化按键
	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);	
#endif

#if (KEY_NUM >= 3)
	/************************KEY3**************************/
	//选择按键的引脚
	GPIO_InitStructure.GPIO_Pin = KEY3_GPIO_PIN;  
	//使用结构体初始化按键
	GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure);	
#endif

#if (KEY_NUM >= 4)    
    /************************KEY4**************************/
	//选择按键的引脚
	GPIO_InitStructure.GPIO_Pin = KEY4_GPIO_PIN;  
	//使用结构体初始化按键
	GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure);
#endif

#if ESP_ENABLE  
    GPIO_InitStructure.GPIO_Pin = ESP_GPIO_PIN;  
	//使用结构体初始化按键
	GPIO_Init(ESP_GPIO_PORT, &GPIO_InitStructure);
#endif
}

 /*
 * 函数名：Single_Key_Scan
 * 描述  ：检测单独是否有按键按下
 * 输入  ：GPIOx：x 可以是 A，B，C，D或者 E
 *		     GPIO_Pin：待读取的端口位 	
 * 输出  ：KEY_OFF(没按下按键)、KEY_ON（按下按键）
 */
uint8_t Single_Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	/*检测是否有按键按下 */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		delay_ms(10);
		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )
		{
			#if (KEY_LONG_PRESS == 0)
			/*等待按键释放 */
			while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);
			#endif
			return KEY_ON;
		}  
		return KEY_OFF;	 
	}
	else
		return KEY_OFF;
}

uint8_t Group_Key_Scan(uint8_t mode)
{	 
	static u8 key_up=1;//按键按松开标志
	if(mode)key_up=1;  //支持连按 
    
#if   (KEY_NUM == 1)
	if(key_up&&(KEY1==KEY_ON))
#elif (KEY_NUM == 2)
    if(key_up&&(KEY1==KEY_ON||KEY2==KEY_ON))
#elif (KEY_NUM == 3)
    if(key_up&&(KEY1==KEY_ON||KEY2==KEY_ON||KEY3==KEY_ON))
#elif (KEY_NUM == 4)
    if(key_up&&(KEY1==KEY_ON||KEY2==KEY_ON||KEY3==KEY_ON||KEY4==KEY_ON))
#endif       
	{
		delay_ms(10);//去抖动 
		key_up=0;
        #if (KEY_NUM >= 1)
		if(KEY1==KEY_ON)return KEY1_PRES;
        #endif
        #if (KEY_NUM >= 2)
		else if(KEY2==KEY_ON)return KEY2_PRES;
        #endif
        #if (KEY_NUM >= 3)
		else if(KEY3==KEY_ON)return KEY3_PRES;
        #endif
        #if (KEY_NUM >= 4)
        else if(KEY4==KEY_ON)return KEY4_PRES;
        #endif
	}
#if   (KEY_NUM == 1)
	else if(KEY1==KEY_OFF)key_up=1; 
#elif (KEY_NUM == 2)
    else if(KEY1==KEY_OFF&&KEY2==KEY_OFF)key_up=1; 
#elif (KEY_NUM == 3)
    else if(KEY1==KEY_OFF&&KEY2==KEY_OFF&&KEY3==KEY_OFF)key_up=1; 
#elif (KEY_NUM == 4)
    else if(KEY1==KEY_OFF&&KEY2==KEY_OFF&&KEY3==KEY_OFF&&KEY4==KEY_OFF)key_up=1; 
#endif	    
 	return 0;// 无按键按下
}
/*********************************************END OF FILE**********************/
