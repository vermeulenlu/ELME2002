

#ifndef _OPPONENTS_H_
#define _OPPONENTS_H_

#include "CtrlStruct.h"
#include "MyApp_mAbassi.h"


typedef struct Opponents{
  double x[2];
  double y[2];
  double theta[2];
  uint32_t start_time;
} Opponents;

void init_opponents(CtrlStruct *cvs);
void update_opponents(CtrlStruct *cvs, int res_left, int res_right);


#endif
