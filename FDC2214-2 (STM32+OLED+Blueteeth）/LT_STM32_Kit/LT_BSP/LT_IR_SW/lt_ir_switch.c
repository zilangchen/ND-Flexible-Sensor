/**
  ******************************************************************************
  * @file    lt_ir_switch.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-9-2
  * @brief   红外开关应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:零涂LT_STM32_Kit开发板 
  * 管脚    :PA12  
  * 触发方式:低电平触发
  * 使用示例:
			IR_SW_GPIO_Config();
			if(Read_IR_SW_Status() == IR_SW_ON)
			{
				printf("阻挡\r\n");
			}
			else
			{
				printf("没有阻挡\r\n");
			}
  * 微信    :lingtu15679003191
  * 扣扣    :1372090604
  *
  ******************************************************************************
  */
#include "delay.h"  
#include "lt_ir_switch.h"  

/**
  * @brief  配置红外开关管用到的I/O口
  * @param  无
  * @retval 无
  */
void IR_SW_GPIO_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/*开启按键端口的时钟*/
	RCC_APB2PeriphClockCmd(IR_SW_GPIO_CLK, ENABLE);
	
	#if(IR_SW_TRI_LEVEL == 0)
		//设置红外开关管的引脚为上拉输入
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	#else
		//设置红外开关管的引脚为下拉输入
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	#endif

	//选择红外开关管的引脚
	GPIO_InitStructure.GPIO_Pin = IR_SW_GPIO_PIN; 
	//使用结构体初始化红外开关管
	GPIO_Init(IR_SW_GPIO_PORT, &GPIO_InitStructure);
}

 /*
 * 函数名：Read_IR_SW_Status
 * 描述  ：读取红外开关管的状态
 * 输入  ：GPIOx：x 可以是 A，B，C，D或者 E
 *		     GPIO_Pin：待读取的端口位 	
 * 输出  ：IR_SW_OFF(红外管没有被阻挡)、IR_SW_ON（红外管被阻挡）
 */
uint8_t Read_IR_SW_Status(void)
{			
	/*检测红外管是否被阻挡 */
	if(GPIO_ReadInputDataBit(IR_SW_GPIO_PORT,IR_SW_GPIO_PIN) == IR_SW_ON )  
	{	 
		delay_ms(10);
		if(GPIO_ReadInputDataBit(IR_SW_GPIO_PORT,IR_SW_GPIO_PIN) == IR_SW_ON )  
		{
			return IR_SW_ON;
		}  
		return IR_SW_OFF;	 
	}
	else
		return IR_SW_OFF;
}
/*********************************************END OF FILE**********************/
