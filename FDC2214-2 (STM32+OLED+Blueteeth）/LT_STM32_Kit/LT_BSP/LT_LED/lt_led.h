#ifndef __LT_LED_H
#define	__LT_LED_H


#include "stm32f10x.h"

/********************************************��ֲ�޶��㿪ʼ*****************************************************/
/* ����LED1���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�LED���� */
#define LED1_GPIO_PORT    	GPIOE		                /* GPIO�˿� */
#define LED1_GPIO_CLK 	    RCC_APB2Periph_GPIOE		/* GPIO�˿�ʱ�� */
#define LED1_GPIO_PIN		    GPIO_Pin_2		
#define EN485_pin           GPIO_Pin_9
#define EN485_GPIO_PORT     GPIOE
#define EN485_GPIO_CLK 	    RCC_APB2Periph_GPIOE
/* ����LED2���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�LED���� */
#define LED2_GPIO_PORT    	GPIOB			              /* GPIO�˿� */
#define LED2_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define LED2_GPIO_PIN		GPIO_Pin_3		

/* ������ƽ*/
#define LED_TRI_LEVEL			0							/* 0:�͵�ƽ����   1:�ߵ�ƽ���� */
/********************************************��ֲ�޶������*****************************************************/


/** the macro definition to trigger the led on or off 
  * 1 - off
  *0 - on
  */
#if(LED_TRI_LEVEL == 0)
	#define ON  0
	#define OFF 1
#else
	#define ON  1
	#define OFF 0
#endif

/* ʹ�ñ�׼�Ĺ̼������IO*/
#define LED1(a)	if (a)	\
					GPIO_SetBits(LED1_GPIO_PORT,LED1_GPIO_PIN);\
					else		\
					GPIO_ResetBits(LED1_GPIO_PORT,LED1_GPIO_PIN)

#define LED2(a)	if (a)	\
					GPIO_SetBits(LED2_GPIO_PORT,LED2_GPIO_PIN);\
					else		\
					GPIO_ResetBits(LED2_GPIO_PORT,LED2_GPIO_PIN)




/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)		{p->BSRR=i;}	 //���Ϊ�ߵ�ƽ		
#define digitalLo(p,i)		{p->BRR=i;}	 //����͵�ƽ
#define digitalToggle(p,i) 	{p->ODR ^=i;} //�����ת״̬


/* �������IO�ĺ� */
#define LED1_TOGGLE		 	digitalToggle(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED2_TOGGLE		 	digitalToggle(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define EN485_TX        digitalHi(EN485_GPIO_PORT,EN485_pin)
#if(LED_TRI_LEVEL == 0) 
	#define LED1_OFF		digitalHi(LED1_GPIO_PORT,LED1_GPIO_PIN)
	#define LED1_ON			digitalLo(LED1_GPIO_PORT,LED1_GPIO_PIN)
#else
	#define LED1_OFF		digitalLo(LED1_GPIO_PORT,LED1_GPIO_PIN)
	#define LED1_ON			digitalHi(LED1_GPIO_PORT,LED1_GPIO_PIN)
#endif


//#define LED2_TOGGLE		 digitalToggle(LED2_GPIO_PORT,LED2_GPIO_PIN)
//#define LED2_OFF		   digitalHi(LED2_GPIO_PORT,LED2_GPIO_PIN)
//#define LED2_ON			   digitalLo(LED2_GPIO_PORT,LED2_GPIO_PIN)



void LED_GPIO_Config(void);

#endif /* __LED_H */
