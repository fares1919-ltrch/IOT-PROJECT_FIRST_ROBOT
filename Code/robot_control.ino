// Include the Servo library
#include <Servo.h>

// Pin definitions for the L298N motor driver
#define EN_A 11 // Enable1 L298 Pin enA
#define IN1 10  // Motor1 L298 Pin in1
#define IN2 8   // Motor1 L298 Pin in2
#define IN3 7   // Motor2 L298 Pin in3
#define IN4 6   // Motor2 L298 Pin in4
#define EN_B 5  // Enable2 L298 Pin enB

// Pin definitions for sensors
#define LEFT_SENSOR A0     // IR left sensor
#define MIDDLE_SENSOR A1   // IR middle sensor
#define RIGHT_SENSOR A2    // IR right sensor
#define ECHO_PIN A4        // Ultrasonic Echo pin
#define TRIGGER_PIN A3     // Ultrasonic Trigger pin

// Servo setup
Servo servoMotor;

// Constants
const int MIN_DISTANCE = 20; // Minimum distance to avoid an obstacle
const int SERVO_CENTER = 70; // Servo's centered position
const int SERVO_LEFT = 140;
const int SERVO_RIGHT = 0;
const int MOTOR_SPEED = 50;  // Default motor speed

// Global variables
int distanceFront, distanceLeft, distanceRight;

// Function prototypes
long readUltrasonicDistance();
void scanSides();
void compareDistances();
void moveForward();
void turnRight();
void turnLeft();
void stopRobot();

void setup() {
  Serial.begin(9600); // Initialize serial communication

  // Configure sensor pins
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(MIDDLE_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);

  // Configure ultrasonic sensor pins
  pinMode(ECHO_PIN, INPUT);
  pinMode(TRIGGER_PIN, OUTPUT);

  // Configure motor driver pins
  pinMode(EN_A, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(EN_B, OUTPUT);

  // Initialize motors at reduced speed
  analogWrite(EN_A, MOTOR_SPEED);
  analogWrite(EN_B, MOTOR_SPEED);

  // Initialize servo
  servoMotor.attach(A5);
  servoMotor.write(SERVO_CENTER);
  delay(500);

  // Perform servo calibration
  for (int angle = SERVO_CENTER; angle <= SERVO_LEFT; angle += 5) {
    servoMotor.write(angle);
    delay(50);
  }
  for (int angle = SERVO_LEFT; angle >= SERVO_RIGHT; angle -= 5) {
    servoMotor.write(angle);
    delay(50);
  }
  servoMotor.write(SERVO_CENTER);
  delay(500);

  distanceFront = readUltrasonicDistance();
}

void loop() {
  // Read sensors and update distances
  distanceFront = readUltrasonicDistance();
  int leftSensor = digitalRead(LEFT_SENSOR);
  int middleSensor = digitalRead(MIDDLE_SENSOR);
  int rightSensor = digitalRead(RIGHT_SENSOR);

  // Log sensor values
  Serial.print("Front Distance: ");
  Serial.println(distanceFront);
  Serial.print("Left Sensor: ");
  Serial.println(leftSensor);
  Serial.print("Middle Sensor: ");
  Serial.println(middleSensor);
  Serial.print("Right Sensor: ");
  Serial.println(rightSensor);

  // Robot behavior logic
  if (middleSensor == HIGH) { // Line detected by middle sensor
    if (distanceFront > MIN_DISTANCE) {
      moveForward();
    } else {
      scanSides();
    }
  } else if (rightSensor == HIGH) { // Line detected by right sensor
    turnLeft();
  } else if (leftSensor == HIGH) { // Line detected by left sensor
    turnRight();
  } else { // No line detected
    stopRobot();
  }

  delay(10);
}

long readUltrasonicDistance() {
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration / 29 / 2; // Convert to centimeters
}

void scanSides() {
  stopRobot();
  delay(100);

  // Scan right side
  servoMotor.write(SERVO_LEFT);
  delay(500);
  distanceRight = readUltrasonicDistance();
  Serial.print("Right Distance: ");
  Serial.println(distanceRight);

  // Scan left side
  servoMotor.write(SERVO_RIGHT);
  delay(500);
  distanceLeft = readUltrasonicDistance();
  Serial.print("Left Distance: ");
  Serial.println(distanceLeft);

  // Center servo
  servoMotor.write(SERVO_CENTER);
  delay(500);

  // Compare distances and decide the turn direction
  compareDistances();
}

void compareDistances() {
  if (distanceLeft > distanceRight) {
    turnRight();
  } else {
    turnLeft();
  }
  delay(500);
  moveForward();
  delay(1000);
}

void moveForward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void turnLeft() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void stopRobot() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
} 
