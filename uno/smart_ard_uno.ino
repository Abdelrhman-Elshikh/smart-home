#include "Board_helper.h"

void setup() {
  //lcd//
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Hello!");
  delay(10);
  dht.begin();
  delay(2000);
  lcd_static();

  //servo//
  servo.attach(servo_pin);
  servo.write(servo_lock_pos);

  //buzzer//
  pinMode(buz_pin, OUTPUT);

  //ir
  pinMode(ir_pin, INPUT);

  //LED
  pinMode(light_pin, OUTPUT);
  digitalWrite(light_pin, LOW);

  //nodemcu
  node.begin(115200);
  Serial.begin(115200);
}
void loop() {
  // get key value
  char key = customKeypad.getKey();
  if (key == 'A') {
    write_password();
  }
  if (digitalRead(ir_pin) == LOW) {
    digitalWrite(light_pin, HIGH);
  }
  readnode();
  lcd_static();
  temperature();
  humidity();
  speak();
  delay(1000);
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// send data to nodemcu///////////////////////////////////////////////////
void speak(void) {
  //conter for array
  int i = 0;
  //send array content
  while (i < 4) {
    node.write(readingData[i]);
    i++;
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////// listening to nodemcu///////////////////////////////////////////////////
void readnode(void) {
  byte iloop = 0;
  while (node.available() > 0) {
    delay(10);
    nodeData[iloop] = node.read();
    Serial.println(nodeData[iloop]);
    iloop++;
  }
  if (nodeData[0] == 0x10 && nodeData[1] == 0x01) {
    if (nodeData[2] == 0x11) {
      openDoor();
      Serial.print("opend");
      nodeData[0] = 0;
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// print the base for lcd///////////////////////////////////////////////////
void lcd_static() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("temp = ");
  lcd.setCursor(0, 1);
  lcd.print("humi = ");
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////// print temperature on lcd//////////////////////////////////////////////////
void temperature() {
  float temp = dht.readTemperature();
  if (isnan(temp)) {
    return;
  }
  lcd.setCursor(8, 0);
  lcd.print(temp);
  readingData[3] = temp;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////// print humidity on lcd//////////////////////////////////////////////
void humidity() {
  float humi = dht.readHumidity();
  if (isnan(humi)) {
    return;
  }
  lcd.setCursor(8, 1);
  lcd.print(humi);
  readingData[2] = humi;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////write password/////////////////////////////////////////////////////
void write_password() {
  uint16_t password = 0;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter password");
  char key = customKeypad.getKey();
  //if key does not equal Enter or Cancel
  while (1) {
    //if key is number
    if (key < 10 && key != NO_KEY) {
      if ((password * 10) + key <= 9999)
        password = (password * 10) + key;
    }
    //if key is delete
    else if (key == 'C') {
      password = int(password / 10);
    }
    //if key is Enter
    else if (key == 'A') {
      //if password right
      if (password == mypassword) {
        openDoor();
        return;
      }
      //if password wrong
      else if (password != mypassword) {
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("wrong password");
        //beeb alarm 3 times
        for (uint8_t i = 0; i < 3; i++) {
          digitalWrite(buz_pin, HIGH);
          delay(200);
          digitalWrite(buz_pin, LOW);
          delay(200);
        }
        return;
      }
    }
    //if key is Cancel
    else if (key == 'B') {
      return;
    }
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Enter password");
    lcd.setCursor(5, 1);
    lcd.print(password);
    key = customKeypad.getKey();
    delay(200);
  }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////open door/////////////////////////////////////////////////////
void openDoor(void) {
  lcd.clear();
  lcd.setCursor(5, 0);
  lcd.print("wellcome");
  //opne door for 5 sec
  servo.write(servo_unlock_pos);
  delay(5000);
  servo.write(servo_lock_pos);
}