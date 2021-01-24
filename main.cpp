
/*
    Name:       nakat_20201226.ino
    https://alexgyver.ru/encoder/
    https://alexgyver.ru/gyverstepper/
    https://alexgyver.ru/arduino-algorithms/
    https://alexgyver.ru/gyverstepper/
    https://electricdiylab.com/?s=Wire.h
    4.5.6.7.8.9.10.11.12.13.
*/

#include <Arduino.h>
#include <Stepper.h>
#include <Servo.h>
#include "GyverEncoder.h"
// объявленние методов
void nakat();
void muvA1();
void muvA2();
void muvA3();
void cuter();
void stopCuter();
void goBottonCuter();
void goTopCuter();
void parent();
//void nakat_ON();

uint32_t myTimer0, myTimer1, myTimer2;// переменные времени

int ZERo_val = 0;
int x = 0;
#define Drv_PIN 13                                          // включить, выключить мотор отрезкм *белый
#define Dir_PIN 12                                          // направление движения ножа отрезки *светлозеленый
#define ZERo  11                                            // 11 пин для позиционирования датчика начальной точки толкателя
#define MAGNIT 9                                            // 9 на реле могнита *желтый
//#define nakat_ON_OF 10                                    //
Stepper myStepper(200, 7, 8);                               //управление шаговым двигателем

#define PUSK_PIN 6                                          // кнопка принудительной отрезки
#define top_PIN 5                                           // датчик верхнего положения ножа 4 надо уточнить
#define botom_PIN 4                                         // датчик нижнего положения ножа  5
#define CLK A0
#define DT A1
#define SW A2

Encoder enc1(CLK, DT, SW);

int value = 0;


// датчик нижнего положения  ножа
//-----------------------------------------Управление накатным станком
const int8_t RELE_PIN  = 3;                                 // реле включения накатного станка * зудуный темно
const int8_t READ_PIN = 2;                                   // датчик готовности накатного станка
const uint32_t WORK = 1000;                                 //время работы накатного станка
bool rele1 = false;
int8_t in_data;
//----------------------------------------Управление протяжкой
boolean fl_t = false;
boolean fl_b = false;
boolean f_m = false;
boolean pusk = false;                                       // включает отрезку
boolean cutt = false;
boolean btnState = false;                                   // флаг датчика ZERO
boolean a1 = false;
boolean a2 = false;
boolean a3 = false;
int distnc_constant = 60;                                  //растояние от датчика zero до места отрезки
int L_shpilka = 400;                                    // 65мм длина заготовки
int L_a2 = 200;                                         // мм длина накатанной резьбы равна с каждой стороны
int zazor = 100;

void setup() {

  Serial.begin(115200);
  enc1.setType(TYPE1);        // тип энкодера TYPE1 одношаговый, TYPE2 двухшаговый. Если ваш энкодер работает странно, смените тип\=
  enc1.setFastTimeout(40);    // таймаут на скорость isFastR. По умолч. 50

  pinMode(RELE_PIN, OUTPUT);
  pinMode(READ_PIN, INPUT_PULLUP);
  //pinMode(nakat_ON_OF, INPUT_PULLUP);
  pinMode(Dir_PIN, OUTPUT);                                 // направление движения отрезчика
  pinMode(Drv_PIN, OUTPUT);                                  // стоп/пуск отрезчика
  pinMode(top_PIN, INPUT);                                  // сенсор верх нож
  pinMode(botom_PIN, INPUT);                                 // сенсор низ нож
  pinMode(ZERo, INPUT_PULLUP);                               // zero  кнопка на 11 и GND
  myStepper.setSpeed(400);
}
/*
  __________Z____G             a1       a2|      |
  1----------Z-----------------A          |      |         a1         вывод в зону отрезки
  2----------Z----A'B----------A          |      |         a1         отрезал
  3----------Z--------------------------A'B----------A     a2         вывод в зону накатки
  4----------Z--------------------------A'B>>>>>>>***A     a2         накатали
  5----------Z---------------------A' A***<<<<<<<B         a3         отвели проволоку, включили магнит, развернули 180 заготовку, выключили магнит
  6----------Z--------------------------A'A***<<<<***B              вывод в зону накатки, накатка
  7----------Z--------------------------A'A***^^^^***B              включили магнит,развернули 90, выключили магнит
  //Serial.print(a1);
*/
void loop() {
  // обязательная функция отработки. Должна постоянно опрашиваться
  enc1.tick();

 /* if (enc1.isRight()) value++;        // если был поворот направо, увеличиваем на 1
  if (enc1.isLeft()) value--;         // если был поворот налево, уменьшаем на 1

  if (enc1.isRightH()) value += 5;    // если было удержание + поворот направо, увеличиваем на 5
  if (enc1.isLeftH()) value -= 5;     // если было удержание + поворот налево, уменьшаем на 5

  if (enc1.isFastR()) value += 10;    // если был быстрый поворот направо, увеличиваем на 10
  if (enc1.isFastL()) value -= 10;    // если был быстрый поворот налево, уменьшаем на 10

  if (enc1.isTurn()) {                // если был совершён поворот (индикатор поворота в любую сторону)
    Serial.println(value);            // выводим значение при повороте
  }
  // конец энкодер
  */
  nakat();
  Serial.print("a1"); Serial.print(a1); Serial.print("a2"); Serial.println(a2);
  muvA1(); //1 движение с исходной точки в точку а1
  ZERo_val = digitalRead(ZERo);
  //parent();//возврат толкателя в исходное состояние
  uint32_t now = millis(); while (millis() - now < 500) {}//задержка
  cuter();

}
// вывод в зону отрезки и отрезал
void muvA1() {

  if (a1 == false)
  {
    myStepper.step((distnc_constant + L_shpilka) * (1));// двигаем проволоку до точки a1
  }
  a1 = true;// достигли точки а1
}
// вывод в зону накатки и накатали
void muvA2() {

  if ( a2 == false)
  {
    myStepper.step((L_a2) * (1));// двигаем проволоку до точки a2
  }
  a2 = true;// достигли точки а2
}
//делаем : отвели проволоку, включили магнит, развернули 180 заготовку, выключили магнит
void muvA3()
{
  if (a3 == false)
  {
    myStepper.step((L_a2) * (-1));// вернуть проволоку назад на длиеу накатки
  }
  a3 = true;//достигли успеха точки а3
}

void cuter() {
  if (fl_t == false && fl_b == false && digitalRead(PUSK_PIN) == false && digitalRead(botom_PIN) == !false && digitalRead(top_PIN) == !false) {
    goBottonCuter();                 //опустили нож в исходное состояние
  }
  else if (fl_t == false && fl_b == false && digitalRead(PUSK_PIN) == false && digitalRead(botom_PIN) == false && digitalRead(top_PIN) == !false) {
    stopCuter();                     // стоим внзу
    fl_b = true;                     // запомнили что были внизу
  }
  if ((fl_t == false) && (fl_b == true) && (digitalRead(PUSK_PIN) == !false) && (digitalRead(botom_PIN) == false) && (digitalRead(top_PIN) == !false)) {
    goTopCuter();                    //после сигнала PUSK_PIN) == !0 движимся вверх
    pusk = true;
  }
  else if ((fl_t == false) && (fl_b == true) && pusk == true && digitalRead(botom_PIN) == !false && digitalRead(top_PIN) == false) {
    stopCuter();
    uint32_t now = millis(); while (millis() - now < 300) {}//задержка
    goBottonCuter();                 //опустили нож в исходное состояние
    fl_t = true;
    fl_b = false;
  }
  else if (fl_t == true && fl_b == false && pusk == true && digitalRead(botom_PIN) == false && digitalRead(top_PIN) == !false) {
    stopCuter();                     //стоим внзу
    pusk = false;
    fl_t = false;
    fl_b = true;
    muvA2();
  }
}
void stopCuter() {
  digitalWrite(Dir_PIN, LOW);
  digitalWrite(Drv_PIN, LOW);
}
void goBottonCuter() {
  digitalWrite(Dir_PIN, LOW);
  digitalWrite(Drv_PIN, HIGH);
}
void goTopCuter() {
  digitalWrite(Dir_PIN, HIGH);
  digitalWrite(Drv_PIN, HIGH);
}
//метод возврата толкателя в исходное состояние
void parent()  
{
  do {
    x++;
    myStepper.step(1 * (-1));
    ZERo_val = digitalRead(ZERo);
  } while (ZERo_val == HIGH);
}
//void nakat_ON() { digitalWrite(10, HIGH); delay(2000); digitalWrite(10, LOW); }

void nakat() {
  if (rele1 == true || ! digitalRead(READ_PIN))
  {
    digitalWrite(RELE_PIN, HIGH);
    delay(WORK);
    digitalWrite(RELE_PIN, LOW);
    rele1 = false;
  }
}