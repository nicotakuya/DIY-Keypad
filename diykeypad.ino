// DIY keypad by takuya matsubara
// for Raspberry Pi Pico(Arduino-Pico)

#include <Wire.h>
#include <SPI.h>
#include "8x8font.h"

#define USE_KEYPAD     1  // 16keypad(0=disable / 1=enable)
#define USE_RTC        0  // Realtime Clock(0=disable / 1=enable)
#define USE_MOTOR      0  // Vibration Motor(0=disable / 1=enable)
#define USE_AUDIO      0  // Speaker(0=disable / 1=enable)

#define ATM0177B3A 1  // 1.7inch TFT Display
#define ATM0130B3  2  // 1.3inch TFT Display
#define SSD1306    3  // OLED Display

#define DISP_TYPE    1 // DISPLAY TYPE(1/2/3)
#define DISP_ROTATE  3 // TURN SCREEN(0=normal / 1=turn90 / 2=turn180 / 3=turn270)

#define SOFT_SPI  0 // (0=hardware / 1=software)

#if DISP_TYPE==ATM0177B3A
#define VRAMWIDTH   128  // width[pixel]
#define VRAMHEIGHT  160  // height[pixel]
#define FONTSIZE    2
#endif

#if DISP_TYPE==ATM0130B3
#define VRAMWIDTH   240  // width[pixel]
#define VRAMHEIGHT  240  // height[pixel]
#define FONTSIZE    4
#endif

#if DISP_TYPE==SSD1306
#define VRAMWIDTH   128 // width[pixel]
#define VRAMHEIGHT  64  // height[pixel]
#define FONTSIZE    2
#endif

#define VRAMSIZE   (VRAMWIDTH*VRAMHEIGHT*2) // 115200 Bytes

#if DISP_ROTATE==0
#define VRAMXRANGE VRAMWIDTH
#define VRAMYRANGE VRAMHEIGHT
#endif
#if DISP_ROTATE==1
#define VRAMYRANGE VRAMWIDTH
#define VRAMXRANGE VRAMHEIGHT
#endif
#if DISP_ROTATE==2
#define VRAMXRANGE VRAMWIDTH
#define VRAMYRANGE VRAMHEIGHT
#endif
#if DISP_ROTATE==3
#define VRAMYRANGE VRAMWIDTH
#define VRAMXRANGE VRAMHEIGHT
#endif
#define VRAMXMAX  (VRAMXRANGE-1)
#define VRAMYMAX  (VRAMYRANGE-1)

unsigned char vram[VRAMSIZE];
int putch_x = 0;
int putch_y = 0;
unsigned int putch_color = 0xffff;
int putch_zoom = 1;

#define KEY_UP     0x2000
#define KEY_DOWN   0x0020
#define KEY_RIGHT  0x0400
#define KEY_LEFT   0x0100
#define KEY_CANCEL 0x0001
#define KEY_ENTER  0x0008

// sprite pattern
PROGMEM const unsigned char sp_pattern[] = {
  0b0010100,
  0b0010100,
  0b0011100,
  0b0011100,
  0b1111111,
  0b1111111,
  0b1011101,

  0b0111110,
  0b1111111,
  0b1000001,
  0b1111111,
  0b1111111,
  0b1100011,
  0b0110110,

  0b0111110,
  0b1111111,
  0b1100011,
  0b1111111,
  0b1111111,
  0b0110110,
  0b1100011,

  0b0110110,
  0b0011100,
  0b0011100,
  0b0011100,
  0b0011100,
  0b0011100,
  0b0001000,

  0b1001001,
  0b0101010,
  0b0000000,
  0b1100011,
  0b0000000,
  0b0101010,
  0b1001001,

  0b0001000,
  0b0011100,
  0b0011100,
  0b0011100,
  0b0011100,
  0b0011100,
  0b0110110,
  
  0b0011100,
  0b0111110,
  0b1110111,
  0b1100011,
  0b1110111,
  0b0111110,
  0b0011100
};

// vector
PROGMEM const unsigned char vect_table[]={
    0,/* 0 degrees*/
    4,/* 1 degrees*/
    9,/* 2 degrees*/
   13,/* 3 degrees*/
   18,/* 4 degrees*/
   22,/* 5 degrees*/
   27,/* 6 degrees*/
   31,/* 7 degrees*/
   35,/* 8 degrees*/
   40,/* 9 degrees*/
   44,/* 10 degrees*/
   48,/* 11 degrees*/
   53,/* 12 degrees*/
   57,/* 13 degrees*/
   61,/* 14 degrees*/
   66,/* 15 degrees*/
   70,/* 16 degrees*/
   74,/* 17 degrees*/
   78,/* 18 degrees*/
   83,/* 19 degrees*/
   87,/* 20 degrees*/
   91,/* 21 degrees*/
   95,/* 22 degrees*/
   99,/* 23 degrees*/
  103,/* 24 degrees*/
  107,/* 25 degrees*/
  111,/* 26 degrees*/
  115,/* 27 degrees*/
  119,/* 28 degrees*/
  123,/* 29 degrees*/
  127,/* 30 degrees*/
  131,/* 31 degrees*/
  135,/* 32 degrees*/
  138,/* 33 degrees*/
  142,/* 34 degrees*/
  146,/* 35 degrees*/
  149,/* 36 degrees*/
  153,/* 37 degrees*/
  156,/* 38 degrees*/
  160,/* 39 degrees*/
  163,/* 40 degrees*/
  167,/* 41 degrees*/
  170,/* 42 degrees*/
  173,/* 43 degrees*/
  176,/* 44 degrees*/
  180,/* 45 degrees*/
  183,/* 46 degrees*/
  186,/* 47 degrees*/
  189,/* 48 degrees*/
  192,/* 49 degrees*/
  195,/* 50 degrees*/
  197,/* 51 degrees*/
  200,/* 52 degrees*/
  203,/* 53 degrees*/
  205,/* 54 degrees*/
  208,/* 55 degrees*/
  211,/* 56 degrees*/
  213,/* 57 degrees*/
  215,/* 58 degrees*/
  218,/* 59 degrees*/
  220,/* 60 degrees*/
  222,/* 61 degrees*/
  224,/* 62 degrees*/
  226,/* 63 degrees*/
  228,/* 64 degrees*/
  230,/* 65 degrees*/
  232,/* 66 degrees*/
  234,/* 67 degrees*/
  236,/* 68 degrees*/
  237,/* 69 degrees*/
  239,/* 70 degrees*/
  240,/* 71 degrees*/
  242,/* 72 degrees*/
  243,/* 73 degrees*/
  244,/* 74 degrees*/
  245,/* 75 degrees*/
  246,/* 76 degrees*/
  247,/* 77 degrees*/
  248,/* 78 degrees*/
  249,/* 79 degrees*/
  250,/* 80 degrees*/
  251,/* 81 degrees*/
  252,/* 82 degrees*/
  252,/* 83 degrees*/
  253,/* 84 degrees*/
  253,/* 85 degrees*/
  253,/* 86 degrees*/
  254,/* 87 degrees*/
  254,/* 88 degrees*/
  254,/* 89 degrees*/
  255 /* 90 degrees*/
};

typedef struct vecline{
  int ang;
  int len;
} VECLINE,*PVECLINE;

void disp_init(void);
void disp_update(void);
void tft_sendcmd_n_byte(unsigned char cmd ,const unsigned char *p ,char cnt);
void vram_cls(void);
unsigned int vram_point(int x,int y);
void vram_pset(int x,int y,unsigned int color);
void vram_line(int x1 ,int y1 ,int x2 ,int y2 ,unsigned int color);
void vram_fill(int x1 ,int y1 ,int x2 ,int y2 ,unsigned int color);
void vram_locate(int textx, int texty);
void vram_textcolor(unsigned int color);
void vram_putch(unsigned char ch);
void vram_putstr(unsigned char *p);
void vram_putdec(unsigned int num);
void vram_puthex(unsigned int num);
void vram_scroll(int xd,int yd);
void vram_spput(int x,int y, int num,unsigned int color);
void vram_spclr(int x,int y);
void spi_sendbyte(unsigned char data);
int vect_x1(int a);
int vect_y1(int a);
void vect_put(PVECLINE pV,int vx,int vy,int angle,int zoom,unsigned int color);
unsigned int color16bit(int r,int g,int b);

// near sin(degree)
int vect_y1(int a)
{
  int ret;
  while(a < 0){
    a+=360;
  }
  while(a >= 360){
    a-=360;
  }

  if(a >= 270){              //270～359degrees
    a-=270;
    ret = (int)vect_table[a];
    return(-ret);
  }
  if(a >= 180){              //180～269degrees
    a-=180;
    ret = (int)vect_table[90 - a];
    return(ret);
  }
  if(a >= 90){               //90～179degrees
    a-=90;
    ret = (int)vect_table[a];
    return(ret);
  }
  ret = (int)vect_table[90 - a];
  return(-ret);  //0～90degrees
}

// near cos(degree)
int vect_x1(int a)
{
  return(vect_y1(a+90));
}

// put vectol
void vect_put(PVECLINE pV,int vx,int vy,int angle,int zoom,unsigned int color)
{
  int i;
  int tempkak;
  int x1,y1,x2=0,y2=0;

  for( i=0; pV->ang != -1; i++){
    tempkak = pV->ang + angle;
    x1 = vect_x1(tempkak)* pV->len;
    y1 = vect_y1(tempkak)* pV->len;
    x1 = (zoom*x1>>10) + vx;
    y1 = (zoom*y1>>10) + vy;
    if(i > 0){
      vram_line(x1,y1,x2,y2,color);
    }
    x2 = x1;
    y2 = y1;
    pV++;
  }
}

// ABS
int fnc_abs(int a)
{
  if(a<0)a = -a;
  return (a);
}

// SGN
int fnc_sgn(int a)
{
  if(a<0)return(-1);
  return (1);
}

#if DISP_TYPE==ATM0130B3
#define TFTCLK   2   // clock
#define TFTMOSI  3   // TX
#define TFTMISO  4   // RX
#define TFTCS    5   // chip select
#define TFTCD    6   // command/data
#define TFTRST   7   // reset

PROGMEM const unsigned char gamma1_table[] = {
  0xf0,0x0b,0x11,0x0e,0x0d,0x19,0x36,0x33,0x4b,0x07,0x14,0x14,0x2c,0x2e
};

PROGMEM const unsigned char gamma2_table[] = {
  0xf0,0x0d,0x12,0x0b,0x09,0x03,0x32,0x44,0x48,0x39,0x16,0x16,0x2d,0x30
};

PROGMEM const unsigned char framerate_table[] = {
  0x0c,0x0c,0x00,0x33,0x33
};

// INITIALIZE
void disp_init(void)
{
  pinMode(TFTMOSI, OUTPUT);
  pinMode(TFTCLK, OUTPUT);
  pinMode(TFTCD, OUTPUT);
  pinMode(TFTCS, OUTPUT);
  pinMode(TFTRST, OUTPUT);
  digitalWrite(TFTMOSI, HIGH);
  digitalWrite(TFTCLK, LOW);
  digitalWrite(TFTCD, HIGH);
  digitalWrite(TFTCS, HIGH);
  digitalWrite(TFTRST, HIGH);

#if SOFT_SPI==0
  SPI.setCS(TFTCS);
  SPI.setSCK(TFTCLK);
  SPI.setTX(TFTMOSI);
  SPI.setRX(TFTMISO);
  SPI.begin();
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
#endif
  
  delay(20);
  digitalWrite(TFTRST, LOW);  // reset
  delay(20);
  digitalWrite(TFTRST, HIGH);
  delay(500);
  digitalWrite(TFTCS,LOW);
  tft_sendcmd(0x11);
  delay(100);
  tft_sendcmd_byte(0x36, 0x00);  // MADCTL
  tft_sendcmd_byte(0x3A, 0x55);  // 16bit color/pixel
  // Frame rate
  tft_sendcmd_n_byte(0xB2,framerate_table,5);
  tft_sendcmd_byte(0xB7,0x75);
  // power
  tft_sendcmd_byte(0xC2,0x01);
  tft_sendcmd_byte(0xC3,0x10);
  tft_sendcmd_byte(0xC4,0x20);
  tft_sendcmd_byte(0xC6,0x0F);
  tft_sendcmd_word(0xB0,0x00F0); // RRRR RGGGG GGGB BBBB
  tft_sendcmd_word(0xD0,0xA4A1);
  // gamma
  tft_sendcmd(0x21);
  tft_sendcmd_byte(0xBB,0x3B);
  tft_sendcmd_n_byte(0xE0,gamma1_table,14);
  tft_sendcmd_n_byte(0xE1,gamma2_table,14);
  tft_sendcmd_long(0x2A,0x000000EF); 
  tft_sendcmd_long(0x2B,0x000000EF); 
  tft_sendcmd(0x29);    // Display on
  tft_sendcmd(0x2C);
  digitalWrite(TFTCS,HIGH);
  delay(25);
}

//
void spi_sendbyte(unsigned char data)
{
#if SOFT_SPI==0
  SPI.transfer(data);
#else
  unsigned char bitmask;
  digitalWrite(TFTCLK, LOW);
  bitmask = 0x80; // MSB FIRST
  while(bitmask){
    if(bitmask & data){
      digitalWrite(TFTMOSI, HIGH);
    }else{
      digitalWrite(TFTMOSI, LOW);
    }    
    digitalWrite(TFTCLK, HIGH);
    digitalWrite(TFTCLK, LOW);
    bitmask >>= 1;
  }
#endif
}

//
void spi_send_n_byte(unsigned char *ptr,int bytecnt)
{
#if SOFT_SPI==0
  SPI.transfer(ptr,bytecnt);
#else
  unsigned char bitmask;
  unsigned char data;
  while(bytecnt--){
    data = *ptr++;
    digitalWrite(TFTCLK, LOW);
    bitmask = 0x80; // MSB FIRST
    while(bitmask){
      if(bitmask & data){
        digitalWrite(TFTMOSI, HIGH);
      }else{
        digitalWrite(TFTMOSI, LOW);
      }    
      digitalWrite(TFTCLK, HIGH);
      digitalWrite(TFTCLK, LOW);
      bitmask >>= 1;
    }
  }
#endif
}

//
void tft_sendcmd(unsigned char data)
{
  int i;
  for(i=0;i<5000;i++);
  digitalWrite(TFTCD,LOW);
  spi_sendbyte(data);
  digitalWrite(TFTCD,HIGH);
}

//
void tft_send_data(unsigned char data)
{
  spi_sendbyte(data);
}

//
void tft_sendcmd_n_byte(unsigned char cmd ,const unsigned char *p ,char cnt)
{
  tft_sendcmd(cmd);
  while(cnt--){
    tft_send_data(pgm_read_byte(p++));
  }
}

//
void tft_sendcmd_byte(unsigned char cmd,unsigned char data)
{
  tft_sendcmd(cmd);
  tft_send_data(data);
}

//
void tft_sendcmd_word(unsigned char cmd,unsigned int data)
{
  tft_sendcmd(cmd);
  tft_send_data((unsigned char)(data >> 8));
  tft_send_data((unsigned char)(data & 0xff));
}

//
void tft_sendcmd_long(unsigned char cmd,unsigned long data)
{
  tft_sendcmd(cmd);
  tft_send_data((unsigned char)(data >> 24));
  tft_send_data((unsigned char)((data >> 16)& 0xff));
  tft_send_data((unsigned char)((data >> 8)& 0xff));
  tft_send_data((unsigned char)(data & 0xff));
}

//
void tft_fill(unsigned char x, unsigned char y, unsigned char width, unsigned char height,unsigned int color) {
  int loopcnt = width * height;

  digitalWrite(TFTCS,LOW);
  tft_sendcmd(0x2A);
  tft_send_data(0x00);
  tft_send_data(x);               // start X
  tft_send_data(0x00);
  tft_send_data(x + width - 1);   // end X

  tft_sendcmd(0x2B);
  tft_send_data(0x00);
  tft_send_data(y);               // start Y
  tft_send_data(0x00);
  tft_send_data(y + height - 1);  // end Y

  tft_sendcmd(0x2C);
  while(loopcnt--) {
    tft_send_data(color >> 8);
    tft_send_data(color & 0xff);
  }
  digitalWrite(TFTCS,HIGH);
}

// send VRAM to TFT 
void disp_update(void)
{
  long totalcnt;
  unsigned char *ptr;

  digitalWrite(TFTCS,LOW);
  tft_sendcmd(0x2A);
  tft_send_data(0x00);
  tft_send_data(0);               // start X
  tft_send_data(0x00);
  tft_send_data(VRAMWIDTH - 1);   // end X

  tft_sendcmd(0x2B);
  tft_send_data(0x00);
  tft_send_data(0);               // start Y
  tft_send_data(0x00);
  tft_send_data(VRAMHEIGHT - 1);  // end Y

  ptr = vram;
  totalcnt=VRAMSIZE;
  tft_sendcmd(0x2C);
  while(totalcnt--){
    spi_sendbyte(*ptr++);
  }
  digitalWrite(TFTCS,HIGH);
  delay(1);
}
#endif

#if DISP_TYPE==ATM0177B3A
#define TFTCLK    2   // clock
#define TFTMOSI   3   // TX
#define TFTMISO   4   // RX
#define TFTCS     5   // chip select
#define TFTCD     6   // command/data
#define TFTRST    7   // reset

#define SOFTWARE_RESET            0x01
#define SLEEP_OUT                 0x11
#define PARTIAL_MODE_ON           0x12
#define NORMAL_DISPLAY_MODE_ON    0x13
#define DISPLAY_INVERSION_OFF     0x20
#define DISPLAY_INVERSION_ON      0x21
#define GAMMA_SET                 0x26
#define DISPLAY_OFF               0x28
#define DISPLAY_ON                0x29
#define COLUMN_ADDRESS_SET        0x2A
#define PAGE_ADDRESS_SET          0x2B
#define MEMORY_WRITE              0x2C
#define V_SCROLL_DEFINITION       0x33
#define MEMORY_ACCESS_CONTROL     0x36
#define V_SCROLL_START_ADDRESS    0x37
#define INTERFACE_PIXEL_FORMAT    0x3A
#define FRAME_RATE_CONTROL_1      0xB1
#define DISPLAY_INVERSION_CONTROL 0xB4
#define DISPLAY_FUCTION_SET       0xB6
#define POWER_CONTROL_1           0xC0
#define POWER_CONTROL_2           0xC1
#define VCOM_CONTROL_1            0xC5
#define VCOM_OFFSET_CONTROL       0xC7
#define POSITIVE_GAMMA            0xE0
#define NEGATIVE_GAMMA            0xE1
#define GAM_R_SEL                 0xF2

const unsigned char p_gamma_table[] = {
  0x36,0x29,0x12,0x22,0x1C,0x15,0x42,0xB7,0x2F,0x13,0x12,0x0A,0x11,0x0B,0x06
};
     
const unsigned char n_gamma_table[] = {
  0x09,0x16,0x2D,0x0D,0x13,0x15,0x40,0x48,0x53,0x0C,0x1D,0x25,0x2E,0x34,0x39
};

// INITIALIZE
void disp_init(void)
{
  pinMode(TFTMOSI, OUTPUT);
  pinMode(TFTCLK, OUTPUT);
  pinMode(TFTCD, OUTPUT);
  pinMode(TFTCS, OUTPUT);
  pinMode(TFTRST, OUTPUT);
  digitalWrite(TFTMOSI, HIGH);
  digitalWrite(TFTCLK, LOW);
  digitalWrite(TFTCD, HIGH);
  digitalWrite(TFTCS, HIGH);
  digitalWrite(TFTRST, HIGH);

#if SOFT_SPI==0
  SPI.setCS(TFTCS);
  SPI.setSCK(TFTCLK);
  SPI.setTX(TFTMOSI);
  SPI.setRX(TFTMISO);
  SPI.begin();
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));
#endif

  delay(500);
  tft_sendcmd(SOFTWARE_RESET);
  delay(500);
  tft_sendcmd(SLEEP_OUT);
  delay(5);
  tft_sendcmd_byte(INTERFACE_PIXEL_FORMAT, 0x05);  //16bit
  tft_sendcmd_byte(GAMMA_SET,0x04);
  tft_sendcmd_byte(GAM_R_SEL,0x01);
  tft_sendcmd(NORMAL_DISPLAY_MODE_ON);
  tft_sendcmd_word(DISPLAY_FUCTION_SET,0xff06);
//  tft_sendcmd_15byte(POSITIVE_GAMMA,(unsigned char *)p_gamma_table);
//  tft_sendcmd_15byte(NEGATIVE_GAMMA,(unsigned char *)n_gamma_table);
  tft_sendcmd_word(FRAME_RATE_CONTROL_1,0x0802);
  tft_sendcmd_byte(DISPLAY_INVERSION_CONTROL,0x07);
  tft_sendcmd_word(POWER_CONTROL_1,0x0A02);
  tft_sendcmd_byte(POWER_CONTROL_2,0x02);
  tft_sendcmd_word(VCOM_CONTROL_1,0x5063);
  tft_sendcmd_byte(VCOM_OFFSET_CONTROL,0x00);
  tft_sendcmd_long(COLUMN_ADDRESS_SET,VRAMWIDTH-1); 
  tft_sendcmd_long(PAGE_ADDRESS_SET,VRAMHEIGHT-1); 
//  tft_sendcmd_long(V_SCROLL_DEFINITION,VRAMHEIGHT);
//  spi_sendbyte(0);
//  spi_sendbyte(0);
//  tft_sendcmd_byte(MEMORY_ACCESS_CONTROL,0x00); // RGB
//  tft_sendcmd_word(V_SCROLL_START_ADDRESS,0x0000);
  tft_sendcmd(DISPLAY_ON);
  delay(25);
}

//
void spi_sendbyte(unsigned char data)
{
#if SOFT_SPI==0
  SPI.transfer(data);
#else
  unsigned char bitmask;
  digitalWrite(TFTCLK, LOW);
  bitmask = 0x80;
  while(bitmask){
    digitalWrite(TFTMOSI, ((bitmask & data) != 0));
    digitalWrite(TFTCLK, HIGH);
    digitalWrite(TFTCLK, LOW);
    bitmask >>= 1;
  }
#endif
}

//
void spi_send_n_byte(unsigned char *ptr,int bytecnt)
{
#if SOFT_SPI==0
  SPI.transfer(ptr,bytecnt);
#else
  unsigned char bitmask;
  unsigned char data;
  while(bytecnt--){
    data = *ptr++;
    digitalWrite(TFTCLK, LOW);
    bitmask = 0x80; // MSB FIRST
    while(bitmask){
      if(bitmask & data){
        digitalWrite(TFTMOSI, HIGH);
      }else{
        digitalWrite(TFTMOSI, LOW);
      }    
      digitalWrite(TFTCLK, HIGH);
      digitalWrite(TFTCLK, LOW);
      bitmask >>= 1;
    }
  }
#endif
}

//
void tft_sendcmd(unsigned char data)
{
  digitalWrite(TFTCD,LOW);
  digitalWrite(TFTCS,LOW);
  spi_sendbyte(data);
  digitalWrite(TFTCS,HIGH);
  digitalWrite(TFTCD,HIGH);
}

//
void tft_sendcmd_15byte(unsigned char cmd,unsigned char *data)
{
  int cnt;
  tft_sendcmd(cmd);
  digitalWrite(TFTCS,LOW);
  cnt=15;
  while(cnt--){
    spi_sendbyte(*data++);
  }
  digitalWrite(TFTCS,HIGH);
  delay(1);
}

//
void tft_sendcmd_byte(unsigned char cmd,unsigned char data)
{
  tft_sendcmd(cmd);
  digitalWrite(TFTCS,LOW);
  spi_sendbyte(data);
  digitalWrite(TFTCS,HIGH);
  delay(1);
}

//
void tft_sendcmd_word(unsigned char cmd,unsigned int data)
{
  tft_sendcmd(cmd);
  digitalWrite(TFTCS,LOW);
  spi_sendbyte((unsigned char)(data >> 8));
  spi_sendbyte((unsigned char)(data & 0xff));
  digitalWrite(TFTCS,HIGH);
  delay(1);
}

//
void tft_sendcmd_long(unsigned char cmd,unsigned long data)
{
  tft_sendcmd(cmd);
  digitalWrite(TFTCS,LOW);
  spi_sendbyte((unsigned char)(data >> 24));
  spi_sendbyte((unsigned char)((data >> 16)& 0xff));
  spi_sendbyte((unsigned char)((data >> 8)& 0xff));
  spi_sendbyte((unsigned char)(data & 0xff));
  digitalWrite(TFTCS,HIGH);
  delay(1);
}

// send VRAM to DISPLAY 
void disp_update(void)
{
  unsigned int totalcnt;
  unsigned char *ptr;
  
  tft_sendcmd(MEMORY_WRITE);
  digitalWrite(TFTCS,LOW);
  ptr = vram;
  totalcnt = VRAMSIZE;
  while(totalcnt--){
    spi_sendbyte(*ptr++);
  }
  digitalWrite(TFTCS,HIGH);
  delay(1);
}
#endif

//
#if DISP_TYPE==SSD1306

#define OLEDADDR  (0x78 >> 1) // SSD1306 I2C address

#define SET_CONTRAST_CONTROL  0x81
#define SET_CHARGE_PUMP       0x8D
#define SET_ADDRESSING_MODE   0x20
#define SET_DISPLAY_STARTLINE 0x40
#define SET_SEGMENT_REMAP     0xA1
#define SET_ENTIRE_DISPLAY    0xA4
#define SET_DISPLAY_NORMAL    0xA6
#define SET_MULTIPLEX_RATIO   0xA8
#define SET_DISPLAY_ON        0xAF
#define SET_COM_OUTPUT_SCAN   0xC8
#define SET_DISPLAY_OFFSET    0xD3
#define SET_OSCILLATOR_FREQ   0xD5
#define SET_COM_PINS_HARDWARE 0xDA
#define SET_COLUMN_ADDRESS    0x21
#define SET_PAGE_ADDRESS      0x22

//
void oled_command(unsigned char data)
{
  Wire.beginTransmission(OLEDADDR);
  Wire.write(0b10000000);
  Wire.write(data);             
  Wire.endTransmission();
}

//
void oled_command2(unsigned char data1,unsigned char data2)
{
  Wire.beginTransmission(OLEDADDR);
  Wire.write(0b00000000);
  Wire.write(data1);             
  Wire.write(data2);             
  Wire.endTransmission();
}

// INITIALIZE
void disp_init(void)
{
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.setClock(2000000);  
  Wire.begin();

  delay(50);
  oled_command2(SET_MULTIPLEX_RATIO , 0x3F);
  oled_command2(SET_DISPLAY_OFFSET,0);
  oled_command(SET_DISPLAY_STARTLINE);
  oled_command(SET_COM_OUTPUT_SCAN);
  oled_command(SET_SEGMENT_REMAP);
  oled_command2(SET_COM_PINS_HARDWARE, 0x12);
  oled_command2(SET_CONTRAST_CONTROL , 0x80);
  oled_command(SET_ENTIRE_DISPLAY);
  oled_command(SET_DISPLAY_NORMAL);
  oled_command2(SET_OSCILLATOR_FREQ  , 0x80);
  oled_command2(SET_ADDRESSING_MODE  ,0); 
  oled_command2(SET_CHARGE_PUMP , 0x14);
  oled_command(SET_DISPLAY_ON);
  delay(10);
}

// 
void disp_update(void){
  int i,j,x,y;
  unsigned char *ptr,*ptr2;
  unsigned char work;

  Wire.beginTransmission(OLEDADDR);
  Wire.write(0b00000000);
  Wire.write(SET_COLUMN_ADDRESS);
  Wire.write(0);       // start column
  Wire.write(VRAMWIDTH-1); // end column
  Wire.write(SET_PAGE_ADDRESS);
  Wire.write(0);           // start page
  Wire.write((VRAMHEIGHT/8)-1); // end page
  Wire.endTransmission();

  x=0;
  y=0;
  ptr = vram;
  while(y < VRAMHEIGHT){  
    Wire.beginTransmission(OLEDADDR);
    Wire.write(0b01000000);

    for(i=0; i<8; i++){
      ptr2 = ptr;
      work = 0;
      for(j=0; j<8; j++){  
        work >>= 1;
        if(*ptr2)work |= 0x80;
        ptr2 += VRAMWIDTH*2;
      }
      Wire.write(work);
      x++;
      ptr += 2;
    }
    Wire.endTransmission();
    if(x >= VRAMWIDTH){
      x=0;
      y+=8;
      ptr = vram + (y*VRAMWIDTH*2);
    }
  }
}
#endif

// CALC. COLOR
unsigned int color16bit(int r,int g,int b)
{
// RRRRRGGGGGGBBBBB
// blue :bit4 -bit0 (0-31)
// green:bit10-bit5 (0-63)
// red  :bit15-bit11(0-31)
  r >>= 3;
  g >>= 2;
  b >>= 3;
  return(((unsigned int)r << 11)+(g << 5)+b);
}

// CLEAR VRAM
void vram_cls(void)
{
  long i;
  unsigned char *ptr;

  ptr = vram;
  i = VRAMSIZE;
  while(i--){
    *ptr++ = 0;
  }
}

// GET PIXEL
unsigned int vram_point(int x,int y)
{
  int i;
  unsigned int color;
  unsigned char *ptr;

#if DISP_ROTATE==1
  i=x;
  x=(VRAMWIDTH-1)-y;
  y=i;
#endif
#if DISP_ROTATE==2
  x=(VRAMWIDTH-1)-x;
  y=(VRAMHEIGHT-1)-y;
#endif
#if DISP_ROTATE==3
  i=x;
  x=y;
  y=(VRAMHEIGHT-1)-i;
#endif

  if(x<0)return(0);
  if(y<0)return(0);
  if(x>=VRAMWIDTH)return(0);
  if(y>=VRAMHEIGHT)return(0);

  ptr = vram;
  ptr += ((long)y*(VRAMWIDTH*2)) + (x*2);
  color =  *ptr << 8;
  ptr++;
  color += *ptr;
  return(color);
}

// DRAW PIXEL
void vram_pset(int x,int y,unsigned int color)
{
  int i;
  unsigned char *ptr;

#if DISP_ROTATE==1
  i=x;
  x=(VRAMWIDTH-1)-y;
  y=i;
#endif
#if DISP_ROTATE==2
  x=(VRAMWIDTH-1)-x;
  y=(VRAMHEIGHT-1)-y;
#endif
#if DISP_ROTATE==3
  i=x;
  x=y;
  y=(VRAMHEIGHT-1)-i;
#endif

  if(x<0)return;
  if(y<0)return;
  if(x>=VRAMWIDTH)return;
  if(y>=VRAMHEIGHT)return;
  ptr = vram;
  ptr += ((long)y*(VRAMWIDTH*2)) + (x*2);

  *ptr++ = color >> 8;   //high
  *ptr = color & 0xff;   //low 
}

// BOX FILL
void vram_fill(int x1 ,int y1 ,int x2 ,int y2 ,unsigned int color)
{
  int x,y;
  for(y=y1; y<=y2; y++){
    for(x=x1; x<=x2; x++){
      vram_pset(x, y ,color); //ドット描画
    }
  }
}

// DRAW LINE
void vram_line(int x1 ,int y1 ,int x2 ,int y2 ,unsigned int color)
{
  int xd;    // X2-X1座標の距離
  int yd;    // Y2-Y1座標の距離
  int xs=1;  // X方向の1pixel移動量
  int ys=1;  // Y方向の1pixel移動量
  int e;

  xd = x2 - x1;  // X2-X1座標の距離
  if(xd < 0){
    xd = -xd;  // X2-X1座標の絶対値
    xs = -1;    // X方向の1pixel移動量
  }
  yd = y2 - y1;  // Y2-Y1座標の距離
  if(yd < 0){
    yd = -yd;  // Y2-Y1座標の絶対値
    ys = -1;    // Y方向の1pixel移動量
  }
  vram_pset (x1, y1 ,color); //ドット描画
  e = 0;
  if( yd < xd ) {
    while( x1 != x2) {
      x1 += xs;
      e += (2 * yd);
      if(e >= xd) {
        y1 += ys;
        e -= (2 * xd);
      }
      vram_pset(x1, y1 ,color); //ドット描画
    }
  }else{
    while( y1 != y2) {
      y1 += ys;
      e += (2 * xd);
      if(e >= yd) {
        x1 += xs;
        e -= (2 * yd);
      }
      vram_pset(x1, y1 ,color); //ドット描画
    }
  }
}

// LOCATE
void vram_locate(int textx, int texty)
{
  putch_x = textx;
  putch_y = texty;
}

// TEXT COLOR
void vram_textcolor(unsigned int color)
{
  putch_color = color;
}

// TEXT ZOOM
void vram_textzoom(int zoom)
{
  putch_zoom = zoom;
}

// PRINT CHARACTER
void vram_putch(unsigned char ch)
{
  char i,j;
  unsigned char bitdata;
  int idx,x,y;

  if(ch =='\n')putch_x += VRAMXMAX;
  if(putch_x > (VRAMXRANGE-(8*putch_zoom))){
    putch_x = 0;
    putch_y += 8*putch_zoom;
    y = (VRAMYRANGE-(8*putch_zoom));
    if(putch_y > y){
      vram_scroll(0,putch_y - y);
      putch_y = y;
    }
  }  
  if(ch < 0x20)return;

  ch -= 0x20;
  idx = ((int)ch * 8);
  for(i=0 ;i<8 ;i++) {
    bitdata = font[idx];
    idx++;
    for(j=0; j<8; j++){
      if(bitdata & 0x80){
        x=putch_x+(j*putch_zoom);
        y=putch_y+(i*putch_zoom);
        vram_fill(x,y,x+putch_zoom-1,y+putch_zoom-1,putch_color);
      }
      bitdata <<= 1;
    }
  }
  putch_x += 8*putch_zoom;
}

// PRINT STRING
void vram_putstr(unsigned char *p)
{
  while(*p != 0){
    vram_putch( *p++ );
  }
}

// PRINT DECIMAL
void vram_putdec(unsigned int num)
{
  unsigned char ch;
  unsigned int shift=10000;
  
  while(shift > 0){
    ch = (num / shift) % 10;
    ch += '0';
    vram_putch(ch);
    shift /= 10;
  }
}

// PRINT DECIMAL
void vram_putdec2(unsigned int num)
{
  vram_putch((num / 10)+'0');
  vram_putch((num % 10)+'0');
}

// PRINT HEXADECIMAL
void vram_puthex(unsigned char num)
{
  unsigned char ch;
  char shift=4;

  while(shift >= 0){
    ch = (num >> shift) & 0xf;
    if(ch < 10){
      ch += '0';
    }else{
      ch += ('A'-10);
    }
    vram_putch( ch);
    shift -= 4;
  }
}

#define VRSPSIZE 7
// PRINT SPRITE
void vram_spput(int x,int y, int num,unsigned int color)
{
  int i,j,idx;
  unsigned char dat;

  idx =  num * VRSPSIZE;
  x -= (VRSPSIZE/2);
  y -= (VRSPSIZE/2);

  for(j=0; j<VRSPSIZE; j++){
    dat = sp_pattern[idx];
    idx++;
    for(i=0; i<VRSPSIZE; i++){
      if(dat & (1<<(VRSPSIZE-1))){
        vram_pset(x+i,y+j,color);
      }
      dat <<= 1;
    }
  }
}

// CLEAR SPRITE
void vram_spclr(int x,int y)
{
  char i,j;

  x -= (VRSPSIZE/2);
  y -= (VRSPSIZE/2);

  for(j=0; j<VRSPSIZE; j++){
    for(i=0; i<VRSPSIZE; i++){
      vram_pset(x+i,y+j,0);
    }
  }
}

// SCROLL
void vram_scroll(int xd,int yd)
{
  int x,y;
  unsigned int color;

  for(y=0;y<VRAMYRANGE;y++){
    for(x=0;x<VRAMXRANGE;x++){
      color = vram_point(x+xd, y+yd);
      vram_pset(x,y,color);
    }
  }
}

// BCDを10進に変換
unsigned char bcd_to_num(unsigned char num){
  return((num >> 4)*10 + (num & 0xf));
}

// 10進をBCDに変換
unsigned char num_to_bcd(unsigned char num){
  unsigned char numhigh,numlow;
  numhigh = num / 10;
  numlow = num % 10;
  return((numhigh << 4) + numlow);
}

// DEMO
void chardemo(void)
{
  int x,y;
  unsigned char chrnum;
  unsigned int color1;
  int timeout;

  vram_cls();
  chrnum=0x20;
  vram_locate(0, (VRAMYMAX+1)-(8*putch_zoom));
  color1 = color16bit(128,255,  0);
  vram_textcolor(color1);
  timeout = 100;
  while(timeout--){
    vram_putch(chrnum);
    chrnum++;
    if(chrnum > 0x7f){
      chrnum = 0x20;
    }
    disp_update();
  }
}

// DEMO
void linedemo(void)
{
  int xa,ya,xb,yb;
  int xa1,ya1,xb1,yb1;
  unsigned int color;
  int timeout;

  vram_cls();   // clear VRAM
  xa =random(VRAMXMAX-1)+1;
  ya =random(VRAMYMAX-1)+1;
  xb =random(VRAMXMAX-1)+1;
  yb =random(VRAMYMAX-1)+1;
  xa1 =random(11)-5;
  ya1 =random(11)-5;
  xb1 =random(11)-5;
  yb1 =random(11)-5;

  timeout = 100;
  while(timeout--){
    xa += xa1;
    ya += ya1;
    xb += xb1;
    yb += yb1;
    color = color16bit(random(256),random(256),random(256));
    vram_line(xa ,ya ,xb ,yb ,color);
    if((xa<=0)||(xa>=VRAMXMAX))xa1 = -xa1;
    if((ya<=0)||(ya>=VRAMYMAX))ya1 = -ya1;
    if((xb<=0)||(xb>=VRAMXMAX))xb1 = -xb1;
    if((yb<=0)||(yb>=VRAMYMAX))yb1 = -yb1;
    disp_update();
  }
}

//  DEMO
void balldemo(void)
{
  typedef struct ball{
    int x;
    int y;
    int x1;
    int y1;
  } ST_BALL;   // 構造体

  #define BALLMAX 14
  ST_BALL ball[BALLMAX];

  #define MLT 3

  int x2,y2;
  int xd,yd;
  unsigned char i,j;
  int timeout;
  unsigned int color1,color2,color3;

  color1 = color16bit(255,  0,255);

  for(i=0;i<BALLMAX;i++){
    ball[i].x = random(VRAMXRANGE)*MLT;
    ball[i].y = random(VRAMYRANGE)*MLT;
    ball[i].x1 = random(MLT*2)-MLT;
    ball[i].y1 = random(MLT*2)-MLT;
  }

  timeout=100;
  while(timeout--){
    vram_cls();
    
    for(i=0; i<BALLMAX; i++){
      x2 = ball[i].x;
      y2 = ball[i].y;

      x2 += ball[i].x1;
      y2 += ball[i].y1;
      for(j=0 ;j<BALLMAX ;j++){
        if(i==j)continue;
        xd = x2-ball[j].x;
        yd = y2-ball[j].y;

        if( (fnc_abs(xd) < 7*MLT)&&(fnc_abs(yd) < 7*MLT)){
          ball[i].x1 = fnc_sgn(xd)*(random(MLT*2)+1);
          ball[i].y1 = fnc_sgn(yd)*(random(MLT*2)+1);
          ball[j].x1 = -ball[i].x1;
          ball[j].y1 = -ball[i].y1;
        }
      }
      ball[i].x = x2;
      ball[i].y = y2;

      x2/=MLT;
      y2/=MLT;
      if(x2 <= 3) ball[i].x1 = (random(MLT*2)+1);
      if(x2 >= (VRAMXMAX-3)) ball[i].x1 = -(random(MLT*2)+1);
      if(y2 <= 3) ball[i].y1 = (random(MLT*2)+1);
      if(y2 >= (VRAMYMAX-3)) ball[i].y1 = -(random(MLT*2)+1);
      
      vram_spput(x2,y2,6,color1);
    }
    disp_update();
  }
}

// DEMO
void vectordemo(void)
{
  typedef struct inseki{
    int x;
    int y;
    int x1;
    int y1;
    int ang;
    int a1;
    int zoom;
  } INSEKI, *PINSEKI;
  #define OFST 5
  #define INSEKIMAX 10
  INSEKI insk[INSEKIMAX];
  PINSEKI pInsk;
  int i;
  int timeout;
  int tx,ty;
  VECLINE myship[5] = {
    {  40 ,6},  /*  */
    {  320,6},  /*  */
    {  180,6},  /*  */
    {  40 ,6},  /*  */
    {  -1 ,-1}  /*  */
  };
  unsigned int color1,color2,color3;

  for(i=0; i<INSEKIMAX; i++){
    insk[i].x=-999;
    insk[i].y=0;
    insk[i].x1=0;
    insk[i].y1=0;
  }

  color2 = color16bit(255,255,  0); /*color 2*/

  timeout=100;
  while(timeout--){
    vram_cls();   // clear VRAM

    pInsk = &insk[0];
    for(i=0; i<INSEKIMAX; i++,pInsk++){
      tx = pInsk->x;
      ty = pInsk->y;
      tx += pInsk->x1;
      ty += pInsk->y1;
      if((ty< -(8<<OFST))||(ty>((VRAMYMAX+8)<<OFST))||(tx < -(8<<OFST))||(tx>((VRAMXMAX+8)<<OFST))){
        if(random(2)){
          pInsk->x = -8<<OFST;
          pInsk->x1 = random(40)+60;
        }else{
          pInsk->x = (VRAMXMAX+8)<<OFST;
          pInsk->x1 = -random(40)-60;
        }
        pInsk->ang = random(36)*10;
        pInsk->y = random(6)*((VRAMYMAX<<OFST)/6);
        pInsk->y1 = random(13)-7;
        pInsk->a1 = random(13)-6;
        pInsk->zoom = random(16)+8;
        continue;
      }
      vect_put((PVECLINE)myship, tx>>OFST, ty>>OFST, pInsk->ang, pInsk->zoom,color2);
      pInsk->x = tx;
      pInsk->y = ty;
      pInsk->ang += pInsk->a1;
      if(pInsk->ang >=360) pInsk->ang -= 360;
      if(pInsk->ang <   0) pInsk->ang += 360;
    }
    disp_update();
  }
}

// DEMO
void lifegame(void)
{
  #define WSIZE (VRAMXRANGE/2)

  int x,y;
  int ax,bx;
  unsigned int color;
  char touch;
  int i;
  int timeout;
  int lx1[8]={ 0, 1, 1, 1, 0,-1,-1,-1};
  int ly1[8]={-1,-1, 0, 1, 1, 1, 0,-1};

  vram_cls();
  for(i=0; i<500; i++){
    x= random(WSIZE);
    y= random(WSIZE);
    vram_pset(x,y,color16bit(255,0,255));
  }

  ax=0;
  bx=WSIZE;
  timeout=100;
  while(timeout--){
    for(y=0; y<WSIZE; y++){
      for( x=0; x<WSIZE; x++){
        color=0;
        touch=0;
        for( i=0;i<8;i++){
          if(vram_point(ax+x+lx1[i],y+ly1[i])){
            touch++;
            if(touch>=4){
              break;
            }
          }
        }
        if(touch==2) color=vram_point(ax+x,y);
        if(touch==3) color=color16bit(255,0,255);
        vram_pset(bx+x,y,color);
      }
    }
    x=ax; //swap ax,bx
    ax=bx;
    bx=x;
    disp_update();
  }
}

// DEMO
void spacefight(void)
{
#define TEKIMAX 15
#define MOVEPITCH 30  //移動カウント
#define MOVESEQ   (10+10+2+2) //移動シーケンス
const unsigned char strready[]="READY";
const unsigned char strover[]="GAME OVER";

  int tx[TEKIMAX]; //敵座標
  int ty[TEKIMAX];
  int tb[TEKIMAX];
  char ttime,tmove,tnum;
  char gamespeed=0; //game speed
  int score=0;  //Score
  int ax=0,ay; //My Ship X,Y
  int bx,by; //My Ship Beam
  int cx,cy; //Enemy Beam
  int x,y;
  int i;
  unsigned int key;
  char overflag=1;
  int timeout;
  unsigned int color1,color2,color3;

  color1 = color16bit(255,128,  0);
  color2 = color16bit(255,  0,128);
  color3 = color16bit(128,  0,255);
  timeout=300;
  while(timeout){
    vram_cls();
    if(overflag){
      overflag=0;
      vram_textcolor(color16bit(255,255,0));
      vram_locate(13,VRAMYMAX/2);
      vram_putstr((unsigned char *)strready);
      score=0;
      gamespeed=MOVEPITCH;
      ax=VRAMXRANGE/2;  //プレーヤ座標
      ay=VRAMYRANGE-5;
    }
    vram_spput(ax, ay, 0,color1);    //my ship
    bx=by=-1;   //自弾座標
    cx=cy=-1;   //敵弾座標

    for(i=0;i<TEKIMAX;i++){
      tx[i] = ((i % 5)*(VRAMXRANGE/7))+(VRAMXRANGE/10);
      ty[i] = ((i / 5)*(VRAMXRANGE/7))+(VRAMXRANGE/10);
      tb[i] = 0;
    }

    tmove=0;
    ttime=0;
    disp_update();
    delay(1000);

    while(overflag==0){
      vram_cls(); 
  #if USE_KEYPAD
      key = key_get();
      if(key & KEY_LEFT)ax -= 2;
      if(key & KEY_RIGHT)ax += 2;
      if(ax<8)ax=8;
      if(ax>VRAMXMAX-8)ax=VRAMXMAX-8;
  #endif
      vram_spput(ax, ay, 0,color1);    //my ship

      if(by < -1){
        bx=ax;
        by=ay;
      }else{
        by-=3;
        vram_spput(bx,by,5,color3);  //my meam
      }

      //Enemy Beam
      if(cy == -1){
        i=random(TEKIMAX);
        if(ty[i] != -1){
          cx = tx[i];
          cy = ty[i];
        }
      }else{
        cy++;
        if(cy > VRAMYMAX){
          cy = -1;
        }else{
          vram_spput(cx,cy,3,color3);
        }
      }

      ttime = (ttime+1)%gamespeed;
      if (ttime==0) {
        tmove=(tmove+1) % MOVESEQ;
      }

      tnum=0;
      for(i=0 ;i<TEKIMAX ;i++) {
        x=tx[i];
        y=ty[i];
        if(y == -1)continue;
        tnum++;

        if(tb[i]){
          tb[i]--;
          vram_spput(x,y,4,1);
          if(tb[i]==0)
            ty[i]=-1;
          continue;
        }
        if((fnc_abs(by-y)<5)&&(fnc_abs(bx-x)<5)){ //命中
          if(score < 9999)score++;
          if(gamespeed > 2)gamespeed-=1;
          tb[i]=15;
          by=-1;
          continue;
        }
        if(ttime==0){
          if(tmove < 10){//01234
            x+=2;
          }else if(tmove < (10+2)){//5
            y+=2;
          }else if(tmove < (10+2+10)){//6789a
            x-=2;
          }else{
            y+=2;
          }
          if(y >= VRAMYMAX)
            overflag=1;
        }
        tx[i]=x;
        ty[i]=y;
        vram_spput(x,y ,1+(tmove & 1),color2);
        continue;
      }
      if((fnc_abs(cy-ay)<5)&&(fnc_abs(cx-ax)<5)){
        overflag=1;
      }

      if(tnum==0){  //敵全滅
        gamespeed+=8;
        break;
      }
      disp_update();
      timeout--;
      if(timeout==0)break;
    }
    if(overflag){ //game over
      vram_spclr(ax,ay);
      vram_spput(ax, ay, 4,color1);    //------自機をvramに転送
      vram_locate(8,VRAMYMAX/4);
      vram_putstr((unsigned char *)strover);
      vram_locate(8,VRAMYMAX*3/4);
      vram_putdec(score);
      disp_update();
      delay(1000);
    }
  }
}

//  DEMO
void landscape(void)
{
#define LANDSCAPEY1  ((VRAMYMAX/2)-3)
#define LANDSCAPEY2 ((VRAMYMAX/2)+3)
#define DEGPITCH  15
  int x1,x2,y,i,deg,iso=0;
  int timeout;
  unsigned int color1,color2;

  color1 = color16bit(  0,255,255);
  color2 = color16bit(  0,255,  0);
  timeout=100;
  while(timeout--){
    iso += 3;
    if(iso< 0       )iso += DEGPITCH;
    if(iso>=DEGPITCH)iso -= DEGPITCH;

    vram_cls();
    for(i=-10; i<10; i++){
      x1=i*6;
      x2=i*25;
      vram_line((VRAMXRANGE/2)+x1,LANDSCAPEY1,(VRAMXRANGE/2)+x2, 0,color1);
      vram_line((VRAMXRANGE/2)+x1,LANDSCAPEY2,(VRAMXRANGE/2)+x2,VRAMYMAX,color2);
    }

    for(deg=0; deg<90; deg+=DEGPITCH){
      y=(255+vect_y1(deg+iso))*LANDSCAPEY1/255;
      vram_line(0,LANDSCAPEY1-y,VRAMXMAX,LANDSCAPEY1-y,color1);
      vram_line(0,LANDSCAPEY2+y,VRAMXMAX,LANDSCAPEY2+y,color2);
    }
    disp_update();
  }
}

//  DEMO
void vmeter(void)
{
  int x,y;
  int adc;
  int deg;
  int timeout;
  unsigned int color1,color2,color3;

  vram_cls();
  color1 = color16bit(  0,  0,255); /*color 1*/
  color2 = color16bit(255,  0,  0); /*color 2*/
  color3 = color16bit(255,255,  0); /*color 2*/
  deg=0;
  timeout=100;
  while(timeout--)
  {
    adc = vect_x1(deg);
    vram_scroll(2,0);
    x = VRAMXMAX;
    for(y=0; y<VRAMYRANGE; y+=10){
        vram_pset(x, y ,color1);
    }
    y = (VRAMYRANGE/2)-(adc*(VRAMYRANGE/2)/256); 
    vram_pset(x, y,color2);
    if((deg % 90) == 0){
      for(y=0; y<VRAMYRANGE; y+=2){
        vram_pset(x, y ,color3);
      }
    }
    deg = (deg+5)% 360;
    disp_update();
  }
}

#if USE_AUDIO
#define PIN_AUDIO   17  // GPIO
// INITIALIZE
void audio_init(void)
{
  pinMode(PIN_AUDIO, OUTPUT);
  digitalWrite(PIN_AUDIO , LOW);
  analogWriteRange(256);
}

//
void audio_test(void)
{
  const int hz[] = { 440,494,524,586,660,698,784};
  int notemax = sizeof(hz) / sizeof(hz[0]);
  int i;
  unsigned int keydata;

  vram_putch('\n');
  disp_update();
  for(i=0; i < notemax; i++) {
    analogWrite(PIN_AUDIO,128);
    analogWriteFreq(hz[i]);
    vram_putdec(hz[i]);
    vram_putstr((unsigned char*)"Hz\n");
    disp_update();
    delay(1000);
  }
  analogWrite(PIN_AUDIO,0);
  digitalWrite(PIN_AUDIO , LOW);
}
#endif

#if USE_MOTOR
#define PIN_MOTOR   16  // GPIO

// INITIALIZE
void motor_init(void)
{
  pinMode(PIN_MOTOR, OUTPUT);
  digitalWrite(PIN_MOTOR , LOW);
}

// VIBRATION MOTOR
void motor_set(int timecnt)
{
  digitalWrite(PIN_MOTOR , HIGH);
  delay(timecnt);
  digitalWrite(PIN_MOTOR , LOW);
}

//
void motor_test(void)
{
  unsigned int keydata;
  int loopcnt;

  loopcnt=3;
  while(loopcnt--){
    motor_set(100);      // vibration
    delay(1000);
  }
}
#endif

#if USE_RTC
#define RTC_ADDR  (0xA2 >> 1)  // I2C address

// INITIALIZE
void rtc_init(void)
{
  Wire1.setSDA(18);
  Wire1.setSCL(19);
  Wire1.begin();
}

// 現在時刻表示
void rtc_get(unsigned char *dat)
{
  int i;

  Wire1.beginTransmission(RTC_ADDR);   
  Wire1.write(2);            
  Wire1.endTransmission();      
  Wire1.requestFrom(RTC_ADDR, 7);
  if (Wire1.available() >= 7) {
    for(i=0; i<7; i++){
      *(dat+i) = Wire1.read();
    }
  }

  *dat = bcd_to_num(*dat & 0x7F); // sec
  dat++;
  *dat = bcd_to_num(*dat & 0x7F); // mint
  dat++;
  *dat = bcd_to_num(*dat & 0x3F); // hour
  dat++;
  *dat = bcd_to_num(*dat & 0x3F); // day
  dat++;
  *dat = *dat & 0x7;  // day of week
  dat++;
  *dat = bcd_to_num(*dat & 0x1F); // month
  dat++;
  *dat = bcd_to_num(*dat);        // year
}

// 時刻設定
void rtc_set(unsigned char *dat)
{
  int i;
  Wire1.beginTransmission(RTC_ADDR);   
  Wire1.write(2);
  for(i=0;i<7;i++){
    Wire1.write(num_to_bcd(*dat));
    dat++;
  }            
  Wire1.endTransmission();      
}

//
void rtc_test(void)
{
  unsigned char dat[7];
  int loopcnt;

  loopcnt=10;
  vram_textcolor(color16bit(128,255,  0));
  while(loopcnt--){
    rtc_get(dat);
    vram_cls();
    vram_locate(0, 0);
    vram_putstr((unsigned char*)"20");
    vram_putdec2(dat[6]); //year
    vram_putch('\n');
    vram_putdec2(dat[5]); //month
    vram_putch('/');
    vram_putdec2(dat[3]); //day
    vram_putch('\n');
    vram_putdec2(dat[2]); //hour
    vram_putch(':');
    vram_putdec2(dat[1]); //mint
    vram_putch('\n');
    vram_putdec2(dat[0]); //sec
    disp_update();
    delay(1000);
  }
}
#endif

#if USE_KEYPAD
const char keyin[] = {8,9,10,11};
const char keyout[] = {12,13,14,15};

// INIT KEY MATRIX
void key_init(void)
{
  int i;
  for(i=0; i<4; i++){
    pinMode(keyin[i], INPUT_PULLUP);
    pinMode(keyout[i], INPUT);
  }
}

// READ KEY MATRIX
unsigned int key_get(void)
{
  int i;
  int key_count;
  int work;

  work = 0;
  for(key_count=0; key_count<4; key_count++){
    for(i=0; i<4; i++){
      if(key_count == i){
        digitalWrite(keyout[i] , LOW);
        pinMode(keyout[i], OUTPUT);
      }else{
        pinMode(keyout[i], INPUT_PULLUP);
      }
    }
    for(i=0; i<2000; i++){
    }
    for(i=0; i<4; i++){
      work >>= 1;
      if( digitalRead(keyin[i])== LOW)
        work |= 0x8000;
    }
  }
  return(work);
}

// KEY WAIT
unsigned int key_wait(void)
{
  unsigned int dat;
  while(1){
    if(key_get()==0)break;
    delay(20);
  }
  while(1){
    dat = key_get();
    if(dat)break;
    delay(20);
  }
  return(dat);
}

//
void key_test(void)
{
  unsigned int keydata;
  unsigned int mask;

  vram_cls();
  while(1){
    vram_locate(0,0);
    keydata = key_get();
    mask = 0x8000;
    while(mask){
      if(mask & keydata)
        vram_putch('1');
      else
        vram_putch('0');

      if(mask & 0b0001000100010001)
        vram_putch('\n');

      mask >>= 1;
    }
    disp_update();
    delay(20);
  }
}

//
#define MENUMAX 11
const unsigned char strmenu[MENUMAX][7]={
  "LAND",   /*0*/
  "CHARA",  /*1*/
  "LINE",   /*2*/
  "RTC",    /*3*/
  "AUDIO",  /*4*/
  "MOTOR",  /*5*/
  "FIGHT",  /*6*/
  "VECTOL", /*7*/
  "LIFE",   /*8*/
  "BALL",   /*9*/
  "VMETER"  /*10*/
};

//
void menu(void)
{
  int menunum;
  unsigned int keydata;
  menunum=0;

  while(1){
    vram_cls();
    vram_locate(0,0);
    vram_textcolor(color16bit(255,0,0));
    vram_putstr((unsigned char *)"MENU\n\n");
    vram_textcolor(color16bit(255,255,255));
    vram_putstr((unsigned char *)strmenu[menunum]);
    disp_update();

    keydata = key_wait();
    if(keydata & (KEY_UP + KEY_LEFT)){
      menunum--;
      if(menunum<0)menunum=MENUMAX-1;
    }
    if(keydata & (KEY_DOWN + KEY_RIGHT)){
      menunum++;
      if(menunum>=MENUMAX)menunum=0;
    }
    if(keydata==KEY_ENTER){
      switch(menunum){
      case 0:
        landscape();
        break;
      case 1:
        chardemo();
        break;
      case 2:
        linedemo();
        break;
#if USE_RTC
      case 3:
        rtc_test();
        break;
#endif
#if USE_AUDIO
      case 4:
        audio_test();
        break;
#endif
#if USE_MOTOR
      case 5:
        motor_test();
        break;
#endif
      case 6:
        spacefight();
        break;
      case 7:
        vectordemo();
        break;
      case 8:
        lifegame();
        break;
      case 9:
        balldemo();
        break;
      case 10:
        vmeter();
        break;
      default:
        vram_putstr((unsigned char *)"\nNOT SUPPORT!");
        disp_update();
        delay(3000);
        break;
      }
    }
  }
}
#endif

// COLOR TEST
void tft_colortest(void)
{
  int x,y,r,g,b,mode;
  int vol,vol2;

  for(y=0; y<VRAMYRANGE; y++)
  {
    for(x=0; x<VRAMXRANGE; x++)
    {
      mode = y / (VRAMYRANGE/4);
      vol = 256*x/VRAMXRANGE;
      vol2 = 256*(y % (VRAMYRANGE/4))/(VRAMYRANGE/4);
      r = g = b = 0;
      if(mode==0){
        r = vol;
        b = vol2;
      }
      if(mode==1){
        g = vol;
        r = vol2;
      }
      if(mode==2){
        b = vol;
        g = vol2;
      }
      if(mode==3)r = g = b = vol;
      vram_pset(x,y,color16bit(r,g,b));
    }
  }
  disp_update();
  delay(3000);
}

//----
void setup(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(115200);
#if USE_KEYPAD
  key_init();
#endif
#if USE_RTC
  rtc_init();
#endif
#if USE_AUDIO
  audio_init();
#endif
#if USE_MOTOR
  motor_init();
#endif
  disp_init();
  vram_textzoom(FONTSIZE);
}

//
void loop()
{
  tft_colortest();

#if USE_KEYPAD
  menu();
#else
  landscape();
  chardemo();
  linedemo();
  spacefight();
  vectordemo();
  balldemo();
  lifegame();
  vmeter();
#endif
}
