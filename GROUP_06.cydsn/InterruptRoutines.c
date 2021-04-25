/* ========================================
 *
 * Authors: Michele Pezzi, Dario Comini
 * Date: 27/04/2021
 * 
 * Source file for interrupt routines
 *
 * ========================================
*/
#include "InterruptRoutines.h" 



/*******************************************************************************
* Function Name: Custom_Timer_ISR
********************************************************************************
*
* Summary: Samples Delta-sigma ADC and tells main that sample is ready to be
           processed
*  
* Parameters: void
*  
* Return: void 
*
*******************************************************************************/
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


/*******************************************************************************
* Function Name: EZI2C_ISR_ExitCallback
********************************************************************************
*
* Summary: Checks if a change occurred in slave buffer control registers. If so,
*          Initialize the peripherals and variables according to the state.
*  
* Parameters: void
*  
* Return: void 
*
*******************************************************************************/
void EZI2C_ISR_ExitCallback()
{
    
    //if a change occurred in a control register (in particular if state changed)
    if (checkChanges(slaveBuffer))
    {
        switch(STATE)
        {
            // state 0 (device stopped)
            case DEVICE_STOPPED:
                #if BCP_TRANSMISSION_DEBUG
                    EZI2C_EnableInt();
                #endif 
                stopComponents();
                resetBuffer(slaveBuffer);
                break;
                
            //initialization to sample temperature sensor
            case TMP_SAMPLING:
                init_state(channel = CHANNEL_TMP,active_channels = 1);
                break;
            
            //initialization to sample light sensor
            case LDR_SAMPLING:
                init_state(channel = CHANNEL_LDR,active_channels = 1);
                break;
                
            //initialization to sample both sensors
            case BOTH_SAMPLING:
                init_state(channel = CHANNEL_TMP,active_channels = 2);
                BLUE_LED_Write(BLUE_LED_ON);
                break;
        }
    }
    
    /*
        force BCP communication to the desired frequency 
        by disabling the EZI2C peripheral if master requests to read
        data but the averaged sample is still not ready to be transmitted
    */
    #if BCP_TRANSMISSION_DEBUG
    char EZ_status = EZI2C_GetActivity();

    if (STATE>DEVICE_STOPPED)
    {
        if (EZ_status & EZI2C_STATUS_WRITE1) //write1 was sent
        {
            if (!transmission_ready)
            {
                EZI2C_DisableInt();
            }
        }
        else
        {
            transmission_ready = 0;
        }
    }
    #endif
    
}


/* [] END OF FILE */
