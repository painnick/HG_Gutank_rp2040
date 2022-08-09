#include <Arduino.h>
#include <Servo.h>

#include "guntank_types.h"
#include "guntank_utils.h"

#define PIN_TRACK_A1 10
#define PIN_TRACK_A2 11
#define PIN_TRACK_B1 12
#define PIN_TRACK_B2 13

#define PIN_LEFT_ARM 26
#define PIN_RIGHT_ARM 27

#define PIN_LEFT_GUN 2
#define PIN_RIGHT_GUN 3

#define PIN_BLUETOOTH_TX 8
#define PIN_BLUETOOTH_RX 9

// Published values for SG90 servos; adjust if needed 
#define MIN_MICROS        800 
#define MAX_MICROS        2450 

#define SG90_DELAY 15 // ms
#define MINI_SG_DELAY 15 // ms

#define ANGLE_CENTER 90

#define ARM_ANGLE_MIN 45
#define ARM_ANGLE_MAX 135

// DEBUG
#define _LOG_USB_
#define _SEND_RESPONSE_

Servo armsServo[2];
int gunPins[2] = {PIN_LEFT_GUN, PIN_RIGHT_GUN};
int trackPins[4] = {PIN_TRACK_A1, PIN_TRACK_A2, PIN_TRACK_B1, PIN_TRACK_B2};

UART SerialBT(PIN_BLUETOOTH_TX, PIN_BLUETOOTH_RX, 0, 0);

int armAngles[2] = {ANGLE_CENTER, ANGLE_CENTER};
void turnArm(GUNTANK_SIDE side, GUNTANK_DIRECTION direction, int angle) {
  if (side == GUNTANK_SIDE::UNKNOWN_SIDE)
    return;
  if (direction == GUNTANK_DIRECTION::UNKNOWN_DIRECTION)
    return;

  int newAngle = ANGLE_CENTER;
  if (direction != GUNTANK_DIRECTION::CENTER) {
    newAngle = armAngles[side] + (direction == GUNTANK_DIRECTION::FORWARD ? angle : -angle);
    newAngle = min(ARM_ANGLE_MAX, newAngle);
    newAngle = max(ARM_ANGLE_MIN, newAngle);
  }
  armAngles[side] = newAngle;
  armsServo[side].write(armAngles[side]);

  #ifdef _LOG_USB_
  responseArm(&Serial, side, direction, angle);
  #endif
  #ifdef _SEND_RESPONSE_
  responseArm(&SerialBT, side, direction, angle);
  #endif

  delay(MINI_SG_DELAY);
}

void fire(GUNTANK_SIDE side) {
  if (side == GUNTANK_SIDE::UNKNOWN_SIDE)
    return;

  bool isOn = true;
  for(int i = 0; i < 6; i ++) {
    if (side == GUNTANK_SIDE::BOTH) {
      digitalWrite(gunPins[GUNTANK_SIDE::LEFT], isOn ? HIGH : LOW);
      digitalWrite(gunPins[GUNTANK_SIDE::RIGHT], isOn ? HIGH : LOW);
      delay(100);
    } else {
      digitalWrite(gunPins[side], isOn ? HIGH : LOW);
      delay(100);
    }
    isOn = !isOn;
  }

  #ifdef _LOG_USB_
  responseFire(&Serial, side);
  #endif
  #ifdef _SEND_RESPONSE_
  responseFire(&SerialBT, side);
  #endif
}

void track(GUNTANK_SIDE side, GUNTANK_DIRECTION direction) {
  if (side == GUNTANK_SIDE::UNKNOWN_SIDE)
    return;
  if (direction == GUNTANK_DIRECTION::UNKNOWN_DIRECTION)
    return;

  switch (side) {
    case GUNTANK_SIDE::LEFT:
      switch (direction) {
      case GUNTANK_DIRECTION::FORWARD:
        digitalWrite(trackPins[0], HIGH);
        digitalWrite(trackPins[1], LOW);
        break;
      case GUNTANK_DIRECTION::BACKWARD:
        digitalWrite(trackPins[0], LOW);
        digitalWrite(trackPins[1], HIGH);
        break;
      case GUNTANK_DIRECTION::CENTER:
        digitalWrite(trackPins[0], LOW);
        digitalWrite(trackPins[1], LOW);
        break;
      }
    break;
    case GUNTANK_SIDE::RIGHT:
      switch (direction) {
      case GUNTANK_DIRECTION::FORWARD:
        digitalWrite(trackPins[2], HIGH);
        digitalWrite(trackPins[3], LOW);
        break;
      case GUNTANK_DIRECTION::BACKWARD:
        digitalWrite(trackPins[2], LOW);
        digitalWrite(trackPins[3], HIGH);
        break;
      case GUNTANK_DIRECTION::CENTER:
        digitalWrite(trackPins[2], LOW);
        digitalWrite(trackPins[3], LOW);
        break;
      }
    break;
    case GUNTANK_SIDE::BOTH:
      switch (direction) {
      case GUNTANK_DIRECTION::FORWARD:
        digitalWrite(trackPins[0], HIGH);
        digitalWrite(trackPins[1], LOW);
        digitalWrite(trackPins[2], HIGH);
        digitalWrite(trackPins[3], LOW);
        break;
      case GUNTANK_DIRECTION::BACKWARD:
        digitalWrite(trackPins[0], LOW);
        digitalWrite(trackPins[1], HIGH);
        digitalWrite(trackPins[2], LOW);
        digitalWrite(trackPins[3], HIGH);
        break;
      case GUNTANK_DIRECTION::CENTER:
        digitalWrite(trackPins[0], LOW);
        digitalWrite(trackPins[1], LOW);
        digitalWrite(trackPins[2], LOW);
        digitalWrite(trackPins[3], LOW);
        break;
      }
    break;
  }
  
  #ifdef _LOG_USB_
  responseTrack(&Serial, side, direction);
  #endif
  #ifdef _SEND_RESPONSE_
  responseTrack(&SerialBT, side, direction);
  #endif
}

void setup() {
  #ifdef _LOG_USB_
  Serial.begin(115200);
  #endif

  SerialBT.begin(9600);

  armsServo[GUNTANK_SIDE::LEFT].attach(PIN_LEFT_ARM);
  armsServo[GUNTANK_SIDE::RIGHT].attach(PIN_RIGHT_ARM);
  
  pinMode(PIN_TRACK_A1, OUTPUT);
  pinMode(PIN_TRACK_A2, OUTPUT);
  pinMode(PIN_TRACK_B1, OUTPUT);
  pinMode(PIN_TRACK_B2, OUTPUT);

  pinMode(PIN_LEFT_GUN, OUTPUT);
  pinMode(PIN_RIGHT_GUN, OUTPUT);

  SerialBT.write("AT+NAMEGuntank RP2040");
  while (!SerialBT.available()) {
    delay(100);
  }

  turnArm(GUNTANK_SIDE::BOTH, GUNTANK_DIRECTION::CENTER, 0);
  while (SerialBT.available()) {
    #ifdef _LOG_USB_
    Serial.write(SerialBT.read());
    #endif
  }
  #ifdef _LOG_USB_
  Serial.print("\n");

  Serial.println("R.E.A.D.Y!!!");
  #endif
}

char command[16];

void loop() {
  int index = 0;
  while (SerialBT.available()) {
    int c = SerialBT.read();
    command[index] = c;
    index++;
      unsigned long atThisTime = millis();
    do {
        ;
    } while (atThisTime + 5 > millis()); // 데이터 끊김 방지
  }
  if (index == 0) {
    delay(200);
    return;
  }
  command[index] = 0;
  #ifdef _LOG_USB_
  Serial.print("Command : [");
  Serial.print(command);
  Serial.println("]");
  #endif

  char side;
  char direction;
  int angle;
  switch (command[0])
  {
  case 'a': // Arm. A (L/R/B) (F/B/C) 15
    sscanf(command, "a%c%c%d", &side, &direction, &angle);
    turnArm(convertToSide(side), convertToDirection(direction), angle);
    break;
  case 'f': // Fire. F (L/R/B)
    sscanf(command, "f%c", &side);
    fire(convertToSide(side));
    break;
  case 't': // Track. T (L/R/B) (F/B/C)
    sscanf(command, "t%c%c", &side, &direction);
    track(convertToSide(side), convertToDirection(direction));
    break;
  }
}
