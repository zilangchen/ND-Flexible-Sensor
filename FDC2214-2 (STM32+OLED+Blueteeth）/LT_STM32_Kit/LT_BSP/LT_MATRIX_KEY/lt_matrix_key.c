#include "lt_matrix_key.h"
#include "lt_beep.h"
#include "delay.h"

/**
  ******************************************************************************
  * @file    lt_matrix_key.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-12-13
  * @brief   ���󰴼�Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:��ͿLT_STM32_Kit������ 
  * �ܽ�    :�У�C1��C3��A1��A5
             ��: A7��C5��B1��F11
  * ʹ��ʾ��:
             delay_init();	    	 //��ʱ������ʼ��	  
             NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
             uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
             Matrix_KEY_GPIO_Config();
             
             while(1)
             {	
                KeyNum = Matrix_KEY_Scan();
                if(KeyNum != 0)
                    printf("%c\r\n", KEY_Value2Char(KeyNum));
                
                delay_ms(10);
             }	
  * ΢��    :lingtu15679003191
  * �ۿ�    :1372090604
  *
  ******************************************************************************
  */

/********************************************��ֲ�޶��㿪ʼ*****************************************************/
/* ֻ��Ҫ��������д�����е�IO�ڡ��ܽź�ʱ�Ӷ˿ڣ�IO�ں͹ܽŶ��������ⶨ�� ������һ��IO�� �ܽ�Ҳ���谴˳�� */
KGPT KPIN[2][4]= { 
    {{GPIOB,GPIO_Pin_11, RCC_APB2Periph_GPIOB},{GPIOB,GPIO_Pin_10,RCC_APB2Periph_GPIOB}, {GPIOA,GPIO_Pin_2,RCC_APB2Periph_GPIOB},{GPIOB,GPIO_Pin_1,RCC_APB2Periph_GPIOB}}, //row ����
    {{GPIOB,GPIO_Pin_0,RCC_APB2Periph_GPIOB}, {GPIOA,GPIO_Pin_7,RCC_APB2Periph_GPIOA},  {GPIOA,GPIO_Pin_6,RCC_APB2Periph_GPIOA}, {GPIOA,GPIO_Pin_5,RCC_APB2Periph_GPIOA}}  //col ����
};
/********************************************��ֲ�޶������*****************************************************/
//������ʼ������
void Matrix_KEY_GPIO_Config(void)
{
    u8 i;
    GPIO_InitTypeDef GPIO_InitStructure;
    // RCC_APB2PeriphClockCmd(RCC_APB2_GPIOX,ENABLE);//ʹ��PORTCʱ��
	
    for(i=0; i<4; i++)
    {
        RCC_APB2PeriphClockCmd(KPIN[row][i].RCC_APB2_GPIOX,ENABLE);//ʹ��PORTCʱ��
        GPIO_InitStructure.GPIO_Pin = KPIN[row][i].GPIO_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //���ó��������
        GPIO_Init(KPIN[row][i].GPIOX, &GPIO_InitStructure);//��ʼ��row
        GPIO_ResetBits(KPIN[row][i].GPIOX,KPIN[row][i].GPIO_Pin);
    }
	
    for(i=0; i<4; i++)
    {
        RCC_APB2PeriphClockCmd(KPIN[col][i].RCC_APB2_GPIOX,ENABLE);//ʹ��PORTCʱ��
        GPIO_InitStructure.GPIO_Pin  = KPIN[col][i].GPIO_Pin;//PC5
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //���ó���������
        GPIO_Init(KPIN[col][i].GPIOX, &GPIO_InitStructure);//��ʼ��col
    }
}

u8 ReScan(GPIO_TypeDef* GPIOX,uint16_t colPin,u8 colIndex)
{
    u8 i;
    delay_ms(10);//������ʱ
    for(i = 0; i<4; i++) {
        GPIO_SetBits(KPIN[row][i].GPIOX,KPIN[row][i].GPIO_Pin);//ÿ������1
        if((GPIO_ReadInputDataBit(GPIOX, colPin) == 1)) //�����Ҳ���� �е�ֵ��֪���� Ϊ i
        {
            GPIO_ResetBits(KPIN[row][i].GPIOX,KPIN[row][i].GPIO_Pin); //�лָ� ��0
            return colIndex+i*4+1;//���ص����� Ϊ1-16 ��Ӧ4x4���̵�16����
        }
        GPIO_ResetBits(KPIN[row][i].GPIOX,KPIN[row][i].GPIO_Pin);     //�лָ� ��0
    }
    return 0;
}

u8 KEY_Value2Char(u8 key_value)
{
	switch(key_value)
	{
		case 1:
		case 2:
		case 3:
		{
			key_value = '0' + key_value;
			break;
		}
		case 5:
		case 6:
		case 7:
		{
			key_value--;
			key_value = '0' + key_value;
			break;
		}
		case 9:
		case 10:
		case 11:
		{
			key_value -= 2;
			key_value = '0' + key_value;
			break;
		}
		case 4:
		case 8:
		case 12:
		case 16:
		{
			key_value = 'A'+(key_value/4 - 1);
			break;
		}
		case 13:
		{
			key_value = '*';
			break;
		}
		case 14:
		{
			key_value = '0';
			break;
		}
		case 15:
		{
			key_value = '#';
			break;
		}
		default:
			break;
	}
	
	return key_value;
}

u8 Matrix_KEY_Scan(void)
{
    u8 i,keyvalue;

    for(i = 0; i<4; i++)
    {
        if(GPIO_ReadInputDataBit(KPIN[col][i].GPIOX,KPIN[col][i].GPIO_Pin) == 0)//����� ��ֵΪ i
        {
            delay_ms(10);
            if(GPIO_ReadInputDataBit(KPIN[col][i].GPIOX,KPIN[col][i].GPIO_Pin) == 0)//����� ��ֵΪ i
            {
                BEEP_ON;
                keyvalue = ReScan(KPIN[col][i].GPIOX,KPIN[col][i].GPIO_Pin,i);//����� ȡ��ֵ
                while(GPIO_ReadInputDataBit(KPIN[col][i].GPIOX,KPIN[col][i].GPIO_Pin) == 0);
                //	while((GPIO_ReadInputDataBit(KPIN[col][i].GPIOX,KPIN[col][i].GPIO_Pin) == 0)&&(j<MAX_Time))j++;//���ּ��
                keyvalue = KEY_Value2Char(keyvalue);
                BEEP_OFF;
                return keyvalue;//���ؼ�ֵ
            }
            
        }
    }
    return 0;
}
