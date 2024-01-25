/***********************************************************************************************************************
* DISCLAIMER`
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
* No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws. 
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIESREGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
* LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
* INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
* ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability 
* of this software. By using this software, you agree to the additional terms and conditions found by accessing the 
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2013, 2016 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/

/***********************************************************************************************************************
* File Name    : em_integrator_config.c
* Version      : 1.01.001
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL, CA78K, IAR
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Initial config for Integrator base on platform
************************************************************************************************************************
* History      : 27.04.2017 First creation
***********************************************************************************************************************/

/***********************************************************************************************************************
Includes   <System Includes> , "Project Includes"
***********************************************************************************************************************/
#include "em_integrator.h"

/***********************************************************************************************************************
Typedef definitions
***********************************************************************************************************************/

/***********************************************************************************************************************
Imported global variables and functions (from other files)
***********************************************************************************************************************/
/* 
    Enable/disable integration
    (0) :   Input -> -------> -> HPF -> Output
    (1) :   Input -> Integral -> HPF -> Output
*/
#if PH1_2W == 1
    /* 1P2W config */
    #define EM_ADC_INTEGRATOR_CHANNEL_PHASE_ENABLE      (1)
    #define EM_ADC_INTEGRATOR_CHANNEL_NEUTRAL_ENABLE    (0)
#elif PH1_3W == 1
    /* 1P3W config */
    #define EM_ADC_INTEGRATOR_CHANNEL_PHASE0_ENABLE     (1)
    #define EM_ADC_INTEGRATOR_CHANNEL_PHASE1_ENABLE     (1)
    #define EM_ADC_INTEGRATOR_CHANNEL_NEUTRAL_ENABLE    (0)
#elif PH3_4W == 1
    /* 3P4W config */
    #define INTEGRATOR_ENALBE_PHASE_R_I     (1)
    #define INTEGRATOR_ENALBE_PHASE_Y_I     (1)
    #define INTEGRATOR_ENALBE_PHASE_B_I     (1)
    #define INTEGRATOR_ENALBE_PHASE_N_I     (0)
#else
#error
#endif

/* Config base on platform */
#if PH1_2W == 1
    /* 1P2W config */
    #define INTEGRATOR_0_EN         ( EM_ADC_INTEGRATOR_CHANNEL_PHASE_ENABLE   )
    #define INTEGRATOR_1_EN         ( EM_ADC_INTEGRATOR_CHANNEL_NEUTRAL_ENABLE )
    #define INTEGRATOR_2_EN         ( 0 )
    #define INTEGRATOR_3_EN         ( 0 )
#elif PH1_3W == 1
    /* 1P3W config */
    #define INTEGRATOR_0_EN         ( EM_ADC_INTEGRATOR_CHANNEL_PHASE0_ENABLE   )
    #define INTEGRATOR_1_EN         ( EM_ADC_INTEGRATOR_CHANNEL_PHASE1_ENABLE )
    #define INTEGRATOR_2_EN         ( EM_ADC_INTEGRATOR_CHANNEL_NEUTRAL_ENABLE )
    #define INTEGRATOR_3_EN         ( 0 )
#elif PH3_4W == 1
    /* 3P4W config */
    #define INTEGRATOR_0_EN     (INTEGRATOR_ENALBE_PHASE_R_I)
    #define INTEGRATOR_1_EN     (INTEGRATOR_ENALBE_PHASE_Y_I)
    #define INTEGRATOR_2_EN     (INTEGRATOR_ENALBE_PHASE_B_I)
    #define INTEGRATOR_3_EN     (INTEGRATOR_ENALBE_PHASE_N_I)
#else
#error
#endif

/***********************************************************************************************************************
Exported global variables and functions (to be accessed by other files)
***********************************************************************************************************************/

/* Config for integrator 0 */
__near const R_EM_INTEGRATOR_CONFIG r_em_integrator_config[INTEGRATOR_MAX_CHANNEL] =
{
    /* Config for integrator 0 */
    {
        /* Mask input */
        0,
        /* HPF_INPUT */
        0, 
        /* Integrator */
        INTEGRATOR_0_EN,
        /* HFP_OUTPUT */
        1,
        /* Scale down output */
        0,
        /* Reserved */
        0,
    },

    /* Config for integrator 1 */
    {
        /* Mask input */
        0,
        /* HPF_INPUT */
        0, 
        /* Integrator */
        INTEGRATOR_1_EN,
        /* HFP_OUTPUT */
        1,
        /* Scale down output */
        0,
        /* Reserved */
        0,
    },
    
    /* Config for integrator 2 */
    {
        /* Mask input */
        0,
        /* HPF_INPUT */
        0, 
        /* Integrator */
        INTEGRATOR_2_EN,
        /* HFP_OUTPUT */
        1,
        /* Scale down output */
        0,
        /* Reserved */
        0,
    },
    /* Config for integrator 3 */
    {
        /* Mask input */
        0,
        /* HPF_INPUT */
        0, 
        /* Integrator */
        INTEGRATOR_3_EN,
        /* HFP_OUTPUT */
        1,
        /* Scale down output */
        0,
        /* Reserved */
        0,
    }
};
/***********************************************************************************************************************
Private global variables and functions
***********************************************************************************************************************/
