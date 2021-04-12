/*!
 * \file CtrlStruct_gr2.h
 * \brief Controller main structure
 */

#ifndef _CTRL_STRUCT_GR1_H_
#define _CTRL_STRUCT_GR1_H_

#include "ctrl_io.h"
#include "namespace_ctrl.h"
#include <stdlib.h>

NAMESPACE_INIT(ctrlGr1);

/// Main controller structure

typedef struct RobotPosition RobotPosition;
typedef struct SpeedRegulation SpeedRegulation;
typedef struct RobotCalibration RobotCalibration;
typedef struct PathPlanning PathPlanning;
typedef struct Strategy Strategy;
typedef	struct Localisation Localisation;

typedef struct CtrlStruct
{
	CtrlIn *inputs;   ///< controller inputs
	CtrlOut *outputs; ///< controller outputs
	CtrlOut *py_outputs; ///< python controller outputs

	RobotPosition *rob_pos; ///< robot position
	SpeedRegulation *sp_reg; ///< speed regulation
	RobotCalibration *calib; ///< calibration
	PathPlanning *path; ///< path-planning
	Strategy *strat; ///< strategy
	Localisation *local;

	int main_state; ///< main state
	int robot_id;   ///< ID of the robot
	int team_id;    ///< ID of the team

} CtrlStruct;

// function prototypes
CtrlStruct* init_CtrlStruct(CtrlIn *inputs, CtrlOut *outputs, CtrlOut *py_outputs);
void free_CtrlStruct(CtrlStruct *cvs);

NAMESPACE_CLOSE();

#endif
