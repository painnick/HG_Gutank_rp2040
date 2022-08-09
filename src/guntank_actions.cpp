#include "guntank_actions.h"

int armAngles[2] = {ANGLE_CENTER, ANGLE_CENTER};
void turnArm(Servo* armsServo, GUNTANK_SIDE side, GUNTANK_DIRECTION direction, int angle) {
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

void fire(int* gunPins, GUNTANK_SIDE side) {
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

void track(int* trackPins, GUNTANK_SIDE side, GUNTANK_DIRECTION direction) {
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
