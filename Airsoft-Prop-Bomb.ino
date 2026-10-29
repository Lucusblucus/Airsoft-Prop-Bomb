#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>


#define BOMB_TIME 30 //In seconds
#define VOLUME 10 //Between 0-30

enum AudioFiles { //Audio Files
  Beep = 1,
  Planted,
  Explode,
  Defuse
};

LiquidCrystal_I2C lcd(0x27, 16, 2);

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}};
byte colPins[COLS] = {2, 4, 7, 8};
byte rowPins[ROWS] = {11, 12, A0, A1};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

SoftwareSerial mySerial(10, 9); // D10=RX, D9=TX
DFRobotDFPlayerMini myDFPlayer;

const char masterCode[5] = "1234"; 
const unsigned long bombTime = BOMB_TIME * 1000; // in ms

enum State { DISARMED, ARMED, EXPLODED };
State currentState = DISARMED;

const int buzzerPin = 3;

unsigned long lastBeepAt = 0;

String inputBuffer = "";
unsigned long armedAt = 0;
unsigned long lastDisplayUpdate = 0;

void setup() {

  lcd.init();
  lcd.backlight();

  mySerial.begin(9600);
  if (!myDFPlayer.begin(mySerial)) {
    lcd.setCursor(0, 0);
    lcd.print("DFPlayer error");
    while (true); 
  }
  myDFPlayer.volume(VOLUME);

  setDisarmedState();
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    handleKeyPress(key);
  }

  if (currentState == ARMED) {
    updateCountdown();
    updateBeep();
  } 
}

void handleKeyPress(char key) {
  if (currentState == EXPLODED) {
    return; // game over, ignore input until physically reset
  }

  if (key == '*') { //Clear code
    inputBuffer = "";
    showEntryScreen();
    return;
  }

  if (key >= '0' && key <= '9') { //Enter code
    inputBuffer += key;

    if (inputBuffer.length() == 4) {
      checkCode();
      inputBuffer = "";
    } 
    showEntryScreen();
  }
}

void checkCode() {
  if (inputBuffer.equals(masterCode)) {
    if (currentState == DISARMED) {
      myDFPlayer.stop();
      myDFPlayer.play(Planted);
      delay(2000);
      setArmedState();

    } else if (currentState == ARMED) {
      myDFPlayer.stop();
      myDFPlayer.play(Defuse);
      delay(2000);
      myDFPlayer.stop();
      setDisarmedState();
    }
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wrong code");
    delay(1000);
    if (currentState == ARMED) {
      updateCountdown(); // redraw timer immediately after the message clears
    } else {
      lcd.setCursor(0, 0);
      lcd.print("BOMB DISARMED");
      showEntryScreen();
    }
  }
}

void setArmedState() {
  currentState = ARMED;
  armedAt = millis();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BOMB ARMED");
  myDFPlayer.stop();
  lastBeepAt = 0; 
}

void setDisarmedState() {
  currentState = DISARMED;
  myDFPlayer.stop();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BOMB DISARMED");
  showEntryScreen();
}

void setExplodedState() {
  currentState = EXPLODED;
  myDFPlayer.stop(); 
  myDFPlayer.play(Explode); 
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BOOM!");
  lcd.setCursor(0, 1);
  lcd.print("BOMB EXPLODED");

  delay(4000);
  myDFPlayer.stop(); 
}

void updateCountdown() {
  unsigned long elapsed = millis() - armedAt;

  if (elapsed >= bombTime) {
    setExplodedState();
    return;
  }

  // only redraw once a second, not every loop
  if (millis() - lastDisplayUpdate >= 1000) {
    lastDisplayUpdate = millis();
    unsigned long remaining = (bombTime - elapsed) / 1000;

    lcd.setCursor(0, 0);
    lcd.print("ARMED: ");
    lcd.print(remaining);
    lcd.print("s   "); // trailing spaces clear leftover digits
    showEntryScreen();
  }
}

void updateBeep() {
  unsigned long elapsed = millis() - armedAt;
  if (elapsed >= bombTime) return;
  unsigned long remaining = bombTime - elapsed;

  // 1 beep/sec until the last 12s, then speeds up, floor of 150ms
  unsigned long interval = constrain(remaining / 12, 150, 1000);

  if (millis() - lastBeepAt >= interval) {
    lastBeepAt = millis();
    myDFPlayer.play(Beep);
  }
}

void showEntryScreen() {
  lcd.setCursor(0, 1);
  lcd.print("Code: ");
  lcd.print(inputBuffer);
  lcd.print("    ");
}