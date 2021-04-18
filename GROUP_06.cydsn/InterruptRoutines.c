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
    switch (channel)
    {
        case CHANNEL_TMP:
            *TMP_sample+=ADC_CountsTo_mVolts(ADC_Read32());
            break;
        case CHANNEL_LDR:
            *LDR_sample+=ADC_CountsTo_mVolts(ADC_Read32());
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
        //double exponent = (log10(5*990/(*LDR_sample)-990)-10.6617)/(-0.67609152744);
        //double result = pow(10,exponent);
        //*LDR_sample = (int16) result;
        
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
