/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : platform.h
* Version      : 1.00
* Description  : Platform Property Header file
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _PLATFORM_PROPERTY_H
#define _PLATFORM_PROPERTY_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "compiler.h"
#include "em_type.h"

/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Macro definitions
******************************************************************************/

/******************************************************************************
*   Platform Default Property
*******************************************************************************/
#define EM_PLATFORM_PROPERTY_TARGET_AC_SOURCE_FREQUENCY         50                              /* Target AC Source Frequency */

/******************************************************************************
*   Platform Default Calibration Informations
*******************************************************************************/
/* Meter Constant */
#define EM_CALIB_DEFAULT_SAMPLING_FREQUENCY                     3898.000000                 /* Actual sampling frequency of the meter    */

/* Co-efficient */
#define EM_CALIB_DEFAULT_COEFF_VRMS                             (13212.6113      )          /* VRMS Co-efficient  At Gain 1.0f           */
#define EM_CALIB_DEFAULT_COEFF_I1RMS                            (79681.9298      )          /* I1RMS Co-efficient At Gain 1.0f           */
#define EM_CALIB_DEFAULT_COEFF_I2RMS                            (62415.9454      )          /* I2RMS Co-efficient At Gain 1.0f           */
#define EM_CALIB_DEFAULT_COEFF_ACTIVE_POWER                     (1052806342.4862 )          /* Active Power Co-efficient At Gain 1.0f    */
#define EM_CALIB_DEFAULT_COEFF_REACTIVE_POWER                   (1052806342.4862 )          /* Reactive Power Co-efficient At Gain 1.0f  */
#define EM_CALIB_DEFAULT_COEFF_APPARENT_POWER                   (1052806342.4862 )          /* Apparent Power Co-efficient At Gain 1.0f  */
#define EM_CALIB_DEFAULT_COEFF_ACTIVE_POWER2                    (824669696.7631  )          /* Active Power Co-efficient At Gain 1.0f    */
#define EM_CALIB_DEFAULT_COEFF_REACTIVE_POWER2                  (824669696.7631  )          /* Reactive Power Co-efficient At Gain 1.0f  */
#define EM_CALIB_DEFAULT_COEFF_APPARENT_POWER2                  (824669696.7631  )          /* Apparent Power Co-efficient At Gain 1.0f  */

/* DSAD Gain */
#define EM_CALIB_DEFAULT_PHASE_GAIN                             (32)
#define EM_CALIB_DEFAULT_NEUTRAL_GAIN                           (2)

/* SW Phase Correction */
/* Phase Gain Phase Shift Angle */
#define EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_PHASE_SHIFT          (-2.804991f )               /* Phase Gain Level 0 Phase Shift Angle (in degree) */
#define EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_PHASE_SHIFT          (-2.805976f )               /* Phase Gain Level 1 Phase Shift Angle (in degree) */
/* Neutral Gain Phase Shift Angle */
#define EM_CALIB_DEFAULT_GAIN_NEUTRAL_LEVEL0_PHASE_SHIFT        (-2.826520f )               /* Neutral Gain Level 0 Phase Shift Angle (in degree) */
#define EM_CALIB_DEFAULT_GAIN_NEUTRAL_LEVEL1_PHASE_SHIFT        (-2.808475f )               /* Neutral Gain Level 1 Phase Shift Angle (in degree) */

/* Gain Value List */
/* Phase Gain Value List */
#define EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL0_VALUE                (1.0f       )               /* Phase Gain Level 0 Value (lowest, value is 1.0, fixed)   */
#define EM_CALIB_DEFAULT_GAIN_PHASE_LEVEL1_VALUE                (2.0005384f )               /* Phase Gain Level 1 Value     |                           */

/* Neutral Gain Value List */
#define EM_CALIB_DEFAULT_GAIN_NEUTRAL_LEVEL0_VALUE              (1.0f       )               /* Neutral Gain Level 0 Value (lowest, value is 1.0, fixed)   */
#define EM_CALIB_DEFAULT_GAIN_NEUTRAL_LEVEL1_VALUE              (2.0037093f )               /* Neutral Gain Level 1 Value     |                           */

/******************************************************************************
Variable Externs
******************************************************************************/
extern FAR_PTR const EM_PLATFORM_PROPERTY   g_EM_DefaultProperty;       /* Default Platform Property */
extern FAR_PTR const EM_CALIBRATION         g_EM_DefaultCalibration;    /* Default Platform Calibration */

/******************************************************************************
Functions Prototypes
******************************************************************************/

#endif /* _EM_PLATFORM_PROPERTY_H */

