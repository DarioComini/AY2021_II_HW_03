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
#include "InterruptRoutines.h" 
#include "math.h"

extern char slaveBuffer[BUFFER_SIZE];
extern char channel, active_channels;
extern int16 LDR_sample, TMP_sample;
int32 value_digit;

CY_ISR(Custom_Timer_ISR)
{
    Timer_ReadStatusRegister();
    
    //increment counter 
    char avg_count = incrementAverageCounter(slaveBuffer);
    
    //select channel
    if (STATE == BOTH_SAMPLING)
    {
        channel = !channel;
        MUX_Select(channel);
    }
    
    //read one ADC sample
    sampleSingleChannel(channel,&LDR_sample,&TMP_sample, avg_count);
    
}


void sampleSingleChannel(char channel,int16 *LDR_sample,int16 *TMP_sample, char avg_count)
{
    value_digit = ADC_Read32();
    if(value_digit<0)       value_digit=0;
    if(value_digit>65535)   value_digit=65535;
    
    switch (channel)
    {
        case CHANNEL_TMP:
            *TMP_sample+=ADC_CountsTo_mVolts(value_digit);
            break;
        case CHANNEL_LDR:
            *LDR_sample+=ADC_CountsTo_mVolts(value_digit);
            break;
    }
    
    if (avg_count == SAMPLES_FOR_AVG*active_channels)
    {
        //perform average
        *TMP_sample = *TMP_sample/SAMPLES_FOR_AVG;
        *LDR_sample = *LDR_sample/SAMPLES_FOR_AVG;
        
        //convert in temperature
        float temp = ((float)*TMP_sample - 500.0)/10.0;
        *TMP_sample = (int16)temp;
        
        //convert in lux
        double LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / *TMP_sample - 1.0);
        *TMP_sample = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
        
        //save into I2C
        slaveBuffer[TMP_Bit_15_8] = *TMP_sample>>8;
        slaveBuffer[TMP_Bit_07_0] = *TMP_sample & 0xFF;
        slaveBuffer[LDR_Bit_15_8] = *LDR_sample>>8;
        slaveBuffer[LDR_Bit_07_0] = *LDR_sample & 0xFF;
        
        //reset average, counter and sensor variables
        *TMP_sample = 0;
        *LDR_sample = 0;
        slaveBuffer[CTRL_REGISTER_1_BYTE] &= 0b11;
    }
    
    
}

/* [] END OF FILE */
