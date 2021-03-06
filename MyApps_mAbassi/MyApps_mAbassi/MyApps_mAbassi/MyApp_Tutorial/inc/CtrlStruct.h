
#ifndef _CTRL_STRUCT_H_
#define _CTRL_STRUCT_H_

#include <stdlib.h>

/// Main controller structure

typedef struct SpeedRegulation SpeedRegulation;
typedef struct RobotPosition RobotPosition;
typedef struct PathPlanning PathPlanning;
typedef struct RobotRealPosition RobotRealPosition;
typedef struct Strategy Strategy;
typedef struct Opponents Opponents;

typedef struct CtrlStruct
{
	RobotPosition *rob_pos; ///< robot position
	SpeedRegulation *sp_reg; ///< speed regulation
	PathPlanning *path;
	RobotRealPosition *real_local;
	Strategy *strat;
	Opponents *opp;

	int robot_id;

} CtrlStruct;

// function prototypes
CtrlStruct* init_CtrlStruct(void);
void free_CtrlStruct(CtrlStruct *cvs);

#endif
