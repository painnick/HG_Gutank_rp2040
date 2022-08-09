#include <Arduino.h>
#include <Servo.h>

#include "commons.h"
#include "guntank_types.h"
#include "guntank_utils.h"
#include "guntank_actions.h"

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

Servo armsServo[2];
int gunPins[2] = {PIN_LEFT_GUN, PIN_RIGHT_GUN};
int trackPins[4] = {PIN_TRACK_A1, PIN_TRACK_A2, PIN_TRACK_B1, PIN_TRACK_B2};

UART SerialBT(PIN_BLUETOOTH_TX, PIN_BLUETOOTH_RX, 0, 0);

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

  turnArm(armsServo, GUNTANK_SIDE::BOTH, GUNTANK_DIRECTION::CENTER, 0);
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
    turnArm(armsServo, convertToSide(side), convertToDirection(direction), angle);
    break;
  case 'f': // Fire. F (L/R/B)
    sscanf(command, "f%c", &side);
    fire(gunPins, convertToSide(side));
    break;
  case 't': // Track. T (L/R/B) (F/B/C)
    sscanf(command, "t%c%c", &side, &direction);
    track(trackPins, convertToSide(side), convertToDirection(direction));
    break;
  }
}
