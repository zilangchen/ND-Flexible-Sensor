#ifndef __LT_ULN2003_H
#define	__LT_ULN2003_H


#include "stm32f10x.h"

/********************************************��ֲ�޶��㿪ʼ*****************************************************/
/* ����UNL2003��INX���ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����Ĵ��뼴�ɸı���Ƶ�LED���� */
//IN1
#define IN1_GPIO_PORT     GPIOA
#define IN1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define IN1_GPIO_PIN      GPIO_Pin_4

//IN2
#define IN2_GPIO_PORT     GPIOA
#define IN2_GPIO_CLK      RCC_APB2Periph_GPIOA
#define IN2_GPIO_PIN      GPIO_Pin_5

//IN3
#define IN3_GPIO_PORT     GPIOA
#define IN3_GPIO_CLK      RCC_APB2Periph_GPIOA
#define IN3_GPIO_PIN      GPIO_Pin_6

//IN1
#define IN4_GPIO_PORT     GPIOA
#define IN4_GPIO_CLK      RCC_APB2Periph_GPIOA
#define IN4_GPIO_PIN      GPIO_Pin_7


/********************************************��ֲ�޶������*****************************************************/


/* ֱ�Ӳ����Ĵ����ķ�������IO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //���Ϊ�ߵ�ƽ		
#define digitalLo(p,i)		 {p->BRR=i;}	 //����͵�ƽ

/* �������IO�ĺ� */
#define IN1_HIGH        digitalHi(IN1_GPIO_PORT,IN1_GPIO_PIN)
#define IN1_LOW         digitalLo(IN1_GPIO_PORT,IN1_GPIO_PIN)

#define IN2_HIGH        digitalHi(IN2_GPIO_PORT,IN2_GPIO_PIN)
#define IN2_LOW         digitalLo(IN2_GPIO_PORT,IN2_GPIO_PIN)

#define IN3_HIGH        digitalHi(IN3_GPIO_PORT,IN3_GPIO_PIN)
#define IN3_LOW         digitalLo(IN3_GPIO_PORT,IN3_GPIO_PIN)

#define IN4_HIGH        digitalHi(IN4_GPIO_PORT,IN4_GPIO_PIN)
#define IN4_LOW         digitalLo(IN4_GPIO_PORT,IN4_GPIO_PIN)


enum dir{Pos,Neg};

void ULN2003_GPIO_Config(void);
void SysTick_Delay_Ms(__IO uint32_t ms);
void stepper(uint8_t dir,int speed);


#endif /* __LED_H */
