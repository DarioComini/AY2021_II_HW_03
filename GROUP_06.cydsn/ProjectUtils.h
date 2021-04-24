/* ========================================
 *
 * Authors: Michele Pezzi, Dario Comini
 * Date: 27/04/2021
 * 
 * Header file for general functions and macros
 *
 * ========================================
*/

#include "project.h"

/*
    I2C MACROS
*/
//Slave address space dimensions
#define BUFFER_SIZE 7
#define RW_SIZE		2

//Slave buffer indexing
#define CTRL_REGISTER_1_BYTE 0 
#define CTRL_REGISTER_2_BYTE 1
#define WHO_AM_I 			 2
#define TMP_Bit_15_8		 3
#define TMP_Bit_07_0		 4
#define LDR_Bit_15_8		 5
#define LDR_Bit_07_0		 6

//Slave default value for whoami
#define WHO_AM_I_REG_VALUE      0xBC

//Binary masks for controlreg1 handling
#define MASK_STATUS  		0b11
#define MASK_AVERAGE 		0b111100
#define INDEX_AVERAGE       2

//Firmware states
#define DEVICE_STOPPED  	0
#define TMP_SAMPLING        1
#define LDR_SAMPLING        2
#define BOTH_SAMPLING       3

//Builtin LED activation/deactivation macros
#define BLUE_LED_OFF 0
#define BLUE_LED_ON 1

//Sensor channels
#define CHANNEL_TMP 0
#define CHANNEL_LDR 1

//Constants for sample conversions
#define TMP_SLOPE 10.0
#define TMP_INTERCEPT 500.0
#define SERIES_RESISTANCE           990
#define ACTUAL_Vdd_mV               4650.0
#define TEN_TO_LDR_INTERCEPT        100000 //q = 5 -> pow(10,q) = 100000
#define LDR_SLOPE                   -0.682

/*
    Global variables declarations
*/
extern char STATE, samplesForAverage, timer_period;
extern char ISR_tracker;
extern int16 LDR_sample, TMP_sample;
uint16 divider;
extern uint8_t slaveBuffer[BUFFER_SIZE];
extern char channel, active_channels;


//Functions declarations
void startComponents(void);
void stopComponents(void);
char checkChanges(uint8_t *buffer);
void resetBuffer(uint8_t *buffer);
void init_state(uint8_t *buffer, char channel, char n_channel);

/* [] END OF FILE */
