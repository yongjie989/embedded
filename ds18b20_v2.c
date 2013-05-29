#include <8051.h>

// *******************************************
// 定義
// *******************************************
#define uchar unsigned char
#define nop() __asm nop __endasm;

#define LED			P2
#define SEG_LED		P2
#define SEG_SCAN	P1
#define DQ			P3_3

// *******************************************
// 公用變數
// *******************************************
code uchar segFont[]={0×3f,0×06,0×5b,0×4f,0×66,0×6d,0×7d,0×07,0×7f,0×6f,0×40,0×00};
                      63   6    91   79   102  109  125  7    127  111  64   0    
                      63 = 0011 1111 (0)
                      6  = 0000 0110 (1)
                      91 = 0101 1011 (2)
                      79 = 0100 1111 (3)
                      102= 0110 0110 (4)
                      109= 0110 1101 (5) 
                      125= 0111 1101 (6) 
                      7  = 0000 0111 (7)
                      127= 0111 1111 (8)
                      111= 0110 1111 (9)
                      64 = 0100 0000 (-)
                      0  = 0000 0000 (OFF LED)
// *******************************************
// 函式宣告
// *******************************************
void showSeg(uchar myVal);
void delay(uchar x);
void delay_10us(uchar x);
void DS18B20_init();
void DS18B20_wrbyte(uchar wrbyte);
uchar DS18B20_rdbyte();

// *******************************************
// 主程式
// *******************************************
void main(){

	uchar i,j,k;

	while(1){

		DS18B20_init();
		DS18B20_wrbyte(0xCC);
		DS18B20_wrbyte(0×44);

		DS18B20_init();
		DS18B20_wrbyte(0xCC);
		DS18B20_wrbyte(0xBE);
		i = DS18B20_rdbyte();
		j = DS18B20_rdbyte();

		for(k=0;k<10;k++)
			showSeg((j<<4)|(i>>4));
	}
}

// *******************************************
// 副程式
// *******************************************
void showSeg(uchar myVal){

	uchar FT[3]={
		segFont[(myVal)%10],
		segFont[(myVal/10)%10],
		segFont[(myVal/100)%10]
	};

	uchar i,j;
	for(i=0,j=7;i<3;i++,j–){
		SEG_SCAN	= (SEG_SCAN & 0xF8) | (j & 0×07);
		SEG_LED		= FT[i];
		delay(5);
	}

}

void delay(uchar x){
	uchar i,j;
	for(i=0;i<x;i++)
		for(j=0;j<120;j++);
}

void delay_10us(uchar x){
	while(x–>0){
		nop();	nop();
	}
}

void DS18B20_init(){
	DQ=0;	delay_10us(48);
	DQ=1;	delay_10us(5);

	while(DQ==1);
	delay_10us(20);

	DQ=1;	delay_10us(225);
}

void DS18B20_wrbyte(uchar wrbyte){
	uchar i,j=0;

	for(i=0;i<8;i++){
		DQ=0;						nop();
		DQ=(bit)(wrbyte & 0×01);	delay_10us(5);
		wrbyte>>=1;
		DQ=1;
	}
}

uchar DS18B20_rdbyte(){
	uchar i,rdbyte=0,j=0;

	for(i=0;i<8;i++){
		DQ=0;	nop();
		DQ=1;	nop();

		j=DQ;
		rdbyte=(j<<7)|(rdbyte>>1);	delay_10us(5);

		DQ=1;
    }

	return rdbyte;
}
