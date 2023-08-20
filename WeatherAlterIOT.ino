#include <SoftwareSerial.h>
#include "DHT.h"
#include <Servo.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

SoftwareSerial btSerial = SoftwareSerial(9, 10);

#define DHTPIN 3
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

unsigned long currTime = 0;
unsigned long prevTime = 0;

#define SERVO_PIN 8
Servo myservo;

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define RLED 7
#define YLED 6
#define BLED 5


void setup() {
  Serial.begin(9600);
  btSerial.begin(9600);
  dht.begin();
  myservo.attach(SERVO_PIN);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1,0);
  pinMode(RLED, OUTPUT);
  pinMode(BLED, OUTPUT);
  pinMode(YLED, OUTPUT);
}

void loop() {
  currTime = millis();
  if (currTime - prevTime >= 5000)
  {
    prevTime = currTime;
    float humi = dht.readHumidity();
    float temp = dht.readTemperature();
    float hic = dht.computeHeatIndex(temp, humi, false);
    if (temp >= 0 && temp <= 60)
    {
      Serial.print("온도");
      Serial.print(temp);
      Serial.println(",");
      Serial.print("습도");
      Serial.print(humi);
      Serial.println(",");
      Serial.print("체감 온도");
      Serial.print(hic);
      btSerial.println(String("온도     : ")+String(temp)+String("*C"));
      btSerial.println();
      btSerial.println(String("습도     : ")+String(humi)+String("%"));
      btSerial.println();
      btSerial.println(String("체감 온도 : ")+String(hic)+String("*C"));
      btSerial.println();

      //날씨
      String wt = "a";
      if(humi>65){
        if(temp<0){
          wt = "snow";
        }
        else if(temp>0){
          wt = "rain";
        }
      }
      else{
          wt = "sunny";
      }
      Serial.println();
      Serial.println(wt);

      //불쾌지수
      float di = (1.8*temp)-(0.55*(1-humi/100.0)*(1.8*temp-26))+32;
      Serial.println( di);
      btSerial.println(String("불쾌 지수 : ")+String(di));
      btSerial.println();
      btSerial.println(String("날씨     : ")+String(wt));
      btSerial.println();
      if(di<=69){
        //불쾌지수 좋음
        digitalWrite(RLED, LOW);
        digitalWrite(BLED, HIGH);
        digitalWrite(YLED, LOW);
      }
      else if(di>=70 && di<=75){
        //불쾌지수 보통
        digitalWrite(RLED, LOW);
        digitalWrite(BLED, LOW);
        digitalWrite(YLED, HIGH);
      }
      else if(di>=76){
        //불쾌지수 나쁨
        digitalWrite(RLED, HIGH);
        digitalWrite(BLED, LOW);
        digitalWrite(YLED, LOW);
      }
   
      //LCD
      lcd.init();
      lcd.setCursor(1,0);
      if(wt == "snow"){
        lcd.print("snow");
        lcd.setCursor(0,1);
        //lcd.print("Bring a shawl");
        lcd.print("wear thick clothes");
        myservo.write(235);
      }
      else if(wt == "rain"){
        lcd.print("rain");
        lcd.setCursor(0,1);
        lcd.print("Bring an umbrella");
        myservo.write(135);
      }
      else if(wt == "sunny"){
        myservo.write(180);
        if(temp > 26.0){
          lcd.print("sunny");
          lcd.setCursor(1,1);
          lcd.print("weather is hot");
        }
        else if(temp < 10.0){
          lcd.print("sunny");
          lcd.setCursor(1,1);
          lcd.print("weather is cold");
        }
        else{
          lcd.print("sunny");
          lcd.setCursor(1,1);
          lcd.print("weather is nice");
        }
      }
    }
  }
}
