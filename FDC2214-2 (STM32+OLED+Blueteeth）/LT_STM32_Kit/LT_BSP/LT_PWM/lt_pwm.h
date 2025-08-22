#ifndef __LT_PWM_H
#define __LT_PWM_H

#include "stm32f10x.h"


/********************************************��ֲ�޶��㿪ʼ*****************************************************/
#define            PWM_NUM                   1
#define            PWM_TIM                   TIM3
#define            PWM_TIM_APBxClock_FUN     RCC_APB1PeriphClockCmd
#define            PWM_TIM_CLK               RCC_APB1Periph_TIM3
#define            PWM_TIM_Period            9
#define            PWM_TIM_Prescaler         71
// TIM3 ����Ƚ�ͨ��1
#define            PWM_TIM_CH1_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            PWM_TIM_CH1_PORT          GPIOA
#define            PWM_TIM_CH1_PIN           GPIO_Pin_6

// TIM3 ����Ƚ�ͨ��2
#define            PWM_TIM_CH2_GPIO_CLK      RCC_APB2Periph_GPIOA
#define            PWM_TIM_CH2_PORT          GPIOA
#define            PWM_TIM_CH2_PIN           GPIO_Pin_7

// TIM3 ����Ƚ�ͨ��3
#define            PWM_TIM_CH3_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            PWM_TIM_CH3_PORT          GPIOB
#define            PWM_TIM_CH3_PIN           GPIO_Pin_0

// TIM3 ����Ƚ�ͨ��4
#define            PWM_TIM_CH4_GPIO_CLK      RCC_APB2Periph_GPIOB
#define            PWM_TIM_CH4_PORT          GPIOB
#define            PWM_TIM_CH4_PIN           GPIO_Pin_1
/********************************************��ֲ�޶������*****************************************************/


/**************************��������********************************/

void PWM_Init(void);


#endif	/* __BSP_GENERALTIME_H */


