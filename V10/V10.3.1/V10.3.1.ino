////////////////////////////////////////////
//  кошелек WebMoney. на развитие проекта.//
//  Z421041748563                         //
//  R332084035126                         //
////////////////////////////////////////////
///https://github.com/MajicDesigns
///https://www.drive2.ru/users/borzov161/
// табло ///   10-9-8-7-6-  ///подключено
///////////   -5 -4-3-2-1
//пины на лампы, по упрощенной схеме.только диоды 
//  лампа  -|<|- ардуино
// СКЕТЧ ПОД БИБЛИОТЕКИ 3.0 . СЕНТЯБРЬ2018. 
// БИБЛИОТЕКА СТАЛА ОГРОМНОЙ 26КБ ИЗ 30КБ ЗАНЯТО ТОЛЬКО ЕЮ
// УРЕЗАЛ ПЕРЕДАЧУ НАСТРОЕК С ТЕЛЕФОНА.
// НАДЕЮСЬ АВТОР БИБЛИОТЕК УРЕЖЕТ БИБЛИОТЕКУ
// 
// для творчества оставляет 
#include <EEPROM.h> // подключаем библиотеку EEPROM 
#include <MD_Parola.h>  //библиотека 2 высоты
#include <MD_MAX72xx.h> //
#include <SPI.h>
#include "RUS70.h"///подключаем кирилицу// широкий кратный2.//большие и маленькие русские!!!!!!!
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW   /// было PAROLA_HW// а наш 16
#define MAX_ZONES 2  //2 высоты
#define ZONE_SIZE 10 ///количество девайсов в ряду
#define  MAX_DEVICES (MAX_ZONES * ZONE_SIZE)
#define ZONE_UPPER  1
#define ZONE_LOWER  0
#define  CLK_PIN   13  //выходы зашищаем резисторами по 100  ом
#define DATA_PIN  11  //выходы зашищаем резисторами по 100  ом
#define CS_PIN    10  //выходы зашищаем резисторами по 100  ом
MD_Parola P = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);
//#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0])) ///пересчитываем массив

uint8_t speed= 25; // стартовая скорость//скорость движения//99-медлено 0-быстро
unsigned long taimer1=0, taimer2=0, taimer4=0, taimer5=0, taimer6=0, taimer7=0,time_sbros =0;  // здесь будет храниться время таймеров
uint8_t s=0,y=0,eeprom=0,text_old=3,opros=0,textN =3,len=25,ctop=0; ////флаги //
boolean tormaz=0,nazad=0,pravo=0,flag_tel=0, //флаги //
        levo=0,avariik=0; ////флаги //
//____________________________________________________________________________________________________
#define trm 4    //тормоз // присваиваем пины входов от ламп машины
#define nzd 5    //назад  // скетч использует 4,5,6,7  пины. если возьмете дугие пины нужно менять  "PIND<16 "
#define prv 6    //вправо //
#define lev 7    //налево //

uint8_t zadan_dlina = 9; //количество букв помещающихся на табло//
String Str_telf,Str_old; ///для хранения фраз телефона
//--------------------------------------------------------------------------------------------------------
#define time_invers 500   // время через которое СТОП инвертируется//1000млс=1сек//примерно
#define time_povorot  800 //время межу морганиями поворотов (негорит)
#define time_spasibo  10000 //время горения "спасибо"
unsigned long vremya_long= 15;  // время ~ (сек)отображения фраз с пульта до сброся на  " /!  "   //
uint8_t adrees=0;//адрес ардуино//29184
uint8_t skorst=25;// скорость(1-100)//0 быстро 99-медлено//так в PAROLE
uint8_t jarkost=9;//яркость(1-15)
uint8_t invers=6;// инверсия
uint8_t cikl=1;// цикличность//1нет//2 да
uint8_t vremya=15;//время фраз
uint8_t coxpan=1;//сохранить еером//1нет//2 да
uint8_t iffekt=1;// бежит//1-нет//2- да
uint8_t shtorki=1;//эфект шторки
uint8_t sped_bufer=70; //0 медлено, 99- быстро//так передаем телефоном, потом инвертируем под паролу.
 //_______________________________________________________________________________________________________

char *msg_avto[] = /// буквы "Ё" нет!
 {/////////сервисные фразы. не трогаем их//////////////////хотя... :)
  "ПОЕХАЛИ", //0// фраза -можно поставить на старт
  "СПАСИБО",  //1//
  "\x015\x016\x020\x020\x020\x020\x020\x015\x016", //2// 2 знака "/!  /! /// для табло 16х80//выбираем под свое 
//"\x015\x016\x020\x015\x016"                      //2// 2 знака "/!  /! /// для табло 16х64- без пробелов "\x020" 
  "\x015\x016", //3/// знак /! /////
  "\x019\x020\x01a\x020\x01b\x020\x01c",//4//CТОП//ЖИРНЫМ ШРИФТОМ//для табло 16х80//выбираем под свое табло 
//"\x019\x01a\x01b\x01c",               //4//CТОП//ЖИРНЫМ ШРИФТОМ// "\x020" - это пробел.можно убрать для 16х64 
  "!    >>>>",             //5//поворот вправо//используем эфект шторки (половину его)
  "<<<<    !",             //6//поворот влево//используем эфект шторки (половину его)
  "< НАЗАД >"};           //7// назад//для 16х64 стрелки можно убрать
  //======================================================================================
 char *msg_tel; ///резервируем память под массив фраз с тел
 
 char efkt[] =  /// эфекты из библиотеки////WIPE,  WIPE_CURSOR,  OPENING, OPENING_CURSOR, ....
{  PA_PRINT,       //0// появдяется в центре
   PA_NO_EFFECT,   //1// 
   PA_SCROLL_LEFT, //2// бежит налево
   PA_CLOSING     //3// шторки
 } ;
 //=================считаем длину строки=====================================================
uint8_t dlina_telef(){ // тут ничего не трогаем
byte n=0,count=0;
while(Str_telf.charAt(n) != '\0')
{if (Str_telf.charAt(n) < ' ') n+=2;
else n++;count++;
} return count;
}

void sbros() 
{ P.setInvert(LOW);tormaz=0;nazad=0;levo=0;pravo=0;avariik =0; // сбрасываем флаги. ждем новых команд
} 

///=================setup=============///==================///=====================///============
 void setup() ////!!!!!!!!!!!!!!!!!!!!!!!!!!!!
{ /// блок старта (инициализации). самый ПРОБЛЕМАТИЧНЫЙ участок.!!!!!!!!!!!
  while(millis()<700) {}// оставвил на случай проблемы старта паузу
   P.begin(MAX_ZONES); // инициализация 2 зоны//вот в этой команде косяк (был)
  P.setZone(ZONE_LOWER, 0, ZONE_SIZE - 1);
  P.setFont(ZONE_LOWER, BigFontLower);
  P.setZone(ZONE_UPPER, ZONE_SIZE, MAX_DEVICES-1);
  P.setFont(ZONE_UPPER, BigFontUpper);
 // P.setCharSpacing(P.getCharSpacing()*2);//двойной высоты->двойной интервал//для кирилицы не ставивим, там уже2
  //---------------подключаем входы от машины --------------------------------------------
      digitalWrite(trm,HIGH);  // тормоз// 
      digitalWrite(nzd,HIGH);  // назад // ВКЛ ВНУТРЕНИЕ РЕЗИСТОРЫ НА +5в
      digitalWrite(prv,HIGH);  // поворот право
      digitalWrite(lev,HIGH);  // поворот лево///*/
//digitalWrite(trm,LOW); digitalWrite(nzd,LOW); digitalWrite(prv,LOW);digitalWrite(lev,LOW); //зто для режима тест,имитируем нули
 Serial.begin(9600);  /// для блютуз СН-06
// Serial.begin(38400);// для блютуз СН-05
  //------------чтение сохранненых переменных их энергонезависимой памяти ----------------------- 
if (EEPROM.read(61)==177){ // 61- ячейка как флаг,пока мы ни разу не сохранялись,то настройки по умолчанию (не из памяти)
 skorst=EEPROM.read(31);jarkost=EEPROM.read(32); invers=EEPROM.read(33); 
 vremya=EEPROM.read(34); iffekt=EEPROM.read(35);shtorki=EEPROM.read(36);
 skorst=map(sped_bufer, 10, 99, 60, 5); //ивертирует диапазон под паролу
}
P.setIntensity(jarkost) ; //яркость//
}

 //=================loop===============================================================
void loop() 
{  P.setSpeed(speed);///меняем скорость
  // P.setIntensity(jarkost) ; //яркость//
   unsigned long new_taimer = millis(); //обновляем время в миллисекунд.(для таймеров).
   vremya_long=vremya; //меняем переменные для коррекного умножения ниже
   time_sbros=vremya_long*1000; /// переводим сек в млсек
 //==================== прием по блютуз фраз и команд =========================================
if (Serial.available() !=0)  /// если данные пришли по блютуз
{   Str_telf = Serial.readString();
    flag_tel=1; //опрокидываем флаг
    taimer6 = new_taimer;//запускаем таймер горения фразы
}
 //----------------------обработка команд с телефона----------------------------------------------------
if  (flag_tel) // если фраза с телефона
{  
  String str = Str_telf; // создаем объект
     msg_tel = str.c_str(); len= Str_telf.length() ;//преобразовываем массив//считаем длину
 /*    sscanf(msg_tel, "%d,", &adrees);///берем 5 символов из фразы
     
 if (adrees == 29184 && len>21&&len<26) //если пришла команда, разгбераем её на части отделенные запятой
     {
      sscanf(msg_tel, "%d,%d,%d,%d,%d,%d,%d,%d,%d", 
     &adrees,&sped_bufer,&jarkost,&invers,&cikl, &vremya, &coxpan, &iffekt, &shtorki);// переменные
     //--1-------2---------3--------4-------5-------6---------7-------8--------9------  № п/п
     //29184----70---------9--------6-------1-------15--------2-------1--------1------  значение
    //29184,70,9,6,1,15,2,1,1  // пример для записи EEPROM через порт 
      skorst=map(sped_bufer, 10, 99, 60, 5); //ивертирует диапазон под паролу, после новой команды.
   if (coxpan==2)  //если пришла команда записать//седьмая позиция в команде
      { EEPROM.update(31, skorst);  /// если значение новое перезаписываем.
        EEPROM.update(32, jarkost); //31,32..36 от фоноря взял //лишь бы не первые//
        EEPROM.update(33, invers);
        EEPROM.update(34, vremya);
        EEPROM.update(35, iffekt);
        EEPROM.update(36, shtorki);
        EEPROM.update(61, 177);         // как флаг того - что были сохранены настройки 
       }
    flag_tel=0;adrees = 0;//выходим из записи
     } //
//------------------обработка фраз с телефона--------------------------------------------------------------
 else {//если пришла фраза и это не команда то///*/
     if   (Str_telf!=Str_old) speed=0;  ///быстренько убираем старую строку
     else  speed=skorst; ///возвращаем скорость
    
     if  (dlina_telef() > zadan_dlina||iffekt==2) s=2,y=2;//если фраза не помещаеться//или мы хотим ,что бы бежала.
     else if (dlina_telef() <= zadan_dlina && shtorki==2)  s=0,y=3;//если мы выбрали эфект шторки с телефона
     else s=0,y=1;  // строка статична
   
   P.displayAnimate(); 
       P.setInvert(LOW);//убираем инверсию если она была//подстраховочка
       P.displayZoneText(ZONE_LOWER, msg_tel, PA_CENTER,P.getSpeed(), 0, efkt[s], efkt[y]);
       P.displayZoneText(ZONE_UPPER, msg_tel, PA_CENTER,P.getSpeed(), 0, efkt[s], efkt[y]);
       P.synchZoneStart();  Str_old = Str_telf;
 
    if (new_taimer - taimer6 >= time_sbros && cikl==1 ) //пауза//если цикл=2, то фраза горит бесконечно
       {textN=3;flag_tel=0;adrees = 0; opros=0;sbros(); // по истечении паузы сбрасываем  на знак "/! " ///
       }
  ///  } //
}//
//===============тут управляет автомобиль уже сам================================================================
else if (PIND<16&& !tormaz && !pravo && !levo&& !nazad && !avariik) textN =3,opros=0;// если ничего не нажато и флаги сброшены
/// если, что то НАЖАТО, проверяем что?//по приоритету после первой истины, далее не проверяем (else  if)//
   //---------------------стоп --------------------------------------------------------------------
else if (!tormaz&& digitalRead(trm))///если нажат тормоз первый раз
         {tormaz=1;opros=0;textN =4;taimer1 = new_taimer; ctop=0;}//запускаем таймер инверсии
else if (tormaz)
       { if ( ctop >= invers)    P.setInvert(LOW);         //если прошло хх инверсий, то больше не раздражаем соседа сзади,
         else if (new_taimer - taimer1 > time_invers)  //если прошло хх сек, а тормоз нажат,то
                 { taimer1 = new_taimer;       // запоминаем время
                  P.setInvert(!P.getInvert()); // инвертируем СТОП
                  ++ctop;   
                 } 
       if (!digitalRead(trm))    sbros(); /// отпущен тормоз, сброс
       }
 //-----------------назад-------------------------------------------------------------------------  
else if (!nazad &&digitalRead(nzd))//нажато назад
        {nazad=1;opros=3;textN =7;
        } //увеличиваем скорость(как всегда),для быстрой смены фразы//    
else if (nazad)
        {if (!digitalRead(nzd) ) sbros(); //отпустили назад, сброс 
        }   
   //----------------аварийка-------------------------------------------------------------------------  
else if (!avariik&&digitalRead(prv)&&digitalRead(lev)) //2поворота
        {opros=0; textN =1;avariik=1; taimer4 = new_taimer; taimer5 = new_taimer;
        }  // аварийка // спасибо
else if (avariik) //спасибо горит ХХ сек даже если было однократное моргание //time_sbros 
        {
         if (digitalRead(prv)&&digitalRead(lev)) taimer4 = new_taimer;//2поворота снова моргнули//перевзводим таймер между морганиями аварийки 
         if (new_taimer - taimer5 >= time_spasibo) opros=3,textN =2;//выкл СПАСИБО ели прошло хх сек на 2 знака   "/!  /!  ///
         if (opros==3&&(new_taimer - taimer4 >= time_povorot)) sbros();//больше не моргает аварийка
        }
    //------------нажат поворот влево--------------------------------------------
else if (!levo && !avariik && !digitalRead(prv)&& digitalRead(lev))// вкл поворот влево
         { opros=2;textN =6;levo=1;taimer2 = new_taimer;
         }  // СТРЕЛКИ бегут влево
else if (levo)
        { if (!digitalRead(prv)&& digitalRead(lev))  taimer2 = new_taimer;// вкл поворот влево//перевзвдим  таймер2 
          if (!digitalRead(lev)&&(new_taimer - taimer2 >= time_povorot)) sbros(); // выкл поворот вправо
        }
   //-------------нажат поворот вправо------------------------------------------
else  if (!pravo&& !avariik &&digitalRead(prv)&& !digitalRead(lev))// 
         {opros=2;textN =5;pravo=1;taimer7 = new_taimer;
         }  // СТРЕЛКИ бегут вправо
else if (pravo)
       {if (digitalRead(prv)&& !digitalRead(lev))  taimer7 = new_taimer; // СТРЕЛКИ бегут вправо//певзводим таймер3 
        if (pravo && !digitalRead(prv)&&(new_taimer - taimer7 >= time_povorot)) sbros(); // выкл поворот вправо
       }
  //===========приступаем к анимации===========================================================================
 /// выводим результат на табло//анимация//выбор эфекта//норнализуем скорость//
if (!flag_tel)  //если нет фразы стелефона
    {
     switch (opros) //
        { case 0: s=0;y=1; speed=skorst;  break;  //строка стaтична
          case 1: s=0;y=3; speed=skorst;  break;  //используем эфект шторки (половину его)
          case 2: s=0;y=3; speed=skorst;  break;  // используем эфект шторки (половину его)
          case 3: s=0;y=3; speed=skorst+15; break;//назад и аварийка//эфк шторки//чуть помедленее кони.
          default: s=0;y=1; textN=0;  //если ничего не совпало//строка стaтична
         }
     if (textN!=text_old) speed=0; // при смене фразы увеличиваем скорость для быстрого ухода предыд фразы
      P.displayAnimate();    // крутим анимацию
         P.displayZoneText(ZONE_LOWER, msg_avto[textN], PA_CENTER,P.getSpeed(), 0, efkt[s], efkt[y]);
         P.displayZoneText(ZONE_UPPER, msg_avto[textN], PA_CENTER,P.getSpeed(), 0, efkt[s], efkt[y]);
         P.synchZoneStart(); 
         text_old=textN;    // выравниваем номер фраз чтобы скорость не росла
    }
} ///конец//а кто читал молодец*/
