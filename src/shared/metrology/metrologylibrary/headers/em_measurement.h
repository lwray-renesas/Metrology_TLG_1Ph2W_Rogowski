/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : Measurement.h
* Version      : 1.00
* Description  : EM Core Measurement Middleware Header file
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _EM_MEASUREMENT_H
#define _EM_MEASUREMENT_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "em_core.h"

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/
/* Power */
float32_t EM_GetActivePower(EM_LINE line);
float32_t EM_GetFundamentalActivePower(EM_LINE line);
float32_t EM_GetReactivePower(EM_LINE line);
float32_t EM_GetApparentPower(EM_LINE line);

/* Energy: Get energy data and convert to equivalent energy value */
uint8_t EM_EnergyDataToEnergyValue(EM_OPERATION_DATA *p_data, EM_ENERGY_VALUE *p_value);
/* Energy: Inject custom energy and add directly to metrology energy data 
 * NOTE for EM_AddEnergyData
 *  Call when metrology not running, metrology accumulate to energy counter in DSAD
 *  Bypass pulse output, breaking the alignment between energy counter and number of pulse
*/
uint8_t EM_EnergyValueToEnergyData(EM_OPERATION_DATA *p_data, EM_ENERGY_VALUE *p_value);
uint8_t EM_AddEnergyData(EM_OPERATION_DATA * p_data);

/* VRMS */
float32_t EM_GetVoltageRMS(void);

/* IRMS */
float32_t EM_GetCurrentRMS(EM_LINE line);

/* PF */
float32_t EM_GetPowerFactor(EM_LINE line);
EM_PF_SIGN EM_GetPowerFactorSign(EM_LINE line);

/* Line Frequency */
float32_t EM_GetLineFrequency(void);

/* Checking functions */
EM_LINE EM_GetRMSLine(void);

#endif /* _EM_MEASUREMENT_H */
