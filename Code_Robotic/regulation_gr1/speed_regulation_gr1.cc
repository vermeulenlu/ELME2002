#include "speed_regulation_gr1.h"
#include "namespace_ctrl.h"
#include "user_realtime.h"
#include "set_output.h"

NAMESPACE_INIT(ctrlGr1);

/*! \brief wheels speed regulation
 *
 * \param[in,out] cvs controller main structure
 * \parem[in] r_sp_ref right wheel speed reference [rad/s]
 * \parem[in] l_sp_ref left wheel speed reference [rad/s]
 */

void init_speed_reg(CtrlStruct *cvs){
		cvs->sp_reg->sumErrorL = 0.0;
		cvs->sp_reg->sumErrorR = 0.0;
		cvs->sp_reg->last_t = 0.0;
}

double updateError(CtrlStruct *cvs, double Error, double Deltat, int motor)
{
	if(motor == 0){
		cvs->sp_reg->sumErrorL = cvs->sp_reg->sumErrorL + Error*Deltat;
		return cvs->sp_reg->sumErrorL;
	}
	else{
		cvs->sp_reg->sumErrorR = cvs->sp_reg->sumErrorR + Error*Deltat;
		return cvs->sp_reg->sumErrorR;
	}
}
void speed_regulation(CtrlStruct *cvs, double r_sp_ref, double l_sp_ref)
{
	// variables declaration

	CtrlIn  *inputs;
	CtrlOut *outputs;
	SpeedRegulation *sp_reg;

	// variables initialization
	inputs  = cvs->inputs;
	outputs = cvs->outputs;
	sp_reg  = cvs->sp_reg;


		// Given Variables
		double vlmes = inputs->wheel_speeds[1];
		double vrmes = inputs->wheel_speeds[0];

		double Deltat = inputs->t+15 - sp_reg->last_t;

		double Error_l = 14*(l_sp_ref - vlmes);
		double Error_r = 14*(r_sp_ref - vrmes);

		double Kp = 1.0;
		double Ki = 2.0;

		double PropControl_l = Kp*Error_l;
		double PropControl_r = Kp*Error_r;
		double IntControl_l = Ki*updateError(cvs, Error_l,Deltat,0);
		double IntControl_r = Ki*updateError(cvs, Error_r,Deltat,1);
		double tot_l = PropControl_l + IntControl_l;
		double tot_r = PropControl_r + IntControl_r;

		if(tot_l >= 24){
			tot_l = 24;
		}
		if(tot_l <= -24){
			tot_l = -24;
		}
		if(tot_r >= 24){
			tot_r = 24;
		}
		if(tot_r <= -24){
			tot_r = -24;
		}


		outputs->wheel_commands[1] = 100*(tot_l)/24;
		outputs->wheel_commands[0] = 100*(tot_r)/24;


		// set_plot(inputs->wheel_speeds[0], "left");
		// set_plot(inputs->wheel_speeds[1], "right");
		//
		// set_output(r_sp_ref, "Ref");
		// set_output(inputs->wheel_speeds[0], "Output_wheels");



	// wheel commands

	// last update time
	  sp_reg->last_t = inputs->t+15;
}

NAMESPACE_CLOSE();
