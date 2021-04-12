
#ifndef _LOCALISATION_GR1_H_
#define _LOCALISATION_GR1_H_

#include "namespace_ctrl.h"
#include "CtrlStruct_gr1.h"

NAMESPACE_INIT(ctrlGr1);

typedef struct Localisation{
  double angle_1;
  double distance_1;

  double angle_2;
  double distance_2;

  double angle_3;
  double distance_3;

  double x_estimated;
  double y_estimated;
  double theta_estimated;

  int first_turn;

} Localisation;

void init_localisation(CtrlStruct *cvs);
void update_localisation(CtrlStruct *cvs);

NAMESPACE_CLOSE();

#endif
