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

extern char sample_ready, ISR_tracker;

extern int16 LDR_sample, TMP_sample;
extern int32 value_digit;

CY_ISR(Custom_Timer_ISR)
{
    Timer_ReadStatusRegister();
    ISR_tracker++;
    
    //read one ADC sample
    value_digit = ADC_Read32();
    if(value_digit<0)       value_digit=0;
    if(value_digit>65535)   value_digit=65535;
    
    //flag that sample is ready to be processed in main
    sample_ready = 1;  

}


/* [] END OF FILE */
