
#include "namespace_ctrl.h"
#include "CtrlStruct_gr1.h"
#include <math.h>
#include "opponents_gr1.h"
#include "user_realtime.h"
#include "set_output.h"

NAMESPACE_INIT(ctrlGr1);

void init_opponents(CtrlStruct *cvs){
}

void update_opponents(CtrlStruct *cvs){
    int number_opponents = cvs->inputs->nb_opponents;
    double true_rising [number_opponents];
    double true_falling [number_opponents];
    double theta_opponents[number_opponents];
    int i;
    int j;
    int count;
    int index = 0;
    for(i=0;i<10; i++){
      count = 0;
      for(j=0;j<10;j++){
        if(cvs->inputs->last_falling[i] == cvs->inputs->last_falling_fixed[j]){
          count ++;
        }
      }
      if(index < number_opponents){
        if(count == 0){
          true_falling[index] = cvs->inputs->last_falling[i];
          true_rising[index] = cvs->inputs->last_rising[i];
          index++;
        }
      }
    }

    for(i=0; i<number_opponents; i++){
        theta_opponents[i] = (true_rising[i] + true_falling[i] + 2*M_PI)/2;
    }


    // double theta_1 = (cvs->inputs->last_falling_fixed[angles_beacons[0]]+M_PI + (cvs->inputs->last_rising_fixed[angles_beacons[0]]+M_PI))/2;
    // double theta_2 = (cvs->inputs->last_falling_fixed[angles_beacons[1]]+M_PI + (cvs->inputs->last_rising_fixed[angles_beacons[1]]+M_PI))/2;
    // double theta_3 = (cvs->inputs->last_falling_fixed[angles_beacons[2]]+M_PI + (cvs->inputs->last_rising_fixed[angles_beacons[2]]+M_PI))/2;

}

NAMESPACE_CLOSE();
