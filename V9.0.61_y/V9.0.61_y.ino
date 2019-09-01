///https://github.com/z3t0/Arduino-IRremote
///https://github.com/MajicDesigns/MD_MAX72XX
///https://github.com/MajicDesigns/MD_Parola
///https://www.drive2.ru/users/borzov161/
//https://www.youtube.com/watch?v=mlo7Or2L6I4
// табло ///   10-9-8-7-6-  ///подключено
///////////   -5 -4-3-2-1
//пины на лампы, по упрощенной схеме.только диоды 
//  лампа  -|<|- ардуино
//
#include <MD_Parola.h>  //библиотека 2 высоты
#include <MD_MAX72xx.h> //
#include <SPI.h>
///выбираем для себя 1 какой то шрифт///////// или или//
///#include "RUS43.h"///подключаем кирилицу// узкий шрифт//только большие
// #include "RUS53.h"////подключаем кирилицу//широкий шрифт//только большие.
 #include "RUS60.h"////подключаем кирилицу//широкий шрифт//только большие.
//#include "RUS70.h"///подключаем кирилицу// широкий кратный2.//большие и маленькие русские!!!!!!!1
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

uint8_t speed= 25; // стартовая скорость//скорость движения//99-медлено 0-быстро
unsigned long old_millis1=0, old_millis2=0, old_millis4=0, old_millis5=0, old_millis6=0, old_millis7=0;  // здесь будет храниться время таймеров
uint8_t tormaz=0,nazad=0,i=0,y=0,pravo=0,levo=0,avariik=0,desytki=0,reklama=0,flag=0,dlina=0,ctop=0,opros=5,mytext =0; ////флаги /
//тут ниже меняем под себя____________________________________________________________________________________________________
uint8_t new_distanc = 0;// если new_distanc не равно distanc, то при старте будет гореть хх сек фраза № (0) 
uint8_t distanc= 0; ///стартовое слово появляется 1 раз после вкл. Потом 21 фразa горит " /!\ "
uint8_t trm=4;    //тормоз // присваиваем пины входов от ламп машины
uint8_t nzd=5;    //назад  // скетч использует 4,5,6,7  пины. если возьмете дугие пины нужно менять  "PIND>240 "
uint8_t prv=6;    //вправо //
uint8_t lev=7;    //налево //
uint8_t str_n = 9; //количество букв помещающихся на табло///

uint8_t invers=6; // количество инверсий "СТОП".после чего стоп не будет инвертироватся и раздражать соседа сзади
unsigned long time_invers= 500;   // время через которое СТОП инвертируется//1000млс=1сек//примерно

unsigned long time_sbros = 15000;  // время отображения фраз с пульта до сброся на  " /!\  "   //
unsigned long time_povorot = 800;  // время между сигналами поворотов//тут нет конденсаторов как ранее//
 //____________________________________________________________________________________________________________________________

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0])) ///пересчитываем массив
char *msg[] = /// буквы "Ё" нет!Кириллица только большие буквы !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 {"РОСТОВ",     //0 //
  " НУ И ПРОБКА",    //1 //
  " НУ И ПОГОДКА",//2 //на длинном тексте добавляем впереди пробел, чтоб не сливалось с предыдущ фразой
  " ХОРОШО СТОИМ",    //3/ 
  "ИЗВИНИ",     //4//
  " ПОНЯТЬ И ПРОСТИТЬ",    //5// 
  "ПОДОЖДИ",   //6// 
  "СПАСИБО",   //7//сервисное не трогаем !!!!!!!!!!!!!!!!
  "УДАЧИ",     //8//
  "УЛЫБНИСЬ",  //9// 
  "НЕ ДУДИ",   //10 // 
  " БЫСТРЕЕ НЕ МОГУ",  //11// 
  "S O S",      //12/
  " НУЖНА ПОМОЩЬ",   //13// 
  "БУКСИР",     //14// 
  "ПРИКУРИТЬ",        //15// 
  " ТАНКИ ГРЯЗИ НЕ БОЯТСЯ",    //16// 
  "ПЖЛ АИ92",       //17//
  "DRIVE2",        //18//
  "ШИНУ R14",      // 19//
  //////////////////////сервисные фразы. не трогаем их//////////////////хотя... :)
  "\x015\x016\x020\x020\x020\x020\x020\x015\x016", //20// 2 знака "/!  /! /// для табло 16х80//выбираем под свое табло (всмысле экран) ;))
//"\x015\x016\x020\x015\x016"                     //20// 2 знака "/!  /! /// для табло 16х64- без пробелов  "\x020"
  "\x015\x016", //21/      /// 
  "\x019\x020\x01a\x020\x01b\x020\x01c",//22//CТОП//ЖИРНЫМ ШРИФТОМ//для табло 16х80//выбираем под свое табло (всмысле экран) ;))
//"\x019\x01a\x01b\x01c",               //22//CТОП//ЖИРНЫМ ШРИФТОМ// "\x020" - это пробел.можно убрать для 16х64 
  ">>> >> >",             //23//>>>  //поворот вправо
  "< << <<<",             //24// <<<  ///поворот влево
  "< НАЗАД >"};          //25// назад//для 16х64 стрелки можно убрать
  //======================================================================================
 char efkt[] =  /// эфекты из библиотеки////WIPE,  WIPE_CURSOR,  OPENING, OPENING_CURSOR, ....
{  PA_PRINT,       //0// появдяется в центре
   PA_NO_EFFECT,    //1// 
   PA_SCROLL_LEFT,  //2// бежит налево
   PA_SCROLL_RIGHT, //3// бежит направо
   PA_CLOSING       //4// шторки
  } ;
/////////////считаем длину строки /////////////////////////
int8_t str_dlin(int8_t ind){
byte n=0,count=0;
String Mystr = msg[ind];
while(Mystr.charAt(n) != '\0')
{if (Mystr.charAt(n) < ' ') n+=2;
else n++;count++;
} return count;
}
/////////////////////////////////////////////////////////////
 void zumer() //контроль приема звуком. т.к. не видим,что там сзади авто. 
{ analogWrite(9, 50); delay(30);  //пришла команда ифк- пик-пик//
  analogWrite(9, 0); // выключаем звук///*/
}
void sbros() {
 P.setInvert(LOW);tormaz=0;nazad=0;levo=0;pravo=0;speed=25;avariik =0; // сбрасываем флаги. ждем новых команд
 }
 
void priem()  ///////////принимаем команды ифк////////предварительно кнопки своего пульта нужно считать/////////// 
{ if ( irrecv.decode( &results )) // если данные ифк пришли// запоминаем что
   {  switch ( results.value)  //зумер останавливает бегущ строку,это зло, но не критично(господи прими за эфект ;)
      { case 0xFF4AB5:distanc = desytki ;zumer();break; //0 кнопки пульта.
        case 0xFF6897:distanc=1+desytki ;zumer();break; //1// с1 до 9 сбраываем число анимаций (прокрутки фраз), 
        case 0xFF9867:distanc=2+desytki ;zumer();break; //2// после новой команды с пульта.
        case 0xFFB04F:distanc=3+desytki ;zumer();break; //3// делаем пи пи. куда надо.
        case 0xFF30CF:distanc=4+desytki ;zumer();break; //4//
        case 0xFF18E7:distanc=5+desytki ;zumer();break; //5//
        case 0xFF7A85:distanc=6+desytki ;zumer();break; //6//
        case 0xFF10EF:distanc=7+desytki ;zumer();break; //7//
        case 0xFF38C7:distanc=8+desytki ;zumer();break; //8//
        case 0xFF5AA5:distanc=9+desytki ;zumer();break; //9//
        case 0xFF02FD:opros=9;desytki=0;zumer();break; //ok// выклюить текущию фразу. вкл стартовую фразу//сборс десяток.
        case 0xFF629D:distanc++;zumer();break; //  /  // след фраза.
        case 0xFFA857:distanc--;zumer();break; //  /  // предыдущ фраза.
        case 0xFF22DD:reklama=0;zumer();break; //<//фразы сами уходят.
        case 0xFFC23D:reklama=1;zumer();break; //>//фразы горят всегда.
        case 0xFF52AD:desytki=10;zumer();break; //  #  ///двухзначные 10-19 фразы.
        case 0xFF42BD:desytki= 0;zumer();break; //  *  /// 0-9 фразы.
      } 
     irrecv.resume(); // принимаем следующую команду
   }
}
 //======================================================================================
 void setup(void) ////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{ /// блок старта (инициализации). самый ПРОБЛЕМАТИЧНЫЙ участок.!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
   delay(1000); // копим энергию на "ХОЛДНЫЙ" старт -когда все кондеры разряжены (был долгий перерыв) 
   Serial.begin(57600); //некоторые ардуино стартуют только с этой строкой. другие наоборот с ней не стартуют
  // P.begin();  /// гасим самозагоревшиеся светодиоды - от пульсаций БП//инициализация
 // delay(200); // сново копим энеергию на пуск табло
  
 P.begin(MAX_ZONES);/// инициализируем дисплей на 2 зоны
//  P.setIntensity(16) ; //яркость//мах =15//начиная с 12 почти не меняется уже(хоть сколько пиши).
  P.setZone(ZONE_LOWER, 0, ZONE_SIZE - 1);
  P.setFont(ZONE_LOWER, BigFontLower);
  P.setZone(ZONE_UPPER, ZONE_SIZE, MAX_DEVICES-1);
  P.setFont(ZONE_UPPER, BigFontUpper);
 // P.setCharSpacing(P.getCharSpacing()*2);//двойной высоты->двойной интервал//для кирилицы не ставивим, там уже2
 ////////////подключаем входы от машины //////////////////////////////
      digitalWrite(trm,HIGH);  // тормоз// 
      digitalWrite(nzd,HIGH);  // назад // ВКЛ ВНУТРЕНИЕ РЕЗИСТОРЫ НА +5в
      digitalWrite(prv,HIGH);  // поворот право
      digitalWrite(lev,HIGH);  // поворот лево
      irrecv.enableIRIn();   // запускаем прием ифк
    zumer();  // делаем пи-пи :) хозяин я заработал
     }
 //======================================================================================
void loop(void)
{   P.setSpeed(speed);///меняем скорость
  priem(); ///принимаем команду с пульта
 unsigned long new_millis = millis(); // текущее время в миллисекунд
/////////////////////////////////////////////////////////////////////////////
if   (distanc!=new_distanc) /// елсли пришла новая фраза с пульта
{ if (distanc!=mytext) //увеличиваем скорость до момента появления новой фразы
     {speed=0;old_millis6 = new_millis;P.setInvert(LOW);}///быстренько прокручиваем старую фразу.//запускаем таймер 
  if (distanc>21)   /// проверяем пределы номера фраз    
      { distanc=21;}  /// не более 21
  if ( distanc <1)    /// не менее 0
      { distanc=0;}    
    dlina=str_dlin(distanc); ///считаем длину строки
  if   (dlina > str_n) //если фраза не помещается//зависит от длины табло//18=9 букв кирилицы.
       {opros=8;}  // строка бежит
  else {opros=5;} // строка статична
  if (new_millis - old_millis6 >= time_sbros && reklama==0 ) //пауза//если реклама=1, то фраза горит бесонечно
     { distanc =21;new_distanc=distanc;opros=5;desytki=0;sbros();} // поистечении паузы сбрасываем фразу с пульта на знак "/!\ "
} ///*/
//////////////////////////////////////////////////////////////////////////////
else if ( PIND<16&&tormaz==0&& pravo==0&& levo==0&& nazad==0&& avariik==0 ) // если ничего не нажато и флаги сброшены
    { distanc =21;desytki=0;opros=5; // сбрасываем фразу  на знак "/!\ "  ////
    } ///*/
/// если, что то нажато, проверяем что?//по приоритету после первой истины, далее не проверяем (else  if)//
   ////стоп ///////////////////////////////////////
 else if (tormaz==0&&digitalRead(trm)==HIGH)///если нажат тормоз первый раз
         {tormaz=1;speed=0;opros=7;old_millis1 = new_millis; ctop=0;}//запускаем таймер инверсии
 else if (tormaz==1&&digitalRead(trm)==HIGH&& ctop>=invers)  //если прошло хх инверсий, то больше не раздражаем соседа сзади,
         {old_millis1 = new_millis; P.setInvert(LOW);} // Стоп не инвертируется далее
 else if (tormaz==1&&digitalRead(trm)==HIGH&&(new_millis - old_millis1 >= time_invers))  //если прошло хх сек, а тормоз нажат,то
         {old_millis1 = new_millis; P.setInvert(!P.getInvert()); ctop++;} // инвертируем СТОП*/
 else if (tormaz==1&&digitalRead(trm)==LOW)  /// отпущен тормоз, сброс
         {sbros(); } /// */
 else if (tormaz==1)
         {  }  /// не проверяем ниже условия, пока нажат тормоз//продолжается инверсия
     /////////////назад ///////////////////////////  
 else if (nazad==0&&digitalRead(nzd)==HIGH)//нажато назад
         {speed=0;nazad=1;opros=4;} //увеличиваем скорость(как всегда),для быстрой смены фразы//    
 else if (nazad==1&&digitalRead(nzd)==LOW )
         {sbros();}    //отпустили назад, сброс */
 else if (nazad==1)
         {  }  /// не проверяем ниже условия, пока нажато назад
   /////аварийка/////////////////////////////////  
 else if (avariik==0&&digitalRead(prv)==HIGH&&digitalRead(lev)==HIGH) //2поворота
         { opros=3; avariik=1; old_millis4 = new_millis; old_millis5 = new_millis;}  // аварийка // спасибо
 else if (avariik==1&&digitalRead(prv)==HIGH&&digitalRead(lev)==HIGH) //2поворота
       {  old_millis4 = new_millis;  ///спасибо//перевзводим таймер между морганиями аварийки 
        if (avariik ==1 &&(new_millis - old_millis5 >= time_sbros)) //выкл СПАСИБО ели прошло хх сек
           {opros=9;;desytki=0;} // поистечении паузы сбрасываем фразу с пульта на знаки   "/!\ /!\ ///
       }
 else if (avariik ==1&&(new_millis - old_millis4 >= time_povorot)) //больше не моргает аварийка
         {sbros();}  //выкл спасибо до автосброса по таймеру5
    ///////////нажат поворот влево////////////////////
 else if (levo==0&&avariik ==0&&digitalRead(prv)==LOW&&digitalRead(lev)==HIGH)// вкл поворот влево
         { speed=0; opros=2;levo=1;old_millis2 = new_millis;}  // СТРЕЛКИ бегут влево
 else if (levo==1&&digitalRead(prv)==LOW&&digitalRead(lev)==HIGH)// вкл поворот влево
         {old_millis2 = new_millis;}  // СТРЕЛКИ бегут влево//перевзвдим  таймер2 
 else if (levo==1&&digitalRead(lev)==LOW&&(new_millis - old_millis2 >= time_povorot))  // выкл поворот вправо
         {sbros(); } // сбрасываем */
   ///////нажат поворот вправо/////////////////////
 else  if (pravo==0&&avariik ==0&&digitalRead(prv)==HIGH&&digitalRead(lev)==LOW)// 
          { speed=0; opros=1;pravo=1;old_millis7 = new_millis;}  // СТРЕЛКИ бегут вправо
 else if (pravo==1&&digitalRead(prv)==HIGH&&digitalRead(lev)==LOW)// 
          {old_millis7 = new_millis;}  // СТРЕЛКИ бегут вправо//певзводим таймер3 
 else if (pravo==1&&digitalRead(prv)==LOW&&(new_millis - old_millis7 >= time_povorot))  // выкл поворот вправо
          {sbros(); } // сбрасываем */
  //======================================================================================
 if (P.displayAnimate())  /// выводим результат на табло//анимация//выбор эфекта//норнализуем скорость//
   {    switch  (opros) /// 
      {  case 1: mytext=23;      i=3;y=3; speed=30; break;  //стрелки вправо
         case 2: mytext=24;      i=2;y=2; speed=30; break;  //стрелки влево
         case 3: mytext=7;       i=0;y=1; speed=30; break;  // спасибо 
         case 4: mytext=25;      i=0;y=4; speed=40; break;  // назад
         case 5: mytext=distanc; i=0;y=1; speed=30; break;///пульт /строка статична
         case 7: mytext=22;      i=0;y=1; speed=30; break; // стоп 
         case 8: mytext=distanc; i=2;y=2; speed=30; break; ///  бегущая строка влево.
         case 9: mytext=20;      i=0;y=4; speed=30; break; /// 2 ЗНАКА, вкл после 25сек анимации спасибо (аварийка)
        default: mytext=21;      i=0;y=1; speed=30;//если невыполнилось ни одно условие
      } 
    //  mytext = mytext % ARRAY_SIZE(msg);// возвращает целое число
      P.displayZoneText(ZONE_LOWER, msg[mytext], PA_CENTER,P.getSpeed(), 0, efkt[i], efkt[y]);
      P.displayZoneText(ZONE_UPPER, msg[mytext], PA_CENTER,P.getSpeed(), 0, efkt[i], efkt[y]);
      P.synchZoneStart(); //синхро старт 2 зон
   } 
} ////////конец//а кто читал молодец
