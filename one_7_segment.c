#include <reg52.H>
#include<stdio.h>

//0-9, last is (dot)
unsigned char code dis_7[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x98, 0x7f};

//delay time function
void delay_ms(int time){
	int i,j;
	for(i=0; i< time; i++)
	for(j=0; j<1275;j++);

}

void display(){
	unsigned int i;
	for(i=0; i<sizeof(dis_7); i++){
		P0 = dis_7[i];
		delay_ms(200);
	}
}

int main(void){
	while(1){
		display();
	}
	return 0;
}
