#include "lt_ds18b20.h"
#include "delay.h"	

/**
  ******************************************************************************
  * @file    lt_ds18b20.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-12-1
  * @brief   DS18B20应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:零涂LT_STM32_Kit开发板 
  * 管脚    :DQ: B7
  * 触发方式:低电平
  * 使用实例:
             float temperature;
			 while(DS18B20_GPIO_Config());	//DS18B20初始化
             
             temperature=DS18B20_Get_Temp()*1.0/10;
             printf("temperature=%.1f\r\n", temperature);
  * 微信    :lingtu15679003191
  * 扣扣    :1372090604
  *
  ******************************************************************************
  */  


//复位DS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); 	//SET PG11 OUTPUT
    DS18B20_DQ_OUT_OFF; 	//拉低DQ
    delay_us(750);    	//拉低750us
    DS18B20_DQ_OUT_ON; 	//DQ=1 
	delay_us(15);     	//15US
}
//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
u8 DS18B20_Check(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN();	//SET PG11 INPUT	 
    while (DS18B20_DQ_IN&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}
//从DS18B20读取一个位
//返回值：1/0
u8 DS18B20_Read_Bit(void) 	 
{
    u8 data;
	DS18B20_IO_OUT();	//SET PG11 OUTPUT
    DS18B20_DQ_OUT_OFF; 
	delay_us(2);
    DS18B20_DQ_OUT_ON; 
	DS18B20_IO_IN();	//SET PG11 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN)data=1;
    else data=0;	 
    delay_us(50);           
    return data;
}
//从DS18B20读取一个字节
//返回值：读到的数据
u8 DS18B20_Read_Byte(void)     
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT();	//SET PG11 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT_OFF;	// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT_ON;
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT_OFF;	// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT_ON;
            delay_us(2);                          
        }
    }
}
//开始温度转换
void DS18B20_Start(void) 
{   						               
    DS18B20_Rst();	   
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0x44);	// convert
} 

//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在 

u8 DS18B20_GPIO_Config(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(DS18B20_DQ_GPIO_CLK, ENABLE);	 //使能PORTG口时钟 
	
 	GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;				//PORTG.11 推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);

 	GPIO_SetBits(DS18B20_DQ_GPIO_PORT,DS18B20_DQ_GPIO_PIN);    //输出1

	DS18B20_Rst();

	return DS18B20_Check();
}  
//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL,TH;
	short tem;
    DS18B20_Start ();  			// ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0xbe);	// convert	    
    TL=DS18B20_Read_Byte(); 	// LSB   
    TH=DS18B20_Read_Byte(); 	// MSB  
	    	  
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;					//温度为负  
    }else temp=1;				//温度为正	  	  
    tem=TH; 					//获得高八位
    tem<<=8;    
    tem+=TL;					//获得底八位
    tem=(float)tem*0.625;		//转换     
	if(temp)return tem; 		//返回温度值
	else return -tem;    
}

void DS18B20_IO_IN(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
 	GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;				//PORTG.11 推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		  
 	GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);
}

void DS18B20_IO_OUT(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
 	GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;				//PORTG.11 推挽输出
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);
}

//#define DS18B20_IO_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}  // 设置为上拉/下拉输入模式
//#define DS18B20_IO_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}  // 设置为50Mhz的通用推挽输出模式

 
