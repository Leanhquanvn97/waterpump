// khai bao thu vien
#include<P18F4520.h> 
#include<delays.h>
#include<stdio.h>
#include<i2c.h>

// cau hinh phan cung cho PIC
#pragma config OSC = HS 
#pragma config LVP = OFF 
#pragma config WDT = OFF
#pragma config MCLRE = ON

#define RS_LCD		PORTAbits.RA0
#define RW_LCD		PORTAbits.RA1
#define EN_LCD		PORTAbits.RA2

#define DATA_LCD	PORTD
// Khai bao bien, mang du lieu
int x;
int hh,ss,mm;
char mang[32];

void ghilenh (char lenh)
{
	RS_LCD=0;
	RW_LCD=0;
	EN_LCD=1;
	DATA_LCD=lenh;
	EN_LCD=0;
	Delay1KTCYx(5);
}
void ghidulieu (char dulieu)
{
	RS_LCD=1; // 
	RW_LCD=0;
	EN_LCD=1;
	DATA_LCD=dulieu;
	EN_LCD=0;
	Delay1KTCYx(5);
}
void cauhinhLCD(void)
{
	ghilenh(0x03); //1
	ghilenh(0x38); //2
	ghilenh(0x06);
	ghilenh(0x0f);
	ghilenh(0x14);
	ghilenh(0x01);
}
void ghichuoi (char *str)
{
	while(*str)
	{
		ghidulieu(*str);
		str++;	
	}
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

void main (void) // chuong trinh chinh
{
// Cau hinh ban dau (vao/ra, Timer...vv; )
TRISE=0b00000000; //RE0:RE2 - chieu ra
TRISB=0b00001111; //RB0:RB3 - chieu vao
TRISD=0x00;
TRISA=0x00;
TRISC=0X00;
ADCON1=0b00001111; //
	OpenI2C(MASTER,SLEW_OFF);
	SSPADD=99; 
	cauhinhLCD();
	hh=0;
	mm=0;
	ss=0;
	ghi_hhmmss(15,19,58);
	ghilenh(0x80);
	sprintf(&mang[0],"hh : mm : ss");
	ghichuoi(&mang[0]);
	while(1) // Cac lenh dieu khie
	{
		doc_hhmmss();
		ss=BCD_INT(ss);
		hh=BCD_INT(hh);
		mm=BCD_INT(mm);

		ghilenh(0xC0);
		sprintf(&mang[0],"%2d:%2d:%2d  ",hh,mm,ss);
		ghichuoi(&mang[0]);
		Delay10KTCYx(50); 
	}
}