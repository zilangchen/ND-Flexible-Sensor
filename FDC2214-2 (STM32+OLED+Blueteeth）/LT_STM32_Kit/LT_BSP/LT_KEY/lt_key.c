/**
  ******************************************************************************
  * @file    lt_key.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-9-2
  * @brief   ����Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:��ͿLT_STM32_Kit������ 
  * �ܽ�    :KEY1:PA0   KEY2:PB8    KEY3:PB9   
  * ������ʽ:�͵�ƽ����
  * ʹ��ʾ��:
			 KEY_GPIO_Config();		//��ʼ�������ܽ�
			 // ���KEY1
			 if(Single_Key_Scan(KEY1_GPIO_PORT, KEY1_GPIO_PIN) == KEY_ON)
			 {
				 printf("KEY1 press!\r\n");
			 }
			 // ���KEY2
			 if(Single_Key_Scan(KEY2_GPIO_PORT, KEY2_GPIO_PIN) == KEY_ON)
			 { 
				 printf("KEY2 press!\r\n");
			 }
			 // ���KEY3
			 if(Single_Key_Scan(KEY3_GPIO_PORT, KEY3_GPIO_PIN) == KEY_ON)
			 {
				 printf("KEY3 press!\r\n");
			 }
  * ΢��    :lingtu15679003191
  * �ۿ�    :1372090604
  *
  ******************************************************************************
  */
#include "delay.h"  
#include "lt_key.h"  

/**
  * @brief  ���ð����õ���I/O��
  * @param  ��
  * @retval ��
  */
void KEY_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*���������˿ڵ�ʱ��*/
	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK|KEY3_GPIO_CLK|KEY4_GPIO_CLK|ESP_GPIO_CLK,ENABLE);
	
	#if(KEY_TRI_LEVEL == 0)
		//���ð���������Ϊ��������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	#else
		//���ð���������Ϊ��������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	#endif
 

#if (KEY_NUM >= 1)
	/************************KEY1**************************/
	//ѡ�񰴼�������
	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN; 
	//ʹ�ýṹ���ʼ������
	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
#endif

#if (KEY_NUM >= 2)
	/************************KEY2**************************/
	//ѡ�񰴼�������
	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN; 
	//ʹ�ýṹ���ʼ������
	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);	
#endif

#if (KEY_NUM >= 3)
	/************************KEY3**************************/
	//ѡ�񰴼�������
	GPIO_InitStructure.GPIO_Pin = KEY3_GPIO_PIN;  
	//ʹ�ýṹ���ʼ������
	GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure);	
#endif

#if (KEY_NUM >= 4)    
    /************************KEY4**************************/
	//ѡ�񰴼�������
	GPIO_InitStructure.GPIO_Pin = KEY4_GPIO_PIN;  
	//ʹ�ýṹ���ʼ������
	GPIO_Init(KEY3_GPIO_PORT, &GPIO_InitStructure);
#endif

#if ESP_ENABLE  
    GPIO_InitStructure.GPIO_Pin = ESP_GPIO_PIN;  
	//ʹ�ýṹ���ʼ������
	GPIO_Init(ESP_GPIO_PORT, &GPIO_InitStructure);
#endif
}

 /*
 * ��������Single_Key_Scan
 * ����  ����ⵥ���Ƿ��а�������
 * ����  ��GPIOx��x ������ A��B��C��D���� E
 *		     GPIO_Pin������ȡ�Ķ˿�λ 	
 * ���  ��KEY_OFF(û���°���)��KEY_ON�����°�����
 */
uint8_t Single_Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
{			
	/*����Ƿ��а������� */
	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
	{	 
		delay_ms(10);
		if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )
		{
			#if (KEY_LONG_PRESS == 0)
			/*�ȴ������ͷ� */
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
	static u8 key_up=1;//�������ɿ���־
	if(mode)key_up=1;  //֧������ 
    
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
		delay_ms(10);//ȥ���� 
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
 	return 0;// �ް�������
}
/*********************************************END OF FILE**********************/
