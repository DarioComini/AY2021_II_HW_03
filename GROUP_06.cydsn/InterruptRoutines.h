/* ========================================
 *
 * Authors: Michele Pezzi, Dario Comini
 * Date: 27/04/2021
 * 
 * Header file for interrupt routines
 *
 * ========================================
*/
#ifndef __INTERRUPT_ROUTINES_H
    #define __INTERRUPT_ROUTINES_H
    
    #include "cytypes.h"
    #include "project.h"
    #include "ProjectUtils.h"
    
    /*
        Set this to 1 to force BCP communication to the desired frequency
    */
    #define BCP_TRANSMISSION_DEBUG 0
    
    #if BCP_TRANSMISSION_DEBUG
        extern uint8 transmission_ready;    
    #endif
    
    extern int32 value_digit;
    extern char sample_ready;
    
    CY_ISR_PROTO(Custom_Timer_ISR);
    
#endif

/* [] END OF FILE */
