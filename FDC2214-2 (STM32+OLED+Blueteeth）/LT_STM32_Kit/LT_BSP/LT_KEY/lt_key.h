#ifndef __LT_KEY_H
#define	__LT_KEY_H


#include "stm32f10x.h"

/********************************************��ֲ�޶��㿪ʼ*****************************************************/
/* ����KEY���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�KEY���� */
#define    KEY_NUM           1
#define    KEY1_GPIO_CLK     RCC_APB2Periph_GPIOB
#define    KEY1_GPIO_PORT    GPIOB			   
#define    KEY1_GPIO_PIN	 GPIO_Pin_7

#define    KEY2_GPIO_CLK     RCC_APB2Periph_GPIOB
#define    KEY2_GPIO_PORT    GPIOB		   
#define    KEY2_GPIO_PIN	 GPIO_Pin_8

#define    KEY3_GPIO_CLK     RCC_APB2Periph_GPIOB
#define    KEY3_GPIO_PORT    GPIOB		   
#define    KEY3_GPIO_PIN	 GPIO_Pin_9

#define    KEY4_GPIO_CLK     RCC_APB2Periph_GPIOB
#define    KEY4_GPIO_PORT    GPIOB		   
#define    KEY4_GPIO_PIN	 GPIO_Pin_6

#define    ESP_GPIO_CLK     RCC_APB2Periph_GPIOB
#define    ESP_GPIO_PORT    GPIOB		   
#define    ESP_GPIO_PIN	    GPIO_Pin_13

/* ������ƽ*/
#define    KEY_TRI_LEVEL	 0							/* 0:�͵�ƽ����   1:�ߵ�ƽ���� */
/* �Ƿ�֧�ֳ���*/
#define    KEY_LONG_PRESS	 0							/* 0:��֧��       1:֧��       */

#define    ESP_ENABLE        1
/********************************************��ֲ�޶������*****************************************************/

#define KEY1  GPIO_ReadInputDataBit(KEY1_GPIO_PORT,KEY1_GPIO_PIN)//��ȡ����1
#define KEY2  GPIO_ReadInputDataBit(KEY2_GPIO_PORT,KEY2_GPIO_PIN)//��ȡ����2
#define KEY3  GPIO_ReadInputDataBit(KEY3_GPIO_PORT,KEY3_GPIO_PIN)//��ȡ����3
#define KEY4  GPIO_ReadInputDataBit(KEY4_GPIO_PORT,KEY4_GPIO_PIN)//��ȡ����4

#define ESP   GPIO_ReadInputDataBit(ESP_GPIO_PORT,ESP_GPIO_PIN)//��ȡ����4

#define KEY1_PRES 	    1	//KEY0����
#define KEY2_PRES	    2	//KEY1����
#define KEY3_PRES       3	//KEY2����
#define KEY4_PRES       4	//KEY2����

 /** �������±��ú�
	*  ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
	*  ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
	*/
#if(KEY_TRI_LEVEL == 0)
	#define KEY_ON	0
	#define KEY_OFF	1
#else
	#define KEY_ON	1
	#define KEY_OFF	0
#endif

void KEY_GPIO_Config(void);
uint8_t Single_Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);
uint8_t Group_Key_Scan(uint8_t mode);


#endif /* __KEY_H */

