

#include "namespace_ctrl.h"
#include "CtrlStruct_gr1.h"
#include <math.h>
#include "localisation_gr1.h"
#include "user_realtime.h"
#include "odometry_gr1.h"

NAMESPACE_INIT(ctrlGr1);

 ////////////////////////////////////////////////////////
 /////  INITIALISATION DES POSTIONS INITIALES DU ROBOT //
 ////////////////////////////////////////////////////////

void init_localisation(CtrlStruct *cvs){
  double x = cvs->rob_pos->x;
  double y = cvs->rob_pos->y;
  double theta = cvs->rob_pos->theta;
  cvs->local->x_estimated = x;
  cvs->local->y_estimated = y;
  cvs->local->theta_estimated = theta;
  cvs->local->first_turn = 1;
  double delta_X;
  double delta_Y;

  // first beacon
  delta_X = (x+1.5)-(0.0+1.5);
  delta_Y = (y+1.5)-(-1.562+1.5);
  double angle_1 = atan2(delta_X,delta_Y);
  double distance_1 = sqrt(pow(delta_X,2)+pow(delta_Y,2));
  cvs->local->angle_1 = -angle_1+M_PI;
  cvs->local->distance_1 = distance_1;

  // second beacon
  delta_X = (x+1.5)-(-1.062+1.5);
  delta_Y = (y+1.5)-(1.562+1.5);
  double angle_2 = atan2(delta_X,delta_Y);
  double distance_2 = sqrt(pow(delta_X,2)+pow(delta_Y,2));
  cvs->local->angle_2 = -angle_2+M_PI;
  cvs->local->distance_2 = distance_2;

  // third beacon
  delta_X = (x+1.5)-(1.062+1.5);
  delta_Y = (y+1.5)-(1.562+1.5);
  double angle_3 = atan2(delta_X,delta_Y);
  double distance_3 = sqrt(pow(delta_X,2)+pow(delta_Y,2));
  cvs->local->angle_3 = -angle_3+M_PI;
  cvs->local->distance_3 = distance_3;

  printf("angle 1 = %f, distance 1 = %f\n",(cvs->local->angle_1)*180/M_PI, distance_1);
  printf("angle 2 = %f, distance 2 = %f\n",(cvs->local->angle_2)*180/M_PI, distance_2);
  printf("angle 3 = %f, distance 3 = %f\n",(cvs->local->angle_3)*180/M_PI, distance_3);

}

////////////////////////////////////////////////////////
//// UPDATE DE L'ODOMETRIE DE FACON BASIQUE ////////////
////////////////////////////////////////////////////////

void update_localisation(CtrlStruct *cvs){
  double x = cvs->local->x_estimated;
  double y = cvs->local->y_estimated;
  double theta = cvs->local->theta_estimated;
  int index_last_fall = cvs->inputs->falling_index_fixed;
  int index_last_fall_1;
  int index_last_fall_2;

    if(index_last_fall == 0){
      index_last_fall_1 = 9;
      index_last_fall_2 = 8;
    }
    else if(index_last_fall == 1){
      index_last_fall_1 = 0;
      index_last_fall_2 = 9;
    }
    else{
      index_last_fall_1 = index_last_fall - 1;
      index_last_fall_2 = index_last_fall - 2;
    }

    int min = (cvs->inputs->last_falling_fixed[index_last_fall]+M_PI) - (cvs->local->angle_1);
    int min_1 = (cvs->inputs->last_falling_fixed[index_last_fall_1]+M_PI) - (cvs->local->angle_1);
    int min_2 = (cvs->inputs->last_falling_fixed[index_last_fall_2]+M_PI) - (cvs->local->angle_1);

    // angles_beacons pour les 3 balises
    int angles_beacons[3] = {0,0,0};

    if(min<min_1){
      if(min<min_2){
        angles_beacons[0] = index_last_fall;
        angles_beacons[1] = index_last_fall_1;
        angles_beacons[2] = index_last_fall_2;
      }
      else{
        angles_beacons[0] = index_last_fall_2;
        angles_beacons[1] = index_last_fall;
        angles_beacons[2] = index_last_fall_1;
      }
    }
    else {
      if(min_1<min_2){
        angles_beacons[0] = index_last_fall_1;
        angles_beacons[1] = index_last_fall_2;
        angles_beacons[2] = index_last_fall;
      }
      else{
        angles_beacons[0] = index_last_fall_2;
        angles_beacons[1] = index_last_fall;
        angles_beacons[2] = index_last_fall_1;
      }
    }
    // On a mnt quel edge correspond à quelle balise
    double theta_1 = (cvs->inputs->last_falling_fixed[angles_beacons[0]]+M_PI + (cvs->inputs->last_rising_fixed[angles_beacons[0]]+M_PI))/2;
    double theta_2 = (cvs->inputs->last_falling_fixed[angles_beacons[1]]+M_PI + (cvs->inputs->last_rising_fixed[angles_beacons[1]]+M_PI))/2;
    double theta_3 = (cvs->inputs->last_falling_fixed[angles_beacons[2]]+M_PI + (cvs->inputs->last_rising_fixed[angles_beacons[2]]+M_PI))/2;

    printf("theta1 = %f\n",theta_1*180/M_PI);
    printf("theta2 = %f\n",theta_2*180/M_PI);
    printf("theta3 = %f\n",theta_3*180/M_PI);
  
}

NAMESPACE_CLOSE();
