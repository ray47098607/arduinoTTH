
#include <Arduino.h>

/********************
  2020/12/22 ray47098607

  menu output to standard arduino LCD (LiquidCrystal)
  output: LCD
  input: Analog Joystick and Serial
  www.r-site.net
***/


#include <menu.h>
#include <menuIO/liquidCrystalOut.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <menuIO/analogAxisIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>

using namespace Menu;
//////////////////////
//LCD
#include <Wire.h>

#define RS 2
#define E 3
#define D4 4
#define D5 5
#define D6 6
#define D7 7

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);  //建立 LCD 物件

#define JOY_X A0
#define JOY_Y A1
#define JOY_BTN 8
analogAxis<JOY_X, 10, false> ay;

#define LEDPIN LED_BUILTIN
#define MAX_DEPTH 1

unsigned int on01 = 10;
unsigned int off01 = 90;
unsigned int on02 = 10;
unsigned int off02 = 90;
int test = 500;

MENU(mainMenu, "test menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
     , OP("Op1", doNothing, noEvent)
     , OP("Op2", doNothing, noEvent)
     
     , FIELD(on02, "On02", "ms", 0, 1000, 10, 1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
     , FIELD(off02, "Off02", "ms", 0, 10000, 10, 1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
     , FIELD(test, "test", "ms", 0, 10000, 10, 1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
     , EXIT("<Back")
    );
MENU(led01menu, "led01", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
     , FIELD(on01, "On01", "ms", 0, 1000, 10, 1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
     , FIELD(off01, "Off01", "ms", 0, 10000, 10, 1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
);


#define joyBtn 8
keyMap btnsMap[] = {{ -joyBtn, defaultNavCodes[enterCmd].ch}}; //negative pin numbers use internal pull-up, this is on when low
keyIn<1> btns(btnsMap);// 1 is the number of keys

serialIn menuSerialIn(Serial);

MENU_INPUTS(in, &ay, &btns, &menuSerialIn);

#define MAX_DEPTH 1


MENU_OUTPUTS(out, MAX_DEPTH
             , LIQUIDCRYSTAL_OUT(lcd, {0, 0, 16, 2})
             , NONE
            );

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out); //the navigation root object

void setup() {
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(9600);
  lcd.begin(16, 2);
  while (!Serial);
  lcd.clear();  //清除螢幕

  lcd.setCursor(0, 0);
  lcd.println("Menu 4.1.a");
  lcd.setCursor(0, 0);
  lcd.println("Use JoyStick to ");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.println("control the menu navigation");
  delay(5000);

}

bool blink01(int on01, int off01) {
  return millis() % (unsigned long)(on01 + off01) < (unsigned long)on01;
}
bool blink02(int on02, int off02) {
  return millis() % (unsigned long)(on02 + off02) < (unsigned long)on02;
}

void loop() {
  nav.poll();
  delay(100);
  digitalWrite(12, blink01(on01, off01));
  digitalWrite(11, blink02(on02, off02));
}


