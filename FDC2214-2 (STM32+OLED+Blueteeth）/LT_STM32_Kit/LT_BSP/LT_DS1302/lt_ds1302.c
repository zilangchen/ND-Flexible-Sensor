#include "lt_ds1302.h"

/**
  ******************************************************************************
  * @file    lt_ds1302.c
  * @author  lingtu
  * @version V1.0
  * @date    2022-2-21
  * @brief   DS1302ʱ��ģ��Ӧ�ú����ӿ�
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:��ͿLT_STM32_Kit������ 
  * �ܽ�    :	
            VCC: 	5V
            GND: 	GND
            CLK: 	B13
            DAT: 	B14
            RST:	B15
  * ʹ��ʾ��:
             CALENDAR calendar={0x2022,0x02,0x21,0x19,0x34,0x00,0x00};  //У��ʱ��ֵ��BCD��
             u8 hour = 0;
             u8 min = 0;
             u8 sec = 0;
             
             delay_init();	    	 //��ʱ������ʼ��	  
             NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
             uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
             DS1302_Init();
             
             //    Set_Calendar(&calendar);   //У׼ʱ��, ��ҪУ׼ʱһ���ùرոú���
             
             while(1)
             {	
                Get_Calendar(&calendar);
		
                hour = BCD2DEC(calendar.hour);
                min  = BCD2DEC(calendar.minute);
                sec = BCD2DEC(calendar.second);
		
                printf("%x-%x-%x %2d:%2d:%2d", calendar.year, calendar.month , calendar.date, hour, min, sec);
                delay_ms(10);
             }	
  * ΢��    :lingtu15679003191
  * �ۿ�    :1372090604
  *
  ******************************************************************************
  */
void rtc_delay(uint32_t t)
{
	uint32_t i=0;
	for(i=0;i<t;i++);
	
}

/********************************************************
****�������ƣ�
****�������ã�
****���������
****���������
********************************************************/
void DS1302_Init(void)
{
	uint8_t dat;
	CALENDAR calendar={0x2020,0x02,0x06,0x11,0x51,0x00,0x00};
	
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(DS1302_CLK_GPIO_CLK|DS1302_DAT_GPIO_CLK|DS1302_RST_GPIO_CLK, ENABLE );	//ʹ��ʱ��
	
    /*CLK�ܽ�����*/
	GPIO_InitStructure.GPIO_Pin = DS1302_CLK_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS1302_CLK_GPIO_PORT, &GPIO_InitStructure);
    
    /*DAT�ܽ�����*/
    GPIO_InitStructure.GPIO_Pin = DS1302_DAT_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS1302_DAT_GPIO_PORT, &GPIO_InitStructure);
    
    /*RST�ܽ�����*/
    GPIO_InitStructure.GPIO_Pin = DS1302_RST_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DS1302_RST_GPIO_PORT, &GPIO_InitStructure);
	
	DS1302_CLK=0;
	DS1302_RST=0;
	DS1302_DAT=1;
	
	if(DS_ReadSingleReg(READ_RAM_REG)!=0x02)
	{
		DS_WriteSingleReg(WRITE_PRO,0X00);	//�ر�д����
		DS_WriteSingleReg(WRITE_RAM_REG,0x02);
		Set_Calendar(&calendar);
	}
	

}

/********************************************************
****�������ƣ�void DS_WriteByte(uint8_t data)
****�������ã�DS1302дһ���ֽ�
****���������Ҫд������
****���������
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
****�������ƣ�void DS_WriteSingleReg(uint8_t reg,uint8_t data)
****�������ã�д����DS�Ĵ���
****���������reg,�Ĵ���   data,����
****���������
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
****�������ƣ�uint8_t DS_ReadByte(void)
****�������ã���ȡһ���ֽ�����
****���������
****�������������������
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
****�������ƣ�uint8_t DS_ReadSingleReg(uint8_t reg)
****�������ã���ȡ�Ĵ�����ֵ
****����������Ĵ���
****���������ֵ
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
****�������ƣ�void DS_BurstWrite(uint8_t *data)
****�������ã�Brustģʽ��һ��д8���ֽڵ�8���Ĵ���
****���������8�ֽ����ݣ����ݸ�ʽΪ1~8�Ĵ���������
****���������
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
****�������ƣ�void DS_BurstRead(uint8_t *pbuf)
****�������ã�Burstģʽ��ȡʱ�䣬���ζ�ȡ�˸��Ĵ���
****������������ݻ������׵�ַ
****���������
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
****�������ƣ�void Set_Calendar(CALENDAR *p)
****�������ã�����ʱ��  ʱ���ʾ��BCD��
****���������
****���������
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
	temp[7]=0x80;	//����д����
	DS_WriteSingleReg(WRITE_PRO,0X00);	//�ر�д����
	DS_BurstWrite(temp);
	
}
/********************************************************
****�������ƣ�void Get_Calendar(CALENDAR *p)
****�������ã���ȡʱ������   ΪBCD��
****���������
****���������
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
    
    RCC_APB2PeriphClockCmd(DS1302_DAT_GPIO_CLK, ENABLE);	 //ʹ��OLED_SCLK_GPIO_CLK�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = DS1302_DAT_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		 //  ��������Ϊ����������������
    GPIO_Init(DS1302_DAT_GPIO_PORT, &GPIO_InitStructure);	  //��ʼ��GPIOD3,6
}

void DS1302_SDA_OUT(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(DS1302_DAT_GPIO_CLK, ENABLE);	 //ʹ��OLED_SCLK_GPIO_CLK�˿�ʱ��
    GPIO_InitStructure.GPIO_Pin = DS1302_DAT_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//�ٶ�50MHz
    GPIO_Init(DS1302_DAT_GPIO_PORT, &GPIO_InitStructure);	  //��ʼ��GPIOD3,6
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
