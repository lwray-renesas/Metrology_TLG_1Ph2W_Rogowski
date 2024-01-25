/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : em_operation.h
* Version      : 1.00
* Description  : EM Core Operation Middleware Header file
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _EM_OPERATION_H
#define _EM_OPERATION_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "compiler.h"
#include "em_constraint.h"  /* EM Constraint Definitions */
#include "em_errcode.h"     /* EM Error Code Definitions */
#include "em_type.h"        /* EM Type Definitions */

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
/* Core Operation */
uint8_t EM_Init(EM_PLATFORM_PROPERTY FAR_PTR * p_property, EM_CALIBRATION FAR_PTR * p_calib);
uint8_t EM_Start(void);
uint8_t EM_Stop(void);
EM_SYSTEM_STATE EM_GetSystemState(void);

/* Status */
EM_STATUS EM_GetStatus(void);

/* Energy accumulation mode get/set:
*  Note: Input value larger than 3, set value still 3
*  uint8_t mode:
*   0: EM Stop accumulating. User update by calling separate API
*   1: EM always use Phase channel power for energy accumulation
*   2: EM always use Neutral channel power for energy accumulation
*   3: EM select between Phase and Neutral based on IRMS value and earth diff threshold configuration
*/
uint8_t EM_GetEnergyAccumulationMode(void);
void EM_SetEnergyAccumulationMode(uint8_t mode);

/* Energy accumulation power set (called this when energy accumulation mode is 0)
 * float32_t active: active power (in Watt)
 * float32_t reactive: reactive power (in VAr)
 * float32_t apparent: apparent power (in VA)
 * Apparent sign will be ignored
 * Active and reactive sign will determine the quadrant of energy accumulation:
 * 		QI  : active > 0, reactive > 0: import active, import inductive reactive, import apparent
 *      QII : active < 0, reactive > 0: export active, import capacitive reactive, export apparent
 * 		QIII: active < 0, reactive < 0: export active, export inductive reactive, export apparent
 * 		QIV : active > 0, reactive < 0: import active, export capacitive reactive, import apparent
*/
void EM_SetEnergyAccumulationPower(float32_t active, float32_t reactive, float32_t apparent);

/* Metrology operation data get/set:
 * Used to backup (get) / restore (set) internal metrology data
 * Operation data used backup/restore for:
 *  - Energy counter
 *  - Internal energy accumulation data
*/
uint8_t EM_GetOperationData(EM_OPERATION_DATA * p_operation_data);
uint8_t EM_SetOperationData(EM_OPERATION_DATA * p_operation_data);

#endif /* _EM_OPERATION_H */
