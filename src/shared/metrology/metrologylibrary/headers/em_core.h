/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : em_core.h
* Version      : 1.00
* Description  : EM Core User' Definitions & APIs
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _EM_CORE_H
#define _EM_CORE_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "compiler.h"
#include "em_constraint.h"  /* EM Constraint Definitions */
#include "em_errcode.h"     /* EM Error Code Definitions */
#include "em_type.h"        /* EM Type Definitions */

/* EM Core Component */
#include "em_operation.h"   /* EM Core Operation APIs */
#include "em_calibration.h" /* EM Calibration APIs */
#include "em_measurement.h" /* EM Measurement APIs */

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
/* ADC Interrupt Callback Processing */
void EM_ADC_IntervalProcessing(EM_SAMPLES * p_samples);

/* TIMER Interrupt Callback Processing */
void EM_TIMER_InterruptCallback(void);

#endif /* _EM_CORE_H */
