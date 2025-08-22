#ifndef __LT_FDC2214_IIC_H
#define __LT_FDC2214_IIC_H
#include "sys.h"

/********************************************��ֲ�޶��㿪ʼ*****************************************************/
/* ����FDC2214���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�OLED���� */
#define FDC2214_SCLK_GPIO_PORT    	GPIOB		                /* OLED SCLK�˿� */
#define FDC2214_SCLK_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* OLED SCLK�˿�ʱ�� */
#define FDC2214_SCLK_GPIO_PIN		GPIO_Pin_6			        /* OLED SCLK�ܽ� */

#define FDC2214_SDIN_GPIO_PORT    	GPIOB		                /* OLED SCLK�˿� */
#define FDC2214_SDIN_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* OLED SCLK�˿�ʱ�� */
#define FDC2214_SDIN_GPIO_PIN		GPIO_Pin_7		            /* OLED SCLK�ܽ� */

#define FDC2214_IIC_SCL             PBout(6)
#define FDC2214_IIC_SDA_OUT         PBout(7)
#define FDC2214_IIC_SDA_IN          PBin(7)
/********************************************��ֲ�޶������*****************************************************/

void FDC2214_IIC_Init(void);
void FDC2214_IIC_Start(void);
void FDC2214_IIC_Stop(void);
u8 FDC2214_IIC_Wait_Ack(void);
void FDC2214_IIC_Ack(void);
void FDC2214_IIC_NAck(void);
void FDC2214_IIC_Send_Byte(u8 dat);
u8 FDC2214_IIC_Read_Byte(u8 ack);

void delay_2us(void);
#endif
