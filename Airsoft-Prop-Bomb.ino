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

const int buzzerPin = 3;

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
}

void loop() {
  char key = keypad.getKey();
  if (key) {
    lcd.clear();
    lcd.setCursor(0, 0);

    if (key >= '1' && key <= '9') {
      int freq = map(key - '0', 1, 9, 500, 4000); // key 1 = 500Hz, key 9 = 4000Hz
      lcd.print("Tone: ");
      lcd.print(freq);
      lcd.print("Hz");
      tone(buzzerPin, freq);
    }
    else if (key == 'A') {
      lcd.print("Rising sweep");
      for (int f = 500; f <= 3000; f += 50) {
        tone(buzzerPin, f);
        delay(20);
      }
      noTone(buzzerPin);
    }
    else if (key == 'B') {
      lcd.print("Bomb beep");
      for (int i = 0; i < 5; i++) {
        tone(buzzerPin, 1000);
        delay(150);
        noTone(buzzerPin);
        delay(150);
      }
    }
    else if (key == '#') {
      lcd.print("Stopped");
      noTone(buzzerPin);
    }
  }
}