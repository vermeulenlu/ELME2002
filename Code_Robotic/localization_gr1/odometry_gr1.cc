

#include "namespace_ctrl.h"
#include "CtrlStruct_gr1.h"
#include <math.h>
#include "odometry_gr1.h"
#include "user_realtime.h"
#include "set_output.h"

NAMESPACE_INIT(ctrlGr1);

 ////////////////////////////////////////////////////////
 /////  INITIALISATION DES POSTIONS INITIALES DU ROBOT //
 ////////////////////////////////////////////////////////

void init_odometry(CtrlStruct *cvs){
    int id = cvs->robot_id;
    switch (id) {
      case 0:
        printf("Je rentre ici 0\n");
        cvs->rob_pos->x = -0.35;
        cvs->rob_pos->y = -1.35;
        cvs->rob_pos->theta = 90.0*M_PI/180;
        cvs->rob_pos->last_t = 0.0;
        break;
      case 1:
        printf("Je rentre ici 1\n");
        cvs->rob_pos->x = -0.05;
        cvs->rob_pos->y = 1.35;
        cvs->rob_pos->theta = -90.0*M_PI/180;
        cvs->rob_pos->last_t = 0.0;
        break;
      case 2:
        printf("Je rentre ici 2\n");
        cvs->rob_pos->x = -0.35;
        cvs->rob_pos->y = -1.35;
        cvs->rob_pos->theta = 90.0*M_PI/180;
        cvs->rob_pos->last_t = 0.0;
        break;
      case 3:
        printf("Je rentre ici 3\n");
        cvs->rob_pos->x = -0.05;
        cvs->rob_pos->y = -1.35;
        cvs->rob_pos->theta = 90.0*M_PI/180;
        cvs->rob_pos->last_t = 0.0;
        break;
      default:
    		printf("Initial position error: unknown robot ID: %d !\n", id);
    		exit(EXIT_FAILURE);
    }
}

////////////////////////////////////////////////////////
//// UPDATE DE L'ODOMETRIE DE FACON BASIQUE ////////////
////////////////////////////////////////////////////////

void update_odometry(CtrlStruct *cvs){


  double dt = cvs->inputs->t - cvs->rob_pos->last_t;
  double x = cvs->rob_pos->x;
  double y = cvs->rob_pos->y;
  double theta = cvs->rob_pos->theta;

  double speed_l = cvs->inputs->wheel_speeds[0]*0.03; // vl
  double speed_r = cvs->inputs->wheel_speeds[1]*0.03; // vr

  double delta_sl = speed_l*dt; // delta_sl
  double delta_sr = speed_r*dt; // delta_sr

  double delta_s = (delta_sl+delta_sr)/2; // delta_s
  double delta_theta = (delta_sl-delta_sr)/(2*0.1125); // delta_theta

  double new_x = x + delta_s*cos(theta+(delta_theta/2));
  double new_y = y + delta_s*sin(theta+(delta_theta/2));
  double new_theta = theta+delta_theta;
  // if(new_theta >= 2*M_PI){
  //   new_theta = new_theta - 2*M_PI;
  // }
  // if(new_theta <= -2*M_PI){
  //   new_theta = new_theta + 2*M_PI;
  // }

  //printf("x : %f\n", new_x);
  //printf("y : %f\n", new_y);
  //printf("theta : %f\n", new_theta*180/M_PI);

  // update
  cvs->rob_pos->x = new_x;
  cvs->rob_pos->y = new_y;
  cvs->rob_pos->theta = new_theta;
  cvs->rob_pos->last_t = cvs->inputs->t;

  set_plot(cvs->rob_pos->x, "x_real");
  set_plot(cvs->rob_pos->y, "y_real");
  //set_plot(  cvs->rob_pos->theta, "angle_real");
}

NAMESPACE_CLOSE();
