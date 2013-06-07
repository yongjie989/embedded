

#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

sbit DQ = P1^2 ;  //�w�qDS18B20�ݤfDQ
sbit BEEP=P1^3  ; //���ﾹ�X�ʽu

bit presence  ;

sbit LCD_RS = P2^2 ;
sbit LCD_RW = P2^3 ;
sbit LCD_EN = P2^4 ;

uchar code  cdis1[ ] = {" Yong Jie Huang "} ;
uchar code  cdis2[ ] = {" Temp =    .  C "} ;
uchar code  cdis3[ ] = {" DS18B20  ERR0R "} ;
uchar code  cdis4[ ] = {"  PLEASE CHECK  "} ;

unsigned char data  temp_data[2] = {0x00,0x00} ;
unsigned char data  display[5] =   {0x00,0x00,0x00,0x00,0x00} ;
unsigned char code  ditab[16] =    {0x00,0x01,0x01,0x02,0x03,0x03,0x04,0x04,
                                                        0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09} ;///*�p�Ʀ��A�|�ˤ��J*/
//
void beep() ;
unsigned char code  mytab[8] = {0x0C,0x12,0x12,0x0C,0x00,0x00,0x00,0x00} ;

#define delayNOP() ; {_nop_() ;_nop_() ;_nop_() ;_nop_() ;} ;

/*******************************************************************/
void delay1(int ms)
{
 unsigned char y ;
  while(ms--)
 {
  for(y = 0 ; y<250 ; y++)
  {
   _nop_() ;
   _nop_() ;
   _nop_() ;
   _nop_() ;
  }
 }
}

/******************************************************************/
/*�ˬdLCD�����A                                                   */
/*lcd_busy��1�ɡA���A���ݡClcd-busy��0��,���A�i�g���O�P�ƾڡC   */
/******************************************************************/
bit lcd_busy()
 {
    bit result ;
    LCD_RS = 0 ;
    LCD_RW = 1 ;
    LCD_EN = 1 ;
    delayNOP() ;
    result = (bit)(P0&0x80) ;
    LCD_EN = 0 ;
    return(result) ;
 }

/*�g���O�ƾڨ�LCD                                                  */
/*RS=L�ARW=L�AE=���߽ġAD0-D7=���O�X�C          */
/*******************************************************************/
void lcd_wcmd(uchar cmd)
{
   while(lcd_busy()) ;
    LCD_RS = 0 ;
    LCD_RW = 0 ;
    LCD_EN = 0 ;
    _nop_() ;
    _nop_() ;
    P0 = cmd ;
    delayNOP() ;
    LCD_EN = 1 ;
    delayNOP() ;
    LCD_EN = 0 ;
}

/*******************************************************************/
/*�g��ܼƾڨ�LCD                            */
/*RS=H�ARW=L�AE=���߽ġAD0-D7=�ƾڡC       */
/*******************************************************************/
void lcd_wdat(uchar dat)
{
   while(lcd_busy()) ;
    LCD_RS = 1 ;
    LCD_RW = 0 ;
    LCD_EN = 0 ;
    P0 = dat ;
    delayNOP() ;
    LCD_EN = 1 ;
    delayNOP() ;
    LCD_EN = 0 ;
}


/*  LCD��l�Ƴ]�w                                                  */
/*******************************************************************/
void lcd_init()
{
    delay1(15) ;
    lcd_wcmd(0x01) ;      //�M��LCD����ܤ��e
    lcd_wcmd(0x38) ;      //16*2��ܡA5*7�I�}�A8��ƾ�
    delay1(5) ;
    lcd_wcmd(0x38) ;
    delay1(5) ;
    lcd_wcmd(0x38) ;
    delay1(5) ;

    lcd_wcmd(0x0c) ;      //��ܶ}�A������
    delay1(5) ;
    lcd_wcmd(0x06) ;      //���ʥ���
    delay1(5) ;
    lcd_wcmd(0x01) ;      //�M��LCD����ܤ��e
    delay1(5) ;
}

/*  �]�w��ܦ�m                                    */
/*******************************************************************/
void lcd_pos(uchar pos)
{
  lcd_wcmd(pos | 0x80) ;  //�ƾګ��w=80+�a�}�ܶq
}

/*�۩w�q�r�żg�JCGRAM                                   */
/*******************************************************************/
void  writetab()
{
    unsigned char i ;
    lcd_wcmd(0x40) ;            //�gCGRAM
    for (i = 0 ; i< 8 ; i++)
    lcd_wdat(mytab[ i ]) ;
}

/*us�ũ��ɨ��                                        */
/*******************************************************************/

void Delay(unsigned int num)
{
  while( --num ) ;
}

/*��l��ds1820                                      */
/*******************************************************************/
Init_DS18B20(void)
{
     DQ = 1 ;      //DQ�_��
     Delay(8) ;    //�y������

     DQ = 0 ;      //������NDQ�ԧC
     Delay(90) ;   //��T���� �j�� 480us

     DQ = 1 ;       //�԰��`�u
     Delay(8) ;

     presence = DQ ;    //�p�G=0�h��l�Ʀ��\ =1�h��l�ƥ���
     Delay(100) ;
     DQ = 1 ;

     return(presence) ; //��^�H���A0=presence,1= no presence
}


/* Ū�@�Ӧr�`                     */
/*******************************************************************/
 ReadOneChar(void)
{
unsigned char i = 0 ;
unsigned char dat = 0 ;

for (i = 8 ; i > 0 ; i--)
  {
    DQ = 0 ; // ���߽īH��
    dat >>= 1 ;
    DQ = 1 ; // ���߽īH��

    if(DQ)
     dat |= 0x80 ;
    Delay(4) ;
  }

    return (dat) ;
}

/* �g�@�Ӧr�`                                         */
/*******************************************************************/
 WriteOneChar(unsigned char dat)
{
  unsigned char i = 0 ;
  for (i = 8 ; i > 0 ; i--)
  {
    DQ = 0 ;
    DQ = dat&0x01 ;
    Delay(5) ;

    DQ = 1 ;
    dat>>=1 ;
  }
}

/* Ū���ū�                            */
/*******************************************************************/
 Read_Temperature(void)
{
     Init_DS18B20() ;

     WriteOneChar(0xCC) ;  // ���LŪ�Ǹ��C�����ާ@
     WriteOneChar(0x44) ;  // �Ұʷū��ഫ

     Init_DS18B20() ;
     WriteOneChar(0xCC) ;  //���LŪ�Ǹ��C�����ާ@
     WriteOneChar(0xBE) ;  //Ū���ūױH�s��

     temp_data[0] = ReadOneChar() ;   //�ūקC8��
     temp_data[1] = ReadOneChar() ;   //�ūװ�8��
}

/* �ƾ��ഫ�P�ū����                                              */
/*******************************************************************/
 Disp_Temperature()
{
  display[4]=temp_data[0]&0x0f ;
  display[0]=ditab[display[4]]+0x30 ;     //�d��o�p�Ʀ쪺��

  display[4]=((temp_data[0]&0xf0)>>4)|((temp_data[1]&0x0f)<<4) ;
  display[3]=display[4]/100+0x30 ;
  display[1]=display[4]%100 ;
  display[2]=display[1]/10+0x30 ;
  display[1]=display[1]%10+0x30 ;

    if(display[3]==0x30)        //���쬰0�A�����
   {
     display[3]=0x20 ;
     if(display[2]==0x30)      //�����쬰0�A�����
     display[2]=0x20 ;
   }

     lcd_pos(0x48) ;
     lcd_wdat(display[3]) ;        //�ʦ�����
     lcd_pos(0x49) ;
     lcd_wdat(display[2]) ;        //�Q������
  lcd_pos(0x4a) ;
     lcd_wdat(display[1]) ;        //�Ӧ�����
  lcd_pos(0x4c) ;
     lcd_wdat(display[0]) ;        //�p�Ʀ�����
}

/*******************************************************************/
/* ���ﾹ�T�@�n                 */
/*******************************************************************/
void beep()
  {
    unsigned char y ;
    for (y=0 ;y<100 ;y++)
    {
      Delay(60) ;
      BEEP=!BEEP ;                //BEEP����
    }
    BEEP=1 ;                      //�������ﾹ
 Delay(40000) ;
  }

/* DS18B20 OK ��ܵ��                                             */
/*******************************************************************/
void  Ok_Menu ()
{
    uchar  m ;
    lcd_init() ;                //��l��LCD

    lcd_pos(0) ;                //�]�m��ܦ�m���Ĥ@�檺��1�Ӧr��
     m = 0 ;
    while(cdis1[m] != '\0')
     {                         //��ܦr��
       lcd_wdat(cdis1[m]) ;
       m++ ;
     }

    lcd_pos(0x40) ;             //�]�m��ܦ�m���ĤG���1�Ӧr��
     m = 0 ;
    while(cdis2[m] != '\0')
     {
       lcd_wdat(cdis2[m]) ;      //��ܦr��
       m++ ;
     }

     writetab() ;               //�۩w�q�r�żg�JCGRAM
     delay1(5) ;
     lcd_pos(0x4d) ;
     lcd_wdat(0x00) ;          //��ܦ۩w�q�r��
}


/* DS18B20 ERROR ��ܵ��                             */
/*******************************************************************/
void  Error_Menu ()
{
     uchar  m ;
     lcd_init() ;                //��l��LCD

    lcd_pos(0) ;                //�]�m��ܦ�m���Ĥ@�檺��1�Ӧr��
     m = 0 ;
     while(cdis3[m] != '\0')
     {                         //��ܦr��
       lcd_wdat(cdis3[m]) ;
       m++ ;
     }

     lcd_pos(0x40) ;             //�]�m��ܦ�m���ĤG���1�Ӧr��
     m = 0 ;
     while(cdis4[m] != '\0')
     {
       lcd_wdat(cdis4[m]) ;      //��ܦr��
       m++ ;
     }
}


/* �D���     */
/************************************/
void main()
 {
     Ok_Menu () ;

 do
  {
      Read_Temperature() ;
   Disp_Temperature() ;
     }
    while(!presence)  ;

     Error_Menu () ;

 do
  {
    Init_DS18B20() ;
    beep() ;
     }
 while(presence) ;
}

