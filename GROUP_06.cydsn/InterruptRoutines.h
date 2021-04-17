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
#ifndef __INTERRUPT_ROUTINES_H
    #define __INTERRUPT_ROUTINES_H
    
    #include "cytypes.h"
    #include "project.h"
    #include "ProjectUtils.h"
    
    CY_ISR_PROTO(Custom_Timer_ISR);
    
    void sampleSingleChannel(char channel,int16* LDR_sample,int16* TMP_sample, char avg_count);

#endif

/* [] END OF FILE */
