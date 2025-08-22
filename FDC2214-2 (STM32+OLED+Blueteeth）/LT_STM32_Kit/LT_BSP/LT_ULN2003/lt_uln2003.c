/**
  ******************************************************************************
  * @file    lt_uln2003.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-3-1
  * @brief   uln2003Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:��ͿLT_STM32_Kit������ 
  * �ܽ�    :
            IN1:  A4
            IN1:  A5
            IN1:  A6
            IN1:  A7
            
            VCC: 5V
            GND: GND
  * ������ʽ:�͵�ƽ
  * ʹ��ʵ��:
			enum dir{Pos,Neg};
             
            ULN2003_GPIO_Config();
             
            while(1)
            {
                int i;
                for(i = 0;i < 1000;i++)      //�����ת
                    stepper(Pos,8000);     
                SysTick_Delay_Ms(500000);    //��ʱ
                for(i = 0;i < 1000;i++)      //�����ת
                    stepper(Neg,8000); 
                SysTick_Delay_Ms(500000);    //��ʱ
            }

  * ΢��    :lingtu15679003191
  * �ۿ�    :1372090604
  *
  ******************************************************************************
  */
  
#include "lt_uln2003.h"   

 /**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void ULN2003_GPIO_Config(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
    GPIO_InitTypeDef GPIO_InitStructure;

    /*����������ص�GPIO����ʱ��*/
    RCC_APB2PeriphClockCmd( IN1_GPIO_CLK | IN2_GPIO_CLK | IN3_GPIO_CLK | IN4_GPIO_CLK , ENABLE);
		
	 /*ѡ��Ҫ���Ƶ�GPIO����*/
    GPIO_InitStructure.GPIO_Pin = IN1_GPIO_PIN;	
    /*��������ģʽΪͨ���������*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   
    /*������������Ϊ50MHz */   
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
    /*���ÿ⺯������ʼ��GPIO*/
    GPIO_Init(IN1_GPIO_PORT, &GPIO_InitStructure);	
    
    /*ѡ��Ҫ���Ƶ�GPIO����*/
    GPIO_InitStructure.GPIO_Pin = IN2_GPIO_PIN;
    /*���ÿ⺯������ʼ��GPIO*/
    GPIO_Init(IN2_GPIO_PORT, &GPIO_InitStructure);
    
    /*ѡ��Ҫ���Ƶ�GPIO����*/
    GPIO_InitStructure.GPIO_Pin = IN3_GPIO_PIN;
    /*���ÿ⺯������ʼ��GPIOF*/
    GPIO_Init(IN3_GPIO_PORT, &GPIO_InitStructure);
    
    /*ѡ��Ҫ���Ƶ�GPIO����*/
    GPIO_InitStructure.GPIO_Pin = IN4_GPIO_PIN;
    /*���ÿ⺯������ʼ��GPIOF*/
    GPIO_Init(IN4_GPIO_PORT, &GPIO_InitStructure);
    
    //�������ų�ʼ��Ϊ�͵�ƽ
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
