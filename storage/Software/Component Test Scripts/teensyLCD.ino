#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Hello Teensy!");
  lcd.setCursor(0, 1);
  lcd.print("LCD Working!");
}

void loop() {
  static unsigned long counter = 0;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello Teensy!");
  
  counter++;
  delay(1000); // Update every second
}
