

#include < reg51.h >
#include < intrins.h >

#define uchar unsigned char
#define uint  unsigned int

sbit DQ = P1^2 ;  //定義DS18B20端口DQ
sbit BEEP=P1^3  ; //蜂鳴器驅動線

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
                                                        0x05,0x06,0x06,0x07,0x08,0x08,0x09,0x09} ;///*小數位表，四捨五入*/
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
/*檢查LCD忙狀態                                                   */
/*lcd_busy為1時，忙，等待。lcd-busy為0時,閒，可寫指令與數據。   */
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

/*寫指令數據到LCD                                                  */
/*RS=L，RW=L，E=高脈衝，D0-D7=指令碼。          */
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
/*寫顯示數據到LCD                            */
/*RS=H，RW=L，E=高脈衝，D0-D7=數據。       */
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


/*  LCD初始化設定                                                  */
/*******************************************************************/
void lcd_init()
{
    delay1(15) ;
    lcd_wcmd(0x01) ;      //清除LCD的顯示內容
    lcd_wcmd(0x38) ;      //16*2顯示，5*7點陣，8位數據
    delay1(5) ;
    lcd_wcmd(0x38) ;
    delay1(5) ;
    lcd_wcmd(0x38) ;
    delay1(5) ;

    lcd_wcmd(0x0c) ;      //顯示開，關光標
    delay1(5) ;
    lcd_wcmd(0x06) ;      //移動光標
    delay1(5) ;
    lcd_wcmd(0x01) ;      //清除LCD的顯示內容
    delay1(5) ;
}

/*  設定顯示位置                                    */
/*******************************************************************/
void lcd_pos(uchar pos)
{
  lcd_wcmd(pos | 0x80) ;  //數據指針=80+地址變量
}

/*自定義字符寫入CGRAM                                   */
/*******************************************************************/
void  writetab()
{
    unsigned char i ;
    lcd_wcmd(0x40) ;            //寫CGRAM
    for (i = 0 ; i< 8 ; i++)
    lcd_wdat(mytab[ i ]) ;
}

/*us級延時函數                                        */
/*******************************************************************/

void Delay(unsigned int num)
{
  while( --num ) ;
}

/*初始化ds1820                                      */
/*******************************************************************/
Init_DS18B20(void)
{
     DQ = 1 ;      //DQ復位
     Delay(8) ;    //稍做延時

     DQ = 0 ;      //單片機將DQ拉低
     Delay(90) ;   //精確延時 大於 480us

     DQ = 1 ;       //拉高總線
     Delay(8) ;

     presence = DQ ;    //如果=0則初始化成功 =1則初始化失敗
     Delay(100) ;
     DQ = 1 ;

     return(presence) ; //返回信號，0=presence,1= no presence
}


/* 讀一個字節                     */
/*******************************************************************/
 ReadOneChar(void)
{
unsigned char i = 0 ;
unsigned char dat = 0 ;

for (i = 8 ; i > 0 ; i--)
  {
    DQ = 0 ; // 給脈衝信號
    dat >>= 1 ;
    DQ = 1 ; // 給脈衝信號

    if(DQ)
     dat |= 0x80 ;
    Delay(4) ;
  }

    return (dat) ;
}

/* 寫一個字節                                         */
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

/* 讀取溫度                            */
/*******************************************************************/
 Read_Temperature(void)
{
     Init_DS18B20() ;

     WriteOneChar(0xCC) ;  // 跳過讀序號列號的操作
     WriteOneChar(0x44) ;  // 啟動溫度轉換

     Init_DS18B20() ;
     WriteOneChar(0xCC) ;  //跳過讀序號列號的操作
     WriteOneChar(0xBE) ;  //讀取溫度寄存器

     temp_data[0] = ReadOneChar() ;   //溫度低8位
     temp_data[1] = ReadOneChar() ;   //溫度高8位
}

/* 數據轉換與溫度顯示                                              */
/*******************************************************************/
 Disp_Temperature()
{
  display[4]=temp_data[0]&0x0f ;
  display[0]=ditab[display[4]]+0x30 ;     //查表得小數位的值

  display[4]=((temp_data[0]&0xf0)>>4)|((temp_data[1]&0x0f)<<4) ;
  display[3]=display[4]/100+0x30 ;
  display[1]=display[4]%100 ;
  display[2]=display[1]/10+0x30 ;
  display[1]=display[1]%10+0x30 ;

    if(display[3]==0x30)        //高位為0，不顯示
   {
     display[3]=0x20 ;
     if(display[2]==0x30)      //次高位為0，不顯示
     display[2]=0x20 ;
   }

     lcd_pos(0x48) ;
     lcd_wdat(display[3]) ;        //百位數顯示
     lcd_pos(0x49) ;
     lcd_wdat(display[2]) ;        //十位數顯示
  lcd_pos(0x4a) ;
     lcd_wdat(display[1]) ;        //個位數顯示
  lcd_pos(0x4c) ;
     lcd_wdat(display[0]) ;        //小數位數顯示
}

/*******************************************************************/
/* 蜂鳴器響一聲                 */
/*******************************************************************/
void beep()
  {
    unsigned char y ;
    for (y=0 ;y<100 ;y++)
    {
      Delay(60) ;
      BEEP=!BEEP ;                //BEEP取反
    }
    BEEP=1 ;                      //關閉蜂鳴器
 Delay(40000) ;
  }

/* DS18B20 OK 顯示菜單                                             */
/*******************************************************************/
void  Ok_Menu ()
{
    uchar  m ;
    lcd_init() ;                //初始化LCD

    lcd_pos(0) ;                //設置顯示位置為第一行的第1個字符
     m = 0 ;
    while(cdis1[m] != '\0')
     {                         //顯示字符
       lcd_wdat(cdis1[m]) ;
       m++ ;
     }

    lcd_pos(0x40) ;             //設置顯示位置為第二行第1個字符
     m = 0 ;
    while(cdis2[m] != '\0')
     {
       lcd_wdat(cdis2[m]) ;      //顯示字符
       m++ ;
     }

     writetab() ;               //自定義字符寫入CGRAM
     delay1(5) ;
     lcd_pos(0x4d) ;
     lcd_wdat(0x00) ;          //顯示自定義字符
}


/* DS18B20 ERROR 顯示菜單                             */
/*******************************************************************/
void  Error_Menu ()
{
     uchar  m ;
     lcd_init() ;                //初始化LCD

    lcd_pos(0) ;                //設置顯示位置為第一行的第1個字符
     m = 0 ;
     while(cdis3[m] != '\0')
     {                         //顯示字符
       lcd_wdat(cdis3[m]) ;
       m++ ;
     }

     lcd_pos(0x40) ;             //設置顯示位置為第二行第1個字符
     m = 0 ;
     while(cdis4[m] != '\0')
     {
       lcd_wdat(cdis4[m]) ;      //顯示字符
       m++ ;
     }
}


/* 主函數     */
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

