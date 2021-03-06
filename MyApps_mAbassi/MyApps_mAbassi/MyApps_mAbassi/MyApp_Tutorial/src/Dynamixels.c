
#include "MyApp_mAbassi.h"

#include "mAbassi.h"          /* MUST include "SAL.H" and not uAbassi.h        */
#include "Platform.h"         /* Everything about the target platform is here  */
#include "HWinfo.h"           /* Everything about the target hardware is here  */
#include "dw_i2c.h"
#include "dw_uart.h"
//#include "MyDriver_CAN.h"
//#include "PID.h"
//#include "CtrlStruct.h"
#include <stdio.h>
#include <unistd.h>
//#include "odometry.h"
//#include "path_planning.h"
//#include "Real_localisation.h"
#include "Dynamixels.h"
//#include "strategy.h"

void set_limit_angles(Byte ID){
	//sets min angle to 0 (0x00) and max angle to 300 (0x3ff)
	send_packet(ID, 0x05, 0x03, 0x06, 0);
	send_packet(ID, 0x05, 0x03, 0x08, 0x3ff);
}
void deploy_flag(void){
	set_speed(0x04, 150);
	set_position(0x04, 1023*80/300);
}
void reploy_flag(void){
	set_speed(0x04, 150);
	set_position(0x04, 1024*0/300);
}
void deploy_shovels(void){
	//Left
	set_speed(0x06, 500);
	set_position(0x06, 1024*230/300);
	//Right
	set_speed(0x02, 500);
	set_position(0x02, 1024*0/300);
}
void reploy_shovels(void){
	//Left
	set_speed(0x06, 500);
	set_position(0x06, 1024*90/300);
	//Right
	set_speed(0x02, 500);
	set_position(0x02, 1024*120/300);
}
void deploy_right_arm(void){
	set_speed(0x01, 500);
	set_position(0x01, 1024*115/300);
	TSKsleep(50);
	set_torque(0x01, 50);
}
void reploy_right_arm(void){
	set_speed(0x01, 500);
	set_position(0x01, 1024*35/300);
	set_torque(0x01, 1023);
}
void deploy_left_arm(void){
	set_speed(0x03, 500);
	set_torque(0x03, 1023);
	set_position(0x03, 1023*30/300);
	TSKsleep(50);
	set_torque(0x03, 50);
}
void reploy_left_arm(void){
	set_speed(0x03, 500);
	set_position(0x03, 1023*120/300);
	set_torque(0x03, 1023);
}
void deploy_clamps(void){
	endless_turn(0x3ff, 0x04, 1);
	set_torque(0x04, 1023);
	TSKsleep(OS_MS_TO_TICK(3800));
	endless_turn(0, 0x04, 1);
}
void reploy_clamps(void){
	endless_turn(0x3ff, 0x04, 0);
	set_torque(0x04, 1023);
	TSKsleep(OS_MS_TO_TICK(4050));
	endless_turn(0, 0x04, 0);
}
void endless_turn(uint16_t speed, Byte ID, int sens){
	send_packet(ID,0x05,0x03,CW_ANGLE_LIMIT_REG, 0);
	send_packet(ID,0x05,0x03,CCW_ANGLE_LIMIT_REG, 0);
	if(sens == 1){ // CW
		uint16_t value = (0b000001 << 10) | speed;
		send_packet(ID,0x05,0x03,MOVING_SPEED_REG, value);
	}
	else{
		uint16_t value = (0b000000 << 10) | speed;
		send_packet(ID,0x05,0x03,MOVING_SPEED_REG, value);
	}
}
void set_torque(Byte ID, uint16_t value){
	send_packet(ID,0x05,0x03,TORQUE_LIMIT_REG, value);
}
void set_speed(Byte ID, uint16_t speed){
	send_packet(ID,0x05,0x03,MOVING_SPEED_REG, speed);
}
void set_position(Byte ID, uint16_t position){
	send_packet(ID,0x05,0x03,GOAL_POSITION_REG, position);
}

void get_packet(void){
	uint32_t data0;
	uint32_t data1;
	uint32_t data2;
	uint32_t data3;
	alt_write_word(fpga_rw_ad, 0b100);
	alt_write_word(fpga_read, 1);
	data0 = alt_read_word(fpga_read_data_RX);
	if(data0 == 1){
		printf("TX done !\n");
	}
	else{
		printf("TX Failed !\n");
	}
	alt_write_word(fpga_rw_ad, 0b001);
	data1 = alt_read_word(fpga_read_data_RX);
	alt_write_word(fpga_rw_ad, 0b010);
	data2 = alt_read_word(fpga_read_data_RX);
	alt_write_word(fpga_rw_ad, 0b000);
	data3 = alt_read_word(fpga_read_data_RX);
	if(data3 == 1){
		printf("RX done !\n");
	}
	else{
		printf("RX Failed! \n");
	}
	printf("\n");
	alt_write_word(fpga_read, 0);
}
void send_packet(Byte ID, Byte Length, Byte Instruction, Byte address, uint16_t value){
	for(int i=0; i<2; i++){
		uint32_t word_1 = get_packet_1(ID,Length,Instruction,address,value);
		uint32_t word_2 = get_packet_2(address,value);
		alt_write_word(fpga_reset, 1);
		alt_write_word(fpga_reset, 0);
		alt_write_word(fpga_write, 1);
		alt_write_word(fpga_rw_ad, 0b101);
		alt_write_word(fpga_write_data_TX,word_1);
		//printf("data1 = %x \n", alt_read_word(fpga_write_data_TX));
		alt_write_word(fpga_rw_ad, 0b110);
		alt_write_word(fpga_write_data_TX,word_2);
		//printf("data1 = %x \n", alt_read_word(fpga_write_data_TX));
		alt_write_word(fpga_rw_ad, 0b100);	//added
		TSKsleep(OS_MS_TO_TICK(10));		//added
		alt_write_word(fpga_write, 0);
		//alt_write_word(fpga_read, 1);
		//alt_write_word(fpga_read, 0);
		get_packet();
		TSKsleep(OS_MS_TO_TICK(10));
	}
}
uint32_t get_packet_1(Byte ID, Byte Length, Byte Instruction, Byte address, uint16_t value){
	Byte P1 = address;
	uint16_t P2 = (value & 0b0000000011111111);
	Byte P3 = value >> 8;
	Byte Checksum = ~(ID + Length + Instruction + P1 + P2 + P3);
	uint32_t word_1 = (Checksum << 24) | (Instruction << 16) | (Length << 8) | (ID);
	return word_1;
}
uint32_t get_packet_2(Byte address, uint16_t value){
	Byte zero = 0;
	Byte P1 = address;
	uint16_t P2 = (value & 0b0000000011111111);
	Byte P3 = value >> 8;
	uint32_t word_2 = (zero << 24) | (P3 << 16) | (P2 << 8) | (P1);
	return word_2;
}
