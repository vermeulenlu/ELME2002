
#ifndef _ODOMETRY_GR1_H_
#define _ODOMETRY_GR1_H_

#include "namespace_ctrl.h"
#include "CtrlStruct_gr1.h"

NAMESPACE_INIT(ctrlGr1);

typedef struct RobotPosition{
  double x;
  double y;
  double theta;
  double last_t;

} RobotPosition;

void init_odometry(CtrlStruct *cvs);
void update_odometry(CtrlStruct *cvs);

NAMESPACE_CLOSE();

#endif
