#include "CtrlStruct.h"
#include "PID.h"
#include "odometry.h"
#include "path_planning.h"
#include "Real_localisation.h"
#include "strategy.h"
#include "opponents.h"


/*! \brief initialize the controller structure
 *
 * \param[in] inputs inputs of the controller
 * \param[in] outputs outputs of the controller
 * \return controller main structure
 */
CtrlStruct* init_CtrlStruct(void)
{
	int i;
	CtrlStruct *cvs;

	cvs = (CtrlStruct*) malloc(sizeof(CtrlStruct));

	// speed regulation
	cvs->sp_reg = (SpeedRegulation*) malloc(sizeof(SpeedRegulation));
	cvs->rob_pos = (RobotPosition*) malloc(sizeof(RobotPosition));
	cvs->path = (PathPlanning*) malloc(sizeof(PathPlanning));
	cvs->real_local = (RobotRealPosition*) malloc(sizeof(RobotRealPosition));
	cvs->strat = (Strategy*) malloc(sizeof(Strategy));
	cvs->opp = (Opponents*) malloc(sizeof(Opponents));

	cvs->robot_id = 2;

	return cvs;
}

/*! \brief release controller main structure memory
 *
 * \param[in] cvs controller main structure
 */
void free_CtrlStruct(CtrlStruct *cvs)
{

	free(cvs->sp_reg);
	free(cvs->rob_pos);
	free(cvs->path);
	free(cvs->real_local);
	free(cvs->strat);
	free(cvs->opp);

	free(cvs);
}
