
#ifndef _PATH_PLANNING_GR1_H_
#define _PATH_PLANNING_GR1_H_

#include "namespace_ctrl.h"
#include "CtrlStruct_gr1.h"

NAMESPACE_INIT(ctrlGr1);

typedef struct PathPlanning{
    int map[202][302];
    double potential [202][302][2];
    int lines_size;
    int col_size;
    int detection_range;
    double k_att;
    double k_rep;
}PathPlanning;

void init_path(CtrlStruct *cvs);
void transform(CtrlStruct *cvs, double x, double y, int* buf);
void check_obstacle(CtrlStruct *cvs, double *obstacle, int x, int y);
void planning(CtrlStruct *cvs, double x, double y);
NAMESPACE_CLOSE();

#endif
