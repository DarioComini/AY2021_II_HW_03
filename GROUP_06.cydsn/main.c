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
#include "ProjectUtils.h"
#include "InterruptRoutines.h"
#include <stdio.h>
#include "math.h"

char sample_ready = 0;
char ISR_tracker = 0;
int32 value_digit = 0;

uint8_t slaveBuffer[BUFFER_SIZE] = {0,0,WHO_AM_I_REG_VALUE,0,0,0,0};
char channel = CHANNEL_TMP, active_channels = 0;

uint8 transmission_ready = 0;

int16 LDR_sample = 0, TMP_sample = 0;


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    //enable timer interrupt
    ISR_Timer_StartEx(Custom_Timer_ISR);
    divider = Timer_CLK_GetDividerRegister()+1;
    samplesForAverage = 0;
    timer_period = slaveBuffer[CTRL_REGISTER_2_BYTE];
    
    //initialize I2C slave component
    EZI2C_Start();
    EZI2C_SetBuffer1(BUFFER_SIZE, RW_SIZE, slaveBuffer);
    
    slaveBuffer[CTRL_REGISTER_1_BYTE]=0b10;
    
    for(;;)
    {
        //if data is ready
        if (sample_ready)
        {            
            //add sampled data to sensor variables
            switch (channel)
            {
                case CHANNEL_TMP:
                    TMP_sample+=ADC_CountsTo_mVolts(value_digit);
                    break;
                case CHANNEL_LDR:
                    LDR_sample+=ADC_CountsTo_mVolts(value_digit);
                    break;
            }
            
            if (ISR_tracker == samplesForAverage*active_channels)
            {
                //perform average
                TMP_sample = TMP_sample/samplesForAverage;
                LDR_sample = LDR_sample/samplesForAverage;
                
                //convert in temperature
                TMP_sample = ((TMP_sample - 500.0)/10.0)*100;   //we multiply by 100 to have more resolution
                
                //convert in lux
                double LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / LDR_sample - 1.0);
                LDR_sample = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
                
                //save into I2C
                slaveBuffer[TMP_Bit_15_8] = TMP_sample>>8;
                slaveBuffer[TMP_Bit_07_0] = TMP_sample & 0xFF;
                slaveBuffer[LDR_Bit_15_8] = LDR_sample>>8;
                slaveBuffer[LDR_Bit_07_0] = LDR_sample & 0xFF;
                
                //reset counter and sensor variables
                TMP_sample = 0;
                LDR_sample = 0;
                ISR_tracker = 0;
                
                if (!transmission_ready)
                {
                    transmission_ready = 1;
                    EZI2C_EnableInt();
                }
                
            }
            if (STATE == BOTH_SAMPLING)
            {
                channel = !channel;
                MUX_Select(channel);
            }
            sample_ready = 0;
        }
    }
}

/* [] END OF FILE */
