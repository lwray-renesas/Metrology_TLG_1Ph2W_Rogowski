/******************************************************************************
  Copyright (C) 2011 Renesas Electronics Corporation, All Rights Reserved.
*******************************************************************************
* File Name    : wrp_em_sw_config.h
* Version      : 1.00
* Description  : Wrapper Configuration Header file
******************************************************************************
* History : DD.MM.YYYY Version Description
******************************************************************************/

#ifndef _WRAPPER_EM_SW_CONFIG_H
#define _WRAPPER_EM_SW_CONFIG_H

/******************************************************************************
Includes   <System Includes> , "Project Includes"
******************************************************************************/
#include "r_cg_macrodriver.h"

    /**************************************************************************
    *   ADC Driver Header
    ***************************************************************************/
    #include "r_cg_dsadc.h"
    
    /**************************************************************************
    *   PULSE Driver Header
    ***************************************************************************/
    #include "r_cg_port.h"

    /**************************************************************************
    *   WDT Driver Header
    ***************************************************************************/
    #include "r_cg_wdt.h"

    /**************************************************************************
    *   TIMER Driver Header
    ***************************************************************************/
    #include "r_cg_tau.h"
    
    /**************************************************************************
    *   Platform definition Header
    ***************************************************************************/
    #include "platform.h"
    
    /**************************************************************************
    *   EM Constraint Header
    ***************************************************************************/
    #include "em_constraint.h"
    
/******************************************************************************
Macro definitions
******************************************************************************/

    /******************************************************************************
    *   Noisy Bit Masking
    *******************************************************************************
    *   The Noisy bit masking is optional, this depend on the signal processing
    *     
    *   In default, the Noisy Bit Masking is ENABLE
    *   To DISABLE, please undefine the macro EM_ENABLE_NOISY_BIT_MASKING
    *   To ENABLE, please define the macro.
    *
    *
    */
    //#define EM_ENABLE_NOISY_BIT_MASKING

    #define EM_MASK_OFF_NOISY_BITS_POSITIVE                 (0xF0)
    #define EM_MASK_OFF_NOISY_BITS_NEGATIVE                 (0x0F)
    
    #if defined(FEATURES_WQFR)   || \
        defined(FEATURES_BQFR)   || \
        defined(FEATURES_ALL)    || \
        (0)
        
    #define METER_WRAPPER_ADC_COPY_FUNDAMENTAL_SAMPLE
    
    #endif /* defined(FEATURES_WQFR) ||
              defined(FEATURES_BQFR) || 
              defined(FEATURES_ALL) */
              
    #define METER_WRAPPER_ADC_COPY_NEUTRAL_SAMPLE

    /* This option below will do integration on input sample in case using the rogowski coil  */
#ifdef ROGOWSKI
    #define METER_ENABLE_INTEGRATOR_ON_SAMPLE               (1)
#else
    #define METER_ENABLE_INTEGRATOR_ON_SAMPLE               (0)
#endif

    /* This option below will do detect DC imunity on phase 1 sample */
    #define METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE     (1)

/******************************************************************************
*   EM Core Software Block (SW Block)
*******************************************************************************/
    /**************************************************************************
    *   SW Wrapper Property Configuration & Settings
    ***************************************************************************/
    #define EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL                1           /* (SW Support Only) - Gain switch number of level: 1 or 2   */
    
    #define EM_SW_PROPERTY_ADC_GAIN_PHASE_UPPER_THRESHOLD       970000      /* (SW Support Only) - Gain switch adc upper threshold     */
    #define EM_SW_PROPERTY_ADC_GAIN_PHASE_LOWER_THRESHOLD       61000       /* (SW Support Only) - Gain switch adc lower threshold     */
    
    #define EM_SW_PROPERTY_ADC_GAIN_NEUTRAL_UPPER_THRESHOLD     1900000     /* (SW Support Only) - Gain switch adc upper threshold     */
    #define EM_SW_PROPERTY_ADC_GAIN_NEUTRAL_LOWER_THRESHOLD     120000      /* (SW Support Only) - Gain switch adc lower threshold     */

    #if ((EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL != 1) && (EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL != 2))
	#error " Please define gain switch number of level 1 or 2 only "
    #endif /* (EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL != 1) && (EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL != 2) */

    #if (EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL == 1)
    #define EM_SW_PROPERTY_ADC_GAIN_PHASE_NUMBER_LEVEL          1
    #define EM_SW_PROPERTY_ADC_GAIN_NEUTRAL_NUMBER_LEVEL        1
    #else
    #define EM_SW_PROPERTY_ADC_GAIN_PHASE_NUMBER_LEVEL          2
    #define EM_SW_PROPERTY_ADC_GAIN_NEUTRAL_NUMBER_LEVEL        2
    #endif /* EM_SW_PROPERTY_ADC_GAIN_NUMBER_LEVEL == 1 */
    
    #define EM_SW_PROPERTY_OPERATION_IRMS_NOLOAD_THRESHOLD      (0.010f)
    #define EM_SW_PROPERTY_OPERATION_POWER_NOLOAD_THRESHOLD     (EM_SW_PROPERTY_OPERATION_IRMS_NOLOAD_THRESHOLD * 180.0f)
    #define EM_SW_PROPERTY_OPERATION_NO_VOLTAGE_THRESHOLD       (10.0f)    /* V */
    #define EM_SW_PROPERTY_OPERATION_FREQUENCY_LOW_THRESHOLD    (40.0f)    /* Hz */
    #define EM_SW_PROPERTY_OPERATION_FREQUENCY_HIGH_THRESHOLD   (70.0f)    /* Hz */
    #define EM_SW_PROPERTY_OPERATION_EARTH_DIFF_THRESHOLD       (12.5f)    /* % */
    #define EM_SW_PROPERTY_OPERATION_METER_CONSTANT             (800)     /* imp/kWh */
    #define EM_SW_PROPERTY_OPERATION_PULSE_ON_TIME              (30.0f)    /* ms */
    #define EM_SW_PROPERTY_OPERATION_ENERY_PULSE_RATIO          (1)        /* Energy resolution equal: (ratio * meter constant) Wh */
    #define EM_SW_PROPERTY_OPERATION_PULSE_EXPORT_DIRECTION     (1)        /* Enable (1) / Disable (0) export direction pulse output */
    #define EM_SW_PROPERTY_OPERATION_PULSE_REACTIVE_ENABLE 	    (1)        /* Enable (1) / Disable (0) reactive pulse output */
    #define EM_SW_PROPERTY_OPERATION_PULSE_APPARENT_ENABLE 	    (0)        /* Enable (1) / Disable (0) apparent pulse output */
    /* Check macro definition */
    #if (EM_SW_PROPERTY_OPERATION_PULSE_REACTIVE_ENABLE) && (EM_SW_PROPERTY_OPERATION_PULSE_APPARENT_ENABLE)
        #error "Only enable Reactive pulse or Apparent pulse "
    #endif


    /* Set rounding for parameter */
    #define EM_SW_PROPERTY_ROUND_DIGIT_POWER                    (4)        /* Number of fraction digit to round up power value */
    #define EM_SW_PROPERTY_ROUND_DIGIT_RMS                      (4)        /* Number of fraction digit to round up rms value */
    #define EM_SW_PROPERTY_ROUND_DIGIT_FREQ                     (1)        /* Number of fraction digit to round up line frequency value */
    #define EM_SW_PROPERTY_ROUND_DIGIT_PF                       (3)        /* Number of fraction digit to round up power factor value */

    #if ((EM_SW_PROPERTY_ROUND_DIGIT_POWER > EM_MAX_ROUNDING_DIGIT) ||  \
        (EM_SW_PROPERTY_ROUND_DIGIT_RMS > EM_MAX_ROUNDING_DIGIT)    ||  \
        (EM_SW_PROPERTY_ROUND_DIGIT_FREQ > EM_MAX_ROUNDING_DIGIT)   ||  \
        (EM_SW_PROPERTY_ROUND_DIGIT_PF > EM_MAX_ROUNDING_DIGIT))        
    #error "Please check the metrology rounding digit settings"
    #endif

    #define EM_SW_PROPERTY_SAMP_SHIFTING90_INTERPOLATION_ERROR  (0.0f)      /* (SW Support Only) - The interpolation error of shifting of 90 degree */
    
    /* Sag and Swell */
    #define EM_SW_PROPERTY_SAG_RMS_RISE_THRESHOLD               (145)       /* V */
    #define EM_SW_PROPERTY_SAG_RMS_FALL_THRESHOLD               (127)       /* V */
    #define EM_SW_PROPERTY_SWELL_RMS_RISE_THRESHOLD             (260)       /* V */
    #define EM_SW_PROPERTY_SWELL_RMS_FALL_THRESHOLD             (242)       /* V */
    #define EM_SW_PROPERTY_SAG_DETECTION_HALF_CYCLE             (3)         /* Number of half cycle for sag detection (rise and fall) */
    #define EM_SW_PROPERTY_SWELL_DETECTION_HALF_CYCLE           (3)         /* Number of half cycle for swell detection (rise and fall) */

    /**************************************************************************
    *   WDT Wrapper Configuration for Settings & I/F mapping
    ***************************************************************************/
    /* I/Fs mapping
     *      Macro Name / I/Fs                                   Setting/Description */
    #define EM_WDT_DriverInit()                                 R_WDT_Create()                  /* WDT Init */
    #define EM_WDT_DriverStart()                                {;}                             /* WDT Start */
    #define EM_WDT_DriverStop()                                 {;}                             /* WDT Stop */
    #define EM_WDT_DriverRestart()                              R_WDT_Restart()                 /* WDT Restart */

    /**************************************************************************
    *   EM TIMER Wrapper Configuration for Settings & I/F mapping
    ***************************************************************************/
    /* I/Fs mapping
     *      Macro Name / I/Fs                                   Setting/Description */
    #define EM_TIMER_DriverInit()                               {;}                             /* Init */
    #define EM_TIMER_DriverStart()                              R_TAU0_Channel2_Start()         /* Start */
    #define EM_TIMER_DriverStop()                               R_TAU0_Channel2_Stop()          /* Stop */

    /**************************************************************************
    *   PULSE Wrapper Configuration for Settings & I/F mapping
    ***************************************************************************/
    /* I/Fs Mapping 
     *      Macro Name / I/Fs                                   Setting/Description */
    #define EM_PULSE_ACTIVE_ON_STATEMENT                              {SET_BIT(P1,2,1);}              /* Pulse 0 LED On */
    #define EM_PULSE_ACTIVE_OFF_STATEMENT                             {SET_BIT(P1,2,0);}              /* Pulse 0 LED Off */
   
   #if (EM_SW_PROPERTY_OPERATION_PULSE_REACTIVE_ENABLE) /* USING REACTIVE PULSE*/
        #define EM_PULSE_REACTIVE_ON_STATEMENT                        {SET_BIT(P1,1,1);}        /* Pulse 1 LED On */
        #define EM_PULSE_REACTIVE_OFF_STATEMENT                       {SET_BIT(P1,1,0);}        /* Pulse 1 LED Off */
        #define EM_PULSE_APPARENT_ON_STATEMENT                        {;}                       /* Pulse 2 LED On */
        #define EM_PULSE_APPARENT_OFF_STATEMENT                       {;}                       /* Pulse 2 LED Off */  
    #else /* USING APPARENT PULSE*/
        #define EM_PULSE_REACTIVE_ON_STATEMENT                        {;}        /* Pulse 1 LED On */
        #define EM_PULSE_REACTIVE_OFF_STATEMENT                       {;}        /* Pulse 1 LED Off */
        #define EM_PULSE_APPARENT_ON_STATEMENT                        {SET_BIT(P1,1,1);}        /* Pulse 2 LED On */
        #define EM_PULSE_APPARENT_OFF_STATEMENT                       {SET_BIT(P1,1,0);}        /* Pulse 2 LED Off */
    #endif /* EM_SW_PROPERTY_OPERATION_PULSE_REACTIVE_ENABLE */

    #define EM_PULSE_DriverInit()                               {;}                             /* Pulse Init */        

    /**************************************************************************
    *   ADC Wrapper Configuration for Settings & I/F mapping
    ***************************************************************************/    
    /* Settings */
    /* Channel Name Specification */
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_VOLTAGE  3                               /* Match this with definitions above */
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE    1                               /* Match this with definitions above */
    #define EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_NEUTRAL  0                               /* Match this with definitions above */

    #define EM_ADC_DRIVER_CHANNEL_VOLTAGE                       (dsad_channel_t)(DSADCHANNEL0+EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_VOLTAGE)       /* AD Driver Voltage Channel ID         */
    #define EM_ADC_DRIVER_CHANNEL_PHASE                         (dsad_channel_t)(DSADCHANNEL0+EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE)         /* AD Driver Voltage Channel ID         */
    #define EM_ADC_DRIVER_CHANNEL_NEUTRAL                       (dsad_channel_t)(DSADCHANNEL0+EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_NEUTRAL)       /* AD Driver Voltage Channel ID         */

    #if (EM_PLATFORM_PROPERTY_TARGET_AC_SOURCE_FREQUENCY == 50)
    #define EM_ADC_DRIVER_PHASE_SHIFT_STEP                      (0.012f)                        /* AD Driver Phase step */
    #elif (EM_PLATFORM_PROPERTY_TARGET_AC_SOURCE_FREQUENCY == 60)
    #define EM_ADC_DRIVER_PHASE_SHIFT_STEP                      (0.0144f)                       /* AD Driver Phase step */
    #else
    #error "Unsupported line frequency, 50Hz or 60Hz only"
    #endif /* EM_PLATFORM_PROPERTY_TARGET_AC_SOURCE_FREQUENCY */

    /* Phase Gain Specification */
    #define EM_ADC_GAIN_PHASE_LEVEL0                            (GAIN_X1)                      /* AD Gain Phase Level 0 (lowest) */
    #define EM_ADC_GAIN_PHASE_LEVEL1                            (GAIN_X1)                      /* AD Gain Phase Level 1 */
    /* Neutral Gain Specification */
    #define EM_ADC_GAIN_NEUTRAL_LEVEL0                          (GAIN_X1 )                      /* AD Gain Neutral Level 0 (lowest) */
    #define EM_ADC_GAIN_NEUTRAL_LEVEL1                          (GAIN_X1 )                      /* AD Gain Neutral Level 1 */
    /* Integrator Enable/Disable */
    #define EM_ADC_INTEGRATOR_CHANNEL_PHASE_ENABLE              (1)                             /* Enable/Disable ADC Integrator for Phase channel */
    #define EM_ADC_INTEGRATOR_CHANNEL_NEUTRAL_ENABLE            (0)                             /* Enable/Disable ADC Integrator for Neutral channel */
    /*
     * Software Phase Adjustment (by ADC ISR) for 3 channels
     * Set as 0 if not want to delay
     */
    #define EM_ADC_DELAY_STEP_VOLTAGE_CHANNEL                   (0)                             /* Step for phase adjustment of voltage channel */
    #define EM_ADC_DELAY_STEP_PHASE_CHANNEL                     (5)                             /* Step for phase adjustment of phase channel */
    #define EM_ADC_DELAY_STEP_NEUTRAL_CHANNEL                   (1)                             /* Step for phase adjustment of neutral channel */
    /* I/Fs mapping
     *      Macro Name / I/Fs                                   Setting/Description */
    #define EM_ADC_DriverInit()                                 {;}                                     /* AD Driver Initialization         */
    #define EM_ADC_DriverStart()                                {R_DSADC_Set_OperationOn(); R_DSADC_Start();}                         /* AD Driver Start                  */
    #define EM_ADC_DriverStop()                                 {R_DSADC_Set_OperationOff(); R_DSADC_Stop();}                          /* AD Driver Stop                   */
    #define EM_ADC_DriverSetPhaseStep(channel, step)            R_DSADC_AdjustPhaseDelay(channel, step) /* AD Driver Set Phase Steps   */
    
/******************************************************************************
Typedef definitions
******************************************************************************/

/******************************************************************************
Variable Externs
******************************************************************************/

/******************************************************************************
Functions Prototypes
******************************************************************************/

#endif /* _WRAPPER_EM_SW_CONFIG_H */
