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
// #include "RUS60.h"////подключаем кирилицу//широкий шрифт//только большие.
#include "RUS70.h"///подключаем кирилицу// широкий кратный2.//большие и маленькие русские!!!!!!!1
#define MAX_ZONES 2  ///высота 2 девайса
#define ZONE_SIZE 10  // длина 10 девайсов//тут свое пишем//ограничение не знаю
#define  MAX_DEVICES (MAX_ZONES * ZONE_SIZE)
#define ZONE_UPPER  1
#define ZONE_LOWER  0
#define  CLK_PIN   13  ///пины для табло //
#define DATA_PIN  11
#define CS_PIN    10
MD_Parola P = MD_Parola(CS_PIN, MAX_DEVICES);

#include <IRremote.h>////ифк библиотека//
uint8_t RECV_PIN = 8; /// пин подключения приемника ифк
IRrecv irrecv(RECV_PIN); // присваиваем пин приема
decode_results results;//

uint8_t skorst=30;// стартовая скорость//скорость движения//99-медлено 0-быстро
unsigned long taimer1=0, taimer2=0, taimer4=0, taimer5=0, taimer6=0, taimer7=0;  // здесь будет храниться время таймеров
uint8_t tormaz=0,nazad=0,s=0,y=0,pravo=0,speed= 25,levo=0,avariik=0,desytki=0,reklama=0,text_old=0,dlina=0,ctop=0,opros=5,textN =0; ////флаги /
//тут ниже меняем под себя____________________________________________________________________________________________________
uint8_t new_distanc = 1;// если new_distanc не равно distanc, то при старте будет гореть хх сек фраза № (0) 
uint8_t distanc= 0; ///стартовое слово появляется 1 раз после вкл. Потом 21 фразa горит " /!\ "
#define trm 4    //тормоз // присваиваем пины входов от ламп машины
#define nzd 5    //назад  // скетч использует 4,5,6,7  пины. если возьмете дугие пины нужно менять  "PIND<16 "
#define prv 6    //вправо //
#define lev 7    //налево //
#define str_n  9  //количество букв помещающихся на табло///
#define invers 4  // количество инверсий "СТОП".после чего стоп не будет инвертироватся и раздражать соседа сзади
#define time_invers 500   // время через которое СТОП инвертируется//1000млс=1сек//примерно
#define time_sbros  15000  // время отображения фраз с пульта до сброся на  " /!\  "   //
#define time_povorot  800  // время между сигналами поворотов//тут нет конденсаторов как ранее//
#define time_spasibo  10000 //время горения "спасибо"_____


char *msg[] = /// буквы "Ё" нет!Кириллица только большие буквы !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
 {"161 RUS",     //0 //
  "СПОКОЙНО",    //1 //
  "НЕ ДУДИ",//2 //на длинном тексте добавляем впереди пробел, чтоб не сливалось с предыдущ фразой
  "DRIVE2",    //3/ 
  "ИЗВИНИ",     //4//
  " ПОНЯТЬ И ПРОСТИТЬ",    //5// 
  "ПОДОЖДИ",   //6// 
  "СПАСИБО",   //7//сервисное не трогаем !!!!!!!!!!!!!!!!
  "УДАЧИ",     //8//
  "УЛЫБНИСЬ",  //9// 
  "ХХХХХХ",   //10 // 
  " БЫСТРЕЕ НЕ МОГУ",  //11// 
  "S O S",      //12/
  " НУЖНА ПОМОЩЬ",   //13// 
  "БУКСИР",     //14// 
  " ПОДРАСТУ И СТАНУ ДЖИПОМ",        //15// 
  " ТАНКИ ГРЯЗИ НЕ БОЯТСЯ",    //16// 
  "ПЖЛ АИ92",       //17//
  "ХХХХХХ",        //18//
  "ШИНУ R14",      // 19//
  //////////////////////сервисные фразы. не трогаем их//////////////////хотя... :)
  "\x015\x016\x020\x020\x020\x020\x020\x015\x016", //20// 2 знака "/!  /! /// для табло 16х80//выбираем под свое табло (всмысле экран) ;))
//"\x015\x016\x020\x015\x016"                     //20// 2 знака "/!  /! /// для табло 16х64- без пробелов  "\x020"
  "\x015\x016", //21/      ///! 
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
{ analogWrite(9, 50);  //пришла команда ифк- пик-пик//
  unsigned long OldTime=millis();
  while((millis()-OldTime)<30);
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
 while(millis()<1600) {}//если меняем фразы то подбираем время паузы 
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
 unsigned long new_taimer = millis(); // текущее время в миллисекунд
/////////////////////////////////////////////////////////////////////////////
if   (distanc!=new_distanc) /// елсли пришла новая фраза с пульта
{ if (distanc != textN) //увеличиваем скорость до момента появления новой фразы
      {speed=0; taimer6 = new_taimer;P.setInvert(LOW);///быстренько прокручиваем старую фразу.//запускаем таймер 
  if (distanc>21)   /// проверяем пределы номера фраз    
      { distanc=21;}  /// не более 21
  if ( distanc <1)    /// не менее 0
      { distanc=0;}    
    dlina=str_dlin(distanc); ///считаем длину строки
  if   (dlina > str_n) //если фраза не помещается//зависит от длины табло//18=9 букв кирилицы.
       {opros=8;}  // строка бежит
  else {opros=5;} // строка статична
 }
  if (new_taimer - taimer6 >= time_sbros && reklama==0 ) //пауза//если реклама=1, то фраза горит бесонечно
     { distanc =21;new_distanc=distanc;opros=5;desytki=0;sbros();} // поистечении паузы сбрасываем фразу с пульта на знак "/!\ "
 
} ///*/
//////////////////////////////////////////////////////////////////////////////
else if ( PIND<16&&tormaz==0&& pravo==0&& levo==0&& nazad==0&& avariik==0 ) // если ничего не нажато и флаги сброшены
    { distanc =21;desytki=0;opros=5; // сбрасываем фразу  на знак "/!\ "  ////
    } ///*/
/// если, что то нажато, проверяем что?//по приоритету после первой истины, далее не проверяем (else  if)//
   ////стоп ///////////////////////////////////////
 else if (tormaz==0&&digitalRead(trm)==HIGH)///если нажат тормоз первый раз
         {tormaz=1;speed=0;opros=7;textN =22;taimer1 = new_taimer; ctop=0;}//запускаем таймер инверсии
  else if (tormaz==1)  //если прошло хх инверсий, то больше не раздражаем соседа сзади,
        { if (digitalRead(trm)==HIGH&& ctop>=invers)  //если прошло хх инверсий, то больше не раздражаем соседа сзади,
            {P.setInvert(LOW);} // Стоп не инвертируется далее
         if (digitalRead(trm)==HIGH&&(new_taimer - taimer1 >= time_invers))  //если прошло хх сек, а тормоз нажат,то
            {taimer1 = new_taimer; P.setInvert(!P.getInvert()); ctop++;} // инвертируем СТОП
         if (digitalRead(trm)==LOW)  /// отпущен тормоз, сброс
            {sbros();} /// 
       }
  /////////////назад ///////////////////////////  
 else if (nazad==0&&digitalRead(nzd)==HIGH)//нажато назад
          {speed=0;nazad=1;opros=4;textN =25;} //увеличиваем скорость(как всегда),для быстрой смены фразы//    
 else if (nazad==1)
        {if (digitalRead(nzd)==LOW ) {sbros();} //отпустили назад, сброс 
        } 
   /////аварийка/////////////////////////////////  
 else if (avariik==0&&digitalRead(prv)==HIGH&&digitalRead(lev)==HIGH) //2поворота
         { opros=3; avariik=1;textN =7; taimer4 = new_taimer; taimer5 = new_taimer;}  // аварийка // спасибо
 else if (avariik==1) //спасибо горит ХХ сек даже если было однократное моргание //time_sbros 
        {if (digitalRead(prv)==HIGH&&digitalRead(lev)==HIGH) //2поворота снова моргнули
            {taimer4 = new_taimer;}  //перевзводим таймер между морганиями аварийки 
         if (new_taimer - taimer5 >= time_spasibo) //выкл СПАСИБО ели прошло хх сек
            {opros=9;textN =20;} // поистечении паузы сбрасываем фразу с пульта на 2 знака   "/!  /!  ///
         if (opros==9&&(new_taimer - taimer4 >= time_povorot)) //больше не моргает аварийка
            {sbros();}  //выкл спасибо до автосброса по таймеру5
        }
    //------------нажат поворот влево--------------------------------------------
else if (levo==0&&avariik ==0&&digitalRead(prv)==LOW&&digitalRead(lev)==HIGH)// вкл поворот влево
         { opros=2;levo=1;textN =24;taimer2 = new_taimer;
         }  // СТРЕЛКИ бегут влево
else if (levo==1)
        { if (digitalRead(prv)==LOW&&digitalRead(lev)==HIGH)// вкл поворот влево
             {taimer2 = new_taimer;}  // СТРЕЛКИ бегут влево//перевзвдим  таймер2 
          if (digitalRead(lev)==LOW&&(new_taimer - taimer2 >= time_povorot))  // выкл поворот вправо
             {sbros();} // сбрасываем 
        }
   //-------------нажат поворот вправо------------------------------------------
else  if (pravo==0&&avariik ==0&&digitalRead(prv)==HIGH&&digitalRead(lev)==LOW)// 
         {opros=1;pravo=1;textN =23;taimer7 = new_taimer;}  // СТРЕЛКИ бегут вправо
else if (pravo==1)
       {if (digitalRead(prv)==HIGH&&digitalRead(lev)==LOW)// 
           {taimer7 = new_taimer;}  // СТРЕЛКИ бегут вправо//певзводим таймер3 
        if (pravo==1&&digitalRead(prv)==LOW&&(new_taimer - taimer7 >= time_povorot))  // выкл поворот вправо
           {sbros();} // сбрасываем 
       }
  //======================================================================================
if (textN != text_old) {speed=0;} 
if (P.displayAnimate())  /// выводим результат на табло//анимация//выбор эфекта//норнализуем скорость//
   {    switch  (opros) /// 
      {  case 1:        s=3;y=3; speed=skorst; break;  //стрелки вправо
         case 2:        s=2;y=2; speed=skorst; break;  //стрелки влево
         case 3:        s=0;y=1; speed=skorst; break;  // спасибо 
         case 4:        s=0;y=4; speed=skorst+15; break;  // назад
         case 5:textN=distanc; s=0;y=1; speed=skorst; break;///пульт /строка статична
         case 7:        s=0;y=1; speed=skorst; break; // стоп 
         case 8:textN=distanc; s=2;y=2; speed=skorst; break; ///  бегущая строка влево.
         case 9:        s=0;y=4; speed=skorst; break; /// 2 ЗНАКА, вкл после 25сек анимации спасибо (аварийка)
        default:textN=21;s=0;y=1; speed=skorst;//если невыполнилось ни одно условие
      } 
      P.displayZoneText(ZONE_LOWER, msg[textN], PA_CENTER,P.getSpeed(), 0, efkt[s], efkt[y]);
      P.displayZoneText(ZONE_UPPER, msg[textN], PA_CENTER,P.getSpeed(), 0, efkt[s], efkt[y]);
      P.synchZoneStart(); text_old=textN;
   } 
} ////////конец//а кто читал молодец
