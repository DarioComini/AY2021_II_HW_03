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
#include "project.h"
#include "ProjectUtils.h"
#include "InterruptRoutines.h"
#include <stdio.h>

uint8_t slaveBuffer[BUFFER_SIZE] = {0,0,0xBC,0,0,0,0};

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    ISR_Timer_StartEx(Custom_Timer_ISR);
    startComponents();
    
    EZI2C_SetBuffer1(BUFFER_SIZE, RW_SIZE, slaveBuffer);
    old_STATE = DEVICE_STOPPED;

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        if(checkStatus(slaveBuffer))
        {
            switch(STATE)
            {
                case DEVICE_STOPPED:
                    stopComponents();
                    resetBuffer(slaveBuffer,BUFFER_SIZE);
                    break;
                case TMP_SAMPLING:
                    //timer in modo che overflow period 200
                    break;
                case LDR_SAMPLING:
                    //timer in modo che overflow period 200
                    break;
                case BOTH_SAMPLING:
                    //timer in modo che overflow period 100
                    break;
                
            }
        }
        
    }
}

/* [] END OF FILE */
