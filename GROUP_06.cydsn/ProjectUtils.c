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
#include "ProjectUtils.h" 

static void resetVariables()
{
    TMP_sample = 0;
    LDR_sample = 0;
    ISR_tracker = 0;
}

void startComponents()
{
    Timer_Start();
    ADC_Start();
    ADC_SetGain(14094);//((65535/ACTUAL_Vdd_mV)*1000 + 1);
    EZI2C_Start();
}

void stopComponents()
{
    Timer_Stop();
    ADC_Stop();
    BLUE_LED_Write(BLUE_LED_OFF);
}

char checkStatus(uint8_t *buffer)
{
    char onStatusChanged = 0;
    char new_state = (buffer[CTRL_REGISTER_1_BYTE] & MASK_BIT_0) + (buffer[CTRL_REGISTER_1_BYTE] & MASK_BIT_1);
    if(new_state != STATE)
    {
        onStatusChanged = 1;
        STATE = new_state;
        resetVariables();
    }
    return onStatusChanged;
}

void resetBuffer(uint8_t *buffer, int length)
{
    for(int i = 0; i<length; i++)
    {
        buffer[i] = 0;
    }
    buffer[WHO_AM_I] = WHO_AM_I_REG_VALUE;
}

void init_state(uint8_t *buffer, char channel)
{
    BLUE_LED_Write(BLUE_LED_OFF);
    if(buffer[CTRL_REGISTER_2_BYTE] > 0)                Timer_WritePeriod(buffer[CTRL_REGISTER_2_BYTE]); 
    else                                                Timer_WritePeriod(DEFAULT_TIMER_PERIOD);
    
    if((buffer[CTRL_REGISTER_1_BYTE]>>2 & 0xFF) > 0)    samplesForAverage = (buffer[CTRL_REGISTER_1_BYTE]>>2 & 0xFF);
    else                                                samplesForAverage = DEFAULT_SAMPLES_FOR_AVG;

    MUX_Select(channel);
    
    startComponents();
}

/* [] END OF FILE */
