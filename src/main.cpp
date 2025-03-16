//===== Libraries =====
#include <Arduino.h>            // Adds support for Arduino syntax when using PlatformIO environment in VS Code

#include <Servo.h>              // Adds support for servo control

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <Dabble.h>             // Adds support for communication with Dabble app over bluetooth

#include <CytronMotorDriver.h>  // Adds support for Cytron's Maker Drive H-Bridge motor driver

//===== Pin Definitions =====
// Servo PWM pins
#define LEFT_SERVO       6      // Left servo
#define RIGHT_SERVO      7      // Right servo
// Motor driver pins
#define MOTOR_R_FORWARD  10     // M1A
#define MOTOR_R_BACK     11     // M1B
#define MOTOR_L_FORWARD  8      // M2A
#define MOTRO_L_BACK     9      // M2B
// Motor encoder pins
//                          TODO
// IR pins
#define IR_LEFT          A0     // Left sensor
#define IR_CENTER        A1     // Center sensor
#define IR_RIGHT         A3     // Right sensor
// Ultrasonic pins
// #define ULTRA_ECHO       52     // Ultrasonic echo
// #define ULTRA_TRIG       53     // Ultrasonic trigger

//===== Global Variables =====
// Servo setup
Servo servoL;                   // Initialising servo objects
Servo servoR;
#define SERVO_MIN_PULSE  550    // Minimum pulse width in ms, default 544
#define SERVO_MAX_PULSE  2000   // Minimum pulse width in ms, default 2400
#define SCOOP_DOWN       0      // Scoop down preset angle
#define SCOOP_UP         30     // Scoop raised preset angle

// Motor setup
CytronMD motorL(PWM_PWM, MOTOR_L_FORWARD, MOTRO_L_BACK);   // PWM 1A = Pin 10, PWM 1B = Pin 11
CytronMD motorR(PWM_PWM, MOTOR_R_FORWARD, MOTOR_R_BACK);   // PWM 2A = Pin 8,  PWM 2B = Pin 9

// ===== Function Declarations =====
int InvertedServoPos(int);
void LineFollow();
void RobotForward();
void RobotReverse();
void RobotTurnLeft();
void RobotTurnRight();
void RobotStop();

void setup() {
  // Pinmodes
  pinMode(LEFT_SERVO,      OUTPUT);
  pinMode(RIGHT_SERVO,     OUTPUT);
  pinMode(MOTOR_R_FORWARD, OUTPUT);
  pinMode(MOTOR_R_BACK,    OUTPUT);
  pinMode(MOTOR_L_FORWARD, OUTPUT);
  pinMode(MOTRO_L_BACK,    OUTPUT);
  pinMode(IR_LEFT,    INPUT);
  pinMode(IR_RIGHT,   INPUT);
  pinMode(IR_CENTER,  INPUT);
  // pinMode(ULTRA_ECHO, INPUT);
  // pinMode(ULTRA_TRIG, INPUT);

  // Serial setup
  Serial.begin(250000);       // Setting serial baud rate of mega
  Dabble.begin(38400);        // Setting Dabble baud rate to match rate of HC-05 

  // Servo setup
  servoL.write(SCOOP_DOWN);                    // Preset servos to start with scoop down
  servoR.write(InvertedServoPos(SCOOP_DOWN));
  servoL.attach(LEFT_SERVO,  SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  servoR.attach(RIGHT_SERVO, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
}

void loop() {
  char inputChar;                     // Read in character from serial
  char command = ' ';                 // Validated alphabetic input
  int  value;                         // Validated numeric input 
  
  // Read Dabble gamepad inputs
  Dabble.processInput();              //this function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  if (GamePad.isUpPressed()) {
    Serial.println("Key pressed: UP");
    command = 'U';
  }

  if (GamePad.isDownPressed()) {
    Serial.println("Key pressed: DOWN");
    command = 'D';
  }

  if (GamePad.isLeftPressed()) {
    Serial.println("Key pressed: Left");
    command = 'L';
  }

  if (GamePad.isRightPressed()) {
    Serial.println("Key pressed: Right");
    command = 'R';
  }

  if (GamePad.isSquarePressed()) {
    Serial.println("Key pressed: Square");
    command = 'Q';
  }

  if (GamePad.isCirclePressed()) {
    Serial.println("Key pressed: Circle");
    command = 'C';
  }

  if (GamePad.isCrossPressed()) {
    Serial.println("Key pressed: Cross");
    command = 'X';
  }

  if (GamePad.isTrianglePressed()) {
    Serial.println("Key pressed: Triangle");
    command = 'T';
  }

  if (GamePad.isStartPressed()) {
    Serial.println("Key pressed: Start");
    command = 'A';
  }

  if (GamePad.isSelectPressed()) {
    Serial.println("Key pressed: Select");
    command = 'B';
  }

  // Check if input is a valid alphanumeric character
  if (Serial.available()) {
    inputChar = Serial.read();          // Read in serial character
    if (isAlpha(inputChar)) {           // If input valid pass to command
      Serial.print("Character input: ");
      Serial.println(inputChar);
      command = toUpperCase(inputChar); // Assign input to command
    } else if (isDigit(inputChar)) {    // If input valid digit pass to value and assign command
      Serial.print("Numeric input: ");
      Serial.println(inputChar);
      value = (inputChar - '0') * 10;   // Convert input to int
      command = 'V';                    // Assign 'V' command
    } else {                            // Invalid input
      Serial.print("Invalid input: ");
      Serial.println(Serial.readString());
    }
  }
  
  switch (command) {   // Apply inputs
    case 'A':
      RobotStop();
      break;

    case 'U':          // Up - Drive forward
      RobotForward();
      break;

    case 'D':          // Down - Drive backward
      RobotReverse();
      break;

    case 'L':          // Left - Turn left
      RobotTurnLeft();
      break;

    case 'R':          // Right - Turn right
      RobotTurnRight();
      break;

    case 'Q':          // Square - undefined
      // commands here
      break;

    case 'C':          // Circle - undefined
      // commands here
      break;

    case 'X':                           // Cross - Scoop down
      servoL.write(SCOOP_DOWN);
      servoR.write(InvertedServoPos(SCOOP_DOWN));
      Serial.println("Scoop down");
      break;

    case 'T':                           // Triangle - Scoop up
      servoL.write(SCOOP_UP);
      servoR.write(InvertedServoPos(SCOOP_UP));
      Serial.println("Scoop up");
      break;

    case 'B':                           // Select - Line follow
      LineFollow();
      break;

    case 'V':                           // Serial value input - Serial scoop control
      servoL.write(value);
      servoR.write(InvertedServoPos(value));
      break;
    
    default:                            // Default: invalid command
      Serial.print("Invalid or empty command: ");
      Serial.println(command);
      break;
  }
}

// ===== Functions =====
/**
 * @brief Inverts input relative to 180 degrees
 * 
 * @param servoPos 
 * @return * int 
 */
int InvertedServoPos(int servoPos) {
  int invertServoPos = 180 - servoPos;
  return invertServoPos;
}

void LineFollow() {
  while () {
    int left = digitalRead(IR_LEFT);
    int right = digitalRead(IR_RIGHT);
    int center = digitalRead(IR_CENTER);

    // Move forward if center sensor detects black
    if (center == HIGH && left == LOW && right == LOW) {
      RobotForward();  
      Serial.println("Forward");
    }
    // Turn left if left sensor detects black
    else if (left == HIGH && center == LOW) {
      RobotTurnLeft();
      Serial.println("Left");
    }
    // Turn right if right sensor detects black
    else if (right == HIGH && center == LOW) {
      RobotTurnRight();
      Serial.println("Right");
    }
  }
}
// Motor control functions
/**
 * @brief Sets motors to forward at 80 
 * 
 */
void RobotForward() {
  motorL.setSpeed(80);
  motorR.setSpeed(80);
  Serial.println("Robot forward");
}

/**
 * @brief Sets motors to backwards at 60 
 * 
 */
void RobotReverse() {
  motorL.setSpeed(-60);
  motorR.setSpeed(-60);
  Serial.println("Robot reverse");
}

/**
 * @brief Sets motors to turn left
 * 
 */
void RobotTurnLeft() {
  motorL.setSpeed(-20);  // Left motor slower
  motorR.setSpeed(100);  // Right motor faster
  Serial.println("Robot left");
}

/**
 * @brief Sets motors to turn right
 * 
 */
void RobotTurnRight() {
  motorL.setSpeed(100);  // Left motor faster
  motorR.setSpeed(-20);  // Right motor slower
  Serial.println("Robot right");
}

/**
 * @brief Stops all motors and returns servos to safe
 * 
 */
void RobotStop() {
  motorL.setSpeed(0);         // Left motor stop
  motorR.setSpeed(0);         // Right motor stop
  servoL.write(SCOOP_DOWN);   // Left servo down
  servoR.write(SCOOP_DOWN);   // Right servo down
  Serial.println("Robot stop");
}