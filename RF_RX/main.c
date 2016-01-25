

#include "ioCC1110.h"
#include "rf_cc1110.h"
#define uint unsigned int
#define uchar unsigned char
#define led1	P1_0
#define led2	P1_1
#define key1    P1_3
#define key2    P1_2
INT8U   RFFlag;
void Delay(uint);
void InitUART(void);
void InitialAD(void);
void UartSendString(uchar *data,int len);
uchar Recdata[30]="hdy test";
uchar RFlag=1;
uchar temp;
uint datanumber=0;
uint stringlen;
void Delay(uint n)
{
	uint i;
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
	for(i=0;i<n;i++);
}
void InitUART(void)
{
	CLKCON&=~0x40;
	while(!(SLEEP&0x40));
	CLKCON&=~0x47;
	SLEEP|=0x04;
	PERCFG=0x00;
	P0SEL=0x3C;	//P0 User as uart
	U0CSR|=0x80;	//UART Model
	U0GCR|=11;
	U0BAUD|=34;
	UTX0IF=1;
	U0CSR|=0x40;
	IEN0|=0x84;
}
//Send string from uart
void UartSendString(uchar *data,int len)
{
	int j;
	for(j=0;j<len;j++)
	{
		U0DBUF=*data++;
		while(UTX0IF==0);
		UTX0IF=0;
	}
}

volatile INT8U len;
    int main( void )
{
    INT8U buffer[10], rssi, lqi;
    CLKCON &= ~0x40;              //����
    while(!(SLEEP & 0x40));      //�ȴ������ȶ�
    CLKCON &= ~0x47;             //TICHSPD128��Ƶ��CLKSPD����Ƶ
    SLEEP |= 0x04; 		 //�رղ��õ�RC����
    P1DIR=0x03;
    led1=1;
    led2=1;
    RFFlag=0;
    InitUART();
//    stringlen=strlen((uchar *)Recdata);
    UartSendString(Recdata,10);
    rf_cc1110_init( 433000 );
    IEN0 = 0x81;
    
    while( 1 ){
        
     if(RFFlag)
       UartSendString("leaf",5); 
      len = rf_rec_packet(buffer, &rssi, &lqi, 240) ;
    
     if( len == 10 )
     {
         P1 ^= 0x03    ;        //�յ����ݣ�LED��תһ��
         rf_send_packet( buffer, 10 );//����Ӧ���ź�
         UartSendString(buffer,10); 
     }
     len = 0;
    }
  
  
  

}