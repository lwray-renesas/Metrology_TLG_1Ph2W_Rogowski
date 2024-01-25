/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : em_calibration.h
* Version      : 1.00
* Description  : EM Core Calibration Middleware Header file
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _EM_CALIBRATION_H
#define _EM_CALIBRATION_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
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
/* Get/Set/Reset Calibration Info */
EM_CALIBRATION EM_GetCalibInfo(void);
uint8_t EM_SetCalibInfo(EM_CALIBRATION FAR_PTR * p_calib);
uint8_t EM_CalibInitiate(EM_CALIB_ARGS FAR_PTR * p_calib_args, EM_CALIB_WORK * p_calib_work, EM_CALIB_OUTPUT * p_calib_output);
uint8_t EM_CalibRun(void);

/* RTC Interrupt Callback Processing */
void EM_RTC_CalibInterruptCallback(void);

#endif /* _EM_CALIBRATION_H */
