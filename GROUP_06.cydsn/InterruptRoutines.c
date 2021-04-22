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

extern uint8_t slaveBuffer[BUFFER_SIZE];
extern char channel, active_channels;

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

void EZI2C_ISR_ExitCallback()
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
                    init_state(slaveBuffer, channel = CHANNEL_TMP);
                    break;
                case LDR_SAMPLING:
                    active_channels = 1;
                    init_state(slaveBuffer, channel = CHANNEL_LDR);
                    break;
                case BOTH_SAMPLING:
                    init_state(slaveBuffer, channel = CHANNEL_TMP);
                    BLUE_LED_Write(BLUE_LED_ON);
                    active_channels = 2;
                    break;
                
            }
        }
}


/* [] END OF FILE */
