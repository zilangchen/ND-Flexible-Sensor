#ifndef __LT_DS18B20_H
#define __LT_DS18B20_H 
#include "sys.h"

/********************************************移植修订点开始*****************************************************/
/* 定义LED1连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的LED引脚 */
#define DS18B20_DQ_GPIO_PORT    	GPIOB		                /* GPIO端口 */
#define DS18B20_DQ_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define DS18B20_DQ_GPIO_PIN		    GPIO_Pin_11			        
/********************************************移植修订点结束*****************************************************/


/* 直接操作寄存器的方法控制IO */
#define	digitalHi(p,i)		{p->BSRR=i;}	 //输出为高电平		
#define digitalLo(p,i)		{p->BRR=i;}	 //输出低电平

#define DS18B20_DQ_OUT_ON	digitalHi(DS18B20_DQ_GPIO_PORT,DS18B20_DQ_GPIO_PIN)
#define DS18B20_DQ_OUT_OFF	digitalLo(DS18B20_DQ_GPIO_PORT,DS18B20_DQ_GPIO_PIN)
#define DS18B20_DQ_IN	    GPIO_ReadInputDataBit(DS18B20_DQ_GPIO_PORT,DS18B20_DQ_GPIO_PIN)
   	
u8 DS18B20_GPIO_Config(void);//初始化DS18B20
short DS18B20_Get_Temp(void);//获取温度
void DS18B20_Start(void);//开始温度转换
void DS18B20_Write_Byte(u8 dat);//写入一个字节
u8 DS18B20_Read_Byte(void);//读出一个字节
u8 DS18B20_Read_Bit(void);//读出一个位
u8 DS18B20_Check(void);//检测是否存在DS18B20
void DS18B20_Rst(void);//复位DS18B20    
void DS18B20_IO_IN(void);
void DS18B20_IO_OUT(void);
#endif















