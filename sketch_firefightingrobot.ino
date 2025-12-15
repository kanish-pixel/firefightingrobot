#include <Servo.h>

// ---------- Motor Driver (L293D) Pins ----------
#define motorA1 2
#define motorA2 3
#define motorB1 4
#define motorB2 5
#define motorC1 6
#define motorC2 7
#define motorD1 8
#define motorD2 9

// ---------- Flame Sensors ----------
#define leftFlame A0
#define centerFlame A1
#define rightFlame A2

// ---------- Servo and Pump ----------
#define servoPin 10
#define pumpPin 11

Servo waterServo;

void setup() {
  // Motor setup
  pinMode(motorA1, OUTPUT);
  pinMode(motorA2, OUTPUT);
  pinMode(motorB1, OUTPUT);
  pinMode(motorB2, OUTPUT);
  pinMode(motorC1, OUTPUT);
  pinMode(motorC2, OUTPUT);
  pinMode(motorD1, OUTPUT);
  pinMode(motorD2, OUTPUT);

  // Sensors setup
  pinMode(leftFlame, INPUT);
  pinMode(centerFlame, INPUT);
  pinMode(rightFlame, INPUT);

  // Output setup
  pinMode(pumpPin, OUTPUT);
  digitalWrite(pumpPin, LOW);  // Initialize pump off

  // Servo setup
  waterServo.attach(servoPin);
  waterServo.write(90);  // Start in center position

  Serial.begin(9600);
  Serial.println("🔥 Fire Fighting Robot Initialized...");
}
void loop() {
  int left = analogRead(leftFlame);
  int center = analogRead(centerFlame);
  int right = analogRead(rightFlame);

  // Define your threshold (tune based on your environment)
  int fireThreshold = 400;  // lower value = more fire detected

  Serial.print("LEFT: "); Serial.print(left);
  Serial.print("  CENTER: "); Serial.print(center);
  Serial.print("  RIGHT: "); Serial.println(right);

  bool leftFire = (left < fireThreshold);
  bool centerFire = (center < fireThreshold);
  bool rightFire = (right < fireThreshold);

  if (leftFire || centerFire || rightFire) {
    stopMoving();
    faceFire(leftFire, centerFire, rightFire);
    extinguishFire();
    resumePatrol();
  } else {
    Serial.println("No fire detected 🔍");
    patrolMovement();
  }

  delay(200);
}


// ---------- MOVEMENT FUNCTIONS ----------

void moveForward() {
  digitalWrite(motorA1, HIGH); digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, HIGH); digitalWrite(motorB2, LOW);
  digitalWrite(motorC1, HIGH); digitalWrite(motorC2, LOW);
  digitalWrite(motorD1, HIGH); digitalWrite(motorD2, LOW);
}

void moveBackward() {
  digitalWrite(motorA1, LOW); digitalWrite(motorA2, HIGH);
  digitalWrite(motorB1, LOW); digitalWrite(motorB2, HIGH);
  digitalWrite(motorC1, LOW); digitalWrite(motorC2, HIGH);
  digitalWrite(motorD1, LOW); digitalWrite(motorD2, HIGH);
}

void stopMoving() {
  digitalWrite(motorA1, LOW); digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, LOW); digitalWrite(motorB2, LOW);
  digitalWrite(motorC1, LOW); digitalWrite(motorC2, LOW);
  digitalWrite(motorD1, LOW); digitalWrite(motorD2, LOW);
}

void turnLeft() {
  digitalWrite(motorA1, LOW); digitalWrite(motorA2, HIGH);
  digitalWrite(motorB1, LOW); digitalWrite(motorB2, HIGH);
  digitalWrite(motorC1, HIGH); digitalWrite(motorC2, LOW);
  digitalWrite(motorD1, HIGH); digitalWrite(motorD2, LOW);
}

void turnRight() {
  digitalWrite(motorA1, HIGH); digitalWrite(motorA2, LOW);
  digitalWrite(motorB1, HIGH); digitalWrite(motorB2, LOW);
  digitalWrite(motorC1, LOW); digitalWrite(motorC2, HIGH);
  digitalWrite(motorD1, LOW); digitalWrite(motorD2, HIGH);
}

// ---------- FIRE RESPONSE FUNCTIONS ----------

void faceFire(int left, int center, int right) {
  if (left == LOW) {
    Serial.println("🔥 Fire detected on LEFT!");
    turnLeft();
    delay(700);
    stopMoving();
    waterServo.write(150);
  } 
  else if (right == LOW) {
    Serial.println("🔥 Fire detected on RIGHT!");
    turnRight();
    delay(700);
    stopMoving();
    waterServo.write(30);
  } 
  else {
    Serial.println("🔥 Fire detected in CENTER!");
    stopMoving();
    waterServo.write(90);
  }
}

void extinguishFire() {
  Serial.println("💧 Extinguishing Fire...");
  digitalWrite(pumpPin, HIGH);
  delay(4000); // Spray duration
  digitalWrite(pumpPin, LOW);
  delay(500);
  waterServo.write(90);
  Serial.println("✅ Fire Extinguished!");
  moveBackward();
  delay(1000);
  stopMoving();
}

// ---------- PATROLLING ----------

void patrolMovement() {
  static unsigned long lastMove = 0;
  static int direction = 0;

  if (millis() - lastMove > 3000) {  // change every 3 seconds
    direction = (direction + 1) % 4;
    lastMove = millis();
  }

  switch (direction) {
    case 0: moveForward(); break;
    case 1: turnLeft(); break;
    case 2: moveForward(); break;
    case 3: turnRight(); break;
  }
}

void resumePatrol() {
  Serial.println("🚗 Resuming Patrol...");
  moveForward();
  delay(2000);
  stopMoving();
}
