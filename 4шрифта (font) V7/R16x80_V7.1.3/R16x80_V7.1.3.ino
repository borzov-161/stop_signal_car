/// http://arduinocode.codeplex.com
///https://www.drive2.ru/users/borzov161/
///https://www.drive2.ru/l/459173099263246069/
///https://www.drive2.ru/b/456897934827455433/  //// 
/////////////////////////////////////////////////////////////////////////
#include <MD_Parola.h>  //библиотека 2 высоты
#include <MD_MAX72xx.h> //
#if USE_LIBRARY_SPI
#include <SPI.h>
#endif
//#include "RUS5.h"////подключаем кирилицу//широкий шрифт
 #include "RUS4.h"////подключаем кирилицу// узкий  шрифт
#define MAX_ZONES 2  ///высота 2 девайса
#define ZONE_SIZE 10  // длина 10 девайсов//тут свое пишем//ограничение не знаю
#define  MAX_DEVICES (MAX_ZONES * ZONE_SIZE)
#define ZONE_UPPER  1
#define ZONE_LOWER  0
#define  CLK_PIN   13  ///пины для табло //
#define DATA_PIN  11
#define CS_PIN    10
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);
#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0])) ///пересчитываем массив

#include <IRremote.h>////ифк библиотека//
uint8_t RECV_PIN = 8; /// пин подключения приемника ифк
IRrecv irrecv(RECV_PIN); // присваиваем пин приема
decode_results results;//
uint8_t mytext = 20;///фраза по умолчанию//
uint8_t skorost=15; // стартовая скорость//скорость движения//99-медлено 0-быстро
uint8_t distanc=20;  ///слово с дистации при старте//появляется вторым
uint8_t opros=5; ///переменная для хранения состояния машины
uint8_t rejim=0; ///выбор эффекта режима
uint8_t tormaz=0;   uint8_t  nazad=0; ////флаги
uint8_t i=0;        uint8_t y=0;
uint8_t pravo=0;    uint8_t levo=0; ///флаги
uint8_t spasibo=0;  uint8_t avariik=0;
uint8_t sobutie=0;  // считает нажатия педали// т.к. в MD_Parola не работает пауза. То сборс команд пульта по 6(например) событию.
// мограния поворота идут через кондесатор,считается как 1 нажатие.

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0])) ///пересчитываем массив
char *msg[] = 
 {"161 RUS",    //0 //
  "СЛЕПИШЬ",    //1 // слепишь
  "САМ ТАКОЙ",  //2 //сам такой
  "НЕ ДУДИ",    //3/ //FFCLUB/  оставдяем коментарии, чтоб не забыть где что
  "DRIVE2",    //4//DRIVE2
  "ИЗВИНИ",    //5// извини
  "ПОДОЖДИ",   //6// подожди
  "СПАСИБО",   //7//спасибо
  "УДАЧИ",     //8// удачи
  "УЛЫБНИСЬ",  //9// улыбнись
  ///////////10-20 случайный перебор слов////////////////////////////
  "YAHOOЕЮ ОТ ЭТИХ ДОРОГ", //10 //от этих дорог 
  "ШИНУ R16",     //11// шину R14 ??
  "БУКСИР",       //12/буксир
  "ПЖЛ АИ92",     //13// пжл аи92
  "ТОЛКНИТЕ",     //14// толкните
  "S O S",        //15// SOS
  "ТАНКИ ГРЯЗИ НЕ БОЯТСЯ",    //16// танки грязи ..
  "В КАКОЙ ЯМЕ МОИ НАЛОГИ",   //17//в какой яме мои налоги
  "ПОНЯТЬ И ПРОСТИТЬ",        //18//понять и простить
  "\x0bc\x0bd",               //19// рисунок машина
  //////////////////////сервисные фразы. не трогаем их//////////////////хотя...
  "\x083", //20/:)// мягкий старт// тут как можно меньше писать.А то не стартует ардуино.
  "\x081\x020\x082\x020\x0be\x020\x0bf", //21//C Т О П//ЖИРНЫМ ШРИФТОМ
  ">>> >> >",             //22//>>>  //поворот вправо
  "< << <<<",             //23// <<<  ///поворот влево
  "< НАЗАД >"};               //24// назад
 ///................................................................................................................../// 
 char efkt[] = { /// эфекты из библиотеки////WIPE,  WIPE_CURSOR,  OPENING, OPENING_CURSOR, ....
   PRINT,       //0// появдяется в центре
   NO_EFFECT,   //1// 
   SCROLL_LEFT, //2// бежит налево
   SCROLL_RIGHT,//3// бежит направо
   CLOSING  };  //4// шторки
 
 void zumer() {analogWrite(9, 50);delay(30);  //пришла команда ифк- пик-пик//
                analogWrite(9, 100);delay(30); //контроль приема звуком. т.к. не видим,что там сзади авто. 
                analogWrite(9, 0); }// выключаем звук
 void sbros() {tormaz=0;nazad=0;levo=0;avariik=0;pravo=0;skorost=25;P.setInvert(LOW); sobutie++;} // сбрасываем флаги. ждем новых команд
    ////////////////////////////////////////////////////////////////////////////////////////////////
void setup(void) {
   Serial.begin(57600); ////добавить если не стартонет
   /////////////////////организуем текст в 2 строки//////////////////////////////////////////////////////
  delay(1000); //ждем пока питание устаканится
  P.begin(MAX_ZONES);
  P.setZone(ZONE_LOWER, 0, ZONE_SIZE - 1);
  P.setFont(ZONE_LOWER, BigFontLower);
  P.setZone(ZONE_UPPER, ZONE_SIZE, MAX_DEVICES-1);
  P.setFont(ZONE_UPPER, BigFontUpper);
//  P.setCharSpacing(P.getCharSpacing()*2);//двойной высоты->двойной интервал//для кирилицы не ставивим, там уже2
      P.displayZoneText(ZONE_LOWER,".", CENTER,30, 0,  SCROLL_LEFT,  SCROLL_LEFT);// мягкий старт
      P.displayZoneText(ZONE_UPPER,".", CENTER,30, 0,  SCROLL_LEFT,  SCROLL_LEFT);
      P.synchZoneStart(); ////*/   
       ////////////подключаем входы от машины ////////////////////////////////
      digitalWrite(4,HIGH);  // тормоз// 
      digitalWrite(5,HIGH);  // назад // ВКЛ ВНУТРЕНИЕ РЕЗИСТОРЫ НА +5в
      digitalWrite(6,HIGH);  // поворот право
      digitalWrite(7,HIGH);  // поворот лево
      irrecv.enableIRIn();// запускаем прием ифк
      zumer();  // делаем пи-пи :) 
        }
//////////////////////////////////////////////////////////////////////////////////////////////////////////     
void loop(void)    { 
    int8_t  speed = skorost;  P.setSpeed(speed);///меняем скорость
     if (P.displayAnimate())  { 
             switch  (opros) { 
         case 1: mytext=22;   i=3;y=3; skorost=25;break;  //стрелки вправо
         case 2: mytext=23;   i=2;y=2; skorost=25;break;  //стрелки влево
         case 3: mytext=7;    i=0;y=1;  break;  // спасибо 
         case 4: mytext=24;   i=0;y=4; skorost=40; break;  // назад
         case 5: mytext=distanc;i=0;y=1; break;///пульт /строка статична
         case 7: mytext=21;     i=0;y=1;tormaz++;break; // стоп 
         case 8: mytext=distanc;i=2;y=2;skorost=30;break; ///  бегущая строка влево.
         case 9: mytext=distanc;i=0;y=4;skorost=40;break; ///  эфект шторки
          } 
        mytext = mytext % ARRAY_SIZE(msg);// возвращает целое число от деления
    P.displayZoneText(ZONE_LOWER, msg[mytext], CENTER,P.getSpeed(), 0, efkt[i], efkt[y]);
    P.displayZoneText(ZONE_UPPER, msg[mytext], CENTER,P.getSpeed(), 0, efkt[i], efkt[y]);
     P.synchZoneStart();
     }   //*/
  /////////////////////////////////////////////////////////////////////////////////////////
  if (PIND<240)  ///опрашиваем порт. если,что то нажато. проверяем,что 
            ////стоп ///////////////////////////////////////
     { if(tormaz==0&&digitalRead(4)==LOW)
          {tormaz=1;skorost=0;opros=7;}    //нажат стоп//
       if (tormaz>=100&&digitalRead(4)==LOW)/// вместо паузы.// паузы нельзя в этой бибиотеке MD_Parola
          { P.setInvert(!P.getInvert());tormaz=1;}
       if(digitalRead(4)==HIGH && tormaz>=1)  /// отпущен тормоз
          {sbros();}  /// */
          ///////нажат поворот вправо///////////////////////////////////////////////////////  
       if(pravo==0&&levo==0&&tormaz==0&&nazad==0&&avariik==0&&digitalRead(6)==LOW&&digitalRead(7)==HIGH)// 
         {skorost=0; opros=1;pravo=1;}  // СТРЕЛКИ бегут вправо
        if(pravo==1&&digitalRead(6)==HIGH)  // выкл поворот вправо
         {sbros();}  // сбрасываем */
         ////////нажат поворот влево////////////////////////////////////////////////////////
       if(pravo==0&&levo==0&&tormaz==0&&nazad==0&&avariik==0&&digitalRead(6)==HIGH&&digitalRead(7)==LOW)// вкл поворот влево
         {skorost=0; opros=2;levo=1;}  // СТРЕЛКИ бегут влево
       if(levo==1&&digitalRead(7)==HIGH)  // выкл поворот вправо
         {sbros();}  // сбрасываем */
         /////аварийка/////////////////////////////// //////////////////////////////////////      
       if(pravo==0&&levo==0&&avariik==0&&tormaz==0&&nazad==0&&digitalRead(6)==LOW&&digitalRead(7)==LOW&&spasibo==0) //2поворота
         {skorost=0; opros=3;avariik=1;}  // аварийка // спасибо
       if(avariik==1&&tormaz==0&& nazad==0&&digitalRead(6)==HIGH&&digitalRead(7)==HIGH) // выкл  аварийка
          {sbros();}  // сбрасываем */
        /////////////назад ///////////////////////////////////////////////////////////////////    
       if(nazad==0&&tormaz==0&&digitalRead(5)==LOW)
         {skorost=0;opros=4;nazad=1;}    //нажато назад
       if(nazad==1&&digitalRead(5)==HIGH )
         {sbros();}    //отпустили назад*/
       }
  else if (tormaz==0&&pravo==0&&levo==0&&avariik==0&&nazad==0) // если ничего не нажато
        { if (rejim==0)             { opros=5;}    /// выводим фразу от пульта
          if (rejim==2 && mytext<24){ distanc++; rejim=0;}      //  фраза +  ///от пульта
          if (rejim==3 && mytext>0 ){ distanc--; rejim=0;}      //  фраза -  /// от пульта
          if (rejim==4 )            { opros=8;}      //  строка бежит
          if (rejim==5 )            { opros=9;}    /// эфект шторки
           } ////*/
  else {sbros();}  /// сбрасываем флаги
 
 if (sobutie>=6) { rejim=0;sobutie=0;distanc=20;} // 
         // если было 6 любых событий(торм, повор,..), переходим на стартовое слово.
         // может некогда, может забыли, а там "слепиш" осталось например.
   ///////////принимаем команды ифк////////////////////////////////////////////////// 
  if ( irrecv.decode( &results )) // если данные ифк пришли// запоминаем что
          { switch ( results.value) { //зумер останавливает бегущ строку,это зло, но не критично(господи прими за эфект ;)
        case 0xFDB04F:distanc=0;sobutie=0;zumer();break; //0 кнопки пульта
        case 0xFD00FF:distanc=1;sobutie=0;zumer();break; //1//
        case 0xFD807F:distanc=2;sobutie=0;zumer();break; //2//делаем пи пи. куда надо
        case 0xFD40BF:distanc=3;sobutie=0;zumer();break; //3//
        case 0xFD20DF:distanc=4;sobutie=0;zumer();break; //4//
        case 0xFDA05F:distanc=5;sobutie=0;zumer();break; //5//
        case 0xFD609F:distanc=6;sobutie=0;zumer();break; //6//
        case 0xFD10EF:distanc=7;sobutie=0;zumer();break; //7//
        case 0xFD906F:distanc=8;sobutie=0;zumer();break; //8//
        case 0xFD50AF:distanc=9;sobutie=0;zumer();break; //9//
        case 0xFDA857:distanc=20;rejim=0;zumer();break; //  ok  // выклюить текущию фразу. вкл стартовую фразу
        case 0xFD8877:rejim=2;sobutie=0;zumer();break; //  /\  // след фраза
        case 0xFD9867:rejim=3;sobutie=0;zumer();break; //  \/  // предыдущ фраза
        case 0xFD28D7:rejim=4;zumer();break; //<//строка бежит
        case 0xFD6897:rejim=5;zumer();break; //>//эфект шторки
        case 0xFD708F:spasibo=1;sbros();zumer();break; //  #  ///нажал " # " и  выкл "спасибо" от аварийки.
        case 0xFD30CF:spasibo=0;zumer();break; //  *  ///нажал " * " и  включил "спасибо" от аварийки.
          } 
          irrecv.resume(); // принимаем следующую команду
          }  
          } 
