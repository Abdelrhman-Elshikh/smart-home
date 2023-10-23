//Include the library files
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define BLYNK_TEMPLATE_ID "TMPL25UwwjDRY"
#define BLYNK_TEMPLATE_NAME "smart home"
#define BLYNK_AUTH_TOKEN "VQMj2D2Xh_jMTHp906zusIFF_DQy6H-1"

char auth[] = BLYNK_AUTH_TOKEN;
/*
***************************
change wifi settings only
***************************
*/
char ssid[] = "smart123";   //Enter your WIFI name
char pass[] = "smart1234";  //Enter your WIFI password
/*
***************************
change wifi settings only
***************************
*/
//opject from blynk timer
BlynkTimer timer;
#include <SoftwareSerial.h>  //serial between Arduino and Node
SoftwareSerial ard(D5, D6);  //RX,TX
// received data from arduino
char rcData[10];
//humi data order
#define humiOrder 2
//temp data order
#define tempOrder 3
//data stream
#define door V0
#define light V1
#define humiDataStream V5
#define tempDataStream V4
//data decoding
#define startbit1 0x10
#define startbit2 0x01
#define doorcomand 0x11
////////////////vars/////////////
float temp = 0.0;
float humi = 0.0;
byte iloop = 0;
bool lightIOTFlag = false;
bool lightSWFlage = false;
//Get the button value
BLYNK_WRITE(door) {
  ard.write(startbit1);
  ard.write(startbit2);
  ard.write(doorcomand);
  delay(100);
}
BLYNK_WRITE(light){
  lightIOTFlag = param.asInt();
  delay(100);
}
void setup() {
  pinMode(D7,INPUT);
  pinMode(D8,OUTPUT);
  //Initialize the Blynk library
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  //Staring a timer
  timer.setInterval(1000L, myTimerEvent);
  //Start serial between arduino and nodemcu
  ard.begin(115200);
  Serial.begin(115200);
}

void myTimerEvent() {
  Blynk.virtualWrite(humiDataStream, humi);
  Blynk.virtualWrite(tempDataStream, temp);
}

void loop() {
  //read data from Arduino
  readArduino();
  //Run the Blynk library
  Blynk.run();
  timer.run();
  delay(100);
  lightSWFlage = digitalRead(D7);
  if(lightSWFlage || lightIOTFlag){
    digitalWrite(D8,HIGH);
  }
  else{
    digitalWrite(D8,LOW);
  }
}
void readArduino() {
  while (ard.available() > 0) {
    delay(100);
    rcData[iloop] = ard.read();
    iloop++;
  }
  Serial.println("get");
  if (rcData[0] == 0xA5 && rcData[1] == 0x5A) {
    temp = rcData[tempOrder];
    humi = rcData[humiOrder];
    iloop = 0;
  }
}