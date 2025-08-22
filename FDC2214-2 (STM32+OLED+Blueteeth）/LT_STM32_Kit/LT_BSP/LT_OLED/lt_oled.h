#ifndef __OLED_H
#define __OLED_H
#include "sys.h"
#include "stdlib.h"

#define OLED_MODE 0
#define SIZE 8
#define XLevelL		0x00
#define XLevelH		0x10
#define Max_Column	128
#define Max_Row		64
#define	Brightness	0xFF
#define X_WIDTH 	128
#define Y_WIDTH 	64

/********************************************移植修订点开始*****************************************************/
/* 定义OLED连接的GPIO端口, 用户只需要修改下面的代码即可改变控制的OLED引脚 */
#define OLED_SCLK_GPIO_PORT    	GPIOB		                /* OLED SCLK端口 */
#define OLED_SCLK_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* OLED SCLK端口时钟 */
#define OLED_SCLK_GPIO_PIN		GPIO_Pin_10			        /* OLED SCLK管脚 */

#define OLED_SDIN_GPIO_PORT    	GPIOB		                /* OLED SCLK端口 */
#define OLED_SDIN_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* OLED SCLK端口时钟 */
#define OLED_SDIN_GPIO_PIN		GPIO_Pin_11			        /* OLED SCLK管脚 */

/********************************************移植修订点结束*****************************************************/

#define OLED_SCLK_Clr() GPIO_ResetBits(OLED_SCLK_GPIO_PORT,OLED_SCLK_GPIO_PIN)//SCL
#define OLED_SCLK_Set() GPIO_SetBits(OLED_SCLK_GPIO_PORT,OLED_SCLK_GPIO_PIN)

#define OLED_SDIN_Clr() GPIO_ResetBits(OLED_SDIN_GPIO_PORT,OLED_SDIN_GPIO_PIN)//SDA
#define OLED_SDIN_Set() GPIO_SetBits(OLED_SDIN_GPIO_PORT,OLED_SDIN_GPIO_PIN)


#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


//OLED控制用函数
void OLED_WR_Byte(unsigned dat,unsigned cmd);
void OLED_Display_On(void);
void OLED_Display_Off(void);
void OLED_Init(void);
void OLED_Clear(void);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_Fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 Char_Size);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size);
void OLED_ShowString(u8 x,u8 y, u8 *p,u8 Char_Size);
void OLED_Set_Pos(unsigned char x, unsigned char y);
void OLED_ShowCHinese(u8 x,u8 y,u8 no);
void OLED_DrawBMP(unsigned char x0, unsigned char y0,unsigned char x1, unsigned char y1,unsigned char BMP[]);
void Delay_50ms(unsigned int Del_50ms);
void Delay_1ms(unsigned int Del_1ms);
void fill_picture(unsigned char fill_Data);
void Picture(void);              //me add void
void IIC_start(void);     //add void
void IIC_stop(void);    //add void
void Write_IIC_Command(unsigned char IIC_Command);
void Write_IIC_Data(unsigned char IIC_Data);
void Write_IIC_Byte(unsigned char IIC_Byte);

void IIC_wait_ack(void);//add void
#endif
