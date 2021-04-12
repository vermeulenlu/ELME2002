

#include "namespace_ctrl.h"
#include "CtrlStruct_gr1.h"
#include <math.h>
#include "path_planning_gr1.h"
#include "user_realtime.h"
#include "odometry_gr1.h"
#include "speed_regulation_gr1.h"
#include <unistd.h>
#include <stdio.h>
#include <limits.h>

NAMESPACE_INIT(ctrlGr1);


////////////////////////////////////////////////////////
/////  CREATION DE LA MAP ET DES OBSTACLES ////////// //
////////////////////////////////////////////////////////

void init_path(CtrlStruct *cvs){
  cvs->path->lines_size = 202;
  cvs->path->col_size = 302;
  cvs->path->detection_range = 70;
  cvs->path->k_att = 0.6;
  cvs->path->k_rep = 1200.0;
  // On rempli avec les données de l'image binaire
  int i;
  int j;
  int point;
  FILE *pf;
  pf = fopen ("../../userFiles/ctrl/groups_ctrl/gr1/path_gr1/Mymatrix.txt", "r");
  if (pf == NULL){
      printf("couldnt open file\n");
  }
  rewind(pf);
  for(i = 0; i < cvs->path->lines_size; i++)
  {
      for(j = 0; j < cvs->path->col_size; j++){
          point = int(fgetc(pf))-48;
          cvs->path->map[i][j] = point;
      }
  }
  // CHAQUE OBSTACLE EST REPRESENTE PAR UN 0 , SINON UN 1
  for(i=0; i<cvs->path->lines_size; i++){
    cvs->path->map[i][0] = 1;
    cvs->path->map[i][cvs->path->col_size-1] = 1;
  }
  for(i=0; i<cvs->path->col_size; i++){
    cvs->path->map[0][i] = 1;
    cvs->path->map[cvs->path->lines_size-1][i] = 1;
  }
  // MAP de potentiel
  double obstacle[2] = {0,0};
  int d_x;
  int d_y;
  double rho;
  double F_rep_x;
  double F_rep_y;
  for(i = 0; i < cvs->path->lines_size; i++)
  {
      for(j = 0; j < cvs->path->col_size; j++){
          check_obstacle(cvs, obstacle, i, j);
          if(obstacle[0] >= 10000){
            cvs->path->potential[i][j][0] = 0;
            cvs->path->potential[i][j][1] = 0;
          }
          else{
            d_x = obstacle[0];
            d_y = obstacle[1];
            rho = sqrt(pow(d_x,2)+pow(d_y,2));
            F_rep_x = cvs->path->k_rep*((1/rho)-(1/cvs->path->detection_range))*(1/pow(rho,2))*((d_x)/rho);
            F_rep_y = cvs->path->k_rep*((1/rho)-(1/cvs->path->detection_range))*(1/pow(rho,2))*((d_y)/rho);
            cvs->path->potential[i][j][0] = F_rep_x;
            cvs->path->potential[i][j][1] = F_rep_y;
          }
      }
  }

  FILE* fichier = NULL;
  double tot;

  fichier = fopen("../../userFiles/ctrl/groups_ctrl/gr1/path_gr1/potential.txt", "w");

  if (fichier != NULL)
  {
    for(i = 0; i < cvs->path->lines_size; i++)
    {
      for(j = 0; j < cvs->path->col_size; j++){
          // On l'écrit dans le fichier
          tot = sqrt(pow(cvs->path->potential[i][j][0],2)+pow(cvs->path->potential[i][j][1],2));
          fprintf(fichier, "%f ", tot);
      }
      fprintf(fichier, "\n");
    }
    fclose(fichier);
  }




}

////////////////////////////////////////////////////////////////////////
/////  FONCTION POUR PASSER D'UNE COORDONNEE à UNE ENTREE DANS LA MAP //
////////////////////////////////////////////////////////////////////////
void transform(CtrlStruct *cvs, double x, double y, int *buf){
  x = x + 1;
  y = y + 1.5;

  x = x/2;
  y = y/3;

  x = x*(cvs->path->lines_size-2);
  y = y*(cvs->path->col_size-2);

  buf[0] = int(x);
  buf[1] = int(y);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////  CHECK DANS UNE RANGE SI IL Y A DES OBSTACLES ET RENVOYE LA DISTANCE MINIMALE ET L'ANGLE à LAQUELLE IL SE TROUVE //
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void check_obstacle(CtrlStruct *cvs, double *obstacle, int x, int y){

  int i;
  int j;
  int x_diff;
  int y_diff;
  double minimal_distance = 10000000000;
  int x_diff_final;
  int y_diff_final;
  for(i=x-cvs->path->detection_range; i<x+cvs->path->detection_range; i++){
    for(j=y-cvs->path->detection_range; j<y+cvs->path->detection_range; j++){
      if(i>=0 && j>=0 && (i<=cvs->path->lines_size-1) && (j<= cvs->path->col_size-1) && (cvs->path->map[i][j]==1) && (i!=x) && (j!=y)){
        x_diff = x - i;
        y_diff = y - j;
        if( minimal_distance >= sqrt(pow(x_diff,2)+pow(y_diff,2))){
          minimal_distance = sqrt(pow(x_diff,2)+pow(y_diff,2));
          x_diff_final = x_diff;
          y_diff_final = y_diff;
        }
      }
    }
  }
  if(minimal_distance <= cvs->path->detection_range){
    obstacle[0] = x_diff_final;
    obstacle[1] = y_diff_final;
  }
  else{
    obstacle[0] = 10000000000;
    obstacle[1] = 10000000000;
  }
}

////////////////////////////////////
///// IMPLEMENTATION DU GRADIENT  //
////////////////////////////////////

void planning(CtrlStruct *cvs, double x_goal, double y_goal){
  double x = cvs->rob_pos->x;
  double y = cvs->rob_pos->y;
  double theta = cvs->rob_pos->theta;
  double k_att = cvs->path->k_att;
  double k_rep = cvs->path->k_rep;
  double k_lin = 0.005;
  double k_omega = 4.0;
  double scale = 0.9;
  // Robot's position and goal's position on the grid
  int position[2] = {0};
  transform(cvs, x, y, position);
  int position_goal[2] = {0};
  transform(cvs, x_goal, y_goal, position_goal);

  double F_att_x;
  double F_att_y;
  F_att_x  = -k_att*(position[0] - position_goal[0]);
  F_att_y  = -k_att*(position[1] - position_goal[1]);
  // Attractive force
  if(F_att_x >= 40){F_att_x  = 40;}
  if(F_att_x <= -40){F_att_x  = -40;}
  if(F_att_y >= 40){F_att_y  = 40;}
  if(F_att_y <= -40){F_att_y  = -40;}

  // Reppel force
  double F_tot_x;
  double F_tot_y;
  double F_rep_x = cvs->path->potential[position[0]][position[1]][0];
  double F_rep_y = cvs->path->potential[position[0]][position[1]][1];
  double beta;

  F_tot_x = F_att_x + F_rep_x;
  F_tot_y = F_att_y + F_rep_y;

  if(F_tot_x > 0 && F_tot_y > 0){beta = atan(F_tot_y/F_tot_x);}
  if(F_tot_x < 0 && F_tot_y > 0){beta = M_PI - atan(-F_tot_y/F_tot_x);}
  if(F_tot_x < 0 && F_tot_y < 0){beta = -M_PI + atan(F_tot_y/F_tot_x);}
  if(F_tot_x > 0 && F_tot_y < 0){beta = atan(F_tot_y/F_tot_x);}
  if(F_tot_x == 0 && F_tot_y > 0){beta = M_PI/2;}
  if(F_tot_x == 0 && F_tot_y < 0){beta = -M_PI/2;}

  printf("F_tot_x = %f\n",F_tot_x );
  printf("F_tot_y = %f\n",F_tot_y );
  // printf("\n");
  double alpha = beta - theta;
  double v_lin = k_lin*cos(alpha)*sqrt(pow(F_tot_x,2)+pow(F_tot_y,2));
  double omega = k_omega*sin(alpha);
  // printf("alpha = %f\n", alpha*180/M_PI);
  // printf("\n");

  double speed_lin_wheels = (v_lin/0.03)/scale;
  double speed_ang_wheels = (omega*0.1125/0.03)/scale;

  // printf("speed lin : %f\n", speed_lin_wheels);
  // printf("speed ang : %f\n", speed_ang_wheels);

  double speed_l = speed_lin_wheels - speed_ang_wheels;
  double speed_r = speed_lin_wheels + speed_ang_wheels;

  speed_regulation(cvs, speed_r, speed_l);
}


NAMESPACE_CLOSE();
