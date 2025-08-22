/**
  ******************************************************************************
  * @file    lt_ir_switch.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-9-2
  * @brief   ���⿪��Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:��ͿLT_STM32_Kit������ 
  * �ܽ�    :PA12  
  * ������ʽ:�͵�ƽ����
  * ʹ��ʾ��:
			IR_SW_GPIO_Config();
			if(Read_IR_SW_Status() == IR_SW_ON)
			{
				printf("�赲\r\n");
			}
			else
			{
				printf("û���赲\r\n");
			}
  * ΢��    :lingtu15679003191
  * �ۿ�    :1372090604
  *
  ******************************************************************************
  */
#include "delay.h"  
#include "lt_ir_switch.h"  

/**
  * @brief  ���ú��⿪�ع��õ���I/O��
  * @param  ��
  * @retval ��
  */
void IR_SW_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*���������˿ڵ�ʱ��*/
	RCC_APB2PeriphClockCmd(IR_SW_GPIO_CLK, ENABLE);
	
	#if(IR_SW_TRI_LEVEL == 0)
		//���ú��⿪�عܵ�����Ϊ��������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	#else
		//���ú��⿪�عܵ�����Ϊ��������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	#endif

	//ѡ����⿪�عܵ�����
	GPIO_InitStructure.GPIO_Pin = IR_SW_GPIO_PIN; 
	//ʹ�ýṹ���ʼ�����⿪�ع�
	GPIO_Init(IR_SW_GPIO_PORT, &GPIO_InitStructure);
}

 /*
 * ��������Read_IR_SW_Status
 * ����  ����ȡ���⿪�عܵ�״̬
 * ����  ��GPIOx��x ������ A��B��C��D���� E
 *		     GPIO_Pin������ȡ�Ķ˿�λ 	
 * ���  ��IR_SW_OFF(�����û�б��赲)��IR_SW_ON������ܱ��赲��
 */
uint8_t Read_IR_SW_Status(void)
{			
	/*��������Ƿ��赲 */
	if(GPIO_ReadInputDataBit(IR_SW_GPIO_PORT,IR_SW_GPIO_PIN) == IR_SW_ON )  
	{	 
		delay_ms(10);
		if(GPIO_ReadInputDataBit(IR_SW_GPIO_PORT,IR_SW_GPIO_PIN) == IR_SW_ON )  
		{
			return IR_SW_ON;
		}  
		return IR_SW_OFF;	 
	}
	else
		return IR_SW_OFF;
}
/*********************************************END OF FILE**********************/
