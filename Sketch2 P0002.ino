/*
 Name:		Sketch2_P0002.ino
 Created:	2020/12/4 下午 10:32:30
 Author:	AS-CP6230
*/

/*宣告定義 */

//MENU

#include <menu.h>
#include <menuIO/liquidCrystalOut.h>
#include <menuIO/serialOut.h>
#include <menuIO/serialIn.h>
#include <menuIO/encoderIn.h>
#include <menuIO/keyIn.h>
#include <menuIO/chainStream.h>

using namespace Menu;


int hon = 80;
int hoff = 60;


#define joyX A0
#define joyY A1
#define joysw 40

encoderIn<joyX, joyY> encoder;//simple quad encoder driver
encoderInStream<joyX, joyY> encStream(encoder, 4);// simple quad encoder fake Stream

//a keyboard with only one key as the encoder button
keyMap encBtn_map[] = { {-joysw,defaultNavCodes[enterCmd].ch} };//negative pin numbers use internal pull-up, this is on when low
keyIn<1> encButton(encBtn_map);//1 is the number of keys

//input from the encoder + encoder button + serial
serialIn serial(Serial);
menuIn* inputsList[] = { &encStream,&encButton,&serial };
chainStream<3> in(inputsList);//3 is the number of inputs

#define LEDPIN 13

result doAlert(eventMask e, prompt& item);

result showEvent(eventMask e, navNode& nav, prompt& item) {
	Serial.print("event: ");
	Serial.println(e);
	return proceed;
}

int test = 55;


result action1(eventMask e, navNode& nav, prompt& item) {
	Serial.print("action1 event: ");
	Serial.print(e);
	Serial.println(", proceed menu");
	Serial.flush();
	return proceed;
}

result action2(eventMask e, navNode& nav, prompt& item) {
	Serial.print("action2 event: ");
	Serial.print(e);
	Serial.print(", quiting menu.");
	Serial.flush();
	return quit;
}

int ledCtrl = LOW;

result myLedOn() {
	ledCtrl = HIGH;
	return proceed;
}
result myLedOff() {
	ledCtrl = LOW;
	return proceed;
}

TOGGLE(ledCtrl, setLed, "Led: ", doNothing, noEvent, noStyle//,doExit,enterEvent,noStyle
	, VALUE("On", HIGH, doNothing, noEvent)
	, VALUE("Off", LOW, doNothing, noEvent)
);

int selTest = 0;
SELECT(selTest, selMenu, "Select", doNothing, noEvent, noStyle
	, VALUE("Zero", 0, doNothing, noEvent)
	, VALUE("One", 1, doNothing, noEvent)
	, VALUE("Two", 2, doNothing, noEvent)
);

int chooseTest = -1;
CHOOSE(chooseTest, chooseMenu, "Choose", doNothing, noEvent, noStyle
	, VALUE("First", 1, doNothing, noEvent)
	, VALUE("Second", 2, doNothing, noEvent)
	, VALUE("Third", 3, doNothing, noEvent)
	, VALUE("Last", -1, doNothing, noEvent)
);

//customizing a prompt look!
//by extending the prompt class
class altPrompt :public prompt {
public:
	altPrompt(constMEM promptShadow& p) :prompt(p) {}
	Used printTo(navRoot& root, bool sel, menuOut& out, idx_t idx, idx_t len, idx_t) override {
		return out.printRaw(F("special prompt!"), len);;
	}
};

MENU(subMenu, "Sub-Menu", showEvent, anyEvent, noStyle
	, OP("Sub1", showEvent, anyEvent)
	, OP("Sub2", showEvent, anyEvent)
	, OP("Sub3", showEvent, anyEvent)
	, altOP(altPrompt, "", showEvent, anyEvent)
	, EXIT("<Back")
);

/*extern menu mainMenu;
TOGGLE((mainMenu[1].enabled),togOp,"Op 2:",doNothing,noEvent,noStyle
  ,VALUE("Enabled",enabledStatus,doNothing,noEvent)
  ,VALUE("disabled",disabledStatus,doNothing,noEvent)
);*/

// char* constMEM hexDigit MEMMODE="0123456789ABCDEF";
// char* constMEM hexNr[] MEMMODE={"0","x",hexDigit,hexDigit};
// char buf1[]="0x11";

MENU(mainMenu, "Main menu", doNothing, noEvent, wrapStyle
	, OP("Op1", action1, anyEvent)
	, OP("Op2", action2, enterEvent)
	//,SUBMENU(togOp)
	, FIELD(test, "Test", "%", 0, 100, 10, 1, doNothing, noEvent, wrapStyle)
	, SUBMENU(subMenu)
	, SUBMENU(setLed)
	, OP("LED On", myLedOn, enterEvent)
	, OP("LED Off", myLedOff, enterEvent)
	, SUBMENU(selMenu)
	, SUBMENU(chooseMenu)
	, OP("Alert test", doAlert, enterEvent)
	// ,EDIT("Hex",buf1,hexNr,doNothing,noEvent,noStyle)
	, EXIT("<Back")
);

//const panel panels[] MEMMODE={{0,0,16,2}};
//navNode* nodes[sizeof(panels)/sizeof(panel)];
//panelsList pList(panels,nodes,1);

#define MAX_DEPTH 2
/*idx_t tops[MAX_DEPTH];
liquidCrystalOut outLCD(lcd,tops,pList);//output device for LCD
menuOut* constMEM outputs[] MEMMODE={&outLCD};//list of output devices
outputsList out(outputs,1);//outputs list with 2 outputs*/

MENU_OUTPUTS(out, MAX_DEPTH
	, LIQUIDCRYSTAL_OUT(lcd, { 0,0,16,2 })
	, NONE
);
NAVROOT(nav, mainMenu, MAX_DEPTH, in, out);//the navigation root object

result alert(menuOut& o, idleEvent e) {
	if (e == idling) {
		o.setCursor(0, 0);
		o.print("alert test");
		o.setCursor(0, 1);
		o.print("[select] to continue...");
	}
	return proceed;
}

result doAlert(eventMask e, prompt& item) {
	nav.idleOn(alert);
	return proceed;
}

result idle(menuOut& o, idleEvent e) {
	switch (e) {
	case idleStart:o.print("suspending menu!"); break;
	case idling:o.print("suspended..."); break;
	case idleEnd:o.print("resuming menu."); break;
	}
	return proceed;
}



#define Buzzer 39 //蜂鳴器
#define relay01 31 //繼電器
//RTC 7段顯示器 時鐘
#include <DS3231.h>
#include <TM1637TinyDisplay.h>
// DS1302 初始化設定

DS3231  rtc(SDA, SCL);
Time ti;

// 設定 TM1637 接腳
#define CLK 36
#define DIO 38
TM1637TinyDisplay display(CLK, DIO);
boolean colon = true;




//LCD
#include <Wire.h>
#include <LiquidCrystal.h>
#define RS 22
#define E 24
#define D4 26
#define D5 28
#define D6 30
#define D7 32

LiquidCrystal lcd(RS, E, D4, D5, D6, D7);  //建立 LCD 物件

//DHT

#include <Adafruit_Sensor.h> //DHT先導
#include <DHT_U.h> 
#include <DHT.h>
#define dhtPin 37
#define dhtType DHT11
DHT dht(dhtPin, dhtType); // Initialize DHT sensor

float h = dht.readHumidity();//讀取濕度
float t = dht.readTemperature();//讀取攝氏溫度
float f = dht.readTemperature(true);//讀取華氏溫度


// the setup function runs once when you press reset or power the board


void setup() {
	pinMode(Buzzer, OUTPUT);
	digitalWrite(Buzzer, LOW);

	lcd.begin(16, 2);  //定義 LCD 為 2 列 16 行顯示器
	lcd.clear();  //清除螢幕
	lcd.setCursor(0, 0);   //游標移到左上角
	lcd.print("Hello World!");   //在第一列印出 Hello World!
	delay(1000);

	dht.begin();//啟動DHT

	lcd.clear();  //清除螢幕
	lcd.setCursor(0, 0);   //游標移到左上角
	lcd.print("Now  Opening!!!");   //在第一列印出 Hello World!
	lcd.setCursor(0, 1);
	lcd.print("Please Wait!!!");

	//七段開啟
	display.setBrightness(0x0f);

	fdts();

	display.showString("HELLO");
	
	lcd.setCursor(0, 1);
	lcd.print("Done-------");
	Serial.begin(600);
	rtc.begin();
	/*
	rtc.setDate(12, 12, 2020);
	rtc.setTime(20, 15, 0);
	rtc.setDOW(6);
	*/

	pinMode(relay01, OUTPUT);
	digitalWrite(relay01, HIGH);

	digitalWrite(Buzzer, HIGH);
	/////////////////////////
	pinMode(joysw, INPUT_PULLUP);
	pinMode(LEDPIN, OUTPUT);
	Serial.begin(115200);
	while (!Serial);
	Serial.println("Arduino Menu Library"); Serial.flush();
	encoder.begin();
	
	nav.idleTask = idle;//point a function to be used when menu is suspended
	mainMenu[1].enabled = disabledStatus;
	nav.showTitle = false;
	lcd.setCursor(0, 0);
	lcd.print("Menu 4.x LCD");
	lcd.setCursor(0, 1);
	lcd.print("r-site.net");
	delay(2000);


}

// the loop function runs over and over again until power down or reset
void loop() {

	Serial.println("--");

	float h = dht.readHumidity();//讀取濕度
	float t = dht.readTemperature();//讀取攝氏溫度
	float f = dht.readTemperature(true);//讀取華氏溫度

	if (isnan(h) || isnan(t) || isnan(f)) {
		lcd.clear();  //清除螢幕
		lcd.setCursor(0, 0);   //游標移到左上角
		lcd.println("DHT ERROR!!");
		return;
	}
	char lineHum[100];//濕度
	char lineTemp[100];//濕度

	sprintf(lineHum, "Humidity: %d.%02d%%", (int)h, (int)(h * 100) % 100);
	sprintf(lineTemp, "Temp:   %d.%02d *c", (int)t, (int)(t * 100) % 100);

	Serial.print(lineHum);
	Serial.print("   ");
	Serial.println(lineTemp);

	lcd.clear();  //清除螢幕
	lcd.setCursor(0, 0);   //游標移到左上角
	lcd.print(lineHum);


	lcd.setCursor(0, 1);   //游標移到左上角
	lcd.print(lineTemp);
	delay(5000);


	int i = 10;
	do {
		ti = rtc.getTime();
		lcd.clear();
		char lineYMD[50], lineHMS[50];
		snprintf(lineYMD, sizeof(lineYMD), "Date:%02u-%02u-%02u", ti.year, ti.mon, ti.date);
		snprintf(lineHMS, sizeof(lineHMS), "Time:  %02u:%02u:%02u", ti.hour, ti.min, ti.sec);
		i--;
		Serial.println(lineYMD);
		Serial.println(lineHMS);
	 

		lcd.setCursor(0, 0);
		lcd.print(lineYMD);
		lcd.setCursor(0, 1);
		lcd.print(lineHMS);
	
		delay(1000);
	}while (i > 0);
	//LCD time repeat 10 times

		delay(2000);
		int  dptime= ti.hour *100+ ti.min ;
		uint8_t segto;
		int value = 1244;
		segto = 0x80 | display.encodeDigit((value / 100) % 10);
		display.setSegments(&segto);
		display.showNumberDec(dptime);
		
	//LED time update
		
		if (h>80)
		{digitalWrite(relay01,LOW);
		
		}
		else if (h < 60) {
			digitalWrite(relay01, HIGH);
		};

		delay(1000);
		

		////////////////////////

		nav.poll();
		digitalWrite(LEDPIN, ledCtrl);
		delay(100);//simulate a delay as if other tasks are running



}










void fdts() {

	/* Animation Data - HGFEDCBA Map */
	const uint8_t ANIMATION[104][4] = {
	  { 0x08, 0x08, 0x08, 0x08 },  // Frame 0
	  { 0x48, 0x48, 0x48, 0x48 },  // Frame 1
	  { 0x49, 0x49, 0x49, 0x49 },  // Frame 2
	  { 0x48, 0x48, 0x48, 0x48 },  // Frame 3
	  { 0x08, 0x08, 0x08, 0x08 },  // Frame 4
	  { 0x00, 0x00, 0x00, 0x00 },  // Frame 5
	  { 0x30, 0x00, 0x00, 0x00 },  // Frame 6
	  { 0x36, 0x00, 0x00, 0x00 },  // Frame 7
	  { 0x36, 0x30, 0x00, 0x00 },  // Frame 8
	  { 0x36, 0x36, 0x00, 0x00 },  // Frame 9
	  { 0x36, 0x36, 0x30, 0x00 },  // Frame 10
	  { 0x36, 0x36, 0x36, 0x00 },  // Frame 11
	  { 0x36, 0x36, 0x36, 0x30 },  // Frame 12
	  { 0x36, 0x36, 0x36, 0x36 },  // Frame 13
	  { 0x06, 0x36, 0x36, 0x36 },  // Frame 14
	  { 0x00, 0x36, 0x36, 0x36 },  // Frame 15
	  { 0x00, 0x06, 0x36, 0x36 },  // Frame 16
	  { 0x00, 0x00, 0x36, 0x36 },  // Frame 17
	  { 0x00, 0x00, 0x06, 0x36 },  // Frame 18
	  { 0x00, 0x00, 0x00, 0x36 },  // Frame 19
	  { 0x00, 0x00, 0x00, 0x06 },  // Frame 20
	  { 0x00, 0x00, 0x00, 0x00 },  // Frame 21
	  { 0x00, 0x00, 0x00, 0x02 },  // Frame 22
	  { 0x00, 0x00, 0x00, 0x20 },  // Frame 23
	  { 0x00, 0x00, 0x02, 0x00 },  // Frame 24
	  { 0x00, 0x00, 0x20, 0x00 },  // Frame 25
	  { 0x00, 0x02, 0x00, 0x00 },  // Frame 26
	  { 0x00, 0x20, 0x00, 0x00 },  // Frame 27
	  { 0x02, 0x00, 0x00, 0x00 },  // Frame 28
	  { 0x20, 0x00, 0x00, 0x00 },  // Frame 29
	  { 0x10, 0x00, 0x00, 0x00 },  // Frame 30
	  { 0x04, 0x00, 0x00, 0x00 },  // Frame 31
	  { 0x00, 0x10, 0x00, 0x00 },  // Frame 32
	  { 0x00, 0x04, 0x00, 0x00 },  // Frame 33
	  { 0x00, 0x00, 0x10, 0x00 },  // Frame 34
	  { 0x00, 0x00, 0x04, 0x00 },  // Frame 35
	  { 0x00, 0x00, 0x00, 0x10 },  // Frame 36
	  { 0x00, 0x00, 0x00, 0x04 },  // Frame 37
	  { 0x00, 0x00, 0x00, 0x00 },  // Frame 38
	  { 0x00, 0x00, 0x00, 0x04 },  // Frame 39
	  { 0x00, 0x00, 0x00, 0x10 },  // Frame 40
	  { 0x00, 0x00, 0x04, 0x00 },  // Frame 41
	  { 0x00, 0x00, 0x10, 0x00 },  // Frame 42
	  { 0x00, 0x04, 0x00, 0x00 },  // Frame 43
	  { 0x00, 0x10, 0x00, 0x00 },  // Frame 44
	  { 0x04, 0x00, 0x00, 0x00 },  // Frame 45
	  { 0x10, 0x00, 0x00, 0x00 },  // Frame 46
	  { 0x20, 0x00, 0x00, 0x00 },  // Frame 47
	  { 0x02, 0x00, 0x00, 0x00 },  // Frame 48
	  { 0x00, 0x20, 0x00, 0x00 },  // Frame 49
	  { 0x00, 0x02, 0x00, 0x00 },  // Frame 50
	  { 0x00, 0x00, 0x20, 0x00 },  // Frame 51
	  { 0x00, 0x00, 0x02, 0x00 },  // Frame 52
	  { 0x00, 0x00, 0x00, 0x20 },  // Frame 53
	  { 0x00, 0x00, 0x00, 0x02 },  // Frame 54
	  { 0x00, 0x00, 0x00, 0x00 },  // Frame 55
	  { 0x00, 0x00, 0x00, 0x06 },  // Frame 56
	  { 0x00, 0x00, 0x00, 0x30 },  // Frame 57
	  { 0x00, 0x00, 0x06, 0x00 },  // Frame 58
	  { 0x00, 0x00, 0x06, 0x00 },  // Frame 59
	  { 0x00, 0x00, 0x30, 0x00 },  // Frame 60
	  { 0x00, 0x06, 0x00, 0x00 },  // Frame 61
	  { 0x00, 0x30, 0x00, 0x00 },  // Frame 62
	  { 0x06, 0x00, 0x00, 0x00 },  // Frame 63
	  { 0x30, 0x00, 0x00, 0x00 },  // Frame 64
	  { 0x70, 0x40, 0x40, 0x46 },  // Frame 65
	  { 0x00, 0x00, 0x00, 0x06 },  // Frame 66
	  { 0x00, 0x00, 0x00, 0x00 },  // Frame 67
	  { 0x00, 0x00, 0x00, 0x06 },  // Frame 68
	  { 0x00, 0x00, 0x00, 0x03 },  // Frame 69
	  { 0x00, 0x00, 0x01, 0x01 },  // Frame 70
	  { 0x00, 0x01, 0x01, 0x00 },  // Frame 71
	  { 0x01, 0x01, 0x00, 0x00 },  // Frame 72
	  { 0x21, 0x00, 0x00, 0x00 },  // Frame 73
	  { 0x30, 0x00, 0x00, 0x00 },  // Frame 74
	  { 0x18, 0x00, 0x00, 0x00 },  // Frame 75
	  { 0x08, 0x08, 0x00, 0x00 },  // Frame 76
	  { 0x00, 0x08, 0x08, 0x00 },  // Frame 77
	  { 0x00, 0x00, 0x08, 0x08 },  // Frame 78
	  { 0x00, 0x00, 0x00, 0x0c },  // Frame 79
	  { 0x00, 0x00, 0x00, 0x44 },  // Frame 80
	  { 0x00, 0x00, 0x40, 0x40 },  // Frame 81
	  { 0x00, 0x40, 0x40, 0x00 },  // Frame 82
	  { 0x40, 0x40, 0x00, 0x00 },  // Frame 83
	  { 0x40, 0x00, 0x00, 0x00 },  // Frame 84
	  { 0x00, 0x00, 0x00, 0x00 },  // Frame 85
	  { 0x40, 0x00, 0x00, 0x00 },  // Frame 86
	  { 0x40, 0x40, 0x00, 0x00 },  // Frame 87
	  { 0x00, 0x40, 0x40, 0x00 },  // Frame 88
	  { 0x00, 0x00, 0x40, 0x40 },  // Frame 89
	  { 0x00, 0x00, 0x00, 0x44 },  // Frame 90
	  { 0x00, 0x00, 0x00, 0x0c },  // Frame 91
	  { 0x00, 0x00, 0x08, 0x08 },  // Frame 92
	  { 0x00, 0x08, 0x08, 0x00 },  // Frame 93
	  { 0x08, 0x08, 0x00, 0x00 },  // Frame 94
	  { 0x18, 0x00, 0x00, 0x00 },  // Frame 95
	  { 0x30, 0x00, 0x00, 0x00 },  // Frame 96
	  { 0x21, 0x00, 0x00, 0x00 },  // Frame 97
	  { 0x01, 0x01, 0x00, 0x00 },  // Frame 98
	  { 0x00, 0x01, 0x01, 0x00 },  // Frame 99
	  { 0x00, 0x00, 0x01, 0x01 },  // Frame 100
	  { 0x00, 0x00, 0x00, 0x03 },  // Frame 101
	  { 0x00, 0x00, 0x00, 0x06 },  // Frame 102
	  { 0x00, 0x00, 0x00, 0x00 }   // Frame 103

	};

	// Display Animation sequence
	display.showAnimation(ANIMATION, FRAMES(ANIMATION), TIME_MS(50));

}

