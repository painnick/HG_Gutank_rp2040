#include <Servo.h>

#include "commons.h"
#include "guntank_types.h"
#include "guntank_utils.h"

#define ANGLE_CENTER 90

#define ARM_ANGLE_MIN 45
#define ARM_ANGLE_MAX 135

// Published values for SG90 servos; adjust if needed 
#define MIN_MICROS        800 
#define MAX_MICROS        2450 

#define SG90_DELAY 15 // ms
#define MINI_SG_DELAY 15 // ms

void turnArm(Servo* armsServo, GUNTANK_SIDE side, GUNTANK_DIRECTION direction, int angle);
void fire(int* gunPins, GUNTANK_SIDE side);
void track(int* trackPins, GUNTANK_SIDE side, GUNTANK_DIRECTION direction);