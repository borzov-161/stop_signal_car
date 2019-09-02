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
#define MAX_DEVICES 8
#define CLK_PIN   13
#define DATA_PIN  11
#define CS_PIN    10
uint8_t cycle=0;
// Hardware SPI connection
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
// Arbitrary output pins
// MD_Parola P = MD_Parola(DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

char	*msg[] =
{
  "abcABC",
  "АБВГДЕЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯ",
  "абвгдежзийклмнопрстуфхцшщъыьэюя",
  "MD PAROLA",
  "1234567890",
  "`!@#$%^&*()_+-={};:'\"<>?,./|\\{}",
  "Кириллица",
};
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))

// Global data
typedef struct
{
  textEffect_t  effect;   // text effect to display
  char *        psz;      // text string nul terminated
  uint16_t      speed;    // speed multiplier of library default
  uint16_t      pause;    // pause multiplier for library default
} sCatalog;

sCatalog  catalog[] =
{
  { PA_PRINT, "ЭФЕКТ PRINT", 1, 1 }, //0
  { PA_SLICE, "ЭФЕКТ SLICE", 1, 1 }, //1
  { PA_MESH, "ЭФЕКТ MESH", 20, 1 },  //2
  { PA_FADE, "ЭФЕКТ FADE", 20, 1 },  //3
  { PA_WIPE, "ЭФЕКТ WIPE", 5, 1 },   //4
  { PA_WIPE_CURSOR, "эфк  WPE_C", 4, 1 },  //5 //////////////
  { PA_OPENING, "ЭФЕКТ OPEN", 3, 1 },   //6
  { PA_OPENING_CURSOR , "ЭФЕКТ OPN_C", 4, 1 },   //7
  { PA_CLOSING, "ЭФЕКТ CLOSE", 3, 1 },   //8
  { PA_CLOSING_CURSOR, "ЭФЕКТ CLS_C", 4, 1 },  //9
  { PA_RANDOM, "ЭФЕКТ RAND", 3, 1 },  //10
  { PA_BLINDS, "ЭФЕКТ BLIND", 7, 1 },  //11
  { PA_DISSOLVE, "ЭФЕКТ DSLVE", 7, 1 },  //12
  { PA_SCROLL_UP, "ЭФЕКТ SC_U", 5, 1 },  //13
  { PA_SCROLL_DOWN, "ЭФЕКТ SC_D", 5, 1 },//14
  { PA_SCROLL_LEFT, "ЭФЕКТ SC_L", 5, 1 },  //15
  { PA_SCROLL_RIGHT, "ЭФЕКТ SC_R", 5, 1 },//16
  { PA_SCROLL_UP_LEFT, "ЭФЕКТ SC_UL", 7, 1 }, //17
  { PA_SCROLL_UP_RIGHT, "ЭФЕКТ SC_UR", 7, 1 }, //18  
  { PA_SCROLL_DOWN_LEFT, "ЭФЕКТ SC_DL", 7, 1 }, //19
  { PA_SCROLL_DOWN_RIGHT, "ЭФЕКТ SC_DR", 7, 1 },//20
  { PA_SCAN_HORIZ, "эфк SCANH", 4, 1 },  //21
  { PA_SCAN_VERT, "эфк SCANV", 3, 1 }, //22
  { PA_GROW_UP, "ЭФк GRW_U", 7, 1 }, //23
  { PA_GROW_DOWN, "эюя GRW_D", 7, 1 }, //24
};
void setup(void)
{  Serial.begin(57600);
 // Initialise the Parola library
  P.begin();
  P.setFont(0,font_rus);
  P.setInvert(false);
 
  for (uint8_t i=0; i<ARRAY_SIZE(catalog); i++)
  {
    catalog[i].speed *= P.getSpeed();
    catalog[i].pause *= 500;
  }
}


void loop(void)
{  
  for (uint8_t j=0; j<3; j++)
  {
    textPosition_t  just;

    switch (j)
    {
    case 0: just = PA_LEFT;    break;
    case 1: just = PA_CENTER;  break;
    case 2: just = PA_RIGHT;   break;
    }

    for (uint8_t i=0; i<ARRAY_SIZE(catalog); i++)
    {
      P.displayText(catalog[i].psz, just, catalog[i].speed, catalog[i].pause, catalog[i].effect, catalog[i].effect);

      while (!P.displayAnimate())
        ; // animates and returns true when an animation is completed

      delay(catalog[i].pause);
    }
  }
}
/////////////////////////////////////////////////////////    
/*if ( P.displayAnimate())
     { P.displayZoneText(0, msg[cycle], PA_CENTER, 40, 0, PA_SCROLL_LEFT, PA_SCROLL_LEFT);
     cycle = (cycle + 1) % ARRAY_SIZE(msg);
     }
  if (cycle==5) cycle==0;
}*/

