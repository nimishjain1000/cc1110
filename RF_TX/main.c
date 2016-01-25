

#include "ioCC1110.h"

#include "rf_cc1110.h"

#include "hal_cc1110.h"
extern void InitUART();
extern void UartSendString(INT8U *data,int len);
#define uchar unsigned char
#define LED1  P1_0
#define LED2  P1_1
#define KEY1  P1_3
#define KEY2  P1_2
#define MYADDR  0x08
#define KEY_NO_KEY		0
#define KEY_SHORT_PRESS	1
#define KEY_LONG_PRESS	2
#define KEY_DOWN		3
typedef struct rfInfo
{
	uchar deviceID;
	uchar userID;
	uchar checkSum;
	uchar data[7];
} RFINFO;


INT8U	temp;	//the tmpword
INT8U	sendFlag;	//the key Flag
INT8U	keyDown;	//key down
INT8U	keyUp;		//key up
INT8U	keyCount;	//key down count
INT8U	test[10]= {1,2,3,4,5,6,7,8,9,10};
RFINFO	sendData;
#pragma vector = RF_VECTOR
 __interrupt void RF_ISR1(void)
 {
    S1CON &= ~0x03;
 }
 
 
 #pragma vector = ST_VECTOR
 __interrupt void SLEEP_TIMER_ISR(void)
 {
    IRCON &= ~( 1<<7 ); 
    WORIRQ &= ~( 1<<0 );
 }
#pragma vector=URX0_VECTOR
__interrupt void UART0_ISR(void)
{
	URX0IF=0;
	temp=U0DBUF;
}
void Delay(INT16U n)
{
	INT16U i;
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
}
void ioInit()
{
	P1DIR|=0x03;
	P1DIR&=0xF3;  //KEY1 =p1_3  KEY2 =P1_2
	LED1=1;
	LED2=1;
	sendData.deviceID=MYADDR;
	sendData.userID=0x01;
	sendData.data[0]=0x48;

}
INT8U getKeyCode()
{
	INT8U ret=KEY_NO_KEY;	//default key no key
	if(KEY1==0)
	{
		keyDown=1;
		keyCount++;
		ret=KEY_DOWN;		//key down
	}else
	{
		if(keyDown==1)
		{
			if(keyCount<100)
				ret= KEY_SHORT_PRESS;	//short press
			else
				ret= KEY_LONG_PRESS;	//Long press
			keyDown=0;
			keyCount=0;
		
		}
	}
	return ret;
	


}
void handleShortKeyEvent()
{
	while(MARCSTATE != 0x01);
	rf_send_packet((uchar *)&sendData, sizeof(sendData));

	LED2=0;
	Delay(5000);
	LED2=1;
}
void handleLongKeyEvent()
{

}
int main( void )
{
    INT8U  rssi, lqi, len;
    INT8U  buffer[10];
//	INT8U	keyCode;
    volatile INT32U  delay;
	sendFlag=0;
  //  P1DIR |= ( 1<<0 ) | ( 1<<1 );// ����P1_0, P1_1Ϊ���   
	
    CLKCON &= ~( 1<<7 );          //32.768
    CLKCON &= ~0x40;              //����
    while(!(SLEEP & 0x40));      //�ȴ������ȶ�
    CLKCON &= ~0x47;             //
    SLEEP |= 0x04; 		 //�رղ��õ�RC����
    
    RFIM = 0xFF;
	ioInit();
    rf_cc1110_init( 433000 );
    ADDR=MYADDR;    //Set the RF Device ADDRESS
      
    WORCTL |= 0x00;    //2^15 period
    IEN0 |= 0X20;     //���ж�
    WORIRQ |= 0X10;   //

    EA = 1;
        SLEEP &= ~0x07;
    SLEEP |= 0x01;
    WORCTL |= 0x04;
    PCON |= 0x01;     //����PM1ģʽ
  
    while(!(SLEEP & 0x40));      //�ȴ������ȶ�
    SIDLE();
    SCAL();
  
  //ѭ�����䣬���շ��յ���LED�ᷭת
  //�˴�����sleep timer������ʵ����Ŀ�зǳ����ã�ʱ��ԴΪ�ⲿ32768�߾��Ⱦ���
  while( 1 )
  {

  	Delay(100);
//	keyCode=getKeyCode();
	switch(getKeyCode())
	 {
		case KEY_DOWN:
			LED2=0;
			break;
		case KEY_SHORT_PRESS:
			handleShortKeyEvent();
			LED2=1;
			break;
		case KEY_LONG_PRESS:
			handleLongKeyEvent();
			LED2=1;
			break;
		default:
			break;
	 }

	for( delay = 0; delay < 4; delay ++ )
    {
      WORCTL |= 0x04;
      len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
 	  if( len == 10 )   { break; } 
    }
	if(delay<4)
	  {
		sendFlag=0;
		LED1=0;
	  }
	else
	  {
		sendFlag=0;
		
	  }
    
  }
 }


/*
    for( delay = 0; delay < 20000; delay ++ );
    rf_send_packet( test, 10 );     //
    P1 |= ( 1<<0 );
    //ÿ����һ�Σ���˸һ�Ρ�
      
    for( delay = 0; delay < 2; delay ++ )
    {
      WORCTL |= 0x04;
      len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
      if( len == 10 )   { break; } 
    }
    if( delay >= 2 ) { P1 |= ( 1<<1 ); } //�����ʱ��˵��û�յ�Ӧ�����
    else             { P1 &= ~( 1<<1 ); }//�յ�Ӧ������
    
  }
  */
