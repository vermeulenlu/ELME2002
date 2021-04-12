#ifndef _REAL_LOCALISATION_H_
#define _REAL_LOCALISATION_H_

#include "CtrlStruct.h"

typedef struct RobotRealPosition{
  double x;
  double y;
  double theta;

} RobotRealPosition;

void init_realposition(CtrlStruct *cvs);
void update_realposition(CtrlStruct *cvs);

#endif
