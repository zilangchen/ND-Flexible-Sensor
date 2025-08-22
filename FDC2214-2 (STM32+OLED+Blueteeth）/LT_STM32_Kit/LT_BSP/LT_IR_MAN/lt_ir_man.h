#ifndef __LT_IR_MAN_H
#define	__LT_IR_MAN_H


#include "stm32f10x.h"

/********************************************移植修订点开始*****************************************************/
/* 定义人体红外连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的红外开关引脚 */
#define    IR_MAN_GPIO_CLK     RCC_APB2Periph_GPIOA
#define    IR_MAN_GPIO_PORT    GPIOA			   
#define    IR_MAN_GPIO_PIN	   GPIO_Pin_1

/* 触发电平*/
#define    IR_MAN_TRI_LEVEL	  1							/* 0:低电平触发   1:高电平触发 */
/********************************************移植修订点结束*****************************************************/

 /** 按键按下标置宏
	*  按键按下为高电平，设置 KEY_ON=1， KEY_OFF=0
	*  若按键按下为低电平，把宏设置成KEY_ON=0 ，KEY_OFF=1 即可
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

