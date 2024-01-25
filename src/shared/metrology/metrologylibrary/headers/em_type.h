/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : em_type.h
* Version      : 1.00
* Description  : EM Core Type Definitions
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _EM_TYPE_H
#define _EM_TYPE_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "typedef.h"        /* GSCE Standard Typedef */

/******************************************************************************
Macro definitions
******************************************************************************/
#define EM_SW_SAMP_TYPE                                 int32_t                                 /* Sampling Data Type */

/******************************************************************************
Typedef definitions
******************************************************************************/
typedef enum tagEMLine
{
    EM_LINE_PHASE = 0,
    EM_LINE_NEUTRAL = 3,
} EM_LINE;

typedef enum tagEMPFSign
{
    PF_SIGN_LEAD_C  = -1,       /* Lead, C */
    PF_SIGN_UNITY   = 0,        /* Unity */
    PF_SIGN_LAG_L   = 1     /* Lag, L */
} EM_PF_SIGN;

/* EM Status */
typedef struct tagEMStatus
{
    uint16_t    noload_active : 1;          /* Active No-Load status */
    uint16_t    noload_reactive : 1;        /* Reactive No-Load status */
    uint16_t    noload_active2 : 1;         /* Active2 No-Load status */
    uint16_t    noload_reactive2 : 1;       /* Reactive2 No-Load status */
    uint16_t    is_voltage_sag : 1;         /* Voltage Sag */
    uint16_t    is_voltage_swell : 1;       /* Voltage Swell */
} EM_STATUS;

/* EM Energy Counter */
typedef struct tagEMEnergyCounter
{
    uint64_t active_imp;
    uint64_t active_exp;
    uint64_t reactive_ind_imp;
    uint64_t reactive_ind_exp;
    uint64_t reactive_cap_imp;
    uint64_t reactive_cap_exp;
    uint64_t apparent_imp;
    uint64_t apparent_exp;
} EM_ENERGY_COUNTER;

typedef struct tagEMEnergyValue
{
    struct {
        uint64_t active_imp;
        uint64_t active_exp;
        uint64_t reactive_ind_imp;
        uint64_t reactive_ind_exp;
        uint64_t reactive_cap_imp;
        uint64_t reactive_cap_exp;
        uint64_t apparent_imp;
        uint64_t apparent_exp;
    } integer;
    
    struct {
        float32_t active_imp;
        float32_t active_exp;
        float32_t reactive_ind_imp;
        float32_t reactive_ind_exp;
        float32_t reactive_cap_imp;
        float32_t reactive_cap_exp;
        float32_t apparent_imp;
        float32_t apparent_exp;
    } decimal;
} EM_ENERGY_VALUE;

typedef enum tagEMCalibrationSteps
{
    EM_CALIB_STEP_NOT_INITIATED = 0x00,
    EM_CALIB_STEP_FS,
    EM_CALIB_STEP_IGAIN,
    EM_CALIB_STEP_SIGNALS,
    EM_CALIB_STEP_ANGLE,
} EM_CALIB_STEP;

typedef struct tagEMCalibrationArgs
{
    /* System information */
    uint16_t        rtc_period;         /* 1000 or 500 only (corresponding to 1s or 0.5s period interrupt) */
    uint16_t        max_gvalue;         /* Maximum gain value (before convert to dsasd gain value) (using single gain) */
    uint8_t         stable_ndelay;      /* Advance option: Internal delay between each phase and gain set, in multiple of 10ms (10 is recommended (100ms)) */

    /* Power supply parameters */
    uint8_t         cycle;              /* Number of line cycle used for fs, gain and coefficient calibration */
    uint8_t         cycle_angle;        /* Number of line cycle used for signals angle calibration */
    float32_t       v;                  /* Voltage value on reference power supply */
    float32_t       i;                  /* Current value on reference power supply */
    float32_t       imax;               /* Max measuring current for calibrating meter */

    /* Line selection */
    EM_LINE         line_v;             /* Voltage line selection */
    EM_LINE         line_i;             /* Current line selection */
} EM_CALIB_ARGS;

typedef uint16_t EM_CALIB_WORK[128];    /* 256 bytes even alignment working area for calibration */

typedef struct tagEMCalibrationOutput
{
    float32_t       fs;             /* Sampling frequency */
    float32_t       gain;           /* Current gain */
    float32_t       gain1;          /* Current gain 1 */
    float32_t       vcoeff;         /* Voltage coefficient */
    float32_t       icoeff;         /* Current coefficient */
    float32_t       pcoeff;         /* Power coefficient, for active, reactive and apparent */
    float32_t       angle_error;    /* Angle error between current and volt */
    float32_t       angle_error1;   /* Angle error between current and volt gain 1 */
    EM_CALIB_STEP   step;           /* Current step during calibration */

} EM_CALIB_OUTPUT;

/* EM Platform Property
 * (Can NOT change in run-time, select ONCE and then FIXED) */
typedef struct tagEMPlatformProperty
{
    uint8_t                 target_ac_source_frequency;         /* Target AC Source frequency (50Hz or 60Hz) */
    uint8_t                 reserved;                           /* No use */
    
} EM_PLATFORM_PROPERTY;

/* EM Calibration Informations
 * (Can change in run-time) */
typedef struct tagEMCalibration
{
    float32_t       sampling_frequency; /* Actual sampling frequency of the meter */
    
    /* Co-efficient */
    struct
    {
        /* RMS Co-efficient */
        float32_t   vrms;               /* VRMS Co-efficient */
        float32_t   i1rms;              /* I1RMS Co-efficient - Phase Current Co-efficient */
        float32_t   i2rms;              /* I2RMS Co-efficient - Neutral Current Co-efficient*/
        
        /* Power Co-efficient (for phase line) */
        float32_t   active_power;       /* I1 Active power co-efficient */
        float32_t   reactive_power;     /* I1 Reactive power co-efficient */
        float32_t   apparent_power;     /* I1 Apparent power co-efficient */
        
        /* Power Co-efficient (for neutral line) */
        float32_t   active_power2;      /* I2 Active power co-efficient */
        float32_t   reactive_power2;    /* I2 Reactive power co-efficient */
        float32_t   apparent_power2;    /* I2 Apparent power co-efficient */        
    } coeff;
    
    /* SW Phase Correction */
    struct
    {
        float32_t FAR_PTR * i1_phase_degrees;   /* I1 Phase Angle Degree List */
        float32_t FAR_PTR * i2_phase_degrees;   /* I2 Phase Angle Degree List */
    } sw_phase_correction;
    
    struct
    {
        float32_t FAR_PTR * i1_gain_values;     /* I1 Gain Value List */
        float32_t FAR_PTR * i2_gain_values;     /* I2 Gain Value List */
    } sw_gain;

} EM_CALIBRATION;       /* EM Calibration Informations */

/* EM System State */
typedef enum tagEMSystemState
{
    SYSTEM_STATE_UNINITIALIZED = 0,     /* Uninitialized */
    SYSTEM_STATE_INITIALIZED,           /* Initialized */
    SYSTEM_STATE_RUNNING                /* Running */
} EM_SYSTEM_STATE;  /* EM System State */

typedef struct tagEMSamples
{
    EM_SW_SAMP_TYPE     v;              /* Voltage */
    EM_SW_SAMP_TYPE     i1;             /* Current phase */
    EM_SW_SAMP_TYPE     i2;             /* Current neutral */
    EM_SW_SAMP_TYPE     v_fund;         /* Filtered voltage for fundamental calculation */
    EM_SW_SAMP_TYPE     i1_fund;        /* Filtered current1 for fundamental calculation */
    EM_SW_SAMP_TYPE     i2_fund;        /* Filtered current2 for fundamental calculation */

} EM_SAMPLES;
typedef struct tagEMSoftwareProperty
{   
    struct
    {
        uint8_t     gain_phase_num_level;           /* (SW Support Only) ADC gain phase number of level */
        uint32_t    gain_phase_upper_threshold;     /* (SW Support Only) ADC gain phase upper threshold */
        uint32_t    gain_phase_lower_threshold;     /* (SW Support Only) ADC gain phase lower threshold */
        uint8_t     gain_neutral_num_level;         /* (SW Support Only) ADC gain neutral number of level */
        uint32_t    gain_neutral_upper_threshold;   /* (SW Support Only) ADC gain neutral upper threshold */
        uint32_t    gain_neutral_lower_threshold;   /* (SW Support Only) ADC gain neutral lower threshold */    
    } adc;          /* ADC Module */
    
    struct
    {
        float32_t       irms_noload_threshold;      /* (SW Support Only) Set the threshold for IRMS No Load Detection (Ampere) */
        float32_t       power_noload_threshold;     /* (SW Support Only) Set the threshold for Power No Load Detection (Watt) */
        float32_t       no_voltage_threshold;       /* Voltage lowest RMS level (Volt) */
        float32_t       freq_low_threshold;         /* Lowest frequency (Hz) */
        float32_t       freq_high_threshold;        /* Highest frequency (Hz) */
        float32_t       earth_diff_threshold;       /* Different threshold for selecting Phase-Neutral (%) */
        uint32_t        meter_constant;             /* Meter constant (imp/KWh) */
        float32_t       pulse_on_time;              /* Pulse on time (ms) */
        uint8_t         energy_pulse_ratio;         /* Ratio of energy step vs pulse meter constant: 1-255 */
        uint8_t         pulse_export_direction;     /* Option to output pulse for export direction: 0 or 1 */
        uint8_t         enable_pulse_reactive;              
        uint8_t         enable_pulse_apparent;
    } operation;    /* Metrology Operation */
    
    struct
    {
        uint8_t         power;
        uint8_t         rms;
        uint8_t         freq;
        uint8_t         pf;
    } rounding;     /* Metrology Rounding Digits */

    struct
    {
        float32_t   shifting90_interpolation_error; /* The error of interpolation */
    } samp;         /* Sampling timer module */

    struct
    {
        float32_t       sag_rms_rise_threshold;                     /* (SW Support Only) The VRMS rising threshold to detect Sag Event (restoration) */
        float32_t       sag_rms_fall_threshold;                     /* (SW Support Only) The VRMS falling threshold to detect Sag Event (occurence) */
        float32_t       swell_rms_rise_threshold;                   /* (SW Support Only) The VRMS rising threshold to detect Swell Event (occurence) */
        float32_t       swell_rms_fall_threshold;                   /* (SW Support Only) The VRMS falling threshold to detect Swell Event (restoration) */
        uint16_t        sag_detection_half_cycle;                   /* (SW Support Only) Number of signal half-cycle to detect Sag Event, 0 means no detection */
        uint16_t        swell_detection_half_cycle;                 /* (SW Support Only) Number of signal half-cycle to detect Swell Event, 0 means no detection */
    } sag_swell;    /* Sag and Swell detection */

} EM_SW_PROPERTY;

typedef struct tagEMOperationData
{
    EM_ENERGY_COUNTER energy_counter;
    struct
    {
        uint32_t active_imp;
        uint32_t active_exp;
        uint32_t reactive_ind_imp;
        uint32_t reactive_ind_exp;
        uint32_t reactive_cap_imp;
        uint32_t reactive_cap_exp;
        uint32_t apparent_imp;
        uint32_t apparent_exp;
        uint8_t pulse_active;
        uint8_t pulse_reactive;
        uint8_t pulse_apparent;
        uint8_t padding;
    } remainder;
    
} EM_OPERATION_DATA;
/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

#endif /* _EM_TYPE_H */
