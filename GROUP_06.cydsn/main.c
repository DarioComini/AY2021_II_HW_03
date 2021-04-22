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
int32 value_digit = 0;


int16 LDR_sample = 0, TMP_sample = 0;


int main(void)
{
    uint8_t slaveBuffer[BUFFER_SIZE] = {0,0,WHO_AM_I_REG_VALUE,0,0,0,0};
    char channel = CHANNEL_TMP, active_channels = 0;
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    //enable timer interrupt
    ISR_Timer_StartEx(Custom_Timer_ISR);
    
    //initialize I2C slave component
    EZI2C_Start();
    EZI2C_SetBuffer1(BUFFER_SIZE, RW_SIZE, slaveBuffer);
    
    slaveBuffer[CTRL_REGISTER_1_BYTE]=0b10;
    
    for(;;)
    {
        //if status changed
        if(checkStatus(slaveBuffer))
        {   
            switch(STATE)
            {
                case DEVICE_STOPPED: //completed
                    stopComponents();
                    resetBuffer(slaveBuffer,BUFFER_SIZE);
                    break;
                case TMP_SAMPLING:
                    active_channels = 1;
                    init_state(slaveBuffer,SINGLE_CHANNEL_PERIOD, channel = CHANNEL_TMP);
                    break;
                case LDR_SAMPLING:
                    active_channels = 1;
                    init_state(slaveBuffer,SINGLE_CHANNEL_PERIOD,channel = CHANNEL_LDR);
                    break;
                case BOTH_SAMPLING:
                    init_state(slaveBuffer,DOUBLE_CHANNEL_PERIOD,channel = CHANNEL_TMP);
                    BLUE_LED_Write(BLUE_LED_ON);
                    active_channels = 2;
                    break;
                
            }
        }
        
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
            if (incrementAverageCounter(slaveBuffer) == SAMPLES_FOR_AVG*active_channels)
            {
                //perform average
                TMP_sample = TMP_sample/SAMPLES_FOR_AVG;
                LDR_sample = LDR_sample/SAMPLES_FOR_AVG;
                
                //convert in temperature
                TMP_sample = (TMP_sample - 500.0)/10.0;
                
                //convert in lux
                double LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / LDR_sample - 1.0);
                LDR_sample = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
                
                //save into I2C
                slaveBuffer[TMP_Bit_15_8] = TMP_sample>>8;
                slaveBuffer[TMP_Bit_07_0] = TMP_sample & 0xFF;
                slaveBuffer[LDR_Bit_15_8] = LDR_sample>>8;
                slaveBuffer[LDR_Bit_07_0] = LDR_sample & 0xFF;
                
                //reset average, counter and sensor variables
                TMP_sample = 0;
                LDR_sample = 0;
                slaveBuffer[CTRL_REGISTER_1_BYTE] &= 0b11;
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
