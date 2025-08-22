#ifndef __LT_BEEP_H
#define	__LT_BEEP_H


#include "stm32f10x.h"

/********************************************移植修订点开始*****************************************************/
/* 定义蜂鸣器连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的LED引脚 */
#define BEEP_GPIO_PORT    	GPIOB		              	/* GPIO端口 */
#define BEEP_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define BEEP_GPIO_PIN		GPIO_Pin_15			        		        

/* 触发电平*/
#define BEEP_TRI_LEVEL		0							/* 0:低电平触发   1:高电平触发 */
/********************************************移植修订点结束*****************************************************/

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

/* 使用标准的固件库控制IO*/
#define BEEP(a)	if (a)	\
					GPIO_SetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN);\
					else		\
					GPIO_ResetBits(BEEP_GPIO_PORT,BEEP_GPIO_PIN)


/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)		{p->BSRR=i;}	 //输出为高电平		
#define digitalLo(p,i)		{p->BRR=i;}	 //输出低电平
#define digitalToggle(p,i) 	{p->ODR ^=i;} //输出反转状态


/* 定义控制IO的宏 */
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
