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
* File Name    : inst_read.c
* Version      : 1.00
* Device(s)    : RL78/I1C
* Tool-Chain   : CCRL
* H/W Platform : 
* Description  : 
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"   /* MD Macro Driver */
#include "r_cg_wdt.h"           /* MD WDT Driver */

/* Code Standard */
#include "typedef.h"            /* GSCE Standard Typedef */

/* EM */
#include "em_type.h"
#include "em_measurement.h"
#include "em_operation.h"

/* Application */
#include "inst_read.h"


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

/******************************************************************************
Private global variables and functions
******************************************************************************/
EM_INST_READ_PARAMS g_inst_read_params;

/******************************************************************************
* Function Name: void INST_READ_RTC_InterruptCallBack(void)
* Description  : Capture instantaneous parameters from metrology
*              : This capture done at every 1s interval
* Arguments    : None
* Return Value : None
******************************************************************************/
void INST_READ_RTC_InterruptCallBack(void)
{
    static uint8_t count = 0;
    
    count++;
    if (count >= 2)
    {
        /* Update the parameter */
        
        /* Selected Line*/
		g_inst_read_params.selected_line = EM_GetRMSLine();
		
        /* VRMS */
        g_inst_read_params.vrms = EM_GetVoltageRMS();
        
        /* IRMS */
        g_inst_read_params.irms = EM_GetCurrentRMS(EM_LINE_PHASE);
        
        /* Power factor */
        g_inst_read_params.power_factor = EM_GetPowerFactor(EM_LINE_PHASE);
        g_inst_read_params.power_factor_sign = EM_GetPowerFactorSign(EM_LINE_PHASE);
        
        /* Active power */
        g_inst_read_params.active_power = EM_GetActivePower(EM_LINE_PHASE);
        
        /* Reactive power */
        g_inst_read_params.reactive_power = EM_GetReactivePower(EM_LINE_PHASE);
        
        /* Apparent power */
        g_inst_read_params.apparent_power = EM_GetApparentPower(EM_LINE_PHASE);
        
        /* Fundamental active power */
        g_inst_read_params.fundamental_power = EM_GetFundamentalActivePower(EM_LINE_PHASE);
        
        /* IRMS */
        g_inst_read_params.irms2 = EM_GetCurrentRMS(EM_LINE_NEUTRAL);
        
        /* Power factor */
        g_inst_read_params.power_factor2 = EM_GetPowerFactor(EM_LINE_NEUTRAL);
        g_inst_read_params.power_factor_sign2 = EM_GetPowerFactorSign(EM_LINE_NEUTRAL);
        
        /* Active power */
        g_inst_read_params.active_power2 = EM_GetActivePower(EM_LINE_NEUTRAL);
        
        /* Reactive power */
        g_inst_read_params.reactive_power2 = EM_GetReactivePower(EM_LINE_NEUTRAL);
        
        /* Apparent power */
        g_inst_read_params.apparent_power2 = EM_GetApparentPower(EM_LINE_NEUTRAL);
        
        /* Fundamental active power */
        g_inst_read_params.fundamental_power2 = EM_GetFundamentalActivePower(EM_LINE_NEUTRAL);
        
        g_inst_read_params.freq = EM_GetLineFrequency();
        
		{
	    	EM_OPERATION_DATA em_energy_data;
            EM_ENERGY_VALUE em_energy_value;
            
            /* Critical section, energy update in DSAD */
            DI();
	        EM_GetOperationData(&em_energy_data);
            EI();
            EM_EnergyDataToEnergyValue(&em_energy_data, &em_energy_value);
	        g_inst_read_params.active_energy_total_import           = em_energy_value.integer.active_imp        + em_energy_value.decimal.active_imp      ;
	        g_inst_read_params.active_energy_total_export           = em_energy_value.integer.active_exp        + em_energy_value.decimal.active_exp      ;
	        g_inst_read_params.reactive_energy_lag_total_import     = em_energy_value.integer.reactive_ind_imp  + em_energy_value.decimal.reactive_ind_imp;
	        g_inst_read_params.reactive_energy_lag_total_export     = em_energy_value.integer.reactive_ind_exp  + em_energy_value.decimal.reactive_ind_exp;
	        g_inst_read_params.reactive_energy_lead_total_import    = em_energy_value.integer.reactive_cap_imp  + em_energy_value.decimal.reactive_cap_imp;
	        g_inst_read_params.reactive_energy_lead_total_export    = em_energy_value.integer.reactive_cap_exp  + em_energy_value.decimal.reactive_cap_exp;
	        g_inst_read_params.apparent_energy_total_import         = em_energy_value.integer.apparent_imp      + em_energy_value.decimal.apparent_imp    ;
	        g_inst_read_params.apparent_energy_total_export         = em_energy_value.integer.apparent_exp      + em_energy_value.decimal.apparent_exp    ;
        }

        count = 0;
    }
}