#ifndef _PID_H_
#define _PID_H_

#include "CtrlStruct.h"
#include "MyApp_mAbassi.h"

/// speed regulation
typedef struct SpeedRegulation
{
    uint32_t last_t; ///< last time the speed regulation was updated
	double sumErrorL;
	double sumErrorR;

} SpeedRegulation;

// function prototype
double updateError(CtrlStruct *cvs, double Error, double Deltat, int motor);
void speed_regulation(CtrlStruct *cvs, double r_sp_ref, double l_sp_ref);
void init_speed_reg(CtrlStruct *cvs);
void ctrl_motor(void);
void push_propDC(int dD, int dG);
double Current_Saturation(double RaIa);

#endif
