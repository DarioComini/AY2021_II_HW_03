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

extern char slaveBuffer[BUFFER_SIZE];
extern char channel, active_channels;

int16 LDR_sample = 0, TMP_sample = 0;

CY_ISR(Custom_Timer_ISR)
{
    Timer_ReadStatusRegister();
    
    //increment counter 
    char avg_count = incrementAverageCounter(slaveBuffer);
    
    //select channel
    if (STATE == BOTH_SAMPLING)
    {
        channel = !channel;
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
    
    //convert in mV and sum in a variable
    
    //if average register is = n°canali * 5 salva_average()
    if (avg_count == SAMPLES_FOR_AVG*active_channels)
    {
        //fai media
        *TMP_sample = *TMP_sample/SAMPLES_FOR_AVG;
        *LDR_sample = *LDR_sample/SAMPLES_FOR_AVG;
        
        //salva nell'I2C
        slaveBuffer[TMP_Bit_15_8] = *TMP_sample>>8;
        slaveBuffer[TMP_Bit_07_0] = *TMP_sample & 0x0F;
        slaveBuffer[LDR_Bit_15_8] = *LDR_sample>>8;
        slaveBuffer[LDR_Bit_07_0] = *LDR_sample & 0x0F;
        
        //reset average, counter e variabili dei sensori
        *TMP_sample = 0;
        *LDR_sample = 0;
        slaveBuffer[CTRL_REGISTER_1_BYTE] &= 0b11;
    }
    
    
}

/* [] END OF FILE */
