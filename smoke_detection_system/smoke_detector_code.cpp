#include <LiquidCrystal.h>

LiquidCrystal lcd_1(2, 3, 4, 5, 6, 7);

const int gasPin = A0;
const int buttonPin = A1;
int buttonState;
int gas = 0;
const int gasMedThreshold = 620;
const int gasHighThreshold = 750;
const int buzzerPin = A2;
int greenLED = A5;
int redLED = A4;
int yellowLED = A3;
bool alarmActive = false;
bool ledState = false;
const int FLASH_INTERVAL = 300;
int lastFlashTime = 0;

int gasState = 0;

void setup() {
  Serial.begin(9600); 
  lcd_1.begin(16, 2);
  pinMode(buzzerPin, OUTPUT); 
  analogWrite(greenLED, 255);

  lcd_1.print("Gas: Safe");
}

void loop() {
  buttonState = analogRead(buttonPin);
  gas = analogRead(gasPin);
  
  int buzzOutput = map(gas, 487, 876, 0, 255);
  Serial.println(buttonState);

  if (alarmActive && buttonState > 500) {
    alarmActive = false; 
    analogWrite(greenLED, 255);
    analogWrite(redLED, 0);
    analogWrite(yellowLED, 0);
    noTone(buzzerPin);

    lcd_1.clear();
    lcd_1.print("Gas: Safe");

    gasState = 0;
  }

  if (gas > gasHighThreshold && !alarmActive) {
    if (gasState != 2) {
      lcd_1.clear();
      lcd_1.print("Gas: HIGH!");
      gasState = 2;
    }

    alarmActive = true;
    runAlarm();
  }

  if (gas > gasMedThreshold && !alarmActive) {
    analogWrite(greenLED, 0);
    analogWrite(redLED, 0);
    analogWrite(yellowLED, 255);
    tone(buzzerPin, buzzOutput);

    if (gasState != 1) {
      lcd_1.clear();
      lcd_1.print("Gas: Medium");
      gasState = 1;
    }

  } else if (gas <= gasMedThreshold && !alarmActive) {
    analogWrite(greenLED, 255);
    analogWrite(redLED, 0);
    analogWrite(yellowLED, 0);
    
    if (gasState != 0) {
      lcd_1.clear();
      lcd_1.print("Gas: Safe");
      gasState = 0;
    }
  }

  if (alarmActive) {
    analogWrite(greenLED, 0);
    analogWrite(yellowLED, 0);
    
    lcd_1.setCursor(0, 1);
    lcd_1.print("!!! ALARM !!!");
    
    runAlarm();
  }

  delay(15);
}

void runAlarm() {
  if (millis() - lastFlashTime >= FLASH_INTERVAL) {
    lastFlashTime = millis();
    ledState = !ledState;

    if (ledState) {
      analogWrite(redLED, 255);
      tone(buzzerPin, 300);
    } else {
      analogWrite(redLED, 0);
      noTone(buzzerPin);
    }
  }
}