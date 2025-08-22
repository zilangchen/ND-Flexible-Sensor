#ifndef __MAX30102_H
#define __MAX30102_H
#include "sys.h"

/********************************************��ֲ�޶��㿪ʼ*****************************************************/
/* ����MAX30102���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�MAX30102���� */
#define MAX30102_SCL_GPIO_PORT    	GPIOB		                /* MAX30102 SCL�˿� */
#define MAX30102_SCL_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* MAX30102 SCL�˿�ʱ�� */
#define MAX30102_SCL_GPIO_PIN		GPIO_Pin_6			        /* MAX30102 SCL�ܽ� */

#define MAX30102_SDA_GPIO_PORT    	GPIOB		                /* MAX30102 SDA�˿� */
#define MAX30102_SDA_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* MAX30102 SDA�˿�ʱ�� */
#define MAX30102_SDA_GPIO_PIN		GPIO_Pin_7			        /* MAX30102 SDA�ܽ� */

#define IIC_SCL                     PBout(6)                    //SCL
#define IIC_SDA                     PBout(7)                    //SDA	 
#define READ_SDA                    PBin(7)                     //����SDA 

/********************************************��ֲ�޶������*****************************************************/


////IO��������

//#define SDA_IN()  {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)8<<28;}
//#define SDA_OUT() {GPIOB->CRL&=0X0FFFFFFF;GPIOB->CRL|=(u32)3<<28;}

////IO��������
//#define IIC_SCL    PBout(6) //SCL
//#define IIC_SDA    PBout(7) //SDA	 
//#define READ_SDA   PBin(7)  //����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
void IIC_Read_One_Byte(u8 daddr,u8 addr,u8* data);

void IIC_WriteBytes(u8 WriteAddr,u8* data,u8 dataLength);
void IIC_ReadBytes(u8 deviceAddr, u8 writeAddr,u8* data,u8 dataLength);
void SDA_IN(void);
void SDA_OUT(void);
#endif
















