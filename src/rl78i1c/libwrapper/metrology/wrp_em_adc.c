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
 * File Name    : wrp_em_adc.c
 * Version      : 1.00
 * Device(s)    : RL78/I1C
 * Tool-Chain   : CubeSuite Version 1.5d
 * H/W Platform : RL78/I1C Energy Meter Platform
 * Description  : ADC Wrapper for RL78/I1C Platform
 ******************************************************************************
 * History : DD.MM.YYYY Version Description
 ******************************************************************************/

/******************************************************************************
Includes   <System Includes> , "Project Includes"
 ******************************************************************************/
/* Driver */
#include "r_cg_macrodriver.h"
#include "r_cg_dsadc.h"

/* Wrapper */
#include "wrp_em_sw_config.h"               /* Wrapper Shared Configuration */
#include "wrp_em_adc.h"                     /* Wrapper ADC definitions */
#include "r_dscl_iirbiquad_i32_onestage.h"  /* Library 32bit IIRBiquad */
#if METER_ENABLE_INTEGRATOR_ON_SAMPLE == 1
#include "em_integrator.h"      /* Runge-Kutta Integrator definitions */
#endif
#include "rl78_sw_dc_correction.h"			/* Renesas DC Imunity detection library */

/* Middleware */
#include "em_type.h"
#include "em_core.h"

/* Application */
#include "platform.h"
#include "calibration.h"

/******************************************************************************
Typedef definitions
 ******************************************************************************/
typedef struct
{
	int16_t pos;        /* Position */
	uint16_t step;      /* Step */
	int32_t *p_value;   /* Array of signals */
} shift_circle_t;

/******************************************************************************
Macro definitions
 ******************************************************************************/
#define EM_ADC_CHANNEL_RESULT_REG_ADDR_VOLTAGE              (&(JOIN(DSADCR, EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_VOLTAGE)))
#define EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE                (&(JOIN(DSADCR, EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_PHASE)))
#define EM_ADC_CHANNEL_RESULT_REG_ADDR_NEUTRAL              (&(JOIN(DSADCR, EM_ADC_CURRENT_DRIVER_CHANNEL_OPTIMIZATION_NEUTRAL)))

#if (defined METER_WRAPPER_ADC_COPY_NEUTRAL_SAMPLE)
#define EM_ADC_DRIVER_READ_NEUTRAL() {\
		*((uint16_t *)&g_wrp_adc_samples.i2) = *((uint16_t *)EM_ADC_CHANNEL_RESULT_REG_ADDR_NEUTRAL); \
		*((uint8_t *)&g_wrp_adc_samples.i2 + 0x02) = *((uint8_t *)EM_ADC_CHANNEL_RESULT_REG_ADDR_NEUTRAL + 0x02); \
		*((int8_t *)&g_wrp_adc_samples.i2 + 0x03) = (*((int8_t *)EM_ADC_CHANNEL_RESULT_REG_ADDR_NEUTRAL + 0x02)) >> 7; \
}
#else
#define EM_ADC_DRIVER_READ_NEUTRAL() {;}
#endif

#define EM_ADC_DRIVER_READ_ALL()    {\
		/* Voltage channel */\
		*((uint16_t *)&g_wrp_adc_samples.v)         = *((uint16_t *)EM_ADC_CHANNEL_RESULT_REG_ADDR_VOLTAGE);\
		*((uint8_t *)&g_wrp_adc_samples.v + 0x02)   = *((uint8_t *)EM_ADC_CHANNEL_RESULT_REG_ADDR_VOLTAGE + 0x02);\
		*((int8_t *)&g_wrp_adc_samples.v + 0x03)    = (*((int8_t *)EM_ADC_CHANNEL_RESULT_REG_ADDR_VOLTAGE + 0x02)) >> 7;\
		/* Phase channel */\
		*((uint16_t *)&g_wrp_adc_samples.i1)        = *((uint16_t *)EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE);\
		*((uint8_t *)&g_wrp_adc_samples.i1 + 0x02)  = *((uint8_t *)EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE + 0x02);\
		*((int8_t *)&g_wrp_adc_samples.i1 + 0x03)   = (*((int8_t *)EM_ADC_CHANNEL_RESULT_REG_ADDR_PHASE + 0x02)) >> 7;\
		/* Neutral channel */\
		EM_ADC_DRIVER_READ_NEUTRAL();\
}

		/******************************************************************************
Imported global variables and functions (from other files)
		 ******************************************************************************/

		/******************************************************************************
Exported global variables and functions (to be accessed by other files)
		 ******************************************************************************/

		/******************************************************************************
Private global variables and functions
		 ******************************************************************************/
		/* Variables */
#if (EM_ADC_DELAY_STEP_VOLTAGE_CHANNEL > 0)
		int32_t         g_wrp_adc_voltage_value[EM_ADC_DELAY_STEP_VOLTAGE_CHANNEL+1];
		shift_circle_t  g_wrp_adc_voltage_circle =
		{
				0,                                  /* Pos */
				EM_ADC_DELAY_STEP_VOLTAGE_CHANNEL,  /* Step */
				g_wrp_adc_voltage_value,            /* Array of signals */
		};
#endif
#if (EM_ADC_DELAY_STEP_PHASE_CHANNEL > 0)
		int32_t         g_wrp_adc_phase_value[EM_ADC_DELAY_STEP_PHASE_CHANNEL+1];
		shift_circle_t  g_wrp_adc_phase_circle =
		{
				0,                                  /* Pos */
				EM_ADC_DELAY_STEP_PHASE_CHANNEL,    /* Step */
				g_wrp_adc_phase_value,              /* Array of signals */
		};
#endif
#if (EM_ADC_DELAY_STEP_NEUTRAL_CHANNEL > 0)
		int32_t         g_wrp_adc_neutral_value[EM_ADC_DELAY_STEP_NEUTRAL_CHANNEL+1];
		shift_circle_t  g_wrp_adc_neutral_circle =
		{
				0,                                  /* Pos */
				EM_ADC_DELAY_STEP_NEUTRAL_CHANNEL,  /* Step */
				g_wrp_adc_neutral_value,            /* Array of signals */
		};
#endif

		/* Sub-functions */
		static void EM_ADC_DoPhaseDelay(
				int32_t *signal,
				shift_circle_t *p_circle
		);

		/* Gain level */
		static uint8_t  g_EM_ADC_GainPhaseCurrentLevel;     /* The current level of phase gain */
		static uint8_t  g_EM_ADC_GainNeutralCurrentLevel;   /* The current level of neutral gain */

		/* Static I/F for this module */
		/* Changing gain, apply gain value to the port pin */
		static void EM_ADC_GainPhaseSet(void);              /* Set a suitable gain phase level to a port */
		static void EM_ADC_GainNeutralSet(void);            /* Set a suitable gain neutral level to a port */

#ifdef __DEBUG
		/* Single Line Cycle*/
#define EM_MAX_SAMPLE_BUFFER        (200)
		volatile uint16_t            g_sample_count = EM_MAX_SAMPLE_BUFFER;
		volatile EM_SW_SAMP_TYPE     g_sample0[EM_MAX_SAMPLE_BUFFER];
		volatile EM_SW_SAMP_TYPE     g_sample1[EM_MAX_SAMPLE_BUFFER];
		volatile uint8_t             g_sample1_direction = 0;    /* I1 */
		const uint16_t      g_sample_max_count = EM_MAX_SAMPLE_BUFFER;
#endif

#if (METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE == 1)
		volatile uint8_t g_dc_immunity_state= 0;
		volatile int16_t g_dc_detected_count = 0;
#define DC_DETECTION_COUNT_THRESHOLD        (12000)       /* 256us per detection routine (in DSAD interrupt)
 * Need 6s delay means ~24000 count.
 * But in normal signal condition, this count would start from negative,
 * So choosing half of neccessary count is enough
 */
#endif

		/* Sample storage of wrapper */
		EM_SAMPLES g_wrp_adc_samples;

		/******************************************************************************
		 * Function Name    : static void EM_ADC_GainPhaseSet(void)
		 * Description      : Set a suitable gain phase level to a port
		 * Arguments        : None
		 * Functions Called : None
		 * Return Value     : None
		 ******************************************************************************/
		static void EM_ADC_GainPhaseSet(void)
		{
			switch (g_EM_ADC_GainPhaseCurrentLevel)
			{
			case 0:     /* Set gain level 0 */
				R_DSADC_SetChannelGain(EM_ADC_DRIVER_CHANNEL_PHASE, EM_ADC_GAIN_PHASE_LEVEL0);
				break;
			case 1:     /* Set gain level 1 */
				R_DSADC_SetChannelGain(EM_ADC_DRIVER_CHANNEL_PHASE, EM_ADC_GAIN_PHASE_LEVEL1);
				break;
			default:
				break;
			};
		}

		/******************************************************************************
		 * Function Name    : static void EM_ADC_GainNeutralSet(void)
		 * Description      : Set a suitable gain neutral level to a port
		 * Arguments        : None
		 * Functions Called : None
		 * Return Value     : None
		 ******************************************************************************/
		static void EM_ADC_GainNeutralSet(void)
		{
			switch (g_EM_ADC_GainNeutralCurrentLevel)
			{
			case 0:     /* Set gain level 0 */
				R_DSADC_SetChannelGain(EM_ADC_DRIVER_CHANNEL_NEUTRAL, EM_ADC_GAIN_NEUTRAL_LEVEL0);
				break;
			case 1:     /* Set gain level 1 */
				R_DSADC_SetChannelGain(EM_ADC_DRIVER_CHANNEL_NEUTRAL, EM_ADC_GAIN_NEUTRAL_LEVEL1);
				break;
			default:
				break;
			};
		}

		/******************************************************************************
		 * Function Name    : static void EM_ADC_DoPhaseDelay(
		 *                  :    int32_t *signal,
		 *                  :    int32_t *p_circle
		 *                  : )
		 * Description      : Shift input signal as expected step
		 * Arguments        : *signal  : Signal income then outcome
		 *                  : *p_circle: Circle structure used to control the phase
		 * Functions Called : TBD
		 * Return Value     : None
		 ******************************************************************************/
		static void EM_ADC_DoPhaseDelay(
				int32_t *signal,
				shift_circle_t *p_circle
		)
		{
			int16_t push_pos;
			int16_t pop_pos;

			push_pos = p_circle->pos;
			if (push_pos <= 0)
			{
				pop_pos = p_circle->step;
			}
			else
			{
				pop_pos = push_pos - 1;
			}

			/* PUSH: Add the signal into the data circle at circlepos position */
			p_circle->p_value[push_pos] = *signal;

			/* POP: Get the signal from data circle from (pos + steps) position */
			*signal = p_circle->p_value[pop_pos];

			/* Scroll the circle, scroll left */
			push_pos--;
			if (push_pos == -1)
			{
				push_pos = p_circle->step;
			}

			p_circle->pos = push_pos;
		}

		/******************************************************************************
		 * Function Name    : static void EM_ADC_DoNoisyBitMasking(void)
		 * Description      : Do noisy bit masking for signals
		 * Functions Called : None
		 * Return Value     : None
		 ******************************************************************************/
		static void EM_ADC_DoNoisyBitMasking(void)
		{
			int16_t v_mask;
			int16_t i1_mask;
			int16_t i2_mask;

			/************************************************************
			 * Noisy bit masking
			 *  - Find the signal mask
			 *************************************************************/
#ifdef EM_ENABLE_NOISY_BIT_MASKING
			/* Find the mask of V, I1, I2 for signal accumulation */
			v_mask      = (int16_t)(g_wrp_adc_samples.v       >> 16);
			i1_mask     = (int16_t)(g_wrp_adc_samples.i1      >> 16);
#ifdef METER_WRAPPER_ADC_COPY_NEUTRAL_SAMPLE
			i2_mask     = (int16_t)(g_wrp_adc_samples.i2      >> 16);
#endif /* METER_WRAPPER_ADC_COPY_NEUTRAL_SAMPLE */

			if (v_mask == 0x0000)
			{
				/* Mask off the noise, 4 low-bits */
				*((uint8_t *)&g_wrp_adc_samples.v) &= EM_MASK_OFF_NOISY_BITS_POSITIVE;
			}
			else if (v_mask == 0xffff)
			{
				/* Mask off the noise, 4 low-bits */
				*((uint8_t *)&g_wrp_adc_samples.v) |= EM_MASK_OFF_NOISY_BITS_NEGATIVE;
			}

			if (i1_mask == 0x0000)
			{
				/* Mask off the noise, 4 low-bits */
				*((uint8_t *)&g_wrp_adc_samples.i1) &= EM_MASK_OFF_NOISY_BITS_POSITIVE;
			}
			else if (i1_mask == 0xffff)
			{
				/* Mask off the noise, 4 low-bits */
				*((uint8_t *)&g_wrp_adc_samples.i1) |= EM_MASK_OFF_NOISY_BITS_NEGATIVE;
			}

#ifdef METER_WRAPPER_ADC_COPY_NEUTRAL_SAMPLE
			if (i2_mask == 0x0000)
			{
				/* Mask off the noise, 4 low-bits */
				*((uint8_t *)&g_wrp_adc_samples.i2) &= EM_MASK_OFF_NOISY_BITS_POSITIVE;
			}
			else if (i2_mask == 0xffff)
			{
				/* Mask off the noise, 4 low-bits */
				*((uint8_t *)&g_wrp_adc_samples.i2) |= EM_MASK_OFF_NOISY_BITS_NEGATIVE;
			}
#endif /* METER_WRAPPER_ADC_COPY_NEUTRAL_SAMPLE */
#endif /* EM_ENABLE_NOISY_BIT_MASKING */
		}

		/******************************************************************************
		 * Function Name    : void EM_ADC_Init(void)
		 * Description      : Set a suitable gain neutral level to a port
		 * Arguments        : None
		 * Functions Called : EM_ADC_DriverInit()
		 * Return Value     : None
		 ******************************************************************************/
		void EM_ADC_Init(void)
		{
			/* Init ADC HW Module */
			EM_ADC_DriverInit();
		}

		/******************************************************************************
		 * Function Name    : void EM_ADC_Start(void)
		 * Description      : Start ADC sampling
		 * Arguments        : None
		 * Functions Called : EM_ADC_DriverStart()
		 * Return Value     : None
		 ******************************************************************************/
		void EM_ADC_Start(void)
		{
			/* Initial DC_Imunit Libary and Reset data */
#if (METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE == 1)
			R_DC_Imunity_Init(&g_dc_imunity_config_default_4k_50Hz);
			R_DC_Imunity_Reset_Phase_1();
#endif

#ifdef METER_WRAPPER_ADC_COPY_FUNDAMENTAL_SAMPLE
			R_DSCL_IIRBiquad_i32i32_onestage_Reset();
			R_DSCL_IIRBiquad_i32i32_onestage_Reset2();
			/* Enable 1 current channel */
			//R_DSCL_IIRBiquad_i32i32_onestage_Reset3();
#endif

			/* Start ADC HW Module */
			EM_ADC_DriverStart();
		}

		/******************************************************************************
		 * Function Name    : void EM_ADC_Stop(void)
		 * Description      : Stop ADC sampling
		 * Arguments        : None
		 * Functions Called : EM_ADC_DriverStop()
		 * Return Value     : None
		 ******************************************************************************/
		void EM_ADC_Stop(void)
		{
			/* Stop ADC HW Module */
			EM_ADC_DriverStop();
		}


		/******************************************************************************
		 * Function Name    : void EM_ADC_InterruptCallback(void)
		 * Description      : ADC Interrupt callback processing
		 * Arguments        : None
		 * Functions Called : TBD
		 * Return Value     : None
		 ******************************************************************************/
		void EM_ADC_InterruptCallback(void)
		{
			/* Read all dsad result to buffer */
			EM_ADC_DRIVER_READ_ALL();

		    g_wrp_adc_samples.i1 = g_wrp_adc_samples.i1 * (-1); /* Change direction*/

			EM_ADC_DoNoisyBitMasking();

			/* Phase adjustment for each channel if enabled */
#if (EM_ADC_DELAY_STEP_VOLTAGE_CHANNEL > 0)
			EM_ADC_DoPhaseDelay(
					&g_wrp_adc_samples.v,
					&g_wrp_adc_voltage_circle
			);
#endif

#if (EM_ADC_DELAY_STEP_PHASE_CHANNEL > 0)
			EM_ADC_DoPhaseDelay(
					&g_wrp_adc_samples.i1,
					&g_wrp_adc_phase_circle
			);
#endif

#if (EM_ADC_DELAY_STEP_NEUTRAL_CHANNEL > 0)
			EM_ADC_DoPhaseDelay(
					&g_wrp_adc_samples.i2,
					&g_wrp_adc_neutral_circle
			);
#endif

#ifdef METER_WRAPPER_ADC_COPY_FUNDAMENTAL_SAMPLE
			/* Filter the voltage signal for fundamental calculation, do when normal running only */
			R_DSCL_IIRBiquad_i32i32_onestage_c_nr(g_wrp_adc_samples.v, &g_wrp_adc_samples.v_fund);
			/* Filter the current signal for fundamental calculation, do when normal running only */
			/* Enable 1 current channel */
			R_DSCL_IIRBiquad_i32i32_onestage_c_nr2(g_wrp_adc_samples.i1, &g_wrp_adc_samples.i1_fund);
			//R_DSCL_IIRBiquad_i32i32_onestage_c_nr3(g_wrp_adc_samples.i2, &g_wrp_adc_samples.i2_fund);
#endif

			/* Integrate the signal if using Rogowski coil */
#if (METER_ENABLE_INTEGRATOR_ON_SAMPLE == 1)
#ifndef __DEBUG
			g_datalog_sample_before_integrate_i1 = g_wrp_adc_samples.i1;
			g_datalog_sample_before_integrate_i2 = g_wrp_adc_samples.i2;
#endif
			/* Integrate the signal from Rogowski coil, API will append later */
#if (EM_ADC_INTEGRATOR_CHANNEL_PHASE_ENABLE == 1)
			g_wrp_adc_samples.i1 = EM_ADC_Integrator0(g_wrp_adc_samples.i1);
#endif /* EM_ADC_INTEGRATOR_CHANNEL_PHASE_ENABLE == 1 */
#if (EM_ADC_INTEGRATOR_CHANNEL_NEUTRAL_ENABLE == 1)
			g_wrp_adc_samples.i2 = EM_ADC_Integrator1(g_wrp_adc_samples.i2);
#endif /* EM_ADC_INTEGRATOR_CHANNEL_NEUTRAL_ENABLE == 1 */

#endif


#if (METER_ENABLE_DC_IMUNITY_DETECTION_ON_SAMPLE == 1)
			g_dc_immunity_state = R_DC_Imunity_Calc_Phase_1(g_wrp_adc_samples.i1);
#endif

			/* Debug the signal */
#ifdef __DEBUG
#ifndef METER_ENABLE_MEASURE_CPU_LOAD
			if (g_sample_count < EM_MAX_SAMPLE_BUFFER)
			{
				g_sample0[g_sample_count] = g_wrp_adc_samples.v;
				if (g_sample1_direction == 0)
				{
					g_sample1[g_sample_count] = g_wrp_adc_samples.i1;
				}
				else if (g_sample1_direction == 1)
				{
					g_sample1[g_sample_count] = g_wrp_adc_samples.i2;
				}
				g_sample_count++;
			}
#endif /* METER_ENABLE_MEASURE_CPU_LOAD */
#endif /* __DEBUG */

			{
				{
					/* Normal processing */
					EM_ADC_IntervalProcessing(&g_wrp_adc_samples);
				}
			}
		}

		/******************************************************************************
		 * Function Name    : void EM_ADC_GainReset(EM_LINE line)
		 * Description      : Reset phase gain to lowest level
		 * Arguments        : None
		 * Functions Called : None
		 * Return Value     : None
		 ******************************************************************************/
		void EM_ADC_GainReset(EM_LINE line)
		{
			switch (line)
			{
			case EM_LINE_PHASE:
				g_EM_ADC_GainPhaseCurrentLevel = 0;     /* Lowest */
				EM_ADC_GainPhaseSet();
				break;
			case EM_LINE_NEUTRAL:
				g_EM_ADC_GainNeutralCurrentLevel = 0;       /* Lowest */
				EM_ADC_GainNeutralSet();
				break;
			default:
				break;
			}
		}

		/******************************************************************************
		 * Function Name    : void EM_ADC_GainIncrease(EM_LINE line)
		 * Description      : Increase phase gain 1 level & set the gain level to port
		 * Arguments        : None
		 * Functions Called : EM_ADC_GainPhaseSet()
		 * Return Value     : None
		 ******************************************************************************/
		void EM_ADC_GainIncrease(EM_LINE line)
		{
			switch (line)
			{
			case EM_LINE_PHASE:
				if (g_EM_ADC_GainPhaseCurrentLevel >= EM_GAIN_PHASE_NUM_LEVEL_MAX - 1)    /* Is highest gain? */
				{
					return; /* Do nothing */
				}
				else    /* Still exist higher gain */
				{
					g_EM_ADC_GainPhaseCurrentLevel++;   /* Increase level count */
					EM_ADC_GainPhaseSet();              /* Apply gain */
				}
				break;
			case EM_LINE_NEUTRAL:
				if (g_EM_ADC_GainNeutralCurrentLevel >= EM_GAIN_NEUTRAL_NUM_LEVEL_MAX - 1)    /* Is highest gain? */
				{
					return; /* Do nothing */
				}
				else    /* Still exist higher gain */
				{
					g_EM_ADC_GainNeutralCurrentLevel++; /* Increase level count */
					EM_ADC_GainNeutralSet();            /* Apply gain */
				}
				break;
			default:
				break;
			}

		}

		/******************************************************************************
		 * Function Name    : void EM_ADC_GainDecrease(EM_LINE line)
		 * Description      : Decrease phase gain 1 level & set the gain level to port
		 * Arguments        : None
		 * Functions Called : EM_ADC_GainPhaseSet()
		 * Return Value     : None
		 ******************************************************************************/
		void EM_ADC_GainDecrease(EM_LINE line)
		{
			switch (line)
			{
			case EM_LINE_PHASE:
				if (g_EM_ADC_GainPhaseCurrentLevel == 0)    /* Is lowest gain? */
				{
					return; /* Do nothing */
				}
				else    /* Still exist lower gain */
				{
					g_EM_ADC_GainPhaseCurrentLevel--;   /* Decrease level count */
					EM_ADC_GainPhaseSet();              /* Apply gain */
				}
				break;
			case EM_LINE_NEUTRAL:
				if (g_EM_ADC_GainNeutralCurrentLevel == 0)  /* Is lowest gain? */
				{
					return; /* Do nothing */
				}
				else    /* Still exist lower gain */
				{
					g_EM_ADC_GainNeutralCurrentLevel--; /* Decrease level count */
					EM_ADC_GainNeutralSet();            /* Apply gain */
				}
				break;
			default:
				break;
			}
		}

		/******************************************************************************
		 * Function Name    : uint8_t EM_ADC_GainGetLevel(EM_LINE line)
		 * Description      : Get the current phase gain level
		 * Arguments        : None
		 * Functions Called : None
		 * Return Value     : None
		 ******************************************************************************/
		uint8_t EM_ADC_GainGetLevel(EM_LINE line)
		{
			switch (line)
			{
			case EM_LINE_PHASE:
				return g_EM_ADC_GainPhaseCurrentLevel;
			case EM_LINE_NEUTRAL:
				return g_EM_ADC_GainNeutralCurrentLevel;
			default:
				return 0;
			}
		}

		void EM_ADC_SetGainValue(EM_LINE line, uint8_t gain)
		{
			dsad_gain_t dsad_gain = R_DSADC_GetGainEnumValue(gain);

			switch (line)
			{
			case EM_LINE_PHASE:
				R_DSADC_SetChannelGain(EM_ADC_DRIVER_CHANNEL_PHASE, dsad_gain);
				break;
			case EM_LINE_NEUTRAL:
				R_DSADC_SetChannelGain(EM_ADC_DRIVER_CHANNEL_NEUTRAL, dsad_gain);
				break;
			default:
				break;
			}
		}

		/******************************************************************************
		 * Function Name    : void EM_ADC_SetPhaseCorrection(float32_t degree)
		 * Description      : Set phase correction between I1 and V
		 * Arguments        : degree: Phase (in degree) of V based on I1
		 * Functions Called : TBD
		 * Return Value     : None
		 ******************************************************************************/
		void EM_ADC_SetPhaseCorrection(EM_LINE line, float32_t degree)
		{
			degree = (degree < 0.0f) ? -degree : degree;

			switch (line)
			{
			case EM_LINE_PHASE:
				EM_ADC_DriverSetPhaseStep(
						EM_ADC_DRIVER_CHANNEL_PHASE,
						(uint16_t)(degree / EM_ADC_DRIVER_PHASE_SHIFT_STEP)
				);
				break;
			case EM_LINE_NEUTRAL:
				EM_ADC_DriverSetPhaseStep(
						EM_ADC_DRIVER_CHANNEL_NEUTRAL,
						(uint16_t)(degree / EM_ADC_DRIVER_PHASE_SHIFT_STEP)
				);
				break;
			default:
				break;

			}
		}

