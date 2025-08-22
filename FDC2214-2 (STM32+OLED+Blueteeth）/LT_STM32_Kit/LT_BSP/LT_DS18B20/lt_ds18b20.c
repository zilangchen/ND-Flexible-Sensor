#include "lt_ds18b20.h"
#include "delay.h"	

/**
  ******************************************************************************
  * @file    lt_ds18b20.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-12-1
  * @brief   DS18B20Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:��ͿLT_STM32_Kit������ 
  * �ܽ�    :DQ: B7
  * ������ʽ:�͵�ƽ
  * ʹ��ʵ��:
             float temperature;
			 while(DS18B20_GPIO_Config());	//DS18B20��ʼ��
             
             temperature=DS18B20_Get_Temp()*1.0/10;
             printf("temperature=%.1f\r\n", temperature);
  * ΢��    :lingtu15679003191
  * �ۿ�    :1372090604
  *
  ******************************************************************************
  */  


//��λDS18B20
void DS18B20_Rst(void)	   
{                 
	DS18B20_IO_OUT(); 	//SET PG11 OUTPUT
    DS18B20_DQ_OUT_OFF; 	//����DQ
    delay_us(750);    	//����750us
    DS18B20_DQ_OUT_ON; 	//DQ=1 
	delay_us(15);     	//15US
}
//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
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
//��DS18B20��ȡһ��λ
//����ֵ��1/0
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
//��DS18B20��ȡһ���ֽ�
//����ֵ������������
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
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
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
//��ʼ�¶�ת��
void DS18B20_Start(void) 
{   						               
    DS18B20_Rst();	   
	DS18B20_Check();	 
    DS18B20_Write_Byte(0xcc);	// skip rom
    DS18B20_Write_Byte(0x44);	// convert
} 

//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:���� 

u8 DS18B20_GPIO_Config(void)
{
 	GPIO_InitTypeDef  GPIO_InitStructure;
 	
 	RCC_APB2PeriphClockCmd(DS18B20_DQ_GPIO_CLK, ENABLE);	 //ʹ��PORTG��ʱ�� 
	
 	GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;				//PORTG.11 �������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);

 	GPIO_SetBits(DS18B20_DQ_GPIO_PORT,DS18B20_DQ_GPIO_PIN);    //���1

	DS18B20_Rst();

	return DS18B20_Check();
}  
//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
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
        temp=0;					//�¶�Ϊ��  
    }else temp=1;				//�¶�Ϊ��	  	  
    tem=TH; 					//��ø߰�λ
    tem<<=8;    
    tem+=TL;					//��õװ�λ
    tem=(float)tem*0.625;		//ת��     
	if(temp)return tem; 		//�����¶�ֵ
	else return -tem;    
}

void DS18B20_IO_IN(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
 	GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;				//PORTG.11 �������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		  
 	GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);
}

void DS18B20_IO_OUT(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
	
 	GPIO_InitStructure.GPIO_Pin = DS18B20_DQ_GPIO_PIN;				//PORTG.11 �������
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		  
 	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
 	GPIO_Init(DS18B20_DQ_GPIO_PORT, &GPIO_InitStructure);
}

//#define DS18B20_IO_IN()  {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=8<<12;}  // ����Ϊ����/��������ģʽ
//#define DS18B20_IO_OUT() {GPIOG->CRH&=0XFFFF0FFF;GPIOG->CRH|=3<<12;}  // ����Ϊ50Mhz��ͨ���������ģʽ

 
