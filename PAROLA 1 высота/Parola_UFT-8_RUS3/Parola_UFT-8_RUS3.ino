// Program to demonstrate the MD_Parola library
//
// Demonstrated how UTF-8 multi-byte characters can be mapped to Extended ASCII
// characters that can be displayed with a suitable font file.
//
// NOTE: MD_MAX72xx library must be installed and configured for the LED
// matrix type being used. Refer documentation included in the MD_MAX72xx
// library or see this link:
// https://majicdesigns.github.io/MD_MAX72XX/page_hardware.html
//

#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>
#include "Parola_Fonts_data.h"
//#include "Font_Data_RUS.h" //English + Russian
// Define the number of devices we have in the chain and the hardware interface
// NOTE: These pin numbers will probably not work with your hardware and may
// need to be adapted
#define MAX_DEVICES 8 /// количество девайсов

#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10

uint8_t cycle=0;
uint8_t pause=0;
uint8_t speed=30;
// Hardware SPI connection
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);
char  effekt[] =
 { PA_PRINT, //0///написал весь каталог, для образца
   PA_SLICE,  //1 // в анимации этого скетча задействован только PA_SCROLL_LEFT, PA_SCROLL_LEFT
   PA_MESH,  //2
   PA_FADE,  //3
   PA_WIPE,  //4
   PA_WIPE_CURSOR,  //5
   PA_OPENING,   //6
   PA_OPENING_CURSOR ,  //7
   PA_CLOSING,   //8
   PA_CLOSING_CURSOR,   //9
   PA_RANDOM,  //10
   PA_BLINDS,  //11
   PA_DISSOLVE,  //12
   PA_SCROLL_UP,   //13
   PA_SCROLL_DOWN, //14
   PA_SCROLL_LEFT,   //15
   PA_SCROLL_RIGHT, //16
   PA_SCROLL_UP_LEFT,  //17
   PA_SCROLL_UP_RIGHT,  //18  
   PA_SCROLL_DOWN_LEFT,  //19
   PA_SCROLL_DOWN_RIGHT, //20
   PA_SCAN_HORIZ,   //21
   PA_SCAN_VERT,  //22
   PA_GROW_UP, //23
   PA_GROW_DOWN  //24
   };

char	*msg[] =
{
  "ABCDEFGHIJabcdefghij",  ///0
  "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ",  //1
  "абвгдежзийклмнопрстуфхцшщъыьэюя",  ///2
  "MD.PAROLA",    //3
  "1234567890",    //4
  "`!@#$%^&*()_+-={};:'\"<>?,./|\\{}",   //5
  "\x0c5\x020\x020\x0c6\x0c7\x0c8\x020\x0c9\x0c8\x0ca\x0cb\x020\x020\x0c5"  // МИР ТРУД  ///6
 };
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

void setup(void)
{  Serial.begin(57600);
 // Initialise the Parola library
  P.begin();
  P.setFont(0,font_rus);
}

void loop(void)
{  
    P.setSpeed(speed);///меняем скорость
    uint8_t j=9;   //яркость//1-15//
    P.setIntensity(j) ; //яркость//1-15//
   
if ( P.displayAnimate())
   //       //1 высота(0)//текст № //место старта//скорость //пауза//эфект появления//эфект ухода //
   { P.displayZoneText(0, msg[cycle], PA_CENTER, P.getSpeed(), pause, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
   //пример2 //P.displayZoneText(0, msg[4], PA_CENTER, 30, 0, effekt[0], effekt[19]);
   //пример3// P.displayZoneText(0, "ПРИВЕТ МИР", PA_CENTER, 30, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
   //пример4 //P.displayZoneText(0, msg[3], PA_LEFT, 30, 0, effekt[13], effekt[13]);
   //пример5 //P.displayZoneText(0, msg[3], PA_RIGHT, 30, 0, effekt[15], effekt[15]);
    cycle = (cycle + 1) % ARRAY_SIZE(msg);
   }
  
if (cycle==5) cycle=0;
}

