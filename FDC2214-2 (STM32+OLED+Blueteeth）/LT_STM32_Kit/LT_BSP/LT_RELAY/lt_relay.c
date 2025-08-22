/**
  ******************************************************************************
  * @file    lt_beep.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-9-2
  * @brief   继电器应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:零涂LT_STM32_Kit开发板 
  * 管脚    :RELAY: B6
  * 触发方式:低电平
  * 使用实例:
			 RELAY_GPIO_Config();          //初始化继电器 GPIO
			 RELAY_TOGGLE;                //翻转继电器电平
			 RELAY_OFF;					 //关闭继电器
			 RELAY_ON;                    //打开继电器
  * 微信    :lingtu15679003191
  * 扣扣    :1372090604
  *
  ******************************************************************************
  */
  
#include "lt_relay.h"   

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void RELAY_GPIO_Config(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启LED相关的GPIO外设时钟*/
	RCC_APB2PeriphClockCmd( RELAY_GPIO_CLK, ENABLE);
	/*选择要控制的GPIO引脚*/
	GPIO_InitStructure.GPIO_Pin = RELAY_GPIO_PIN;	

	/*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*调用库函数，初始化GPIO*/
	GPIO_Init(RELAY_GPIO_PORT, &GPIO_InitStructure);		

	/* 关闭BEEP	*/
	RELAY_OFF;
}

/*********************************************END OF FILE**********************/
