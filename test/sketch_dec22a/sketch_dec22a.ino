
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
#define JOY_BTN A2

#define LEDPIN LED_BUILTIN
#define MAX_DEPTH 1

unsigned int on01 = 10;
unsigned int off01 = 90;
unsigned int on02 = 10;
unsigned int off02 = 90;
int test = 500;

MENU(mainMenu, "raytest menu", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
     , OP("Op1", doNothing, noEvent)
     , OP("Op2", doNothing, noEvent)
     , FIELD(on01, "On01", "ms", 0, 1000, 10, 1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
     , FIELD(off01, "Off01", "ms", 0, 10000, 10, 1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
     , FIELD(on02, "On02", "ms", 0, 1000, 10, 1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
     , FIELD(off02, "Off02", "ms", 0, 10000, 10, 1, Menu::doNothing, Menu::noEvent, Menu::noStyle)
     , EXIT("<Back")
    );



#define joyBtn A2
keyMap btnsMap[] = {{ -joyBtn, defaultNavCodes[enterCmd].ch}}; //negative pin numbers use internal pull-up, this is on when low
keyIn<1> btns(btnsMap);// 1 is the number of keys

serialIn menuSerialIn(Serial);

MENU_INPUTS(in, &ay, &btns, &menuSerialIn);

#define MAX_DEPTH 1


MENU_OUTPUTS(out, MAX_DEPTH
             , SERIAL_OUT(lcd)
             , NONE //must have 2 items at least
            );

NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);

void setup() {
  pinMode(LEDPIN, OUTPUT);
  Serial.begin(9600);
  lcd.begin
  while (!Serial);
  lcd.clear();  //清除螢幕

  lcd.setCursor(0, 0);
  lcd.println("Menu 4.x");
  lcd.setCursor(0, 1);
  lcd.println("Use JoyStick");
  delay(1000)
  lcd.setCursor(0, 1);
  Serial.println("to control the menu navigation");
  lcd.autoscroll();
  delay(1000)
}

bool blink(int on01, int off01) {
  return millis() % (unsigned long)(on01 + off01) < (unsigned long)timeOn;
}
bool blink(int on02, int off02) {
  return millis() % (unsigned long)(on02 + off02) < (unsigned long)timeOn;
}

void loop() {
  nav.poll();
  digitalWrite(12, blink(on01, off01));
  digitalWrite(11, blink(on02, off02));
}


