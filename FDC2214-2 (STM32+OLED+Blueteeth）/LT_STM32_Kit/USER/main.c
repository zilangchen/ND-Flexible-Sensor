#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "lt_led.h"
#include "lt_key.h"
#include "lt_beep.h"
#include "lt_relay.h"
#include "lt_ir_switch.h"
#include "lt_ir_man.h"
#include "lt_dht11.h"
#include "lt_adc.h"
#include "lt_pwm.h"
#include "lt_ds18b20.h"
#include "lt_oled.h"
#include "lt_matrix_key.h"
#include "lt_rc522.h"
#include "lt_ds1302.h"
#include "lt_timer.h"
#include "lt_voice.h"
#include "lt_uln2003.h"
#include "lt_fdc2214_iic.h"
#include "lt_fdc2214.h"

/*
    ���ڼ�����MAX30102����������RAMռ��ԼΪ11K��������C8T6�����ֵ
    ������ʹ�õ�MAX30102ģ��ʱ�ټ�����Ӧ���������룬�������
*/
//#include "lt_max30102.h"

#define MAX_TIME_NUM                3

/************************************************
	������
    STM32оƬ��>= STM32F103C8T6��F1ϵ��
    OLEDģ�飺
                VCC: 3.3V
                GND: GND
                SCL: A5
                SDA: A6
    ESP8266��
                RX:  A9
                TX:  A10
                CHECH_IO: A12
                VCC: 3.3V
                GND:GND
    FDC2214ģ�飺
                SCL:   B8
                SDA:   B9
                GND:   GND
                VCC:  3,3V
    
************************************************/

// ADC1ת���ĵ�ѹֵͨ��MDA��ʽ����SRAM
// extern __IO uint16_t ADC_ConvertedValue[NOFCHANEL];
// extern u8 USART_RX_BUF[USART_REC_LEN];
float cap1 =0;
float cap2 =0;
float cap3 =0;
float cap4 =0;
#define    JS_cap1 11;
#define		 JS_cap2 14;
#define		 JS_cap3 14;
#define		 JS_cap4 14;
int main(void)
{	   

    int i = 0;

//    u8 USART_RX_BUF[15] = {0x12, 0x14, 0xAA, 0xA5, 0x09, 0x01, 0x01, 0x34, 0x01, 0x69, 0x01, 0x29, 0xD3, 0x55, 0x12};
//    uint16_t RecIndex = 15;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB |  RCC_APB2Periph_AFIO, ENABLE);//ʹ��PORTB��ʱ�ӡ���������PB3��PB4����
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  //�ر�JTAG��SWD���Ե��Ե�

	delay_init();	    	 //��ʱ������ʼ��	  
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	uart_init(115200);	 //���ڳ�ʼ��Ϊ115200
	//	printf("system start!\r\n");
   // uart2_init(9600);
    
//  RC522_GPIO_Config();
	LED_GPIO_Config();
	KEY_GPIO_Config();
//	BEEP_GPIO_Config();
//  ULN2003_GPIO_Config();
   
    
//	RELAY_GPIO_Config();
//	IR_SW_GPIO_Config();
//	IR_MAN_GPIO_Config();
//	ADCx_Init();
    OLED_Init();
    OLED_Clear();
//    DS1302_Init();
    FDC2214_IIC_Init();
    while(FDC2214_Init());
		printf("FDC2214 Init OK!\r\n");
//  Set_Calendar(&calendar);   //У׼ʱ�亯��
//  max30102_init();
//  Matrix_KEY_GPIO_Config();
//	PWM_Init();
//	while(!DHT11_Init()); 
//  while(DS18B20_GPIO_Config());	//DS18B20��ʼ��

    /*��(0) ��(1) ��(2) ��(3) ϵ(4) ͳ(5) ͨ(6) ��(7) ��(8) λ(9)*/
    /* ��һ�е��ݲ���ϵͳ */
    OLED_ShowCHinese(16, 0, 0);
    OLED_ShowCHinese(32, 0, 1);
    OLED_ShowCHinese(48, 0, 2);
    OLED_ShowCHinese(64, 0, 3);
    OLED_ShowCHinese(80, 0, 4);
    OLED_ShowCHinese(96, 0, 5);
    
    /* �ڶ�����ʾͨ��1��   ͨ��2��  */
  /*  OLED_ShowCHinese(0,  2, 6);
    OLED_ShowCHinese(16, 2, 7);
    OLED_ShowChar(32, 2, '1', 16);
    OLED_ShowChar(40, 2, ':', 16);
    
   /* OLED_ShowCHinese(0+64,  2, 6);
    OLED_ShowCHinese(16+64, 2, 7);
    OLED_ShowChar(32+64, 2, '2', 16);
    OLED_ShowChar(40+64, 2, ':', 16);
    
    /��������ʾͨ��3��   ͨ��4��  /
    OLED_ShowCHinese(0,  4, 6);
    OLED_ShowCHinese(16, 4, 7);
    OLED_ShowChar(32, 4, '3', 16);
    OLED_ShowChar(40, 4, ':', 16);
    
    OLED_ShowCHinese(0+64,  4, 6);
    OLED_ShowCHinese(16+64, 4, 7);
    OLED_ShowChar(32+64, 4, '4', 16);
    OLED_ShowChar(40+64, 4, ':', 16);  */
		
    
    /* ��������ʾ��λ:pF*/
    OLED_ShowCHinese(0,  6, 8);
    OLED_ShowCHinese(16, 6, 9);
    OLED_ShowChar(32, 6, ':', 16);
    OLED_ShowString(40, 6, "pF", 16);

//    while(ESP);
    while(1)
    {
        
        cap1=getcap(0,18,33)-JS_cap1;//�ɼ�����
        delay_ms(10);			
        cap2=getcap(1,18,33)-JS_cap2;//�ɼ�����		
        delay_ms(10);			
        cap3=getcap(2,18,33)-JS_cap3;//�ɼ�����
        delay_ms(10);			
        cap4=getcap(3,18,33)-JS_cap4;//�ɼ�����
        delay_ms(10);	
//            cap1=getcap(0,18,33);//�ɼ�����
//        delay_ms(100);			
//            cap2=getcap(1,18,33);//�ɼ�����		
//        delay_ms(100);			
//            cap3=getcap(2,18,33);//�ɼ�����
//        delay_ms(100);			
//            cap4=getcap(3,18,33);//�ɼ�����
//        delay_ms(100);				
			LED1_ON;
			printf("Capacity_value1= %5.3f pF\r\n",cap1);
		  printf("Capacity_value2= %5.3f pF\r\n",cap2);
		  printf("Capacity_value3= %5.3f pF\r\n",cap3);
      printf("Capacity_value4= %5.3f pF\r\n",cap4);
			printf("\r\n");
        /* �ڶ�����ʾͨ��1��   */
				OLED_ShowCHinese(0,  2, 6);
        OLED_ShowCHinese(16, 2, 7);
        OLED_ShowChar(32, 2, '1', 16);
        OLED_ShowChar(40, 2, ':', 16);
				OLED_ShowNum(48,    2, cap1, 6, 16);
			 /* ��������ʾͨ��2��   */
				OLED_ShowCHinese(0,  4, 6);
        OLED_ShowCHinese(16, 4, 7);
        OLED_ShowChar(32, 4, '2', 16);
        OLED_ShowChar(40, 4, ':', 16);
        OLED_ShowNum(48, 4, cap2, 6, 16);
        //OLED_ShowNum(48,    4, cap[2], 5, 16);
       // OLED_ShowNum(48+64, 4, cap[3], 5, 16);
        delay_ms(200);
				        /* �ڶ�����ʾͨ��3��   */
				OLED_ShowCHinese(0,  2, 6);
        OLED_ShowCHinese(16, 2, 7);
        OLED_ShowChar(32, 2, '3', 16);
        OLED_ShowChar(40, 2, ':', 16);
				OLED_ShowNum(48,    2, cap3, 6, 16);
			 /* ��������ʾͨ��4��   */
				OLED_ShowCHinese(0,  4, 6);
        OLED_ShowCHinese(16, 4, 7);
        OLED_ShowChar(32, 4, '4', 16);
        OLED_ShowChar(40, 4, ':', 16);
        OLED_ShowNum(48, 4, cap4, 6, 16);
				LED1_OFF	;
    }  
 }
