#ifndef _DYNAMIXELS_H_
#define _DYNAMIXELS_H_


void set_limit_angles(Byte ID);
void deploy_flag(void);
void reploy_flag(void);
void deploy_shovels(void);
void reploy_shovels(void);
void endless_turn(uint16_t speed, Byte ID, int sens);
void deploy_right_arm(void);
void reploy_right_arm(void);
void deploy_left_arm(void);
void reploy_left_arm(void);
void set_torque(Byte ID, uint16_t value);
void set_speed(Byte ID, uint16_t speed);
void set_position(Byte ID, uint16_t position);
void get_packet(void);
void send_packet(Byte ID, Byte Length, Byte Instruction, Byte address, uint16_t value);
uint32_t get_packet_1(Byte ID, Byte Length, Byte Instruction, Byte address, uint16_t value);
uint32_t get_packet_2(Byte address, uint16_t value);


#endif
