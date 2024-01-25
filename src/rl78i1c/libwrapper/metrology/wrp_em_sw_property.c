/******************************************************************************
* DISCLAIMER

* This software is supplied by Renesas Electronics Corporation and is only 
* intended for use with Renesas products. No other uses are authorized.

* This software is owned by Renesas Electronics Corporation and is protected under 
* all applicable laws, including copyright laws.

* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES 
* REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, 
* INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
* PARTICULAR PURPOSE AND NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY 
* DISCLAIMED.

* TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS 
* ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE 
* FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES 
* FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR ITS 
* AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.

* Renesas reserves the right, without notice, to make changes to this 
* software and to discontinue the availability of this software.  
* By using this software, you agree to the additional terms and 
* conditions found by accessing the following link:
* http://www.renesas.com/disclaimer
******************************************************************************/
/* Copyright (C) 2011 Renesas Electronics Corporation. All rights reserved.  */
/****************************************************************************** 
* File Name    : wrp_em_hw_property.c
* Version      : 1.00
* Device(s)    : RL78/I1C R5F100LE
* Tool-Chain   : CubeSuite+ Version 1.00
* H/W Platform : RL78/I1C Energy Meter Platform
* Description  : Wrapper setting SW Property APIs
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "wrp_em_sw_config.h"
#include "wrp_em_sw_property.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Imported global variables and functions (from other files)
******************************************************************************/

/******************************************************************************
Exported global variables and functions (to be accessed by other files)
******************************************************************************/
static FAR_PTR const EM_SW_PROPERTY em_sw_property = 
{
    /* ADC */
    {
        EM_SW_PROPERTY_ADC_GAIN_PHASE_NUMBER_LEVEL,
        EM_SW_PROPERTY_ADC_GAIN_PHASE_UPPER_THRESHOLD,
        EM_SW_PROPERTY_ADC_GAIN_PHASE_LOWER_THRESHOLD,
        
        EM_SW_PROPERTY_ADC_GAIN_NEUTRAL_NUMBER_LEVEL,
        EM_SW_PROPERTY_ADC_GAIN_NEUTRAL_UPPER_THRESHOLD,
        EM_SW_PROPERTY_ADC_GAIN_NEUTRAL_LOWER_THRESHOLD,
    },
    
    /* Operation */
    {
        EM_SW_PROPERTY_OPERATION_IRMS_NOLOAD_THRESHOLD,
        EM_SW_PROPERTY_OPERATION_POWER_NOLOAD_THRESHOLD,
        EM_SW_PROPERTY_OPERATION_NO_VOLTAGE_THRESHOLD,
        EM_SW_PROPERTY_OPERATION_FREQUENCY_LOW_THRESHOLD,
        EM_SW_PROPERTY_OPERATION_FREQUENCY_HIGH_THRESHOLD,
        EM_SW_PROPERTY_OPERATION_EARTH_DIFF_THRESHOLD,
        EM_SW_PROPERTY_OPERATION_METER_CONSTANT,
        EM_SW_PROPERTY_OPERATION_PULSE_ON_TIME,
        EM_SW_PROPERTY_OPERATION_ENERY_PULSE_RATIO,
        EM_SW_PROPERTY_OPERATION_PULSE_EXPORT_DIRECTION,
	    EM_SW_PROPERTY_OPERATION_PULSE_REACTIVE_ENABLE,
	    EM_SW_PROPERTY_OPERATION_PULSE_APPARENT_ENABLE,
    },

    /* Rounding */
    {
        EM_SW_PROPERTY_ROUND_DIGIT_POWER,
        EM_SW_PROPERTY_ROUND_DIGIT_RMS,
        EM_SW_PROPERTY_ROUND_DIGIT_FREQ,
        EM_SW_PROPERTY_ROUND_DIGIT_PF,
    },
    
    /* Phase shift 90 Interpolation error */
    {
        EM_SW_PROPERTY_SAMP_SHIFTING90_INTERPOLATION_ERROR,
    },

    /* Sag and Swell */
    {
        
        EM_SW_PROPERTY_SAG_RMS_RISE_THRESHOLD,
        EM_SW_PROPERTY_SAG_RMS_FALL_THRESHOLD,
        EM_SW_PROPERTY_SWELL_RMS_RISE_THRESHOLD,
        EM_SW_PROPERTY_SWELL_RMS_FALL_THRESHOLD,
        EM_SW_PROPERTY_SAG_DETECTION_HALF_CYCLE,
        EM_SW_PROPERTY_SWELL_DETECTION_HALF_CYCLE,
    },
};

/******************************************************************************
Private global variables and functions
******************************************************************************/

/******************************************************************************
* Function Name : EM_SW_GetProperty
* Interface     : NEAR_PTR EM_SW_PROPERTY * EM_SW_GetProperty(void)
* Description   : Get SW Property Setting From Wrapper Layer
* Arguments     : None
* Function Calls: None
* Return Value  : const EM_SW_PROPERTY*, point to the SW Property Structure
******************************************************************************/
EM_SW_PROPERTY FAR_PTR * EM_SW_GetProperty(void)
{
    return (EM_SW_PROPERTY FAR_PTR *)&em_sw_property;
}
