#include "CtrlStruct_gr1.h"
#include "namespace_ctrl.h"
#include "speed_regulation_gr1.h"
#include "odometry_gr1.h"
#include "path_planning_gr1.h"
#include "localisation_gr1.h"
#include "opponents_gr1.h"

NAMESPACE_INIT(ctrlGr1);

/*! \brief initialize the controller structure
 *
 * \param[in] inputs inputs of the controller
 * \param[in] outputs outputs of the controller
 * \return controller main structure
 */
CtrlStruct* init_CtrlStruct(CtrlIn *inputs, CtrlOut *outputs, CtrlOut *py_outputs)
{
	int i;
	CtrlStruct *cvs;

	cvs = (CtrlStruct*) malloc(sizeof(CtrlStruct));

	// io
	cvs->inputs  = inputs;
	cvs->outputs = outputs;

	// speed regulation
	cvs->sp_reg = (SpeedRegulation*) malloc(sizeof(SpeedRegulation));
	cvs->rob_pos = (RobotPosition*) malloc(sizeof(RobotPosition));
	cvs->path = (PathPlanning*) malloc(sizeof(PathPlanning));
	cvs->local = (Localisation*) malloc(sizeof(Localisation));

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
	free(cvs->local);

	free(cvs);
}

NAMESPACE_CLOSE();
