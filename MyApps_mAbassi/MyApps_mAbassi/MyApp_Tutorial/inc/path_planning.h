#ifndef _PATH_PLANNING_H_
#define _PATH_PLANNING_H_

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

typedef struct PathPlanning{
    int map[202][302];
    double potential [202][302][2];
    int lines_size;
    int col_size;
    int detection_range;
    double k_att;
    double k_rep;
    double starting_position[2];
}PathPlanning;

void init_path(CtrlStruct *cvs);
void transform(CtrlStruct *cvs, double x, double y, int* buf);
void check_obstacle(CtrlStruct *cvs, double *obstacle, int x, int y, int v, int map[202][302]);
double planning(CtrlStruct *cvs, double x, double y);
double impose_orientation(CtrlStruct *cvs, double angle);
int Impose_translation(CtrlStruct *cvs, int sens, double distance);

#endif
