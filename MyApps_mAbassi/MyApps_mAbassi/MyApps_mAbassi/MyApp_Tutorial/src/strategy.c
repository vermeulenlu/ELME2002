#include "MyApp_mAbassi.h"

#include "mAbassi.h"          /* MUST include "SAL.H" and not uAbassi.h        */
#include "Platform.h"         /* Everything about the target platform is here  */
#include "HWinfo.h"           /* Everything about the target hardware is here  */
#include "dw_i2c.h"
#include "dw_uart.h"
#include "MyDriver_CAN.h"
#include "PID.h"
#include "CtrlStruct.h"
#include <stdio.h>
#include <unistd.h>
#include "odometry.h"
#include "path_planning.h"
#include "Real_localisation.h"
#include "Dynamixels.h"
#include "strategy.h"
#include "MyApp_I2C.h"


#define FINISH 0
#define STOP 1
#define TEST 2
#define ORIENTATION 3
#define RECUL 4
#define CONTOUR 7
#define GO_QG 8
#define GO_BASE 9
#define GO_FLAGS 10
#define CALIB_FLAGS_1 11
#define CALIB_FLAGS_2 12
#define PUT_FLAGS 13
#define ON_THE_WALL 14
#define AJUST_1 15
#define AJUST_2 16
#define FORWARD_MOVE 17
#define END 18
#define FIND_LINE 19
#define TEST2 20
#define TEST3 21

/*
Initialisation of the structure
*/
void init_state(CtrlStruct *cvs){
  cvs->strat->state = TEST;
  cvs->strat->next_state = TEST;
  cvs->strat->t_start = 0;
  cvs->strat->cmd_orient = 0;
}

/*
(0,0) is at the center
x-dir is the short length
y-dir is the long length
x positive is to the windshield side
x-negative is to the girouette side
y-positive is on the opposite side of where the robot starts
y-negative is where we start
*/


void update_state(CtrlStruct *cvs)
{
  int res;
  double dt;
  switch (cvs->strat->state)
  {
    case STOP:
    	deploy_flag();
    	push_propDC(0,0);
    	break;

    case FINISH:
        if(cvs->robot_id == 0 || cvs->robot_id == 1){
          res = planning(cvs, -0.25 , 1.35);
        }
        else{
          res = planning(cvs, -0.35 , -1.45);
        }
        if(res == 1){
          cvs->strat->cmd_orient = 0;
          cvs->strat->next_state = STOP;
          cvs->strat->state = ORIENTATION;
          break;
        }
        break;
    case TEST:
        if(cvs->robot_id == 0 || cvs->robot_id == 1){
          res = planning(cvs, 0.8 , -1.1);
        }
        else{
          res = planning(cvs, 0.7 , 1.1);
        }
        if(res == 1){
          cvs->strat->state = ORIENTATION;
          cvs->strat->next_state = TEST2;
          cvs->strat->cmd_orient = 178;
          deploy_shovels();
          cvs->strat->t_start = alt_read_word(fpga_time);
          break;
        }
        break;
    case TEST2:
        if(cvs->robot_id == 0 || cvs->robot_id == 1){
          res = planning(cvs, 0.4 , 0.3);
        }
        else{
          res = planning(cvs, 0.4 , -0.3);
        }
        if(res == 1){
          cvs->strat->state = GO_BASE;
          break;
        }
        break;
    case TEST3:
        if(cvs->robot_id == 0 || cvs->robot_id == 1){
          res = planning(cvs, -0.6 , 1.35);
        }
        else{
          res = planning(cvs, -0.6 , -1.35);
        }
        if(res == 1){
        	cvs->strat->state = ORIENTATION;
        	cvs->strat->next_state = STOP;
        	cvs->strat->cmd_orient = 0;
          break;
        }
        break;
    case RECUL:
        speed_regulation(cvs, -0.2/0.03, -0.2/0.03);
        dt = (alt_read_word(fpga_time) - cvs->strat->t_start)/10000;
        if(dt > 1){
          reploy_shovels();
          push_propDC(0.0, 0.0);
          wait(cvs,1000);
          cvs->strat->state = cvs->strat->next_state;
          break;
        }
        break;
    case CONTOUR:	//going to the girouette
        res = planning(cvs, -0.7, 0.2);
        if(res == 1){
          deploy_shovels();
          cvs->strat->state = GO_BASE;
          break;
        }
        break;
    case GO_BASE:	//go to small base to drop the cups, then go backwards
        res = planning(cvs, 0.9, -0.3);
        if(res == 1){
          cvs->strat->state = RECUL;
          cvs->strat->next_state = TEST3;
          cvs->strat->t_start = alt_read_word(fpga_time);
          break;
        }
        break;
    case ORIENTATION:
        if(cvs->robot_id == 0 || cvs->robot_id == 1){
          res = impose_orientation(cvs, cvs->strat->cmd_orient);
        }
        else{
          res = impose_orientation(cvs, cvs->strat->cmd_orient);
        }
        if(res == 1){
          cvs->strat->t_start = alt_read_word(fpga_time);
          cvs->strat->state = cvs->strat->next_state;
          wait(cvs, 500);
          break;
        }
        break;
    case GO_FLAGS:
        res = planning(cvs, 0.8, -1.3);
        if(res == 1){
          cvs->strat->state = CALIB_FLAGS_1;
          break;
        }
        break;
    case CALIB_FLAGS_1:
        res = impose_orientation(cvs, 90);	//turn 90 degrees clockwise
        if(res == 1){
            cvs->strat->t_start = alt_read_word(fpga_time);
            cvs->strat->state = ON_THE_WALL;
            break;
        }
        break;
    case ON_THE_WALL:	//updates position once situated against the wall after backing up to the wall
        dt = (alt_read_word(fpga_time) - cvs->strat->t_start)/10000;
        push_propDC(-15,-15);
        if(dt > 3){
          push_propDC(0,0);
          wait(cvs, 1000);
          cvs->path->starting_position[0] = cvs->real_local->x;
          cvs->path->starting_position[1] = cvs->real_local->y;
          cvs->strat->state = CALIB_FLAGS_2;
          break;
        }
        break;
    case CALIB_FLAGS_2:
        res = Impose_translation(cvs,1,0.15);	//move in x-negative direction for 15cm
        if(res == 1){
          cvs->strat->state = ORIENTATION;
          cvs->strat->cmd_orient = 178;
          cvs->strat->next_state = AJUST_1;
          cvs->strat->t_start = alt_read_word(fpga_time);
          break;
        }
        break;
    case AJUST_1:
        dt = (alt_read_word(fpga_time) - cvs->strat->t_start)/10000;
        push_propDC(10,20);	//pushes against the wall at an angle
        if(dt>3){
            cvs->strat->state = AJUST_2;
            cvs->strat->t_start = alt_read_word(fpga_time);
            break;
        }
        break;
    case AJUST_2:
        dt = (alt_read_word(fpga_time) - cvs->strat->t_start)/10000;
        push_propDC(20,10);	//pushes against the wall at an angle
        if(dt>3){
            deploy_right_arm();
            deploy_left_arm();
            cvs->strat->state = PUT_FLAGS;
            wait(cvs,500);
            cvs->strat->t_start = alt_read_word(fpga_time);
            break;
        }
        break;
    case PUT_FLAGS:
        dt = (alt_read_word(fpga_time) - cvs->strat->t_start)/10000;
        speed_regulation(cvs, -0.15/0.03, -0.15/0.03);
        if(dt > 5){
          reploy_right_arm();
          reploy_left_arm();
          push_propDC(0,0);
          wait(cvs,500);
          cvs->strat->state = FINISH;
          break;
        }
        break;
  	default:
  		//printf("Error: unknown state : %d !\n", cvs->state);
  		exit(EXIT_FAILURE);
  }

}


/*//Works for microswitch
void init_state(CtrlStruct *cvs, ALT_AVALON_I2C_DEV_t *i2c_dev){
  cvs->strat->state = ON_THE_WALL;
  cvs->strat->next_state = FORWARD_MOVE;
  cvs->strat->t_start = 0;
  cvs->strat->cmd_orient = 0;
}
void update_state(CtrlStruct *cvs, ALT_AVALON_I2C_DEV_t *i2c_dev)
{
  int res;
  double dt;
  switch (cvs->strat->state)
  {
    case ON_THE_WALL:	//updates position once situated against the wall after backing up to the wall
        //dt = (alt_read_word(fpga_time) - cvs->strat->t_start)/10000;
        push_propDC(-15,-15);
		//read switch value,
		//if backswitches are ON, go to next_state
		int stateMicro = 0;

		//ALT_AVALON_MICROSWITCH_NB_t microswitch = BACK_RIGHT_MICROSWITCH;
		//stateMicro = read_state_microswitch(i2c_dev, microswitch);
		stateMicro = read_state_microswitch(i2c_dev, BACK_RIGHT_MICROSWITCH);
		while(stateMicro != 1) {
			//stateMicro = read_state_microswitch(i2c_dev, microswitch);
			stateMicro = read_state_microswitch(i2c_dev, BACK_RIGHT_MICROSWITCH);
		}

		push_propDC(0,0);
        wait(cvs, 1000);
        cvs->path->starting_position[0] = cvs->real_local->x;
        cvs->path->starting_position[1] = cvs->real_local->y;
        //cvs->strat->state = FORWARD_MOVE;
        cvs->strat->state = cvs->strat->next_state;;
        break;

	case FORWARD_MOVE:
		MTXLOCK_STDIO();
		printf("In FORWARD_MOVE \n");
		MTXUNLOCK_STDIO();
        res = Impose_translation(cvs, 2, 0.30, i2c_dev);	//move in x-negative direction for 20cm
		MTXLOCK_STDIO();
		printf("After IMPOSE_TRANSLATION \n");
		MTXUNLOCK_STDIO();
		if(res == 1) {
			push_propDC(0,0);
		}
  	default:
  		//printf("Error: unknown state : %d !\n", cvs->state);
  		exit(EXIT_FAILURE);
  }
}*/

//
////For sonar
//void init_state(CtrlStruct *cvs, ALT_AVALON_I2C_DEV_t *i2c_dev){
//  cvs->strat->state = TEST;
//  cvs->strat->next_state = TEST;
//  cvs->strat->t_start = 0;
//  cvs->strat->cmd_orient = 0;
//}
//void update_state(CtrlStruct *cvs, ALT_AVALON_I2C_DEV_t *i2c_dev)
//{
//  int res;
//  double dt;
//  switch (cvs->strat->state)
//  {
//    case TEST:
//        if(cvs->robot_id == 0 || cvs->robot_id == 1){
//          //res = planning(cvs, 0.8 , -1.1, i2c_dev);
//		  MTXLOCK_STDIO();
//		  printf("WRONG ID !\n");
//		  MTXUNLOCK_STDIO();
//		  res = Impose_translation(cvs, 2, 0.20, i2c_dev);
//        }
//        else{
//		  //res = planning(cvs, -0.2 , -0.4, i2c_dev);		//just move a bit forwards
//		  res = Impose_translation(cvs, 2, 0.50, i2c_dev);
//		  //push_propDC(15,15);
//		  if ((res == 2) || (res == 3)) {
//			  push_propDC(0,0);
//			  MTXLOCK_STDIO();
//			  printf("OBSTACLE !!!\n");
//			  MTXUNLOCK_STDIO();
//			  cvs->strat->state = RECUL;
//		  }
//		  if (res == 1) {
//			  push_propDC(0,0);
//			  //cvs->strat->next_state = END;
//			  cvs->strat->state = END;
//
//		  }
//		  break;
//        }
//        break;
//    case RECUL:
//        speed_regulation(cvs, -0.2/0.03, -0.2/0.03);
//        dt = (alt_read_word(fpga_time) - cvs->strat->t_start)/10000;
//        if(dt > 1){
//          //reploy_shovels();
//          push_propDC(0.0, 0.0);
//          wait(cvs,1000);
//          cvs->strat->state = END;
//          break;
//        }
//        break;
//    case END:
//        push_propDC(0, 0);
//
//  	default:
//  		//printf("Error: unknown state : %d !\n", cvs->state);
//  		exit(EXIT_FAILURE);
//  }
//}
//
//
////For color sensor
///*void init_state(CtrlStruct *cvs, ALT_AVALON_I2C_DEV_t *i2c_dev){
//  cvs->strat->state = TEST;
//  cvs->strat->next_state = TEST;
//  cvs->strat->t_start = 0;
//  cvs->strat->cmd_orient = 0;
//}
//void update_state(CtrlStruct *cvs, ALT_AVALON_I2C_DEV_t *i2c_dev)
//{
//  int res;
//  double dt;
//  switch (cvs->strat->state)
//  {
//    case FIND_LINE:
//		//follow black and white line
//
//		cvs->strat->t_start = alt_read_word(fpga_time);
//		speed_regulation(cvs, 0.2/0.03, 0.2/0.03);
//        dt = (alt_read_word(fpga_time) - cvs->strat->t_start)/10000;
//
//
//		if(dt > 1){
//          reploy_shovels();
//          push_propDC(0.0, 0.0);
//          wait(cvs,1000);
//          cvs->strat->state = END;
//          break;
//        }
//        break;
//
//        else{
//			//WONT WORK ! IT WILL STOP planning from working
//			//PUT THE SONAR DETECTION IN planning fct or run in other CORE !
//		  res = planning(cvs, -0.1 , -0.8, i2c_dev);		//just move a bit forwards
//		  //res = Impose_translation(cvs, 2, 0.10, i2c_dev);
//		  //ALT_AVALON_SONAR_NB_t sonar = LEFT_SONAR;
//		  int range = range_in_cm(i2c_dev, LEFT_SONAR);
//		  while (range == 0 || range > 30) {
//			  if(res == 1){
//					push_propDC(0,0);
//					cvs->strat->state = END;
//					//cvs->strat->t_start = alt_read_word(fpga_time);
//					break;
//			  }
//			  range = range_in_cm(i2c_dev, sonar);
//		  }
//		  MTXLOCK_STDIO();
//		  printf("OBSTACLE !!!\n");
//		  MTXUNLOCK_STDIO();
//		  cvs->strat->next_state = RECUL;
//		  break;
//        }
//        break;
//    case RECUL:
//        speed_regulation(cvs, -0.2/0.03, -0.2/0.03);
//        dt = (alt_read_word(fpga_time) - cvs->strat->t_start)/10000;
//        if(dt > 1){
//          reploy_shovels();
//          push_propDC(0.0, 0.0);
//          wait(cvs,1000);
//          cvs->strat->state = END;
//          break;
//        }
//        break;
//    case END:
//        push_propDC(0, 0);
//
//  	default:
//  		//printf("Error: unknown state : %d !\n", cvs->state);
//  		exit(EXIT_FAILURE);
//  }
//}*/
