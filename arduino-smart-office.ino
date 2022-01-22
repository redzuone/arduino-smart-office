#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219_1(0x40);
Adafruit_INA219 ina219_2(0x41);
Adafruit_INA219 ina219_3(0x44);
Adafruit_INA219 ina219_4(0x45);

unsigned long previousMillis = 0;
unsigned long previousMillisPirOne = 0;
unsigned long previousMillisPirTwo = 0;

// pins
int motorOnePin = 7;
int motorTwoPin = 8;
int ledOnePin = 12;
int ledTwoPin = 13;
int pirOnePin = 5;
int pirTwoPin = 6;
int hc05_rx = 3;
int hc05_tx = 4;

int testButton = 2; // button for quick testing - mode

int newData = 0;
int mode = 2;
// 0 is off mode
// 1  manual mode
// 2 auto mode

// bluetooth codes
String motorOneOff = "0";
String motorOneOn = "1";
int ledOneOff = "2";
int ledOneOn = "3";
int reqData = "4";
int motorTwoOff = "5";
int motorTwoOn = "6";
int ledTwoOff = "7";
int ledTwoOn = "8";
int toggleMode = "9";

SoftwareSerial MyBlue(hc05_rx, hc05_tx); // RX | TX 
// connect arduino tx->hc05 rx and arduino rx->hc05 tx

void bluetooth();
void turnOffAll();
void modeOne(String);
void modeTwo();
void sendData();
void getIna219Data(Adafruit_INA219 ina219);

void setup() {
  Serial.begin(9600);
  MyBlue.begin(9600);
  Serial.println("Ready");
  pinMode(motorOnePin, OUTPUT);
  pinMode(motorTwoPin, OUTPUT);
  pinMode(ledOnePin, OUTPUT);
  pinMode(ledTwoPin, OUTPUT);
  pinMode(pirOnePin, INPUT);
  pinMode(pirTwoPin, INPUT);
  pinMode(testButton, INPUT_PULLUP);

  if (! ina219_1.begin()) {
    Serial.println("Failed to find INA219 chip 1");
    while (1) { delay(10); }
  }
  if (! ina219_2.begin()) {
    Serial.println("Failed to find INA219 chip 2");
    while (1) { delay(10); }
  }
  if (! ina219_3.begin()) {
    Serial.println("Failed to find INA219 chip 3");
    while (1) { delay(10); }
  }
  if (! ina219_4.begin()) {
    Serial.println("Failed to find INA219 chip 4");
    while (1) { delay(10); }
  }
}
void loop() {
  unsigned long currentMillis = millis();
  
  // fetch bluetooth data
  String c;
  if (MyBlue.available()) {
    newData = 1;
    c = MyBlue.readString();
    Serial.println(c);
  }

  // change mode if relevant
  if (c == toggleMode) {
    mode += 1;
    if (mode > 2) {
      mode = 0;
      
      MyBlue.println("mode"+String(mode));
    }
  }

  // send data if requested
  if (c == reqData) {
    sendData();
  }

  // execute appropirate code depending on mode
  if(newData == 1 || mode == 2) {
    if(mode == 0) {
      Serial.println("mode 0");
      MyBlue.println("mode 0");
      turnOffAll();
    } else if(mode == 1) {
      Serial.println("mode 1");
      modeOne(c);
    } else if(mode == 2) {
      //Serial.println("mode 2");
      modeTwo();
    }
    newData = 0;
  }
  
  
  // send information from serial monitor - mostly for testing - may not work
  // enter 0, 1, or 2 in serial monitor to change mode | 4 to call sendData();
  if (Serial.available()) {
    int seriall = Serial.read();
    if (seriall == '0') {
      mode = 0;
      Serial.println("mode 0 serial");
    } else if (seriall == '1') {
      mode = 1;
      Serial.println("mode 1 ser");
    } else if (seriall == '2') {
      mode = 2;
      Serial.println("mode 2 serial");
    } else if (seriall == '4') {
      sendData();
      Serial.println("sendData()");
    }
    newData = 0;
  }
}

// functions

// 
void modeOne(String c) {
  if (c == motorOneOff) {
    digitalWrite(motorOnePin, LOW);
    //Serial.println(c);
    MyBlue.println("motor 1 off");
  } else if (c == motorOneOn) {
    digitalWrite(motorOnePin, HIGH);
    MyBlue.println("motor 1 on");
  } else if (c == ledOneOff) {
    digitalWrite(ledOnePin, LOW);
    MyBlue.println("led 1 off");
  } else if (c == ledOneOn) {
    digitalWrite(ledOnePin, HIGH);
    MyBlue.println("led 1 on");
  } else if (c == motorTwoOff) {
    digitalWrite(motorTwoPin, LOW);
    MyBlue.println("motor 2 off");
  } else if (c == motorTwoOn) {
    digitalWrite(motorTwoPin, HIGH);
    MyBlue.println("motor 2 on");
  } else if (c == ledTwoOff) {
    digitalWrite(ledTwoPin, LOW);
    MyBlue.println("led 2 off");
  } else if (c == ledTwoOn) {
    digitalWrite(ledTwoPin, HIGH);
    MyBlue.println("led 2 on");
  } else if( c == reqData) {
    // sendData carried out in main loop
  } else {
    MyBlue.println("u");
    Serial.print(c);
  }
}

void modeTwo() {
  while(!MyBlue.available()) {
    unsigned long currentMillis = millis();
    int pirOneValue = digitalRead(pirOnePin);
    int pirTwoValue = digitalRead(pirTwoPin);
    unsigned long currentMillisPirOne = millis();
    unsigned long currentMillisPirTwo = millis();
    if (pirOneValue && currentMillisPirOne - previousMillisPirOne > 1000) {
      Serial.println("reset timer pir 1");
      //turn on led, motor
      digitalWrite(ledOnePin, HIGH);
      digitalWrite(motorOnePin, HIGH);
      // reset timer
      previousMillisPirOne = millis();
      
    } else if (!pirOneValue && currentMillisPirOne - previousMillisPirOne > 3000){
      // only execute this after 3s passed and pirOne is on
      // Serial.println(String(previousMillisPirOne)+" "+String(currentMillisPirOne));
      // Serial.println("pir 1 off");
      // turn  off
      digitalWrite(ledOnePin, LOW);
      digitalWrite(motorOnePin, LOW);
    }
    
    if (pirTwoValue && currentMillisPirTwo - previousMillisPirTwo > 1000) {
      //Serial.println("reset timer pir 2");
      digitalWrite(ledTwoPin, HIGH);
      digitalWrite(motorTwoPin, HIGH);
      previousMillisPirTwo = millis();
      
    } else if (!pirTwoValue && currentMillisPirTwo - previousMillisPirTwo > 3000) {
      digitalWrite(ledTwoPin, LOW);
      digitalWrite(motorTwoPin, LOW);
    }
  }
}

void sendData() {
  Serial.println("data");
  MyBlue.println("data" + String(random(1, 10)));
  Adafruit_INA219 ina219Array[4] = {ina219_1, ina219_2, ina219_3, ina219_4};
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  for (int i = 0; i < 4; i++) {
    shuntvoltage = ina219Array[i].getShuntVoltage_mV();
    busvoltage = ina219Array[i].getBusVoltage_V();
    current_mA = ina219Array[i].getCurrent_mA();
    power_mW = ina219Array[i].getPower_mW();
    loadvoltage = busvoltage + (shuntvoltage / 1000);
  
    String inaData = "\nINA " + String(i) + "\nBus Voltage:   " + String(busvoltage) + " V";
    inaData += "\nShunt Voltage: " + String(shuntvoltage) + " mV";
    inaData += "\nLoad Voltage:  " + String(loadvoltage) + " V";
    inaData += "\nCurrent:       " + String(current_mA) + " mA";
    inaData += "\nPower:         " + String(power_mW) + " mW";
    
    Serial.println(inaData);
  }
  
  /*shuntvoltage = ina219_1.getShuntVoltage_mV();
  busvoltage = ina219_1.getBusVoltage_V();
  current_mA = ina219_1.getCurrent_mA();
  power_mW = ina219_1.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  String inaData = "INA 1\nBus Voltage:   " + String(busvoltage) + " V";
  inaData += "\nShunt Voltage: " + String(shuntvoltage) + " mV";
  inaData += "\nLoad Voltage:  " + String(loadvoltage) + " V";
  inaData += "\nCurrent:       " + String(current_mA) + " mA";
  inaData += "\nPower:         " + String(power_mW) + " mW";
  
  Serial.println(inaData);*/
  //getIna219Data(ina219Array[0]);
}

void getIna219Data(Adafruit_INA219 ina219) {
  Serial.println("test" +String(ina219.getShuntVoltage_mV()));
}

void turnOffAll() {
  digitalWrite(motorOnePin, LOW);
  digitalWrite(motorTwoPin, LOW);
  digitalWrite(ledOnePin, LOW);
  digitalWrite(ledOnePin, LOW);
}

void pir() {
  
}
