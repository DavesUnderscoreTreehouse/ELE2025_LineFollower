//===== Libraries =====
#include <Arduino.h>                  // Adds support for Arduino syntax when using PlatformIO environment in VS Code

#include <Servo.h>                    // Adds support for servo control

#define CUSTOM_SETTINGS
#define INCLUDE_GAMEPAD_MODULE
#include <Dabble.h>                   // Adds support for communication with Dabble app over bluetooth

#include <CytronMotorDriver.h>        // Adds support for Cytron's Maker Drive H-Bridge motor driver

//===== Pin Definitions =====
// Servo PWM pins
#define LEFT_SERVO       6            // Left servo
#define RIGHT_SERVO      7            // Right servo
// Motor driver pins
#define MOTOR_L_FORWARD  8            // M2B
#define MOTRO_L_BACK     9            // M2A
#define MOTOR_R_FORWARD  10           // M1A
#define MOTOR_R_BACK     11           // M1B
// Motor encoder pins
#define MOTOR_L_A        18           // Left motor encoder pin A
#define MOTOR_L_B        19           // Left motor encoder pin B
#define MOTOR_R_A        20           // Right motor encoder pin A
#define MOTOR_R_B        21           // Right motor encoder pin B
// IR pins
#define IR_LEFT          A0           // Left sensor
#define IR_CENTER        A1           // Center sensor
#define IR_RIGHT         A2           // Right sensor
#define IR_WALL          A3           // Wall sensor
// Ultrasonic pins
#define ULTRA_ECHO       52           // Ultrasonic echo
#define ULTRA_TRIG       53           // Ultrasonic trigger
// LED
#define LED_RED          22           // Indicator LED

//===== Global Variables =====
// Servo
Servo servoL;                         // Initialising servo objects
Servo servoR;
#define SERVO_MIN_PULSE  600          // Minimum pulse width in ms, default 544
#define SERVO_MAX_PULSE  2400         // Minimum pulse width in ms, default 2400
#define SCOOP_DOWN       15           // Scoop down preset angle
#define SCOOP_UP         45           // Scoop raised preset angle

// Motor
CytronMD motorL(PWM_PWM, MOTOR_L_FORWARD, MOTRO_L_BACK);   // PWM 1A = Pin 10, PWM 1B = Pin 11
CytronMD motorR(PWM_PWM, MOTOR_R_FORWARD, MOTOR_R_BACK);   // PWM 2A = Pin 8,  PWM 2B = Pin 9

// Motor encoder
volatile unsigned long countMLA = 0;  // Encoder count A for left motor
volatile unsigned long countMLB = 0;  // Encoder count B for left motor
volatile unsigned long countMRA = 0;  // Encoder count A for Right motor
volatile unsigned long countMRB = 0;  // Encoder count B for Right motor

// Telemetry
unsigned long previousTime = 0;       // Millis time last telemetry packet was sent
unsigned long dataTimeout = 1000;     // Length of time in milliseconds between telemetry packets

// ===== Function Declarations =====
int InvertedServoPos(int);
float UltrasonicDectection(unsigned long);
void LineFollow();
void RobotForward();
void RobotRemoteForward();
void RobotReverse();
void RobotTurnLeft();
void RobotTurnRight();
void RobotStop();
void TickMLA();
void TickMLB();
void TickMRA();
void TickMRB();

void setup() {
  // Pinmodes
  pinMode(LEFT_SERVO,      OUTPUT);
  pinMode(RIGHT_SERVO,     OUTPUT);
  pinMode(MOTOR_R_FORWARD, OUTPUT);
  pinMode(MOTOR_R_BACK,    OUTPUT);
  pinMode(MOTOR_L_FORWARD, OUTPUT);
  pinMode(MOTRO_L_BACK,    OUTPUT);
  pinMode(ULTRA_TRIG,      OUTPUT);
  pinMode(LED_RED,         OUTPUT);
  pinMode(MOTOR_L_A,  INPUT);
  pinMode(MOTOR_L_B,  INPUT);
  pinMode(MOTOR_R_A,  INPUT);
  pinMode(MOTOR_R_B,  INPUT);
  pinMode(IR_LEFT,    INPUT);
  pinMode(IR_LEFT,    INPUT);
  pinMode(IR_RIGHT,   INPUT);
  pinMode(IR_CENTER,  INPUT);
  pinMode(ULTRA_ECHO, INPUT);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(MOTOR_L_A), TickMLA, RISING);
  attachInterrupt(digitalPinToInterrupt(MOTOR_L_B), TickMLB, RISING);
  attachInterrupt(digitalPinToInterrupt(MOTOR_R_A), TickMRA, RISING);
  attachInterrupt(digitalPinToInterrupt(MOTOR_R_B), TickMRB, RISING);

  // Serial setup
  Serial.begin(250000);               // Setting serial baud rate of mega
  Serial2.begin(38400);               // Setting serial baud rate of serial 2 port for bluetooth telemetry
  Dabble.begin(38400);                // Setting Dabble baud rate to match rate of HC-05 

  // Servo setup
  servoL.write(SCOOP_DOWN);           // Preset servos to start with scoop down
  servoR.write(InvertedServoPos(SCOOP_DOWN));
  servoL.attach(LEFT_SERVO,  SERVO_MIN_PULSE, SERVO_MAX_PULSE);
  servoR.attach(RIGHT_SERVO, SERVO_MIN_PULSE, SERVO_MAX_PULSE);
}

void loop() {
  char inputChar;                     // Read in character from serial
  char command = ' ';                 // Validated alphabetic input
  int  value;                         // Validated numeric input 
  
  // Data output
  if ((millis() - previousTime) > dataTimeout) {
    previousTime = millis();
    Serial2.print("Distance to object: ");
    Serial2.println(UltrasonicDectection(10000));  // Read ultrasonic distance
    // Serial2.println("Servo positions (us): ");
    // Serial2.print("Left servo: ");
    // Serial2.println(servoL.readMicroseconds());
    // Serial2.print("Right servo: ");
    // Serial2.println(servoR.readMicroseconds());
    // Serial2.print("Left motor: ");
    // Serial2.print("Right motor: ");
  }

  // Read Dabble gamepad inputs
  Dabble.processInput();              // This function is used to refresh data obtained from smartphone.Hence calling this function is mandatory in order to get data properly from your mobile.
  //Serial.print("Key pressed: ");
  if (GamePad.isUpPressed()) {
    Serial.print("UP");
    command = 'U';
  }

  if (GamePad.isDownPressed()) {
    Serial.print("DOWN");
    command = 'D';
  }

  if (GamePad.isLeftPressed()) {
    Serial.print("Left");
    command = 'L';
  }

  if (GamePad.isRightPressed()) {
    Serial.print("Right");
    command = 'R';
  }

  if (GamePad.isSquarePressed()) {
    Serial.print("Square");
    command = 'Q';
  }

  if (GamePad.isCirclePressed()) {
    Serial.print("Circle");
    command = 'C';
  }

  if (GamePad.isCrossPressed()) {
    Serial.print("Cross");
    command = 'X';
  }

  if (GamePad.isTrianglePressed()) {
    Serial.print("Triangle");
    command = 'T';
  }

  if (GamePad.isStartPressed()) {
    Serial.print("Start");
    command = 'A';
  }

  if (GamePad.isSelectPressed()) {
    Serial.print("Select");
    command = 'B';
  }
  //Serial.println();

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
      RobotRemoteForward();
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
      // Serial.print("Invalid or empty command: ");
      //Serial.println(command);
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

/**
 * @brief Calculted distance to object using ultra sonic sensor
 * 
 * @param timeout unsigned long, duration of listen for echo in microseconds
 * @return int, distance to object -1 if no echo detected
 */
 float UltrasonicDectection(unsigned long timeout) {
  long duration = 0;           // Duration of sound wave
  float distance = 0;          // Distance calculated to object
  // Send sound pulse
  digitalWrite(ULTRA_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(ULTRA_TRIG, HIGH);
  delayMicroseconds(15);
  digitalWrite(ULTRA_TRIG, LOW);

  duration = pulseIn(ULTRA_ECHO, HIGH, timeout);   // Listen for echo response
  if (duration == 0) {
    //Serial.println("No echo detected");          // Print error
    return -1;                                   // Return error value
  } else {
    //Serial.print("Ultrasonic duration: ");
    //Serial.println(duration);
    distance = (duration / 2) * .0343;          // Calculate distance from time and speed of sound
    //Serial.print("Ultrasonic distance: ");
    //Serial.println(distance);
    return distance;
  }
 }

/**
 * @brief Line following pathing algorithm, runs until stopped by button press
 * 
 */
void LineFollow() {
  bool doLineFollow = true;
  //unsigned long lineUSTimeout = 8000;
  int left;
  int right;
  int center;
  int wall;
  //previousTime = millis();
  servoL.write(SCOOP_UP);   // Left servo down
  servoR.write(InvertedServoPos(SCOOP_UP));   // Right servo down
  Serial2.println("Do line following");
  digitalWrite(LED_RED, HIGH);
  delay(500);
  while (doLineFollow) {
    left = digitalRead(IR_LEFT);
    right = digitalRead(IR_RIGHT);
    center = digitalRead(IR_CENTER);
    wall = digitalRead(IR_WALL);

    // if ((millis() - previousTime) > lineUSTimeout)  {
    //   if (UltrasonicDectection(5000) < 5.5){
    //     Serial.println(UltrasonicDectection(5000));
    //     RobotStop();
    //     doLineFollow = false;
    //   }
    // }

    // Move forward if center sensor detects black
    if (center == HIGH && left == LOW && right == LOW) {
      RobotForward();  
      Serial.println("Line follow: Forward");
    }
    // Turn left if left sensor detects black
    else if (left == HIGH && center == LOW) {
      RobotTurnLeft();
      Serial.println("Line follow: Left");
    }
    // Turn right if right sensor detects black
    else if (right == HIGH && center == LOW) {
      RobotTurnRight();
      Serial.println("Line follow: Right");
    }
    // Stop if wall sensor detects black
    else if (wall == HIGH) {
      RobotStop();
      Serial.println("Wall detected");
      digitalWrite(LED_RED, LOW);
      delay(20);
      digitalWrite(LED_RED, HIGH);
      delay(20);
      digitalWrite(LED_RED, LOW);
      delay(20);
      digitalWrite(LED_RED, HIGH);
      servoL.write(SCOOP_DOWN);   // Left servo down
      servoR.write(InvertedServoPos(SCOOP_DOWN));   // Right servo down
    }
    // Check if button pressed to stop line following
    Dabble.processInput();
    if (GamePad.isSelectPressed()) {
      RobotStop();
      doLineFollow = false;
      delay(500);
    }
  }
  digitalWrite(LED_RED, LOW);
  Serial2.println("Line following stop");
}
// Motor control functions
/**
 * @brief Sets motors to forward at 80 
 * 
 */
void RobotForward() {
  motorL.setSpeed(100);
  motorR.setSpeed(100);
  Serial.println("Robot forward");
}
void RobotRemoteForward() {
  motorL.setSpeed(120);
  motorR.setSpeed(120);
  Serial.println("Robot forward");
}

/**
 * @brief Sets motors to backwards at 60 
 * 
 */
void RobotReverse() {
  motorL.setSpeed(-200);
  motorR.setSpeed(-200);
  Serial.println("Robot reverse");
}

/**
 * @brief Sets motors to turn left
 * 
 */
void RobotTurnLeft() {
  motorL.setSpeed(-80);  // Left motor slower
  motorR.setSpeed(150);  // Right motor faster
  Serial.println("Robot left");
}

/**
 * @brief Sets motors to turn right
 * 
 */
void RobotTurnRight() {
  motorL.setSpeed(150);  // Left motor faster
  motorR.setSpeed(-80);  // Right motor slower
  Serial.println("Robot right");
}

/**
 * @brief Stops all motors and returns servos to safe
 * 
 */
void RobotStop() {
  motorL.setSpeed(0);         // Left motor stop
  motorR.setSpeed(0);         // Right motor stop
  servoL.write(SCOOP_UP);   // Left servo down
  servoR.write(InvertedServoPos(SCOOP_UP));   // Right servo down
  Serial.println("Robot stop");
}

// ISRs
/**
 * @brief Increment countMLA, left motor A count
 * 
 */
void TickMLA() {
  countMLA++;
}
/**
 * @brief Increment countMLB, left motor B count
 * 
 */
void TickMLB() {
  countMLB++;
}
/**
 * @brief Increment countMRA, right motor A count
 * 
 */
void TickMRA() {
  countMRA++;
}
/**
 * @brief Increment countMRB, right motor B count
 * 
 */
void TickMRB() {
  countMRB++;
}