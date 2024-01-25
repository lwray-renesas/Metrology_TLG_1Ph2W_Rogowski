/***********************************************************************************************************************
 * DISCLAIMER
 * This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products.
 * No other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
 * applicable laws, including copyright laws.
 * THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING THIS SOFTWARE, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NON-INFRINGEMENT.  ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED.TO THE MAXIMUM EXTENT PERMITTED NOT PROHIBITED BY
 * LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES SHALL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS SOFTWARE, EVEN IF RENESAS OR
 * ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability
 * of this software. By using this software, you agree to the additional terms and conditions found by accessing the
 * following link:
 * http://www.renesas.com/disclaimer
 *
 * Copyright (C) 2015, 2021 Renesas Electronics Corporation. All rights reserved.
 ***********************************************************************************************************************/

/***********************************************************************************************************************
 * File Name    : r_cg_rtc.c
 * Version      : Applilet4 for RL78/I1C V1.01.07.02 [08 Nov 2021]
 * Device(s)    : R5F10NLG
 * Tool-Chain   : CCRL
 * Description  : This file implements device driver for RTC module.
 * Creation Date: 18/10/2023
 ***********************************************************************************************************************/

/***********************************************************************************************************************
Includes
 ***********************************************************************************************************************/
#include "r_cg_macrodriver.h"
#include "r_cg_rtc.h"
/* Start user code for include. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */
#include "r_cg_userdefine.h"

/***********************************************************************************************************************
Pragma directive
 ***********************************************************************************************************************/
/* Start user code for pragma. Do not edit comment generated here */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
Global variables and functions
 ***********************************************************************************************************************/
extern volatile rtc_capture_value_t g_rtc_calendar_rtcic0;
extern volatile rtc_capture_value_t g_rtc_calendar_rtcic1;
extern volatile rtc_capture_value_t g_rtc_calendar_rtcic2;
extern volatile uint32_t g_rtc_binary_rtcic0;
extern volatile uint32_t g_rtc_binary_rtcic1;
extern volatile uint32_t g_rtc_binary_rtcic2;
extern volatile uint8_t g_lvd_vrtc_ready_flag;    /* VRTC pin voltage ready flag */
/* Start user code for global. Do not edit comment generated here */
/* CAUTIONS:
 * Those bits can be write directly without checking for synchronization:
 *  . RCR1:
 *      RTCOS[3]
 *  . RCR2:
 *      ADJ30[2], RTCOE[3]
 *  . RCR5: All
 *  . RADJ:
 * Value written to belows will reflected after 4 read operations:
 *  . All calendar counters
 *  . All alarm compare and enable registers
 *  . RCR2: AADJE[4], AADJP[5], HR24[6]
 *  . RCR3, RCR4
 */
#define ABS(x)                  ((x) <  0  ? (-(x)) : (x))                          /* Get absolute value */
/* End user code. Do not edit comment generated here */

/***********************************************************************************************************************
 * Function Name: R_RTC_Create
 * Description  : This function initializes the real-time clock module.
 * Arguments    : None
 * Return Value : None
 ***********************************************************************************************************************/
void R_RTC_Create(void)
{
	volatile uint8_t tmp;
	volatile uint16_t w_count;

	VRTCEN = 1U;    /* enables input clock supply */

	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count <= RTC_STARTWAITTIME; w_count++)
	{
		NOP();
	}

	if ((_00_RTC_POWERON_RESET_OCCUR == RTCPORSR) && (0 == LVDVRTCF))
	{
		RTCPORSR = _01_RTC_POWERON_RESET_CHECK;
		g_lvd_vrtc_ready_flag = 0U;
		tmp = RCR2;
		tmp &= (uint8_t)~_01_RTC_COUNTER_NORMAL;
		RCR2 = tmp;
		RTCRMK = 1U;    /* disable INTRTCPRD interrupt */
		RTCRIF = 0U;    /* clear INTRTCPRD interrupt flag */
		RTCAMK = 1U;    /* disable INTRTCALM interrupt */
		RTCAIF = 0U;    /* clear INTRTCALM interrupt flag */
		/* Set INTRTCPRD low priority */
		RTCRPR1 = 1U;
		RTCRPR0 = 1U;
		tmp = RCR1;
		tmp &= (uint8_t)~_01_RTC_ALARM_ENABLE;
		RCR1 = tmp;
		tmp = RCR1;
		tmp &= (uint8_t)~_04_RTC_PERIODIC_ENABLE;
		RCR1 = tmp;
		RCR5GD = 0x00U;
		RCR5GD = 0x72U;
		RCR5GD = 0x64U;
		RCR5 = 0x00U;
		RCR5GD = 0x00U;
		RCR4 = _00_RTC_SELECT_FSUB;
		tmp = RCR3;
		tmp |= _01_RTC_INPUT_ENABLE;
		RCR3 = tmp;

		/* Change the waiting time according to the system */
		for (w_count = 0U; w_count <= RTC_6CLOCKWAITTIME; w_count++)
		{
			NOP();
		}

		tmp = RCR2;
		tmp &= (uint8_t)~_80_RTC_BINARY_MODE;
		RCR2 = tmp;

		/* Change the waiting time according to the system */
		for (w_count = 0U; w_count <= RTC_STARTWAITTIME; w_count++)
		{
			NOP();
		}

		tmp = RCR2;
		tmp |= _02_RTC_RESET_WRITER_INITIALIZED;
		RCR2 = tmp;

		/* Change the waiting time according to the system */
		for (w_count = 0U; w_count <= RTC_RESETWAITTIME; w_count++)
		{
			NOP();
		}

		tmp = RCR1;
		tmp |= _D0_RTC_INTRTCPRD_CLOCK_7 | _04_RTC_PERIODIC_ENABLE | _00_RTC_ALARM_DISABLE;
		RCR1 = tmp;
		tmp = RCR2;
		tmp |= _00_RTC_CALENDER_MODE | _40_RTC_24HOUR_MODE | _00_RTC_RTCOUT_DISABLE;
		RCR2 = tmp;
	}
}

/***********************************************************************************************************************
 * Function Name: R_RTC_Start
 * Description  : This function enables the real-time clock.
 * Arguments    : None
 * Return Value : None
 ***********************************************************************************************************************/
void R_RTC_Start(void)
{
	volatile uint8_t tmp;

	RTCRIF = 0U;    /* clear INTRTCPRD interrupt flag */
	RTCRMK = 0U;    /* enable INTRTCPRD interrupt */
	tmp = RCR2;
	tmp |= _01_RTC_COUNTER_NORMAL;
	RCR2 = tmp;
}

/***********************************************************************************************************************
 * Function Name: R_RTC_Stop
 * Description  : This function disables the real-time clock.
 * Arguments    : None
 * Return Value : None
 ***********************************************************************************************************************/
void R_RTC_Stop(void)
{
	volatile uint8_t tmp;
	volatile uint16_t w_count;

	tmp = RCR2;
	tmp &= (uint8_t)~_01_RTC_COUNTER_NORMAL;
	RCR2 = tmp;

	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count <= RTC_STARTWAITTIME; w_count++)
	{
		NOP();
	}

	tmp = RCR2;
	tmp |= _02_RTC_RESET_WRITER_INITIALIZED;
	RCR2 = tmp;

	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count <= RTC_RESETWAITTIME; w_count++)
	{
		NOP();
	}

	RTCRMK = 1U;    /* disable INTRTCPRD interrupt */
	RTCRIF = 0U;    /* clear INTRTCPRD interrupt flag */
}













/***********************************************************************************************************************
 * Function Name: R_RTC_Set_ConstPeriodInterruptOn
 * Description  : This function enables constant-period interrupt.
 * Arguments    : period -
 *                    the constant period of INTRTCPRD
 * Return Value : status -
 *                    MD_OK or MD_ARGERROR
 ***********************************************************************************************************************/
MD_STATUS R_RTC_Set_ConstPeriodInterruptOn(rtc_int_period_t period)
{
	MD_STATUS status = MD_OK;

	if ((period < SEC1_256) || (period > SEC2S))
	{
		status = MD_ARGERROR;
	}
	else
	{
		RTCRMK = 1U;    /* disable INTRTCPRD interrupt */
		RCR1 = (RCR1 & _09_RTC_CLEAR_PERIOD) | period | _04_RTC_PERIODIC_ENABLE;
		RTCRIF = 0U;    /* clear INTRTCPRD interrupt flag */
		RTCRMK = 0U;    /* enable INTRTCPRD interrupt */
	}

	return (status);
}

/***********************************************************************************************************************
 * Function Name: R_RTC_Set_ConstPeriodInterruptOff
 * Description  : This function disables constant-period interrupt.
 * Arguments    : None
 * Return Value : None
 ***********************************************************************************************************************/
void R_RTC_Set_ConstPeriodInterruptOff(void)
{
	RCR1 = (RCR1 & (uint8_t)~_04_RTC_PERIODIC_ENABLE);
	RTCRIF = 0U;    /* clear INTRTCPRD interrupt flag */
}




/* Start user code for adding. Do not edit comment generated here */
/******************************************************************************
 * Function Name: uint16_t R_RTC_Get_RADJ_Waittime(void)
 * Note         : THIS FUNCTION STRICTLY DEPEND ON THE MCU
 *              : BELOW IMPLEMENTATION IS USED FOR RL78I1B ONLY
 *              : WHEN CHANGE TO OTHER MCU, PLEASE RE-IMPLEMENT THIS FUNCTION
 * Description  :
 * Arguments    :
 * Return Value : None
 ******************************************************************************/
uint16_t R_RTC_Get_RADJ_Waittime(void)
{
	uint16_t waittime;

	switch (HOCODIV)
	{
	case 0: /*IS_MCU_RUN_AT_24MHZ*/
		waittime = RTC_RADJWAITTIME_24;
		break;
	case 1:/* IS_MCU_RUN_AT_12MHZ */
		waittime = RTC_RADJWAITTIME_12;
		break;
	case 2:/* IS_MCU_RUN_AT_06MHZ */
		waittime = RTC_RADJWAITTIME_06;
		break;
	case 3:/* IS_MCU_RUN_AT_03MHZ */
		waittime = RTC_RADJWAITTIME_03;
		break;
	default:
		waittime = RTC_RADJWAITTIME_24;
		break;
	}
	return waittime;
}
/******************************************************************************
 * Function Name: R_RTC_Compensate
 * Description  : This function compensate the error on sub-clock by a provided ppm
 * Arguments    : float ppm: ppm error of sub-clock crystal
 * Return Value : None
 *******************************************************************************/
void R_RTC_Compensate(float32_t ppm)
{
	volatile uint8_t tmp;
	volatile uint16_t w_count;
	volatile uint16_t waittime;
	float32_t abs_ppm = ABS(ppm);
	uint8_t radj_reg = 0;

	/* Supply clock to access RTC domain register */
	VRTCEN = 1U;

	/* This feature only run on 32.768KHz clock source : RCKSEL is bit 0 */
	if ((RCR4 & _00_RTC_SELECT_FSUB) == 0)
	{
		/* Clear adjustment first : PMADJ bit [7:6] */
		while ((RADJ & _C0_RTC_TIMER_ERROR_ADJUST_ALL) != 0)
		{
			RADJ &= (uint8_t)~_C0_RTC_TIMER_ERROR_ADJUST_ALL;
		}

		/* Enable automatic adj : AADJE bit 4 */
		tmp = RCR2;
		tmp |= _10_RTC_ADJUSTMENT_ENABLE;
		RCR2 = tmp;
		/* Change the waiting time according to the system */
		for (w_count = 0U; w_count <= RTC_WAITTIME; w_count++)
		{
			NOP();
		}

		/* Enable sub or add */
		if (ppm > 0.0f)
		{
			radj_reg |= _80_RTC_TIMER_ERROR_ADJUST_MINUS;       /* sub */
		}
		else
		{
			radj_reg |= _40_RTC_TIMER_ERROR_ADJUST_PLUS;        /* add */
		}

		tmp = RCR2;
		if (abs_ppm <= 32.043f)         /* 1 minutes adjustment */
		{
			tmp &= (uint8_t)~_20_RTC_CALENDER_10SECONDS;
			radj_reg |= ( (uint8_t)(abs_ppm * (32768.0f * 60.0f / 1e6) + 0.5f) ) & 0x3F;
		}
		else if (abs_ppm <= 192.26f)    /* 10-second adjustment */
		{
			tmp |= _20_RTC_CALENDER_10SECONDS;
			radj_reg |= ( (uint8_t)(abs_ppm * (32768.0f * 10.0f / 1e6) + 0.5f) ) & 0x3F;
		}
		else
		{
			/* Out of control !! no adj at all */
			tmp &= (uint8_t)~(_20_RTC_CALENDER_10SECONDS | _10_RTC_ADJUSTMENT_ENABLE);
			radj_reg = 0;
		}

		RCR2 = tmp;
		/* Change the waiting time according to the system */
		for (w_count = 0U; w_count <= RTC_WAITTIME; w_count++)
		{
			NOP();
		}
		waittime = R_RTC_Get_RADJ_Waittime();
		RADJ = radj_reg;
		/* Change the waiting time according to the system */
		for (w_count = 0U; w_count <= waittime; w_count++)
		{
			NOP();
		}
	}
}

/******************************************************************************
 * Function Name: R_RTC_StopCompensation
 * Description  : Stop the rtc compensation
 * Arguments    : None
 * Return Value : None
 *******************************************************************************/
void R_RTC_StopCompensation(void)
{
	volatile uint8_t tmp;
	volatile uint16_t w_count;
	volatile uint16_t waittime;

	/* Supply clock to access RTC domain register */
	VRTCEN = 1U;

	/* AADJE bit 4, AADJP is bit 5 */
	tmp = RCR2;
	tmp &= (uint8_t)~(_20_RTC_CALENDER_10SECONDS | _10_RTC_ADJUSTMENT_ENABLE);
	RCR2 = tmp;
	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count <= RTC_WAITTIME; w_count++)
	{
		NOP();
	}
	waittime = R_RTC_Get_RADJ_Waittime();
	RADJ = 0;
	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count <= waittime; w_count++)
	{
		NOP();
	}
}


/***********************************************************************************************************************
 * Function Name: R_RTC_Set_CalendarCounterValue
 * Description  : This function changes the calendar real-time clock value.
 * Arguments    : counter_write_val -
 *                    the expected real-time clock value(BCD code)
 * Return Value : status -
 *                    MD_OK or MD_BUSY1
 ***********************************************************************************************************************/
MD_STATUS R_RTC_Set_CalendarCounterValue(rtc_counter_value_t counter_write_val)
{
	MD_STATUS status = MD_OK;
	volatile uint16_t  w_count;
	volatile uint8_t tmp;

	tmp = RCR2;
	tmp &= (uint8_t)~_01_RTC_COUNTER_NORMAL;
	RCR2 = tmp;

	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count < RTC_WAITTIME; w_count++)
	{
		NOP();
	}

	if (_01_RTC_COUNTER_NORMAL == (RCR2 & _01_RTC_COUNTER_NORMAL))
	{
		status = MD_BUSY1;
	}
	else
	{
		RSECCNT = counter_write_val.sec;
		RMINCNT = counter_write_val.min;
		RHRCNT = counter_write_val.hour;
		RWKCNT = counter_write_val.week;
		RDAYCNT = counter_write_val.day;
		RMONCNT = counter_write_val.month;
		RYRCNT = counter_write_val.year;
	}
	/* Read back to make sure value is set */
	for (w_count = 0; w_count < 4; w_count++)
	{
		tmp = RSECCNT;
		tmp = RMINCNT;
		tmp = RHRCNT;
		tmp = RWKCNT;
		tmp = RDAYCNT;
		tmp = RMONCNT;
		tmp = (uint8_t)RYRCNT;
	}

	/* Restart counting : START is bit 0 */
	tmp = RCR2;
	tmp |= _01_RTC_COUNTER_NORMAL;
	RCR2 = tmp;
	/* Change the waiting time according to the system */
	for (w_count = 0U; w_count <= RTC_STARTWAITTIME; w_count++)
	{
		NOP();
	}
	return (status);
}


/***********************************************************************************************************************
 * Function Name: R_RTC_Get_CalendarCounterValue
 * Description  : This function reads the results of real-time clock and store them in the variables.
 * Arguments    : counter_read_val -
 *                    the expected real-time clock value(BCD code)
 * Return Value : status -
 *                    MD_OK or MD_ERROR
 ***********************************************************************************************************************/
MD_STATUS R_RTC_Get_CalendarCounterValue(rtc_counter_value_t * const counter_read_val)
{
	MD_STATUS status = MD_OK;
	volatile uint8_t tmp;

	tmp = RSR;
	tmp &= (uint8_t)~_02_RTC_SECOND_CARRY;
	RSR = tmp;
	counter_read_val->sec = RSECCNT;
	counter_read_val->min = RMINCNT;

	if ((RCR2 & 0x40) == 0x40)
	{
		counter_read_val->hour = RHRCNT & 0xBF;
	}
	else
	{
		counter_read_val->hour = RHRCNT;
	}

	counter_read_val->week = RWKCNT;
	counter_read_val->day = RDAYCNT;
	counter_read_val->month = RMONCNT;
	counter_read_val->year = RYRCNT;
	if (_02_RTC_SECOND_CARRY == (RSR & _02_RTC_SECOND_CARRY))
	{
		status = MD_ERROR;
	}

	return (status);
}

/* End user code. Do not edit comment generated here */
