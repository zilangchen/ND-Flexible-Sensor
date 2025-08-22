#ifndef __LT_IR_MAN_H
#define	__LT_IR_MAN_H


#include "stm32f10x.h"

/********************************************��ֲ�޶��㿪ʼ*****************************************************/
/* ��������������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶĺ��⿪������ */
#define    IR_MAN_GPIO_CLK     RCC_APB2Periph_GPIOA
#define    IR_MAN_GPIO_PORT    GPIOA			   
#define    IR_MAN_GPIO_PIN	   GPIO_Pin_1

/* ������ƽ*/
#define    IR_MAN_TRI_LEVEL	  1							/* 0:�͵�ƽ����   1:�ߵ�ƽ���� */
/********************************************��ֲ�޶������*****************************************************/

 /** �������±��ú�
	*  ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
	*  ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
	*/
#if(IR_MAN_TRI_LEVEL == 0)
	#define IR_MAN_ON	0
	#define IR_MAN_OFF	1
#else
	#define IR_MAN_ON	1
	#define IR_MAN_OFF	0
#endif

void 	IR_MAN_GPIO_Config(void);
uint8_t Read_IR_MAN_Status(void);

#endif /* __KEY_H */

