/**
  ******************************************************************************
  * @file    lt_ir_main.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-9-2
  * @brief   人体红外应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:零涂LT_STM32_Kit开发板 
  * 管脚    :PA8  
  * 触发方式:低电平触发
  * 使用示例:
			IR_MAN_GPIO_Config();
			if(Read_IR_MAN_Status() == IR_MAN_ON)
			{
				printf("有人\r\n");
			}
			else
			{
				printf("没人\r\n");
			}
  * 微信    :lingtu15679003191
  * 扣扣    :1372090604
  *
  ******************************************************************************
  */
#include "delay.h"  
#include "lt_ir_man.h"  

/**
  * @brief  配置红外开关管用到的I/O口
  * @param  无
  * @retval 无
  */
void IR_MAN_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启按键端口的时钟*/
	RCC_APB2PeriphClockCmd(IR_MAN_GPIO_CLK, ENABLE);
	
	#if(IR_MAN_TRI_LEVEL == 0)
		//设置红外开关管的引脚为上拉输入
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	#else
		//设置红外开关管的引脚为下拉输入
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	#endif

	//选择红外开关管的引脚
	GPIO_InitStructure.GPIO_Pin = IR_MAN_GPIO_PIN; 
	//使用结构体初始化红外开关管
	GPIO_Init(IR_MAN_GPIO_PORT, &GPIO_InitStructure);
}

 /*
 * 函数名：Read_IR_MAN_Status
 * 描述  ：读取人体红外的状态
 * 输入  ：GPIOx：x 可以是 A，B，C，D或者 E
 *		     GPIO_Pin：待读取的端口位 	
 * 输出  ：IR_MAN_OFF(没人)、IR_MAN_ON（有人）
 */
uint8_t Read_IR_MAN_Status(void)
{			
	/*检测红外管是否被阻挡 */
	if(GPIO_ReadInputDataBit(IR_MAN_GPIO_PORT,IR_MAN_GPIO_PIN) == IR_MAN_ON )  
	{	 
		delay_ms(10);
		if(GPIO_ReadInputDataBit(IR_MAN_GPIO_PORT,IR_MAN_GPIO_PIN) == IR_MAN_ON )  
		{
			return IR_MAN_ON;
		}  
		return IR_MAN_OFF;	 
	}
	else
		return IR_MAN_OFF;
}
/*********************************************END OF FILE**********************/
