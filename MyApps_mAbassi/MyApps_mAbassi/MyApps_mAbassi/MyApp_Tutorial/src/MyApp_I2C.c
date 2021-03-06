/******************************************************************************
*                                                                             *
* License Agreement                                                           *
*                                                                             *
* Copyright (c) 2016 Altera Corporation, San Jose, California, USA.           *
* All rights reserved.                                                        *
*                                                                             *
* Permission is hereby granted, free of charge, to any person obtaining a     *
* copy of this software and associated documentation files (the "Software"),  *
* to deal in the Software without restriction, including without limitation   *
* the rights to use, copy, modify, merge, publish, distribute, sublicense,    *
* and/or sell copies of the Software, and to permit persons to whom the       *
* Software is furnished to do so, subject to the following conditions:        *
*                                                                             *
* The above copyright notice and this permission notice shall be included in  *
* all copies or substantial portions of the Software.                         *
*                                                                             *
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR  *
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,    *
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE *
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER      *
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING     *
* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER         *
* DEALINGS IN THE SOFTWARE.                                                   *
*                                                                             *
* This agreement shall be governed in all respects by the laws of the State   *
* of California and by the laws of the United States of America.              *
*                                                                             *
******************************************************************************/
#define ALT_AVALON_I2C

#include <stddef.h>		
#include <stdio.h>		
#include <stdlib.h>		
#include <stdint.h>		


#include "MyApp_I2C.h"
#include "MyApp_mAbassi.h"		

#include "mAbassi.h"          /* MUST include "SAL.H" and not uAbassi.h        */
#include "Platform.h"         /* Everything about the target platform is here  */
#include "HWinfo.h"           /* Everything about the target hardware is here  */
#include "dw_uart.h"
#include "CtrlStruct.h"

/* ------------------------------------------------------------------------------------------------ */
/* I2C COMMUNICATION */
/* ------------------------------------------------------------------------------------------------ */

void instantiate_device(ALT_AVALON_I2C_DEV_t *device)
{
	//Device Instantiation
	//ALT_AVALON_I2C_DEV_t device;
	device->i2c_base = I2C_0_base;	
	device->irq_ID = I2C_0_IRQ;
	device->callback = 0x0;
	device->callback_context = 0x0;
	device->control = 0;
	device->master_target_address = 0;
	device->cmd_fifo_size = I2C_0_FIFO_DEPTH;
	device->rx_fifo_size = I2C_0_FIFO_DEPTH;
	device->ip_freq_in_hz = I2C_0_FREQ;
	device->address_mode = 0;					//to change if a 10bit address is needed
}

/* Initializes the I2C Module */
void alt_avalon_i2c_init (ALT_AVALON_I2C_DEV_t *dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg, const ALT_AVALON_I2C_ADDR_MODE_t addrMode, const int speed_frq )
{
    //ALT_AVALON_I2C_MASTER_CONFIG_t cfg;

    /* disable ip */
    alt_avalon_i2c_disable(dev);

    /* Disable interrupts */   // ???
    //alt_avalon_i2c_int_disable(dev,ALT_AVALON_I2C_ISR_ALLINTS_MSK);

    /* clear ISR register content */
    alt_avalon_i2c_int_clear(dev,ALT_AVALON_I2C_ISR_ALL_CLEARABLE_INTS_MSK);
	
    /* set the cmd fifo threshold */		
    alt_avalon_i2c_tfr_cmd_fifo_threshold_set(dev,ALT_AVALON_I2C_TFR_CMD_FIFO_NOT_FULL);
    
    /* set the tx fifo threshold */		
    alt_avalon_i2c_rx_fifo_threshold_set(dev,ALT_AVALON_I2C_RX_DATA_FIFO_FULL);
	
	//ADDED TO TEST CTRL REGISTER - PART1 -------------------------------
	/*ALT_AVALON_I2C_RX_DATA_FIFO_THRESHOLD_t threshold_rx;
	ALT_AVALON_I2C_TFR_CMD_FIFO_THRESHOLD_t threshold_tfr;
	MTXLOCK_STDIO();	
	alt_avalon_i2c_rx_fifo_threshold_get(dev, &threshold_rx);
	printf("CTRL : rx_fifo_thr : %d \n", threshold_rx );
	MTXUNLOCK_STDIO();
	MTXLOCK_STDIO();	
	alt_avalon_i2c_tfr_cmd_fifo_threshold_get(dev, &threshold_tfr);
	printf("CTRL : tfr_fifo_thr : %d \n", threshold_tfr );
	MTXUNLOCK_STDIO(); */
    
    /* set the default bus speed */
    //cfg.speed_mode = ALT_AVALON_I2C_SPEED_FAST;			
    cfg.speed_mode = ALT_AVALON_I2C_SPEED_FAST;		
    
    /*set the address mode */
    //cfg.addr_mode = ALT_AVALON_I2C_ADDR_MODE_7_BIT;
    cfg.addr_mode = addrMode;
    
    /* set the bus speed */
    alt_avalon_i2c_master_config_speed_set(dev, &cfg, speed_frq);		
	
    /* write the cfg information */
    alt_avalon_i2c_master_config_set(dev,&cfg);

    /*
     * Creating semaphores used to protect access to the registers 
     * when running in a multi-threaded environment.
     */
    /*
	int error;
	error = ALT_SEM_CREATE (&dev->regs_lock, 1);		//ADD LATER ON

    if (!error)
    {        
        // Install IRQ handler 		----------------------- NOT NECESSARY
        alt_ic_isr_register(dev->irq_controller_ID, dev->irq_ID, alt_avalon_i2c_irq, dev, 0x0);
    }
    else
    {
        alt_printf("failed to create semaphores\n");
    }*/

    return;
}


/* Changing the I2C instance for the COLOR SENSOR */
void alt_avalon_i2c_change_config_color_sensor (ALT_AVALON_I2C_DEV_t *dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg)
{
	int speed_colorSensor = 350000; 		//400kHz max

	/* set the address */
	alt_avalon_i2c_master_target_set(dev, ADDRESS_COLOR_SENSOR);
    /* set the default bus speed */
    cfg.speed_mode = ALT_AVALON_I2C_SPEED_FAST;			
    /*set the address mode */
    cfg.addr_mode = ALT_AVALON_I2C_ADDR_MODE_7_BIT;
    /* set the bus speed */
    alt_avalon_i2c_master_config_speed_set(dev, &cfg, speed_colorSensor);		
    /* write the cfg information */
    alt_avalon_i2c_master_config_set(dev, &cfg);

}

/* Changing the I2C instance for the SONAR */
void alt_avalon_i2c_change_config_sonar(ALT_AVALON_I2C_DEV_t *dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg, ALT_AVALON_SONAR_NB_t sonar)
{
	int speed_sonar = 40000;
	
	/* set the address */
	alt_avalon_i2c_master_target_set(dev, sonar >> 1);
    /* set the default bus speed */
    cfg.speed_mode = ALT_AVALON_I2C_SPEED_STANDARD;			
    /*set the address mode */
    cfg.addr_mode = ALT_AVALON_I2C_ADDR_MODE_7_BIT;
    /* set the bus speed */
    alt_avalon_i2c_master_config_speed_set(dev, &cfg, speed_sonar);		
    /* write the cfg information */
    alt_avalon_i2c_master_config_set(dev, &cfg);
}

/* Changing the I2C instance for the MICROSWITCH */
void alt_avalon_i2c_change_config_microswitch (ALT_AVALON_I2C_DEV_t *dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg)
{
	int speed_microswitch = 80000; 		//100kHz max
	
	/* set the address */
	alt_avalon_i2c_master_target_set(dev, ADDRESS_MICROSWITCH);
    /* set the default bus speed */
    cfg.speed_mode = ALT_AVALON_I2C_SPEED_STANDARD;			
    /*set the address mode */
    cfg.addr_mode = ALT_AVALON_I2C_ADDR_MODE_7_BIT;
    /* set the bus speed */
    alt_avalon_i2c_master_config_speed_set(dev, &cfg, speed_microswitch);		
    /* write the cfg information */
    alt_avalon_i2c_master_config_set(dev, &cfg);
}


/* optional irq callback */
static void optional_irq_callback(void * context)
{
   int timeout=100000;
   alt_u32 bytes_read;

   ALT_AVALON_I2C_DEV_t *i2c_dev = context;
   IRQ_DATA_t *irq = i2c_dev->callback_context;

   if (irq->irq_busy==2)  /*receive request*/
   {
       alt_avalon_i2c_rx_read_available(i2c_dev, irq->buffer, irq->size, &bytes_read);
       irq->size-=bytes_read;
       irq->buffer+=bytes_read;
       if (irq->size > 0)
       {
         /* clear ISR register content */
         alt_avalon_i2c_int_clear(i2c_dev,ALT_AVALON_I2C_ISR_ALL_CLEARABLE_INTS_MSK);
         /* re-enable the RX_READY interrupt */
         alt_avalon_i2c_int_enable(i2c_dev,ALT_AVALON_I2C_ISER_RX_READY_EN_MSK);
         return;
       }
    }

    /*transaction should be done so no or minimal looping should occur*/
    /*for a write, this code will only be reached after the cmd fifo is*/
    /*empty (sent).  For a read this code will only be reached after all*/
    /*bytes have been received.*/
    while (alt_avalon_i2c_is_busy(i2c_dev)) 
    { 
      if (--timeout == 0)
      {
         break;
      }
    }

    /*disable the ip.  The ip is disabled and enabled for each transaction.*/
    alt_avalon_i2c_disable(i2c_dev);

    irq->irq_busy=0;
}

/* Associate a user-specific routine with the i2c interrupt handler. */
void alt_avalon_i2c_register_callback(
    ALT_AVALON_I2C_DEV_t *dev,
    alt_avalon_i2c_callback callback,
    alt_u32 control,
    void *context)
{
    dev->callback         = callback;
    dev->callback_context = context;
    dev->control          = control;

    return ;
}

void alt_avalon_i2c_register_optional_irq_handler(ALT_AVALON_I2C_DEV_t *i2c_dev,IRQ_DATA_t * irq_data)
{
   irq_data->irq_busy=0;
   alt_avalon_i2c_register_callback(i2c_dev,optional_irq_callback,0,irq_data);
}

/* enable the avalon i2c ip */
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_enable(ALT_AVALON_I2C_DEV_t *i2c_dev)
{
   IRQ_DATA_t *irq_data = i2c_dev->callback_context;
   alt_u32 enable_status;
       
   /*if the ip is already enabled, return a busy status*/
   void *ALT_AVALON_I2C_CTRL = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_CTRL_REG;
   enable_status = ( alt_read_word(ALT_AVALON_I2C_CTRL) & ALT_AVALON_I2C_CTRL_EN_MSK) >> ALT_AVALON_I2C_CTRL_EN_OFST;
   if (enable_status)
   {
	 MTXLOCK_STDIO();	
     printf("BUSY in enable fct \n");
     MTXUNLOCK_STDIO();
     return ALT_AVALON_I2C_BUSY;
   }
   
   /*if the optional irq callback is registered ensure irq_busy is 0*/
   if (i2c_dev->callback == optional_irq_callback)
   {
     irq_data->irq_busy=0;
   }
   
   /* enable ip */
   //void *ALT_AVALON_I2C_CTRL = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_CTRL_REG;
   unsigned long current_CTRL = alt_read_word(ALT_AVALON_I2C_CTRL);
   alt_write_word(ALT_AVALON_I2C_CTRL, current_CTRL | ALT_AVALON_I2C_CTRL_EN_MSK);

   return ALT_AVALON_I2C_SUCCESS;
}

/* disable the avalon i2c ip */
void alt_avalon_i2c_disable(ALT_AVALON_I2C_DEV_t *i2c_dev)
{
   /* disable ip by putting bit1 of CTRL reg at 0*/
   void *ALT_AVALON_I2C_CTRL = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_CTRL_REG;
   //alt_write_word(ALT_AVALON_I2C_CTRL, (alt_u32)(0 & ALT_AVALON_I2C_CTRL_EN_MSK));
   unsigned long current_CTRL = alt_read_word(ALT_AVALON_I2C_CTRL);
   /*MTXLOCK_STDIO();	
   printf("current CTRL %d \n", current_CTRL);
   MTXUNLOCK_STDIO();*/	
   alt_write_word(ALT_AVALON_I2C_CTRL, (alt_u32)(current_CTRL & ~ALT_AVALON_I2C_CTRL_EN_MSK));
  
}

/* populate the the master config structure from the register values */		//WHAT??
void alt_avalon_i2c_master_config_get(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                          ALT_AVALON_I2C_MASTER_CONFIG_t* cfg)
{
    cfg->addr_mode = i2c_dev->address_mode;
    
	void *ALT_AVALON_I2C_CTRL = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_CTRL_REG;
	cfg->speed_mode = (ALT_AVALON_I2C_SPEED_t)((alt_read_word(ALT_AVALON_I2C_CTRL) & ALT_AVALON_I2C_CTRL_BUS_SPEED_MSK ) >> ALT_AVALON_I2C_CTRL_BUS_SPEED_OFST);
    
    void *ALT_AVALON_I2C_SCL_HIGH = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_SCL_HIGH_REG;
	cfg->scl_hcnt = (alt_u16)((alt_read_word(ALT_AVALON_I2C_SCL_HIGH) & ALT_AVALON_I2C_SCL_HIGH_COUNT_PERIOD_MSK ) >> ALT_AVALON_I2C_SCL_HIGH_COUNT_PERIOD_OFST);
	
    void *ALT_AVALON_I2C_SCL_LOW = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_SCL_LOW_REG;
	cfg->scl_lcnt = (alt_u16)((alt_read_word(ALT_AVALON_I2C_SCL_LOW) & ALT_AVALON_I2C_SCL_LOW_COUNT_PERIOD_MSK ) >> ALT_AVALON_I2C_SCL_LOW_COUNT_PERIOD_OFST);

	void *ALT_AVALON_I2C_SDA_CNT = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_SDA_HOLD_REG;
	cfg->scl_lcnt = (alt_u16)((alt_read_word(ALT_AVALON_I2C_SDA_CNT) & ALT_AVALON_I2C_SDA_HOLD_COUNT_PERIOD_MSK ) >> ALT_AVALON_I2C_SDA_HOLD_COUNT_PERIOD_OFST);

}

//USE THIS OR THE FCT ABOVE, NOT BOTH
/* set the registers from the master config structure */
void alt_avalon_i2c_master_config_set(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                          const ALT_AVALON_I2C_MASTER_CONFIG_t* cfg)
{
    i2c_dev->address_mode   =   cfg->addr_mode;
	//we want to set the data speed_mode shifted by 1 to the left (00 or 10) to the BUS_SPEED register (bit2)
	void *ALT_AVALON_I2C_CTRL = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_CTRL_REG;
    unsigned long current_CTRL = alt_read_word(ALT_AVALON_I2C_CTRL);
	//alt_write_word(ALT_AVALON_I2C_CTRL, (current_CTRL | ((alt_u32)((cfg->speed_mode) << ALT_AVALON_I2C_CTRL_BUS_SPEED_OFST) & ~ALT_AVALON_I2C_CTRL_BUS_SPEED_MSK)));
	if (((cfg->speed_mode) << ALT_AVALON_I2C_CTRL_BUS_SPEED_OFST) == 0b00) {
		alt_write_word(ALT_AVALON_I2C_CTRL, current_CTRL & ~ALT_AVALON_I2C_CTRL_BUS_SPEED_MSK);
	}
	else if (((cfg->speed_mode) << ALT_AVALON_I2C_CTRL_BUS_SPEED_OFST) == 0b10) {
		alt_write_word(ALT_AVALON_I2C_CTRL, current_CTRL | ALT_AVALON_I2C_CTRL_BUS_SPEED_MSK);
	}
	
	void *ALT_AVALON_I2C_SCL_HIGH = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_SCL_HIGH_REG;
	alt_write_word(ALT_AVALON_I2C_SCL_HIGH, cfg->scl_hcnt );

	void *ALT_AVALON_I2C_SCL_LOW = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_SCL_LOW_REG;
    alt_write_word(ALT_AVALON_I2C_SCL_LOW, cfg->scl_lcnt );
	
	void *ALT_AVALON_I2C_SDA_HOLD = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_SDA_HOLD_REG;
    alt_write_word(ALT_AVALON_I2C_SDA_HOLD, cfg->sda_cnt );

}

/* This function returns the speed based on parameters of the
 * I2C master configuration.
*/
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_master_config_speed_get(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                                const ALT_AVALON_I2C_MASTER_CONFIG_t* cfg,
                                                alt_u32 * speed_in_hz)
{
   if ((cfg->scl_lcnt == 0) || (cfg->scl_hcnt == 0))
   {
       return ALT_AVALON_I2C_BAD_ARG;
   }
    
   *speed_in_hz = (i2c_dev->ip_freq_in_hz) / (cfg->scl_lcnt + cfg->scl_hcnt);

   return ALT_AVALON_I2C_SUCCESS;
}

/*This is a utility function that computes parameters for the I2C master
 * configuration that best matches the speed requested. */
 ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_master_config_speed_set(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                                ALT_AVALON_I2C_MASTER_CONFIG_t * cfg,
                                                alt_u32 speed_in_hz)
{
    alt_u32 scl_lcnt,scl_hcnt;

    /* If speed is not standard or fast return range error */
    if ((speed_in_hz > ALT_AVALON_I2C_FS_MAX_HZ) || (speed_in_hz < ALT_AVALON_I2C_SS_MIN_HZ) || (speed_in_hz == 0))
    {
        return ALT_AVALON_I2C_RANGE;
    }

     /* <lcount> = <internal clock> / 2 * <speed, Hz> */
    scl_lcnt = (i2c_dev->ip_freq_in_hz) / (speed_in_hz << 1);

    /* adjust h/l by predetermined amount */
    scl_hcnt = scl_lcnt + ALT_AVALON_I2C_DIFF_LCNT_HCNT;
    scl_lcnt = scl_lcnt - ALT_AVALON_I2C_DIFF_LCNT_HCNT;

    if (speed_in_hz > ALT_AVALON_I2C_FS_MIN_HZ)
    {
       cfg->speed_mode = ALT_AVALON_I2C_SPEED_FAST;
    }
    else 
    {
       cfg->speed_mode = ALT_AVALON_I2C_SPEED_STANDARD;    
    }

    cfg->scl_lcnt = scl_lcnt;
    cfg->scl_hcnt = scl_hcnt;
    cfg->sda_cnt  = scl_lcnt - (scl_lcnt / 2);

    return ALT_AVALON_I2C_SUCCESS;

}

/*Returns ALT_AVALON_I2C_TRUE if the I2C controller is busy. The I2C controller is busy if
 * not in the IDLE state */
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_is_busy(ALT_AVALON_I2C_DEV_t *i2c_dev)
{

    void *ALT_AVALON_I2C_STATUS = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_STATUS_REG;
    if ((alt_read_word(ALT_AVALON_I2C_STATUS)) & ALT_AVALON_I2C_STATUS_CORE_STATUS_MSK)
    {
       return ALT_AVALON_I2C_TRUE;
    }

    return ALT_AVALON_I2C_FALSE;
}

/*Read all available bytes from the receive FIFO up to max_bytes_to_read.  If max_bytes_to_read = 0 then read all available */
void alt_avalon_i2c_rx_read_available(ALT_AVALON_I2C_DEV_t *i2c_dev, alt_u8 *buffer, alt_u32 max_bytes_to_read, alt_u32 *bytes_read)
{
    *bytes_read = 0;
    void *ALT_AVALON_I2C_RX_DATA_FIFO_LVL = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_RX_DATA_FIFO_LVL_REG;
	while (alt_read_word(ALT_AVALON_I2C_RX_DATA_FIFO_LVL))
    {
	   void *ALT_AVALON_I2C_RX_DATA = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_RX_DATA_REG;
       buffer[*bytes_read] = (alt_u8)(alt_read_word(ALT_AVALON_I2C_RX_DATA));
       *bytes_read+=1; 
       if ((*bytes_read == max_bytes_to_read) && (max_bytes_to_read != 0)) break;       
    }
}

/*when a byte is available, reads a single data byte from the receive FIFO. */ //PROBLEM FUNCTION
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_rx_read(ALT_AVALON_I2C_DEV_t *i2c_dev, alt_u8 *val)
{
    alt_u32 status = ALT_AVALON_I2C_SUCCESS;
    alt_u32 timeout = 100000;

	void *ALT_AVALON_I2C_RX_DATA_FIFO_LVL = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_RX_DATA_FIFO_LVL_REG;
	while ((  alt_read_word(ALT_AVALON_I2C_RX_DATA_FIFO_LVL)) == 0) 
    {
      if (timeout<10) {
		  MTXLOCK_STDIO();	
		  //printf("TSKsleep12 \n");
		  MTXUNLOCK_STDIO();
		  TSKsleep(OS_MS_TO_TICK(10));	//alt_busy_sleep(10000)
	  }
      if (--timeout == 0)
      {
        status = ALT_AVALON_I2C_TIMEOUT;
        break;
      }
    }

	void *ALT_AVALON_I2C_RX_DATA = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_RX_DATA_REG;
    *val = (alt_u8)(alt_read_word(ALT_AVALON_I2C_RX_DATA));
        
    return status;
}

/* When space is available, writes the Transfer Command FIFO. */
// Write START, STOP, ADDRESS, MODE		//TO TEST
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_cmd_write(ALT_AVALON_I2C_DEV_t *i2c_dev, 
                                                      alt_u8 val,
                                                      alt_u8 issue_restart,
                                                      alt_u8 issue_stop)
{
    
	/*MTXLOCK_STDIO();	
	printf("in 'cmd_write'");
	MTXUNLOCK_STDIO(); */
	alt_u32 timeout = 1000;
    ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;

	void *ALT_AVALON_I2C_ISR = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_ISR_REG;
	/*MTXLOCK_STDIO();	
	printf("ISR : %d \n", alt_read_word(ALT_AVALON_I2C_ISR) );
	MTXUNLOCK_STDIO();  */
	while ((  alt_read_word(ALT_AVALON_I2C_ISR)  & ALT_AVALON_I2C_ISR_TX_READY_MSK)==0) 
    {
	  MTXLOCK_STDIO();	
	  printf("ISSUE \n");
	  MTXUNLOCK_STDIO();
	  if (timeout<10) {
		  MTXLOCK_STDIO();	
		  printf("TSKsleep \n");
		  MTXUNLOCK_STDIO();
		  TSKsleep(OS_MS_TO_TICK(10));  //alt_busy_sleep(10000)  		//1000
	  }
      if (--timeout == 0)
      {
        MTXLOCK_STDIO();	
	    printf("TIMEOUT : TX not ready ! \n");
	    MTXUNLOCK_STDIO();
		return ALT_AVALON_I2C_TIMEOUT;
      }
    }

	void *ALT_AVALON_I2C_TFR_CMD = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_TFR_CMD_REG;
	unsigned long data = (val | (issue_restart << ALT_AVALON_I2C_TFR_CMD_STA_OFST) | (issue_stop << ALT_AVALON_I2C_TFR_CMD_STO_OFST) );

	alt_write_word(ALT_AVALON_I2C_TFR_CMD , data );
	//DONT TRY TO READ TRD_CMD !!

	/*TSKsleep(50);		//TO BE REMOVED WHEN THERE ARE NO ERRORS !!!! -------------------------------------------------------------------
	
    //check for nack error
    alt_avalon_i2c_check_nack(i2c_dev,&status);
	MTXLOCK_STDIO();	
	printf("status of NACK ERROR : %d \n", status);
	MTXUNLOCK_STDIO();
    
    //check for arb lost
    alt_avalon_i2c_check_arblost(i2c_dev,&status);
	MTXLOCK_STDIO();	
	printf("status of ARBLOST ERROR : %d \n", status);
	MTXUNLOCK_STDIO();
	
	MTXLOCK_STDIO();	
	printf("END OF CMD_WRITE \n");
	MTXUNLOCK_STDIO();*/
	
    return status;
}

/*send 7 or 10 bit i2c address to cmd fifo*/	
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_send_address(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                                       const alt_u32 rw_bit,
                                                       const alt_u8 issue_restart)
{
    alt_u32 status;
        
    if (i2c_dev->address_mode == ALT_AVALON_I2C_ADDR_MODE_10_BIT)
    {
		MTXLOCK_STDIO();	
		printf("Writing 10 bit ADDRESS using cmd_write \n");
		MTXUNLOCK_STDIO();
		//To send the first 7 bits (MSB) of the 10 bit address
		status = alt_avalon_i2c_cmd_write(i2c_dev,(((i2c_dev->master_target_address | TARGET_ADDR_MASK_10BIT) >> 7) & 0xfe) | rw_bit,issue_restart,ALT_AVALON_I2C_NO_STOP);
        MTXLOCK_STDIO();	
		printf("Status after send_address PART1 : %d \n", status); 	//sends 0 (False or Success)
		printf("7 first bits : %x \n", (((i2c_dev->master_target_address | TARGET_ADDR_MASK_10BIT) >> 7) & 0xfe) | rw_bit);
		MTXUNLOCK_STDIO();
		
		//To send the last 3 bits of the 10 bit address
		status = alt_avalon_i2c_cmd_write(i2c_dev,i2c_dev->master_target_address & 0xff,ALT_AVALON_I2C_NO_RESTART,ALT_AVALON_I2C_NO_STOP);      
		MTXLOCK_STDIO();	
		printf("Status after send_address PART2: %d \n", status); 	//sends -5 (Nack)
		printf("7 last bits : %x \n", i2c_dev->master_target_address & 0xff);		
		MTXUNLOCK_STDIO();
	}
    else
    {
	   /*MTXLOCK_STDIO();	
	   printf("Address : %d \n", i2c_dev->master_target_address); //CORRECT : address = 41 = 0x29
	   MTXUNLOCK_STDIO();*/
       status = alt_avalon_i2c_cmd_write(i2c_dev,(i2c_dev->master_target_address << 1) | rw_bit,issue_restart,ALT_AVALON_I2C_NO_STOP);
	   /*MTXLOCK_STDIO();	
	   printf("status after writing ADDRESS : %d \n", status);
	   MTXUNLOCK_STDIO();*/
    }
    
    return status;
}

/* This function returns the current target address. */
void alt_avalon_i2c_master_target_get(ALT_AVALON_I2C_DEV_t * i2c_dev, alt_u32 * target_addr)
{
    *target_addr=i2c_dev->master_target_address;
}

/* This function updates the target address for any upcoming I2C bus IO. */
void alt_avalon_i2c_master_target_set(ALT_AVALON_I2C_DEV_t * i2c_dev, alt_u32 target_addr)
{
    i2c_dev->master_target_address=target_addr;
	/*MTXLOCK_STDIO();	
	printf("After master_target_set function \n");
	MTXUNLOCK_STDIO();*/
}

/*if nack detected, status is set to ALT_AVALON_I2C_NACK_ERR*/
void alt_avalon_i2c_check_nack(ALT_AVALON_I2C_DEV_t *i2c_dev,ALT_AVALON_I2C_STATUS_CODE * status)
{    
	void *ALT_AVALON_I2C_ISR = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_ISR_REG;
    if ( alt_read_word(ALT_AVALON_I2C_ISR)  & ALT_AVALON_I2C_ISR_NACK_DET_MSK)
    {
      *status=ALT_AVALON_I2C_NACK_ERR;
    }
}

/*if arb lost is detected, status is set to ALT_AVALON_I2C_ARB_LOST_ERR*/
void alt_avalon_i2c_check_arblost(ALT_AVALON_I2C_DEV_t *i2c_dev,ALT_AVALON_I2C_STATUS_CODE * status)
{      
	void *ALT_AVALON_I2C_ISR = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_ISR_REG;
    if ( alt_read_word(ALT_AVALON_I2C_ISR)  & ALT_AVALON_I2C_ISR_ARBLOST_DET_MSK)
    {
      *status=ALT_AVALON_I2C_ARB_LOST_ERR;
    }
}


ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_interrupt_transaction_status(ALT_AVALON_I2C_DEV_t *i2c_dev)
{
    ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;
    IRQ_DATA_t *irq_data = i2c_dev->callback_context;
    alt_u32 timeout=10000 * irq_data->size + 10000;
    alt_u32 saveints,temp_bytes_read;
    
    /* save current enabled interrupts */
    alt_avalon_i2c_enabled_ints_get(i2c_dev,&saveints);
    
    /* disable the enabled interrupts */
    alt_avalon_i2c_int_disable(i2c_dev,saveints);
    
    alt_avalon_i2c_check_nack(i2c_dev,&status);

    if (status!=ALT_AVALON_I2C_SUCCESS)
    {
      if (irq_data->irq_busy)
      {
        while (alt_avalon_i2c_is_busy(i2c_dev))
        {
			  if (timeout<10) {
				MTXLOCK_STDIO();	
				printf("TSKsleep2 \n");
				MTXUNLOCK_STDIO();
				TSKsleep(OS_MS_TO_TICK(10));		//alt_busy_sleep(10000)
			  }
              if (--timeout == 0)
              {
                 status = ALT_AVALON_I2C_TIMEOUT;
                 break;
              }
        }
          
        /*clear any rx entries */
        alt_avalon_i2c_rx_read_available(i2c_dev, irq_data->buffer,0,&temp_bytes_read);
       
        /*disable the ip.  The ip is disabled and enabled for each transaction. */
        alt_avalon_i2c_disable(i2c_dev);
          
        /*abort the transaction */
        irq_data->irq_busy=0;
      }
      
      /*return nack error so transaction can be retried*/
      return status;
    }
    
    if (irq_data->irq_busy)
    {
        /*re-enable the interrupts*/
        alt_avalon_i2c_int_enable(i2c_dev,saveints);
        
        /*return transaction still busy*/
        return ALT_AVALON_I2C_BUSY;
    }
    
    /*return transaction completed status, ok to do another transaction*/
    return ALT_AVALON_I2C_SUCCESS;
}

/*transmit function with retry and optionally interrupts*/
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_master_tx(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                       const alt_u8 * buffer,
                                       const alt_u32 size,
                                       const alt_u8 use_interrupts)
{
    ALT_AVALON_I2C_STATUS_CODE status;
    alt_u32 retry=10000;  
    
	/*MTXLOCK_STDIO();	
	printf("LOOP of 'master_tx' \n");
	MTXUNLOCK_STDIO();*/
    while (retry--)
    {

      if (retry<10) {
		  MTXLOCK_STDIO();	
	      printf("TSKsleep3 \n");
	      MTXUNLOCK_STDIO();
		  TSKsleep(OS_MS_TO_TICK(10)); //alt_busy_sleep(10000)
	  }
	  /*MTXLOCK_STDIO();	
	  printf("Point 2 of 'master_tx' \n");
	  MTXUNLOCK_STDIO();*/
      if (use_interrupts)
      {
         status = alt_avalon_i2c_master_transmit_using_interrupts(i2c_dev, buffer, size, ALT_AVALON_I2C_NO_RESTART, ALT_AVALON_I2C_STOP); 
      }
      else
      {
         status = alt_avalon_i2c_master_transmit(i2c_dev, buffer, size, ALT_AVALON_I2C_NO_RESTART, ALT_AVALON_I2C_STOP);
      }
      if ((status==ALT_AVALON_I2C_ARB_LOST_ERR) || (status==ALT_AVALON_I2C_NACK_ERR) || (status==ALT_AVALON_I2C_BUSY)) continue;
      break;
    }

    return status;
}        

/*receive function with retry and optionally interrupts*/
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_master_rx(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                       alt_u8 * buffer,
                                       const alt_u32 size,
                                       const alt_u8 use_interrupts)
{
    ALT_AVALON_I2C_STATUS_CODE status;
    alt_u32 retry=10000;  
    
    if (use_interrupts) 
    {
      while (retry--) 
      {
        if (retry<10) {
			MTXLOCK_STDIO();	
			printf("TSKsleep4 \n");
			MTXUNLOCK_STDIO();
			TSKsleep(OS_MS_TO_TICK(10));	// alt_busy_sleep(10000);      
		}
        status = alt_avalon_i2c_master_receive_using_interrupts(i2c_dev, buffer, size, ALT_AVALON_I2C_NO_RESTART, ALT_AVALON_I2C_STOP);     
        if ((status==ALT_AVALON_I2C_ARB_LOST_ERR) || (status==ALT_AVALON_I2C_NACK_ERR) || (status==ALT_AVALON_I2C_BUSY)) continue;
        break;
      }
    }
    else
    {
      while (retry--) 
      {
        if (retry<10) {
			MTXLOCK_STDIO();	
			printf("TSKsleep5 \n");
			MTXUNLOCK_STDIO();
			TSKsleep(OS_MS_TO_TICK(10));  //alt_busy_sleep(10000); 
		}			
        status = alt_avalon_i2c_master_receive(i2c_dev, buffer, size, ALT_AVALON_I2C_NO_RESTART, ALT_AVALON_I2C_STOP);     
        if ((status==ALT_AVALON_I2C_ARB_LOST_ERR) || (status==ALT_AVALON_I2C_NACK_ERR) || (status==ALT_AVALON_I2C_BUSY)) continue;
        break;
      }
    }
    
    return status;
}        

/*transmit, restart, recieve function using retry and optionally interrupts */
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_master_tx_rx(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                       const alt_u8 * txbuffer,
                                       const alt_u32 txsize,
                                       alt_u8 * rxbuffer,
                                       const alt_u32 rxsize,
                                       const alt_u8 use_interrupts)                                       
{
    ALT_AVALON_I2C_STATUS_CODE status;
    alt_u32 retry=10000;  
    
    if (use_interrupts)
    {
      while (retry--) 
      {
        if (retry<10) {
			MTXLOCK_STDIO();	
			printf("TSKsleep6 \n");
			MTXUNLOCK_STDIO();
			TSKsleep(OS_MS_TO_TICK(10));  //alt_busy_sleep(10000);     
		}			
        status = alt_avalon_i2c_master_transmit_using_interrupts(i2c_dev, txbuffer, txsize, ALT_AVALON_I2C_NO_RESTART, ALT_AVALON_I2C_NO_STOP);     
        if ((status==ALT_AVALON_I2C_ARB_LOST_ERR) || (status==ALT_AVALON_I2C_NACK_ERR) || (status==ALT_AVALON_I2C_BUSY)) continue;
  
        status = alt_avalon_i2c_master_receive_using_interrupts(i2c_dev, rxbuffer, rxsize, ALT_AVALON_I2C_RESTART, ALT_AVALON_I2C_STOP);     
        if ((status==ALT_AVALON_I2C_ARB_LOST_ERR) || (status==ALT_AVALON_I2C_NACK_ERR) || (status==ALT_AVALON_I2C_BUSY)) continue;
  
        break;
      }
    }
    else 
    {
      while (retry--) 
      {
        if (retry<10) {
			MTXLOCK_STDIO();	
			printf("TSKsleep7 \n");
			MTXUNLOCK_STDIO();
			TSKsleep(OS_MS_TO_TICK(10));  	//alt_busy_sleep(10000); 
		}
        status = alt_avalon_i2c_master_transmit(i2c_dev, txbuffer, txsize, ALT_AVALON_I2C_NO_RESTART, ALT_AVALON_I2C_NO_STOP);
        if ((status==ALT_AVALON_I2C_ARB_LOST_ERR) || (status==ALT_AVALON_I2C_NACK_ERR) || (status==ALT_AVALON_I2C_BUSY)) continue;
        status = alt_avalon_i2c_master_receive(i2c_dev, rxbuffer, rxsize, ALT_AVALON_I2C_RESTART, ALT_AVALON_I2C_STOP);     
        if ((status==ALT_AVALON_I2C_ARB_LOST_ERR) || (status==ALT_AVALON_I2C_NACK_ERR) || (status==ALT_AVALON_I2C_BUSY)) continue;
  
        break;
      }
    }
    
    return status;
}                                       
 
/*This function issues a write command and transmits data to the I2C bus. */
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_master_transmit(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                        const alt_u8 * buffer,
                                        alt_u32 size,
                                        const alt_u8 issue_restart,
                                        const alt_u8 issue_stop)
{
    ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;
    alt_u32 timeout=size * 10000;
    
    if (size==0)
    {
      return ALT_AVALON_I2C_SUCCESS;
    }
    
    /*if a new transaction, enable ip and clear int status*/
    if (!issue_restart) 
    {
	  /*MTXLOCK_STDIO();	
	  printf("In the restart loop \n");
	  MTXUNLOCK_STDIO();*/
      /*enable the ip.  The ip is disabled and enabled for each transaction.*/
      status = alt_avalon_i2c_enable(i2c_dev);
	  
	  
      if (status != ALT_AVALON_I2C_SUCCESS)
      {
		MTXLOCK_STDIO();	
	    printf("error after enable function \n");
	    MTXUNLOCK_STDIO();
        return status;
      }

      /*Clear the ISR reg*/ 
      alt_avalon_i2c_int_clear(i2c_dev,ALT_AVALON_I2C_ISR_ALL_CLEARABLE_INTS_MSK);
    }

	//ADDED TO ENABLE TO TX_ready BIT OF ISR
	alt_avalon_i2c_isr_enable(i2c_dev, ALT_AVALON_I2C_ISR_TX_READY_MSK);
	
    /*Start Write, transmit address. */
    status = alt_avalon_i2c_send_address(i2c_dev,ALT_AVALON_I2C_WRITE,issue_restart);  
	
	/*MTXLOCK_STDIO();	
	printf("Status after send_address : %d \n", status); 	//sends 0 (False or Success)
	MTXUNLOCK_STDIO();*/
	  
    if (status == ALT_AVALON_I2C_SUCCESS)
    {
        while ((size > 1) && (status == ALT_AVALON_I2C_SUCCESS))
        {
            status = alt_avalon_i2c_cmd_write(i2c_dev, *buffer, ALT_AVALON_I2C_NO_RESTART, ALT_AVALON_I2C_NO_STOP);
            /*MTXLOCK_STDIO();	
			printf("Busy writing to cmd_write loop n.%d \n", size);
			MTXUNLOCK_STDIO();*/
            ++buffer;
            --size;
        }

        /* Last byte */
        if (status == ALT_AVALON_I2C_SUCCESS)
        {
			/*MTXLOCK_STDIO();	
			printf("Writing the last byte \n");
			MTXUNLOCK_STDIO();*/
            status = alt_avalon_i2c_cmd_write(i2c_dev, *buffer, ALT_AVALON_I2C_NO_RESTART, issue_stop);

            ++buffer;
            --size;
        }
    }
	
	if (status != ALT_AVALON_I2C_SUCCESS)
    {
		MTXLOCK_STDIO();	
	    printf("status error after transaction \n");
	    MTXUNLOCK_STDIO();
		return status;
    }
    
    /*if end of transaction, wait until the ip is idle then disable the ip*/
    if ((issue_stop) || (status != ALT_AVALON_I2C_SUCCESS)) 
    {

        while (alt_avalon_i2c_is_busy(i2c_dev))
        {
			/*MTXLOCK_STDIO();	
			printf("ISSUE : busy\n");
			MTXUNLOCK_STDIO();*/
            if (timeout<10) {
				MTXLOCK_STDIO();	
				printf("TSKsleep8 \n");
				MTXUNLOCK_STDIO();
				TSKsleep(OS_MS_TO_TICK(10));	//alt_busy_sleep(10000);
			}
            if (--timeout == 0)
            {
               status = ALT_AVALON_I2C_TIMEOUT;
               break;
            }
        }
     
        /*check for a nack error*/
        alt_avalon_i2c_check_nack(i2c_dev,&status);
        
        /*disable the ip.  The ip is disabled and enabled for each transaction.*/
        alt_avalon_i2c_disable(i2c_dev);
    }


    return status;
}

/*This function issues a write command and transmits data to the I2C bus  */
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_master_transmit_using_interrupts(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                        const alt_u8 * buffer,
                                        alt_u32 size,
                                        const alt_u8 issue_restart,
                                        const alt_u8 issue_stop)
{
    ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;
    alt_u32 timeout=size*10000;
    IRQ_DATA_t *irq_data = i2c_dev->callback_context;    
    
    if (size==0)
    {
      return ALT_AVALON_I2C_SUCCESS;
    }
    
    /*IS the optional interrupt handler registered??*/
    if (i2c_dev->callback != optional_irq_callback)
    {
       return ALT_AVALON_I2C_BAD_ARG;    
    }
    
    /*if a new transaction, enable ip and clear int status*/
    if (!issue_restart) 
    {
      /*enable the ip.  The ip is disabled and enabled for each transaction.*/
      status = alt_avalon_i2c_enable(i2c_dev);
      if (status != ALT_AVALON_I2C_SUCCESS)
      {
        return status;
      }

      /*Clear the ISR reg*/
      alt_avalon_i2c_int_clear(i2c_dev,ALT_AVALON_I2C_ISR_ALL_CLEARABLE_INTS_MSK);
    }

    /*Start Write, transmit address. */
    status = alt_avalon_i2c_send_address(i2c_dev,ALT_AVALON_I2C_WRITE,issue_restart);
        
    if (status == ALT_AVALON_I2C_SUCCESS)
    {
        while ((size > 1) && (status == ALT_AVALON_I2C_SUCCESS))
        {
            status = alt_avalon_i2c_cmd_write(i2c_dev, *buffer, ALT_AVALON_I2C_NO_RESTART, ALT_AVALON_I2C_NO_STOP);
            
            ++buffer;
            --size;
        }

        /* Last byte */
        if (status == ALT_AVALON_I2C_SUCCESS)
        {
            status = alt_avalon_i2c_cmd_write(i2c_dev, *buffer, ALT_AVALON_I2C_NO_RESTART, issue_stop);

            ++buffer;
            --size;
        }
    }
    
    /*if error, wait until the ip is idle then disable the ip*/
    if (status != ALT_AVALON_I2C_SUCCESS) 
    {

        while (alt_avalon_i2c_is_busy(i2c_dev))
        {
            if (timeout<10) {
				MTXLOCK_STDIO();	
				printf("TSKsleep9 \n");
				MTXUNLOCK_STDIO();
				TSKsleep(OS_MS_TO_TICK(10)); //alt_busy_sleep(10000);  
			}				
            if (--timeout == 0)
            {
               status = ALT_AVALON_I2C_TIMEOUT;
               break;
            }
        }
     
        /*disable the ip.  The ip is disabled and enabled for each transaction.*/
        alt_avalon_i2c_disable(i2c_dev);
    }
    else
    {
       if (issue_stop)
       {
         /* clear ISR register content */
         alt_avalon_i2c_int_clear(i2c_dev,ALT_AVALON_I2C_ISR_ALL_CLEARABLE_INTS_MSK);
         /* set the cmd fifo threshold */
         alt_avalon_i2c_tfr_cmd_fifo_threshold_set(i2c_dev,ALT_AVALON_I2C_TFR_CMD_FIFO_EMPTY);
         /* set the interrupt transaction busy bit */
         irq_data->irq_busy=1;
         /* enable the TX_READY interrupt */
         alt_avalon_i2c_int_enable(i2c_dev,ALT_AVALON_I2C_ISER_TX_READY_EN_MSK);
       }
    }
    
    return status;
}

/*This function receives one or more data bytes transmitted from a slave in 
 * response to read requests issued from this master. */
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_master_receive(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                       alt_u8 * buffer,
                                       const alt_u32 size,
                                       const alt_u8 issue_restart,
                                       const alt_u8 issue_stop)
{
    ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;
    alt_u32 timeout;
    alt_u32 bytes_read=0;
    alt_u32 bytes_written=0;
    alt_u32 temp_bytes_read;
    
    if (size==0)
    {
      return ALT_AVALON_I2C_SUCCESS;
    }
    
    /*if a new transaction, enable ip and clear int status*/
    if (!issue_restart) 
    {
       /*enable the ip.  The ip is disabled and enabled for each transaction.*/
      status = alt_avalon_i2c_enable(i2c_dev);
      if (status != ALT_AVALON_I2C_SUCCESS)
      {
        return status;
      }

      /*Clear the ISR reg*/
      alt_avalon_i2c_int_clear(i2c_dev,ALT_AVALON_I2C_ISR_ALL_CLEARABLE_INTS_MSK);
    }

    /*Start Write, transmit address. */
    status = alt_avalon_i2c_send_address(i2c_dev,ALT_AVALON_I2C_READ,issue_restart);

    if (status == ALT_AVALON_I2C_SUCCESS)
    {
        while ((bytes_written < (size-1)) && (status == ALT_AVALON_I2C_SUCCESS))
        {
			
			status = alt_avalon_i2c_cmd_write(i2c_dev, 0, ALT_AVALON_I2C_NO_RESTART, ALT_AVALON_I2C_NO_STOP);
            bytes_written++;
            if (status == ALT_AVALON_I2C_SUCCESS)
            {
               alt_avalon_i2c_rx_read_available(i2c_dev, buffer,0,&temp_bytes_read);
               buffer+=temp_bytes_read;
               bytes_read+=temp_bytes_read;
            }
        }

        /* Last byte */
        if (status == ALT_AVALON_I2C_SUCCESS)
        {
            status = alt_avalon_i2c_cmd_write(i2c_dev, 0, ALT_AVALON_I2C_NO_RESTART, issue_stop);
        }
    }
    
    while ((bytes_read < size) && (status==ALT_AVALON_I2C_SUCCESS)) 
    {
        status=alt_avalon_i2c_rx_read(i2c_dev, buffer);
        buffer++;
        bytes_read++;
    }

    /*if end of transaction, wait until the ip is idle then disable the ip*/
    if ((issue_stop) || (status != ALT_AVALON_I2C_SUCCESS)) 
    {
        timeout=10000 * size;
        while (alt_avalon_i2c_is_busy(i2c_dev))
        {
            if (timeout<10) {
				MTXLOCK_STDIO();	
				printf("TSKsleep10 \n");
				MTXUNLOCK_STDIO();
				TSKsleep(OS_MS_TO_TICK(10)); //alt_busy_sleep(10000);
			}
            if (--timeout == 0)
            {
               status = ALT_AVALON_I2C_TIMEOUT;
               break;
            }
        }

        /*check for nack error*/
        alt_avalon_i2c_check_nack(i2c_dev,&status);    
        
        /*disable the ip.  The ip is disabled and enabled for each transaction.*/
        alt_avalon_i2c_disable(i2c_dev);
    }

    return status;
}

/*This function receives one or more data bytes transmitted from a slave in 
 * response to read requests issued from this master.  Uses the optional interrupt routine. */
ALT_AVALON_I2C_STATUS_CODE alt_avalon_i2c_master_receive_using_interrupts(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                       alt_u8 * buffer,
                                       const alt_u32 size,
                                       const alt_u8 issue_restart,
                                       const alt_u8 issue_stop)
{
    ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;
    IRQ_DATA_t *irq_data = i2c_dev->callback_context;    
    alt_u32 timeout;
    alt_u32 bytes_written=0;
    
    if (size==0)
    {
      return ALT_AVALON_I2C_SUCCESS;
    }
    
    /*Is the optional interrupt handler registered??*/
    if (i2c_dev->callback != optional_irq_callback)
    {
       return ALT_AVALON_I2C_BAD_ARG;    
    }
    
    /*if a new transaction, enable ip and clear int status*/
    if (!issue_restart) 
    {
      /*enable the ip.  The ip is disabled and enabled for each transaction.*/
      status = alt_avalon_i2c_enable(i2c_dev);
      if (status != ALT_AVALON_I2C_SUCCESS)
      {
        return status;
      }

      /*Clear the ISR reg*/
      alt_avalon_i2c_int_clear(i2c_dev,ALT_AVALON_I2C_ISR_ALL_CLEARABLE_INTS_MSK);
      
    }

    /*Start Write, transmit address. */
    status = alt_avalon_i2c_send_address(i2c_dev,ALT_AVALON_I2C_READ,issue_restart);

    if (status == ALT_AVALON_I2C_SUCCESS)
    {
        while ((bytes_written < (size-1)) && (status == ALT_AVALON_I2C_SUCCESS))
        {
            status = alt_avalon_i2c_cmd_write(i2c_dev, 0, ALT_AVALON_I2C_NO_RESTART, ALT_AVALON_I2C_NO_STOP);
            bytes_written++;
        }

        /* Last byte */
        if (status == ALT_AVALON_I2C_SUCCESS)
        {
            status = alt_avalon_i2c_cmd_write(i2c_dev, 0, ALT_AVALON_I2C_NO_RESTART, issue_stop);
        }
    }
    
    /*if error, wait until the ip is idle then disable the ip*/
    if (status != ALT_AVALON_I2C_SUCCESS) 
    {
        timeout=10000 * size;
        while (alt_avalon_i2c_is_busy(i2c_dev))
        {
            if (timeout<10) {
				MTXLOCK_STDIO();	
				printf("TSKsleep11 \n");
				MTXUNLOCK_STDIO();
				TSKsleep(OS_MS_TO_TICK(10)); //alt_busy_sleep(10000);
			}
            if (--timeout == 0)
            {
               status = ALT_AVALON_I2C_TIMEOUT;
               break;
            }
        }
     
        /*disable the ip.  The ip is disabled and enabled for each transaction.*/
        alt_avalon_i2c_disable(i2c_dev);
    }
    else
    {
       if (issue_stop)
       {
         /* clear ISR register content */
         alt_avalon_i2c_int_clear(i2c_dev,ALT_AVALON_I2C_ISR_ALL_CLEARABLE_INTS_MSK);
         /* set the cmd fifo threshold */
         alt_avalon_i2c_rx_fifo_threshold_set(i2c_dev,ALT_AVALON_I2C_RX_DATA_FIFO_1_ENTRY);
         /* set the interrupt transaction busy bit  2 = receive */
         irq_data->irq_busy=2;
         
         irq_data->buffer = buffer;
         irq_data->size = size;
         
         /* enable the RX_READY interrupt */
         alt_avalon_i2c_int_enable(i2c_dev,ALT_AVALON_I2C_ISER_RX_READY_EN_MSK);
       }
    }

    return status;
}

/* Returns the current I2C controller interrupt status conditions. */
void alt_avalon_i2c_int_status_get(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                       alt_u32 *status)
{
	void *ALT_AVALON_I2C_ISR = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_ISR_REG;
	void *ALT_AVALON_I2C_ISER = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_ISER_REG;
	*status = ( alt_read_word(ALT_AVALON_I2C_ISR)  &   alt_read_word(ALT_AVALON_I2C_ISER)   );
}

/*Returns the I2C controller raw interrupt status conditions irrespective of
 * the interrupt status condition enablement state. */
void alt_avalon_i2c_int_raw_status_get(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                           alt_u32 *status)
{
	void *ALT_AVALON_I2C_ISR = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_ISR_REG;
	*status = ( alt_read_word(ALT_AVALON_I2C_ISR));
}

/*Clears the specified I2C controller interrupt status conditions identified
 * in the mask. */
void alt_avalon_i2c_int_clear(ALT_AVALON_I2C_DEV_t *i2c_dev, const alt_u32 mask)
{
    void *ALT_AVALON_I2C_ISR = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_ISR_REG;
	alt_write_word(ALT_AVALON_I2C_ISR, mask);
	
}

/*Disable the specified I2C controller interrupt status conditions identified in
 * the mask. */
void alt_avalon_i2c_int_disable(ALT_AVALON_I2C_DEV_t *i2c_dev, const alt_u32 mask)
{
   alt_u32 enabled_ints;
    
   alt_avalon_i2c_enabled_ints_get(i2c_dev,&enabled_ints);
   enabled_ints &=  (~mask);
   void *ALT_AVALON_I2C_ISER = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_ISER_REG;
   alt_write_word(ALT_AVALON_I2C_ISER, ALT_AVALON_I2C_ISR_ALLINTS_MSK & enabled_ints);
   
}

/*Enable the specified I2C controller interrupt status conditions identified in
 * the mask. */
void alt_avalon_i2c_int_enable(ALT_AVALON_I2C_DEV_t *i2c_dev, const alt_u32 mask)
{
    alt_u32 enabled_ints;
    
    alt_avalon_i2c_enabled_ints_get(i2c_dev,&enabled_ints);
    enabled_ints |= mask;
	void *ALT_AVALON_I2C_ISER = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_ISER_REG;
	alt_write_word(ALT_AVALON_I2C_ISER, ALT_AVALON_I2C_ISR_ALLINTS_MSK & enabled_ints);

}

/*gets the enabled i2c interrupts. */
void alt_avalon_i2c_enabled_ints_get(ALT_AVALON_I2C_DEV_t *i2c_dev, alt_u32 * enabled_ints)
{
	void *ALT_AVALON_I2C_ISER = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_ISER_REG;
	*enabled_ints= alt_read_word(ALT_AVALON_I2C_ISER) & ALT_AVALON_I2C_ISR_ALLINTS_MSK;

}

/*Gets the current receive FIFO threshold level value. */			
void alt_avalon_i2c_rx_fifo_threshold_get(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                              ALT_AVALON_I2C_RX_DATA_FIFO_THRESHOLD_t *threshold)
{
	void *ALT_AVALON_I2C_CTRL = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_CTRL_REG;
	*threshold = ( alt_read_word(ALT_AVALON_I2C_CTRL) & ALT_AVALON_I2C_CTRL_RX_DATA_FIFO_THD_MSK) >>  ALT_AVALON_I2C_CTRL_RX_DATA_FIFO_THD_OFST;

}

/*sets the current receive FIFO threshold level value. */		
void alt_avalon_i2c_rx_fifo_threshold_set(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                              const ALT_AVALON_I2C_RX_DATA_FIFO_THRESHOLD_t threshold)
{

	void *ALT_AVALON_I2C_CTRL = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_CTRL_REG;
    unsigned long current_CTRL = alt_read_word(ALT_AVALON_I2C_CTRL);
	if ((threshold << ALT_AVALON_I2C_CTRL_RX_DATA_FIFO_THD_OFST) == 0b000000) {
		alt_write_word(ALT_AVALON_I2C_CTRL, current_CTRL & ~ALT_AVALON_I2C_CTRL_RX_DATA_FIFO_THD_MSK);
	}
	else if ((threshold << ALT_AVALON_I2C_CTRL_RX_DATA_FIFO_THD_OFST) == 0b010000) {
		alt_write_word(ALT_AVALON_I2C_CTRL, ( current_CTRL & ~(0b100000) ) | 0b010000 );
	}
	else if ((threshold << ALT_AVALON_I2C_CTRL_RX_DATA_FIFO_THD_OFST) == 0b100000) {
		alt_write_word(ALT_AVALON_I2C_CTRL, ( current_CTRL & ~(0b010000) ) | 0b100000 );
	}
	else if ((threshold << ALT_AVALON_I2C_CTRL_RX_DATA_FIFO_THD_OFST) == 0b110000) {
		alt_write_word(ALT_AVALON_I2C_CTRL, current_CTRL | ALT_AVALON_I2C_CTRL_RX_DATA_FIFO_THD_MSK);
	}

}

/*Gets the current Transfer Command FIFO threshold level value.*/	
void alt_avalon_i2c_tfr_cmd_fifo_threshold_get(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                              ALT_AVALON_I2C_TFR_CMD_FIFO_THRESHOLD_t *threshold)
{
	void *ALT_AVALON_I2C_CTRL = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_CTRL_REG;
	*threshold = (alt_read_word(ALT_AVALON_I2C_CTRL) & ALT_AVALON_I2C_CTRL_TFR_CMD_FIFO_THD_MSK) >> ALT_AVALON_I2C_CTRL_TFR_CMD_FIFO_THD_OFST;

}

/*Sets the current Transfer Command FIFO threshold level value.*/	
void alt_avalon_i2c_tfr_cmd_fifo_threshold_set(ALT_AVALON_I2C_DEV_t *i2c_dev,
                                              const ALT_AVALON_I2C_TFR_CMD_FIFO_THRESHOLD_t threshold)
{
	
	void *ALT_AVALON_I2C_CTRL = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_CTRL_REG;
    unsigned long current_CTRL = alt_read_word(ALT_AVALON_I2C_CTRL);
	if ((threshold << ALT_AVALON_I2C_CTRL_TFR_CMD_FIFO_THD_OFST) == 0b0000) {
		alt_write_word(ALT_AVALON_I2C_CTRL, current_CTRL & ~ALT_AVALON_I2C_CTRL_TFR_CMD_FIFO_THD_MSK);
	}
	else if ((threshold << ALT_AVALON_I2C_CTRL_TFR_CMD_FIFO_THD_OFST) == 0b0100) {
		alt_write_word(ALT_AVALON_I2C_CTRL, ( current_CTRL & ~(0b1000) ) | 0b0100 );
	}
	else if ((threshold << ALT_AVALON_I2C_CTRL_TFR_CMD_FIFO_THD_OFST) == 0b1000) {
		alt_write_word(ALT_AVALON_I2C_CTRL, ( current_CTRL & ~(0b0100) ) | 0b1000 );
	}
	else if ((threshold << ALT_AVALON_I2C_CTRL_TFR_CMD_FIFO_THD_OFST) == 0b1100) {
		alt_write_word(ALT_AVALON_I2C_CTRL, current_CTRL | ALT_AVALON_I2C_CTRL_TFR_CMD_FIFO_THD_MSK);
	}

}

//ADDED FUNCTION
/*Enables bits from the ISR register*/
void alt_avalon_i2c_isr_enable(ALT_AVALON_I2C_DEV_t *i2c_dev, const alt_u32 mask)
{
    alt_u32 enabled_isr;
    
    alt_avalon_i2c_enabled_isr_get(i2c_dev,&enabled_isr);
    enabled_isr |= mask;
	void *ALT_AVALON_I2C_ISR = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_ISR_REG;
	alt_write_word(ALT_AVALON_I2C_ISR, ALT_AVALON_I2C_ISR_ALLINTS_MSK & enabled_isr);
	
	/*MTXLOCK_STDIO();	
	printf("TX_ready bit : %d \n", (alt_read_word(ALT_AVALON_I2C_ISR) & ALT_AVALON_I2C_ISR_TX_READY_MSK));
	MTXUNLOCK_STDIO();*/

}

//ADDED FUNCTION
/*Retrieves enables ISR bits*/
void alt_avalon_i2c_enabled_isr_get(ALT_AVALON_I2C_DEV_t *i2c_dev, alt_u32 * enabled_isr)
{
	void *ALT_AVALON_I2C_ISR = i2c_dev->i2c_base + (alt_u32)ALT_AVALON_I2C_ISR_REG;
	*enabled_isr= alt_read_word(ALT_AVALON_I2C_ISR) & ALT_AVALON_I2C_ISR_ALLINTS_MSK;

}

// ---------------------------------------------------------------------------------
// COLOR SENSOR functions
// ---------------------------------------------------------------------------------

//COLOR SENSOR
//Enables the Color Sensor
ALT_AVALON_I2C_STATUS_CODE enabling_ColorSensor(ALT_AVALON_I2C_DEV_t *i2c_dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg)
{
	alt_avalon_i2c_change_config_color_sensor(i2c_dev, cfg);
	
	ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;	
	
	unsigned char txbuffer[0x10];
	
	//COMMAND (for ENABLE register) : 1 00 address = 1 00 00000 = 128 = 0x80
	//DATA (for ENABLE register) : 000 0 0 0 1 1 = 3 = 0x03
	txbuffer[0] = 0x80;	//COMMAND
	txbuffer[1] = 0x03;	//DATA
	
	status = alt_avalon_i2c_master_tx(i2c_dev, txbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);
	if (status != ALT_AVALON_I2C_SUCCESS) 	{
		MTXLOCK_STDIO();	
		printf("Enabling of Color Sensor FAILED \n");		
		MTXUNLOCK_STDIO();  	}
	else 	{
		MTXLOCK_STDIO();	
		printf("Color Sensor Enabled \n");		
		MTXUNLOCK_STDIO();  	}
	return status;
}

//COLOR SENSOR
//Reads ID of the Color Sensor
void reading_ID_ColorSensor(ALT_AVALON_I2C_DEV_t *i2c_dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg) 
{
	alt_avalon_i2c_change_config_color_sensor(i2c_dev, cfg);
	
	ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;	
	// READING THE ID OF THE COLORSENSOR
	//COMMAND (for ID register) : 1 00 address = 1 00 0x12 = 1 01 (18) = 1 01 10010 = 178 = 0xB2
	unsigned char txbuffer[0x08];
	unsigned char rxbuffer[0x08];
	txbuffer[0] = 0xB2;
	status = alt_avalon_i2c_master_tx_rx(i2c_dev, txbuffer, 1, rxbuffer, 1, ALT_AVALON_I2C_NO_INTERRUPTS);  
	if (status == ALT_AVALON_I2C_SUCCESS) 	
	{
		MTXLOCK_STDIO();	
		printf("ID Color Sensor (hex) : %x \n", rxbuffer[0]);		
		MTXUNLOCK_STDIO(); 	
	}
}

/*//COLOR SENSOR
//Returns the color that has been measured
BOARD_COLOR_t read_color(ALT_AVALON_I2C_DEV_t *i2c_dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg)
{
	alt_avalon_i2c_change_config_color_sensor(i2c_dev, cfg);

	ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;	
	unsigned char txbuffer[0x08];
	unsigned char rxbuffer[0x10];
		
	//COMMAND (for RDATAL and RDATAH register) : 1 01 address
	//CLEAR : 1 01 0x14 = 160 + 0x14 = 160 + 20 = 180	= 0xB6
	//RED 	:						 = 160 + 22 = 182
	//GREEN : 						 = 160 + 24 = 184
	//BLUE  :						 = 160 + 26 = 186
		
	txbuffer[0] = 180 + 2*COLOR_SENSOR_CLEAR;
	status = alt_avalon_i2c_master_tx_rx(i2c_dev, txbuffer, 1, rxbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);  
	if (status == ALT_AVALON_I2C_SUCCESS) 	
	{
		unsigned short value = rxbuffer[0] | (rxbuffer[1] << 8);
		MTXLOCK_STDIO();	
		printf("Color Data : %d \n", value);		
		MTXUNLOCK_STDIO(); 
		if (value < 8000 ) {
			MTXLOCK_STDIO();	
			printf("Black : %d \n", value);		
			MTXUNLOCK_STDIO(); 
			return BLACK;
		}
		else if ((value > 8000) & (value < 12000)) {
			MTXLOCK_STDIO();	
			printf("Blue : %d \n", value);		
			MTXUNLOCK_STDIO(); 
			return BLUE;
		}
		else if ((value > 12000) & (value < 14000)) {
			MTXLOCK_STDIO();	
			printf("White : %d \n", value);		
			MTXUNLOCK_STDIO(); 
			return WHITE;
		}
		else if (value > 14000) {
			MTXLOCK_STDIO();	
			printf("Yellow : %d \n", value);		
			MTXUNLOCK_STDIO(); 
			return YELLOW;
		}
	}
	else {
		MTXLOCK_STDIO();	
		printf("ERROR reading color data - Step1 \n");		
		MTXUNLOCK_STDIO(); 
		return NO_COLOR;
	}
	
	//CHANGE FUNCTION SUCH THAT THE FIRST TIME THIS FUNCTION IS CALLED, tx_rx IS CALLED
	//BUT AFTERWARDS JUST rx IS CALLED
		//status = alt_avalon_i2c_master_rx(i2c_dev, rxbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);

	//ADD OTHER COLORS : the 3 lighter shades of blue, red, gree, dark yellow, 
}*/

/*//COLOR SENSOR
//Returns the color that has been measured
BOARD_COLOR_t read_color(ALT_AVALON_I2C_DEV_t *i2c_dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg)
{
	alt_avalon_i2c_change_config_color_sensor(i2c_dev, cfg);

	ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;	
	unsigned char txbuffer[0x08];
	unsigned char rxbuffer[0x10];
	
	unsigned short clear;
	unsigned short red;
	unsigned short green;
	unsigned short blue;
	
		
	//COMMAND (for RDATAL and RDATAH register) : 1 01 address
	//CLEAR : 1 01 0x14 = 160 + 0x14 = 160 + 20 = 180	= 0xB6
	//RED 	:						 = 160 + 22 = 182
	//GREEN : 						 = 160 + 24 = 184
	//BLUE  :						 = 160 + 26 = 186
		
	txbuffer[0] = 180 + 2*COLOR_SENSOR_CLEAR;
	status = alt_avalon_i2c_master_tx_rx(i2c_dev, txbuffer, 1, rxbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);  
	clear = rxbuffer[0] | (rxbuffer[1] << 8);
	if (status != ALT_AVALON_I2C_SUCCESS) {
		MTXLOCK_STDIO();	
		printf("ERROR reading color data\n");		
		MTXUNLOCK_STDIO(); 
		return NO_COLOR;
	}
	else {
		MTXLOCK_STDIO();	
		printf("CLEAR Color Data : %d \n", clear);		
		MTXUNLOCK_STDIO(); 
	}
	
	txbuffer[0] = 180 + 2*COLOR_SENSOR_RED;
	status = alt_avalon_i2c_master_tx_rx(i2c_dev, txbuffer, 1, rxbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);  
	red = rxbuffer[0] | (rxbuffer[1] << 8);
	if (status != ALT_AVALON_I2C_SUCCESS) {
		MTXLOCK_STDIO();	
		printf("ERROR reading color data\n");		
		MTXUNLOCK_STDIO(); 
		return NO_COLOR;
	}
	else {
		MTXLOCK_STDIO();	
		printf("RED Color Data : %d \n", red);		
		MTXUNLOCK_STDIO(); 
	}
	
	txbuffer[0] = 180 + 2*COLOR_SENSOR_GREEN;
	status = alt_avalon_i2c_master_tx_rx(i2c_dev, txbuffer, 1, rxbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);  
	green = rxbuffer[0] | (rxbuffer[1] << 8);
	if (status != ALT_AVALON_I2C_SUCCESS) {
		MTXLOCK_STDIO();	
		printf("ERROR reading color data\n");		
		MTXUNLOCK_STDIO(); 
		return NO_COLOR;
	}
	else {
		MTXLOCK_STDIO();	
		printf("GREEN Color Data : %d \n", green);		
		MTXUNLOCK_STDIO(); 
	}
	
	txbuffer[0] = 180 + 2*COLOR_SENSOR_BLUE;
	status = alt_avalon_i2c_master_tx_rx(i2c_dev, txbuffer, 1, rxbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);  
	blue = rxbuffer[0] | (rxbuffer[1] << 8);
	if (status != ALT_AVALON_I2C_SUCCESS) {
		MTXLOCK_STDIO();	
		printf("ERROR reading color data\n");		
		MTXUNLOCK_STDIO(); 
		return NO_COLOR;
	else {
		MTXLOCK_STDIO();	
		printf("BLUE Data : %d \n", blue);		
		MTXUNLOCK_STDIO(); 

		}
	}
	return BLUE; //just for now
	
	//CHANGE FUNCTION SUCH THAT THE FIRST TIME THIS FUNCTION IS CALLED, tx_rx IS CALLED
	//BUT AFTERWARDS JUST rx IS CALLED
		//status = alt_avalon_i2c_master_rx(i2c_dev, rxbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);

}*/

// ---------------------------------------------------------------------------------
// SONAR functions
// ---------------------------------------------------------------------------------

//SONAR
//Changes the ADDRESS of the sonar
ALT_AVALON_I2C_STATUS_CODE changing_address_sonar(ALT_AVALON_I2C_DEV_t *i2c_dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg, ALT_AVALON_SONAR_NB_t sonar, unsigned char new_addr)
{
	alt_avalon_i2c_change_config_sonar(i2c_dev, cfg, sonar);
	
	ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;	
	
	unsigned char txbuffer[0x10];
	
	txbuffer[0] = 0x00;		//
	txbuffer[1] = 0xA0;
	status = alt_avalon_i2c_master_tx(i2c_dev, txbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);
	
	txbuffer[0] = 0x00;		//
	txbuffer[1] = 0xAA;
	status = alt_avalon_i2c_master_tx(i2c_dev, txbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);
	
	txbuffer[0] = 0x00;		//
	txbuffer[1] = 0xA5;
	status = alt_avalon_i2c_master_tx(i2c_dev, txbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);
	
	txbuffer[0] = 0x00;		//
	txbuffer[1] = new_addr;
	status = alt_avalon_i2c_master_tx(i2c_dev, txbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);
	
	
	if (status != ALT_AVALON_I2C_SUCCESS) 	{
		MTXLOCK_STDIO();	
		printf("Changing ADDRESS FAILED \n");		
		MTXUNLOCK_STDIO();  	}
	else 	{
		MTXLOCK_STDIO();	
		printf("New ADDRESS : %x [mm]\n", new_addr);		
		MTXUNLOCK_STDIO();  	}
	return status;
}

//SONAR
//Measures and returns the range in CENTIMETERS
int range_sonar(ALT_AVALON_I2C_DEV_t *i2c_dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg, ALT_AVALON_SONAR_NB_t sonar, CtrlStruct *cvs)
{
	alt_avalon_i2c_change_config_sonar(i2c_dev, cfg, sonar);
	
	ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;	
	
	unsigned char txbuffer[0x10];
	unsigned char rxbuffer[0x10];
	
	txbuffer[0] = 0x00;		//command register
	txbuffer[1] = 0x51;		//result in centimeters
	status = alt_avalon_i2c_master_tx(i2c_dev, txbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);
	int revision_nb;
	revision_nb = reading_software_revision2(i2c_dev);
	while (revision_nb == 255) {
	   revision_nb = reading_software_revision2(i2c_dev);
	}
	txbuffer[0] = 0x03;		//read Range Low Byte
	status = alt_avalon_i2c_master_tx_rx(i2c_dev, txbuffer, 1, rxbuffer, 1, ALT_AVALON_I2C_NO_INTERRUPTS);  
	if (status == ALT_AVALON_I2C_SUCCESS) 	{
		if (sonar == LEFT_SONAR) {
//			MTXLOCK_STDIO();
//			printf("Range of LEFT sonar : %d [cm]\n", rxbuffer[0] );
//			MTXUNLOCK_STDIO();
			return rxbuffer[0];
		}
		else if (sonar == RIGHT_SONAR) {
//			MTXLOCK_STDIO();
//			printf("\nRange of RIGHT sonar : %d [cm]\n", rxbuffer[0] );
//			MTXUNLOCK_STDIO();
			return rxbuffer[0];
		}
	}
	else {
		MTXLOCK_STDIO();	
		printf("ERROR reading range data \n");		
		MTXUNLOCK_STDIO(); 
	}
	
	return -1;
}

//SONAR
//Changes the max analogue gain
ALT_AVALON_I2C_STATUS_CODE changing_gain(ALT_AVALON_I2C_DEV_t *i2c_dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg, int gain, ALT_AVALON_SONAR_NB_t sonar)
{
	alt_avalon_i2c_change_config_sonar(i2c_dev, cfg, sonar);
	
	ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;	
	unsigned char txbuffer[0x10];
	
	//COMMAND (for GAIN register) : location = 00000010 = 2 = 0x02
	//DATA (for GAIN register) : [40,700]
	txbuffer[0] = 1;		//COMMAND
	txbuffer[1] = gain;		//DATA		
	
	status = alt_avalon_i2c_master_tx(i2c_dev, txbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);
	if (status != ALT_AVALON_I2C_SUCCESS) 	{
		MTXLOCK_STDIO();	
		printf("Changing gain FAILED \n");		
		MTXUNLOCK_STDIO();  	}
	else 	{
		MTXLOCK_STDIO();	
		printf("New gain of SONAR : %d \n", gain);		
		MTXUNLOCK_STDIO();  	}
	return status;

}

//SONAR
//Reads the software revision, and sends back value
int reading_software_revision2(ALT_AVALON_I2C_DEV_t *i2c_dev)
{

    ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;

    unsigned char txbuffer[0x08];
    unsigned char rxbuffer[0x08];

    txbuffer[0] = 0x00;

    status = alt_avalon_i2c_master_tx_rx(i2c_dev, txbuffer, 1, rxbuffer, 1, ALT_AVALON_I2C_NO_INTERRUPTS);

    /*if (status == ALT_AVALON_I2C_SUCCESS)
    {
        MTXLOCK_STDIO();
        printf("Software revision : %d \n", rxbuffer[0] );
        MTXUNLOCK_STDIO();
        return rxbuffer[0];
    }
    else {
        MTXLOCK_STDIO();
        printf("ERROR reading revision software \n");
        MTXUNLOCK_STDIO();
    }*/

    return rxbuffer[0];
}

//SONAR
//Changes the accessible range of the sonar
ALT_AVALON_I2C_STATUS_CODE changing_range(ALT_AVALON_I2C_DEV_t *i2c_dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg, int range, ALT_AVALON_SONAR_NB_t sonar)
{
	alt_avalon_i2c_change_config_sonar(i2c_dev, cfg, sonar);
	
	ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;	
	
	//default range = 11m
	int value = (range - 43)/43;	//computes the value to put in the register 
	
	unsigned char txbuffer[0x10];
	
	//COMMAND (for RANGE register) : location = 00000010 = 2 = 0x02
	//DATA (for RANGE register) : 000 0 0 0 1 1 = 3 = 0x03
	txbuffer[0] = 2;		//COMMAND
	txbuffer[1] = value;	//DATA
	
	status = alt_avalon_i2c_master_tx(i2c_dev, txbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);
	if (status != ALT_AVALON_I2C_SUCCESS) 	{
		MTXLOCK_STDIO();	
		printf("Changing range FAILED \n");		
		MTXUNLOCK_STDIO();  	}
	else 	{
		MTXLOCK_STDIO();	
		printf("New range of SONAR: %d [mm]\n", range);		
		MTXUNLOCK_STDIO();  	}
	return status;
}

// ---------------------------------------------------------------------------------
// MICROSWITCH functions
// ---------------------------------------------------------------------------------

//MICROSWITCH
//Reads and returns the data from a chosen microswitch
//returns 0 if microswitch is OFF; 1 if ON
// !!!! VERIFY THE MICROSWITCH NUMBERS !!!!
int read_state_microswitch(ALT_AVALON_I2C_DEV_t *i2c_dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg, ALT_AVALON_MICROSWITCH_NB_t microswitch)
{
	alt_avalon_i2c_change_config_microswitch(i2c_dev, cfg);
	
	ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;	
	unsigned char rxbuffer[0x08];
			
	status = alt_avalon_i2c_master_rx(i2c_dev, rxbuffer, 1, ALT_AVALON_I2C_NO_INTERRUPTS);  
	
	if (status == ALT_AVALON_I2C_SUCCESS) 	
	{
		unsigned char microRow = rxbuffer[0] & 0b00111111;
		unsigned char stateMicro = microRow & microswitch;
		if (stateMicro == 0) {
			MTXLOCK_STDIO();	
			printf("microswitches is OFF \n");		
			MTXUNLOCK_STDIO(); 
			return 0;
		}
		else {
			MTXLOCK_STDIO();	
			printf("microswitch is ON \n");		
			MTXUNLOCK_STDIO(); 
			return 1;
		}
	}
	else {
		MTXLOCK_STDIO();	
		printf("ERROR reading microswitch data \n");		
		MTXUNLOCK_STDIO(); 
		return -1;
	}
}

// ---------------------------------------------------------------------------------
// STEPPER functions
// ---------------------------------------------------------------------------------

//STEPPER - CHANGE
//Tests the stepper 
ALT_AVALON_I2C_STATUS_CODE test_function_stepper(ALT_AVALON_I2C_DEV_t *i2c_dev)
{
	//REGISTER0 ADDRESS = 000xxxxx (0 to 17)
	//REGISTER1 CONTROL ST1 = 000 1 1 0 0 1 = 16 + 8 + 1 = 25
	//REGISTER2 STEP NUMBER MSB ST1 = 00000000 = 0b0
	//REGISTER3 STEP NUMBER LSB ST1 = 00001000 = 8
	//REGISTER4 SPEED 0.25ms	= 00000001
	
	ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;	
	
	unsigned char txbuffer[0x28];
	unsigned char rxbuffer[0x08];
	
	txbuffer[0] = 0x01;			//R0		register choice
	txbuffer[1] = 0b00000101;	//R1		control
	txbuffer[2] = 0xf0;			//R2		MSB step number
	txbuffer[3] = 0xff;			//R3		LSB step number
	txbuffer[4] = 0x01;			//R4		speed
	
	status = alt_avalon_i2c_master_tx(i2c_dev, txbuffer, 5, ALT_AVALON_I2C_NO_INTERRUPTS);
	
	
	txbuffer[0] = 17;			//R0	
	txbuffer[1] = 0x00;			//R17	

	
	status = alt_avalon_i2c_master_tx(i2c_dev, txbuffer, 2, ALT_AVALON_I2C_NO_INTERRUPTS);
	
	
	
	if (status == ALT_AVALON_I2C_SUCCESS) 	
	{
		MTXLOCK_STDIO();	
		printf("Test STEPPER complete \n");		
		MTXUNLOCK_STDIO(); 
	}
	else {
		MTXLOCK_STDIO();	
		printf("ERROR testing STEPPER \n");		
		MTXUNLOCK_STDIO(); 
	}
	
	return status;
}

//STEPPER - CHANGE
//Reads one of the stepper registers
ALT_AVALON_I2C_STATUS_CODE test_read_stepper(ALT_AVALON_I2C_DEV_t *i2c_dev)
{
	//REGISTER0 ADDRESS = 000xxxxx (0 to 17)
	//REGISTER1 CONTROL ST1 = 000 1 1 0 0 1 = 16 + 8 + 1 = 25
	//REGISTER2 STEP NUMBER MSB ST1 = 00000000 = 0b0
	//REGISTER3 STEP NUMBER LSB ST1 = 00001000 = 8
	//REGISTER4 SPEED 0.25ms	= 00000001
	
	ALT_AVALON_I2C_STATUS_CODE status = ALT_AVALON_I2C_SUCCESS;	
	
	unsigned char txbuffer[0x08];
	unsigned char rxbuffer[0x08];
	
	//reads the firmware version
	txbuffer[0] = 18;			//R0		register choice
	
	status = alt_avalon_i2c_master_tx(i2c_dev, txbuffer, 1, ALT_AVALON_I2C_NO_INTERRUPTS);
	status = alt_avalon_i2c_master_rx(i2c_dev, rxbuffer, 1, ALT_AVALON_I2C_NO_INTERRUPTS);
	
	
	if (status == ALT_AVALON_I2C_SUCCESS) 	
	{
		MTXLOCK_STDIO();	
		printf("Test STEPPER complete : %d \n", rxbuffer[0]);		
		MTXUNLOCK_STDIO(); 
	}
	else {
		MTXLOCK_STDIO();	
		printf("ERROR testing STEPPER \n");		
		MTXUNLOCK_STDIO(); 
	}
	return status;
}

/*//!
Only 1 I2C instance is used in this code. 
All sensors will use this same device.
Hence, the address, speed, and mode will have to be changed everytime we use a different sensor.

COLOR SENSOR
- address : 0x29 (7 bits)
- speed : 350000 Hz (400kHz max)
- mode : FAST		CAN IT ALSO BE STANDARD ??? 

SONAR
- address : 0x70 (7 bits)
- speed : 40000 Hz 
- mode : STANDARD

MICROSWITCH
- address : 0x20 (7 bits)
- speed : 80000 Hz (100kHz max)
- mode : STANDARD

STEPPER
- address : /
- speed : /
- mode : /

*/

//void Task_I2C(void) {
void init_I2C(ALT_AVALON_I2C_DEV_t *i2c_dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg) {
	
	// ---------------------------------------------------------------------------
	// Instantiation of I2C instance
	// ---------------------------------------------------------------------------
	
	instantiate_device(i2c_dev);
	alt_avalon_i2c_init(i2c_dev, cfg, ALT_AVALON_I2C_ADDR_MODE_7_BIT, 40000);
	MTXLOCK_STDIO();	
	printf("\nInstantiation of I2C instance done \n");
	MTXUNLOCK_STDIO();
	
	// ---------------------------------------------------------------------------	
	// TO DO EVERY TIME POWERED ON
	// ---------------------------------------------------------------------------
	
	ALT_AVALON_I2C_STATUS_CODE status;
	//int range = 500; 	//max distance in millimeters
	//int gain = 140;  	//[40,700] (default = 700)
	
	//	Changing range and gain of both the LEFT and RIGHT sonar
	status = changing_range(i2c_dev, cfg, SONAR_RANGE, LEFT_SONAR);
	status = changing_gain(i2c_dev, cfg, SONAR_GAIN, LEFT_SONAR);
	status = changing_range(i2c_dev, cfg, SONAR_RANGE, RIGHT_SONAR);
	status = changing_gain(i2c_dev, cfg, SONAR_GAIN, RIGHT_SONAR);
			
	//	Enabling the color sensor
	status = enabling_ColorSensor(i2c_dev, cfg);
	
	/*int value1;
	int value2;
	int i;
	//for(i=0; i<10000; i++) {
	for(;;) {
		value1 = range_sonar(i2c_dev, cfg, RIGHT_SONAR);
		value2 = range_sonar(i2c_dev, cfg, LEFT_SONAR);
		
		wait(cvs, 700);
	}*/
	
	/*int sw;
	for(;;) {
		sw = read_state_microswitch(i2c_dev, cfg, BACK_RIGHT_MICROSWITCH)
		TSKsleep(OS_MS_TO_TICK(500));
	}*/
	
	/*//COLOR SENSOR
	BOARD_COLOR_t color;
	for(;;) {
		color = read_color(i2c_dev, cfg);
		wait(cvs, 1000);
	}*/
}

void Object_Detection_Sonar(ALT_AVALON_I2C_DEV_t *i2c_dev, ALT_AVALON_I2C_MASTER_CONFIG_t cfg, int *range, CtrlStruct *cvs) {

	int range_left;
	int range_right;
	
	range_right = range_sonar(i2c_dev, cfg, RIGHT_SONAR, cvs);
	range_left = range_sonar(i2c_dev, cfg, LEFT_SONAR, cvs);
	
	//printf("range left : %d, range_right : %d\n",range_left,range_right);

	range[0] = range_left;
	range[1] = range_right;
}

//54 functions 
