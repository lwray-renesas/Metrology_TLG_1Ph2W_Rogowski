/*
 * calibration.c
 *
 *  Created on: 23 Oct 2023
 *      Author: a5126135
 */

#include "calibration.h"
#include "wrp_em_sw_config.h"
#include "config_storage.h"
#include "string.h"
#include "debug.h"

static volatile uint8_t is_calibrating = 0U;

uint8_t CALIBRATION_Occuring(void)
{
	return is_calibrating;
}

uint8_t CALIBRATION_Invoke(uint16_t calib_c, uint16_t calib_cp, float32_t calib_imax, float32_t calib_v, float32_t calib_i, uint8_t calib_neutral)
{
	EM_CALIBRATION calib;
	EM_CALIB_ARGS calib_args;
	EM_CALIB_OUTPUT calib_output;
	EM_CALIB_WORK calib_work;
	uint8_t line;
	uint8_t line_max;
	uint8_t rlt, em_rlt;
	uint16_t c, cp;
	float32_t imax, v, i;
	dsad_channel_t  channel;                /* Choose calibrated channel */
	float32_t       degree_list[EM_GAIN_PHASE_NUM_LEVEL_MAX];
	float32_t       gain_list[EM_GAIN_PHASE_NUM_LEVEL_MAX];

	/* Signal to applicaiton we are calibrating*/
	is_calibrating = 1U;

	/* Include neutral calibration? */
	if (calib_neutral)
	{
		line_max = 2;
	}
	else
	{
		line_max = 1;
	}

	for (line = 0; line < line_max; line++)
	{
		/* Fixed parameter */
		calib_args.rtc_period = 500;
		calib_args.max_gvalue = 64;
		calib_args.stable_ndelay = 10;
		/* Settable parameter */
		calib_args.cycle = calib_c;
		calib_args.cycle_angle = calib_cp;
		calib_args.imax = calib_imax;
		calib_args.v = calib_v;
		calib_args.i = calib_i;
		calib_args.line_v = EM_LINE_PHASE;
		if (line == 0)
		{
			calib_args.line_i = EM_LINE_PHASE;
		}
		else
		{
			calib_args.line_i = EM_LINE_NEUTRAL;
		}

		memset(&degree_list[0], 0, sizeof(degree_list));
		memset(&gain_list[0], 0, sizeof(gain_list));

#if (EM_SW_PROPERTY_ADC_GAIN_PHASE_NUMBER_LEVEL != 1)
		if (line == 0)
		{
			calib_args.imax = 0.0f;
		}
#endif /* EM_SW_PROPERTY_ADC_GAIN_PHASE_NUMBER_LEVEL != 1*/

#if (EM_SW_PROPERTY_ADC_GAIN_NEUTRAL_NUMBER_LEVEL != 1)
		if (line == 1)
		{
			calib_args.imax = 0.0f;
		}
#endif /* EM_SW_PROPERTY_ADC_GAIN_NEUTRAL_NUMBER_LEVEL != 1 */

		DEBUG_Printf((uint8_t *)"Initiate Calibration ");

		if(0U == line)
		{
			DEBUG_Printf((uint8_t *)"Phase...");
		}
		else
		{
			DEBUG_Printf((uint8_t *)"Neutral...");
		}

		calib = EM_GetCalibInfo();

		rlt = EM_CalibInitiate(&calib_args, &calib_work, &calib_output);

		if (rlt != EM_OK)
		{
			if (rlt == EM_ERROR_NOT_RUNNING)
			{
				DEBUG_Printf((uint8_t *)"FAILED, EM not started\n\r");
			}
			else if (rlt == EM_ERROR_CALIBRATING_NULL)
			{
				DEBUG_Printf((uint8_t *)"FAILED, Null pointer\n\r");
			}
			else if (rlt == EM_ERROR_CALIBRATING_INVALID_LINE)
			{
				DEBUG_Printf((uint8_t *)"FAILED, Invalid EM_LINE selection\n\r");
			}
			else if (rlt == EM_ERROR_CALIBRATING_CYCLE)
			{
				DEBUG_Printf((uint8_t *)"FAILED, Invalid calib args, check manual for correct settings\n\r");
			}
			else if (rlt == EM_ERROR_CALIBRATING_V_I)
			{
				DEBUG_Printf((uint8_t *)"FAILED, Invalid calib args, check manual for correct settings\n\r");
			}
			else if (rlt == EM_ERROR_CALIBRATING_RTC_PERIOD)
			{
				DEBUG_Printf((uint8_t *)"FAILED, Invalid calib args, check manual for correct settings\n\r");
			}
			else if (rlt == EM_ERROR_CALIBRATING_IMAX_AND_NUM_OF_GAIN)
			{
				DEBUG_Printf((uint8_t *)"FAILED, Invalid calib args, check manual for correct settings\n\r");
			}
			else if (rlt == EM_ERROR_STARTUP_RTC)
			{
				DEBUG_Printf((uint8_t *)"FAILED, No RTC interrupt, check if function put into interrupt\n\r");
			}
			else
			{
				DEBUG_Printf((uint8_t *)"FAILED, Unknown Error\n\r");
			}
		}
		else
		{
			DEBUG_Printf((uint8_t *)"Success\n\r");

			DEBUG_Printf((uint8_t *)"Run Calibration\n\r");

			/* EM Calibrate initiated OK */
			while (TRUE)
			{
				R_WDT_Restart();

				rlt = EM_CalibRun();

				/* Calib output is updated by metrology calibration */
				if (rlt == EM_CALIBRATING || rlt == EM_OK)
				{
					/* Continue */
					if (calib_output.step == EM_CALIB_STEP_FS)
					{
						DEBUG_Printf((uint8_t *)"Finished calibrating sampling frequency\n\r");
					}
					else if (calib_output.step == EM_CALIB_STEP_IGAIN)
					{
						DEBUG_Printf((uint8_t *)"Finished calibrating current gain\n\r");
					}
					else if (calib_output.step == EM_CALIB_STEP_SIGNALS)
					{
						DEBUG_Printf((uint8_t *)"Finished calibrating coefficient\n\r");
					}
					else if (calib_output.step == EM_CALIB_STEP_ANGLE)
					{
						DEBUG_Printf((uint8_t *)"Finished calibrating phase shift\n\r");
					}
					else
					{
						/* Do Nothing*/
					}
				}
				else
				{
					/* Error here */
					if (rlt == EM_ERROR_CALIBRATING_NOT_STARTED)
					{
						DEBUG_Printf((uint8_t *)"FAILED: Call EM_CalibInitiate first\n\r");
					}
					else if (rlt == EM_ERROR_CALIBRATING_FAILED_FS_OUT_RANGE)
					{
						DEBUG_Printf((uint8_t *)"FAILED: Fs out of range, expected: EM_SAMPLING_FREQUENCY_CALIBRATION, calculated calib_output.fs\n\r");
					}
					else if (rlt == EM_ERROR_CALIBRATING_FAILED_IGAIN_OUT_RANGE)
					{
						/* If using dual gain, no check the gain */
						if (*((uint32_t *)&calib_args.imax) != 0)
						{
							DEBUG_Printf((uint8_t *)"FAILED: ADC gain too large calib_output.gain, check input signal\n\r");
						}
					}
					/* If angle_error is correct then angle_error1 is correct
					 * But if angle_error cannot, then calibration stop
					 * So there's only need to print out angle
					 */
					else if (rlt == EM_ERROR_CALIBRATING_FAILED_MAX_ANGLE)
					{
						DEBUG_Printf((uint8_t *)"FAILED: Out of adjustment range, angle: %.4f\n\r", calib_output.angle_error);
						if(1U == line)
						{
							DEBUG_Printf((uint8_t *)"FAILED: CT direction incorrect.\n\r", calib_output.angle_error);
							DEBUG_Printf((uint8_t *)"\x1b[38;2;255;165;0mTry reverse CT polarity or run command \"invertpolarity 1\"\x1b[0m\n\r", calib_output.angle_error);
						}
					}
					else if (rlt == EM_ERROR_CALIBRATING_FAILED_OUT_ANGLE)
					{
						DEBUG_Printf((uint8_t *)"FAILED: Angle out of calculation range\n\r");
					}
					else if (rlt == EM_ERROR_CALIBRATING_FAILED_REVERSE)
					{
						DEBUG_Printf((uint8_t *)"FAILED: V and I angle reversed\n\r");
					}
					else if (rlt == EM_ERROR_CALIBRATING_FAILED_V_LEAD_I)
					{
						DEBUG_Printf((uint8_t *)"FAILED: V lead I, V must lag I, angle: calib_output.angle_error (degree)\n\r");
					}
					else
					{
						/* Do Nothing*/
					}
				}

				if (rlt != EM_CALIBRATING)
				{
					break;
				}
			}
		}

		if (rlt == 0)
		{
			calib.sampling_frequency = calib_output.fs;
			calib.coeff.vrms = calib_output.vcoeff;
			degree_list[0] = calib_output.angle_error;
			gain_list[0] = calib_output.gain;
			if (*((uint32_t *)&calib_args.imax) == 0)
			{
				degree_list[1] = calib_output.angle_error1;
				gain_list[1] = calib_output.gain1;
			}
			if (calib_args.line_i == EM_LINE_PHASE)
			{
				calib.coeff.i1rms = calib_output.icoeff;
				calib.coeff.active_power = calib_output.pcoeff;
				calib.coeff.reactive_power = calib_output.pcoeff;
				calib.coeff.apparent_power = calib_output.pcoeff;
				calib.sw_phase_correction.i1_phase_degrees = degree_list;
				calib.sw_gain.i1_gain_values = gain_list;
				channel = EM_ADC_DRIVER_CHANNEL_PHASE;
			}
#ifdef METER_WRAPPER_ADC_COPY_NEUTRAL_SAMPLE
			else
			{
				calib.coeff.i2rms = calib_output.icoeff;
				calib.coeff.active_power2 = calib_output.pcoeff;
				calib.coeff.reactive_power2 = calib_output.pcoeff;
				calib.coeff.apparent_power2 = calib_output.pcoeff;
				calib.sw_phase_correction.i2_phase_degrees = degree_list;
				calib.sw_gain.i2_gain_values = gain_list;
				channel = EM_ADC_DRIVER_CHANNEL_NEUTRAL;
			}
#endif

			DEBUG_Printf((uint8_t *)"Stop EM and set calib info\n\r");
			EM_Stop();

			em_rlt = EM_SetCalibInfo(&calib);
			if (em_rlt != EM_OK)
			{
				/* Set calibration failed */
				DEBUG_Printf((uint8_t *)"Failed to set calib info\n\r");
				is_calibrating = 0U;
				EM_Start();
				return em_rlt;
			}

			/* Driver ADC Gain */
			if (*((uint32_t *)&calib_args.imax) != 0)
			{
				R_DSADC_SetChannelGain(
						channel,
						R_DSADC_GetGainEnumValue((uint8_t)calib_output.gain)
				);
			}

			DEBUG_Printf((uint8_t *)"Backing up calib data to dataflash\n\r");
			/* Backup calibration info to dataflash */
			if (CONFIG_Backup(CONFIG_ITEM_CALIB) != 0)
			{
				/* Restart the EM */
				DEBUG_Printf((uint8_t *)"Failed to backup\n\r");
				is_calibrating = 0U;
				EM_Start();
				return 3;
			}

		}
		else
		{
			/* Calib initiate or calib run failed, restart the EM */
			DEBUG_Printf((uint8_t *)"Calibration Failed - restarting EM\n\r");
			is_calibrating = 0U;
			EM_Start();
			return rlt;
		}

		DEBUG_Printf((uint8_t *)"Restarting EM...\n\r");
		is_calibrating = 0U;
		EM_Start();
	}

	DEBUG_Printf((uint8_t *)"Calibration Finished\n\r");
	is_calibrating = 0U;
	return 0;
}
