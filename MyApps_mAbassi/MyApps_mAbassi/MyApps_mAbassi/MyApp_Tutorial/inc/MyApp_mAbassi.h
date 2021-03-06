/* ------------------------------------------------------------------------------------------------ */
/* Include                                                                                         */

#ifndef MYAPP_H_
#define MYAPP_H_

#include <stdbool.h>
#include <stdint.h>

#include "socal/alt_gpio.h"
#include "socal/hps.h"
#include "socal/socal.h"

#include "alt_generalpurpose_io.h"
#include "alt_interrupt.h"
#include "hps_0.h"
#include "CtrlStruct.h"
#include "MyApp_I2C.h"

/* ------------------------------------------------------------------------------------------------ */
/* Define                                                                                           */

#define assert(e) ((e) ? (void)0 : printf("Error in %s, Line : %d\r\n", __FILE__, __LINE__))

// |=============|==========|==============|==========|
// | Signal Name | HPS GPIO | Register/bit | Function |
// |=============|==========|==============|==========|
// |   HPS_LED   |  GPIO53  |   GPIO1[24]  |    I/O   |
// |=============|==========|==============|==========|
#define HPS_LED_IDX        (ALT_GPIO_1BIT_53)                      // GPIO53
#define HPS_LED_PORT       (alt_gpio_bit_to_pid(HPS_LED_IDX))      // ALT_GPIO_PORTB
#define HPS_LED_PORT_BIT   (alt_gpio_bit_to_port_pin(HPS_LED_IDX)) // 24 (from GPIO1[24])
#define HPS_LED_MASK       (1 << HPS_LED_PORT_BIT)

// |=============|==========|==============|==========|
// | Signal Name | HPS GPIO | Register/bit | Function |
// |=============|==========|==============|==========|
// |  HPS_KEY_N  |  GPIO54  |   GPIO1[25]  |    I/O   |
// |=============|==========|==============|==========|
#define HPS_KEY_N_IDX      (ALT_GPIO_1BIT_54)                        // GPIO54
#define HPS_KEY_N_PORT     (alt_gpio_bit_to_pid(HPS_KEY_N_IDX))      // ALT_GPIO_PORTB
#define HPS_KEY_N_PORT_BIT (alt_gpio_bit_to_port_pin(HPS_KEY_N_IDX)) // 25 (from GPIO1[25])
#define HPS_KEY_N_MASK     (1 << HPS_KEY_N_PORT_BIT)

// IRQ for the 2 buttons
#define GPT_BUTTON_IRQ   ALT_INT_INTERRUPT_F2S_FPGA_IRQ0 + BUTTON_PIO_IRQ
#define GPT_SPI_IRQ      ALT_INT_INTERRUPT_F2S_FPGA_IRQ0 + SPI_RASPBERRYPI_IRQ

// PIO Registers
#define PIOdirection        1*4
#define PIOinterruptmask    2*4
#define PIOedgecapture      3*4
#define PIOoutset           4*4
#define PIOoutclear         5*4

// SPI Registers and Control
#define SPI_RXDATA     0
#define SPI_TXDATA     1*4
#define SPI_STATUS     2*4
#define SPI_CONTROL    3*4
#define SPI_EOP_VALUE  6*4

#define SPI_STATUS_EOP    0x200
#define SPI_STATUS_E      0x100
#define SPI_STATUS_RRDY   0x80
#define SPI_STATUS_TRDY   0x40
#define SPI_STATUS_TMT    0x20
#define SPI_STATUS_TOE    0x10
#define SPI_STATUS_ROE    0x08

#define SPI_CONTROL_IEOP  0x200
#define SPI_CONTROL_IE    0x100
#define SPI_CONTROL_IRRDY 0x80
#define SPI_CONTROL_ITRDY 0x40
#define SPI_CONTROL_ITOE  0x10
#define SPI_CONTROL_IROE  0x08

// To protect non- multithread-safe functions in the standard libraries
#define MTXLOCK_ALLOC()		MTXlock(G_OSmutex, -1)
#define MTXUNLOCK_ALLOC()	MTXunlock(G_OSmutex)
#define MTXLOCK_STDIO()		MTXlock(G_OSmutex, -1)
#define MTXUNLOCK_STDIO()	MTXunlock(G_OSmutex)

/* ------------------------------------------------------------------------------------------------ */
/* Global variables                                                                                 */

#ifdef MYAPP
    void *fpga_leds = ALT_LWFPGASLVS_ADDR + LED_PIO_BASE;
    void *fpga_buttons = ALT_LWFPGASLVS_ADDR + BUTTON_PIO_BASE;
    void *fpga_spi = ALT_LWFPGASLVS_ADDR + SPI_RASPBERRYPI_BASE;
    void *fpga_switches = ALT_LWFPGASLVS_ADDR + DIPSW_PIO_BASE;
    void *fpga_write = ALT_LWFPGASLVS_ADDR + WRITE_BASE;
    void *fpga_read = ALT_LWFPGASLVS_ADDR + READ_BASE;
    void *fpga_rw_ad = ALT_LWFPGASLVS_ADDR + ADDRESS_BASE;
    void *fpga_write_data_TX = ALT_LWFPGASLVS_ADDR + WRITEDATA_TXD_BASE;
    void *fpga_read_data_RX = ALT_LWFPGASLVS_ADDR + READDATA_RXD_BASE;
    void *fpga_reset = ALT_LWFPGASLVS_ADDR + CLK_HPS_BASE;
    void *fpga_speed_right = ALT_LWFPGASLVS_ADDR + SPEED_RIGHT_BASE;
    void *fpga_speed_left = ALT_LWFPGASLVS_ADDR + SPEED_LEFT_BASE;
    void *fpga_time = ALT_LWFPGASLVS_ADDR + PIO_TIMER_BASE;
    void *fpga_rpi_x = ALT_LWFPGASLVS_ADDR + PI_DATA_X_BASE ;
    void *fpga_rpi_y = ALT_LWFPGASLVS_ADDR + PI_DATA_Y_BASE ;
    void *fpga_rpi_angle = ALT_LWFPGASLVS_ADDR + PI_DATA_ANGLE_BASE ;
	ALT_AVALON_I2C_DEV_t I2C_instance;
	CtrlStruct *cvs;
	ALT_AVALON_I2C_MASTER_CONFIG_t cfg;
#else
    extern void *fpga_leds;
    extern void *fpga_buttons;
    extern void *fpga_spi;
    extern void *fpga_switches;
    extern void *fpga_write;
    extern void *fpga_read;
    extern void *fpga_rw_ad;
    extern void *fpga_write_data_TX;
    extern void *fpga_read_data_RX;
    extern void *fpga_reset;
    extern void *fpga_speed_left;
    extern void *fpga_speed_right;
    extern void *fpga_time;
	extern ALT_AVALON_I2C_DEV_t I2C_instance;
	extern CtrlStruct *cvs;
	extern ALT_AVALON_I2C_MASTER_CONFIG_t cfg;
    extern void *fpga_rpi_x;
    extern void *fpga_rpi_y;
    extern void *fpga_rpi_angle;
#endif

/* ------------------------------------------------------------------------------------------------ */
/* Tasks & Functions
 *                                                                                */
typedef unsigned char Byte;
#define CW_ANGLE_LIMIT_REG			0x06
#define CCW_ANGLE_LIMIT_REG			0x08
#define LED_REG						0x19
#define CW_COMPLIANCE_MARGIN_REG	0x1A
#define CCW_COMPLIANCE_MARGIN_REG	0x1B
#define CW_COMPLIANCE_SLOPE_REG		0x1C
#define CCW_COMPLIANCE_SLOPE_REG	0x1D
#define GOAL_POSITION_REG			0x1E
#define MOVING_SPEED_REG			0x20
#define TORQUE_LIMIT_REG			0x22
#define PRESENT_POSITION_REG		0x24
#define PRESENT_SPEED_REG			0x26
#define PRESENT_LOAD_REG			0x28
#define LOCK_REG					0x2F
#define TORQUE_ENABLE				0x22

void Task_FPGA_Button(void);
void toogle_FPGA_LED(int button);
void setup_FPGA_LEDS(void);
void Task_Main(void);
void Hardware_setup(void);
void CAN_initial(void);
void wait(CtrlStruct *cvs, int microsec);
void Task_Sonar(void);

void spi_CallbackInterrupt (uint32_t icciar, void *context);
void button_CallbackInterrupt (uint32_t icciar, void *context);
void setup_Interrupt( void );
void setup_hps_gpio( void );

#endif
