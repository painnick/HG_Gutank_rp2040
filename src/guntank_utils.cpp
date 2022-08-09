#include "guntank_utils.h"

GUNTANK_SIDE convertToSide(char c) {
  if (c == 'l')
    return GUNTANK_SIDE::LEFT;
  else if (c == 'r')
    return GUNTANK_SIDE::RIGHT;
  else if (c == 'b')
    return GUNTANK_SIDE::BOTH;
  else
    return GUNTANK_SIDE::UNKNOWN_SIDE;
}

GUNTANK_DIRECTION convertToDirection(char c) {
  if (c == 'f')
    return GUNTANK_DIRECTION::FORWARD;
  else if (c == 'b')
    return GUNTANK_DIRECTION::BACKWARD;
  else if (c == 'c')
    return GUNTANK_DIRECTION::CENTER;
  else
    return GUNTANK_DIRECTION::UNKNOWN_DIRECTION;
}

void responseSide(UART* serialBt, GUNTANK_SIDE side) {
  switch (side) {
  case GUNTANK_SIDE::LEFT:
    serialBt->print("Left");
    break;
  case GUNTANK_SIDE::RIGHT:
    serialBt->print("Right");
    break;
  case GUNTANK_SIDE::BOTH:
    serialBt->print("Both");
    break;
  default:
    break;
  }
}

void responseDirection(UART* serialBt, GUNTANK_DIRECTION direction) {
  switch (direction) {
  case GUNTANK_DIRECTION::FORWARD:
    serialBt->print("Foward");
    break;
  case GUNTANK_DIRECTION::BACKWARD:
    serialBt->print("Backward");
    break;
  case GUNTANK_DIRECTION::CENTER:
    serialBt->print("Center(Stop)");
    break;
  default:
    break;
  }
}

void responseTrack(UART* serialBt, GUNTANK_SIDE side, GUNTANK_DIRECTION direction) {
  serialBt->print("Track ");
  responseSide(serialBt, side);
  serialBt->print(" ");
  responseDirection(serialBt, direction);
  serialBt->print("\r\n");
}

void responseArm(UART* serialBt, GUNTANK_SIDE side, GUNTANK_DIRECTION direction, int angle) {
  serialBt->print("Arm ");
  responseSide(serialBt, side);
  serialBt->print(" ");
  responseDirection(serialBt, direction);
  serialBt->print(" ");
  serialBt->print(angle);
  serialBt->print("\r\n");
}

void responseFire(UART* serialBt, GUNTANK_SIDE side) {
  serialBt->print("Fire! ");
  responseSide(serialBt, side);
  serialBt->print("\r\n");
}
