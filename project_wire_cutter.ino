#include <LiquidCrystal.h>
#include <Servo.h>
#include "pitches.h"
// chars ---------
byte DOWN[8] = {
  0b00000,
  0b01110,
  0b01110,
  0b11111,
  0b01110,
  0b00100,
  0b00000,
  0b00000
};
byte UP[8] = {
  0b00000,
  0b00000,
  0b00100,
  0b01110,
  0b11111,
  0b01110,
  0b01110,
  0b00000
};
byte RIGHT[8] = {
  0b00000,
  0b00100,
  0b00110,
  0b11111,
  0b00110,
  0b00100,
  0b00000,
  0b00000
};
byte LEFT[8] = {
  0b00000,
  0b00100,
  0b01100,
  0b11111,
  0b01100,
  0b00100,
  0b00000,
  0b00000
};
//----------------
Servo servoMotor;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int value = 0;
int stepper_pins[] = {10, 9, 8, 7};
int step_order[][4] = {
  {HIGH, LOW, HIGH, LOW},
  {LOW, HIGH, HIGH, LOW},
  {LOW, HIGH, LOW, HIGH},
  {HIGH, LOW, LOW, HIGH}
};
int WireLength = 50;
int prl = 0;
int WireCount = 3;
int prc = 0;
boolean MODIFY_LENGTH = true;
boolean MODIFY_UNIT = false;
boolean MODIFY_COUNT = false;
boolean SHIFTING = false;
String units[] = {"mm", "cm", "dm", "m"};
float units_co[] = {0.001, 0.01, 0.1, 1};
String unit = "mm";
String pru = "";
void setup() {
  Serial.begin(9600);
  // declaring costum chars
  lcd.createChar(0, UP); // create a new custom character UP ARROW
  lcd.createChar(1, DOWN); // create a new custom character DOWN ARROW
  lcd.createChar(2, LEFT); // create a new custom character LEFT ARROW
  lcd.createChar(3, RIGHT); // create a new custom character RIGHT ARROW
  lcd.begin(16, 2);
  pinMode(13, OUTPUT);
  for (int i = 0 ; i < 4 ; i++) {
    pinMode(stepper_pins[i], OUTPUT);
    digitalWrite(stepper_pins[i], LOW);
  }
  servoMotor.attach(6);



}

void loop() {
  Scene1();
  int bt = get_button(250, 500, 750, 100);
  if (bt == 0 || bt == 1) {
    modify_values(bt);
  } else if (bt == 2 || bt == 3) {
    shift_modifiers(bt);
  }
  Serial.println(String(MODIFY_LENGTH) + " | " + String(MODIFY_UNIT) + " | " + String(MODIFY_COUNT));
}
// lcd Scenes
void Scene1() {
  // this virtual page is responsible for setting the length and unit
  if (prl != WireLength || prc != WireCount || SHIFTING || pru != unit) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("L=" + String(WireLength) + unit + "|N=" + String(WireCount));
    lcd.setCursor(0, 1);
    lcd.write((byte)2);
    if (MODIFY_LENGTH)lcd.print("Length L");
    if (MODIFY_UNIT)lcd.print("Unit " + unit);
    if (MODIFY_COUNT)lcd.print("Count N");
    lcd.write((byte)3);
    lcd.setCursor(15, 0);
    lcd.write((byte)0);
    lcd.setCursor(15, 1);
    lcd.write((byte)1);
    prl = WireLength;
    prc = WireCount;
    SHIFTING = false;
    pru = unit;

  }
}
// nececcary functions
int get_button(int value1, int value2, int value3, int value4) {
  int value = analogRead(A0);
  if (value < value1) return -1;
  else if (value >= value1 && value < value2) return 0;
  else if (value >= value2 && value < value3) return 1;
  else if (value >= value3 && value < value4) return 2;
  else if (value >= value4) return 3;
}
void stepDir(int dir) {
  for (int i = 0 ; i < 4; i++) {
    for (int j = 0 ; j < 4; j++) {
      digitalWrite(stepper_pins[j], step_order[i][j]);
      Serial.println(stepper_pins[j] + " is set to " + step_order[i][j]);
    }
    Serial.println("-----------------------");
    delay(10);
  }
}
void cut() {
  for (int i = 0 ; i < 180; i++) {
    servoMotor.write(i);
    delay(1);
  }
  servoMotor.write(0);
  delay(1);
}
void modify_values(int dir) {
  if (MODIFY_LENGTH) {
    if (dir == 1 && WireLength > 1) {
      WireLength -= 1;
    } else if (dir == 0) {
      WireLength += 1;
    }
  }
  else if (MODIFY_UNIT) {
    int unitIndex = 0;
    for (int i = 0 ; i < (sizeof(units) / sizeof(units[0])); i++) {
      if (units[i] == unit) {
        unitIndex = i;
        break;
      }
    }
    if (dir == 0 && unitIndex < (sizeof(units) / sizeof(units[0])) - 1) {
      unit = units[unitIndex + 1];
      WireLength = round(WireLength * (units_co[unitIndex] / units_co[unitIndex + 1]));
    } else if (dir == 1 && unitIndex > 0) {
      unit = units[unitIndex - 1];
      WireLength = round(WireLength * (units_co[unitIndex] / units_co[unitIndex - 1]));
    }
  } else if (MODIFY_COUNT) {
    if (dir == 0) {
      WireCount += 1;
    } else if (dir == 1 && WireCount > 0) {
      WireCount -= 1;
    }
  }
  delay(300);
}
void shift_modifiers(int dir) {
  boolean stat[] = {MODIFY_UNIT, MODIFY_COUNT, MODIFY_LENGTH};
  if (dir == 3) {
    MODIFY_LENGTH = stat[0];
    MODIFY_UNIT = stat[1];
    MODIFY_COUNT = stat[2];
  }else{
    MODIFY_LENGTH = stat[1];
    MODIFY_UNIT = stat[2];
    MODIFY_COUNT = stat[0];
  }
  SHIFTING = true;
  delay(300);

}
void make_sound(int note) {

}