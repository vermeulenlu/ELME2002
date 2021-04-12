/*!
 * \author Nicolas Van der Noot
 * \file speed_regulation_ex.h
 * \brief speed regulation to track wheel speed references
 */

#ifndef _SPEED_REGULATION_GR1_H_
#define _SPEED_REGULATION_GR1_H_

#include "namespace_ctrl.h"
#include "CtrlStruct_gr1.h"

NAMESPACE_INIT(ctrlGr1);

/// speed regulation
typedef struct SpeedRegulation
{
	double last_t; ///< last time the speed regulation was updated
	double sumErrorL;
	double sumErrorR;

} SpeedRegulation;

// function prototype
double updateError(CtrlStruct *cvs, double Error, double Deltat, int motor);
void speed_regulation(CtrlStruct *cvs, double r_sp_ref, double l_sp_ref);
void init_speed_reg(CtrlStruct *cvs);

NAMESPACE_CLOSE();

#endif
