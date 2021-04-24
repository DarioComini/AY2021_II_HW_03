/* ========================================
 *
 * Authors: Michele Pezzi, Dario Comini
 * Date: 27/04/2021
 * 
 * main source file
 * 
 * This code handles the sample data and related conversion 
 * into the measure of interest of two sensors 
 * (TMP for temperature in °C, LDR for light in lux)
 * The user may set the number of samples to be averaged before
 * transmission and the frequency of the update using I2C communication
 * from Bridge Control Panel.
 * 
 * To start the conversion the user has to choose between three states:
 *      - 1: sample only temperature data
 *      - 2: sample only light data
 *      - 3: sample from both sensors in an alternating pattern
 * 
 * ========================================
*/
#include "project.h"
#include "ProjectUtils.h"
#include "InterruptRoutines.h"
#include <stdio.h>
#include "math.h"

#if BCP_TRANSMISSION
    uint8 transmission_ready = 0;
#endif

/*Global variables definitions*/
char sample_ready = 0;
char ISR_tracker = 0;
int32 value_digit = 0;
uint8_t slaveBuffer[BUFFER_SIZE] = {0,0,WHO_AM_I_REG_VALUE,0,0,0,0};
char channel = CHANNEL_TMP, active_channels = 0;
int16 LDR_sample = 0, TMP_sample = 0;

char STATE = DEVICE_STOPPED, samplesForAverage = 0, timer_period = 0;

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    //enable timer interrupt
    ISR_Timer_StartEx(Custom_Timer_ISR);
    
    //initialize value of timer_clk divider
    divider = Timer_CLK_GetDividerRegister()+1;
    
    //Initialize timer to top-design period value (happens only once)
    Timer_Start();
    Timer_Stop();
    
    
    
    //UART_Start();
    //CyDelay(100);
    //UART_PutString("\fBegin");
    
    
    
    //initialize I2C slave component
    EZI2C_Start();
    EZI2C_SetBuffer1(BUFFER_SIZE, RW_SIZE, slaveBuffer);
    
    for(;;)
    {
        //if data is ready
        if (sample_ready)
        {            
            //add sampled data to sensor variables
            switch (channel)
            {
                case CHANNEL_TMP:
                    TMP_sample+=ADC_CountsTo_mVolts(value_digit);
                    break;
                case CHANNEL_LDR:
                    LDR_sample+=ADC_CountsTo_mVolts(value_digit);
                    break;
            }
            
            //if the number of samples to be averaged is reached
            if (ISR_tracker == samplesForAverage*active_channels)
            {
                //perform average
                TMP_sample = TMP_sample/samplesForAverage;
                LDR_sample = LDR_sample/samplesForAverage;
                
                //convert in temperature
                TMP_sample = ((TMP_sample - TMP_INTERCEPT)/TMP_SLOPE)*100;   //we multiply by 100 to have more resolution
                
                //convert in lux
                double LDR = SERIES_RESISTANCE * (ACTUAL_Vdd_mV / LDR_sample - 1.0);
                LDR_sample = (int16) (pow(LDR/TEN_TO_LDR_INTERCEPT, 1/LDR_SLOPE));
                
                //save into I2C
                slaveBuffer[TMP_Bit_15_8] = TMP_sample>>8;
                slaveBuffer[TMP_Bit_07_0] = TMP_sample & 0xFF;
                slaveBuffer[LDR_Bit_15_8] = LDR_sample>>8;
                slaveBuffer[LDR_Bit_07_0] = LDR_sample & 0xFF;
                
                //reset counter and sensor variables
                TMP_sample = 0;
                LDR_sample = 0;
                ISR_tracker = 0;
                
                /*
                    Re-enable EZI2C interrupt if sample is ready
                */
                #if BCP_TRANSMISSION
                    if (!transmission_ready)
                    {
                        transmission_ready = 1;
                        EZI2C_EnableInt();
                    }
                #endif
                
            }
            
            //switch channels to sample alternately both sensors
            if (STATE == BOTH_SAMPLING)
            {
                channel = !channel;
                MUX_Select(channel);
            }
            
            //reset flag for ADC sampling
            sample_ready = 0;
        }
    }
}

/* [] END OF FILE */
