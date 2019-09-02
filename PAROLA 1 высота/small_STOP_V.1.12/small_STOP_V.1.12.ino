//*************************************************************************************
// этот скетч является обновлением СТОП 2в1. Адаптированным под библиотеки Parola 3.0.2
// также адатация кирилицы под среду IDE 1.8.8 
// дабавленна функция конвертации кириллицы
/* нужно убрать лишние эффекты в файле=  MD_Parola.h
#define ENA_MISC 0    ///< Enable miscellaneous animations< выключить разные анимации
#define ENA_WIPE 0    ///< Enable wipe type animations// выключить анимацию типа очистки
#define ENA_SCAN 0    ///< Enable scanning animations //выключить анимацию сканирования
#define ENA_SCR_DIA 0 ///< Enable diagonal scrolling animation//выключить анимацию диагональной прокрутки
#define ENA_OPNCLS 1  ///< Enable open and close scan effects //включить эффекты открытия и закрытия сканирования
#define ENA_GROW 0    ///< Enable grow effects//Выключить эффекты роста
#define ENA_SPRITE 0  ///< Enable sprite effects//Выключить эффекты спрайтов
# define ENA_GRAPHICS 0 // / < Enable Graphics functionality//Выключить графические функции
#define USE_LOCAL_FONT 0  //Выключить родной англ шрифт */
#include <EEPROM.h> // подключаем библиотеку EEPROM 
#include <MD_Parola.h>  //библиотека 2 высоты
#include <MD_MAX72xx.h> //
#include <SPI.h>
#include "Fonts_RUS_5x8.h"///подключаем кирилицу// широкий кратный2.//большие и маленькие русские!!!!!!!
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define  CLK_PIN   13  //выходы зашищаем резисторами по 100  ом
#define DATA_PIN  11  //выходы зашищаем резисторами по 100  ом
#define CS_PIN    10  //выходы зашищаем резисторами по 100  ом
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
//#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0])) ///пересчитываем массив

uint8_t speed= 25; // стартовая скорость//скорость движения//99-медлено 0-быстро
unsigned long taimer1=0, taimer2=0, taimer4=0, taimer5=0, taimer6=0, taimer7=0,time_sbros =0;  // здесь будет храниться время таймеров
uint8_t s=0,y=0,eeprom=0,text_old=3,opros=0,textN =3,len=25,lennn=0,ctop=0; ////флаги //
boolean tormaz=0,nazad=0,pravo=0,flag_tel=0, //флаги //
        levo=0,flag_Jstik=0,avariik=0; ////флаги //
unsigned int val = 1200;
 //____________________________________________________________________________________________________
#define PinTorz 4    //тормоз // присваиваем пины входов от ламп машины
#define PinNazd 5    //назад  // скетч использует 4,5,6,7  пины. если возьмете дугие пины нужно менять  "PIND<16 "
#define PinPrav 6    //вправо //
#define PinLev 7     //налево //
#define AnalogPin  A0  //джойстик//
uint8_t zadan_dlina = MAX_DEVICES+2; //количество букв помещающихся на табло//
String Str_telf; ///для хранения фраз телефона
boolean str_old=0;

//--------------------------------------------------------------------------------------------------------
#define time_invers 500   // время через которое СТОП инвертируется//1000млс=1сек//примерно
#define time_povorot  800 //время межу морганиями поворотов (негорит)
#define time_spasibo  10000 //время горения "спасибо"
unsigned long vremya_long= 15;  // время ~ (сек)отображения фраз с пульта до сброся на  " /!  "   //
int adrees=0;  //адрес ардуино//29184
int skorst=25; // скорость(1-100)//0 быстро 99-медлено//так в PAROLE
int jarkost=9; //яркость(1-15)
int invers=6;  // инверсия
int cikl=1;    // цикличность//1нет//2 да
int vremya=15; //время фраз
int coxpan=1;  //сохранить еером//1нет//2 да
int iffekt=1;  // бежит//1-нет//2- да
int shtorki=1; //эфект шторки
int sped_bufer=70; //0 медлено, 99- быстро//так передаем телефоном, потом инвертируем под паролу.

 //_______________________________________________________________________________________________________
#define masiv 11
char *msg_avto[masiv] = /// буквы "Ё" нет!
 {/////////сервисные фразы. не трогаем их//////////////////хотя... :)
  "ПОЕХАЛИ",   //0// фраза -можно поставить на старт
  "СПАСИБО",   //1//
  "<- ! ->",   //2// 
  "<<<>>>",    //3/// 
  "СТОП",      //4//CТОП//
  "! >>>>",    //5//поворот вправо//используем эфект шторки (половину его)
  "<<<< !",    //6//поворот влево//используем эфект шторки (половину его)
  "НАЗАД",    //7// назад//
  "1раз",    //8// тут свое пишем для джойстика
  "2два",    //9//  тут свое пишем для джойстика
  "3три"    //10//  тут свое пишем для джойстика
  };
  //======================================================================================
 char *msg_tel; ///резервируем память под массив фраз с тел
 // char* RUS2 ;
 char efkt[] =  /// эфекты из библиотеки////WIPE,  WIPE_CURSOR,  OPENING, OPENING_CURSOR, ....
{  PA_PRINT,       //0// появдяется в центре
   PA_NO_EFFECT,   //1// 
   PA_SCROLL_LEFT, //2// бежит налево
   PA_CLOSING      //3// шторки
 } ;
 //=================считаем длину строки=====================================================
uint8_t dlina_telef(String Str_tel){ // тут ничего не трогаем
byte n=0,count=0;
while(Str_tel.charAt(n) != '\0')
{if (Str_tel.charAt(n) < ' ') n+=2;
else n++;count++;
} return count;
}

void sbros() 
{ P.setInvert(false); tormaz=0; nazad=0; levo=0; pravo=0; avariik =0; // сбрасываем флаги. ждем новых команд
  Str_telf=""; //msg_tel="";// чистим память
} 

///=================setup=============///==================///=====================///============
 void setup() {
  P.begin();
  P.setInvert(false);
  P.setFont(RusASCII);
  //---------------подключаем входы от машины --------------------------------------------
  digitalWrite(PinTorz,HIGH);  // тормоз// 
  digitalWrite(PinNazd,HIGH);  // назад // ВКЛ ВНУТРЕНИЕ РЕЗИСТОРЫ НА +5в
  digitalWrite(PinPrav,HIGH);  // поворот право
  digitalWrite(PinLev,HIGH);  // поворот лево///*/
 Serial.begin(9600);  /// для блютуз СН-06
// Serial.begin(38400);// для блютуз СН-05
  //------------чтение сохранненых переменных их энергонезависимой памяти ----------------------- 
if (EEPROM.read(61)==177){ // 61- ячейка как флаг,пока мы ни разу не сохранялись,то настройки по умолчанию (не из памяти)
 skorst=EEPROM.read(31);jarkost=EEPROM.read(32); invers=EEPROM.read(33); 
 vremya=EEPROM.read(34); iffekt=EEPROM.read(35);shtorki=EEPROM.read(36);
 skorst=map(sped_bufer, 10, 99, 60, 5); //ивертирует диапазон под паролу
}
vremya_long=vremya; //меняем переменные для коррекного умножения ниже
time_sbros=vremya_long*1000; /// переводим сек в млсек
P.setIntensity(jarkost) ; //яркость//
for (byte i=0; i < masiv;i++ ) {
     char* RUS = msg_avto[i]; 
     utf8Ascii(RUS); // конвертируем массив UTF-8  ASCII
     }
}

 //=================loop===============================================================
void loop() {
   unsigned long new_taimer = millis(); //обновляем время в миллисекунд.(для таймеров).
 //--------------------------------------------------------------------------------
    val = analogRead(AnalogPin); // считываем значение с аналогового входа в переменную val
if (val < 820 && !flag_Jstik)  {
    flag_Jstik=1; flag_tel=0; taimer6 = new_taimer;  //запускаем таймер горения фразы
    if      (val < 70)   opros=0, textN=8; // сверяем переменную, если val меньше 50 возвращаем 1 (первая кнопка)
    else if (val < 350)  opros=0, textN=9; // если val меньше 150 вторая кнопка и т.д.
    else if (val < 600)  opros=0, textN=10; //
    else if (val < 820)  opros=4, textN=1;  //эффект= "бегущ", фраза= спасибо
    else flag_Jstik =0;  // если ничего не подошло,выходим
    P.setInvert(false);
    }
else if (flag_Jstik ) 
        {if (new_taimer - taimer6 >= time_sbros && cikl==1 ) //пауза//если цикл=2, то фраза горит бесконечно
             {textN=0; flag_tel=0; flag_Jstik =0;  opros=0; sbros(); // по истечении паузы сбрасываем  на знак "/! " ///
              P.setSpeed(0);///меняем скорость
              P.displayClear();
              s=0; y=1;
             }   
        }
 //==================== прием по блютуз фраз и команд =========================================
else if (Serial.available() )  /// если данные пришли по блютуз
     { Str_telf = Serial.readString();
       flag_tel=1; str_old=1; //опрокидываем флаг
       taimer6 = new_taimer;  //запускаем таймер горения фразы
     }
 //----------------------обработка команд с телефона----------------------------------------------------
if  (flag_tel) // если фраза с телефона
  { msg_tel = Str_telf.c_str(); len= Str_telf.length() ;//преобразовываем массив//считаем длину
    sscanf(msg_tel, "%d,", &adrees);     //берем 5 символов из фразы
    if (adrees == 29184 && len>21&&len<26) //если пришла команда, разгбераем её на части отделенные запятой
     {
      sscanf(msg_tel, "%d,%d,%d,%d,%d,%d,%d,%d,%d", 
     &adrees,&sped_bufer,&jarkost,&invers,&cikl, &vremya, &coxpan, &iffekt, &shtorki);// переменные
     //--1-------2---------3--------4-------5-------6---------7-------8--------9------  № п/п
     //29184----70---------9--------6-------1-------15--------2-------1--------1------  значение
    //29184,70,9,6,1,15,2,1,1  // пример для записи EEPROM через порт 
      skorst=map(sped_bufer, 10, 99, 60, 5); //ивертирует диапазон под паролу, после новой команды.
      vremya_long=vremya; //меняем переменные для коррекного умножения ниже
      time_sbros=vremya_long*1000;  /// переводим сек в млсек
      P.setIntensity(jarkost) ;     //яркость//
    if (coxpan==2)  //если пришла команда записать//седьмая позиция в команде
       {EEPROM.update(31, skorst);  /// если значение новое перезаписываем.
        EEPROM.update(32, jarkost); //31,32..36 от фоноря взял //лишь бы не первые//
        EEPROM.update(33, invers);
        EEPROM.update(34, vremya);
        EEPROM.update(35, iffekt);
        EEPROM.update(36, shtorki);
        EEPROM.update(61, 177);         // как флаг того - что были сохранены настройки 
       }
    flag_tel=0; adrees = 0;//выходим из записи
     } //
//------------------обработка фраз с телефона--------------------------------------------------------------
 else {//если пришла фраза и это не команда то
    if(str_old) 
      {lennn = dlina_telef(Str_telf);
       if  (lennn > zadan_dlina || iffekt==2) s=2,y=2;//если фраза не помещаеться//или мы хотим ,что бы бежала.
       else if (lennn <= zadan_dlina && shtorki==2)  s=0,y=3;//если мы выбрали эфект шторки с телефона
       else s=0,y=1;  // строка статична
       utf8Ascii(msg_tel); // конвектируем кириллицу
       str_old = 0;
      }
  if  (P.displayAnimate())  {
        P.setInvert(false);//убираем инверсию если она была//подстраховочка
        P.displayText( msg_tel, PA_CENTER,P.getSpeed(), 0, efkt[s], efkt[y]);
        //str_old = 0; 
       }
    if (new_taimer - taimer6 >= time_sbros && cikl==1 ) //пауза//если цикл=2, то фраза горит бесконечно
       {flag_tel=0;adrees = 0; opros=0; sbros(); textN =0; // по истечении паузы сбрасываем  на знак "/! " ///
        P.setSpeed(0); P.displayClear();
        s=0;y=1;
       } 
  }//
}//=============== тут управляет автомобиль уже сам ================================================================
else if (PIND<16 && !tormaz && !pravo && !levo&& !nazad && !avariik) // если ничего не нажато и флаги сброшены
        {textN =3; opros=0;
        }
// если, что то НАЖАТО, проверяем что?//по приоритету после первой истины, далее не проверяем (else  if)//
   //---------------------стоп --------------------------------------------------------------------
else if (!tormaz&& digitalRead(PinTorz)) ///если нажат тормоз первый раз
         {tormaz=1;opros=0;textN =4;taimer1 = new_taimer; ctop=0;//запускаем таймер инверсии
         }
else if (tormaz)
        {if ( ctop >= invers)    P.setInvert(false);   //если прошло хх инверсий, то больше не раздражаем соседа сзади,
         else if (new_taimer - taimer1 > time_invers)  //если прошло хх сек, а тормоз нажат,то
                 { taimer1 = new_taimer;       // запоминаем время
                  P.setInvert(!P.getInvert()); // инвертируем СТОП
                  ++ctop;   
                 } 
       if (!digitalRead(PinTorz))    sbros(); /// отпущен тормоз, сброс
       }
 //-----------------назад-------------------------------------------------------------------------  
else if (!nazad &&digitalRead(PinNazd))//нажато назад
        {nazad=1;opros=3;textN =7;
        } //увеличиваем скорость(как всегда),для быстрой смены фразы//    
else if (nazad)
        {if (!digitalRead(PinNazd) ) sbros(); //отпустили назад, сброс 
        }   
   //----------------аварийка-------------------------------------------------------------------------  
else if (!avariik && digitalRead(PinPrav) && digitalRead(PinLev)) //2поворота
        {opros=0; textN =1;avariik=1; taimer4 = new_taimer; taimer5 = new_taimer;
        }  // аварийка // спасибо
else if (avariik) //спасибо горит ХХ сек даже если было однократное моргание //time_sbros 
        {
         if (digitalRead(PinPrav)&&digitalRead(PinLev)) taimer4 = new_taimer;//2поворота снова моргнули//перевзводим таймер между морганиями аварийки 
         if (new_taimer - taimer5 >= time_spasibo) opros=3,textN =2;//выкл СПАСИБО ели прошло хх сек на 2 знака   "/!  /!  ///
         if (opros==3&&(new_taimer - taimer4 >= time_povorot)) sbros();//больше не моргает аварийка
        }
    //------------нажат поворот влево--------------------------------------------
else if (!levo && !avariik && !digitalRead(PinPrav)&& digitalRead(PinLev))// вкл поворот влево
         { opros=2;textN =6;levo=1;taimer2 = new_taimer;
         }  // СТРЕЛКИ бегут влево
else if (levo)
        { if (!digitalRead(PinPrav)&& digitalRead(PinLev))  taimer2 = new_taimer;// вкл поворот влево//перевзвдим  таймер2 
          if (!digitalRead(PinLev)&&(new_taimer - taimer2 >= time_povorot)) sbros(); // выкл поворот вправо
        }
   //-------------нажат поворот вправо------------------------------------------
else  if (!pravo&& !avariik &&digitalRead(PinPrav)&& !digitalRead(PinLev))// 
         {opros=2;textN =5;pravo=1;taimer7 = new_taimer;
         }  // СТРЕЛКИ бегут вправо
else if (pravo)
       {if (digitalRead(PinPrav)&& !digitalRead(PinLev))  taimer7 = new_taimer; // СТРЕЛКИ бегут вправо//певзводим таймер3 
        if (pravo && !digitalRead(PinPrav)&&(new_taimer - taimer7 >= time_povorot)) sbros(); // выкл поворот вправо
       }
  //===========приступаем к анимации===========================================================================
 /// выводим результат на табло//анимация//выбор эфекта//норнализуем скорость//
if (!flag_tel)  //если нет фразы стелефона
    {
     switch (opros) //
        { case 0: s=0; y=1; P.setSpeed(skorst);  break;  //строка стaтична
          case 1: s=0; y=3; P.setSpeed(skorst);   break;  //используем эфект шторки (половину его)
          case 2: s=0; y=3; P.setSpeed(skorst);   break;  // используем эфект шторки (половину его)
          case 3: s=0; y=3; P.setSpeed(skorst+15);  break;//назад и аварийка//эфк шторки//чуть помедленее кони.
          case 4: s=2; y=2; P.setSpeed(skorst);  break;   //эффект беущая строка 
          default:s=0; y=1; textN=0;  //если ничего не совпало//строка стaтична
         }
     if (textN!=text_old) 
         { // при смене фраз  для быстрого ухода предыд фразы
          P.setSpeed(0);///меняем скорость
          P.displayClear();
         }
     if (P.displayAnimate())  {
         P.displayText( msg_avto[textN], PA_CENTER,P.getSpeed(), 0, efkt[s], efkt[y]);
         text_old=textN;// выравниваем номер фраз чтобы скорость не росла
        } 
    }
} ///===============================================================================================

//------------конвектор отсекающий первый байт кириллицы-------------------------------------------------
uint8_t utf8Ascii(uint8_t ascii)
{
 static uint8_t cPrev;
  uint8_t c = '\0';
  if (ascii < 0x7f )   // Standard ASCII-set 0..0x7F, no conversion
  {
    cPrev = '\0';
    c = ascii;
  }
  else 
  {
    switch (cPrev)  // Conversion depending on preceding UTF8-character
    {
    case 194: c = ascii; break;
    case 195: c = ascii | 0xC0;  break;
     case 208: c = ascii | 0x100;  break;// кириллица
     case 209: c = ascii | 0x100;  break;// кириллица 
    }
    cPrev = ascii;   // save last char
  }
  return(c);
}
void utf8Ascii(char* s)
// In place conversion UTF-8 string to Extended ASCII
// The extended ASCII string is always shorter.
{
  uint8_t c, k = 0;
  char *cp = s;
  while (*s != '\0')
  {
    c = utf8Ascii(*s++);
    if (c != '\0')
      *cp++ = c;
  }
  *cp = '\0';   // terminate the new string
}
