#ifndef	_DS1302_H
#define	_DS1302_H
#include "sys.h"

typedef struct
{
	uint16_t 	year;
	uint8_t  	month;
	uint8_t		date;
	uint8_t		hour;
	uint8_t		minute;
	uint8_t		second;
	uint8_t		week;
}CALENDAR;	//BCD码表示时间

/********************************************移植修订点开始*****************************************************/
/* 定义DS1302连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的DS1302引脚 */
#define DS1302_CLK_GPIO_PORT    	GPIOB		                /* DS1302 CLK端口 */
#define DS1302_CLK_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* DS1302 CLK端口时钟 */
#define DS1302_CLK_GPIO_PIN		    GPIO_Pin_12			        /* DS1302 CLK管脚 */

#define DS1302_DAT_GPIO_PORT    	GPIOB		                /* DS1302 DAT端口 */
#define DS1302_DAT_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* DS1302 DAT端口时钟 */
#define DS1302_DAT_GPIO_PIN		    GPIO_Pin_13			        /* DS1302 DAT管脚 */

#define DS1302_RST_GPIO_PORT    	GPIOB		                /* DS1302 RST端口 */
#define DS1302_RST_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* DS1302 RST端口时钟 */
#define DS1302_RST_GPIO_PIN		    GPIO_Pin_14			        /* DS1302 RST管脚 */

#define	DS1302_CLK		            PBout(12)                   /* DS1302 CLK管脚 */
#define	DS1302_DAT		            PBout(13)                   /* DS1302 DAT管脚 */
#define	DS1302_RST		            PBout(14)                   /* DS1302 RST管脚 */
#define	READ_DS1302_DAT	            PBin(13)                    /* DS1302 DAT管脚 */

/********************************************移植修订点结束*****************************************************/


//IO方向设置
 
//#define SDA_IN()  {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=0X08000000;}
//#define SDA_OUT() {GPIOB->CRH&=0XF0FFFFFF;GPIOB->CRH|=0X03000000;}



//DS1302操作寄存器命令
//读指令
#define	READ_SEC	0X81
#define	READ_MIN	0X83
#define	READ_HOU	0X85
#define	READ_DAT	0X87
#define	READ_MON	0X89
#define	READ_WEK	0X8B
#define	READ_YEA	0X8D
#define	READ_PRO	0X8F

//写指令
#define	WRITE_SEC	0X80
#define	WRITE_MIN	0X82
#define	WRITE_HOU	0X84
#define	WRITE_DAT	0X86
#define	WRITE_MON	0X88
#define	WRITE_WEK	0X8A
#define	WRITE_YEA	0X8C
#define	WRITE_PRO	0X8E

#define WRITE_RAM_REG           0xc0
#define READ_RAM_REG            0xc1

#define DataToBcd(x) ((x/10)*16+(x%10))
#define BcdToData(x) ((x/16)*10+(x%16))

void DS_WriteSingleReg(uint8_t reg,uint8_t data);
uint8_t DS_ReadSingleReg(uint8_t reg);
void DS_BurstWrite(uint8_t *data);
void DS_BurstRead(uint8_t *pbuf);
void DS1302_Init(void);
void Set_Calendar(CALENDAR *p);
void Get_Calendar(CALENDAR *p);
void DS1302_SDA_IN(void);
void DS1302_SDA_OUT(void);
uint8_t BCD2DEC(uint8_t bcd);
uint8_t DEC2BCD(uint8_t dec);
#endif



