# Arduino Smart Office Proof-of-Concept

Control and monitor power usage of DC motors and LEDs in an Android app via Bluetooth.
- Three operating modes: Off, User, and Auto
  - Auto: Turn on or off the components based on presence of motion presence in the area
  - User: Manual control of each component
  - Off: Turn all components off
- Monitor voltage, currrent, power of each component

## Hardware
- Arduino UNO
- HC-05 Bluetooth module
- INA219 modules
- HC-SR501 PIR motion sensors 
- LEDs, 5V DC motors

## Files

SmartOfficeApp.aia - MIT App Inventor project file
SmartOfficeApp.apk - App created using MIT App inventor
arduino-smart-office.ino - Arduino code

## Pictures
![smart-office0](https://github.com/redzuone/arduino-smart-office/assets/21290781/ea7c78fa-e11b-4fdd-b634-e72860c9d6e7)

![smart-office1](https://github.com/redzuone/arduino-smart-office/assets/21290781/b468dd36-94cb-42d6-91df-b356f4da73a1)

![smart-office2](https://github.com/redzuone/arduino-smart-office/assets/21290781/6ce5039e-a680-4e56-baef-61ff3bde5e93)
