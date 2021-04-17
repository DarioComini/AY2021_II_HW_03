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
#include <stdio.h>

uint8_t slaveBuffer[BUFFER_SIZE] = {0,0,0xBC,0,0,0,0};

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    //ISR_Timer_StartEx(Custom_Timer_ISR);
    startComponents();
    
    EZI2C_SetBuffer1(BUFFER_SIZE, RW_SIZE, slaveBuffer);

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
