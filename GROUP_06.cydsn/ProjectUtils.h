/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"

#define BUFFER_SIZE 7
#define RW_SIZE		2

#define MASK_BIT_0  		(1<<CTRL_REG_Status_Bit_0)
#define MASK_BIT_1  		(1<<CTRL_REG_Status_Bit_1)

#define DEVICE_STOPPED  	0
#define TMP_SAMPLING        1
#define LDR_SAMPLING        2
#define BOTH_SAMPLING       3

#define CTRL_REGISTER_1_BYTE 0 
#define CTRL_REGISTER_2_BYTE 1
#define WHO_AM_I 			 2
#define TMP_Bit_15_8		 3
#define TMP_Bit_07_0		 4
#define LDR_Bit_15_8		 5
#define LDR_Bit_07_0		 6

#define CTRL_REG_Status_Bit_0 			0
#define CTRL_REG_Status_Bit_1 			1

#define WHO_AM_I_REG_VALUE      0xBC

#define BLUE_LED_OFF 0
#define BLUE_LED_ON 1

#define DEFAULT_TIMER_PERIOD        200

#define CHANNEL_TMP 0
#define CHANNEL_LDR 1

#define DEFAULT_SAMPLES_FOR_AVG     5

#define SERIES_RESISTANCE           990
#define ACTUAL_Vdd_mV               4650.0
#define TEN_TO_LDR_INTERCEPT        100000 //q = 5 -> pow(10,q) = 100000
#define LDR_SLOPE                   -0.682

char STATE, samplesForAverage;
extern char ISR_tracker;
extern int16 LDR_sample, TMP_sample;

//MACRO FOR FUNCTIONS
void startComponents(void);
void stopComponents(void);
char checkStatus(uint8_t *buffer);
void resetBuffer(uint8_t *buffer, int length);
void init_state(uint8_t *buffer, char channel);

/* [] END OF FILE */
