/**
  ******************************************************************************
  * @file    lt_led.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-9-2
  * @brief   led应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:零涂LT_STM32_Kit开发板 
  * 管脚    :LED1: C13
  * 触发方式:低电平
  * 使用实例:
			 LED_GPIO_Config();          //初始化LED GPIO
			 LED1_TOGGLE;                //翻转LED1电平
			 LED1_OFF;					 //关闭LED1
			 LED1_ON;                    //打开LED1
  * 微信    :lingtu15679003191
  * 扣扣    :1372090604
  *
  ******************************************************************************
  */
  
#include "lt_led.h"   

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void LED_GPIO_Config(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启LED相关的GPIO外设时钟*/
	RCC_APB2PeriphClockCmd( LED1_GPIO_CLK, ENABLE);
	/*选择要控制的GPIO引脚*/
	GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN|EN485_pin ;	

	/*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*调用库函数，初始化GPIO*/
	GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);	
	
    RCC_APB2PeriphClockCmd( LED2_GPIO_CLK, ENABLE);
    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;

    /*调用库函数，初始化GPIO*/
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);		

	/* 关闭LED1灯	*/
	EN485_TX;
	LED1_OFF;
	LED2(0);
	/* 关闭LED2灯	*/
//	LED2_OFF;
}

/*********************************************END OF FILE**********************/
