#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// void setup() {

//   lcd.print("Hello, Hello, Hello,");
//   lcd.setCursor(0, 1);
//   lcd.print("World");
// }

// void loop() {
//   // nothing needed here for the test
// }

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

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    lcd.print(key);
    lcd.setCursor(0, 0);
  }
}