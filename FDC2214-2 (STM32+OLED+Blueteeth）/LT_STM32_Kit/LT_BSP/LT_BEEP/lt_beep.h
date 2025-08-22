#ifndef __LT_BEEP_H
#define	__LT_BEEP_H


#include "stm32f10x.h"

/********************************************��ֲ�޶��㿪ʼ*****************************************************/
/* ������������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�LED���� */
#define BEEP_GPIO_PORT    	GPIOB		              	/* GPIO�˿� */
#define BEEP_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define BEEP_GPIO_PIN		GPIO_Pin_15			        		        

/* ������ƽ*/
#define BEEP_TRI_LEVEL		0							/* 0:�͵�ƽ����   1:�ߵ�ƽ���� */
/********************************************��ֲ�޶������*****************************************************/

/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
//#if(BEEP_TRI_LEVEL == 0)
//	#define BEEP_ON  0
//	#define BEEP_OFF 1
//#else
//	#define BEEP_ON  1
//	#define BEEP_OFF 0
//#endif

/* ʹ�ñ�׼�Ĺ̼������IO*/
#define BEEP(a)	if (a)	\
					GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);\
					else		\
					GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN)


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)		{p->BSRR=i;}	 //���Ϊ�ߵ�ƽ		
#define digitalLo(p,i)		{p->BRR=i;}	 //����͵�ƽ
#define digitalToggle(p,i) 	{p->ODR ^=i;} //�����ת״̬


/* �������IO�ĺ� */
#define BEEP_TOGGLE		 		digitalToggle(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#if(BEEP_TRI_LEVEL == 0)
	#define BEEP_OFF		   	digitalHi(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
	#define BEEP_ON			   	digitalLo(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#else
	#define BEEP_OFF		   	digitalLo(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
	#define BEEP_ON			   	digitalHi(BEEP_GPIO_PORT,BEEP_GPIO_PIN)
#endif

void BEEP_GPIO_Config(void);

#endif /* __LED_H */
