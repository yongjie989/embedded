#include <intrins.h>	
#include <AT89C5131.h>	
#include <stdio.h>
#include <intrins.h>
//http://www.maximintegrated.com/app-notes/index.mvp/id/162

#define FALSE 0
#define TRUE 1
////////////////////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
//
unsigned char ROM[8]; // ROM Bit
unsigned char lastDiscrep = 0; // last discrepancy
unsigned char doneFlag = 0; // Done flag
unsigned char FoundROM[5][8]; // table of found ROM codes
unsigned char numROMs;
unsigned char dowcrc;

//(us - 15) / 8
void delay(unsigned int uSek)
{   for( ;uSek!=0;uSek--){}
}

bit ow_reset(void) {
   bit presence;
   P3_7 = 0;
   delay(59);
   P3_7 = 1;
   delay(8);
   presence = P3_7;
   delay(52);
   return(presence); 
}

unsigned char read_bit(void) {
   unsigned char i;
   P3_7 = 0;
   P3_7 = 1; 
   for (i=0; i<3; i++);
   /*_nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();
   _nop_();_nop_();_nop_();*/
   return(P3_7); 
}

void write_bit(char bitval) {
   P3_7 = 0; 
   if(bitval==1) P3_7 = 1; 
   delay(11); 
   P3_7 = 1;
}

unsigned char read_byte(void) {
   unsigned char i;
   unsigned char value = 0;
   for (i=0;i<8;i++) {
      if(read_bit()) value|=0x01<<i; 
      delay(13); 
   }
   return(value);
}

void write_byte(char val)
{
   unsigned char i;
   unsigned char temp;
   for (i=0; i<8; i++) {
      temp = val>>i; 
      temp &= 0x01; 
      write_bit(temp); 
   }
   delay(11);
}

void Read_Temperature(void)
{
   char get[10];
   char temp_lsb,temp_msb;
   int k,temperatur;
   char temp_f,temp_c;
   
   ow_reset();
   write_byte(0xCC); //1100 1100
   write_byte(0x44); //0100 0100
   delay(11);
   
   ow_reset();
   write_byte(0xCC); //1100 1100
   write_byte(0xBE); //1011 1110
   
   for (k=0;k<9;k++){
      get[k]=read_byte();
   }
   
   temp_msb = get[1]; 
   temp_lsb = get[0];
   temperatur = ((signed int)temp_msb << 8) | temp_lsb;
   temperatur /= 16;
   printf("get0 %x\tget1 %x \n", get[0], get[1]);
   printf("%d C \n ", temperatur);
}


void main () {
unsigned char x;
   PCON |= 0x80;
   TH1 = 243;
   TL1 = 243;
   TMOD = 0x20;
   TR1 = 1;
   SCON = 0x52;
   
   P2=0;
   ow_reset();
   write_byte(0xCC);   // Skip ROM, keinen speziellen Adressieren
   write_byte(0x4E);   // Write Scratchpad (3 Bytes mussen folgen)
   write_byte(0b01011111);
   while (1) {
       Read_Temperature();  
   }
}