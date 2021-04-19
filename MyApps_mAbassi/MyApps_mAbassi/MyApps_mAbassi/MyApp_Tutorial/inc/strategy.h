#ifndef _STRATEGY_H_
#define _STRATEGY_H_

#include "CtrlStruct.h"
#include "MyApp_I2C.h"

/// speed regulation
typedef struct Strategy
{
	int state;
	int next_state;
	uint32_t t_start;
	double cmd_orient;
} Strategy;

// function prototype
//void update_state(CtrlStruct *cvs);
void update_state(CtrlStruct *cvs);
void init_state(CtrlStruct *cvs);
void pop_last(CtrlStruct *cvs);
void pop(CtrlStruct *cvs, int index);

#endif
