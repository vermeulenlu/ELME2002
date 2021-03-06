

#include "CtrlStruct.h"
#include <math.h>
#include "opponents.h"
#include "odometry.h"
#include "path_planning.h"
#include "Real_localisation.h"
#include "MyApp_I2C.h"


/*
Initialisation of the structure
*/
void init_opponents(CtrlStruct *cvs){

  cvs->opp->x[0] = 100000;
  cvs->opp->x[1] = 100000;
  cvs->opp->y[0] = 100000;
  cvs->opp->y[1] = 100000;
  cvs->opp->start_time = alt_read_word(fpga_time);
}

/*
Function to compute the position of the opponents
*/
void update_opponents(CtrlStruct *cvs, int res_left, int res_right){
    double x_robot = cvs->real_local->x;
    double y_robot = cvs->real_local->y;
    double angle_robot = cvs->real_local->theta;
    double dx = cos(angle_robot);
    double dy = sin(angle_robot);
    double x_pos;
    double y_pos;

    uint32_t t = alt_read_word(fpga_time);
    if((double)((t - cvs->opp->start_time)/10000) > 7){
    	cvs->opp->x[0] = 100000;
    	cvs->opp->y[0] = 100000;
    	cvs->path->scale = 10;
    	cvs->path->k_omega = 10;
    }

    if(res_left<30 && res_right<30 && res_left!=0 && res_right !=0){ // Le robot est en face
      x_pos = x_robot + dx*(res_left+res_right)/200;
      y_pos = y_robot + dy*(res_left+res_right)/200;
      if(x_pos > -0.80 && x_pos < 0.80 && y_pos > -1.30 && y_pos < 1.30 && (res_right-res_left < 5)){
    	cvs->opp->start_time = alt_read_word(fpga_time);
        cvs->opp->x[0] = x_pos;
        cvs->opp->y[0] = y_pos;
        cvs->path->scale = 5;
        cvs->path->k_omega = 25/2;
      }
      printf("x_opp : %f\n", cvs->opp->x[0]);
      printf("y_opp : %f\n", cvs->opp->y[0]);
    }
}
