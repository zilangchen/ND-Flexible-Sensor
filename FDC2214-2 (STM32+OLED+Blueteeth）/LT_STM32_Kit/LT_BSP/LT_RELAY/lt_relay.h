#ifndef __LT_RELAY_H
#define	__LT_RELAY_H


#include "stm32f10x.h"

/********************************************移植修订点开始*****************************************************/
/* 定义继电器连接的GPIO端口 */
#define RELAY_GPIO_PORT    	GPIOB		              	/* GPIO端口 */
#define RELAY_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define RELAY_GPIO_PIN		GPIO_Pin_6	

/* 触发电平*/
#define RELAY_TRI_LEVEL		0							/* 0:低电平触发   1:高电平触发 */
/********************************************移植修订点结束*****************************************************/

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

/* 使用标准的固件库控制IO*/
#define RELAY(a)	if (a)	\
					GPIO_SetBits(RELAY_GPIO_PORT,RELAY_GPIO_PIN);\
					else		\
					GPIO_ResetBits(RELAY_GPIO_PORT,RELAY_GPIO_PIN)


/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)		{p->BSRR=i;}	 //输出为高电平		
#define digitalLo(p,i)		{p->BRR=i;}	 	 //输出低电平
#define digitalToggle(p,i) 	{p->ODR ^=i;}    //输出反转状态


/* 定义控制IO的宏 */
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
