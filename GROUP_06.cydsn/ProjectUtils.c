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

char checkChanges(uint8_t *buffer)
{
    char onStatusChanged = 0;
    char new_state = (buffer[CTRL_REGISTER_1_BYTE] & MASK_STATUS);
    char new_timer = buffer[CTRL_REGISTER_2_BYTE]; 
    if(new_timer < 2)
    {
        new_timer = 2;
        buffer[CTRL_REGISTER_2_BYTE] = 2;
    }
    
    
    char new_average =  (buffer[CTRL_REGISTER_1_BYTE] & MASK_AVERAGE)>>INDEX_AVERAGE;
    if(new_average == 0)
    {
        new_average = 1;
        buffer[CTRL_REGISTER_1_BYTE] |= 1<<INDEX_AVERAGE;
    }
    
    if(new_state != STATE || new_timer != timer_period || new_average != samplesForAverage)
    {
        onStatusChanged = 1;
        resetVariables();
        //assign changes
        STATE = new_state;
        samplesForAverage = new_average;
        timer_period = new_timer;
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

void init_state(uint8_t *buffer, char channel, char n_channel)
{
    BLUE_LED_Write(BLUE_LED_OFF);
    Timer_WritePeriod(buffer[CTRL_REGISTER_2_BYTE]);
    Timer_CLK_SetDivider(divider/(n_channel)-1);
    
    MUX_Select(channel);
    startComponents();
}

/* [] END OF FILE */
