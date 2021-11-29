#include <P18f4520.h>
#include <delays.h>
#include <math.h>
#include <usart.h>
#include <stdio.h>
#include <i2c.h>



#pragma config OSC = HS 
#pragma config LVP = OFF 
#pragma config WDT = OFF
#pragma config MCLRE = ON
#define RS_LCD      PORTAbits.RA1
#define RW_LCD      PORTAbits.RA2
#define EN_LCD      PORTAbits.RA3

#define DATA_LCD   PORTD

#define DHT_DATA = PORTAbits.RA0
#define SET_PIN TRISA
#define DHT_ER 0
#define DHT_OK 1
#define DHT_ND 0
#define DHT_DA 1
char M[30];

int nhietdo = 0, doam=0,nd=0,dm=0,nhietdo1 = 0, doam1=0,nhietdo2 = 0, doam2=0;
short Time_out;
int k=0,l=0,p=0,q=0,t=0,kt=0;
int i=0;
int hh,ss,mm,h,m,s;
char mang[32];
char a[7],c[7],d[7],g[7];
char b[7];
char x;
char chr;
int rh, temp;

void EEPROM_Write (int address, char data)
{
    /*Write Operation*/
    EEADR=address;	/* Write address to the EEADR register*/
    EEDATA=data;	/* Copy data to the EEDATA register for write */
    EECON1bits.EEPGD=0;	/* Access data EEPROM memory*/
    EECON1bits.CFGS=0;	/* Access flash program or data memory*/
    EECON1bits.WREN=1;	/* Allow write to the memory*/
    INTCONbits.GIE=0;	/* Disable global interrupt*/
    
    /* Below sequence in EECON2 Register is necessary 
    to write data to EEPROM memory*/
    EECON2=0x55;
    EECON2=0xaa;
    
    EECON1bits.WR=1;	/* Start writing data to EEPROM memory*/
    INTCONbits.GIE=1;	/* Enable interrupt*/
    
    while(PIR2bits.EEIF==0);/* Wait for write operation complete */
    PIR2bits.EEIF=0;	/* Reset EEIF for further write operation */
    
}

void EEPROM_WriteString(int address,char *data)
{
    /*Write Operation for String*/
    while(*data!=0)
    {
        EEPROM_Write(address,*data);
        address++;
        *data++;
    }    
}

char EEPROM_Read (int address)
{
    /*Read operation*/
    EEADR=address;	/* Read data at location 0x00*/
    EECON1bits.WREN=0;	/* WREN bit is clear for Read operation*/  
    EECON1bits.EEPGD=0;	/* Access data EEPROM memory*/
    EECON1bits.RD=1;	/* To Read data of EEPROM memory set RD=1*/
    return(EEDATA);
}

void doc_hhmmss (void)
{
	StartI2C();//bat dau giao tiep i2c
	IdleI2C();//kiem tra duong truyen i2c
	WriteI2C(0xd0);//gui dia chi ds1307, lenh ghi
	IdleI2C();
	WriteI2C(0x00);//gui dia chi s
	IdleI2C();
	RestartI2C();//reset khung truyen
	IdleI2C();
	WriteI2C(0xd1);//gui dia chi ds1307, lenh doc
	IdleI2C();
	ss=ReadI2C();//doc gia tri s
	IdleI2C();
	AckI2C();//gui bits ACK
	IdleI2C();
	mm=ReadI2C();//doc gia tri m
	IdleI2C();
	AckI2C();//gui bits ACK
	IdleI2C();
	hh=ReadI2C();//doc gia tri h
	IdleI2C();
	NotAckI2C();//gui bits NACK, ket thuc
	IdleI2C();
	StopI2C();//ket thuc khung truyen
}
char INT_BCD(char temp)
{
	char ch,dv,kq;
	ch=temp/10;
	dv=temp%10;
	kq=(ch<<4)+dv;
	return kq;
}
void ghi_hhmmss(char h, char m, char s)
{
	StartI2C();
	IdleI2C();
	WriteI2C(0xd0);
	IdleI2C();
	WriteI2C(0x00);//dia chi thanh ghi Seconds
	IdleI2C();
	s=INT_BCD(s);
	WriteI2C(s);//giay
	IdleI2C();
	m=INT_BCD(m);
	WriteI2C(m);//phut
	IdleI2C();
	h=INT_BCD(h);
	WriteI2C(h);
	IdleI2C();
	StopI2C();	
}

int BCD_INT (int temp)
{
	temp=(((temp&0x70)>>4)*10)+(temp&0x0f);
	return temp;
}
unsigned int CKL,CKH,CK;


unsigned int DHT_GetTemHumi (unsigned char select)
{

unsigned char buffer[5]={0,0,0,0,0};
unsigned char ii,i,checksum;
char data = 0; 

SET_PIN=(0x00);// set la cong ra
PORTAbits.RA0 = 0;
Delay1KTCYx(25000); // it nhat 18ms
SET_PIN=(0xff);



while(PORTAbits.RA0);      
while(!(PORTAbits.RA0)); 
while(PORTAbits.RA0);     

//Bat dau doc du lieu


for(i=0;i<5;i++)
{
for(ii=0;ii<8;ii++)
{

while(PORTAbits.RA0==0);
Delay10TCYx(10); //50
if(PORTAbits.RA0==1)
{
buffer[i]|=(1<<(7-ii));

}
while(PORTAbits.RA0==1);
}
}


//Tinh toan check sum
checksum=buffer[0]+buffer[1]+buffer[2]+buffer[3];
//Kiem tra check sum
if((checksum)!=buffer[4])return DHT_ER;
//Lay du lieu
if (select==DHT_ND)
{
//Return the value has been choosen
temp = buffer[2]*256+buffer[3];
return(temp);
}
else if(select==DHT_DA)
{
rh = buffer[0]*256+buffer[1];
return(rh);
}
return DHT_OK;
}

void docchuoi(char chuoi[]){
	char temp;
	

}

void main()
{
	TRISD = 0x00;
	TRISC = 0x90;
	ADCON1 = 0x0f;
	OpenUSART( USART_TX_INT_OFF &
         USART_RX_INT_ON &
         USART_SYNC_SLAVE &
         USART_ASYNCH_MODE &
         USART_EIGHT_BIT &
         USART_CONT_RX &
         USART_BRGH_HIGH, 51);
		
	OpenI2C(MASTER,SLEW_OFF);
	SSPADD=199; 
	hh=0;
	mm=0;
	ss=0;
	nd=EEPROM_Read (0x02);
	dm=EEPROM_Read (0x01);
	k=10;
	l=0;
	p=0;
	kt=0;
	while(1)
	{
		

		nhietdo1 = DHT_GetTemHumi(DHT_ND);
		doam1= DHT_GetTemHumi (DHT_DA);
		nhietdo2=nhietdo1%10;
		doam2=doam1%10;
		nhietdo=nhietdo1/10;
		doam=doam1/10;

		doc_hhmmss();
		ss=BCD_INT(ss);
		hh=BCD_INT(hh);
		mm=BCD_INT(mm);
		
		x=ReadUSART();
		while(BusyUSART());
		if(x!=0){
		if(x=='a'){
			getsUSART(a,1);
		}
		if(x=='b'){
			PORTDbits.RD0=0;
			m=0;
			a[0]=0;
		}
		if(x=='c'){
			getsUSART(c,1);
			nd=c[0];
			EEPROM_Write (0x02, nd);
		}
		if(x=='d'){
			getsUSART(d,1);
			dm=d[0];
			EEPROM_Write (0x01, dm);
		}
		if(x=='g'){
			c[0]=0;
			d[0]=0;
			a[0]=0;
			dm=0;
			nd=0;
			EEPROM_Write (0x02, 0);
			EEPROM_Write (0x01, 0);
			m=0;
			PORTDbits.RD0=0;
		}
		if(x=='h')
			l=1;
		if(x=='p')
			p=1;
		if(x=='v')
			kt=1;
		if(x=='i')
			kt=0;
		
		}
		if(kt==1){
			if(k==10&&l==1&&p==0){
				if(PORTDbits.RD0==1){
					sprintf(&M[0],"a@%d.%d@%d.%d",nhietdo,nhietdo2,doam,doam2);
					putsUSART(&M[0]);
					while(BusyUSART());
				}
				else
				{
					if(t==1){
						sprintf(&M[0],"b@%d.%d@%d.%d@%d@%d@%d@",nhietdo,nhietdo2,doam,doam2,nd,dm,a[0]);
						putsUSART(&M[0]);
						while(BusyUSART());
						t=0;
					}
					else
					sprintf(&M[0],"b@%d.%d@%d.%d",nhietdo,nhietdo2,doam,doam2);
					putsUSART(&M[0]);
					while(BusyUSART());
					
				}
			k=0;
			}
			
			if(l==0&&p==0){
				if(PORTDbits.RD0==1){
				sprintf(&M[0],"a@%d.%d@%d.%d@%d@%d@%d@",nhietdo,nhietdo2,doam,doam2,nd,dm,a[0]);
				putsUSART(&M[0]);
				while(BusyUSART());
				
				}
				else{
				sprintf(&M[0],"b@%d.%d@%d.%d@%d@%d@%d@",nhietdo,nhietdo2,doam,doam2,nd,dm,a[0]);
				putsUSART(&M[0]);
				while(BusyUSART());
				}
	
			k=0;
			}
			if(l==1&&p==1){
				if(PORTDbits.RD0==1){
				sprintf(&M[0],"a@%d.%d@%d.%d@%d@%d@%d@",nhietdo,nhietdo2,doam,doam2,nd,dm,a[0]);
				putsUSART(&M[0]);
				while(BusyUSART());
				
				}
				else{
				sprintf(&M[0],"b@%d.%d@%d.%d@%d@%d@%d@",nhietdo,nhietdo2,doam,doam2,nd,dm,a[0]);
				putsUSART(&M[0]);
				while(BusyUSART());
				}
			p=0;
			k=0;
			l=0;
			}
		}

		
		if(a[0]>0){
			if(q==0){
				m=a[0]+ss;
				PORTDbits.RD0=1;
				if(m>60)
					m=m-60;
				q=1;
			}
			if(m==ss){
				PORTDbits.RD0=0;
				a[0]=0;
				m=0;
				q=0;
				t=1;
			}
		}
		
		if(nd>0&&dm>0){
			if(nhietdo>nd||doam<dm||m<mm){
				PORTDbits.RD0=1;
				a[0]=10;
			}
			else
				PORTDbits.RD0=0;
		}
			

		k++;
			
	}
}