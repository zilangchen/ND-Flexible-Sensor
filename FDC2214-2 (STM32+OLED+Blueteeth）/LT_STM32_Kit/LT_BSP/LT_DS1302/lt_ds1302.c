#include "lt_ds1302.h"

/**
  ******************************************************************************
  * @file    lt_ds1302.c
  * @author  lingtu
  * @version V1.0
  * @date    2022-2-21
  * @brief   DS1302时钟模块应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:零涂LT_STM32_Kit开发板 
  * 管脚    :	
            VCC: 	5V
            GND: 	GND
            CLK: 	B13
            DAT: 	B14
            RST:	B15
  * 使用示例:
             CALENDAR calendar={0x2022,0x02,0x21,0x19,0x34,0x00,0x00};  //校验时间值，BCD码
             u8 hour = 0;
             u8 min = 0;
             u8 sec = 0;
             
             delay_init();	    	 //延时函数初始化	  
             NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
             uart_init(115200);	 //串口初始化为115200
             DS1302_Init();
             
             //    Set_Calendar(&calendar);   //校准时间, 不要校准时一定得关闭该函数
             
             while(1)
             {	
                Get_Calendar(&calendar);
		
                hour = BCD2DEC(calendar.hour);
                min  = BCD2DEC(calendar.minute);
                sec = BCD2DEC(calendar.second);
		
                printf("%x-%x-%x %2d:%2d:%2d", calendar.year, calendar.month , calendar.date, hour, min, sec);
                delay_ms(10);
             }	
  * 微信    :lingtu15679003191
  * 扣扣    :1372090604
  *
  ******************************************************************************
  */
void rtc_delay(uint32_t t)
{
	uint32_t i=0;
	for(i=0;i<t;i++);
	
}

/********************************************************
****函数名称：
****函数作用：
****输入参数：
****输出参数：
********************************************************/
void DS1302_Init(void)
{
	uint8_t dat;
	CALENDAR calendar={0x2020,0x02,0x06,0x11,0x51,0x00,0x00};
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DS1302_CLK_GPIO_CLK|DS1302_DAT_GPIO_CLK|DS1302_RST_GPIO_CLK, ENABLE );	//使能时钟
	
    /*CLK管脚配置*/
	GPIO_InitStructure.GPIO_Pin = DS1302_CLK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS1302_CLK_GPIO_PORT, &GPIO_InitStructure);
    
    /*DAT管脚配置*/
    GPIO_InitStructure.GPIO_Pin = DS1302_DAT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS1302_DAT_GPIO_PORT, &GPIO_InitStructure);
    
    /*RST管脚配置*/
    GPIO_InitStructure.GPIO_Pin = DS1302_RST_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS1302_RST_GPIO_PORT, &GPIO_InitStructure);
	
	DS1302_CLK=0;
	DS1302_RST=0;
	DS1302_DAT=1;
	
	if(DS_ReadSingleReg(READ_RAM_REG)!=0x02)
	{
		DS_WriteSingleReg(WRITE_PRO,0X00);	//关闭写保护
		DS_WriteSingleReg(WRITE_RAM_REG,0x02);
		Set_Calendar(&calendar);
	}
	

}

/********************************************************
****函数名称：void DS_WriteByte(uint8_t data)
****函数作用：DS1302写一个字节
****输入参数：要写的数据
****输出参数：
********************************************************/
void DS_WriteByte(uint8_t data)
{
	uint8_t i=0;
	uint32_t temp=0;
	
	DS1302_SDA_OUT();
	for(i=0;i<8;i++)
	{
		if(data&0x01)
			DS1302_DAT=1;
		else
			DS1302_DAT=0;
		DS1302_CLK=1;
		DS1302_CLK=0;
		data>>=1;
	}
	DS1302_DAT=1;
}
/********************************************************
****函数名称：void DS_WriteSingleReg(uint8_t reg,uint8_t data)
****函数作用：写数据DS寄存器
****输入参数：reg,寄存器   data,数据
****输出参数：
********************************************************/
void DS_WriteSingleReg(uint8_t reg,uint8_t data)
{
	DS1302_RST=0;
	DS1302_CLK=0;
	DS1302_RST=1;
	
	DS_WriteByte(reg);
	DS_WriteByte(data);
	DS1302_RST=0;
	DS1302_CLK=0;
}

/********************************************************
****函数名称：uint8_t DS_ReadByte(void)
****函数作用：读取一个字节数据
****输入参数：
****输出参数：读出的数据
********************************************************/
uint8_t DS_ReadByte(void)
{
	uint8_t data=0;
	uint8_t i=0;
	
	DS1302_SDA_IN();
	for(i=0;i<8;i++)
	{
		data>>=1;
		if(READ_DS1302_DAT)
			data|=0X80;
		else
			data&=0x7f;
		DS1302_CLK=1;
		DS1302_CLK=0;

	}
	return data;
}


/********************************************************
****函数名称：uint8_t DS_ReadSingleReg(uint8_t reg)
****函数作用：读取寄存器的值
****输入参数：寄存器
****输出参数：值
********************************************************/
uint8_t DS_ReadSingleReg(uint8_t reg)
{
	uint8_t data=0;
	
	DS1302_RST=0;
	DS1302_CLK=0;
	DS1302_RST=1;
	
	DS_WriteByte(reg);
	data=DS_ReadByte();
	DS1302_RST=0;
	return data;
}

/********************************************************
****函数名称：void DS_BurstWrite(uint8_t *data)
****函数作用：Brust模式，一次写8个字节到8个寄存器
****输入参数：8字节数据，数据格式为1~8寄存器的数据
****输出参数：
********************************************************/
void DS_BurstWrite(uint8_t *data)
{
	uint8_t i=0;
	
	DS1302_RST=1;
	DS_WriteByte(0XBE);
	for(i=0;i<8;i++)
	{
		DS_WriteByte(data[i]);
	}
	DS1302_RST=0;
}
/********************************************************
****函数名称：void DS_BurstRead(uint8_t *pbuf)
****函数作用：Burst模式读取时间，依次读取八个寄存器
****输入参数：数据缓存区首地址
****输出参数：
********************************************************/
void DS_BurstRead(uint8_t *pbuf)
{
	uint8_t i=0;
	
	DS1302_RST=1;
	DS_WriteByte(0XBF);
	for(i=0;i<8;i++)
	{
		pbuf[i]=DS_ReadByte();
	}
	DS1302_RST=0;
}

/********************************************************
****函数名称：void Set_Calendar(CALENDAR *p)
****函数作用：设置时间  时间表示用BCD码
****输入参数：
****输出参数：
********************************************************/
void Set_Calendar(CALENDAR *p)
{
	uint8_t temp[8]={0};
	
	temp[0]=p->second;
	temp[1]=p->minute;
	temp[2]=p->hour;
	temp[3]=p->date;
	temp[4]=p->month;
	temp[5]=p->week;
	p->year&=0xff;
	temp[6]=p->year;
	temp[7]=0x80;	//开启写保护
	DS_WriteSingleReg(WRITE_PRO,0X00);	//关闭写保护
	DS_BurstWrite(temp);
	
}
/********************************************************
****函数名称：void Get_Calendar(CALENDAR *p)
****函数作用：获取时间日期   为BCD码
****输入参数：
****输出参数：
********************************************************/
void Get_Calendar(CALENDAR *p)
{
	uint8_t data[8]={0};
	
	DS_BurstRead(data);
	p->second=data[0]&0x7f;
	p->minute=data[1];
	p->hour=data[2];
	p->date=data[3];
	p->month=data[4];
	p->week=data[5];
	p->year=data[6]|(0x20<<8);
}

void DS1302_SDA_IN(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(DS1302_DAT_GPIO_CLK, ENABLE);	 //使能OLED_SCLK_GPIO_CLK端口时钟
    GPIO_InitStructure.GPIO_Pin = DS1302_DAT_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //  这里配置为上拉和下拉都可以
    GPIO_Init(DS1302_DAT_GPIO_PORT, &GPIO_InitStructure);	  //初始化GPIOD3,6
}

void DS1302_SDA_OUT(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(DS1302_DAT_GPIO_CLK, ENABLE);	 //使能OLED_SCLK_GPIO_CLK端口时钟
    GPIO_InitStructure.GPIO_Pin = DS1302_DAT_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
    GPIO_Init(DS1302_DAT_GPIO_PORT, &GPIO_InitStructure);	  //初始化GPIOD3,6
    GPIO_SetBits(DS1302_DAT_GPIO_PORT,DS1302_DAT_GPIO_PIN);
}

uint8_t BCD2DEC(uint8_t bcd)  
{  
    return (bcd-(bcd>>4)*6);
}  

uint8_t DEC2BCD(uint8_t dec)  
{  
    return (dec+(dec/10)*6);
}
