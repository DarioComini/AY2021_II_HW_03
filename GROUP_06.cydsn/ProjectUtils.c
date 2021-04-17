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
}

char checkStatus(uint8_t *buffer)
{
    char onStatusChanged = 0;
    STATE = (buffer[CTRL_REGISTER_1_BYTE] & MASK_BIT_0) + (buffer[CTRL_REGISTER_1_BYTE] & MASK_BIT_1);
    if(old_STATE != STATE)
    {
        onStatusChanged = 1;
    }
    return onStatusChanged;
}


/* [] END OF FILE */
