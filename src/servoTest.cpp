#include <Arduino.h>
#include <Servo.h>

Servo servoL;
Servo servoR;

// global variables
int servoBottom = 10;
int servoTop = 50;
int servoPos = 0;

// put function declarations here:
int InvertedServoPos(int);

void setup() {
  // serial setup
  Serial.begin(115200);
  Serial.println("Starting");
  // servo setup
  servoL.attach(10);
  servoR.attach(11);
}

void loop() {
  if (Serial.available() > 0) {
    String incomingData;
    // read the incoming byte:
    incomingData = Serial.readString();
    Serial.print("Recieved data: ");
    Serial.println(incomingData);
    int intData = (incomingData.toInt());
    servoL.write(intData);          // tell servo to go to position in variable 'pos'
    servoR.write(InvertedServoPos(intData));
  }

  for (servoPos = servoBottom; servoPos <= servoTop; servoPos += 1) { // goes from 18 degrees to 50 degrees in steps of 1 degree
    servoL.write(servoPos);          // tell servo to go to position in variable 'pos'
    Serial.println(servoPos);
    servoR.write(InvertedServoPos(servoPos));
    delay(100);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);
  for (servoPos = servoTop; servoPos >= servoBottom; servoPos -= 1) { // goes from 50 degrees to 18 degrees in steps of 1 degree
    servoL.write(servoPos);          // tell servo to go to position in variable 'pos'
    Serial.println(servoPos);
    servoR.write(InvertedServoPos(servoPos));
    delay(100);                       // waits 15ms for the servo to reach the position
  }
  delay(1000);
}

int InvertedServoPos(int servoPos){
  int invertServoPos = 180 - servoPos;
  return invertServoPos;
}