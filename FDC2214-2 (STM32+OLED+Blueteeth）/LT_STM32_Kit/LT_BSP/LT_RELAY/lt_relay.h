#ifndef __LT_RELAY_H
#define	__LT_RELAY_H


#include "stm32f10x.h"

/********************************************��ֲ�޶��㿪ʼ*****************************************************/
/* ����̵������ӵ�GPIO�˿� */
#define RELAY_GPIO_PORT    	GPIOB		              	/* GPIO�˿� */
#define RELAY_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define RELAY_GPIO_PIN		GPIO_Pin_6	

/* ������ƽ*/
#define RELAY_TRI_LEVEL		0							/* 0:�͵�ƽ����   1:�ߵ�ƽ���� */
/********************************************��ֲ�޶������*****************************************************/

/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */

#if(RELAY_TRI_LEVEL == 0)
	#define LED_ON  0
	#define LED_OFF 1
#else
	#define LED_ON  1
	#define LED_OFF 0
#endif

/* ʹ�ñ�׼�Ĺ̼������IO*/
#define RELAY(a)	if (a)	\
					GPIO_SetBits(RELAY_GPIO_PORT,RELAY_GPIO_PIN);\
					else		\
					GPIO_ResetBits(RELAY_GPIO_PORT,RELAY_GPIO_PIN)


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)		{p->BSRR=i;}	 //���Ϊ�ߵ�ƽ		
#define digitalLo(p,i)		{p->BRR=i;}	 	 //����͵�ƽ
#define digitalToggle(p,i) 	{p->ODR ^=i;}    //�����ת״̬


/* �������IO�ĺ� */
#define RELAY_TOGGLE		 	digitalToggle(RELAY_GPIO_PORT,RELAY_GPIO_PIN)
#if(RELAY_TRI_LEVEL == 0)
	#define RELAY_OFF		   	digitalHi(RELAY_GPIO_PORT,RELAY_GPIO_PIN)
	#define RELAY_ON			digitalLo(RELAY_GPIO_PORT,RELAY_GPIO_PIN)
#else
	#define RELAY_OFF		   	digitalLo(RELAY_GPIO_PORT,RELAY_GPIO_PIN)
	#define RELAY_ON			digitalHi(RELAY_GPIO_PORT,RELAY_GPIO_PIN)
#endif

void RELAY_GPIO_Config(void);

#endif /* __LED_H */
