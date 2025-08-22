/**
  ******************************************************************************
  * @file    lt_led.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-9-2
  * @brief   ledӦ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:��ͿLT_STM32_Kit������ 
  * �ܽ�    :LED1: C13
  * ������ʽ:�͵�ƽ
  * ʹ��ʵ��:
			 LED_GPIO_Config();          //��ʼ��LED GPIO
			 LED1_TOGGLE;                //��תLED1��ƽ
			 LED1_OFF;					 //�ر�LED1
			 LED1_ON;                    //��LED1
  * ΢��    :lingtu15679003191
  * �ۿ�    :1372090604
  *
  ******************************************************************************
  */
  
#include "lt_led.h"   

 /**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void LED_GPIO_Config(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LED��ص�GPIO����ʱ��*/
	RCC_APB2PeriphClockCmd( LED1_GPIO_CLK, ENABLE);
	/*ѡ��Ҫ���Ƶ�GPIO����*/
	GPIO_InitStructure.GPIO_Pin = LED1_GPIO_PIN|EN485_pin ;	

	/*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*������������Ϊ50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*���ÿ⺯������ʼ��GPIO*/
	GPIO_Init(LED1_GPIO_PORT, &GPIO_InitStructure);	
	
    RCC_APB2PeriphClockCmd( LED2_GPIO_CLK, ENABLE);
    /*ѡ��Ҫ���Ƶ�GPIO����*/
    GPIO_InitStructure.GPIO_Pin = LED2_GPIO_PIN;

    /*���ÿ⺯������ʼ��GPIO*/
    GPIO_Init(LED2_GPIO_PORT, &GPIO_InitStructure);		

	/* �ر�LED1��	*/
	EN485_TX;
	LED1_OFF;
	LED2(0);
	/* �ر�LED2��	*/
//	LED2_OFF;
}

/*********************************************END OF FILE**********************/
