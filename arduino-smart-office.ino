#include <SoftwareSerial.h>
#include <Wire.h>
#include <Adafruit_INA219.h>

// note:
// this sketch use 115200 baud rate. default for hc05 is 9600. change baud rate in setup() or use AT commands for hc05
// pay attention to pin number
// many parts are duplicated. could be simplified by turning them into functions

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
String reqData = "4";
String toggleModeCode = "togMode";
String ledOneToggle = "led1toggle";
String motorOneToggle = "motor1toggle";
String ledTwoToggle = "led2toggle";
String motorTwoToggle = "motor2toggle";

// state
int newData = 0;
String mode = "off"; // off, user, auto
int ledOneState = 0;
int motorOneState = 0;
int ledTwoState = 0;
int motorTwoState = 0;
int pirOneState = 0;
int pirTwoState = 0;

SoftwareSerial MyBlue(hc05_rx, hc05_tx); // RX | TX 

void turnOffAll();
void modeOne(String);
void modeTwo();
void sendData();
void serialTest();

void setup() {
  Serial.begin(115200);
  MyBlue.begin(115200);
  Serial.println("Ready");
  MyBlue.println("Ready");
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
  if (c == toggleModeCode) {
  //if (c.indexOf(toggleModeCode) != -1) {
    Serial.print("toggling mode - ");
    if (mode == "off") {
      mode = "user";
      Serial.print("user");
    } else if (mode == "user") {
      mode = "auto";
      turnOffAll();
      Serial.print("auto");
    } else if (mode == "auto") {
      mode = "off";
      Serial.print("off");
    }
    MyBlue.println("mode "+ mode);
  }

  // send data if requested
  if (c == reqData) {
    sendData();
  }

  // execute appropriate code depending on mode
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

void modeOne(String c) {
  if (c == ledOneToggle && ledOneState == 0) {
    digitalWrite(ledOnePin, HIGH);
    ledOneState = 1;
    MyBlue.println("led1on");
  } else if (c == ledOneToggle && ledOneState == 1) {
    digitalWrite(ledOnePin, LOW);
    ledOneState = 0;
    MyBlue.println("led1off");
    
  } else if (c == motorOneToggle && motorOneState == 0) {
    digitalWrite(motorOnePin, HIGH);
    motorOneState = 1;
    MyBlue.println("motor1on");
  } else if (c == motorOneToggle && motorOneState == 1) {
    digitalWrite(motorOnePin, LOW);
    motorOneState = 0;
    MyBlue.println("motor1off");
    
  } else if (c == ledTwoToggle && ledTwoState == 0) {
    digitalWrite(ledTwoPin, HIGH);
    ledTwoState = 1;
    MyBlue.println("led2on");
  } else if (c == ledTwoToggle && ledTwoState == 1) {
    digitalWrite(ledTwoPin, LOW);
    ledTwoState = 0;
    MyBlue.println("led2off");
    
  } else if (c == motorTwoToggle && motorTwoState == 0) {
    digitalWrite(motorTwoPin, HIGH);
    motorTwoState = 1;
    MyBlue.println("motor2on");
  } else if (c == motorTwoToggle && motorTwoState == 1) {
    digitalWrite(motorTwoPin, LOW);
    motorTwoState = 0;
    MyBlue.println("motor2off");
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
      // turn on led, motor if pirOne is high. checks every 1s
      digitalWrite(ledOnePin, HIGH);
      digitalWrite(motorOnePin, HIGH);
      //MyBlue.println("pir1high,led1on,motor1on");
      if (!pirOneState) { // sends bl code to update info on app and change state 
        MyBlue.println("room1on");
        Serial.println("room1on");
        pirOneState = 1;
      }
      // reset timer
      previousMillisPirOne = millis();
      
    } else if (!pirOneValue && currentMillisPirOne - previousMillisPirOne > 5000 && pirOneState == 1){
      // only execute this once after 5s passed and pirOne is low
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
      if (!pirTwoState) { // pir high
        MyBlue.println("room2on");
        Serial.println("room2on");
        pirTwoState = 1;
      }
      previousMillisPirTwo = millis();
      
    } else if (!pirTwoValue && currentMillisPirTwo - previousMillisPirTwo > 5000 && pirTwoState == 1) {
      digitalWrite(ledTwoPin, LOW);
      digitalWrite(motorTwoPin, LOW);
      MyBlue.println("room2off");
      Serial.println("room2off");
      pirTwoState = 0;
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

void turnOffAll() {
  digitalWrite(motorOnePin, LOW);
  digitalWrite(motorTwoPin, LOW);
  digitalWrite(ledOnePin, LOW);
  digitalWrite(ledOnePin, LOW);
  ledOneState = 0;
  motorOneState = 0;
  ledTwoState = 0;
  motorTwoState = 0;
  MyBlue.println("led1off,motor1off,led2off,motor2off");
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
