/**
  ******************************************************************************
  * @file    lt_uln2003.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-3-1
  * @brief   uln2003应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:零涂LT_STM32_Kit开发板 
  * 管脚    :
            IN1:  A4
            IN1:  A5
            IN1:  A6
            IN1:  A7
            
            VCC: 5V
            GND: GND
  * 触发方式:低电平
  * 使用实例:
			enum dir{Pos,Neg};
             
            ULN2003_GPIO_Config();
             
            while(1)
            {
                int i;
                for(i = 0;i < 1000;i++)      //电机正转
                    stepper(Pos,8000);     
                SysTick_Delay_Ms(500000);    //延时
                for(i = 0;i < 1000;i++)      //电机反转
                    stepper(Neg,8000); 
                SysTick_Delay_Ms(500000);    //延时
            }

  * 微信    :lingtu15679003191
  * 扣扣    :1372090604
  *
  ******************************************************************************
  */
  
#include "lt_uln2003.h"   

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
void ULN2003_GPIO_Config(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*开启引脚相关的GPIO外设时钟*/
    RCC_APB2PeriphClockCmd( IN1_GPIO_CLK | IN2_GPIO_CLK | IN3_GPIO_CLK | IN4_GPIO_CLK , ENABLE);
		
	 /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Pin = IN1_GPIO_PIN;	
    /*设置引脚模式为通用推挽输出*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
    /*设置引脚速率为50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    /*调用库函数，初始化GPIO*/
    GPIO_Init(IN1_GPIO_PORT, &GPIO_InitStructure);	
    
    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Pin = IN2_GPIO_PIN;
    /*调用库函数，初始化GPIO*/
    GPIO_Init(IN2_GPIO_PORT, &GPIO_InitStructure);
    
    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Pin = IN3_GPIO_PIN;
    /*调用库函数，初始化GPIOF*/
    GPIO_Init(IN3_GPIO_PORT, &GPIO_InitStructure);
    
    /*选择要控制的GPIO引脚*/
    GPIO_InitStructure.GPIO_Pin = IN4_GPIO_PIN;
    /*调用库函数，初始化GPIOF*/
    GPIO_Init(IN4_GPIO_PORT, &GPIO_InitStructure);
    
    //所有引脚初始化为低电平
    IN1_LOW;
    IN2_LOW;
    IN3_LOW;
    IN4_LOW;
}

void SysTick_Delay_Ms(__IO uint32_t ms)
{
	uint32_t i;
	SysTick_Config(72000);
	
	for(i=0; i<ms ; i++)
	{
		while(!(SysTick->CTRL)&(1<<16))
			;
	}
	SysTick->CTRL &= SysTick_CTRL_ENABLE_Msk;
}

void stepper(uint8_t dir,int speed)
{
	if(dir == Pos)
	{
		IN1_HIGH;
		SysTick_Delay_Ms(speed);
		IN1_LOW;
		IN2_HIGH;
		SysTick_Delay_Ms(speed);
		IN2_LOW;
		IN3_HIGH;
		SysTick_Delay_Ms(speed);
		IN3_LOW;
		IN4_HIGH;
		SysTick_Delay_Ms(speed);
		IN4_LOW;
	}
	else
	{
		IN1_HIGH;
		SysTick_Delay_Ms(speed);
		IN1_LOW;
		IN4_HIGH;
		SysTick_Delay_Ms(speed);
		IN4_LOW;
		IN3_HIGH;
		SysTick_Delay_Ms(speed);
		IN3_LOW;
		IN2_HIGH;
		SysTick_Delay_Ms(speed);
		IN2_LOW;
	}
}

/*********************************************END OF FILE**********************/
