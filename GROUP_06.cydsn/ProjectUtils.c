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
}

void startComponents()
{
    Timer_Start();
    ADC_Start();
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
    STATE = (buffer[CTRL_REGISTER_1_BYTE] & MASK_BIT_0) + (buffer[CTRL_REGISTER_1_BYTE] & MASK_BIT_1);
    if(old_STATE != STATE)
    {
        onStatusChanged = 1;
        old_STATE = STATE;
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

char incrementAverageCounter(char *buffer)
{
    char count = buffer[CTRL_REGISTER_1_BYTE] >> 2;
	count++;
	buffer[CTRL_REGISTER_1_BYTE] = STATE | (count<<2);
    
    return count;
}


/* [] END OF FILE */
