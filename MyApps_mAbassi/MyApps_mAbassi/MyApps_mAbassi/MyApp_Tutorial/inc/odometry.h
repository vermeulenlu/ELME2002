#ifndef _ODOMETRY_H_
#define _ODOMETRY_H_

#include "CtrlStruct.h"

typedef struct RobotPosition{
  double x;
  double y;
  double theta;
  double last_t;

} RobotPosition;

void init_odometry(CtrlStruct *cvs);
void update_odometry(CtrlStruct *cvs);

#endif
