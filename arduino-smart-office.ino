#include <SoftwareSerial.h>

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
int mode = 0;
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
  
  
  // send information from serial monitor - mostly for testing
  // enter 0, 1, or 2 in serial monitor to change mode
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
    }
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
    if (pirOneValue) {
      Serial.println("pir 1 on");
      //turn on
      digitalWrite(ledOnePin, HIGH);
      digitalWrite(motorOnePin, HIGH);
      // reset timer
      previousMillisPirOne = millis();
      
    } else if (currentMillisPirOne - previousMillisPirOne > 5000 && !pirOneValue){
      // only execute this after 5s passed and pirOne is on
      // Serial.println(String(previousMillisPirOne)+" "+String(currentMillisPirOne));
      // Serial.println("pir 1 off");
      // turn  off
      digitalWrite(ledOnePin, LOW);
      digitalWrite(motorOnePin, LOW);
    }
    
    if (pirTwoValue) {
      //Serial.println("pir 2");
      digitalWrite(ledTwoPin, HIGH);
      digitalWrite(motorTwoPin, HIGH);
      previousMillisPirTwo = millis();
      
    } else if (currentMillisPirTwo - previousMillisPirTwo > 5000 && !pirTwoValue) {
      digitalWrite(ledTwoPin, LOW);
      digitalWrite(motorTwoPin, LOW);
    }
  }
}

void sendData() {
  Serial.println("data");
  MyBlue.println("data" + String(random(1, 10)));
  
}

void turnOffAll() {
  digitalWrite(motorOnePin, LOW);
  digitalWrite(motorTwoPin, LOW);
  digitalWrite(ledOnePin, LOW);
  digitalWrite(ledOnePin, LOW);
}

void pir() {
  
}
