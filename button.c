#include <reg52.H>
#include<stdio.h>
/*
 * For this case, I has put one switch button in P2.0 and one close button in P2.1 then
 * change the digit to next one when press switch button (P2.0), and close 7 segment LED 
 * when press close button (P2.1)
 */
//0-9, last is (dot)
unsigned char code dis_7[]={0xc0,0xf9,0xa4,0xb0,0x99,0x92,0x82,0xf8,0x80,0x98, 0x7f};
unsigned int i = 0;

//delay time function
void delay_ms(int time){
	int i,j;
	for(i=0; i< time; i++)
	for(j=0; j<1275;j++);

}

void display(){
	P0 = dis_7[i];
    i+=1;
}

int main(void){
	while(1){
        if(!P20)
            display();
        if(!P21)
            P0 = 0xff;
	}
	return 0;
}