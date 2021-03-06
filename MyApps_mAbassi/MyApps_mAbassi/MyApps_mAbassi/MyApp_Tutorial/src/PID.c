#include "PID.h"
#include "MyApp_mAbassi.h"
#include <math.h>
#include <stdio.h>
#include "MyDriver_CAN.h"


void init_speed_reg(CtrlStruct *cvs){
		cvs->sp_reg->sumErrorL = 0.0;
		cvs->sp_reg->sumErrorR = 0.0;
		cvs->sp_reg->last_t = alt_read_word(fpga_time);
}

void ctrl_motor(void){
	uint32_t tx_Identifier, rx_Identifier;
	uint8_t  tx_Data[8],    rx_Data[8];
	uint8_t  tx_Length,     rx_Length;
	uint8_t  tx_FrameType;
	int rx, tx;

	tx_Identifier = 0x408;
	tx_Length     = 3;
	tx_Data[0] = 0x1E;
	tx_Data[1] = 0x30;
	tx_Data[2] = 0x00;
	CAN_sendMsg(tx_Identifier,tx_Data, tx_Length, MCP2515_TX_STD_FRAME);
	//TSKsleep(10);
	rx = CAN_readMsg(&rx_Identifier, rx_Data, &rx_Length);
	//TSKsleep(10);
}

void push_propDC(int dD, int dG){
	uint32_t tx_Identifier, rx_Identifier;
	uint8_t  tx_Data[8],    rx_Data[8];
	uint8_t  tx_Length,     rx_Length;
	uint8_t  tx_FrameType;
	int rx;

	tx_Identifier = 0x408;
	uint8_t dcGc = (128*(dG+4)/100.0 +128);
	uint8_t dcDc = (128*(dD+7)/100.0 +128);
	tx_Length     = 3;
	tx_Data[0] = 0x25;
	tx_Data[1] = 0xFF;
	tx_Data[2] = dcGc >> 2;
	CAN_sendMsg(tx_Identifier,tx_Data, tx_Length,MCP2515_TX_STD_FRAME);
	rx = CAN_readMsg(&rx_Identifier, rx_Data, &rx_Length);

	tx_Data[0] = 0x26;
	tx_Data[1] = 0xFF;
	tx_Data[2] = dcDc >> 2;
	CAN_sendMsg(tx_Identifier,tx_Data, tx_Length,MCP2515_TX_STD_FRAME);
	rx = CAN_readMsg(&rx_Identifier, rx_Data, &rx_Length);

}

double updateError(CtrlStruct *cvs,double Error, double Deltat, int motor)
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
double Current_Saturation(double RaIa){
	if(RaIa > 5.112){
		return 5.112;
	}
	if(-RaIa > 5.112){
		return -5.112;
	}
	else{
		return RaIa;
	}
}
void speed_regulation(CtrlStruct *cvs,double r_sp_ref, double l_sp_ref)
{
		uint32_t t = alt_read_word(fpga_time);

		//printf("t : %lu\n", t);

		// Given Variables
		int vlmes1 = (int)alt_read_word(fpga_speed_left);
		int vrmes1 = (int)alt_read_word(fpga_speed_right);


		int sens_Left = (vlmes1 >> 15);
		int sens_Right = (vrmes1 >> 15);

		if(sens_Left == 1){
			vlmes1 = (vlmes1 & 0b111111111111111);
		}
		else{
			vlmes1 = -(vlmes1 & 0b111111111111111);
		}

		if(sens_Right == 0){
			vrmes1 = (vrmes1 & 0b111111111111111);
		}
		else{
			vrmes1 = -(vrmes1 & 0b111111111111111);
		}


		double vlmes = vlmes1*2*M_PI/(4*2048*0.1);
		double vrmes = vrmes1*2*M_PI/(4*2048*0.1);

//		printf("left : %f\n", vlmes);
//		printf("right : %f\n", vrmes);
//		printf("\n");

		double Deltat = (double)(t - cvs->sp_reg->last_t)/10000;
		double Error_l = 19*(l_sp_ref - vlmes);
		double Error_r = 19*(r_sp_ref - vrmes);
		double Kp =  0.0391;
		double Ki = 0.1194;

		double PropControl_l = Kp*Error_l;
		double PropControl_r = Kp*Error_r;
		double IntControl_l = Ki*updateError(cvs, Error_l,Deltat,0);
		double IntControl_r = Ki*updateError(cvs, Error_r,Deltat,1);
		double tot_l = PropControl_l + IntControl_l;
		double tot_r = PropControl_r + IntControl_r;

		tot_l = tot_l + 0.0261*vlmes*19;
		tot_r = tot_r + 0.0261*vrmes*19;

			// Voltage saturation
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

	   // last update time
		push_propDC(tot_l*100/24, tot_r*100/24);
	  cvs->sp_reg->last_t = t;
}
