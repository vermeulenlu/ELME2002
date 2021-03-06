

#include "CtrlStruct.h"
#include <math.h>
#include "path_planning.h"
#include "odometry.h"
#include "PID.h"
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include "Real_localisation.h"
#include "MyApp_mAbassi.h"
#include "opponents.h"


/*
Creation of the map used to localize the robot and the obstacles
*/

void init_path(CtrlStruct *cvs){
	cvs->path->lines_size = 202;
	  cvs->path->col_size = 302;
	  cvs->path->detection_range = 30;
	  cvs->path->k_att = 100;
	  cvs->path->k_rep = 20000.0;
	  cvs->path->scale = 10;
	  cvs->path->k_omega = 10;
  cvs->path->starting_position[0] = 0;
  cvs->path->starting_position[1] = 0;
  // We fill in the map matrix with the datas of the pre-computed binary map
  // --> 1 is an obstacle, 0 is a free space

  int i;
  int j;
//  int point;
//  FILE *pf;
//  pf = fopen ("/src/Mymatrix.txt.txt", "r");
//  if (pf == NULL){
//      printf("couldnt open file\n");
//  }
//  rewind(pf);
  for(i = 0; i < cvs->path->lines_size; i++)
  {
      for(j = 0; j < cvs->path->col_size; j++){
          //point = (int)fgetc(pf)-48;
          //cvs->path->map[i][j] = point;
    	  cvs->path->map[i][j] = 0;
      }
  }

  // Computation of the fixe potential field
  // --> Each cell contains the Fx and Fy force to be applied if the robot is on it
  double obstacle[2] = {0,0};
  int d_x;
  int d_y;
  double rho;
  double F_rep_x;
  double F_rep_y;
//  for(i = 0; i < cvs->path->lines_size; i++)
//  {
//      for(j = 0; j < cvs->path->col_size; j++){
//          check_obstacle(cvs, obstacle, i, j, 0 , cvs->path->map);
//          if(obstacle[0] >= 10000){
//            cvs->path->potential[i][j][0] = 0;
//            cvs->path->potential[i][j][1] = 0;
//          }
//          else{
//            d_x = obstacle[0];
//            d_y = obstacle[1];
//            rho = sqrt(pow(d_x,2)+pow(d_y,2));
//            F_rep_x = cvs->path->k_rep*((1/rho)-(1/cvs->path->detection_range))*(1/pow(rho,2))*((d_x)/rho);
//            F_rep_y = cvs->path->k_rep*((1/rho)-(1/cvs->path->detection_range))*(1/pow(rho,2))*((d_y)/rho);
//            cvs->path->potential[i][j][0] = F_rep_x;
//            cvs->path->potential[i][j][1] = F_rep_y;
//          }
//      }
//  }

  // We fill in an txt file in order to display the potential field obtained in matlab
//  FILE* fichier = NULL;
//  double tot;
//  fichier = fopen("../../userFiles/ctrl/groups_ctrl/gr1/path_gr1/potential.txt", "w");
//  if (fichier != NULL)
//  {
//    for(i = 0; i < cvs->path->lines_size; i++)
//    {
//      for(j = 0; j < cvs->path->col_size; j++){
//          // On l'crit dans le fichier
//          tot = sqrt(pow(cvs->path->potential[i][j][0],2)+pow(cvs->path->potential[i][j][1],2));
//          fprintf(fichier, "%f ", tot);
//      }
//      fprintf(fichier, "\n");
//    }
//    fclose(fichier);
//  }
}

/*
Fonction to convert an analog position to a digital position corresponding to a cell in the map matrix
*/
void transform(CtrlStruct *cvs, double x, double y, int *buf){
  x = x + 1;
  y = y + 1.5;

  x = x/2;
  y = y/3;

  x = x*(cvs->path->lines_size-2);
  y = y*(cvs->path->col_size-2);

  buf[0] = (int)x;
  buf[1] = (int)y;
}


/*
Function used to compute the closest obstacle from one point, and to return the distance and the angle wrt the obstacles
--> used in the computation of the potential field
*/
void check_obstacle(CtrlStruct *cvs, double *obstacle, int x, int y) {

  int i;
  int j;
  int x_diff;
  int y_diff;
  double minimal_distance = 10000000000;

  double x_diff_end;
  double y_diff_end;

  for(i=x-cvs->path->detection_range; i<x+cvs->path->detection_range; i++){
    for(j=y-cvs->path->detection_range; j<y+cvs->path->detection_range; j++){
        if(i>=0 && j>=0 && (i<=cvs->path->lines_size-1) && (j<= cvs->path->col_size-1) && (cvs->path->map[i][j]==1) && (i!=x) && (j!=y)){
          x_diff = x - i;
          y_diff = y - j;
          if( minimal_distance >= sqrt(pow(x_diff,2)+pow(y_diff,2))){
            minimal_distance = sqrt(pow(x_diff,2)+pow(y_diff,2));
            x_diff_end = x_diff;
            y_diff_end = y_diff;
          }
        }
      }
    }



  if(minimal_distance <= cvs->path->detection_range){
    obstacle[0] = x_diff_end;
    obstacle[1] = y_diff_end;
  }
  else{
    obstacle[0] = 10000000000;
    obstacle[1] = 10000000000;
  }
}

/*
Function to impose a orientation to the Robot. A P controller is used
*/
double impose_orientation(CtrlStruct *cvs, double angle){
  angle = M_PI*angle/180;
  double dy = cos(cvs->real_local->theta);
  double dx = sin(cvs->real_local->theta);
  double angle2 = -atan2(dy,dx);
  double error = angle2 - angle;
  if(abs(error*180/M_PI) < 2){
    push_propDC(0, 0);
    return 1;
  }
  else{
      push_propDC(15, -15);
      return 0;
  }
}

// sens : 0 x positif
// sens : 1 x negatif
// sens : 2 y positif
// sens : 3 y negatif
int Impose_translation(CtrlStruct *cvs, int sens, double distance){
    if(sens == 0){
      return (planning(cvs, cvs->path->starting_position[0] + distance, cvs->path->starting_position[1]));
    }
    if(sens == 1){
      return (planning(cvs, cvs->path->starting_position[0] - distance, cvs->path->starting_position[1]));
    }
    if(sens == 2){
      return (planning(cvs, cvs->path->starting_position[0], cvs->path->starting_position[1]+distance));
    }
    if(sens == 3){
      return (planning(cvs, cvs->path->starting_position[0], cvs->path->starting_position[1])-distance);
    }
}

/*
Function to take the pre-computed forces to be applied to the Robot
and to act on it depending of those forces. The robot is attracted to
the position (x_goal, y_goal)
*/
double planning(CtrlStruct *cvs, double x_goal, double y_goal){
	double x = cvs->real_local->x;
	double y = cvs->real_local->y;
	double theta = cvs->real_local->theta;
	double k_lin = 0.0001;
	double k_omega = cvs->path->k_omega;
	double scale = cvs->path->scale;
	int stop = 0;
	double k_att;
	memset(cvs->path->map, 0, sizeof (cvs->path->map));
	//cvs->path->map = {{0}};

  // Robot's position and goal's position on the grid
  int position[2] = {0,0};
  transform(cvs, x, y, position);
  int position_goal[2] = {0,0};
  transform(cvs, x_goal, y_goal, position_goal);

  double F_att_x;
  double F_att_y;
  double d2 = sqrt(pow(position[0] - position_goal[0],2)+pow(position[1] - position_goal[1],2));

  // If the goal is close, don't slow down to much
  // and if you are at the correct position (+- 5 cm), stop the robot
  if(d2 < 5){
    stop = 1;
  }
  if(d2 < 50 && d2>=5){
    k_att = cvs->path->k_att*4;
  }
  if(d2 < 100 && d2>=50){
    k_att = cvs->path->k_att*2;
  }
  if(d2 > 100){
    k_att = cvs->path->k_att;
  }


  F_att_x  = -k_att*(position[0] - position_goal[0]);
  F_att_y  = -k_att*(position[1] - position_goal[1]);

  // Reppel forces
  double F_tot_x;
  double F_tot_y;
  double F_rep_x = 0; //cvs->path->potential[position[0]][position[1]][0];
  double F_rep_y = 0; //cvs->path->potential[position[0]][position[1]][1];

  double beta;
  F_tot_x = F_att_x + F_rep_x ;
  F_tot_y = F_att_y + F_rep_y ;

  // Depending of the orientation the beta angle is not the same
  if(F_tot_x > 0 && F_tot_y > 0){beta = atan(F_tot_y/F_tot_x);}
  if(F_tot_x < 0 && F_tot_y > 0){beta = M_PI - atan(-F_tot_y/F_tot_x);}
  if(F_tot_x < 0 && F_tot_y < 0){beta = -M_PI + atan(F_tot_y/F_tot_x);}
  if(F_tot_x > 0 && F_tot_y < 0){beta = atan(F_tot_y/F_tot_x);}
  if(F_tot_x == 0 && F_tot_y > 0){beta = M_PI/2;}
  if(F_tot_x == 0 && F_tot_y < 0){beta = -M_PI/2;}
  if(F_tot_x > 0 && F_tot_y == 0){beta = 0;}
  if(F_tot_x < 0 && F_tot_y == 0){beta = M_PI;}


  // Here, we decide what to do if a opponent is on our way to the goal
  // Actually, if it does, a new reppel force is added, depending of the
  // position of the opponent
  int i;
  int j;

  double F_rep_x_bis = 0;
  double F_rep_y_bis = 0;
  int position_map_opponents_1[2][2] ;


   for(int z=0; z<1; z++){
        transform(cvs, cvs->opp->x[0], cvs->opp->y[0], position_map_opponents_1[z]);
        if(position_map_opponents_1[z][0] > 0 && position_map_opponents_1[z][0] < 210 && position_map_opponents_1[z][1] > 0 && position_map_opponents_1[z][0] < 310){
          for(i=position_map_opponents_1[z][0]-cvs->path->detection_range; i<position_map_opponents_1[z][0]+cvs->path->detection_range; i++){
            for(j=position_map_opponents_1[z][1]-cvs->path->detection_range; j<position_map_opponents_1[z][1]+cvs->path->detection_range; j++){
              if(i>=0 && j>=0 && (i<=cvs->path->lines_size-1) && (j<= cvs->path->col_size-1)){
                cvs->path->map[i][j] = 1;
                }
              }
            }
         }
     }

      double obstacle[2] = {1000000,1000000};
      check_obstacle(cvs, obstacle, position[0], position[1]);

     if(obstacle[0] >= 10000){
       F_rep_x_bis  = 0;
       F_rep_y_bis  = 0;
     }
     else{
       double d_x = obstacle[0];
       double d_y = obstacle[1];
       double rho = sqrt(pow(d_x,2)+pow(d_y,2));
       // if(rho < 15){
       //   stop = 1;
       // }
       F_rep_x_bis  = 5*cvs->path->k_rep*((1/rho)-(1/(cvs->path->detection_range)))*(1/pow(rho,2))*((d_x)/rho)/2;
       F_rep_y_bis  =  5*cvs->path->k_rep*((1/rho)-(1/(cvs->path->detection_range)))*(1/pow(rho,2))*((d_y)/rho)/2;
     }

   F_tot_x = F_att_x + F_rep_x + F_rep_x_bis;
   F_tot_y = F_att_y + F_rep_y + F_rep_y_bis;

  // We recompute the beta angle because the total force might have been changed
  if(F_tot_x > 0 && F_tot_y > 0){beta = atan(F_tot_y/F_tot_x);}
  if(F_tot_x < 0 && F_tot_y > 0){beta = M_PI + atan(F_tot_y/F_tot_x);}
  if(F_tot_x < 0 && F_tot_y < 0){beta = -M_PI + atan(F_tot_y/F_tot_x);}
  if(F_tot_x > 0 && F_tot_y < 0){beta = atan(F_tot_y/F_tot_x);}
  if(F_tot_x == 0 && F_tot_y > 0){beta = M_PI/2;}
  if(F_tot_x == 0 && F_tot_y < 0){beta = -M_PI/2;}
  if(F_tot_x > 0 && F_tot_y == 0){beta = 0;}
  if(F_tot_x < 0 && F_tot_y == 0){beta = M_PI;}

  // Computation of the speed to apply the the wheel depending on the forces applied to the robot
  double alpha = beta - theta;
  double v_lin = k_lin*cos(alpha)*sqrt(pow(F_tot_x,2)+pow(F_tot_y,2));
  double omega = k_omega*sin(alpha);
  double speed_lin_wheels = (v_lin/(0.03*78/100))/scale;
  double speed_ang_wheels = (omega*0.10/(0.03*78/100))/scale;
  double speed_l = speed_lin_wheels - speed_ang_wheels;
  double speed_r = speed_lin_wheels + speed_ang_wheels;
  // If close to the goal, stop
  if(stop == 1){
    speed_regulation(cvs, 0, 0);
    return 1;
  }
  else{
    speed_regulation(cvs, speed_r, speed_l);
    return 0;
  }
}
