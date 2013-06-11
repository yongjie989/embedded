#include <reg52.H>
#include<stdio.h>

void serial(unsigned char x) {
	SBUF = x;
	while(TI==0);
	TI=0;
}

void rs_puts(char *str) {
	unsigned int i;
	for(i=0; str[i]!=0; i++) {
		serial(str[i]);
	}
}

void main(void){
	TMOD = 0x20;
	TH1 = 0xFD;
	SCON = 0x50;
	TR1 = 1;
	while(1==1){
		rs_puts("Hello, Yong Jie Huang!\n\r");
	}

}


