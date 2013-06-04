#include <reg52.H>
#include<stdio.h>
/*
 * For this case, we set up 8 LED in P1.0 - P1.7 to implemented effect of marquee
 */

//delay time function
void delay_ms(int time){
	int i,j;
	for(i=0; i< time; i++)
	for(j=0; j<1275;j++);

}


int main(void){
	while(1){
        P10 = 1; P11 = 0;
        delay_ms(100);
        P11 = 1; P12 = 0;
        delay_ms(100);
        P12 = 1; P13 = 0;
        delay_ms(100);
        P13 = 1; P14 = 0;
        delay_ms(100);
        P14 = 1; P15 = 0;
        delay_ms(100);
        P15 = 1; P16 = 0;
        delay_ms(100);
        P16 = 1; P17 = 0;
        delay_ms(100);
        P17 = 1; P10 = 0;
        delay_ms(100);
        
	}
	return 0;
}