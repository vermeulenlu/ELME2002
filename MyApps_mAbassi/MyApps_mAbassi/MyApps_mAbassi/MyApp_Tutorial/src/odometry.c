
#include "CtrlStruct.h"
#include "math.h"
#include "odometry.h"
#include "Real_localisation.h"
#include "MyApp_mAbassi.h"

 ////////////////////////////////////////////////////////
 /////  INITIALISATION DES POSTIONS INITIALES DU ROBOT //
 ////////////////////////////////////////////////////////

 /*
 Initialisation of the structure
 */
void init_odometry(CtrlStruct *cvs){
    int id = cvs->robot_id;
    switch (id) {
      case 0:
        cvs->rob_pos->x = -0.35;
        cvs->rob_pos->y = 1.35;
        cvs->rob_pos->theta = -90.0*M_PI/180;
        cvs->rob_pos->last_t = alt_read_word(fpga_time);
        break;
      case 1:
        cvs->rob_pos->x = -0.05;
        cvs->rob_pos->y = 1.35;
        cvs->rob_pos->theta = -90.0*M_PI/180;
        cvs->rob_pos->last_t = alt_read_word(fpga_time);
        break;
      case 2:
        cvs->rob_pos->x = -0.35;
        cvs->rob_pos->y = -1.25;
        cvs->rob_pos->theta = 90.0*M_PI/180;
        cvs->rob_pos->last_t = alt_read_word(fpga_time);
        break;
      case 3:
        cvs->rob_pos->x = -0.05;
        cvs->rob_pos->y = -1.25;
        cvs->rob_pos->theta = 90.0*M_PI/180;
        cvs->rob_pos->last_t = alt_read_word(fpga_time);
        break;
      default:
    		printf("Initial position error: unknown robot ID: %d !\n", id);
    		exit(EXIT_FAILURE);
    }
}

/*
Odometry update
*/
void update_odometry(CtrlStruct *cvs){

  uint32_t t = alt_read_word(fpga_time);
  double dt = (t - cvs->rob_pos->last_t)/10000;
  double x = cvs->real_local->x;
  double y = cvs->real_local->y;
  double theta = cvs->real_local->theta;

  int vlmes1 = (int)alt_read_word(fpga_speed_left);
  int vrmes1 = (int)alt_read_word(fpga_speed_right);

  int sens_Left = (vlmes1 >> 15);
  int sens_Right = (vrmes1 >> 15);

  if(sens_Left == 1){
    vlmes1 = (vlmes1 & 0b111111111111111);
  }
  else{
    vlmes1 = -(vlmes1 & 0b111111111111111);
  }

  if(sens_Right == 0){
    vrmes1 = (vrmes1 & 0b111111111111111);
  }
  else{
    vrmes1 = -(vrmes1 & 0b111111111111111);
  }

  double vlmes = vlmes1*2*M_PI/(4*2048*0.1);
  double vrmes = vrmes1*2*M_PI/(4*2048*0.1);


  double speed_l = vlmes*0.03*78/100; // vl
  double speed_r = vrmes*0.03*78/100; // vr

  double delta_sl = speed_l*dt; // delta_sl
  double delta_sr = speed_r*dt; // delta_sr

  double delta_s = (delta_sl+delta_sr)/2; // delta_s
  double delta_theta = (delta_sr-delta_sl)/(2*0.1); // delta_theta

  double new_x = x + delta_s*cos(theta+(delta_theta/2));
  double new_y = y + delta_s*sin(theta+(delta_theta/2));
  double new_theta = theta+delta_theta;

  // update
  cvs->rob_pos->x = new_x;
  cvs->rob_pos->y = new_y;
  cvs->rob_pos->theta = new_theta;
  cvs->rob_pos->last_t = t;

}
