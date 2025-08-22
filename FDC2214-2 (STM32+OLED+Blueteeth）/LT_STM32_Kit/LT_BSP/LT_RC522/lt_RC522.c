#include "lt_rc522.h"
#include "usart.h"
#include "delay.h"

/**
  ******************************************************************************
  * @file    lt_matrix_key.c
  * @author  lingtu
  * @version V1.0
  * @date    2021-12-13
  * @brief   矩阵按键应用函数接口
  ******************************************************************************
  * @attention
  *
  * 实验平台:零涂LT_STM32_Kit开发板 
  * 管脚    :(建议不要修改)	
            VCC: 	3.3V
            RST: 	B6
            GND: 	GND
            IRQ: 	NC
            MISO:	B14
            MOSI:   B15
            SCK:	B13
            SDA:    B5
  * 使用示例:
             char cid[5] = {0};
             
             delay_init();	    	 //延时函数初始化	  
             NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
             uart_init(115200);	 //串口初始化为115200
             RC522_GPIO_Config();
             
             while(1)
             {	
                RC522_Read_ID_Once(cid);
                if(strlen(cid) > 0)
                {
                    printf("ID:%s\r\n", cid);
                    memset(cid, 0x00, 5);
                }
                
                delay_ms(10);
             }	
  * 微信    :lingtu15679003191
  * 扣扣    :1372090604
  *
  ******************************************************************************
  */

#define   RC522_DELAY()  delay_us( 2 ) 

/*????*/
unsigned char CT[2];            //???
unsigned char SN[4];            //??
unsigned char RFID[16];			    //??RFID 
unsigned char lxl_bit=0;
unsigned char card1_bit=0;
unsigned char card2_bit=0;
unsigned char card3_bit=0;
unsigned char card4_bit=0;
unsigned char total=0;
unsigned char lxl[4]={196,58,104,217};
unsigned char card_1[4]={83,106,11,1};
unsigned char card_2[4]={208,121,31,57};
unsigned char card_3[4]={176,177,143,165};
unsigned char card_4[4]={5,158,10,136};
u8 KEY[6]={0xff,0xff,0xff,0xff,0xff,0xff};
u8 AUDIO_OPEN[6] = {0xAA, 0x07, 0x02, 0x00, 0x09, 0xBC};
unsigned char RFID1[16]={0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x07,0x80,0x29,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char status;
unsigned char s=0x08;

void RC522_Read_ID_Once(char *cardID)
{
	char Str1[20],Str2[20];
	u8 card_type[2];
	u8 card_ID[4];
	u8 statusRt;
	//PcdAntennaOn();
	statusRt = PcdRequest(PICC_REQIDL, card_type);//????????
	if(statusRt == MI_OK){
		sprintf ( Str1, "card_type: %02X%02X",
					card_type [ 0 ],
					card_type [ 1 ]); 
		if( PcdAnticoll (card_ID) == MI_OK){//?????
			sprintf ( Str2, "The Card ID is: %02X%02X%02X%02X",
					card_ID [ 0 ],
					card_ID [ 1 ],
					card_ID [ 2 ],
					card_ID [ 3 ] ); 
			if(PcdSelect(card_ID) == MI_OK){
				if(PcdHalt() == MI_OK){
					sprintf ( cardID, "%02X%02X%02X%02X", card_ID [ 0 ], card_ID [ 1 ], card_ID [ 2 ], card_ID [ 3 ] );
				}
			}
		}
	}
}
/**
  * @Name    RC522_Test
  * @brief   ????
  * @param   None
  * @retval
  * @author  NameisBoy
  * @Data    2020-09-04
 **/
void RC522_Test(void)
{
	char cStr [ 30 ], tStr[30];
  uint8_t ucArray_ID [ 4 ];                                                                                           
	uint8_t ucStatusReturn;   
  while ( 1 )
  { 
		if ( ( ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID ) ) != MI_OK )  
			ucStatusReturn = PcdRequest ( PICC_REQALL, ucArray_ID );
			
			if ( ucStatusReturn == MI_OK  )
			{	
					sprintf ( tStr, "The Card Type is: %02X%02X",
										ucArray_ID [ 0 ],
										ucArray_ID [ 1 ]);
									
					printf ( "%s\r\n",tStr ); 		
				if ( PcdAnticoll ( ucArray_ID ) == MI_OK )                                                                   
				{
					sprintf ( cStr, "The Card ID is: %02X%02X%02X%02X",
										ucArray_ID [ 0 ],
										ucArray_ID [ 1 ],
										ucArray_ID [ 2 ],
										ucArray_ID [ 3 ] );
									
					printf ( "%s\r\n",cStr ); 						
				}		
			}	
  }		
}


void RC522_GPIO_Config ( void )
{
	RC522_GPIO_Init();
	SPI2_Init(); 
	RC522_Reset_Disable();
	RC522_CS_Disable();
	PcdReset ();
    PcdAntennaOff(); 
	delay_us(1);
    PcdAntennaOn();               
	M500PcdConfigISOType ( 'A' );
}

/**
  * @Name    RC522_GPIO_Init
  * @brief   ???RC522 GPIO??
  * @param   None
  * @retval
  * @author  NameisBoy
  * @Data    2020-09-04
 **/
void RC522_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE );//PORTB???? 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;	 
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void SPI2_Init (void)	
{
    SPI_InitTypeDef  SPI_InitStructure; 

    RC522_CS_Disable();	

    SPI_Cmd(SPI2 , DISABLE); 

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);

    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;           
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                               
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                            
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;                                   
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;                                 
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                                    
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;         
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                          
    SPI_InitStructure.SPI_CRCPolynomial = 7;                                     

    SPI_Init(SPI2 , &SPI_InitStructure);


    SPI_Cmd(SPI2 , ENABLE); 
 }

void PcdReset ( void )
{
    RC522_Reset_Disable();
    delay_us ( 1 );
    RC522_Reset_Enable();
    delay_us ( 1 );
    RC522_Reset_Disable();
    delay_us ( 1 );
    WriteRawRC ( CommandReg, 0x0f );

    while ( ReadRawRC ( CommandReg ) & 0x10 );
	
    delay_us ( 1 );
    WriteRawRC ( ModeReg, 0x3D );                
    WriteRawRC ( TReloadRegL, 30 );               
    WriteRawRC ( TReloadRegH, 0 );			     
    WriteRawRC ( TModeReg, 0x8D );				 
    WriteRawRC ( TPrescalerReg, 0x3E );			 
    WriteRawRC ( TxAutoReg, 0x40 );				 	
}

u8 SPI_RC522_SendByte ( u8 byte )
{
    u8 retry=0;
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET) 
    {
        retry++;
        if(retry>200)return 0;
    }
    SPI_I2S_SendData(SPI2, byte); 
    retry=0;
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
    {
        retry++;
        if(retry>200)return 0;
    }
    return SPI_I2S_ReceiveData(SPI2); 
}

u8 ReadRawRC ( u8 ucAddress )
{
	u8 ucAddr, ucReturn;
	ucAddr = ( ( ucAddress << 1 ) & 0x7E ) | 0x80;      

	RC522_CS_Enable();
	SPI_RC522_SendByte ( ucAddr );
	ucReturn = SPI_RC522_ReadByte ();
	RC522_CS_Disable();
	return ucReturn;
}

void WriteRawRC ( u8 ucAddress, u8 ucValue )
{  
	u8 ucAddr;
	ucAddr = ( ucAddress << 1 ) & 0x7E;   
	
	RC522_CS_Enable();	
	SPI_RC522_SendByte ( ucAddr );
	SPI_RC522_SendByte ( ucValue );
	RC522_CS_Disable();	
}

void M500PcdConfigISOType ( u8 ucType )
{
	if ( ucType == 'A')                    
  {
		ClearBitMask ( Status2Reg, 0x08 );		

    WriteRawRC ( ModeReg, 0x3D );
		WriteRawRC ( RxSelReg, 0x86 );
		WriteRawRC ( RFCfgReg, 0x7F );   
		WriteRawRC ( TReloadRegL, 30 );
		WriteRawRC ( TReloadRegH, 0 );
		WriteRawRC ( TModeReg, 0x8D );
		WriteRawRC ( TPrescalerReg, 0x3E );
		delay_us   ( 2 );
		
		PcdAntennaOn ();
   }
}

void SetBitMask ( u8 ucReg, u8 ucMask )  
{
    u8 ucTemp;

    ucTemp = ReadRawRC ( ucReg );
    WriteRawRC ( ucReg, ucTemp | ucMask );         
}

void ClearBitMask ( u8 ucReg, u8 ucMask )  
{
    u8 ucTemp;
    ucTemp = ReadRawRC ( ucReg );
	
    WriteRawRC ( ucReg, ucTemp & ( ~ ucMask) );  
	
}

void PcdAntennaOn ( void )
{
    u8 uc;
    uc = ReadRawRC ( TxControlReg );
	
    if ( ! ( uc & 0x03 ) )
			SetBitMask(TxControlReg, 0x03);

}

void PcdAntennaOff ( void )
{
    ClearBitMask ( TxControlReg, 0x03 );
}

void ShowID(u16 x,u16 y, u8 *p, u16 charColor, u16 bkColor) 
{
    u8 num[9];

    printf("ID>>>%s\r\n", num);

}

char PcdComMF522 ( u8 ucCommand, u8 * pInData, u8 ucInLenByte, u8 * pOutData, u32 * pOutLenBit )		
{
    char cStatus = MI_ERR;
    u8 ucIrqEn   = 0x00;
    u8 ucWaitFor = 0x00;
    u8 ucLastBits;
    u8 ucN;
    u32 ul;

    switch ( ucCommand )
    {
       case PCD_AUTHENT:		
          ucIrqEn   = 0x12;		
          ucWaitFor = 0x10;		
          break;
			 
       case PCD_TRANSCEIVE:		
          ucIrqEn   = 0x77;		
          ucWaitFor = 0x30;		
          break;
			 
       default:
         break;
			 
    }
   
    WriteRawRC ( ComIEnReg, ucIrqEn | 0x80 );		
    ClearBitMask ( ComIrqReg, 0x80 );			
    WriteRawRC ( CommandReg, PCD_IDLE );
    SetBitMask ( FIFOLevelReg, 0x80 );	
    
    for ( ul = 0; ul < ucInLenByte; ul ++ )
		WriteRawRC ( FIFODataReg, pInData [ ul ] );    		
			
    WriteRawRC ( CommandReg, ucCommand );					
   
    
    if ( ucCommand == PCD_TRANSCEIVE )
			SetBitMask(BitFramingReg,0x80);  				
    
    ul = 1000;
		
    do 														
    {
         ucN = ReadRawRC ( ComIrqReg );							
         ul --;
    } while ( ( ul != 0 ) && ( ! ( ucN & 0x01 ) ) && ( ! ( ucN & ucWaitFor ) ) );		
		
    ClearBitMask ( BitFramingReg, 0x80 );					
		
    if ( ul != 0 )
    {
		if ( ! (( ReadRawRC ( ErrorReg ) & 0x1B )) )			
		{
			cStatus = MI_OK;
			
			if ( ucN & ucIrqEn & 0x01 )					
			  cStatus = MI_NOTAGERR;   
				
			if ( ucCommand == PCD_TRANSCEIVE )
			{
				ucN = ReadRawRC ( FIFOLevelReg );			
				
				ucLastBits = ReadRawRC ( ControlReg ) & 0x07;	
				
				if ( ucLastBits )
					* pOutLenBit = ( ucN - 1 ) * 8 + ucLastBits;   	
				else
					* pOutLenBit = ucN * 8;   					
				
				if ( ucN == 0 )	
                    ucN = 1;    
				
				if ( ucN > MAXRLEN )
					ucN = MAXRLEN;   
				
				for ( ul = 0; ul < ucN; ul ++ )
				  pOutData [ ul ] = ReadRawRC ( FIFODataReg );   
			}		
        }
			else
				cStatus = MI_ERR;   
    }
   
   SetBitMask ( ControlReg, 0x80 );           
   WriteRawRC ( CommandReg, PCD_IDLE ); 
	
   return cStatus;
}

char PcdRequest ( u8 ucReq_code, u8 * pTagType )
{
    char cStatus;  
    u8 ucComMF522Buf [ MAXRLEN ]; 
    u32 ulLen;

    ClearBitMask ( Status2Reg, 0x08 );	
    WriteRawRC ( BitFramingReg, 0x07 );
    SetBitMask ( TxControlReg, 0x03 );	

    ucComMF522Buf [ 0 ] = ucReq_code;		

    cStatus = PcdComMF522 ( PCD_TRANSCEIVE,	ucComMF522Buf, 1, ucComMF522Buf, & ulLen );	

    if ( ( cStatus == MI_OK ) && ( ulLen == 0x10 ) )	
    {    
       * pTagType = ucComMF522Buf [ 0 ];
       * ( pTagType + 1 ) = ucComMF522Buf [ 1 ];
    }
     
    else
     cStatus = MI_ERR;

    return cStatus;
}

char PcdAnticoll ( u8 * pSnr )
{
    char cStatus;
    u8 uc, ucSnr_check = 0;
    u8 ucComMF522Buf [ MAXRLEN ]; 
	  u32 ulLen;

    ClearBitMask ( Status2Reg, 0x08 );		
    WriteRawRC ( BitFramingReg, 0x00);		
    ClearBitMask ( CollReg, 0x80 );			
   
    ucComMF522Buf [ 0 ] = 0x93;	
    ucComMF522Buf [ 1 ] = 0x20;
   
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, & ulLen);
	
    if ( cStatus == MI_OK)		
    {
			for ( uc = 0; uc < 4; uc ++ )
			{
					* ( pSnr + uc )  = ucComMF522Buf [ uc ];			
					ucSnr_check ^= ucComMF522Buf [ uc ];
			}
			
        if ( ucSnr_check != ucComMF522Buf [ uc ] )
        		cStatus = MI_ERR;    		 
    }
    SetBitMask ( CollReg, 0x80 );

    return cStatus;
}

char PcdSelect ( u8 * pSnr )
{
    char ucN;
    u8 uc;
    u8 ucComMF522Buf [ MAXRLEN ]; 
    u32  ulLen;

    ucComMF522Buf [ 0 ] = PICC_ANTICOLL1;
    ucComMF522Buf [ 1 ] = 0x70;
    ucComMF522Buf [ 6 ] = 0;
    
    for ( uc = 0; uc < 4; uc ++ )
    {
        ucComMF522Buf [ uc + 2 ] = * ( pSnr + uc );
        ucComMF522Buf [ 6 ] ^= * ( pSnr + uc );
    }
        
    CalulateCRC ( ucComMF522Buf, 7, & ucComMF522Buf [ 7 ] );
    ClearBitMask ( Status2Reg, 0x08 );
    ucN = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, & ulLen );
    
    if ( ( ucN == MI_OK ) && ( ulLen == 0x18 ) )
      ucN = MI_OK;  
    else
      ucN = MI_ERR;    

    return ucN; 
}

void CalulateCRC ( u8 * pIndata, u8 ucLen, u8 * pOutData )
{
    u8 uc, ucN;

    ClearBitMask(DivIrqReg,0x04);
    WriteRawRC(CommandReg,PCD_IDLE);
    SetBitMask(FIFOLevelReg,0x80);
    
    for ( uc = 0; uc < ucLen; uc ++)
        WriteRawRC ( FIFODataReg, * ( pIndata + uc ) );   

    WriteRawRC ( CommandReg, PCD_CALCCRC );
    uc = 0xFF;

    do {
        ucN = ReadRawRC ( DivIrqReg );
        uc --;} 
    while ( ( uc != 0 ) && ! ( ucN & 0x04 ) );
        
    pOutData [ 0 ] = ReadRawRC ( CRCResultRegL );
    pOutData [ 1 ] = ReadRawRC ( CRCResultRegM );
    
}

char PcdAuthState ( u8 ucAuth_mode, u8 ucAddr, u8 * pKey, u8 * pSnr )
{
    char cStatus;
    u8 uc, ucComMF522Buf [ MAXRLEN ];
    u32 ulLen;

    ucComMF522Buf [ 0 ] = ucAuth_mode;
    ucComMF522Buf [ 1 ] = ucAddr;
    
    for ( uc = 0; uc < 6; uc ++ )
        ucComMF522Buf [ uc + 2 ] = * ( pKey + uc );   
    
    for ( uc = 0; uc < 6; uc ++ )
        ucComMF522Buf [ uc + 8 ] = * ( pSnr + uc );   

    cStatus = PcdComMF522 ( PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, & ulLen );
    
    if ( ( cStatus != MI_OK ) || ( ! ( ReadRawRC ( Status2Reg ) & 0x08 ) ) ){
            cStatus = MI_ERR; 
    }
		
    return cStatus;    
}

char PcdWrite ( u8 ucAddr, u8 * pData )
{
    char cStatus;
      u8 uc, ucComMF522Buf [ MAXRLEN ];
    u32 ulLen;

    ucComMF522Buf [ 0 ] = PICC_WRITE;
    ucComMF522Buf [ 1 ] = ucAddr;
    
    CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
 
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );

    if ( ( cStatus != MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
      cStatus = MI_ERR;   
        
    if ( cStatus == MI_OK )
    {
      memcpy(ucComMF522Buf, pData, 16);
      for ( uc = 0; uc < 16; uc ++ )
              ucComMF522Buf [ uc ] = * ( pData + uc );  
            
      CalulateCRC ( ucComMF522Buf, 16, & ucComMF522Buf [ 16 ] );

      cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, & ulLen );
            
            if ( ( cStatus != MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
        cStatus = MI_ERR;   
            
    } 

    return cStatus;
    
}

char PcdRead ( u8 ucAddr, u8 * pData )
{
    char cStatus;
    u8 uc, ucComMF522Buf [ MAXRLEN ]; 
    u32 ulLen;

    ucComMF522Buf [ 0 ] = PICC_READ;
    ucComMF522Buf [ 1 ] = ucAddr;
    
    CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
   
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );
    
    if ( ( cStatus == MI_OK ) && ( ulLen == 0x90 ) )
    {
            for ( uc = 0; uc < 16; uc ++ )
        * ( pData + uc ) = ucComMF522Buf [ uc ];   
    }
        
    else
      cStatus = MI_ERR;   
    
    return cStatus;

}

char PcdHalt( void )
{
    u8 ucComMF522Buf [ MAXRLEN ]; 
    u32  ulLen;

    ucComMF522Buf [ 0 ] = PICC_HALT;
    ucComMF522Buf [ 1 ] = 0;

    CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
    PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );

    return MI_OK;   
}
