
#include "CtrlStruct.h"
#include "math.h"
#include "Real_localisation.h"
#include "odometry.h"
#include "MyApp_mAbassi.h"
void init_realposition(CtrlStruct *cvs){
	cvs->real_local->x = cvs->rob_pos->x;
	cvs->real_local->y = cvs->rob_pos->y;
	cvs->real_local->theta = cvs->rob_pos->theta;
}
/*
If one wants to combine the informations of the odometry with the informations
of the Triangularisation, It can be done here. We don't combine those informations
since it gives worse results than if we did.
*/
void update_realposition(CtrlStruct *cvs){
  double x_odo = cvs->rob_pos->x;
  double y_odo = cvs->rob_pos->y;
  double theta_odo = cvs->rob_pos->theta;

  cvs->real_local->x = x_odo;
  cvs->real_local->y = y_odo;

  if(theta_odo > 2*M_PI){
    cvs->real_local->theta = theta_odo-2*M_PI;
  }
  if(theta_odo < -2*M_PI){
    cvs->real_local->theta = 2*M_PI + theta_odo;
  }
  else{
    cvs->real_local->theta = theta_odo;
  }
  //printf("x = %f\n", cvs->real_local->theta*180/M_PI);
  //printf("x = %f\n", cvs->real_local->x);
  //printf("y = %f\n", cvs->real_local->y);
  //printf("\n");
}
