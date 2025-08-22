#ifndef __LT_DS18B20_H
#define __LT_DS18B20_H 
#include "sys.h"

/********************************************��ֲ�޶��㿪ʼ*****************************************************/
/* ����LED1���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�LED���� */
#define DS18B20_DQ_GPIO_PORT    	GPIOB		                /* GPIO�˿� */
#define DS18B20_DQ_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define DS18B20_DQ_GPIO_PIN		    GPIO_Pin_11			        
/********************************************��ֲ�޶������*****************************************************/


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)		{p->BSRR=i;}	 //���Ϊ�ߵ�ƽ		
#define digitalLo(p,i)		{p->BRR=i;}	 //����͵�ƽ

#define DS18B20_DQ_OUT_ON	digitalHi(DS18B20_DQ_GPIO_PORT,DS18B20_DQ_GPIO_PIN)
#define DS18B20_DQ_OUT_OFF	digitalLo(DS18B20_DQ_GPIO_PORT,DS18B20_DQ_GPIO_PIN)
#define DS18B20_DQ_IN	    GPIO_ReadInputDataBit(DS18B20_DQ_GPIO_PORT,DS18B20_DQ_GPIO_PIN)
   	
u8 DS18B20_GPIO_Config(void);//��ʼ��DS18B20
short DS18B20_Get_Temp(void);//��ȡ�¶�
void DS18B20_Start(void);//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);//����һ���ֽ�
u8 DS18B20_Read_Bit(void);//����һ��λ
u8 DS18B20_Check(void);//����Ƿ����DS18B20
void DS18B20_Rst(void);//��λDS18B20    
void DS18B20_IO_IN(void);
void DS18B20_IO_OUT(void);
#endif















