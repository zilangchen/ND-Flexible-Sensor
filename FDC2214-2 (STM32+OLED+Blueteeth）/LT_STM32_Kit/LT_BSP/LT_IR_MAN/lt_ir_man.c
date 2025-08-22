/**
  ******************************************************************************
  * @file    lt_ir_main.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-9-2
  * @brief   �������Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:��ͿLT_STM32_Kit������ 
  * �ܽ�    :PA8  
  * ������ʽ:�͵�ƽ����
  * ʹ��ʾ��:
			IR_MAN_GPIO_Config();
			if(Read_IR_MAN_Status() == IR_MAN_ON)
			{
				printf("����\r\n");
			}
			else
			{
				printf("û��\r\n");
			}
  * ΢��    :lingtu15679003191
  * �ۿ�    :1372090604
  *
  ******************************************************************************
  */
#include "delay.h"  
#include "lt_ir_man.h"  

/**
  * @brief  ���ú��⿪�ع��õ���I/O��
  * @param  ��
  * @retval ��
  */
void IR_MAN_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*���������˿ڵ�ʱ��*/
	RCC_APB2PeriphClockCmd(IR_MAN_GPIO_CLK, ENABLE);
	
	#if(IR_MAN_TRI_LEVEL == 0)
		//���ú��⿪�عܵ�����Ϊ��������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	#else
		//���ú��⿪�عܵ�����Ϊ��������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	#endif

	//ѡ����⿪�عܵ�����
	GPIO_InitStructure.GPIO_Pin = IR_MAN_GPIO_PIN; 
	//ʹ�ýṹ���ʼ�����⿪�ع�
	GPIO_Init(IR_MAN_GPIO_PORT, &GPIO_InitStructure);
}

 /*
 * ��������Read_IR_MAN_Status
 * ����  ����ȡ��������״̬
 * ����  ��GPIOx��x ������ A��B��C��D���� E
 *		     GPIO_Pin������ȡ�Ķ˿�λ 	
 * ���  ��IR_MAN_OFF(û��)��IR_MAN_ON�����ˣ�
 */
uint8_t Read_IR_MAN_Status(void)
{			
	/*��������Ƿ��赲 */
	if(GPIO_ReadInputDataBit(IR_MAN_GPIO_PORT,IR_MAN_GPIO_PIN) == IR_MAN_ON )  
	{	 
		delay_ms(10);
		if(GPIO_ReadInputDataBit(IR_MAN_GPIO_PORT,IR_MAN_GPIO_PIN) == IR_MAN_ON )  
		{
			return IR_MAN_ON;
		}  
		return IR_MAN_OFF;	 
	}
	else
		return IR_MAN_OFF;
}
/*********************************************END OF FILE**********************/
