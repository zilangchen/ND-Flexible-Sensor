#include "lt_fdc2214_iic.h"

void FDC2214_SDA_IN()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin=FDC2214_SDIN_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(FDC2214_SDIN_GPIO_PORT,&GPIO_InitStructure);

}

void FDC2214_SDA_OUT()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin=FDC2214_SDIN_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
    GPIO_Init(FDC2214_SDIN_GPIO_PORT,&GPIO_InitStructure);
}

void FDC2214_IIC_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    /*配置FDC2214 SCLK*/
    RCC_APB2PeriphClockCmd(FDC2214_SCLK_GPIO_CLK, ENABLE);	 //使能OLED_SCLK_GPIO_CLK端口时钟
    GPIO_InitStructure.GPIO_Pin = FDC2214_SCLK_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
    GPIO_Init(FDC2214_SCLK_GPIO_PORT, &GPIO_InitStructure);	  //初始化GPIOD3,6
    GPIO_SetBits(FDC2214_SCLK_GPIO_PORT,FDC2214_SCLK_GPIO_PIN);

    /*配置FDC2214 SDIN*/
    RCC_APB2PeriphClockCmd(FDC2214_SDIN_GPIO_CLK, ENABLE);	 //使能OLED_SCLK_GPIO_CLK端口时钟
    GPIO_InitStructure.GPIO_Pin = FDC2214_SDIN_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
    GPIO_Init(FDC2214_SDIN_GPIO_PORT, &GPIO_InitStructure);	  //初始化GPIOD3,6

    // 给两个管脚高电平
    GPIO_SetBits(FDC2214_SDIN_GPIO_PORT,FDC2214_SDIN_GPIO_PIN);
    GPIO_SetBits(FDC2214_SCLK_GPIO_PORT,FDC2214_SCLK_GPIO_PIN);
}

void delay_2us(void)
{
    //一个NOP是1/72us  所以72个NOP是1us
    u32 i=0;
    for(i=0; i<2; i++)
    {
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();

    }
}
void FDC2214_IIC_Start(void)
{
    FDC2214_SDA_OUT();
    FDC2214_IIC_SDA_OUT=1;
    FDC2214_IIC_SCL=1;
    delay_2us();
    FDC2214_IIC_SDA_OUT=0;
    delay_2us();
    FDC2214_IIC_SCL=0;
    delay_2us();
}


void FDC2214_IIC_Stop(void)
{
    FDC2214_SDA_OUT();
    FDC2214_IIC_SCL=0;
    delay_2us();
    FDC2214_IIC_SDA_OUT=0;
    delay_2us();
    FDC2214_IIC_SCL=1;
    delay_2us();
    FDC2214_IIC_SDA_OUT=1;
    delay_2us();
}

u8 FDC2214_IIC_Wait_Ack(void)
{
    u8 waitTime=0;
    FDC2214_SDA_IN();
    FDC2214_IIC_SDA_OUT=1;
    delay_2us();
    FDC2214_IIC_SCL=1;
    delay_2us();
    while(FDC2214_IIC_SDA_IN)
    {
        waitTime++;
        if(waitTime>250)
        {
            FDC2214_IIC_Stop();
            return 1;
        }
    }
    FDC2214_IIC_SCL=0;
    return 0;
}

void FDC2214_IIC_Ack(void)
{
    FDC2214_IIC_SCL=0;
    delay_2us();
    FDC2214_SDA_OUT();
    FDC2214_IIC_SDA_OUT=0;//发送0是ACK
    delay_2us();
    FDC2214_IIC_SCL=1;
    delay_2us();
    FDC2214_IIC_SCL=0;
    delay_2us();
}

void FDC2214_IIC_NAck(void)
{
    FDC2214_IIC_SCL=0;
    delay_2us();
    FDC2214_SDA_OUT();
    FDC2214_IIC_SDA_OUT=1;//发送1是nACK
    delay_2us();
    FDC2214_IIC_SCL=1;
    delay_2us();
    FDC2214_IIC_SCL=0;
    delay_2us();
}

void FDC2214_IIC_Send_Byte(u8 dat)
{
    u8 t;
    FDC2214_SDA_OUT();
    FDC2214_IIC_SCL=0;
    for(t=0; t<8; t++)
    {

        delay_2us();
        FDC2214_IIC_SDA_OUT=(dat&0x80)>>7;
        dat<<=1;
        FDC2214_IIC_SCL=1;
        delay_2us();
        FDC2214_IIC_SCL=0;
        delay_2us();
    }
    //FDC2214_IIC_SCL=0;
}

u8 FDC2214_IIC_Read_Byte(u8 ack)
{
    u8 i,receive=0;
    FDC2214_IIC_SDA_OUT=1;
    FDC2214_SDA_IN();
    for(i=0; i<8; i++ )
    {
        receive<<=1;

        FDC2214_IIC_SCL=0;
        delay_2us();
        FDC2214_IIC_SCL=1;
        delay_2us();
        if(FDC2214_IIC_SDA_IN)receive++;//最低位置1
    }
    FDC2214_IIC_SCL=0;
    if (!ack)
        FDC2214_IIC_NAck();
    else
        FDC2214_IIC_Ack();
    return receive;
}





