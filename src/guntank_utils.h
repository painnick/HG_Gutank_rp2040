#pragma once

#include <Serial.h>
#include "guntank_types.h"

GUNTANK_SIDE convertToSide(char c);
GUNTANK_DIRECTION convertToDirection(char c);

void responseTrack(UART* serialBt, GUNTANK_SIDE side, GUNTANK_DIRECTION direction);
void responseArm(UART* serialBt, GUNTANK_SIDE side, GUNTANK_DIRECTION direction, int angle);
void responseFire(UART* serialBt, GUNTANK_SIDE side);
