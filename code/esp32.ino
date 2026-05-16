#include <Wire.h>
#include <Adafruit_VL53L1X.h>
#include <ESP32Servo.h>

// I2C SHTD Pins for Sequential Boot
const int XSHUT_LEFT = 10;
const int XSHUT_FRONT = 11;
const int XSHUT_RIGHT = 12;

// Drive Motor (TB6612FNG Channel A)
const int PWMA = 5;  
const int AIN1 = 18; 
const int AIN2 = 19; 

// Downforce Fan (TB6612FNG Channel B)
const int PWMB = 4;  
const int BIN1 = 16; 
const int BIN2 = 17; 

// Steering
const int SERVO_PIN = 6;
Servo steeringServo;

Adafruit_VL53L1X tofLeft, tofFront, tofRight;

// Serial Parsing Memory
String inputBuffer = "";
int targetX = 0;
int targetY = 0;

void setup() {
  Serial.begin(115200); // USB output for debugging
  // UART connection from Pi (Define RX, TX pins for ESP32-S3)
  Serial1.begin(115200, SERIAL_8N1, 44, 43); 

  pinMode(PWMA, OUTPUT); pinMode(AIN1, OUTPUT); pinMode(AIN2, OUTPUT);
  pinMode(PWMB, OUTPUT); pinMode(BIN1, OUTPUT); pinMode(BIN2, OUTPUT);
  
  steeringServo.attach(SERVO_PIN);

  // Initialize all ToF sensors to OFF
  pinMode(XSHUT_LEFT, OUTPUT); pinMode(XSHUT_FRONT, OUTPUT); pinMode(XSHUT_RIGHT, OUTPUT);
  digitalWrite(XSHUT_LEFT, LOW); digitalWrite(XSHUT_FRONT, LOW); digitalWrite(XSHUT_RIGHT, LOW);
  delay(10);

  Wire.begin();

  // Boot Sequence: Left
  digitalWrite(XSHUT_LEFT, HIGH);
  delay(10);
  tofLeft.begin(0x29);
  tofLeft.setAddress(0x30);

  // Boot Sequence: Front
  digitalWrite(XSHUT_FRONT, HIGH);
  delay(10);
  tofFront.begin(0x29);
  tofFront.setAddress(0x31);

  // Boot Sequence: Right
  digitalWrite(XSHUT_RIGHT, HIGH);
  delay(10);
  tofRight.begin(0x29);
  tofRight.setAddress(0x32);
  
  // Engage Downforce Fan (Boost Converter handles 12V output)
  digitalWrite(BIN1, HIGH);
  digitalWrite(BIN2, LOW);
  analogWrite(PWMB, 255); 
}

void loop() {
  // Check for incoming Vision packets
  while (Serial1.available() > 0) {
    char rc = Serial1.read();
    if (rc == '>') {
      parseData();
      inputBuffer = ""; // Reset buffer after parsing
    } else {
      inputBuffer += rc;
    }
  }
}

void parseData() {
  // Isolate arithmetic values from the string format <X:val,Y:val
  if (inputBuffer.startsWith("<X:")) {
    int commaIndex = inputBuffer.indexOf(',');
    targetX = inputBuffer.substring(3, commaIndex).toInt();
    targetY = inputBuffer.substring(commaIndex + 3).toInt();
    
    Serial.print("Target Acquired -> X: ");
    Serial.print(targetX);
    Serial.print(" Y: ");
    Serial.println(targetY);
    
    // Steering and Drive PID logic will execute here using targetX
  }
}
