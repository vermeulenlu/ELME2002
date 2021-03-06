/* ------------------------------------------------------------------------------------------------ */

#define MYAPP

#include "MyApp_mAbassi.h"

#include "mAbassi.h"          /* MUST include "SAL.H" and not uAbassi.h        */
#include "Platform.h"         /* Everything about the target platform is here  */
#include "HWinfo.h"           /* Everything about the target hardware is here  */
#include "dw_i2c.h"
#include "dw_uart.h"
#include "MyDriver_CAN.h"
#include "PID.h"
#include "CtrlStruct.h"
#include <stdio.h>
#include <unistd.h>
#include "odometry.h"
#include "path_planning.h"
#include "Real_localisation.h"
#include "Dynamixels.h"
#include "strategy.h"
#include "MyApp_I2C.h"
#include "opponents.h"



/* PROJET */
void Task_Main(void){

	 CAN_initial();
	 cvs = init_CtrlStruct();
	 MBX_t     *PtrMbx;
	 intptr_t *PtrMsg_sonar = (intptr_t) NULL;
	 PtrMbx = MBXopen("Mailbox_sonar", 1);

//	 deploy_flag();
//	 wait(cvs, 1000);
//	 reploy_flag();
//	 wait(cvs, 1000);
//	 deploy_right_arm();
//	 wait(cvs , 100);
//	 deploy_left_arm();
//	 wait(cvs, 1000);
//	 reploy_right_arm();
//	 reploy_left_arm();

	 ctrl_motor();
	 push_propDC(0,0);
	 init_speed_reg(cvs);
	 init_odometry(cvs);
	 init_realposition(cvs);
	 init_opponents(cvs);
	 init_path(cvs);
	 push_propDC(0,0);
	 init_state(cvs);
	 deploy_shovels();
	 push_propDC(0,0);
	 wait(cvs, 1000);
	 reploy_shovels();
	 wait(cvs, 4000);
	 int res;
	 BOARD_COLOR_t color;
	 int range_left = 0;
	 int range_right = 0;
	 for(;;){
		 if(MBXget(PtrMbx, &PtrMsg_sonar, 0) == 0){
			 range_left = PtrMsg_sonar[0];
			 range_right = PtrMsg_sonar[1];
		 }
		 update_opponents(cvs,range_left, range_right);
		 update_odometry(cvs);
		 update_realposition(cvs);
		 update_state(cvs);
		 //res = planning(cvs, 0.0, 1.0);
		 //speed_regulation(cvs,0.05/0.03, 0.05/0.03);

	 }
}

void wait(CtrlStruct *cvs, int microsec){
	TSKsleep(OS_MS_TO_TICK(microsec));
	cvs->sp_reg->last_t = alt_read_word(fpga_time);
	cvs->rob_pos->last_t = alt_read_word(fpga_time);
}
void CAN_initial(void){
	//printf("ici0\n");
	SEMwait(SEMopen("SemSetup"), -1);
	//printf("ici1\n");
	DE0_SELECT_LT_SPI();
	//printf("ici2\n");
	CAN_init();
}

void Task_Sonar(void) {

    MBX_t     *PrtMbx;
    intptr_t *PtrMsg_sonar = (intptr_t) NULL;
    PrtMbx = MBXopen("Mailbox_sonar", 1);

    int range_left;
    int range_right;
    int buffer[2] = {0,0};

    range_right = range_sonar(&I2C_instance, cfg, RIGHT_SONAR, cvs);

    for (;;) {
        int range_left = range_sonar(&I2C_instance, cfg, LEFT_SONAR, cvs);
        int range_right = range_sonar(&I2C_instance, cfg, RIGHT_SONAR, cvs);

            buffer[0] = range_left;
            buffer[1] = range_right;
            PtrMsg_sonar = buffer;
            MBXput(PrtMbx, PtrMsg_sonar, 0);
    }
}

void Task_FPGA_Button(void){

    MBX_t    *PrtMbx;
    intptr_t  PtrMsg = (intptr_t) NULL;
    SEM_t    *PtrSem;

    PrtMbx = MBXopen("MyMailbox", 128);
    PtrSem = SEMopen("MySemaphore");

    for( ;; )
    {
        SEMwait(PtrSem, -1);            // -1 = Infinite blocking
        SEMreset(PtrSem);
        MTXLOCK_STDIO();
        int button = (int) alt_read_word(fpga_buttons) - 1;
        printf("Receive IRQ from Button %d\n", button);  // The Keys O and 1 seem to be inverted somewhere...
        toogle_FPGA_LED(button);
        MTXUNLOCK_STDIO();

        MBXput(PrtMbx, PtrMsg, -1);     // -1 = Infinite blocking
    }
}
void spi_CallbackInterrupt (uint32_t icciar, void *context){

    // Do something
    MTXLOCK_STDIO();
    printf("INFO: IRQ from SPI : %08x (status = %x)\r\n",
        (unsigned int) alt_read_word(fpga_spi + SPI_RXDATA),
        (unsigned int) alt_read_word(fpga_spi + SPI_STATUS));
    MTXUNLOCK_STDIO();
    alt_write_word(fpga_spi + SPI_TXDATA, 0x113377FF);

    // Clear the status of SPI core
    alt_write_word(fpga_spi + SPI_STATUS, 0x00);
}
void button_CallbackInterrupt (uint32_t icciar, void *context){
    SEM_t    *PtrSem;

    // Clear the interruptmask of PIO core
    alt_write_word(fpga_buttons + PIOinterruptmask, 0x0);

    // Enable the interruptmask and edge register of PIO core for new interrupt
    alt_write_word(fpga_buttons + PIOinterruptmask, 0x3);
    alt_write_word(fpga_buttons + PIOedgecapture, 0x3);

    PtrSem = SEMopen("MySemaphore");
    SEMpost(PtrSem);
}
void setup_Interrupt( void ){
    // IRQ from Key0 and Key1
    OSisrInstall(GPT_BUTTON_IRQ, (void *) &button_CallbackInterrupt);
    GICenable(GPT_BUTTON_IRQ, 128, 1);

    // Enable interruptmask and edgecapture of PIO core for buttons 0 and 1
    alt_write_word(fpga_buttons + PIOinterruptmask, 0x3);
    alt_write_word(fpga_buttons + PIOedgecapture, 0x3);

    // IRQ from SPI slave connected to the RaspberryPI
    OSisrInstall(GPT_SPI_IRQ, (void *) &spi_CallbackInterrupt);
    GICenable(GPT_SPI_IRQ, 128, 1);

    // Initialize TXDATA to something (for testing purpose)
    alt_write_word(fpga_spi + SPI_TXDATA, 0x0103070F);
    alt_write_word(fpga_spi + SPI_EOP_VALUE, 0x55AA55AA);
    // Enable interrupt
    alt_write_word(fpga_spi + SPI_CONTROL, SPI_CONTROL_IRRDY + SPI_CONTROL_IE);
}
void setup_hps_gpio(){
    uint32_t hps_gpio_config_len = 2;
    ALT_GPIO_CONFIG_RECORD_t hps_gpio_config[] = {
        {HPS_LED_IDX  , ALT_GPIO_PIN_OUTPUT, 0, 0, ALT_GPIO_PIN_DEBOUNCE, ALT_GPIO_PIN_DATAZERO},
        {HPS_KEY_N_IDX, ALT_GPIO_PIN_INPUT , 0, 0, ALT_GPIO_PIN_DEBOUNCE, ALT_GPIO_PIN_DATAZERO}
    };

    assert(ALT_E_SUCCESS == alt_gpio_init());
    assert(ALT_E_SUCCESS == alt_gpio_group_config(hps_gpio_config, hps_gpio_config_len));
}
void toogle_hps_led(){
    uint32_t hps_led_value = alt_read_word(ALT_GPIO1_SWPORTA_DR_ADDR);
    hps_led_value >>= HPS_LED_PORT_BIT;
    hps_led_value = !hps_led_value;
    hps_led_value <<= HPS_LED_PORT_BIT;
    alt_gpio_port_data_write(HPS_LED_PORT, HPS_LED_MASK, hps_led_value);
}
void Hardware_setup(void){
	MBX_t    *PrtMbx;
	intptr_t PtrMsg;
	SEM_t    *PtrSem = SEMopen("SemSetup");

	setup_hps_gpio();               // This is the Adam&Eve Task and we have first to setup everything
	setup_Interrupt();
	SEMpost(PtrSem);
	MTXLOCK_STDIO();
	printf("in Hardware setup \n");
	MTXUNLOCK_STDIO();
	init_I2C(&I2C_instance, cfg);
	Task_Sonar();
}
