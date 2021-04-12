/* ------------------------------------------------------------------------------------------------ */

#include "mAbassi.h"
#include "Platform.h"								/* Everything about the target processor is here*/
#include "HWinfo.h"									/* Everything about the tagret hardware is here	*/
#include "SysCall.h"								/* System Call layer stuff						*/

#include "dw_uart.h"
#include "dw_i2c.h"
#include "CtrlStruct.h"

/* ------------------------------------------------------------------------------------------------ */
/* App variables																					*/

int G_UartDevIn  = UART_DEV;						/* Needed by the system call layer if used		*/
int G_UartDevOut = UART_DEV;						/* Needed by the system call layer if used		*/
int G_UartDevErr = UART_DEV;						/* Needed by the system call layer if used		*/

/* ------------------------------------------------------------------------------------------------ */
/* App functions																					*/

extern void Task_FPGA_Button(void);
extern void Task_Main(void);
extern void Hardware_setup(void);

/* ------------------------------------------------------------------------------------------------ */

int main(void)
{
TSK_t *Task;

	setvbuf(stdin,  NULL, _IONBF, 0);				/* By default, Newlib library flush the I/O		*/
	setvbuf(stdout, NULL, _IONBF, 0);				/* buffer when full or when new-line			*/
	setvbuf(stderr, NULL, _IONBF, 0);

/* ------------------------------------------------ */
/* Start uAbassi									*/

	OSstart();										/* Start uAbassi								*/

	TSKsetCore(TSKmyID(), 0);						/* Make sure App_0 will always run on Core #0	*/

	SysCallInit();									/* Initialize the System Call layer				*/

	OSintOn();										/* Enable the interrupts						*/

/* ------------------------------------------------ */
/* SysTick Set-up									*/
													/* Timer tick needed for Sleep() & timeouts		*/
	OSisrInstall(OS_TICK_INT, &TIMtick);			/* Install the RTOS timer tick function			*/
	TIMERinit(((((OS_TICK_SRC_FREQ/OS_TICK_PRESCL)/100000)*(OX_TIMER_US))/(10)));
	GICenable(OS_TICK_INT, 128, 1);					/* Priority == mid / Edge trigger				*/

/* ------------------------------------------------ */
/* UART set-up										*/
/* 8 bits, No Parity, 1 Stop						*/
/* Cyclone V has 2 UART                             */

	uart_init(UART_DEV, BAUDRATE, 8, 0, 10, UART_Q_SIZE_RX, UART_Q_SIZE_TX, UART_FILT_OUT_LF_CRLF);
	i2c_init(0, I2C_XL345_WIDTH, I2C_XL345_SPEED);

	OSisrInstall(UART0_INT, &UARTintHndl_0);
	GICenable(-UART0_INT, 128, 0);					/* UART Must use level and not edge trigger		*/

	OSisrInstall(UART1_INT, &UARTintHndl_1);
	GICenable(-UART1_INT, 128, 0);					/* UART Must use level and not edge trigger		*/

/* ------------------------------------------------ */
/* Application set-up								*/

	Task = TSKcreate("TaskProjet", 0, 8192, &Task_Main, 0);
	TSKsetCore(Task, 1);							/* Create new task, will always run on core #1	*/
	TSKresume(Task);								/* when BMP (G_OS_MP_TYPE == 4 or 5)			*/

#if defined(USE_SHELL)
    TSKcreate("Shell", OX_PRIO_MIN, 16384, OSshell, 1);
    TSKsleep(OS_MS_TO_TICK(100));                   /* Make sure Shell Task initialization if over  */
#endif

	TSKsetCore(TSKmyID(), 0);						/* Make sure Task_HPS_Led always run on Core #0	*/

	Hardware_setup();									/* Go into core #0 Task_HPS_Led code			*/

	for(;;);										/* Should never come back here					*/
}

/* ------------------------------------------------------------------------------------------------ */
/* Not used but needed to build uAbassi																*/

void TIMcallBack(void)
{
	return;
}

/* EOF */
