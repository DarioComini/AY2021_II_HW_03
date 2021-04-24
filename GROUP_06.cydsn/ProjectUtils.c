/* ========================================
 *
 * Authors: Michele Pezzi, Dario Comini
 * Date: 27/04/2021
 * 
 * Source file for some general functions
 *
 * ========================================
*/
#include "ProjectUtils.h" 

/*******************************************************************************
* Function Name: resetVariables
********************************************************************************
*
* Summary: Resets sensor variables and interrupt tracker (sampling counter)
*  
* Parameters: void
*  
* Return: void 
*
*******************************************************************************/
static void resetVariables()
{
    TMP_sample = 0;
    LDR_sample = 0;
    ISR_tracker = 0;
}

/*******************************************************************************
* Function Name: startComponents
********************************************************************************
*
* Summary: Starts project peripherals (Timer, ADC)
           sets ADC actual gain for mV conversion
*  
* Parameters: void
*  
* Return: void 
*
*******************************************************************************/
void startComponents()
{
    Timer_Start();
    ADC_Start();
    ADC_SetGain(14094);//((65535/ACTUAL_Vdd_mV)*1000 + 1);
}

/*******************************************************************************
* Function Name: stopComponents
********************************************************************************
*
* Summary: Stops project peripherals (Timer, ADC)
           Switches of builtin LED
*  
* Parameters: void
*  
* Return: void 
*
*******************************************************************************/
void stopComponents()
{
    Timer_Stop();
    ADC_Stop();
    BLUE_LED_Write(BLUE_LED_OFF);
}

/*******************************************************************************
* Function Name: checkChanges
********************************************************************************
*
* Summary: Checks if any changes were made in control registers 1 or 2.
           It also checks if the changes are respecting the allowed values
*  
* Parameters: EZI2C slave buffer
*  
* Return: 1 if the STATE was changed, 0 otherwise.
*
*******************************************************************************/
char checkChanges(uint8_t *buffer)
{  
    char onStatusChanged = 0;
    
    /*
        Read the parameters written in the slave buffer.
    */
    
    char new_timer = buffer[CTRL_REGISTER_2_BYTE];
    char new_average =  (buffer[CTRL_REGISTER_1_BYTE] & MASK_AVERAGE)>>INDEX_AVERAGE;
    char new_state = (buffer[CTRL_REGISTER_1_BYTE] & MASK_STATUS);
    
    // if any parameter has changed, reset service variables
    if (new_timer != timer_period || new_average != samplesForAverage || new_state != STATE)
    {
        resetVariables();
    }
    
    /*
        Check if timer_period is greater than 2. If it's different from the previous timer period, update it
    */
    if (new_timer != timer_period)
    {
        if(new_timer < 2)
        {
            new_timer = 2;
            buffer[CTRL_REGISTER_2_BYTE] = 2;
        }
        timer_period = new_timer;
        
        //Update timer component to new timer value
        Timer_WritePeriod(timer_period);
    }
    
    /*
        Check if new_average is not 0. If it's different from the previous average bits, update it
    */
    if (new_average != samplesForAverage)
    {
        if(new_average == 0)
        {
            new_average = 1;
            buffer[CTRL_REGISTER_1_BYTE] |= 1<<INDEX_AVERAGE;
        }
        samplesForAverage = new_average;
    }
    
    /*
        If new_state is different from the previous state, update it
    */
    if(new_state != STATE)
    {
        STATE = new_state;
        onStatusChanged = 1;
    }

    return onStatusChanged;
}

/*******************************************************************************
* Function Name: resetBuffer
********************************************************************************
*
* Summary: Resets EZI2C Buffer to default values
*          (all 0 except for WHOAMI to its default)
*  
* Parameters: EZI2C slave buffer
*  
* Return: void
*
*******************************************************************************/
void resetBuffer(uint8_t *buffer)
{
    for(int i = 0; i<BUFFER_SIZE; i++)
    {
        buffer[i] = 0;
    }
    buffer[WHO_AM_I] = WHO_AM_I_REG_VALUE;
}

/*******************************************************************************
* Function Name: init_state
********************************************************************************
*
* Summary: Initializes the new state by configuring the peripherals accordingly
*  
* Parameters: EZI2C slave buffer, first channel activated, number of channels active
*  
* Return: void 
*
*******************************************************************************/
void init_state(char channel, char n_channel)
{
    //Turn off the builtin LED
    BLUE_LED_Write(BLUE_LED_OFF);
    
    //If two channels need to be sampled, double the interrupt clock frequency
    Timer_CLK_SetDivider(divider/(n_channel)-1);
    
    //Select the first channel to sample
    MUX_Select(channel);
    
    //Start peripherals
    startComponents();
}

/* [] END OF FILE */
