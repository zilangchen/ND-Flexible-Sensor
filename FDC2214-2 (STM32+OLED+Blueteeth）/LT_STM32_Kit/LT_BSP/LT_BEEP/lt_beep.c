/**
  ******************************************************************************
  * @file    lt_beep.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-9-2
  * @brief   ������Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:��ͿLT_STM32_Kit������ 
  * �ܽ�    :BEEP: B7
  * ������ʽ:�͵�ƽ
  * ʹ��ʵ��:
			 BEEP_GPIO_Config();          //��ʼ��BEEP GPIO
			 BEEP_TOGGLE;                //��תBEEP��ƽ
			 BEEP_OFF;					 //�ر�BEEP
			 BEEP_ON;                    //��BEEP
  * ΢��    :lingtu15679003191
  * �ۿ�    :1372090604
  *
  ******************************************************************************
  */
  
#include "lt_beep.h"   

 /**
  * @brief  ��ʼ������LED��IO
  * @param  ��
  * @retval ��
  */
void BEEP_GPIO_Config(void)
{
	/*����һ��GPIO_InitTypeDef���͵Ľṹ��*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*����LED��ص�GPIO����ʱ��*/
	RCC_APB2PeriphClockCmd( BEEP_GPIO_CLK, ENABLE);
	/*ѡ��Ҫ���Ƶ�GPIO����*/
	GPIO_InitStructure.GPIO_Pin = BEEP_GPIO_PIN;	

	/*��������ģʽΪͨ���������*/
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

	/*������������Ϊ50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*���ÿ⺯������ʼ��GPIO*/
	GPIO_Init(BEEP_GPIO_PORT, &GPIO_InitStructure);		

	/* �ر�BEEP	*/
	BEEP_OFF;
}

/*********************************************END OF FILE**********************/
