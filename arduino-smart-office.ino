#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

// note:
// this sketch use 115200 baud rate. default for hc05 is 9600. change baud rate in setup() or use AT commands for hc05
// pay attention to pin number

// todo: toggle instead of on and off

Adafruit_INA219 *ina219_1 = new Adafruit_INA219(0x40);
Adafruit_INA219 *ina219_2 = new Adafruit_INA219(0x41);
Adafruit_INA219 *ina219_3 = new Adafruit_INA219(0x44);
Adafruit_INA219 *ina219_4 = new Adafruit_INA219(0x45);
Adafruit_INA219 ina219Array[4] = {*ina219_1, *ina219_2, *ina219_3, *ina219_4};

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
// connect arduino's tx->hc05's rx and arduino's rx->hc05's tx

int testButton = 2; // button for quick testing - mode



// bluetooth codes
String motorOneOff = "0";
String motorOneOn = "1";
String ledOneOff = "2";
String ledOneOn = "3";
String reqData = "4";
String motorTwoOff = "5";
String motorTwoOn = "6";
String ledTwoOff = "7";
String ledTwoOn = "8";
String toggleModeCode = "togMode";

// state
int newData = 0;
String mode = "off";
// 0 is off mode
// 1  manual mode
// 2 auto mode
int ledOneState = 0;
int motorOneState = 0;
int ledTwoState = 0;
int motorTwoState = 0;
int pirOneState = 0;
int pirTwoState = 0;

SoftwareSerial MyBlue(hc05_rx, hc05_tx); // RX | TX 

void bluetooth();
void turnOffAll();
void modeOne(String);
void modeTwo();
void sendData();
void getIna219Data(Adafruit_INA219 ina219);
void serialTest();

void setup() {
  Serial.begin(115200);
  MyBlue.begin(115200);
  Serial.println("Readyy");
  pinMode(motorOnePin, OUTPUT);
  pinMode(motorTwoPin, OUTPUT);
  pinMode(ledOnePin, OUTPUT);
  pinMode(ledTwoPin, OUTPUT);
  pinMode(pirOnePin, INPUT);
  pinMode(pirTwoPin, INPUT);
  pinMode(testButton, INPUT_PULLUP);
  
  uint32_t currentFrequency;
  if (! ina219Array[0].begin()) {
    Serial.println("Failed to find INA219 chip 1");
    while (1) { delay(10); }
  }
  if (! ina219Array[1].begin()) {
    Serial.println("Failed to find INA219 chip 2");
    while (1) { delay(10); }
  }
  if (! ina219Array[2].begin()) {
    Serial.println("Failed to find INA219 chip 3");
    while (1) { delay(10); }
  }
  if (! ina219Array[3].begin()) {
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
    Serial.println("received: "+String(c));
  }

  // change mode if relevant
  if (c.indexOf(toggleModeCode) != -1) {
    Serial.println("toggling mode");
    if (mode == "off") {
      mode = "user";
    } else if (mode == "user") {
      mode = "auto";
    } else if (mode == "auto") {
      mode = "off";
    }
      MyBlue.println("mode: "+String(mode));
  }

  // send data if requested
  if (c == reqData) {
    sendData();
  }

  // execute appropirate code depending on mode
  if(newData == 1 || mode == "auto") {
    if(mode == "off") {
      Serial.println("mode 0");
      turnOffAll();
    } else if(mode == "user") {
      Serial.println("mode 1");
      modeOne(c);
    } else if(mode == "auto") {
      Serial.println("mode 2");
      modeTwo();
    }
    newData = 0;
  }
  
  
  // send information from serial monitor - mostly for testing - may not work
  // enter 0, 1, or 2 in serial monitor to change mode | 4 to call sendData();
  serialTest();
}

// functions

// 
void modeOne(String c) {
  if (c == motorOneOff) {
    digitalWrite(motorOnePin, LOW);
    MyBlue.println("motor 1 off");
  } else if (c == motorOneOn) {
    digitalWrite(motorOnePin, HIGH);
    MyBlue.println("motor 1 on");
  } else if (c == ledOneOff) {
    digitalWrite(ledOnePin, LOW);
    MyBlue.println("led 1 off");
  } else if (c == ledOneOn) {
    digitalWrite(ledOnePin, HIGH);
    MyBlue.println("led1on");
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
  } else if (c == reqData) {
    // sendData carried out in main loop
  } else if (c == toggleModeCode) {
    //pass
  } else {
    MyBlue.println("u");
    Serial.println("print in modeOne: "+String(c));
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
      //MyBlue.println("pir1high,led1on,motor1on");
      if (!pirOneState) { // pir high
        MyBlue.println("room1on");
        Serial.println("room1on");
        pirOneState = 1;
      }
      // reset timer
      previousMillisPirOne = millis();
      
    } else if (!pirOneValue && currentMillisPirOne - previousMillisPirOne > 3000 && pirOneState == 1){
      // only execute this after 3s passed and pirOne is on
      // Serial.println(String(previousMillisPirOne)+" "+String(currentMillisPirOne));
      // Serial.println("pir 1 off");
      // turn  off
      digitalWrite(ledOnePin, LOW);
      digitalWrite(motorOnePin, LOW);
      MyBlue.println("room1off");
      Serial.println("room1off");
      pirOneState = 0;
      //MyBlue.println("pir1low,led1off,motor1off");
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

    if (!pirOneState && pirOneValue) { // pir high
      //.println("room1on");
      //pirOneState = 1;
    } else if (pirOneState && !pirOneValue) { //pir low
      //MyBlue.println("room1off");
      //pirOneState = 0;
    }
  
    // only for testing with serial monitor
    serialTest();
  }
}

void sendData() {
  //Serial.println("data");
  //MyBlue.println("data" + String(random(1, 10)));
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;
  String data = "";
  for (int i = 0; i < 4; i++) {
    shuntvoltage = ina219Array[i].getShuntVoltage_mV();
    busvoltage = ina219Array[i].getBusVoltage_V();
    current_mA = ina219Array[i].getCurrent_mA();
    power_mW = ina219Array[i].getPower_mW();
    loadvoltage = busvoltage + (shuntvoltage / 1000);
  
    /*String inaData = "\nINA " + String(i) + "\nBus Voltage:   " + String(busvoltage) + " V";
    inaData += "\nShunt Voltage: " + String(shuntvoltage) + " mV";
    inaData += "\nLoad Voltage:  " + String(loadvoltage) + " V";
    inaData += "\nCurrent:       " + String(current_mA) + " mA";
    inaData += "\nPower:         " + String(power_mW) + " mW";*/
    //Serial.println(inaData);
    
    Serial.println("INA "+ String(i));
    Serial.print("Bus Voltage:   "); Serial.print(busvoltage); Serial.println(" V");
    Serial.print("Shunt Voltage: "); Serial.print(shuntvoltage); Serial.println(" mV");
    Serial.print("Load Voltage:  "); Serial.print(loadvoltage); Serial.println(" V");
    Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");
    Serial.print("Power:         "); Serial.print(power_mW); Serial.println(" mW");
    Serial.println("");
    
    //MyBlue.print("data,pir," + String(i) + ",voltage," + String(loadvoltage) + ""
    //  ",current," + String(current_mA)+",power,"+String(power_mW));
    data += String(loadvoltage)+","+String(current_mA)+","+String(power_mW)+",";
  }
  data += "data"+String(random(1, 10));
  Serial.println(data);
  MyBlue.println(data);
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

void serialTest() {
  if (Serial.available()) {
    //Serial.println("open");
    int seriall = Serial.read();
    if (seriall == '0' || seriall == "off") {
      mode = "off";
      Serial.println("mode 0 serial");
    } else if (seriall == '1' || seriall == "user") {
      mode = "user";
      Serial.println("mode 1 ser");
    } else if (seriall == '2' || seriall == "auto") {
      mode = "auto";
      Serial.println("mode 2 serial");
    } else if (seriall == '4' || seriall == "data") {
      //4Serial.println("sendData()");
      sendData();
    }
    newData = 0;
  }
}
