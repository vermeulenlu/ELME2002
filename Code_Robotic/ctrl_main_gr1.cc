/*!
 * \file ctrl_main_gr2.cc
 * \brief Initialization, loop and finilization of the controller written in C (but compiled as C++)
 */

 #include "ctrl_main_gr1.h"
 #include "namespace_ctrl.h"
 #include "speed_regulation_gr1.h"
 #include "odometry_gr1.h"
 #include "path_planning_gr1.h"
 #include "localisation_gr1.h"
 #include "opponents_gr1.h"
 #include "math.h"
 #include <stdio.h>
 #include <unistd.h>
 #include "set_output.h"

NAMESPACE_INIT(ctrlGr1);

/*! \brief initialize controller operations (called once)
 *
 * \param[in] cvs controller main structure
 */
void controller_init(CtrlStruct *cvs)
{
   init_speed_reg(cvs);
   init_odometry(cvs);
   init_localisation(cvs);
   init_path(cvs);
}

/*! \brief controller loop (called every timestep)
 *
 * \param[in] cvs controller main structure
 */
void controller_loop(CtrlStruct *cvs)
{

    //update_localisation(cvs);
    //update_odometry(cvs);
    //planning(cvs, 0.27, 1.3);
    update_opponents(cvs);
    //printf("postion tower %f\n",cvs->inputs->tower_pos*180/M_PI);
    //printf("rising edge tower %f\n",cvs->inputs->last_rising_fixed[0]*180/M_PI);
}

/*! \brief last controller operations (called once)
 *
 * \param[in] cvs controller main structure
 */
void controller_finish(CtrlStruct *cvs)
{

}

NAMESPACE_CLOSE();
